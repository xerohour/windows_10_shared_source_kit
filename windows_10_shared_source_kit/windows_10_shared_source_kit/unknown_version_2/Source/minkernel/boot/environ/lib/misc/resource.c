/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    resource.c

Abstract:

    Provides image resource support.

--*/

// ------------------------------------------------------------------- Includes

#include <bootlibp.h>

// ---------------------------------------------------------------- Definitions

#define DEFAULT_LOCALE L"en-US"
#define MUI_IMAGE_FLAGS \
    (IMAGE_FLAGS_MAP_ONE_TO_ONE | \
     IMAGE_FLAGS_NO_RELOCATIONS | \
     IMAGE_FLAGS_BOOT_MUI)

// --------------------------------------------------------------------- Locals

//
// Define global variables which specify the preferred locale data to use when
// presenting UI. Also define a pointer that will refer to the end of the PE
// image in question to be used for data validation.
//

static PVOID ResMuiImageBase = NULL;
static ULONG ResMuiImageSize = 0;
static PIMAGE_RESOURCE_DIRECTORY ResRootDirectoryPrimary = NULL;
static ULONG ResRootDirectoryPrimaryOffset = 0;
static PVOID ResPeImageBasePrimary = NULL;
static PVOID ResPeImageEndPrimary = NULL;
static PWSTR ResLocale = NULL;

//
// Define global variables which specify the fallback locale data to use if
// necessary. Also define a pointer that will refer to the end of the PE image
// in question to be used for data validation.
//

static PIMAGE_RESOURCE_DIRECTORY ResRootDirectoryFallback = NULL;
static ULONG ResRootDirectoryFallbackOffset = 0;
static PVOID ResPeImageBaseFallback = NULL;
static PVOID ResPeImageEndFallback = NULL;

//
// Define global variables which specify the current locale to use if necessary.
// This will be either the fallback locale or the primary locale. Also define
// a pointer that will refer to the end of the PE image in question to be used
// for data validation.
//

static PIMAGE_RESOURCE_DIRECTORY ResRootDirectory = NULL;
static ULONG ResRootDirectoryOffset = 0;
static PVOID ResPeImageBase = NULL;
static PVOID ResPeImageEnd = NULL;

//
// Define a global variable used to indicate an established initialization
// between the display library (font file) and the resource subsystem.
//

static BOOLEAN ResLoadedFontFiles = FALSE;

// ----------------------------------------------- Internal Function Prototypes

BOOLEAN
ResDoMuiResourceChecksumsMatch (
    __in PRC_CONFIG PrimaryMuiResource,
    __in PRC_CONFIG SecondaryMuiResource
    );

NTSTATUS
ResFindDataEntry (
    __in ULONG Section,
    __in_opt PULONG Id,
    __in_opt PWSTR Name,
    __deref_out PIMAGE_RESOURCE_DATA_ENTRY *DataEntryOut,
    __deref_out_bcount((*DataEntryOut)->Size) PVOID *ResourceOut
    );

NTSTATUS
ResFindDataEntryFromImage (
    __in_opt PVOID ImageBase,
    __in_opt ULONG ImageSize,
    __in ULONG Section,
    __in_opt PULONG Id,
    __in_opt PWSTR Name,
    __deref_out PIMAGE_RESOURCE_DATA_ENTRY *DataEntryOut,
    __deref_out PVOID *ResourceOut
    );

PVOID
ResFindDirectoryEntry (
    __in PIMAGE_RESOURCE_DIRECTORY Directory,
    __in_opt PULONG Id,
    __in_opt PWSTR Name,
    __in PUCHAR SectionStart
    );

PRC_CONFIG
ResFindMuiSection (
    __in PIMAGE_RESOURCE_DIRECTORY Directory,
    __in ULONG DataOffset
    );

NTSTATUS
ResInitializeMuiResources (
    VOID
    );

NTSTATUS
ResSelectLocale (
    __in BOOLEAN UsePrimaryLocale
    );

// ------------------------------------------------------------------ Functions

NTSTATUS
BlpResourceCodeIntegrityFlags (
    __in PBOOT_APPLICATION_ENTRY ApplicationEntry,
    __out PULONG ImageFlags
    )

/*++

Routine Description:

    This function determines the code integrity validation flags that should
    be used when loading the resources for the boot application described by the
    given application entry.

Arguments:

    ApplicationEntry - Supplies the application entry describing the boot
        application.

    ImageFlags - Supplies a pointer to a variable that receives a bitmask
        containing the code integrity validation flags that should be applied
        when loading resources for this application.

Return Value:

    STATUS_SUCCESS is returned if the resource load should proceed using
        the returned load flags.

    An error code is returned if a fundamental code integrity problem occurs
        and application resource load should be blocked immediately.

--*/

{

    BOOLEAN IntegrityChecksDisabled;

    //
    // Query the code integrity configuration elements from the option list
    // associated with the given application entry.  Note that the query
    // routine internally detects entries describing an OS loader WinPE boot
    // attempt and forces integrity checks off in these cases.
    //

    BlImgQueryCodeIntegrityBootOptions(ApplicationEntry,
                                       &IntegrityChecksDisabled,
                                       NULL,
                                       NULL);

    //
    // If integrity checks are explicitly disabled, then it is likely that
    // this boot entry describes a test application of some sort.  Validating
    // the hash is an option since it would allow integrity check failures to
    // be logged or possibly reported to a boot debugger.  Reporting failures
    // to the boot debugger is not desirable for test applications and no
    // persistent logging of boot application integrity check failures is
    // done at this time.  Therefore, no hash validation is performed when
    // loading boot applications that disable integrity checks in their option
    // list.
    //

    if (IntegrityChecksDisabled != FALSE) {
        *ImageFlags = 0;
        return STATUS_SUCCESS;
    }

    //
    // Integrity checks are not explicitly disabled in the application boot
    // options, so set image flags that will block image load unless the image
    // is correctly signed and carries a valid image hash.
    //

    *ImageFlags = IMAGE_FLAGS_VALIDATE_IMAGE_HASH |
                  IMAGE_FLAGS_VALID_HASH_REQUIRED;

    return STATUS_SUCCESS;
}

NTSTATUS
BlpResourceDestroy (
    VOID
    )

/*++

Routine Description:

    Cleans up any data associated with localizable resources in memory.

Arguments:

    None

Return Value:

    STATUS_SUCCESS, or an appropriate error status.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceRes);
    Status = STATUS_SUCCESS;
    if (ResMuiImageBase != NULL) {
        Status = BlImgUnLoadImage(ResMuiImageBase,
                                  ResMuiImageSize,
                                  MUI_IMAGE_FLAGS);
    }

    if (ResLocale != NULL) {
        BlMmFreeHeap(ResLocale);
        ResLocale = NULL;
    }

    BlpInterfaceExit(InterfaceRes);
    return Status;
}


NTSTATUS
BlpResourceInitialize (
    VOID
    )

/*++

Routine Description:

    Initializes the resource engine to allow reading of localizable resources
    from in memory.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    ULONG HorizontalPixels;
    PVOID ImageBase;
    ULONG ImageSize;
    PIMAGE_SECTION_HEADER SectionHeader;
    NTSTATUS Status;
    BOOLEAN UsePrimaryLocale;
    ULONG VerticalPixels;

    BlpInterfaceEnter(InterfaceRes);

    //
    // Initialize module globals.
    // N.B. If the library is re-initialized, this routine is called a second
    //      time.  To ensure there are no stale values, always initialized
    //      globals during initialization.
    //

    ResMuiImageBase = NULL;
    ResMuiImageSize = 0;
    ResRootDirectoryPrimary = NULL;
    ResRootDirectoryPrimaryOffset = 0;
    ResPeImageBasePrimary = NULL;
    ResPeImageEndPrimary = NULL;
    ResRootDirectoryFallback = NULL;
    ResRootDirectoryFallbackOffset = 0;
    ResPeImageBaseFallback = NULL;
    ResPeImageEndFallback = NULL;
    ResRootDirectory = NULL;
    ResRootDirectoryOffset = 0;
    ResPeImageBase = NULL;
    ResPeImageEnd = NULL;
    ResLoadedFontFiles = FALSE;
    if (ResLocale != NULL) {
        BlMmFreeHeap(ResLocale);
        ResLocale = NULL;
    }

    //
    // Find the resource section for the currently executing boot application
    // and load a pointer to the root of the resource directory as well as its
    // relative position in the image.
    //
    // N.B. A Boot Environment Application does not require a resource section.
    //

    Status = BlGetApplicationBaseAndSize(&ImageBase, &ImageSize);
    if (NT_SUCCESS(Status)) {

        //
        // If a resource section is present, internally save its pointer and
        // offset in the image.
        //

        SectionHeader = BlImgFindSection(ImageBase, ImageSize, ".rsrc");
        if (SectionHeader != NULL) {
            ResPeImageBaseFallback = ImageBase;
            ResPeImageEndFallback = Add2Ptr(ImageBase, ImageSize);
            ResRootDirectoryFallbackOffset = SectionHeader->VirtualAddress;
            ResRootDirectoryFallback = Add2Ptr(ImageBase,
                                               SectionHeader->VirtualAddress);
        }

        //
        // If the local console supports graphical display, attempt to load
        // MUI resources for the preferred language locale.  When successful,
        // use the MUI resources for all resource string lookups, and therefore
        // use the preferred locale.
        //

        UsePrimaryLocale = FALSE;
        Status = BlDisplayGetScreenResolution(&HorizontalPixels,
                                              &VerticalPixels);

        if (NT_SUCCESS(Status) &&
            ((HorizontalPixels != TEXT_MODE_HORIZONTAL_RESOLUTION) ||
             (VerticalPixels != TEXT_MODE_VERTICAL_RESOLUTION))) {

            Status = ResInitializeMuiResources();
            if (NT_SUCCESS(Status)) {
                UsePrimaryLocale = TRUE;
            }
        }

        Status = ResSelectLocale(UsePrimaryLocale);
    }

    BlpInterfaceExit(InterfaceRes);
    return Status;
}

NTSTATUS
BlResourceForceLoadFonts (
    VOID
    )

/*++

Routine Description:

    This routine forces any deferred font files to be loaded.

Arguements:

    None.

Return Value:

    NT Status code indicating outcome.

--*/

{

    NTSTATUS Status;

    //
    // If this is the first message lookup, force any deferred boot font files
    // to be loaded.  If this operation results in a failure, fallback to using
    // the embedded resources and possibly even textmode display.
    //

    Status = STATUS_SUCCESS;
    if (ResLoadedFontFiles == FALSE) {
        Status = BlpDisplayLoadDeferredFontFiles();
        if (!NT_SUCCESS(Status)) {
            Status = ResSelectLocale(FALSE);
            if (!NT_SUCCESS(Status)) {
                goto BlResourceForceLoadFontsEnd;
            }

            Status = BlpDisplayLoadDeferredFontFiles();
            if (!NT_SUCCESS(Status)) {
                goto BlResourceForceLoadFontsEnd;
            }
        }

        ResLoadedFontFiles = TRUE;
    }

BlResourceForceLoadFontsEnd:
    return Status;
}

PWCHAR
BlResourceFindHtml (
    __in_z PWCHAR Name
    )

/*++

Routine Description:

    This routine searches the resource section for an RT_HTML resource
    that matches the supplied Id.

Arguements:

    Name - Supplies a pointer to the wide-character string name of the
        resource.

Return Value:

    Returns a pointer to the RT_HTML resource, or NULL if it is not found.

--*/

{

    PIMAGE_RESOURCE_DATA_ENTRY HtmlDataEntry;
    PWCHAR HtmlRes;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceRes);
    HtmlRes = NULL;

    Status = ResFindDataEntry(23, // RT_HTML,
                              NULL,
                              Name,
                              &HtmlDataEntry,
                              &HtmlRes);

    if (!NT_SUCCESS(Status)) {
        goto FindHtmlEnd;
    }

    //
    // Look for the signature 0xFEFF (UTF-16, big-endian) that tells us
    // the format of our resource.
    //

    if (HtmlRes[0] == 0xFEFF) {

        //
        // Slide the resource left one char this first time, and
        // null-terminate it.  Then, on subsequent calls, just make
        // sure the resource is is null-terminated.
        //
        // N.B. Use RtlMoveMemory here since the source and destination
        //      ovelap.
        //

        MARK_SAFE_SUBTRACTION(HtmlDataEntry->Size, sizeof(WCHAR));
        RtlMoveMemory(&HtmlRes[0],
                      &HtmlRes[1],
                      HtmlDataEntry->Size - sizeof(WCHAR));

        HtmlRes[(HtmlDataEntry->Size / sizeof(WCHAR)) - 1] = L'\0';

    } else if (HtmlRes[(HtmlDataEntry->Size / sizeof(WCHAR)) - 1] != L'\0') {
        HtmlRes = NULL;
    }

FindHtmlEnd:
    BlpInterfaceExit(InterfaceRes);
    return HtmlRes;
}

PWCHAR
BlResourceFindMessage (
    __in ULONG Id
    )

/*++

Routine Description:

    The resource section of a boot application contains two directories in
    the root directory; a message table and resource data.  This routine
    will find the specified message in the message table.

    A message table can support multiple locales.  This routine only returns
    the message ID for the first local in the table.  When boot applications
    can support more than one locale, this routine should modified.

Arguments:

    Id - Supplies the message ID to look up.

Return Value:

    Pointer to the requested message.

    NULL - failure.

--*/

{

    PIMAGE_RESOURCE_DATA_ENTRY DataEntry;
    ULONG Index;
    PMESSAGE_RESOURCE_BLOCK MessageBlock;
    PMESSAGE_RESOURCE_DATA MessageData;
    PMESSAGE_RESOURCE_ENTRY MessageEntry;
    PVOID MessageEntryEnd;
    PWCHAR MessageString;
    PIMAGE_RESOURCE_DIRECTORY NextDirectory;
    ULONG NumberOfBlocks;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceRes);
    MessageString = NULL;

    //
    // Error out early if the Boot Application does not have a resource
    // section.
    //

    if (ResRootDirectory == NULL) {
        goto BlResourceFindMessageEnd;
    }

    //
    // If this is the first message lookup, force any deferred boot font files
    // to be loaded.  If this operation results in a failure, fallback to using
    // the embedded resources and possibly even textmode display.
    //

    if (ResLoadedFontFiles == FALSE) {
        Status = BlpDisplayLoadDeferredFontFiles();
        if (!NT_SUCCESS(Status)) {
            Status = ResSelectLocale(FALSE);
            if (!NT_SUCCESS(Status)) {
                goto BlResourceFindMessageEnd;
            }

            Status = BlpDisplayLoadDeferredFontFiles();
            if (!NT_SUCCESS(Status)) {
                Status = BlDisplaySetScreenResolution(
                             TEXT_MODE_HORIZONTAL_RESOLUTION,
                             TEXT_MODE_VERTICAL_RESOLUTION);

                if (!NT_SUCCESS(Status)) {
                    goto BlResourceFindMessageEnd;
                }
            }
        }

        ResLoadedFontFiles = TRUE;
    }

    //
    // When a message table is present in the resource section, it will be
    // found under the following directory path, with the specified directory
    // IDs.
    //
    // \Root\RT_MESSAGETABLE\RT_CURSOR\LangId.
    //

    Index = 11; // RT_MESSAGETABLE.
    NextDirectory = ResFindDirectoryEntry(ResRootDirectory,
                                          &Index,
                                          NULL,
                                          (PUCHAR)ResRootDirectory);

    if (NextDirectory == NULL) {
        goto BlResourceFindMessageEnd;
    }

    Index = 1;  // RT_CURSOR.
    NextDirectory = ResFindDirectoryEntry(NextDirectory,
                                          &Index,
                                          NULL,
                                          (PUCHAR)ResRootDirectory);

    if (NextDirectory == NULL) {
        goto BlResourceFindMessageEnd;
    }

    //
    // Obtain the message table for the first local ID.  Right now, a boot
    // application only supports one locale.  But if this functin call should
    // find the appropriate table based on locale id.
    //

    Index = (ULONG)-1;
    DataEntry = ResFindDirectoryEntry(NextDirectory,
                                      &Index,
                                      NULL,
                                      (PUCHAR)ResRootDirectory);

    if (!DataEntry) {
        goto BlResourceFindMessageEnd;
    }

    //
    // Obtain a pointer to the start of the resource messages.  Message
    // strings are maintained in a list of resource blocks where each
    // block contains a list of messages.  Walk each block in search of a
    // the block containing the requested block.
    //

    MessageData = Add2Ptr(ResRootDirectory,
                          DataEntry->OffsetToData - ResRootDirectoryOffset);

    NumberOfBlocks = MessageData->NumberOfBlocks;
    MessageBlock = MessageData->Blocks;
    while (NumberOfBlocks--) {
        if ((Id >= MessageBlock->LowId) &&
            (Id <= MessageBlock->HighId)) {

            //
            // The requested ID is within this block, scan forward until
            // the requested string is found.
            //

            MessageEntry = Add2Ptr(MessageData, MessageBlock->OffsetToEntries);
            Index = Id - MessageBlock->LowId;
            while (Index--) {
                MessageEntry = Add2Ptr(MessageEntry, MessageEntry->Length);
            }

            //
            // If the requested message entry is not within the extent of the
            // actual PE image, then the file is poorly formed.  Ensure that the
            // message entry length will not cause overflow during boundary
            // checking.
            //

            MessageEntryEnd = Add2Ptr(MessageEntry, MessageEntry->Length);
            if ((MessageEntryEnd < (PVOID)MessageEntry) ||
                ((PVOID)MessageEntry < ResPeImageBase) ||
                (MessageEntryEnd > ResPeImageEnd)) {

                goto BlResourceFindMessageEnd;
            }

            MessageString = (PWCHAR)MessageEntry->Text;
            goto BlResourceFindMessageEnd;
        }

        //
        // Check the next block for this ID.
        //

        MessageBlock += 1;
    }

BlResourceFindMessageEnd:
    BlpInterfaceExit(InterfaceRes);
    return MessageString;
}

PVOID
BlResourceFindData (
    __in ULONG Id,
    __out PULONG Size
    )

/*++

Routine Description:

    Looks up a resource data resource in the given image.

Arguments:

    Id - Supplies the data ID to look up.

    Size - Supplies a pointer to a variable that receives the size in bytes of
        the data resource.

Return Value:

    PVOID - pointer to the image.

    NULL - failure.

--*/

{

    return BlResourceFindDataFromImage(NULL, 0, Id, Size);
}

PVOID
BlResourceFindDataFromImage (
    __in_opt PVOID ImageBase,
    __in_opt ULONG ImageSize,
    __in ULONG Id,
    __out PULONG Size
    )

/*++

Routine Description:

    Looks up a resource data resource in the passed in image.

Arguments:

    ImageBase - Supplies the base of the loaded image in memory. (optional)

    ImageSize - Supplies the size of the loaded image. (optional)

    Id - Supplies the data ID to look up.

    Size - Supplies a pointer to a variable that receives the size in bytes of
        the data resource.

Return Value:

    PVOID - pointer to the image.

    NULL - failure.

--*/

{

    PIMAGE_RESOURCE_DATA_ENTRY DataEntry;
    PVOID DataPointer;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceRes);
    DataPointer = NULL;

    Status = ResFindDataEntryFromImage(ImageBase,
                                       ImageSize,
                                       10,
                                       &Id,
                                       NULL,
                                       &DataEntry,
                                       &DataPointer);

    if (!NT_SUCCESS(Status)) {
        goto FindDataEnd;
    }

    *Size = DataEntry->Size;

FindDataEnd:
    BlpInterfaceExit(InterfaceRes);
    return DataPointer;
}

PVOID
BlResourceFindDataFromImageByName (
    __in_opt PVOID ImageBase,
    __in_opt ULONG ImageSize,
    __in PWSTR Name,
    __out PULONG Size
    )

/*++

Routine Description:

    Looks up a data resource by name in the passed in image.

Arguments:

    ImageBase - Supplies the base of the loaded image in memory. (optional)

    ImageSize - Supplies the size of the loaded image. (optional)

    Name - Supplies the resource name to look up.

    Size - Supplies a pointer to a variable that receives the size in bytes of
        the data resource.

Return Value:

    PVOID - pointer to the image.

    NULL - failure.

--*/

{

    PIMAGE_RESOURCE_DATA_ENTRY DataEntry;
    PVOID DataPointer = NULL;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceRes);

    Status = ResFindDataEntryFromImage(ImageBase,
                                       ImageSize,
                                       10,
                                       NULL,
                                       Name,
                                       &DataEntry,
                                       &DataPointer);

    if (!NT_SUCCESS(Status)) {
        goto FindDataEnd;
    }

    *Size = DataEntry->Size;

FindDataEnd:
    BlpInterfaceExit(InterfaceRes);
    return DataPointer;
}

PVOID
BlResourceFindBitmap (
    __in ULONG Id,
    __out PULONG Height,
    __out PULONG Width
    )

/*++

Routine Description:

    Looks up a bitmap resource in the given image.

Arguments:

    Id - Supplies the bitmap ID to look up.

    Height - Supplies a pointer to the location that will be filled with the
        height of the bitnmap.

    Width - Supplies a pointer to the location that will be filled with the
        width of the bitnmap.

Return Value:

    PVOID - pointer to the image.

    NULL - failure.

--*/

{

    PBITMAP_IMAGE Bitmap;
    PIMAGE_RESOURCE_DATA_ENTRY BitmapDataEntry;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceRes);
    Bitmap = NULL;

    Status = ResFindDataEntry(2, // RT_BITMAP
                              &Id,
                              NULL,
                              &BitmapDataEntry,
                              &Bitmap);

    if (!NT_SUCCESS(Status)) {
        goto FindBitmapEnd;
    }

    *Height = Bitmap->Height;
    *Width = Bitmap->Width;

FindBitmapEnd:
    BlpInterfaceExit(InterfaceRes);
    return Bitmap;
}

PVOID
BlResourceFindVersionFromImage (
    __in PVOID ImageBase,
    __in ULONG ImageSize,
    __out PULONG Size
    )

/*++

Routine Description:

    This routine looks up a version resource in the passed in image.

Arguments:

    ImageBase - Supplies the base of the loaded image in memory.

    ImageSize - Supplies the size of the loaded image.

    Size - Supplies a pointer to a variable that receives the size in bytes of
        the version resource.

Return Value:

    PVOID - pointer to the version resource.

    NULL - failure.

--*/

{

    PIMAGE_RESOURCE_DATA_ENTRY DataEntry;
    PVOID DataPointer;
    ULONG Id;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceRes);
    DataPointer = NULL;
    Id = 1; // VS_VERSION_INFO;

    if (ImageBase == NULL || ImageSize == 0) {
        goto FindVersionEnd;
    }

    Status = ResFindDataEntryFromImage(ImageBase,
                                       ImageSize,
                                       16, //RT_VERSION
                                       &Id,
                                       NULL,
                                       &DataEntry,
                                       &DataPointer);

    if (!NT_SUCCESS(Status)) {
        goto FindVersionEnd;
    }

    *Size = DataEntry->Size;

FindVersionEnd:
    BlpInterfaceExit(InterfaceRes);
    return DataPointer;
}

BOOLEAN
BlResourceVerifyOriginalFilenameFromImage (
    __in PVOID ImageBase,
    __in ULONG ImageSize,
    __in PCWSTR Filename
    )

/*++

Routine Description:

    This routine determines if a supplied file name matches the Original
    Filename value within an image's version resource.

Arguments:

    ImageBase - Supplies the base of the loaded image in memory.

    ImageSize - Supplies the size of the loaded image.

    Filename - Supplies a pointer to a string containing the file name to
         verify against the version resource of the image.

Return Value:

    TRUE if the file name matches the Original Filename.

    FALSE if it is not a match, including if there is no version resource.

--*/

{

    ULONG FilenameLength;
    BOOLEAN Match;
    ULONG PaddedLength;
    PVOID Resource;
    ULONG ResourceSize;
    PCWSTR StartOfFileName;
    PCRES_STRING String;
    PCRES_STRINGFILEINFO StringInfo;
    PCRES_STRINGTABLE StringTable;
    PCWSTR Value;
    PCRES_VS_VERSIONINFO VersionInfo;

    Match = FALSE;

    //
    // Find the version resource.
    //

    Resource = BlResourceFindVersionFromImage(ImageBase,
                                              ImageSize,
                                              &ResourceSize);

    if (Resource == NULL || ResourceSize == 0) {
        goto VerifyOriginalFilenameFromImageEnd;
    }

    //
    // Check that the resource is a VS_VERSION_INFO.
    //

    VersionInfo = (PCRES_VS_VERSIONINFO)Resource;
    if (ResourceSize < sizeof(RES_VS_VERSIONINFO) ||
        ResourceSize < VersionInfo->wLength) {

        goto VerifyOriginalFilenameFromImageEnd;
    }

    if (wcscmp(VersionInfo->VersionInfoString, RES_VS_VERSION_INFO_STRING) != 0) {
        goto VerifyOriginalFilenameFromImageEnd;
    }

    ResourceSize -= sizeof(RES_VS_VERSIONINFO);
    if (ResourceSize < sizeof(RES_STRINGFILEINFO)) {
        goto VerifyOriginalFilenameFromImageEnd;
    }

    //
    // Get the StringFileInfo and validate that it is a text type.
    //

    StringInfo = (PCRES_STRINGFILEINFO)(VersionInfo + 1);

    if (StringInfo->wLength > ResourceSize ||
        StringInfo->wValueLength != 0 ||
        StringInfo->wType != RES_TYPE_TEXT ||
        wcscmp(StringInfo->StringFileInfoString, RES_STRINGFILEINFO_STRING) != 0) {
            goto VerifyOriginalFilenameFromImageEnd;
    }

    ResourceSize -= sizeof(RES_STRINGFILEINFO);
    if (ResourceSize < sizeof(RES_STRINGTABLE)) {
        goto VerifyOriginalFilenameFromImageEnd;
    }

    //
    // Get the StringTable of the StringFileInfo.
    //

    StringTable = (PCRES_STRINGTABLE)(StringInfo + 1);

    if (StringTable->wLength > ResourceSize ||
        StringTable->wValueLength != 0 ||
        StringTable->wType != RES_TYPE_TEXT) {
            goto VerifyOriginalFilenameFromImageEnd;
    }

    //
    // Jump to the file name if a path is supplied.
    //

    StartOfFileName = wcsrchr(Filename, L'\\');
    if (StartOfFileName != NULL) {
        Filename = StartOfFileName + 1;
    }

    //
    // Search the string table for "OriginalFilename" and check that its value
    // equals the filename we are looking for.
    //

    FilenameLength = (ULONG)wcslen(Filename) + 1;

    ResourceSize -= sizeof(RES_STRINGTABLE);
    String = (PCRES_STRING)(StringTable + 1);

    while (ResourceSize >= sizeof(RES_STRING)) {
        if (String->wLength > ResourceSize ||
            String->wType != RES_TYPE_TEXT) {
                goto VerifyOriginalFilenameFromImageEnd;
        }

        if (String->wValueLength == FilenameLength &&
            wcscmp(&String->szKey, RES_ORIGINALFILENAME_STRING) == 0) {

            Value = &String->szKey + ARRAYSIZE(RES_ORIGINALFILENAME_STRING);
            if (_wcsicmp(Value, Filename) == 0) {
                Match = TRUE;
            }

            goto VerifyOriginalFilenameFromImageEnd;
        }

        //
        // Need to pad the length up to the next 32-bit aligned.
        //

        PaddedLength = ALIGN_RANGE_UP(String->wLength, sizeof(ULONG));

        //
        // In the case of integer overflow exit immediately.
        //

        if (PaddedLength < String->wLength) {
            goto VerifyOriginalFilenameFromImageEnd;
        }

        if (PaddedLength > ResourceSize) {
            goto VerifyOriginalFilenameFromImageEnd;
        }

        String = (PCRES_STRING)Add2Ptr(String, PaddedLength);
        ResourceSize -= PaddedLength;
    }

VerifyOriginalFilenameFromImageEnd:
    return Match;
}

// --------------------------------------------------------- Internal Functions

BOOLEAN
ResDoMuiResourceChecksumsMatch (
    __in PRC_CONFIG PrimaryMuiResource,
    __in PRC_CONFIG SecondaryMuiResource
    )

/*++

Routine Description:

    Determines if two MUI resource checksums match.

Arguments:

    PrimaryMuiResource - Supplies the primary mui resource to compare against.

    SecondaryMuiResource - Supplies the secondary mui resource which is compared
        to the Primary resource.

Return Value:

    TRUE if the resources match.

    FALSE otherwise.

--*/

{

    BOOLEAN ReturnValue;

    //
    // A MUI resource must have a valid signature.
    //

    if ((PrimaryMuiResource->dwSignature != RC_CONFIG_SIGNATURE) ||
        (SecondaryMuiResource->dwSignature != RC_CONFIG_SIGNATURE)) {

        return FALSE;
    }

    if (RtlEqualMemory(&PrimaryMuiResource->dwServiceCheckSum,
                       &SecondaryMuiResource->dwServiceCheckSum,
                       sizeof(PrimaryMuiResource->dwServiceCheckSum)) == FALSE) {

        ReturnValue = FALSE;

    } else {
        ReturnValue = TRUE;
    }

    return ReturnValue;
}

NTSTATUS
ResFindDataEntry (
    __in ULONG Section,
    __in_opt PULONG Id,
    __in_opt PWSTR Name,
    __deref_out PIMAGE_RESOURCE_DATA_ENTRY *DataEntryOut,
    __deref_out_bcount((*DataEntryOut)->Size) PVOID *ResourceOut
    )

/*++

Routine Description:

    This routine finds a resource data entry in the given resource section.
    It first searches the MUI file and if the resource is not present falls
    back to using the application image.

Arguments:

    Section - Supplies the number of the resource section to look up.

    Id - Supplies the numbered ID of the resource to look up.  Either this
        parameter or Name must be supplied.

    Name - Supplies the named ID of the resource to look up.  Either this
        parameter or Id must be supplied.

    DataEntryOut - Supplies a pointer to a variable that receives the location
        of the resource data entry in the image.

    ResourceOut - Supplies a pointer to a variable that receives the location
        of the resource itself in the image.

Return Value:

    STATUS_SUCCESS if the resource is found.

    STATUS_NOT_FOUND otherwise.

--*/

{

    NTSTATUS Status;

    Status = ResFindDataEntryFromImage(NULL,
                                       0,
                                       Section,
                                       Id,
                                       Name,
                                       DataEntryOut,
                                       ResourceOut);

    return Status;
}

NTSTATUS
ResFindDataEntryFromImage (
    __in_opt PVOID ImageBase,
    __in_opt ULONG ImageSize,
    __in ULONG Section,
    __in_opt PULONG Id,
    __in_opt PWSTR Name,
    __deref_out PIMAGE_RESOURCE_DATA_ENTRY *DataEntryOut,
    __deref_out PVOID *ResourceOut
    )

/*++

Routine Description:

    This routine finds a resource data entry in the given resource section.
    It first searches the MUI file and if the resource is not present falls
    back to using the application image.

Arguments:

    ImageBase - Supplies the base of the loaded image in memory. (optional)

    ImageSize - Supplies the size of the loaded image. (optional)

    Section - Supplies the number of the resource section to look up.

    Id - Supplies the numbered ID of the resource to look up.  Either this
        parameter or Name must be supplied.

    Name - Supplies the named ID of the resource to look up.  Either this
        parameter or Id must be supplied.

    DataEntryOut - Supplies a pointer to a variable that receives the location
        of the resource data entry in the image.

    ResourceOut - Supplies a pointer to a variable that receives the location
        of the resource itself in the image.

Return Value:

    STATUS_SUCCESS if the resource is found.

    STATUS_NOT_FOUND otherwise.

--*/

{

    PIMAGE_RESOURCE_DATA_ENTRY DataEntry;
    PVOID DataPointer;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY DirectoryEntry;
    BOOLEAN Fallback;
    ULONG Index;
    PIMAGE_RESOURCE_DIRECTORY NextDirectory;
    PVOID PeImageBase;
    PVOID PeImageEnd;
    PIMAGE_RESOURCE_DIRECTORY RootDirectory;
    ULONG RootDirectoryOffset;
    PIMAGE_SECTION_HEADER SectionHeader;
    NTSTATUS Status;

    Fallback = TRUE;
    Status = STATUS_NOT_FOUND;

    //
    // If the caller specified an image to search, then get the root directory
    // from that image and search within it. Otherwise attempt to load resources
    // from the boot application's MUI file or from the boot application image
    // itself.
    //

    if (ARGUMENT_PRESENT(ImageBase) != FALSE) {
        if (ImageSize == 0) {
            goto FindDataEntryEnd;
        }

        SectionHeader = BlImgFindSection(ImageBase, ImageSize, ".rsrc");
        if (SectionHeader == NULL) {
            Status = STATUS_INVALID_IMAGE_FORMAT;
            goto FindDataEntryEnd;
        }

        PeImageBase = ImageBase;
        PeImageEnd = Add2Ptr(ImageBase, ImageSize);
        RootDirectoryOffset = SectionHeader->VirtualAddress;
        RootDirectory = Add2Ptr(ImageBase, RootDirectoryOffset);
        if ((PVOID)RootDirectory < ImageBase) {
            Status = STATUS_INVALID_PARAMETER;
            goto FindDataEntryEnd;
        }

        Fallback = FALSE;

    } else {

        //
        // Data resources may not be split into the MUI file.  If the resource
        // is not present in the MUI file, then load from the actual application
        // image, and not the MUI file.
        //

        PeImageBase = ResPeImageBase;
        PeImageEnd = ResPeImageEnd;
        RootDirectory = ResRootDirectory;
        RootDirectoryOffset = ResRootDirectoryOffset;
    }

FindDataEntryTryAgain:

    //
    // Error out early if the boot application does not have a resource
    // section.
    //

    if (RootDirectory == NULL) {
        goto FindDataEntryEnd;
    }

    Index = Section;
    NextDirectory = ResFindDirectoryEntry(RootDirectory,
                                          &Index,
                                          NULL,
                                          (PUCHAR)RootDirectory);

    if (NextDirectory == NULL) {
        if ((Fallback != FALSE) &&
            (RootDirectory != ResRootDirectoryFallback)) {

            PeImageBase = ResPeImageBaseFallback;
            PeImageEnd = ResPeImageEndFallback;
            RootDirectory = ResRootDirectoryFallback;
            RootDirectoryOffset = ResRootDirectoryFallbackOffset;
            goto FindDataEntryTryAgain;
        }

        goto FindDataEntryEnd;
    }

    NextDirectory = ResFindDirectoryEntry(NextDirectory,
                                          Id,
                                          Name,
                                          (PUCHAR)RootDirectory);

    if (NextDirectory == NULL) {
        goto FindDataEntryEnd;
    }

    DirectoryEntry = Add2Ptr(NextDirectory,
                             sizeof(IMAGE_RESOURCE_DIRECTORY));

    if ((PVOID)DirectoryEntry < (PVOID)NextDirectory) {
        Status = STATUS_INVALID_PARAMETER;
        goto FindDataEntryEnd;
    }

    DataEntry = Add2Ptr(RootDirectory, DirectoryEntry->OffsetToData);
    if (((PVOID)DataEntry < PeImageBase) || ((PVOID)DataEntry > PeImageEnd)) {
        Status = STATUS_INVALID_PARAMETER;
        goto FindDataEntryEnd;
    }

    DataPointer = Add2Ptr(PeImageBase, DataEntry->OffsetToData);
    if (((PVOID)DataPointer < PeImageBase) ||
        ((PVOID)DataPointer > PeImageEnd)) {

        Status = STATUS_INVALID_PARAMETER;
        goto FindDataEntryEnd;
    }

    if ((Add2Ptr(DataPointer, DataEntry->Size) > PeImageEnd) ||
        (Add2Ptr(DataPointer, DataEntry->Size) < DataPointer)) {

        Status = STATUS_INVALID_PARAMETER;
        goto FindDataEntryEnd;
    }

    *DataEntryOut = DataEntry;
    *ResourceOut = DataPointer;
    Status = STATUS_SUCCESS;

FindDataEntryEnd:
    return Status;
}

PVOID
ResFindDirectoryEntry (
    __in PIMAGE_RESOURCE_DIRECTORY Directory,
    __in_opt PULONG Id,
    __in_opt PWSTR Name,
    __in PUCHAR SectionStart
    )

/*++

Routine Description:

    Searches through a resource directory for the requested entry.  Entries
    can only be specified by ID.

    If the provided ID is -1, the first entry is returned.

Arguments:

    Directory - Supplies the resource directory to search.

    Id - Supplies a pointer to the ID to search for.  -1 means return the
        first ID found.

    Name - Supplies a pointer to a null-terminated string of the resource
        name to search for.

    SectionStart - Supplies a pointer to the start of the resource section.

Return Value:

    Pointer to the found resource directory.

    NULL for failure.

--*/

{

    PIMAGE_RESOURCE_DIR_STRING_U DirectoryName;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY FoundDirectory;
    ULONG Index;
    SIZE_T CharLen;
    ULONG Offset;

    //
    // One or the other, but not both...
    //

    if (((Id == NULL) && (Name == NULL)) ||
        ((Id != NULL) && (Name != NULL))) {
        return NULL;
    }

    FoundDirectory = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)(Directory + 1);

    //
    // Search for resource by Id.  (Skip over named resources.)
    //

    if (Id != NULL) {
        FoundDirectory += Directory->NumberOfNamedEntries;

        //
        // Return the pointer to the requested directory entry.
        //

        for (Index = 0; Index < Directory->NumberOfIdEntries; Index++) {
            if ((FoundDirectory->Id == (USHORT)*Id) || (*Id == (ULONG)-1)) {

                //
                // Found a match.  Return a pointer to the start of the
                // requested entry.
                //
                // N.B. The high bit of OffsetToData is set if the entry is
                //      another directory.
                //

                Offset = (FoundDirectory->OffsetToData &
                          ~IMAGE_RESOURCE_DATA_IS_DIRECTORY);

                return Add2Ptr(SectionStart, Offset);
            }

            FoundDirectory += 1;
        }

        return NULL;
    }

    //
    // Search for a named resource.
    //

    for (Index = 0; Index < Directory->NumberOfNamedEntries; Index++) {
        Offset = (FoundDirectory->NameOffset &
                  ~IMAGE_RESOURCE_NAME_IS_STRING);

        DirectoryName = Add2Ptr(SectionStart, Offset);
        CharLen = wcslen(Name);
        if ((CharLen == DirectoryName->Length) &&
            (RtlCompareMemory(DirectoryName->NameString, Name, CharLen) ==
             CharLen)) {

            //
            // Found a match.  Return a pointer to the start of the
            // requested entry.
            //
            // N.B. The high bit of OffsetToData is set if the entry is
            //      another directory.
            //

            Offset = (FoundDirectory->OffsetToData &
                      ~IMAGE_RESOURCE_DATA_IS_DIRECTORY);

            return Add2Ptr(SectionStart, Offset);
        }

        FoundDirectory += 1;
    }

    return NULL;
}

PRC_CONFIG
ResFindMuiSection (
    __in PIMAGE_RESOURCE_DIRECTORY Directory,
    __in ULONG DataOffset
    )

/*++

Routine Description:

    Searches through a resource directory for a resource configuration section
    which is a child of the MUI directory.

Arguments:

    Directory - Supplies the resource directory to search.

    DataOffset - Supplies the offset of the data from the base of the file.

Return Value:

    Pointer to the found resource configuration section.

    NULL for failure.

--*/

{

    PIMAGE_RESOURCE_DATA_ENTRY ConfigDataEntry;
    PIMAGE_RESOURCE_DIRECTORY ConfigDirectory;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY ConfigDirectoryEntry;
    PRC_CONFIG ConfigSection;
    ULONG Id;
    PIMAGE_RESOURCE_DIRECTORY MuiDirectory;

    ConfigSection = NULL;

    if (Directory == NULL) {
        goto FindMuiSectionEnd;
    }

    //
    // Locate the parent MUI directory.
    //

    MuiDirectory = ResFindDirectoryEntry(Directory,
                                         NULL,
                                         L"MUI",
                                         (PUCHAR)Directory);

    if (MuiDirectory == NULL) {
        goto FindMuiSectionEnd;
    }

    //
    // The MUI directory has only one child which is another directory.
    //

    Id = (ULONG)-1;
    ConfigDirectory = ResFindDirectoryEntry(MuiDirectory,
                                            &Id,
                                            NULL,
                                            (PUCHAR)Directory);

    if (ConfigDirectory == NULL) {
        goto FindMuiSectionEnd;
    }

    //
    // The configuration directory entry contains a pointer to the configuration
    // data.  The data offset can be used to calculate the actual pointer to the
    // resource configuration section.
    //

    ConfigDirectoryEntry = Add2Ptr(ConfigDirectory,
                                   sizeof(IMAGE_RESOURCE_DIRECTORY));

    ConfigDataEntry = Add2Ptr(Directory, ConfigDirectoryEntry->OffsetToData);
    ConfigSection = Add2Ptr(Directory,
                            ConfigDataEntry->OffsetToData - DataOffset);

FindMuiSectionEnd:
    return ConfigSection;
}


NTSTATUS
ResInitializeMuiResources (
    VOID
    )

/*++

Routine Description:

    Finds and loads the MUI file for the currently executing boot application.

Arguments:

    None.

Return Value:

    NT Status code indicating outcome.

--*/

{

    PWSTR ApplicationBaseDirectory;
    PBOOT_ENVIRONMENT_DEVICE ApplicationDevice;
    PWSTR ApplicationImageName;
    PRC_CONFIG BaseFileMuiSection;
    OPEN_FLAGS DeviceFlags;
    DEVICE_ID DeviceId;
    PVOID ImageBase;
    ULONG ImageFlags;
    ULONG ImageSize;
    PWSTR LocalePath;
    ULONG LocalLoadInformation;
    PRC_CONFIG MuiFileMuiSection;
    PWSTR MuiPath;
    ULONG MuiPathSize;
    PIMAGE_SECTION_HEADER SectionHeader;
    NTSTATUS Status;
    PIMAGE_RESOURCE_DIRECTORY UnvalidatedMuiFileDirectory;
    ULONG UnvalidatedMuiFileOffset;

    ApplicationDevice = NULL;
    DeviceId = INVALID_DEVICE_ID;
    ApplicationBaseDirectory = NULL;
    ApplicationImageName = NULL;
    ImageBase = NULL;
    ImageSize = 0;
    LocalePath = NULL;
    MuiPath = NULL;

    //
    // Generate the path to the MUI file, which is of the form:
    //     <application path>\<locale id>\<ImageName>.MUI
    //
    // Start by retreiving the constituent parts, then allocate sufficient
    // space.
    //

    Status = BlGetApplicationImageName(&ApplicationBaseDirectory,
                                       &ApplicationImageName);

    if (!NT_SUCCESS(Status)) {
        goto InitializeMuiResourcesExit;
    }

    Status = BlGetApplicationOptionString(BCDE_LIBRARY_TYPE_PREFERRED_LOCALE,
                                          &LocalePath);

    if (!NT_SUCCESS(Status)) {
        goto InitializeMuiResourcesExit;
    }

    //
    // Calculate the required size, including space for seperate backslashes,
    // and a terminating .MUI string (including a null terminator).
    //

    MuiPathSize = (ULONG)wcslen(ApplicationBaseDirectory);
    MuiPathSize += 1;
    MuiPathSize += (ULONG)wcslen(LocalePath);
    MuiPathSize += 1;
    MuiPathSize += (ULONG)wcslen(ApplicationImageName);
    MuiPathSize += 5;
    MuiPathSize *= sizeof(WCHAR);
    MuiPath = BlMmAllocateHeap(MuiPathSize);
    if (MuiPath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto InitializeMuiResourcesExit;
    }

    swprintf_s(MuiPath,
               MuiPathSize / sizeof(WCHAR),
               L"%s\\%s\\%s.MUI",
               ApplicationBaseDirectory,
               LocalePath,
               ApplicationImageName);

    Status = BlGetApplicationOptionDevice(BCDE_LIBRARY_TYPE_APPLICATION_DEVICE,
                                          &ApplicationDevice,
                                          NULL);

    if (!NT_SUCCESS(Status)) {
        goto InitializeMuiResourcesExit;
    }

    //
    // Open the device containing the MUI files.  A UDP device must be
    // opened with both read and write access since packets must be sent and
    // received in order to TFTP download the MUI files.
    //

    if (ApplicationDevice->DeviceType == DEVICE_UDP) {
        DeviceFlags = OPEN_READ_WRITE;

    } else {
        DeviceFlags = OPEN_READ;
    }

    Status = BlDeviceOpen(ApplicationDevice,
                          DeviceFlags,
                          &DeviceId);

    if (!NT_SUCCESS(Status)) {
        goto InitializeMuiResourcesExit;
    }

    //
    // Determine the image code integrity flags, if any, that should be used
    // when loading this boot application's resources into memory.
    //

    Status = BlpResourceCodeIntegrityFlags(BlGetApplicationEntry(),
                                           &ImageFlags);

    if (!NT_SUCCESS(Status)) {
        goto InitializeMuiResourcesExit;
    }

    ImageFlags |= MUI_IMAGE_FLAGS;

    //
    // For PCAT BootMgr, skip signature expiration check.
    //

#ifdef MISC_MIN
    ImageFlags |= IMAGE_FLAGS_NO_SIGNATURE_EXPIRATION_CHECKS;
#endif

    //
    // Load the MUI file and find the resource section.  If the resource section
    // matches the primary file, the MUI file is valid and the pointers to the
    // MUI resources can be saved into global variables for later use.
    //

    Status = BlImgLoadPEImageWithInfo(DeviceId,
                                      MEMORY_TYPE_APPLICATION_DATA_STRUCTURE,
                                      MuiPath,
                                      &ImageBase,
                                      &ImageSize,
                                      NULL,
                                      ImageFlags,
                                      &LocalLoadInformation);

    if (!NT_SUCCESS(Status)) {
        goto InitializeMuiResourcesExit;
    }

    SectionHeader = BlImgFindSection(ImageBase, ImageSize, ".rsrc");
    if (SectionHeader == NULL) {
        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto InitializeMuiResourcesExit;
    }

    UnvalidatedMuiFileOffset = SectionHeader->VirtualAddress;
    UnvalidatedMuiFileDirectory = Add2Ptr(ImageBase, UnvalidatedMuiFileOffset);
    MuiFileMuiSection = ResFindMuiSection(UnvalidatedMuiFileDirectory,
                                          UnvalidatedMuiFileOffset);

    if (MuiFileMuiSection == NULL) {
        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto InitializeMuiResourcesExit;
    }

    BaseFileMuiSection = ResFindMuiSection(ResRootDirectoryFallback,
                                           ResRootDirectoryFallbackOffset);

    if (BaseFileMuiSection == NULL) {
        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto InitializeMuiResourcesExit;
    }

    if (ResDoMuiResourceChecksumsMatch(BaseFileMuiSection,
                                       MuiFileMuiSection) == FALSE) {

        BlStatusPrint(
            L"MUI: %s checksum does not match primary file checksum\n",
            MuiPath);

        Status = STATUS_IMAGE_CHECKSUM_MISMATCH;
        goto InitializeMuiResourcesExit;
    }

    ResMuiImageBase = ImageBase;
    ResMuiImageSize = ImageSize;
    ResRootDirectoryPrimaryOffset = UnvalidatedMuiFileOffset;
    ResRootDirectoryPrimary = UnvalidatedMuiFileDirectory;
    ResPeImageBasePrimary = ImageBase;
    ResPeImageEndPrimary = Add2Ptr(ImageBase, ImageSize);
    ResLocale = LocalePath;
    LocalePath = NULL;
    Status = STATUS_SUCCESS;

InitializeMuiResourcesExit:
    if (DeviceId != INVALID_DEVICE_ID) {
        BlDeviceClose(DeviceId);
    }

    if (ApplicationDevice != NULL) {
        BlMmFreeHeap(ApplicationDevice);
    }

    if (ApplicationBaseDirectory != NULL) {
        BlMmFreeHeap(ApplicationBaseDirectory);
    }

    if (ApplicationImageName != NULL) {
        BlMmFreeHeap(ApplicationImageName);
    }

    if (LocalePath != NULL) {
        BlMmFreeHeap(LocalePath);
    }

    if (MuiPath != NULL) {
        BlMmFreeHeap(MuiPath);
    }

    if (!NT_SUCCESS(Status)) {
        if (ImageBase != NULL) {
            BlImgUnLoadImage(ImageBase,
                             ImageSize,
                             IMAGE_FLAGS_MAP_ONE_TO_ONE);
        }
    }

    return Status;
}

NTSTATUS
ResSelectLocale (
    __in BOOLEAN UsePrimaryLocale
    )

/*++

Routine Description:

    This routine selects the locale to use at runtime.

Arguements:

    UsePrimaryLocale - Supplies a boolean indicating if the primary locale
        should be used.

Return Value:

    None.

--*/

{

    NTSTATUS Status;

    //
    // Register the locale with the display library.  A failure indicates that
    // the display engine can not support displaying character glyphs for the
    // specified locale.  If a failure does occur, fallback to the English
    // resources embedded in the image.
    //
    // N.B. If deferred font file loading is used, the error handling of
    //      missing or corrupt font files does not exist until resource lookup.
    //

    Status = STATUS_UNSUCCESSFUL;
    if (UsePrimaryLocale != FALSE) {

        ASSERT(ResLocale != NULL);

        ResRootDirectory = ResRootDirectoryPrimary;
        ResRootDirectoryOffset = ResRootDirectoryPrimaryOffset;
        ResPeImageBase = ResPeImageBasePrimary;
        ResPeImageEnd = ResPeImageEndPrimary;
        Status = BlpDisplayRegisterLocale(ResLocale);
    }

    if (!NT_SUCCESS(Status)) {
        ResRootDirectory = ResRootDirectoryFallback;
        ResRootDirectoryOffset = ResRootDirectoryFallbackOffset;
        ResPeImageBase = ResPeImageBaseFallback;
        ResPeImageEnd = ResPeImageEndFallback;
        Status = BlpDisplayRegisterLocale(DEFAULT_LOCALE);
    }

    //
    // When a locale was successfully registered with the display library,
    // set a global flag which will force a deferred font file to be loaded
    // before looking up a resource.  This will ensure that the necessary font
    // file is valid before committing to the locale.
    //
    // If a locale could not be registered, fallback to textmode and leverage
    // firmware services to display all string messages.
    //

    if (NT_SUCCESS(Status)) {
        ResLoadedFontFiles = FALSE;

    } else {
        Status = BlDisplaySetScreenResolution(TEXT_MODE_HORIZONTAL_RESOLUTION,
                                              TEXT_MODE_VERTICAL_RESOLUTION);
    }

    return Status;
}


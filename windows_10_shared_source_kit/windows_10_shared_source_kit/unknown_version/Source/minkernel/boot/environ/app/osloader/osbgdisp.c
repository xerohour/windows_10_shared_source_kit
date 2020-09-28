/*++

Copyright (c) Microsoft Corporation

Module Name:

    osbgdisp.c

Abstract:

    This module implements support for drawing screens with BGFX in the OS
    loader.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"

//
// ---------------------------------------------------------------- Definitions
//

#define REGION_LIST_FLAGS_NONE        0x00000000
#define REGION_LIST_FLAGS_WRAP_PARENT 0x00000001
#define REGION_LIST_FLAGS_WRAP_CHILD  0x00000002

#define INVALID_HANDLE (HANDLE)0

//
// Color definitions that match what BGFX expects.
//

#define BGFX_COLOR_BLACK 0xFF000000
#define BGFX_COLOR_DARK_BLUE 0xFF2067B2
#define BGFX_COLOR_LIGHT_BLUE 0xFFFFFFFF
#define BGFX_COLOR_DARK_WHITE 0xFFCDCDCD
#define BGFX_COLOR_WHITE 0xFFFFFFFF

//
// Minimum macro.
//

#define MIN(_a, _b) (((_a) <= (_b)) ? (_a): (_b))

//
// Display Offsets
//
// The following defines indicate where each display element will appear on the
// screen. They are all based off Z = Min(Height, Width). A square with sides
// of length Z is horizontally centered and vertically top-justified on the
// screen. All percentages are in reference to the Z-box. Z is in pixels.
//

//
// Header Information:
//
// The header left margin is 23% from the far left of the screen.
// The header is offset 5.5% of Z pixels from the top of the screen.
// The header is 7.5% of Z pixels in height.
// The header region is 80% of Z pixels wide.
//

#define HEADER_MARGIN 230
#define HEADER_OFFSET 55
#define HEADER_HEIGHT 75
#define HEADER_WIDTH 800

//
// Title Information:
//
// The title left margin is 23.4% from the far left of the screen.
// The title is offset 15% of Z pixels from the top of the screen.
// The title is 4% of Z pixels in height.
// The title is 80% of Z pixels in width.
//

#define TITLE_MARGIN 234
#define TITLE_OFFSET 150
#define TITLE_HEIGHT 40
#define TITLE_WIDTH 800

//
// Input Information:
//
// The input left margin is 23.5% from the far left of the screen.
// The input box is 20% of Z pixels from the top of the screen.
// The input text is 3% of Z in height.
// The input box is 70% of Z in width.
// The input box has padding that is 1% of Z in width.
// The input box has a minimum padding of 12 pixels.
//

#define INPUT_MARGIN 235
#define INPUT_OFFSET 200
#define INPUT_HEIGHT 30
#define INPUT_WIDTH 700
#define INPUT_PADDING 10
#define INPUT_PADDING_MINIMUM 8

//
// Body Information:
//
// The body left margin is 23.5% from the far left of the screen.
// If there is an input box, the body is 27% of Z from the top of the screen.
// If there is no input, the body is 21% of Z from the top of the screen.
// The small body text is 3% of Z in height.
// The large body text is 3% of Z in height.
// The body is 80% of Z in width.
//

#define BODY_MARGIN 235
#define BODY_OFFSET_INPUT 270
#define BODY_OFFSET_NO_INPUT 210
#define BODY_HEIGHT_SMALL 30
#define BODY_HEIGHT_LARGE 40
#define BODY_WIDTH 800

//
// Footer Information:
//
// The footer left margin is 23.4% from the far left of the screen.
// The footer starts 80% of Z from the top of the screen and grows up.
// The footer text is 4% of Z in height.
// The footer is 80% of Z in width.
//

#define FOOTER_MARGIN 234
#define FOOTER_OFFSET 800
#define FOOTER_HEIGHT 40
#define FOOTER_WIDTH 800

//
// The right margin is the one exception, as it is not a percentage of the
// Z-box. It is 7 percent of the total width.
//

#define RIGHT_MARGIN 70

//
// The above numbers are based on percentage out of 1000.
//

#define TOTAL_SIZE 1000

//
// Define for each screen element type.
//

typedef enum _BG_DISPLAY_ELEMENT {
    HeaderMargin,
    HeaderOffset,
    HeaderHeight,
    HeaderWidth,
    TitleMargin,
    TitleOffset,
    TitleHeight,
    TitleWidth,
    InputMargin,
    InputOffset,
    InputHeight,
    InputWidth,
    InputPadding,
    BodyMargin,
    BodyOffsetInput,
    BodyOffsetNoInput,
    BodyHeightSmall,
    BodyHeightLarge,
    BodyWidth,
    FooterMargin,
    FooterOffset,
    FooterHeight,
    FooterWidth,
    RightMargin
} BG_DISPLAY_ELEMENT, *PBG_DISPLAY_ELEMENT;

//
// ----------------------------------------------------------------- Structures
//

typedef struct _BG_DISPLAY_REGION {
    HANDLE Handle;
    PWSTR String;
    OFFSET Offset;
    ULONG Width;
    ULONG Height;
    ULONG Padding;
    ULONG CursorIndex;
    ULONG Foreground;
    ULONG Background;
    ULONG Flags;
} BG_DISPLAY_REGION, *PBG_DISPLAY_REGION;

typedef struct _BG_DISPLAY_REGION_LIST_ENTRY {
    LIST_ENTRY ListEntry;
    PBG_DISPLAY_REGION Region;
    ULONG Flags;
} BG_DISPLAY_REGION_LIST_ENTRY, *PBG_DISPLAY_REGION_LIST_ENTRY;

typedef struct _BG_DISPLAY_STATE {
    PBG_DISPLAY_REGION Header;
    ULONG TitleListCount;
    PBG_DISPLAY_REGION_LIST_ENTRY TitleList;
    PBG_DISPLAY_REGION Input;
    ULONG BodyListCount;
    PBG_DISPLAY_REGION_LIST_ENTRY BodyList;
    ULONG FooterListCount;
    PBG_DISPLAY_REGION_LIST_ENTRY FooterList;
} BG_DISPLAY_STATE, *PBG_DISPLAY_STATE;

//
// -------------------------------------------------------------------- Globals
//

PBG_DISPLAY_STATE OslDisplayState = NULL;
PBG_DISPLAY_STATE OslPreviousState = NULL;

//
// ------------------------------------------------ Privates Routine Prototypes
//

NTSTATUS
OslpBgDisplayAddRegionListItem (
    __in PBG_DISPLAY_REGION_LIST_ENTRY RegionListHead,
    __in PULONG RegionListCount,
    __in BOOLEAN InsertFirst,
    __deref_out PBG_DISPLAY_REGION *Region,
    __in OFFSET Offset,
    __in ULONG Width,
    __in ULONG Height,
    __in ULONG ForegroundColor,
    __in ULONG BackgroundColor,
    __in ULONG Flags
    );

NTSTATUS
OslpBgDisplayAddTitleRegion (
    __in_z PWCHAR String
    );

VOID
OslpBgDisplayClearRegion (
    __in PBG_DISPLAY_REGION Region,
    __in ULONG RegionListFlags
    );

VOID
OslpBgDisplayClearRegionList (
    __in PBG_DISPLAY_REGION_LIST_ENTRY RegionList
    );

NTSTATUS
OslpBgDisplayClearScreen (
    __in ULONG Color
    );

NTSTATUS
OslpBgDisplayDestroy (
    __in PBG_DISPLAY_STATE *DisplayState
    );

VOID
OslpBgDisplayDestroyRegion (
    __in PBG_DISPLAY_REGION Region,
    __in ULONG RegionListFlags
    );

VOID
OslpBgDisplayDestroyRegionList (
    __in PBG_DISPLAY_REGION_LIST_ENTRY RegionList
    );

NTSTATUS
OslpBgDisplayInitializeHeader (
    __deref_out PBG_DISPLAY_REGION *Region
    );

NTSTATUS
OslpBgDisplayInitializeInput (
    __deref_out PBG_DISPLAY_REGION *Region
    );

NTSTATUS
OslpBgDisplayInitializeRegion (
    __deref_out PBG_DISPLAY_REGION *Region,
    __in OFFSET Offset,
    __in ULONG Width,
    __in ULONG Height,
    __in ULONG Padding,
    __in ULONG CursorIndex,
    __in ULONG Foreground,
    __in ULONG Background,
    __in ULONG Flags
    );

NTSTATUS
OslpBgDisplayInitializeRegionList (
    __deref_out PBG_DISPLAY_REGION_LIST_ENTRY *RegionListHead
    );

VOID
OslpBgDisplayRenderInputRegion (
    __in PBG_DISPLAY_REGION Region
    );

VOID
OslpBgDisplayRenderRegion (
    __in PBG_DISPLAY_REGION Region
    );

VOID
OslpBgDisplayRenderRegionList (
    __in PBG_DISPLAY_REGION_LIST_ENTRY RegionList,
    __in PULONG RegionListCount
    );

NTSTATUS
OslpBgDisplaySetInput (
    __in_z PWCHAR String,
    __in ULONG Flags,
    __in ULONG CursorIndex
    );

VOID
OslpBgDisplayValidateRegionWidth (
    __in ULONG RegionOffset,
    __inout PULONG RegionWidth
    );

ULONG
OslpBgGetElementSize (
    __in BG_DISPLAY_ELEMENT Element
    );

VOID
OslpBgInsertWrapRegion (
    __in PWCHAR String,
    __in PBG_DISPLAY_REGION_LIST_ENTRY RegionListEntry,
    __in PBG_DISPLAY_REGION_LIST_ENTRY RegionListHead,
    __in PULONG RegionListCount
    );

VOID
OslpBgUpdateInputBodyOffset (
    VOID
    );

//
// ------------------------------------------------------------------- Routines
//

NTSTATUS
OslBgDisplayAddBodyRegion (
    __in_z PWCHAR String,
    __in ULONG Flags,
    __out_opt HANDLE *Handle
    )

/*++

Routine Description:

    This routine adds a body region to the body region list using the the
    supplied string. It will additionally return the handle to the newly
    created body region so it can be updated later.

Arguments:

    String - Supplies a string to set for the newly added body region.

    Flags - Supplies flags for initializing this body text region.

    Handle - Supplies a pointer that receives the address of the created region
        handle. This is optional.

Return Value:

    NT status code.

--*/

{

    ULONG Height;
    PBG_DISPLAY_REGION LastRegion;
    PLIST_ENTRY ListEntry;
    OFFSET Offset;
    PBG_DISPLAY_REGION Region;
    PBG_DISPLAY_REGION_LIST_ENTRY RegionListEntry;
    NTSTATUS Status;
    ULONG Width;

    NT_ASSERT(OslDisplayState != NULL);

    if (OslDisplayState->BodyList == NULL) {
        Status = OslpBgDisplayInitializeRegionList(&OslDisplayState->BodyList);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        OslDisplayState->BodyListCount = 0;
    }

    //
    // This new region will be displayed below the last region in the list. If
    // the list is empty, it will be displayed at the starting body offset.
    //

    if (CHECK_FLAG(Flags, OSL_BGFX_FLAGS_BODY_TEXT_LARGE) != FALSE) {
        Height = OslpBgGetElementSize(BodyHeightLarge);

    } else {
        Height = OslpBgGetElementSize(BodyHeightSmall);
    }

    if (IsListEmpty(&OslDisplayState->BodyList->ListEntry) != FALSE) {
        if (OslDisplayState->Input == NULL) {
            Offset.Y = OslpBgGetElementSize(BodyOffsetNoInput);

        } else {
            Offset.Y = OslpBgGetElementSize(BodyOffsetInput);
        }

        Offset.X = OslpBgGetElementSize(BodyMargin);
        Width = OslpBgGetElementSize(BodyWidth);

        //
        // Make sure the width doesn't go off the edge of the screen.
        //

        OslpBgDisplayValidateRegionWidth(Offset.X, &Width);

    } else {
        ListEntry = OslDisplayState->BodyList->ListEntry.Blink;
        RegionListEntry = CONTAINING_RECORD(ListEntry,
                                            BG_DISPLAY_REGION_LIST_ENTRY,
                                            ListEntry);

        LastRegion = RegionListEntry->Region;
        Offset.X = LastRegion->Offset.X;
        Offset.Y = LastRegion->Offset.Y + LastRegion->Height;
        Width = LastRegion->Width;
    }

    //
    // Add the region to the body list, creating the region context based on
    // the dimension data gathered above.
    //

    Status = OslpBgDisplayAddRegionListItem(OslDisplayState->BodyList,
                                            &OslDisplayState->BodyListCount,
                                            FALSE,
                                            &Region,
                                            Offset,
                                            Width,
                                            Height,
                                            BGFX_COLOR_LIGHT_BLUE,
                                            BGFX_COLOR_DARK_BLUE,
                                            BG_DISPLAY_STRING_FLAGS_WRAP);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Set the string into the newly allocated region and return the region as
    // a handle if it was requested.
    //

    Region->String = String;
    if (ARGUMENT_PRESENT(Handle) != FALSE) {
        *Handle = Region;
    }

    return Status;
}

NTSTATUS
OslBgDisplayAddFooterRegion (
    __in_z PWCHAR String
    )

/*++

Routine Description:

    This routine adds a footer region to the footer region list using the the
    supplied string.

Arguments:

    String - Supplies a string to set for the newly added footer region.

Return Value:

    NT status code.

--*/

{

    ULONG Height;
    PBG_DISPLAY_REGION LastRegion;
    OFFSET Offset;
    PBG_DISPLAY_REGION Region;
    PBG_DISPLAY_REGION_LIST_ENTRY RegionListEntry;
    NTSTATUS Status;
    ULONG Width;

    NT_ASSERT(OslDisplayState != NULL);

    if (OslDisplayState->FooterList == NULL) {
        Status = OslpBgDisplayInitializeRegionList(
                     &OslDisplayState->FooterList);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        OslDisplayState->FooterListCount = 0;
    }

    //
    // This new region will be displayed above the last region in the list. If
    // the list is empty, it will be displayed at the starting footer offset.
    //

    if (IsListEmpty(&OslDisplayState->FooterList->ListEntry) != FALSE) {
        Offset.X = OslpBgGetElementSize(FooterMargin);
        Offset.Y = OslpBgGetElementSize(FooterOffset);
        Width = OslpBgGetElementSize(FooterWidth);
        Height = OslpBgGetElementSize(FooterHeight);

        //
        // Make sure the width doesn't go off the edge of the screen.
        //

        OslpBgDisplayValidateRegionWidth(Offset.X, &Width);

    } else {
        RegionListEntry = CONTAINING_RECORD(
                              OslDisplayState->FooterList->ListEntry.Flink,
                              BG_DISPLAY_REGION_LIST_ENTRY,
                              ListEntry);

        LastRegion = RegionListEntry->Region;
        Offset.X = LastRegion->Offset.X;
        Offset.Y = LastRegion->Offset.Y - LastRegion->Height;
        Height = LastRegion->Height;
        Width = LastRegion->Width;
    }

    //
    // Add the region to the footer list, creating the region context based on
    // the dimension data gathered above.
    //

    Status = OslpBgDisplayAddRegionListItem(OslDisplayState->FooterList,
                                            &OslDisplayState->FooterListCount,
                                            TRUE,
                                            &Region,
                                            Offset,
                                            Width,
                                            Height,
                                            BGFX_COLOR_WHITE,
                                            BGFX_COLOR_DARK_BLUE,
                                            BG_DISPLAY_STRING_FLAGS_WRAP);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Set the string into the newly allocated region.
    //

    Region->String = String;
    return Status;
}

NTSTATUS
OslBgDisplayClearScreen (
    VOID
    )

/*++

Routine Description:

    This routine clears the screen of all contents.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

    return OslpBgDisplayClearScreen(BGFX_COLOR_BLACK);
}

NTSTATUS
OslBgDisplayDestroy (
    VOID
    )

/*++

Routine Description:

    This routine destroys the display by destroying the current and previous
    display states and clearing the screen to black.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

    OslpBgDisplayDestroy(&OslPreviousState);
    OslpBgDisplayDestroy(&OslDisplayState);
    OslBgDisplayClearScreen();
    return STATUS_SUCCESS;
}

NTSTATUS
OslBgDisplayInitialize (
    VOID
    )

/*++

Routine Description:

    This routine initializes a BGFX display state, saving the previous state if
    it exists.

Arguments:

    None.

Return Value:

    None.

--*/

{

    //
    // If the display state has already been initialized, save it as the
    // previous state and allocate a new state for this new screen.
    //
    // N.B. Saving the previous state is an optimization for transitioning
    //      between screens.
    //

    if (OslDisplayState != NULL) {
        OslPreviousState = OslDisplayState;
        OslDisplayState = NULL;
    }

    //
    // Allocate memory for the display state.
    //

    OslDisplayState = BlMmAllocateHeap(sizeof(BG_DISPLAY_STATE));
    if (OslDisplayState == NULL) {
        return STATUS_NO_MEMORY;
    }

    //
    // Initialize the display state regions to NULL;
    //

    RtlZeroMemory(OslDisplayState, sizeof(BG_DISPLAY_STATE));
    return STATUS_SUCCESS;
}

NTSTATUS
OslBgDisplayInitializeInput (
    VOID
    )

/*++

Routine Description:

    This routine initializes the input region for the screen.

Arguments:

    None.

Return Value:

    None.

--*/

{

    return OslpBgDisplaySetInput(L" ", BG_DISPLAY_STRING_FLAGS_NONE, 0);
}

VOID
OslBgDisplayRender (
    VOID
    )

/*++

Routine Description:

    This routine renders the screen contents based on the current display
    state. It is optimized to avoid a full screen clear so it takes into
    account the previous screen contents.

Arguments:

    None.

Return Value:

    None.

--*/

{

    //
    // Do nothing if there is nothing new to display.
    //

    if (OslDisplayState == NULL) {
        return;
    }

    //
    // If this is the first bitlocker screen, clear the screen.
    //

    if (OslPreviousState == NULL) {
        OslpBgDisplayClearScreen(BGFX_COLOR_DARK_BLUE);
    }

    //
    // Render the header. The previous state does not need to be destroyed here
    // since the header expands the whole region.
    //

    OslpBgDisplayRenderRegion(OslDisplayState->Header);

    //
    // Render the title. The previous title, input, body and footer state needs
    // to be destroyed for a few reasons.
    //
    // Title, input and body:
    // 1. The previous title may have different number of regions than the new
    // title. Since the exact lines of the new title is unknown until rendered,
    // and the location of the input and title depend on the title, choose
    // to destroy all of them.
    //

    // Footer:
    // 1. The previous footer had more regions than the new footer.
    //

    if (OslPreviousState != NULL) {
        OslpBgDisplayClearRegionList(OslPreviousState->TitleList);
        OslpBgDisplayClearRegion(OslPreviousState->Input,
                                 REGION_LIST_FLAGS_NONE);

        OslpBgDisplayClearRegionList(OslPreviousState->BodyList);
        if ((OslPreviousState->FooterListCount >
                OslDisplayState->FooterListCount)) {

            OslpBgDisplayClearRegionList(OslPreviousState->FooterList);
        }
    }

    //
    // Display the title, input, body, and footer regions.
    //

    OslpBgDisplayRenderRegionList(OslDisplayState->TitleList,
                                  &OslDisplayState->TitleListCount);

    OslpBgUpdateInputBodyOffset();
    OslpBgDisplayRenderInputRegion(OslDisplayState->Input);
    OslpBgDisplayRenderRegionList(OslDisplayState->BodyList,
                                  &OslDisplayState->BodyListCount);

    OslpBgDisplayRenderRegionList(OslDisplayState->FooterList,
                                  &OslDisplayState->FooterListCount);

    //
    // Destroy the previous state.
    //

    OslpBgDisplayDestroy(&OslPreviousState);
    return;
}

NTSTATUS
OslBgDisplaySetHeader (
    __in_z PWCHAR String
    )

/*++

Routine Description:

    This routine sets the header string to the supplied value. If the header
    region has not yet been initialized, this routine initializes it.

Arguments:

    String - Supplies the string to display in the header region.

Return Value:

    NT status code.

--*/

{

    NTSTATUS Status;

    NT_ASSERT(OslDisplayState != NULL);

    if (OslDisplayState->Header == NULL) {
        Status = OslpBgDisplayInitializeHeader(&OslDisplayState->Header);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }
    }

    //
    // Free the old string.
    //
    // N.B. Some of the region strings are not heap allocated, but come from
    //      the resource file instead. This is OK because BlMmFreeHeap just
    //      ignores these allocations.
    //

    if (OslDisplayState->Header->String != NULL) {
        BlMmFreeHeap(OslDisplayState->Header->String);
    }

    OslDisplayState->Header->String = String;
    return STATUS_SUCCESS;
}

NTSTATUS
OslBgDisplaySetTitle (
    __in_z PWCHAR String
    )

/*++

Routine Description:

    This routine sets the title string to the supplied value. If the title
    region has not yet been initialized, this routine initializes it.

Arguments:

    String - Supplies the string to display in the title region.

Return Value:

    NT status code.

--*/

{

    NT_ASSERT(OslDisplayState != NULL);

    OslpBgDisplayDestroyRegionList(OslDisplayState->TitleList);
    OslDisplayState->TitleList = NULL;
    OslpBgDisplayAddTitleRegion(String);
    return STATUS_SUCCESS;
}

VOID
OslBgDisplayUpdateBodyRegion (
    __in HANDLE Handle,
    __in_z PWCHAR String
    )

/*++

Routine Description:

    This routine updates the body region associated with the given handle with
    the new string.

Arguments:

    Handle - Supplies a handle to the body region that needs to be updated.

    String - Supplies the string to display in the body region.

Return Value:

    None.

--*/

{

    PBG_DISPLAY_REGION Region;

    NT_ASSERT(Handle != INVALID_HANDLE);

    Region = (PBG_DISPLAY_REGION)Handle;

    //
    // Free the old string.
    //
    // N.B. Some of the region strings are not heap allocated, but come from
    //      the resource file instead. This is OK because BlMmFreeHeap just
    //      ignores these allocations.
    //

    if (Region->String != NULL) {
        BlMmFreeHeap(Region->String);
    }

    Region->String = String;

    //
    // Need to do something here in case this change wraps. Would need to
    // re-render the rest of the body list. That said, if the region wrapped
    // before, and this update also wraps, then the previous wrap would need to
    // be deleted. Perhaps need a BOOL to mark that it wrapped, so the next
    // region could be destroyed before the update?
    //

    OslpBgDisplayRenderRegion(Region);
    return;
}

NTSTATUS
OslBgDisplayUpdateInput (
    __in_z PWCHAR String,
    __in ULONG Flags,
    __in ULONG CursorIndex
    )

/*++

Routine Description:

    This routine updates the input region with the new string.

Arguments:

    String - Supplies the string to display in the input region.

    Flags - Supplies flags dictating how to display the strings.

    CursorIndex - Supplies the index of the cursor into the string.

Return Value:

    NT status code.

--*/

{

    NTSTATUS Status;

    Status = OslpBgDisplaySetInput(String, Flags, CursorIndex);
    if (NT_SUCCESS(Status)) {
        OslpBgDisplayRenderInputRegion(OslDisplayState->Input);
    }

    return Status;
}

NTSTATUS
OslBgDisplayUpdateTitle (
    __in_z PWCHAR String
    )

/*++

Routine Description:

    This routine updates the title region with the new string.

Arguments:

    String - Supplies the string to display in the title region.

Return Value:

    NT status code.

--*/

{

    ULONG PreviousTitleListCount;
    NTSTATUS Status;

    PreviousTitleListCount = 0;

    //
    // Clear the previous title region before update.
    //

    if (OslDisplayState->TitleList != NULL) {
        PreviousTitleListCount = OslDisplayState->TitleListCount;
    }

    Status = OslBgDisplaySetTitle(String);
    if (NT_SUCCESS(Status)) {
        OslpBgDisplayRenderRegionList(OslDisplayState->TitleList,
                                      &OslDisplayState->TitleListCount);

        //
        // If the current title and the previous title have different number
        // of regions, input and body are needed to be updated as well.
        //

        if (PreviousTitleListCount != OslDisplayState->TitleListCount &&
            PreviousTitleListCount != 0) {

            OslBgDisplayRender();
        }

    }

    return Status;
}

//
// ----------------------------------------------------------- Private Routines
//

NTSTATUS
OslpBgDisplayAddRegionListItem (
    __in PBG_DISPLAY_REGION_LIST_ENTRY RegionListHead,
    __in PULONG RegionListCount,
    __in BOOLEAN InsertFirst,
    __deref_out PBG_DISPLAY_REGION *Region,
    __in OFFSET Offset,
    __in ULONG Width,
    __in ULONG Height,
    __in ULONG ForegroundColor,
    __in ULONG BackgroundColor,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine initializes a text region and adds it to the supplied region
    list.

Arguments:

    RegionListHead - Supplies a pointer to the head of the list that the new
       region should be added to.

    RegionListCount - Supplies a pointer to the number of elements in the list.

    InsertFirst - Supplies an indication of whether the caller wants the
        element inserted first. The default is to add the region last.

    Region - Supplies a pointer to a value that receives the address of the
        newly allocated region.

    Offset - Supplies the offset on the display where the new region should be
        drawn.

    Width - Supplies the width of the new region.

    Height - Supplies the height of the new region.

    ForegroundColor - Supplies the foreground color of the new region.

    BackgroundColor - Supplies the background color of the new region.

    Flags - Supplies flags for the new region.

Return Value:

    NT status code.

--*/

{

    PBG_DISPLAY_REGION_LIST_ENTRY RegionListEntry;
    NTSTATUS Status;

    //
    // Initialize the region with the parameters passed into the routine.
    //

    Status = OslpBgDisplayInitializeRegion(Region,
                                           Offset,
                                           Width,
                                           Height,
                                           0,
                                           0,
                                           ForegroundColor,
                                           BackgroundColor,
                                           Flags);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Allocate a region list entry for this region and insert it into the list.
    //

    RegionListEntry = BlMmAllocateHeap(sizeof(BG_DISPLAY_REGION_LIST_ENTRY));
    if (RegionListEntry == NULL) {
        return STATUS_NO_MEMORY;
    }

    RegionListEntry->Flags = REGION_LIST_FLAGS_NONE;
    RegionListEntry->Region = *Region;
    if (InsertFirst != FALSE) {
        InsertHeadList(&RegionListHead->ListEntry, &RegionListEntry->ListEntry);

    } else {
        InsertTailList(&RegionListHead->ListEntry, &RegionListEntry->ListEntry);
    }

    *RegionListCount += 1;
    return Status;
}

NTSTATUS
OslpBgDisplayAddTitleRegion (
    __in_z PWCHAR String
    )

/*++

Routine Description:

    This routine initializes the title region based on the size of the display.

Arguments:

    String - Supplies the title text to be added to the title region.

Return Value:

    NT status code.

--*/

{

    ULONG Height;
    PBG_DISPLAY_REGION LastRegion;
    OFFSET Offset;
    PBG_DISPLAY_REGION Region;
    PBG_DISPLAY_REGION_LIST_ENTRY RegionListEntry;
    NTSTATUS Status;
    ULONG Width;

    NT_ASSERT(OslDisplayState != NULL);

    if (OslDisplayState->TitleList == NULL) {
        Status = OslpBgDisplayInitializeRegionList(&OslDisplayState->TitleList);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        OslDisplayState->TitleListCount = 0;
    }

    //
    // Initialize the display offsets. This new region will be displayed below
    // the last region in the list. If the list is empty, it will be displayed
    // at the starting title offset.
    //

    if (IsListEmpty(&OslDisplayState->TitleList->ListEntry) != FALSE) {
        Offset.X = OslpBgGetElementSize(TitleMargin);
        Offset.Y = OslpBgGetElementSize(TitleOffset);
        Width = OslpBgGetElementSize(TitleWidth);
        Height = OslpBgGetElementSize(TitleHeight);

        //
        // Make sure the width doesn't go off the edge of the screen.
        //

        OslpBgDisplayValidateRegionWidth(Offset.X, &Width);

    } else {
        RegionListEntry = CONTAINING_RECORD(
                              OslDisplayState->TitleList->ListEntry.Blink,
                              BG_DISPLAY_REGION_LIST_ENTRY,
                              ListEntry);

        LastRegion = RegionListEntry->Region;
        Offset.X = LastRegion->Offset.X;
        Offset.Y = LastRegion->Offset.Y + LastRegion->Height;
        Height = LastRegion->Height;
        Width = LastRegion->Width;
    }

    //
    // Add the region to the header list, creating the region context based on
    // the dimension data gathered above.
    //

    Status = OslpBgDisplayAddRegionListItem(OslDisplayState->TitleList,
                                            &OslDisplayState->TitleListCount,
                                            FALSE,
                                            &Region,
                                            Offset,
                                            Width,
                                            Height,
                                            BGFX_COLOR_WHITE,
                                            BGFX_COLOR_DARK_BLUE,
                                            BG_DISPLAY_STRING_FLAGS_WRAP);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Set the string into the newly allocated region.
    //

    Region->String = String;
    return Status;
}

VOID
OslpBgDisplayClearRegion (
    __in PBG_DISPLAY_REGION Region,
    __in ULONG RegionListFlags
    )

/*++

Routine Description:

    This routine clears the region by drawing a single space string in the
    region, and blending the foreground and background colors with the default
    dark blue screen background.

Arguments:

    Region - Supplies a pointer to the region that needs to be cleared.

    RegionListFlags - Supplies flags for the region list entry the region
        belongs to.

Return Value:

    None.

--*/

{

    PWCHAR Remainder;
    NTSTATUS Status;

    if (Region == NULL) {
        return;
    }

    //
    // Free the old string, being sure not to free a wrapped string twice.
    //
    // N.B. Some of the region strings are not heap allocated, but come from
    //      the resource file instead. This is OK because BlMmFreeHeap just
    //      ignores these allocations.
    //

    if ((Region->String != NULL) &&
        (CHECK_FLAG(RegionListFlags, REGION_LIST_FLAGS_WRAP_CHILD) == FALSE)) {

        BlMmFreeHeap(Region->String);
    }

    Region->Foreground = BGFX_COLOR_DARK_BLUE;
    Region->Background = BGFX_COLOR_DARK_BLUE;
    Region->String = L" ";

    //
    // Regions that can wrap require a remainder argument and cannot take a
    // cursor index. Treat them separately here.
    //

    if (CHECK_FLAG(Region->Flags, BG_DISPLAY_STRING_FLAGS_WRAP) != FALSE) {
        Status = BgDisplayStringEx(Region->String,
                                   &Region->Offset,
                                   &Region->Width,
                                   &Region->Height,
                                   &Region->Foreground,
                                   &Region->Background,
                                   NULL,
                                   Region->Flags,
                                   NULL,
                                   &Region->Handle,
                                   &Remainder,
                                   NULL);

    } else {
        Status = BgDisplayStringEx(Region->String,
                                   &Region->Offset,
                                   &Region->Width,
                                   &Region->Height,
                                   &Region->Foreground,
                                   &Region->Background,
                                   NULL,
                                   Region->Flags,
                                   &Region->CursorIndex,
                                   &Region->Handle,
                                   NULL,
                                   &Region->Padding);
    }

    NT_ASSERT(NT_SUCCESS(Status));

    //
    // Set the string to NULL.
    //

    Region->String = NULL;
    return;
}

VOID
OslpBgDisplayClearRegionList (
    __in PBG_DISPLAY_REGION_LIST_ENTRY RegionList
    )

/*++

Routine Description:

    This routine traverses a region list and clears each region from the
    screen.

Arguments:

    RegionList - Supplies a region list whose regions need to be cleared from
        the screen.

Return Value:

    None.

--*/

{

    PLIST_ENTRY CurrentEntry;
    PLIST_ENTRY ListHead;
    PBG_DISPLAY_REGION_LIST_ENTRY RegionListEntry;

    if (RegionList == NULL) {
        return;
    }

    ListHead = &RegionList->ListEntry;
    if (IsListEmpty(ListHead) != FALSE) {
        return;
    }

    CurrentEntry = ListHead->Flink;
    while (CurrentEntry != ListHead) {
        RegionListEntry = CONTAINING_RECORD(CurrentEntry,
                                            BG_DISPLAY_REGION_LIST_ENTRY,
                                            ListEntry);

        OslpBgDisplayClearRegion(RegionListEntry->Region,
                                 RegionListEntry->Flags);

        CurrentEntry = CurrentEntry->Flink;
    }

    return;
}

NTSTATUS
OslpBgDisplayClearScreen (
    __in ULONG Color
    )

/*++

Routine Description:

    This routine clears the screen with the specified color and invalidates any
    BGRT table that may be describing an OEM bitmap on the screen.

Arguments:

    Color - Supplies the color to use when clearing the screen.

Return Value:

    NT status code.

--*/

{

    NTSTATUS Status;

    Status = BgClearScreen(Color);
    if (NT_SUCCESS(Status)) {
        BlDisplayInvalidateOemBitmap();
    }

    return Status;
}

NTSTATUS
OslpBgDisplayDestroy (
    __in PBG_DISPLAY_STATE *DisplayState
    )

/*++

Routine Description:

    This routine destroys the provided display state. It does not actually
    manipulate the screen contents.

Arguments:

    DisplayState - Supplies a pointer to the address of the display state to be
        destroyed.

Return Value:

    NT status code.

--*/

{

    PBG_DISPLAY_STATE LocalDisplayState;

    LocalDisplayState = *DisplayState;
    if (LocalDisplayState != NULL) {
        OslpBgDisplayDestroyRegion(LocalDisplayState->Header,
                                   REGION_LIST_FLAGS_NONE);

        OslpBgDisplayDestroyRegionList(LocalDisplayState->TitleList);
        OslpBgDisplayDestroyRegion(LocalDisplayState->Input,
                                   REGION_LIST_FLAGS_NONE);

        OslpBgDisplayDestroyRegionList(LocalDisplayState->BodyList);
        OslpBgDisplayDestroyRegionList(LocalDisplayState->FooterList);
        BlMmFreeHeap(LocalDisplayState);
        *DisplayState = NULL;
    }

    return STATUS_SUCCESS;
}

VOID
OslpBgDisplayDestroyRegion (
    __in PBG_DISPLAY_REGION Region,
    __in ULONG RegionListFlags
    )

/*++

Routine Description:

    This routine destroys an individual region, freeing all of its allocations.

Arguments:

    Region - Supplies a pointer to the region that needs to be destroyed.

    RegionListFlags - Supplies the region list flags of the list entry that
        contains this region.

Return Value:

    None.

--*/

{

    NTSTATUS Status;

    if (Region == NULL) {
        return;
    }

    //
    // Destroy the BGFX text region by passing in the destroy flag and the
    // region handle.
    //

    if (Region->Handle != INVALID_HANDLE) {
        Status = BgDisplayStringEx(NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   NULL,
                                   BG_DISPLAY_STRING_FLAGS_DESTROY,
                                   NULL,
                                   &Region->Handle,
                                   NULL,
                                   NULL);

        NT_ASSERT(Status == STATUS_SUCCESS);

    }

    //
    // Free the old string. Do not free a wrapped string twice.
    //
    // N.B. Some of the region strings are not heap allocated, but come from
    //      the resource file instead. This is OK because BlMmFreeHeap just
    //      ignores these allocations.
    //

    if ((Region->String != NULL) &&
        (CHECK_FLAG(RegionListFlags, REGION_LIST_FLAGS_WRAP_CHILD) == FALSE)) {

        BlMmFreeHeap(Region->String);
    }

    BlMmFreeHeap(Region);
    return;
}

VOID
OslpBgDisplayDestroyRegionList (
    __in PBG_DISPLAY_REGION_LIST_ENTRY RegionList
    )

/*++

Routine Description:

    This routine traverses a region list and destroys each region in the list.

Arguments:

    RegionList - Supplies a pointer to a region list to be destroyed.

Return Value:

    None.

--*/

{

    PLIST_ENTRY CurrentEntry;
    PLIST_ENTRY ListHead;
    PBG_DISPLAY_REGION_LIST_ENTRY RegionListEntry;

    if (RegionList == NULL) {
        return;
    }

    ListHead = &RegionList->ListEntry;
    if (IsListEmpty(ListHead) != FALSE) {
        return;
    }

    CurrentEntry = ListHead->Flink;
    while (CurrentEntry != ListHead) {
        RegionListEntry = CONTAINING_RECORD(CurrentEntry,
                                            BG_DISPLAY_REGION_LIST_ENTRY,
                                            ListEntry);

        OslpBgDisplayDestroyRegion(RegionListEntry->Region,
                                   RegionListEntry->Flags);

        CurrentEntry = CurrentEntry->Flink;
        BlMmFreeHeap(RegionListEntry);
    }

    BlMmFreeHeap(RegionList);
    return;
}

NTSTATUS
OslpBgDisplayInitializeHeader (
    __deref_out PBG_DISPLAY_REGION *Region
    )

/*++

Routine Description:

    This routine initializes the header region, determining its offset, height
    and width, and foreground and background colors.

Arguments:

    Region - Supplies a pointer to a variable that receives an allocated
        display region for the header.

Return Value:

    NT status code.

--*/

{

    ULONG Height;
    OFFSET Offset;
    NTSTATUS Status;
    ULONG Width;

    //
    // Initialize the header region based on the display size.
    //

    Offset.X = OslpBgGetElementSize(HeaderMargin);
    Offset.Y = OslpBgGetElementSize(HeaderOffset);
    Width = OslpBgGetElementSize(HeaderWidth);
    Height = OslpBgGetElementSize(HeaderHeight);

    //
    // Make sure the width doesn't go off the edge of the screen.
    //

    OslpBgDisplayValidateRegionWidth(Offset.X, &Width);

    //
    // Determine the location of the header region based on the display size.
    //

    Status = OslpBgDisplayInitializeRegion(Region,
                                           Offset,
                                           Width,
                                           Height,
                                           0,
                                           0,
                                           BGFX_COLOR_WHITE,
                                           BGFX_COLOR_DARK_BLUE,
                                           BG_DISPLAY_STRING_FLAGS_NONE);

    return Status;
}

NTSTATUS
OslpBgDisplayInitializeInput (
    __deref_out PBG_DISPLAY_REGION *Region
    )

/*++

Routine Description:

    This routine initializes the region for the input control box.

Arguments:

    Region - Supplies a pointer to that will receives the address of the input
        control region.

Return Value:

    NT status code.

--*/

{

    ULONG Height;
    OFFSET Offset;
    ULONG Padding;
    NTSTATUS Status;
    ULONG Width;

    //
    // Initialize the input region based on the display size.
    //

    Offset.X = OslpBgGetElementSize(InputMargin);
    Offset.Y = OslpBgGetElementSize(InputOffset);
    Width = OslpBgGetElementSize(InputWidth);
    Height = OslpBgGetElementSize(InputHeight);
    Padding = OslpBgGetElementSize(InputPadding);

    //
    // Make sure the width doesn't go off the edge of the screen.
    //

    OslpBgDisplayValidateRegionWidth(Offset.X, &Width);

    //
    // Determine the location of the header region based on the display size.
    //

    Status = OslpBgDisplayInitializeRegion(Region,
                                           Offset,
                                           Width,
                                           Height,
                                           Padding,
                                           0,
                                           BGFX_COLOR_BLACK,
                                           BGFX_COLOR_DARK_WHITE,
                                           BG_DISPLAY_STRING_FLAGS_NONE);

    return Status;
}

NTSTATUS
OslpBgDisplayInitializeRegion (
    __deref_out PBG_DISPLAY_REGION *Region,
    __in OFFSET Offset,
    __in ULONG Width,
    __in ULONG Height,
    __in ULONG Padding,
    __in ULONG CursorIndex,
    __in ULONG Foreground,
    __in ULONG Background,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine allocates and initializes a display region based on the
    supplied parameters.

Arguments:

    Region - Supplies a pointer to a value that receives the address of the
        newly allocated region.

    Offset - Supplies the offset on the display where the new region should be
        drawn.

    Width - Supplies the width of the new region.

    Height - Supplies the height of the new region.

    Padding - Supplies the padding for the new region.

    CursorIndex - Supplies the cursor index into the string where a cursor
        should be drawn.

    ForegroundColor - Supplies the foreground color of the new region.

    BackgroundColor - Supplies the background color of the new region.

    Flags - Supplies flags for the new region.

Return Value:

--*/

{

    PBG_DISPLAY_REGION LocalRegion;

    LocalRegion = BlMmAllocateHeap(sizeof(BG_DISPLAY_REGION));
    if (LocalRegion == NULL) {
        return STATUS_NO_MEMORY;
    }

    LocalRegion->Handle = INVALID_HANDLE;
    LocalRegion->String = NULL;
    LocalRegion->Offset = Offset;
    LocalRegion->Width = Width;
    LocalRegion->Height = Height;
    LocalRegion->Padding = Padding;
    LocalRegion->CursorIndex = CursorIndex;
    LocalRegion->Foreground = Foreground;
    LocalRegion->Background = Background;
    LocalRegion->Flags = Flags;
    *Region = LocalRegion;
    return STATUS_SUCCESS;
}

NTSTATUS
OslpBgDisplayInitializeRegionList (
    __deref_out PBG_DISPLAY_REGION_LIST_ENTRY *RegionListHead
    )

/*++

Routine Description:

    This routine initializes a region list, setting it to empty.

Arguments:

    RegionListHead - Supplies a pointer that receives the address of the region
        list head.

Return Value:

    NT status code.

--*/

{

    PBG_DISPLAY_REGION_LIST_ENTRY ListEntry;
    ListEntry = BlMmAllocateHeap(sizeof(BG_DISPLAY_REGION_LIST_ENTRY));
    if (ListEntry == NULL) {
        return STATUS_NO_MEMORY;
    }

    InitializeListHead(&ListEntry->ListEntry);
    *RegionListHead = ListEntry;
    return STATUS_SUCCESS;
}

VOID
OslpBgDisplayRenderInputRegion (
    __in PBG_DISPLAY_REGION Region
    )

/*++

Routine Description:

    This routine renders an input region to the screen.

Arguments:

    Region - Supplies a pointer to the region to be rendered.

Return Value:

    None.

--*/

{

    NTSTATUS Status;

    if (Region == NULL) {
        return;
    }

    Status = BgDisplayStringEx(Region->String,
                               &Region->Offset,
                               &Region->Width,
                               &Region->Height,
                               &Region->Foreground,
                               &Region->Background,
                               NULL,
                               Region->Flags,
                               &Region->CursorIndex,
                               &Region->Handle,
                               NULL,
                               &Region->Padding);

    NT_ASSERT(Status == STATUS_SUCCESS);

    return;
}

VOID
OslpBgDisplayRenderRegion (
    __in PBG_DISPLAY_REGION Region
    )

/*++

Routine Description:

    This routine renders a region to the screen.

Arguments:

    Region - Supplies a pointer to the region to be rendered.

Return Value:

    None.

--*/

{

    PWCHAR Remainder;
    NTSTATUS Status;

    if (Region == NULL) {
        return;
    }

    Status = BgDisplayStringEx(Region->String,
                               &Region->Offset,
                               &Region->Width,
                               &Region->Height,
                               &Region->Foreground,
                               &Region->Background,
                               NULL,
                               Region->Flags,
                               NULL,
                               &Region->Handle,
                               &Remainder,
                               NULL);

    NT_ASSERT(Status == STATUS_SUCCESS);

    return;
}

VOID
OslpBgDisplayRenderRegionList (
    __in PBG_DISPLAY_REGION_LIST_ENTRY RegionList,
    __in PULONG RegionListCount
    )

/*++

Routine Description:

    This routine traverses a region list, rendering each region to the screen.

Arguments:

    RegionList - Supplies a pointer to the region list to display.

    RegionListCount - Supplies the number of elements in the region list
        supplied by RegionList.

Return Value:

    None.

--*/

{

    PLIST_ENTRY CurrentEntry;
    PLIST_ENTRY ListHead;
    PBG_DISPLAY_REGION Region;
    PBG_DISPLAY_REGION_LIST_ENTRY RegionListEntry;
    PWCHAR Remainder;
    NTSTATUS Status;

    if (RegionList == NULL) {
        return;
    }

    ListHead = &RegionList->ListEntry;
    if (IsListEmpty(ListHead) != FALSE) {
        return;
    }

    CurrentEntry = ListHead->Flink;
    while (CurrentEntry != ListHead) {
        RegionListEntry = CONTAINING_RECORD(CurrentEntry,
                                            BG_DISPLAY_REGION_LIST_ENTRY,
                                            ListEntry);

        Remainder = NULL;
        Region = RegionListEntry->Region;
        Status = BgDisplayStringEx(Region->String,
                                   &Region->Offset,
                                   &Region->Width,
                                   &Region->Height,
                                   &Region->Foreground,
                                   &Region->Background,
                                   NULL,
                                   Region->Flags,
                                   NULL,
                                   &Region->Handle,
                                   &Remainder,
                                   NULL);

        NT_ASSERT(Status == STATUS_SUCCESS);

        //
        // Handle wrap code here by creating, inserting and displaying a
        // new region with the remaining text. And then continue rendering to
        // the screen.
        //

        if (CHECK_FLAG(Region->Flags, BG_DISPLAY_STRING_FLAGS_WRAP) != FALSE) {
            OslpBgInsertWrapRegion(Remainder,
                                   RegionListEntry,
                                   RegionList,
                                   RegionListCount);
        }

        CurrentEntry = CurrentEntry->Flink;
    }

    return;
}

NTSTATUS
OslpBgDisplaySetInput (
    __in_z PWCHAR String,
    __in ULONG Flags,
    __in ULONG CursorIndex
    )

/*++

Routine Description:

    This routine sets the input string to the specified value. If the input
    region has not yet been created, it creates the region.

Arguments:

    String - Supplies a pointer to the string to be displayed in the input
        region.

    Flags - Supplies the flags to use when calling the BGFX library to
        display the string.

    CursorIndex - Supplies the index into the string where the cursor should be
        drawn.

Return Value:

--*/

{

    NTSTATUS Status;

    NT_ASSERT(OslDisplayState != NULL);

    if (OslDisplayState->Input == NULL) {
        Status = OslpBgDisplayInitializeInput(&OslDisplayState->Input);
        if (!NT_SUCCESS(Status)) {
            return Status;
        }
    }

    //
    // Free the old string.
    //
    // N.B. Some of the region strings are not heap allocated, but come from
    //      the resource file instead. This is OK because BlMmFreeHeap just
    //      ignores these allocations.
    //

    if (OslDisplayState->Input->String != NULL) {
        BlMmFreeHeap(OslDisplayState->Input->String);
    }

    //
    // Update the cached string and render the region.
    //

    OslDisplayState->Input->String = String;
    OslDisplayState->Input->Flags = Flags;
    OslDisplayState->Input->CursorIndex = CursorIndex;
    return STATUS_SUCCESS;;
}

VOID
OslpBgDisplayValidateRegionWidth (
    __in ULONG RegionOffset,
    __inout PULONG RegionWidth
    )

/*++

Routine Description:

    This routine validates the region width, making sure it does not extend
    beyond the end of the right margin's edge of the screen.

Arguments:

    RegionOffset - Supplies the horizontal offset of the region.

    RegionWidth - Supplies a pointer to the region width. This will be modified
        as necessary.

Return Value:

    None.

--*/

{

    ULONG Height;
    ULONG Margin;
    ULONG RightEdge;
    NTSTATUS Status;
    ULONG Width;

    Height = 0;
    Width = 0;
    Status = BlDisplayGetScreenResolution(&Width, &Height);

    NT_ASSERT(NT_SUCCESS(Status));

    Margin = OslpBgGetElementSize(RightMargin);
    RightEdge = Width - Margin;
    if ((RegionOffset + *RegionWidth) > RightEdge) {
        *RegionWidth = RightEdge - RegionOffset;
    }

    return;
}

ULONG
OslpBgGetElementSize (
    __in BG_DISPLAY_ELEMENT Element
    )

/*++

Routine Description:

    This routine returns the size, in pixels, for the given display element. It
    calculates the pixel size based on the height and width of the currently
    display. Details are at the beginning of this file.

Arguments:

    Element - Supplies the display element whose size to retrieve.

Return Value:

    The size, in pixels, of the provided display element.

--*/

{

    ULONG ElementSize;
    ULONG Height;
    NTSTATUS Status;
    ULONG Width;
    ULONG ZValue;

    Height = 0;
    Width = 0;
    Status = BlDisplayGetScreenResolution(&Width, &Height);

    NT_ASSERT(NT_SUCCESS(Status));

    ZValue = MIN(Height, Width);

    switch (Element) {
    case HeaderMargin:
        ElementSize = (ZValue * HEADER_MARGIN) / TOTAL_SIZE;
        break;

    case HeaderOffset:
        ElementSize = (ZValue * HEADER_OFFSET) / TOTAL_SIZE;
        break;

    case HeaderHeight:
        ElementSize = (ZValue * HEADER_HEIGHT) / TOTAL_SIZE;
        break;

    case HeaderWidth:
        ElementSize = (ZValue * HEADER_WIDTH) / TOTAL_SIZE;
        break;

    case TitleMargin:
        ElementSize = (ZValue * TITLE_MARGIN) / TOTAL_SIZE;
        break;

    case TitleOffset:
        ElementSize = (ZValue * TITLE_OFFSET) / TOTAL_SIZE;
        break;

    case TitleHeight:
        ElementSize = (ZValue * TITLE_HEIGHT) / TOTAL_SIZE;
        break;

    case TitleWidth:
        ElementSize = (ZValue * TITLE_WIDTH) / TOTAL_SIZE;
        break;

    case InputMargin:
        ElementSize = (ZValue * INPUT_MARGIN) / TOTAL_SIZE;
        break;

    case InputOffset:
        ElementSize = (ZValue * INPUT_OFFSET) / TOTAL_SIZE;
        break;

    case InputHeight:
        ElementSize = (ZValue * INPUT_HEIGHT) / TOTAL_SIZE;
        break;

    case InputWidth:
        ElementSize = (ZValue * INPUT_WIDTH) / TOTAL_SIZE;
        break;

    case InputPadding:
        ElementSize = (ZValue * INPUT_PADDING) / TOTAL_SIZE;
        if (ElementSize < INPUT_PADDING_MINIMUM) {
            ElementSize = INPUT_PADDING_MINIMUM;
        }

        break;

    case BodyMargin:
        ElementSize = (ZValue * BODY_MARGIN) / TOTAL_SIZE;
        break;

    case BodyOffsetInput:
        ElementSize = (ZValue * BODY_OFFSET_INPUT) / TOTAL_SIZE;
        break;

    case BodyOffsetNoInput:
        ElementSize = (ZValue * BODY_OFFSET_NO_INPUT) / TOTAL_SIZE;
        break;

    case BodyHeightSmall:
        ElementSize = (ZValue * BODY_HEIGHT_SMALL) / TOTAL_SIZE;
        break;

    case BodyHeightLarge:
        ElementSize = (ZValue * BODY_HEIGHT_LARGE) / TOTAL_SIZE;
        break;

    case BodyWidth:
        ElementSize = (ZValue * BODY_WIDTH) / TOTAL_SIZE;
        break;

    case FooterMargin:
        ElementSize = (ZValue * FOOTER_MARGIN) / TOTAL_SIZE;
        break;

    case FooterOffset:
        ElementSize = (ZValue * FOOTER_OFFSET) / TOTAL_SIZE;
        break;

    case FooterHeight:
        ElementSize = (ZValue * FOOTER_HEIGHT) / TOTAL_SIZE;
        break;

    case FooterWidth:
        ElementSize = (ZValue * FOOTER_WIDTH) / TOTAL_SIZE;
        break;

    case RightMargin:
        ElementSize = (Width * RIGHT_MARGIN) / TOTAL_SIZE;
        break;

    default:

        NT_ASSERT(FALSE);

        ElementSize = 0;
        break;
    }

    return ElementSize;
}

VOID
OslpBgInsertWrapRegion (
    __in PWCHAR String,
    __in PBG_DISPLAY_REGION_LIST_ENTRY RegionListEntry,
    __in PBG_DISPLAY_REGION_LIST_ENTRY RegionListHead,
    __in PULONG RegionListCount
    )

/*++

Routine Description:

    This routine creates and inserts a new region into the region list after
    the provided region list entry. It also pushes the text of the next region
    further down on the screen.

Arguments:

    String - Supplies the string to be displayed for the new region.

    RegionListEntry - Supplies a pointer to the region list entry that the new
        region should be inserted after.

    RegionListHead - Supplies a pointer to the head of the region list the
        entry is being inserted into.

    RegionListCount - Supplies the number of elements in the region list
        supplied by RegionListHead.

Return Value:

    None.

--*/

{

    PBG_DISPLAY_REGION CurrentRegion;
    OFFSET Offset;
    PLIST_ENTRY InsertEntry;
    PBG_DISPLAY_REGION_LIST_ENTRY InsertListEntry;
    PBG_DISPLAY_REGION InsertRegion;
    PLIST_ENTRY NextEntry;
    PBG_DISPLAY_REGION_LIST_ENTRY NextListEntry;
    PBG_DISPLAY_REGION NextRegion;
    NTSTATUS Status;

    //
    // Do not create a child wrap region if the string is null or if the
    // provided region already wrapped before and is being displayed again.
    //

    if ((String == NULL) ||
        (CHECK_FLAG(RegionListEntry->Flags, REGION_LIST_FLAGS_WRAP_PARENT))) {

        return;
    }

    //
    // Create the region and insert it after the supplied region list item.
    //

    CurrentRegion = RegionListEntry->Region;
    Offset.X = CurrentRegion->Offset.X;
    Offset.Y = CurrentRegion->Offset.Y + CurrentRegion->Height;
    Status = OslpBgDisplayAddRegionListItem(RegionListEntry,
                                            RegionListCount,
                                            TRUE,
                                            &InsertRegion,
                                            Offset,
                                            CurrentRegion->Width,
                                            CurrentRegion->Height,
                                            CurrentRegion->Foreground,
                                            CurrentRegion->Background,
                                            BG_DISPLAY_STRING_FLAGS_WRAP);


    if (!NT_SUCCESS(Status)) {
        goto InsertWrapRegionEnd;
    }

    InsertRegion->String = String;

    //
    // Mark this as a child region so it does not free its string. The parent
    // will free the string.
    //

    InsertEntry = RegionListEntry->ListEntry.Flink;
    InsertListEntry = CONTAINING_RECORD(InsertEntry,
                                        BG_DISPLAY_REGION_LIST_ENTRY,
                                        ListEntry);

    InsertListEntry->Flags |= REGION_LIST_FLAGS_WRAP_CHILD;

    //
    // Move the offsets of the remaining items down by the height of the newly
    // created region.
    //

    NextEntry = InsertListEntry->ListEntry.Flink;
    while (NextEntry != &RegionListHead->ListEntry) {
        NextListEntry = CONTAINING_RECORD(NextEntry,
                                          BG_DISPLAY_REGION_LIST_ENTRY,
                                          ListEntry);

        NextRegion = NextListEntry->Region;
        NextRegion->Offset.Y += InsertRegion->Height;
        NextEntry = NextListEntry->ListEntry.Flink;
    }

    //
    // Now that the new entry has been inserted after the provided entry, mark
    // the provided entry as one that wrapped.
    //

    RegionListEntry->Flags |= REGION_LIST_FLAGS_WRAP_PARENT;

InsertWrapRegionEnd:
    return;
}

VOID
OslpBgUpdateInputBodyOffset (
    VOID
    )

/*++

Routine Description:

    This routine updates the offset for the input and body regions according to
    the number of lines in the title.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PLIST_ENTRY CurrentEntry;
    PBG_DISPLAY_REGION InputRegion;
    PLIST_ENTRY ListHead;
    ULONG LastRegionOffset;
    ULONG OffsetIncrease;
    PBG_DISPLAY_REGION Region;
    PBG_DISPLAY_REGION_LIST_ENTRY RegionListEntry;

    //
    // If the current title and the previous title have different number of
    // lines, then the input Y offset and all the body Y offsets need to be
    // updated accordingly, either increased or decreased by the total height
    // of the extra title lines. The default settings only factor in 1 line
    // for the title text as the value of PreviousTitleListCount is
    // initialized to 1.
    //

    OffsetIncrease = OslpBgGetElementSize(TitleHeight);
    OffsetIncrease *= (OslDisplayState->TitleListCount - 1);

    //
    // Update the input region starting offset if the region exists.
    //

    InputRegion = OslDisplayState->Input;
    if (InputRegion != NULL) {
        InputRegion->Offset.Y = OslpBgGetElementSize(InputOffset);
        InputRegion->Offset.Y += OffsetIncrease;
    }

    //
    // Update each of the body region elements.
    //

    if (OslDisplayState->BodyList == NULL) {
        return;
    }

    ListHead = &OslDisplayState->BodyList->ListEntry;
    if (IsListEmpty(ListHead) != FALSE) {
        return;
    }

    CurrentEntry = ListHead->Flink;
    LastRegionOffset = 0;
    while (CurrentEntry != ListHead) {
        RegionListEntry = CONTAINING_RECORD(CurrentEntry,
                                            BG_DISPLAY_REGION_LIST_ENTRY,
                                            ListEntry);

        Region = RegionListEntry->Region;
        if (LastRegionOffset == 0) {
            if (InputRegion == NULL) {
                Region->Offset.Y = OffsetIncrease;
                Region->Offset.Y += OslpBgGetElementSize(BodyOffsetNoInput);

            } else {
                Region->Offset.Y = OffsetIncrease;
                Region->Offset.Y += OslpBgGetElementSize(BodyOffsetInput);
            }

        } else {
            Region->Offset.Y = LastRegionOffset;
        }

        LastRegionOffset = Region->Offset.Y + Region->Height;
        CurrentEntry = CurrentEntry->Flink;
    }

    return;
}


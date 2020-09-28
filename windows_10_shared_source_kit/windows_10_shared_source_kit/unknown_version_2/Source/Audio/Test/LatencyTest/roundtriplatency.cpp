    Parameters->MaxCylinders = DiskInformation->Cylinders - 1;
        Parameters->SectorsPerTrack = (USHORT)DiskInformation->SectorsPerTrack;
        Parameters->MaxHeads = (USHORT)(DiskInformation->Heads - 1);
        Parameters->NumberDrives = (USHORT)DiskCount;
        Parameters += 1;
    }

    return;
}

NTSTATUS
OslpDeleteControllerNode (
    __inout PCONFIGURATION_COMPONENT_DATA Node
    )

/*++

Routine Description:

    Utility routine to deallocate a config tree node.

Arguments:

    Node - pointer to the node to deallocate.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if an allocation fails.

--*/

{

    PCONFIGURATION_COMPONENT Component;

    Component = &Node->ComponentEntry;
    if (Node != NULL) {
        if (Component->Identifier != NULL) {
            BlMmFreeHeap(Component->Identifier);
        }

        if (Node->ConfigurationData != NULL) {
            BlMmFreeHeap(Node->ConfigurationData);
        }

        BlMmFreeHeap(Node);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
OslpCleanupControllerNode (
    __inout PCONFIGURATION_COMPONENT_DATA ConfigurationTreeEntry
    )

/*++

Routine Description:

    Utility routine to deallocate a configuration tree.  It walks the
    entire configuration tree and deallocates all nodes.

Arguments:

    ConfigurationTreeEntry - pointer to root of tree to delete.

Return Value:

    STATUS_SUCCESS when successful.

--*/

{

    PCONFIGURATION_COMPONENT_DATA CurrentEntry;
    PCONFIGURATION_COMPONENT_DATA ParentEntry;
    PCONFIGURATION_COMPONENT_DATA SiblingEntry;

    CurrentEntry = ConfigurationTreeEntry;
    while (CurrentEntry != NULL) {

        //
        // Keep traversing down if you see a child.
        //

        if (CurrentEntry->Child) {
            ParentEntry = CurrentEntry;
            CurrentEntry = CurrentEntry->Child;
            ParentEntry->Child = NULL;

        } else if (CurrentEntry->Sibling) {

            //
            // There are no children for this node.  It can be deleted,
            // but the sibling must be saved off first so it can be
            // traversed.
            //

            SiblingEntry = CurrentEntry->Sibling;

            ASSERT( SiblingEntry->Parent == CurrentEntry->Parent);

            OslpDeleteControllerNode(CurrentEntry);
            CurrentEntry = SiblingEntry;

        } else {

            //
            // This entry has neither children nor siblings.
            // It can be deleted, but we must save off the parent
            // pointer.
            //

            ParentEntry = CurrentEntry->Parent;

            ASSERT((ParentEntry == NULL) || (ParentEntry->Child == NULL));

            OslpDeleteControllerNode(CurrentEntry);
            CurrentEntry = ParentEntry;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
OslpInitializeControllerNode (
    __in CONFIGURATION_CLASS Class,
    __in CONFIGURATION_TYPE Type,
    __in ULONG AffinityMask,
    __in_bcount_opt(IdentifierLength) PCHAR IdentifierString,
    __in size_t IdentifierLength,
    __deref_out PCONFIGURATION_COMPONENT_DATA *ControllerNode
    )

/*++

Routine Description:

    Utility routine to allocate and initialize a configuration tree entry.

Arguments:

    Class - specifies the class for the controller node member data.
    Type - specifies the type for the controller node member data.
    AffinityMask - specifies the Affinity for the controller node member data.
    IdentifierString - if specified, specifies the identifier member data.
    IdentifierLength - specifies the length in bytes of IdentifierString.
    ControllerNode - On output, contains a pointer to a newly allocated
        configuration tree entry.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if an allocation fails.

--*/

{

    PCONFIGURATION_COMPONENT Component;
    PCONFIGURATION_COMPONENT_DATA ConfigurationTreeEntry;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;
    *ControllerNode = NULL;
    ConfigurationTreeEntry = BlMmAllocateHeap(sizeof(CONFIGURATION_COMPONENT_DATA));
    if (ConfigurationTreeEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto InitializeControllerNodeEnd;
    }

    //
    //  Initialize the configuration node.
    //

    RtlZeroMemory(ConfigurationTreeEntry, sizeof(CONFIGURATION_COMPONENT_DATA));
    Component = &ConfigurationTreeEntry->ComponentEntry;
    Component->Class = Class;
    Component->Type = Type;
    Component->AffinityMask = AffinityMask;
    Component->IdentifierLength = (ULONG)IdentifierLength;
    if (Component->IdentifierLength) {
        Component->Identifier = BlMmAllocateHeap(IdentifierLength);
        if (Component->Identifier == NULL) {
            Status = STATUS_NO_MEMORY;
            goto InitializeControllerNodeEnd;
        }

        strcpy_s(Component->Identifier, IdentifierLength, IdentifierString);
    }

    *ControllerNode = ConfigurationTreeEntry;

InitializeControllerNodeEnd:

    if (!NT_SUCCESS(Status)) {
        OslpCleanupControllerNode(ConfigurationTreeEntry);
    }

    return Status;
}

#include <windows.h>
#include <ntverp.h>
#include "..\include\FrameworkResource.h"

#define VER_FILETYPE                VFT_APP
#define VER_FILESUBTYPE             VFT_UNKNOWN
#define VER_FILEDESCRIPTION_STR     "Direct3D conformance test"
#define VER_INTERNALNAME_STR        "multisample"
#define VER_ORIGINALFILENAME_STR    "Multisample.exe"

/////////////////////////////////////////////////////////////////////////////
//
// Menu
//

IDR_MENU MENU 
BEGIN
    POPUP "File"
    BEGIN
        MENUITEM "Close",                       IDM_CLOSE
    END
    POPUP "Options"
    BEGIN
        MENUITEM "Image Compare",               ID_OPTIONS_IMAGECOMPARE
    EN
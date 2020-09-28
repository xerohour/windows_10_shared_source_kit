/*****************************************************************************
 * resource.cpp - resource list object implementation
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 */

#include "private.h"
#include <ntintsafe.h>




/*****************************************************************************
 * Factory.
 */

#pragma code_seg("PAGE")

/*****************************************************************************
 * CreateResourceList()
 *****************************************************************************
 * Creates a resource list object.
 */
NTSTATUS
CreateResourceList
(
    OUT     PUNKNOWN *  Unknown,
    IN      REFCLSID,
    IN      PUNKNOWN    UnknownOuter    OPTIONAL,
    IN      POOL_TYPE   PoolType
)
{
    PAGED_CODE();

    ASSERT(Unknown);

    _DbgPrintF(DEBUGLVL_LIFETIME,("Creating RESOURCELIST"));

    STD_CREATE_BODY
    (
        CResourceList,
        Unknown,
        UnknownOuter,
        PoolType
    );
}

/*****************************************************************************
 * PcNewResourceList()
 *****************************************************************************
 * Creates and initializes a resource list.
 * Creates an empty resource list if both of the PCM_RESOURCE_LIST parameters
 * are NULL.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcNewResourceList
(
    __out       PRESOURCELIST *     OutResourceList,
    __in_opt    PUNKNOWN            OuterUnknown,
    __in        POOL_TYPE           PoolType,
    __in        PCM_RESOURCE_LIST   TranslatedResources,
    __in        PCM_RESOURCE_LIST   UntranslatedResources
)
{
    PAGED_CODE();

    ASSERT(OutResourceList);

    //
    // Validate Parameters.
    //
    if (NULL == OutResourceList)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcNewResourceList : Invalid Parameter"));        
        return STATUS_INVALID_PARAMETER;
    }

    PUNKNOWN    unknown;
    NTSTATUS    ntStatus =
        CreateResourceList
        (
            &unknown,
            GUID_NULL,
            OuterUnknown,
            PoolType
        );

    if (NT_SUCCESS(ntStatus))
    {
        PRESOURCELISTINIT resourceList;
        ntStatus =
            unknown->QueryInterface
            (
                IID_IResourceList,
                (PVOID *) &resourceList
            );

        if (NT_SUCCESS(ntStatus))
        {
            ntStatus =
                resourceList->Init
                (
                    TranslatedResources,
                    UntranslatedResources,
                    PoolType
                );

            if (NT_SUCCESS(ntStatus))
            {
                *OutResourceList = resourceList;
            }
            else
            {
                resourceList->Release();
            }
        }

        unknown->Release();
    }

    return ntStatus;
}

/*****************************************************************************
 * PcNewResourceSublist()
 *****************************************************************************
 * Creates and initializes an empty resource list derived from another
 * resource list.
 */
PORTCLASSAPI
NTSTATUS
NTAPI
PcNewResourceSublist
(
    __out       PRESOURCELIST *     OutResourceList,
    __in_opt    PUNKNOWN            OuterUnknown,
    __in        POOL_TYPE           PoolType,
    __in        PRESOURCELIST       ParentList,
    __in        ULONG               MaximumEntries
)
{
    PAGED_CODE();

    ASSERT(OutResourceList);
    ASSERT(ParentList);
    ASSERT(MaximumEntries);

    //
    // Validate Parameters.
    //
    if (NULL == OutResourceList ||
        NULL == ParentList ||
        0    == MaximumEntries)
    {
        _DbgPrintF(DEBUGLVL_TERSE, ("PcNewResourceSubList : Invalid Parameter"));
        return STATUS_INVALID_PARAMETER;
    }

    PUNKNOWN    unknown;
    NTSTATUS    ntStatus =
        CreateResourceList
        (
            &unknown,
            GUID_NULL,
            OuterUnknown,
            PoolType
        );

    if (NT_SUCCESS(ntStatus))
    {
        PRESOURCELISTINIT resourceList;
        ntStatus =
            unknown->QueryInterface
            (
                IID_IResourceList,
                (PVOID *) &resourceList
            );

        if (NT_SUCCESS(ntStatus))
        {
            ntStatus =
                resourceList->InitFromParent
                (
                    ParentList,
                    MaximumEntries,
                    PoolType
                );

            if (NT_SUCCESS(ntStatus))
            {
                *OutResourceList = resourceList;
            }
            else
            {
                resourceList->Release();
            }
        }

        unknown->Release();
    }

    return ntStatus;
}





/*****************************************************************************
 * Member functions.
 */

/*****************************************************************************
 * CResourceList::~CResourceList()
 *****************************************************************************
 * Destructor.
 */
CResourceList::
~CResourceList
(   void
)
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_LIFETIME,("Destroying RESOURCELIST (0x%08x)",this));

    if (Translated)
    {
        ExFreePool(Translated);
    }

    if (Untranslated)
    {
        ExFreePool(Untranslated);
    }
}

/*****************************************************************************
 * CResourceList::NonDelegatingQueryInterface()
 *****************************************************************************
 * Obtains an interface.
 */
STDMETHODIMP_(NTSTATUS)
CResourceList::
NonDelegatingQueryInterface
(
    __in    REFIID  Interface,
    __out   PVOID * Object
)
{
    PAGED_CODE();

    ASSERT(Object);

    if (IsEqualGUIDAligned(Interface,IID_IUnknown))
    {
        *Object = PVOID(PUNKNOWN(this));
    }
    else
    if (IsEqualGUIDAligned(Interface,IID_IResourceList))
    {
        *Object = PVOID(PRESOURCELISTINIT(this));
    }
    else
    {
        *Object = NULL;
    }

    if (*Object)
    {
        PUNKNOWN(*Object)->AddRef();
        return STATUS_SUCCESS;
    }

    return STATUS_INVALID_PARAMETER;
}


/*****************************************************************************
 * CResourceList::Init()
 *****************************************************************************
 * Initializes the object.
 */
STDMETHODIMP_(NTSTATUS)
CResourceList::
Init
(
    IN      PCM_RESOURCE_LIST   TranslatedResources,
    IN      PCM_RESOURCE_LIST   UntranslatedResources,
    IN      POOL_TYPE           PoolType
)
{
    PAGED_CODE();

    _DbgPrintF(DEBUGLVL_LIFETIME,("Initializing RESOURCELIST (0x%08x)",this));
    
    // check NULL resource lists.
    if (!TranslatedResources && !UntranslatedResources)
    {
      EntriesAllocated = EntriesInUse = 0;
      Translated = Untranslated = NULL;
      return STATUS_SUCCESS;
    }
    
    // this check fails if _one_ of the resource lists is NULL, which should
    // never happen.
    ASSERT (TranslatedResources);
    ASSERT (UntranslatedResources);
    if (!TranslatedResources || !UntranslatedResources)
    {
        return STATUS_INVALID_PARAMETER;
    }

    EntriesAllocated =
        EntriesInUse =
            UntranslatedResources->List[0].PartialResourceList.Count;

    ULONG listSize =
        (   sizeof(CM_RESOURCE_LIST)
        +   (   (EntriesInUse - 1)
            *   sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)
            )
        );

    NTSTATUS ntStatus = STATUS_SUCCESS;

    Translated = PCM_RESOURCE_LIST(ExAllocatePoolWithTag(PoolType,listSize,'lRcP'));
    if (Translated)
    {
        Untranslated =
            PCM_RESOURCE_LIST(ExAllocatePoolWithTag(PoolType,listSize,'lRcP'));
        if (Untranslated)
        {
            RtlCopyMemory(Untranslated,UntranslatedResources,listSize);
            RtlCopyMemory(Translated,TranslatedResources,listSize);
        }
        else
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            ExFreePool(Translated);
            Translated = NULL;
        }
    }
    else
    {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    return ntStatus;
}

/*****************************************************************************
 * CResourceList::InitFromParent()
 *****************************************************************************
 * Initializes the object from a parent object.
 */
STDMETHODIMP_(NTSTATUS)
CResourceList::
InitFromParent
(
    IN      PRESOURCELIST       ParentList,
    IN      ULONG               MaximumEntries,
    IN      POOL_TYPE           PoolType
)
{
    PAGED_CODE();

    ASSERT(ParentList);
    ASSERT(MaximumEntries);

    ULONG listSize = 0;

    NTSTATUS ntStatus = STATUS_SUCCESS;

    if  ((0 == MaximumEntries) || 
        !NT_SUCCESS(RtlULongMult(MaximumEntries - 1, sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR), &listSize)) ||
        !NT_SUCCESS(RtlULongAdd(listSize, sizeof(CM_RESOURCE_LIST), &listSize)))
    {
        Translated = NULL;  //  Force failure on any overflows
    }
    else
    {
        Translated = PCM_RESOURCE_LIST(ExAllocatePoolWithTag(PoolType,listSize,'lRcP'));
    }

    if (Translated)
    {
        Untranslated = PCM_RESOURCE_LIST(ExAllocatePoolWithTag(PoolType,listSize,'lRcP'));
        if (Untranslated)
        {
            EntriesAllocated    = MaximumEntries;
            EntriesInUse        = 0;

            // Copy headers from the parent.
            if( NULL != ParentList->TranslatedList() )
            {
                RtlZeroMemory(Translated,listSize);

                RtlCopyMemory
                (
                    Translated,
                    ParentList->TranslatedList(),
                    listSize < (sizeof(CM_RESOURCE_LIST) - sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)) ? listSize : (sizeof(CM_RESOURCE_LIST) - sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR))
                );

                Translated->List[0].PartialResourceList.Count = EntriesInUse;
            } else
            {
                ExFreePool(Translated);
                Translated = NULL;
            }

            if( NULL != ParentList->UntranslatedList() )
            {
                RtlZeroMemory(Untranslated,listSize);

                RtlCopyMemory
                (
                    Untranslated,
                    ParentList->UntranslatedList(),
                    listSize < (sizeof(CM_RESOURCE_LIST) - sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR)) ? listSize : (sizeof(CM_RESOURCE_LIST) - sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR))
                );

                Untranslated->List[0].PartialResourceList.Count = EntriesInUse;
            } else
            {
                ExFreePool(Untranslated);
                Untranslated = NULL;
            }
        }
        else
        {
            ntStatus = STATUS_INSUFFICIENT_RESOURCES;
            ExFreePool(Translated);
            Translated = NULL;
        }
    }
    else
    {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    return ntStatus;
}

/*****************************************************************************
 * CResourceList::NumberOfEntries()
 *****************************************************************************
 * Determine the number of entries in the list.
 */
STDMETHODIMP_(ULONG)
CResourceList::
NumberOfEntries
(   void
)
{
    PAGED_CODE();

    return EntriesInUse;
}

/*****************************************************************************
 * CResourceList::NumberOfEntriesOfType()
 *****************************************************************************
 * Determines the number of entries of a given type in the list.
 */
STDMETHODIMP_(ULONG)
CResourceList::
NumberOfEntriesOfType
(
    IN      CM_RESOURCE_TYPE    Type
)
{
    PAGED_CODE();

    if (!Untranslated) {
       return 0;
    }

    PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor =
        Untranslated->List[0].PartialResourceList.PartialDescriptors;

    ULONG entriesOfType = 0;

    for
    (   ULONG count = EntriesInUse;
        count--;
        descriptor++
    )
    {
        if (descriptor->Type == Type)
        {
            entriesOfType++;
        }
    }

    return entriesOfType;
}

/*****************************************************************************
 * CResourceList::FindTranslatedEntry()
 *****************************************************************************
 * Finds a translated entry.
 */
STDMETHODIMP_(PCM_PARTIAL_RESOURCE_DESCRIPTOR)
CResourceList::
FindTranslatedEntry
(
    IN      CM_RESOURCE_TYPE    Type,
    IN      ULONG               Index
)
{
    PAGED_CODE();

    if (!Translated) {
       return 0;
    }

    PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor =
        Translated->List[0].PartialResourceList.PartialDescriptors;

    ULONG count = EntriesInUse;

    if (count)
    {
        while (descriptor)
        {
            if (count-- == 0)
            {
                descriptor = NULL;
            }
            else
            {
                if (descriptor->Type == Type)
                {
                    if (Index-- == 0)
                    {
                        break;
                    }
                }

                descriptor++;
            }
        }
    }

    return descriptor;
}

/*****************************************************************************
 * CResourceList::FindUntranslatedEntry()
 *****************************************************************************
 * Finds an untranslated entry.
 */
STDMETHODIMP_(PCM_PARTIAL_RESOURCE_DESCRIPTOR)
CResourceList::
FindUntranslatedEntry
(
    IN      CM_RESOURCE_TYPE    Type,
    IN      ULONG               Index
)
{
    PAGED_CODE();

    if (!Untranslated) {
       return 0;
    }

    PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor =
        Untranslated->List[0].PartialResourceList.PartialDescriptors;

    ULONG count = EntriesInUse;

    if (count)
    {
        while (descriptor)
        {
            if (count-- == 0)
            {
                descriptor = NULL;
            }
            else
            {
                if (descriptor->Type == Type)
                {
                    if (Index-- == 0)
                    {
                        break;
                    }
                }

                descriptor++;
            }
        }
    }

    return descriptor;
}

/*****************************************************************************
 * CResourceList::AddEntry()
 *****************************************************************************
 * Adds an entry.
 */
STDMETHODIMP_(NTSTATUS)
CResourceList::
AddEntry
(
    IN      PCM_PARTIAL_RESOURCE_DESCRIPTOR TranslatedDescr,
    IN      PCM_PARTIAL_RESOURCE_DESCRIPTOR UntranslatedDescr
)
{
    PAGED_CODE();

    ASSERT(TranslatedDescr);
    ASSERT(UntranslatedDescr);

    NTSTATUS ntStatus = STATUS_SUCCESS;

    // when there is no resource list stored in this object, both EntriesInUse
    // and EntriesAllocated are 0.
    if (EntriesInUse < EntriesAllocated)
    {
        Translated->
            List[0].PartialResourceList.PartialDescriptors[EntriesInUse] =
                *TranslatedDescr;

        Untranslated->
            List[0].PartialResourceList.PartialDescriptors[EntriesInUse] =
                *UntranslatedDescr;

        EntriesInUse++;

        // update counts
        Translated->
            List[0].PartialResourceList.Count = EntriesInUse;
        Untranslated->
            List[0].PartialResourceList.Count = EntriesInUse;
    }
    else
    {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
    }

    return ntStatus;
}

/*****************************************************************************
 * CResourceList::AddEntryFromParent()
 *****************************************************************************
 * Adds an entry from a parent.
 */
STDMETHODIMP_(NTSTATUS)
CResourceList::
AddEntryFromParent
(
    IN      PRESOURCELIST       Parent,
    IN      CM_RESOURCE_TYPE    Type,
    IN      ULONG               Index
)
{
    PAGED_CODE();

    ASSERT(Parent);

    PCM_PARTIAL_RESOURCE_DESCRIPTOR translated =
        Parent->FindTranslatedEntry(Type,Index);
    PCM_PARTIAL_RESOURCE_DESCRIPTOR untranslated =
        Parent->FindUntranslatedEntry(Type,Index);

    NTSTATUS ntStatus;

    if (translated && untranslated)
    {
        ntStatus = AddEntry(translated,untranslated);
    }
    else
    {
        ntStatus = STATUS_INVALID_PARAMETER;
    }

    return ntStatus;
}

/*****************************************************************************
 * CResourceList::TranslatedList()
 *****************************************************************************
 * Gets the list of translated resources.
 */
STDMETHODIMP_(PCM_RESOURCE_LIST)
CResourceList::
TranslatedList
(   void
)
{
    PAGED_CODE();

    return Translated;  // Attention: This could be NULL.
}                          

/*****************************************************************************
 * CResourceList::UntranslatedList()
 *****************************************************************************
 * Gets the list of untranslated resources.
 */
STDMETHODIMP_(PCM_RESOURCE_LIST)
CResourceList::
UntranslatedList
(   void
)
{
    PAGED_CODE();

    return Untranslated;   // Attention: This could be NULL.
}

#pragma code_seg()

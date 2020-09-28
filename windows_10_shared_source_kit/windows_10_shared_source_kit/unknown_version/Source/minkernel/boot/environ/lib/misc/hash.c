/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    hash.c

Abstract:

    Implementation of a generic hash table.

    The hash table structure is quite simple.  It is an array of HASH_ENTRYs.
    Associated with the hash table is a function pointer for a hash function,
    which maps an arbitrary key to an index in the hash table.  To account
    for collisions, where multiple keys map to the same hash table index,
    the hash table is actually an array of linked lists.  Each linked list
    contains a list of entries which mapped to the same index.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include <bootlibp.h>

// ------------------------------------------------------------ Data Structures

typedef struct _HASH_ENTRY {
    LIST_ENTRY ListEntry;
    HASH_KEY Key;
    HASH_DATA Data;
} HASH_ENTRY, *PHASH_ENTRY;

/*++

Hash Entry Description :

    The internal data structure for an entry in a hash table.

Fields:

    Size - The size of the Data field.

    Key - Lookup key for the hashed data object.

    Data - Data object associated with Key.

--*/

typedef struct _HASH_TABLE {
    __field_ecount(Size) PLIST_ENTRY Array;
    ULONG Size;
    PHASH_FUNCTION HashFunction;
    PCOMPARE_FUNCTION CompareFunction;
} HASH_TABLE, *PHASH_TABLE;

/*++

Hash Table Description :

    The internal data structure for a hash table.

Fields:

    Array - A hash table is an array of linked lists, where each list contains
        entries that mapped to a particular bucket (index) in the hash table.
        This field is the pointer to the linked list array.

    Size - The number of buckets (indices) in Array.

    HashFunction - Function which performs the hashing algorithm, mapping
        an arbitrary key to a bucket (index) in the hash table.

--*/

// -------------------------------------------------------------------- Globals

ULONG HtTableEntries = 0;
ULONG HtTableSize  = 0;
PHASH_TABLE *HtTableArray = NULL;

#define INITIAL_HASH_TABLE_SIZE (4)
#define DEFAULT_HASH_TABLE_BUCKETS (13)

// ----------------------------------------------- Internal Function Prototypes

BOOLEAN
HtpCompareKeys (
    __in PHASH_KEY Key1,
    __in PHASH_KEY Key2
    );

// ------------------------------------------------------------------ Functions

NTSTATUS
BlHtCreate (
    __in_opt ULONG HashTableSize,
    __in_opt PHASH_FUNCTION HashFunction,
    __in_opt PCOMPARE_FUNCTION CompareFunction,
    __out PHASH_TABLE_ID HashTableId
    )

/*++

Routine Description:

    Creates a hash table based on the caller's specification.  This routine
    will allocate a hash table and on successful return, can be referenced
    by the value pointed to by HashTableId.

Arguments:

    HashTableSize - Supplies the size of the hash table to create.

    HashFunction - Supplies an optional hash function to use when mapping
        keys into the hash table.  If NULL, a default algorithm will be
        utilized.

    CompareFunction - Supplies an optional compare key function to use when
        comparing the keys of two hash table entries. If NULL, a default
        algorithm will be utilized.

    HashTableId - Supplies a pointer to a variable that receives the identifier
        for the created hash table.

Return Value:

    STATUS_SUCCESS when successful. (HashTableId contains the identifier to
        the created hash table).

    STATUS_INVALID_PARAMETER if HashTableId is not a valid pointer.

    STATUS_NO_MEMORY if memory can not be allocated for hash table.

--*/

{

    PHASH_TABLE HashTable;
    ULONG Id;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceHt);
    HashTable = NULL;

    //
    // Parameter checking.
    //

    if (HashTableId == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto HtCreateDone;
    }

    //
    // Allocate the global hash table array if it is currently not allocated.
    //

    if (HtTableSize == 0) {
        HtTableSize = INITIAL_HASH_TABLE_SIZE;
        HtTableArray = BlMmAllocateHeap(HtTableSize * sizeof(PHASH_TABLE));
        if (HtTableArray == NULL) {
            Status = STATUS_NO_MEMORY;
            goto HtCreateDone;
        }

        RtlZeroMemory(HtTableArray, HtTableSize * sizeof(PHASH_TABLE));
        HtTableEntries = 0;
    }

    //
    // Allocate memory for a hash table structure.
    //

    HashTable = BlMmAllocateHeap(sizeof(HASH_TABLE));
    if (HashTable == NULL) {
        Status = STATUS_NO_MEMORY;
        goto HtCreateDone;
    }

    //
    // Initialize the hash table data structure.  The caller can optionally
    // specify either a hash function or hash table size.  Use default
    // values when an option is not present.
    //

    if (HashFunction != NULL) {
        HashTable->HashFunction = HashFunction;

    } else {
        HashTable->HashFunction = DefaultHashFunction;
    }

    if (CompareFunction != NULL) {
        HashTable->CompareFunction = CompareFunction;

    } else {
        HashTable->CompareFunction = HtpCompareKeys;
    }

    if (HashTableSize != 0) {
        HashTable->Size = HashTableSize;

    } else {
        HashTable->Size = DEFAULT_HASH_TABLE_BUCKETS;
    }

    //
    // Allocate and initialize the hash table to the caller's specification.
    //

    HashTable->Array = BlMmAllocateHeap(sizeof(LIST_ENTRY) * HashTable->Size);
    if (HashTable->Array == NULL) {
        Status = STATUS_NO_MEMORY;
        goto HtCreateDone;
    }

    //
    // Initialize the hash table.
    //

    for (Id = 0; Id < HashTable->Size; Id += 1) {
        InitializeListHead(&(HashTable->Array[Id]));
    }

    //
    // Attempt to add the hash table to the global array of managed
    // hash tables.
    //

    Status = BlTblSetEntry(&HtTableArray,
                           &HtTableSize,
                           HashTable,
                           HashTableId,
                           TblDoNotPurgeEntry);

HtCreateDone:
    if (!NT_SUCCESS(Status)) {

        //
        // If we just allocated a hash table array, free it so we don't get
        // out of sync.
        //

        if (HtTableEntries == 0) {
            if (HtTableArray != NULL) {
                BlMmFreeHeap(HtTableArray);
                HtTableArray = NULL;
                HtTableSize = 0;
            }
        }

        //
        // Free any other memory we just allocated.
        //

        if (HashTable != NULL) {
            if (HashTable->Array != NULL) {
                BlMmFreeHeap(HashTable->Array);
            }

            BlMmFreeHeap(HashTable);
        }

    } else {

        //
        // Successful in attempt to create the hash table.
        //

        HtTableEntries += 1;
    }

    BlpInterfaceExit(InterfaceHt);
    return Status;
}

NTSTATUS
BlHtDestroy (
    __in HASH_TABLE_ID HashTableId
    )

/*++

Routine Description:

    Destroys the hash table associated with the specified hash table ID.
    This operation includes the freeing of all memory associated with the
    hash table and invalidating the hash table identifier.

Arguments:

    HashTableId - Supplies an identifier for a hash table.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER when HashTableId is invalid.

--*/

{

    PHASH_ENTRY HashEntry;
    PHASH_TABLE HashTable;
    ULONG Id;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceHt);

    if (HtTableSize <= HashTableId) {
        Status = STATUS_INVALID_PARAMETER;
        goto HtDestroyEnd;
    }

    HashTable = HtTableArray[HashTableId];

    //
    // Walk the hash table, freeing memory allocated for each hash entry.
    //

    for (Id = 0; Id < HashTable->Size; Id += 1) {

        //
        // Walk the collision chain freeing memory allocated for each hash
        // entry.
        //

        NextEntry = HashTable->Array[Id].Flink;
        while (NextEntry != &(HashTable->Array[Id])) {
            HashEntry = CONTAINING_RECORD(NextEntry, HASH_ENTRY, ListEntry);
            NextEntry = NextEntry->Flink;

            BlMmFreeHeap(HashEntry);
        }
    }

    //
    // Free other memory allocated by this module for the hash table.
    //

    BlMmFreeHeap(HashTable->Array);
    BlMmFreeHeap(HashTable);

    //
    // Remove hash table from HtTableArray.
    //

    HtTableArray[HashTableId] = NULL;
    HtTableEntries -= 1;
    Status = STATUS_SUCCESS;


HtDestroyEnd:

    //
    // Free up the hash table array if there is no entries left.  The global
    // table is kept this way to avoid requiring an initialize/destroy routine.
    //

    if (HtTableEntries == 0) {
        if (HtTableArray != NULL) {
            BlMmFreeHeap(HtTableArray);
            HtTableArray = NULL;
            HtTableSize = 0;
        }
    }

    BlpInterfaceExit(InterfaceHt);
    return Status;
}

NTSTATUS
BlHtLookup (
    __in HASH_TABLE_ID HashTableId,
    __in PHASH_KEY Key,
    __out_opt PHASH_DATA *Data
    )

/*++

Routine Description:

    Performs a lookup operation for a key in a hash table.  If the specified
    key exists in the hash table, a pointer to the associating data is
    returned.

Arguments:

    HashTableId - Supplies an identifier for a hash table.

    Key - Supplies the key for the object to lookup.

    Data - Supplies a pointer to a variable that receives a pointer to the data
        associated with the supplied key in the hash table.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the specifed hash table is invalid or either
        the specified key or data pointers are NULL.

    STATUS_NOT_FOUND if the specified key does not exist in the hash table.

--*/

{

    PHASH_ENTRY HashEntry;
    PHASH_TABLE HashTable;
    ULONG Index;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceHt);

    //
    // Parameter checking.  Both the hash table identifier and the lookup
    // key must be valid.  A valid lookup key contains some sort of data and
    // if the VALUE flag is set, the key's value is a ULONG_PTR.  Ensure that
    // the size of the hashed value accurately reflects this request.
    //

    if ((HtTableSize <= HashTableId) ||
        (ARGUMENT_PRESENT(Key) == FALSE) ||
        (CHECK_FLAG(Key->Flags, HASH_KEY_FLAGS_VALUE) &&
         (Key->Size != sizeof(ULONG_PTR)))) {

        Status = STATUS_INVALID_PARAMETER;
        goto HtLookupEnd;
    }

    //
    // Map the specified key to a bucket (index) in the hash table.
    //

    HashTable = HtTableArray[HashTableId];
    Index = HashTable->HashFunction(Key, HashTable->Size);

    //
    // Walk the collision chain looking for an entry with the specified key.
    // If a match is found, return a pointer to the hashed data structure.
    //

    Status = STATUS_NOT_FOUND;
    NextEntry = HashTable->Array[Index].Flink;
    while (NextEntry != &(HashTable->Array[Index])) {
        HashEntry = CONTAINING_RECORD(NextEntry, HASH_ENTRY, ListEntry);
        if (HashTable->CompareFunction(&HashEntry->Key, Key) != FALSE) {
            if (ARGUMENT_PRESENT(Data) != FALSE) {
                *Data = &HashEntry->Data;
            }

            Status = STATUS_SUCCESS;
            break;
        }

        NextEntry = NextEntry->Flink;
    }

HtLookupEnd:
    BlpInterfaceExit(InterfaceHt);
    return Status;
}

NTSTATUS
BlHtStore (
    __in HASH_TABLE_ID HashTableId,
    __in PHASH_KEY Key,
    __in_bcount(DataSize) PVOID Data,
    __in ULONG DataSize
    )

/*++

Routine Description:

    Inserts a key / data pair into the specified hash table.  The entry can be
    looked up or deleted using the same key.

Arguments:

    HashTableId - Supplies an identifier for a hash table.

    Key - Supplies the key for the object to be inserted into the hash table.

    Data - Supplies a pointer to a variable to store in the hash table.

    DataSize - Supplies the size of the data value.

Return Value:

    STATUS_SUCCESS if successful.

    STATUS_INVALID_PARAMETER if the specified hash table is invalid or if
        either the specified key or data are NULL.

    STATUS_OBJECT_NAME_COLLISION if there exists an entry in the hash table
        with the specified key.

    STATUS_NO_MEMORY if an allocation fails for an internal data structure.

--*/

{

    PHASH_ENTRY HashEntry;
    PHASH_TABLE HashTable;
    ULONG Index;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceHt);

    //
    // Parameter checking.  Both the hash table identifier and the lookup
    // key must be valid.  A valid lookup key contains some sort of data and
    // if the VALUE flag is set, the key's value is a ULONG_PTR.  Ensure that
    // the size of the hashed value accurately reflects this request.
    //

    if ((HtTableSize <= HashTableId) ||
        (ARGUMENT_PRESENT(Key) == FALSE) ||
        (ARGUMENT_PRESENT(Data) == FALSE) ||
        (Key->Size == 0) || (Key->Value == NULL) || (DataSize == 0) ||
        (CHECK_FLAG(Key->Flags, HASH_KEY_FLAGS_VALUE) &&
         (Key->Size != sizeof(ULONG_PTR)))) {

        Status = STATUS_INVALID_PARAMETER;
        goto HtStoreEnd;
    }

    HashTable = HtTableArray[HashTableId];

    //
    // Assert that an object with the same key does not already exist in the
    // hash table.
    //

    ASSERT(!NT_SUCCESS(BlHtLookup(HashTableId, Key, NULL)));

    //
    // Allocate and initialize an internal object for the key / data pair.
    // This object contains indirect pointers to any memory allocated by
    // a caller (Key.Value, Data).
    //

    HashEntry = BlMmAllocateHeap(sizeof(HASH_ENTRY));
    if (HashEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto HtStoreEnd;
    }

    RtlCopyMemory(&HashEntry->Key, Key, sizeof(HASH_KEY));
    HashEntry->Data.Size = DataSize;
    HashEntry->Data.Value = Data;

    //
    // Insert the internal hash object into the hash table.
    //

    Index = HashTable->HashFunction(Key, HashTable->Size);
    InsertHeadList(&HashTable->Array[Index], &HashEntry->ListEntry);
    Status = STATUS_SUCCESS;

HtStoreEnd:
    BlpInterfaceExit(InterfaceHt);
    return Status;
}

NTSTATUS
BlHtDelete (
    __in HASH_TABLE_ID HashTableId,
    __in PHASH_KEY Key
    )

/*++

Routine Description:

    Removes the specified entry from the hash table.

Arguments:

    HashTableId - Supplies an identifier for a hash table.

    Key - Supplies the key for the object to be deleted.

Return Value:

    STATUS_SUCCESS if successful.

    STATUS_INVALID_PARAMETER if the specified hash table is invalid or the
        specified key is NULL.

    STATUS_NOT_FOUND if an entry does not exist with the specified Key.

--*/

{

    PHASH_ENTRY HashEntry;
    PHASH_TABLE HashTable;
    ULONG Index;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceHt);

    //
    // Parameter checking.  Both the hash table identifier and the lookup
    // key must be valid.  A valid lookup key contains some sort of data and
    // if the VALUE flag is set, the key's value is a ULONG_PTR.  Ensure that
    // the size of the hashed value accurately reflects this request.
    //

    if ((HtTableSize <= HashTableId) ||
        (ARGUMENT_PRESENT(Key) == FALSE) ||
        (Key->Size == 0) || (Key->Value == NULL) ||
        (CHECK_FLAG(Key->Flags, HASH_KEY_FLAGS_VALUE) &&
         (Key->Size != sizeof(ULONG_PTR)))) {

        Status = STATUS_INVALID_PARAMETER;
        goto HtDeleteEnd;
    }

    //
    // Map the specified Key to a bucket (index) in the hash table.
    //

    HashTable = HtTableArray[HashTableId];
    Index = HashTable->HashFunction(Key, HashTable->Size);

    //
    // Walk the collision chain looking for an entry with the specified
    // key.  Once found, remove the entry from the chain, freeing memory
    // allocated for the hash entry.
    //

    Status = STATUS_NOT_FOUND;
    NextEntry = HashTable->Array[Index].Flink;
    while (NextEntry != &(HashTable->Array[Index])) {
        HashEntry = CONTAINING_RECORD(NextEntry, HASH_ENTRY, ListEntry);
        if (HashTable->CompareFunction(&HashEntry->Key, Key) != FALSE) {

            //
            // Remove the entry from the hash table.
            //

            RemoveEntryList(&HashEntry->ListEntry);
            BlMmFreeHeap(HashEntry);
            Status = STATUS_SUCCESS;
            break;
        }

        NextEntry = NextEntry->Flink;
    }

HtDeleteEnd:
    BlpInterfaceExit(InterfaceHt);
    return Status;
}

NTSTATUS
BlHtGetNext (
    __in HASH_TABLE_ID HashTableId,
    __in_opt PHASH_KEY PreviousKey,
    __out PHASH_KEY HashKey,
    __out PHASH_DATA HashData
    )

/*++

Routine Description:

    Enumerates the entries in the hash table returning a key/data pair each
    time.

Arguments:

    HashTableId - Supplies the id of the hashtable.

    PreviousKey - Supplies a pointer to the key whose next key/data pair is to
        be found. If null, then the very first key/data pair in the hashtable
        are returned.

    HashKey - Supplies a pointer to the key which on return would contain the
        next hash key.

    HashData - Supplies a pointer to the data which on return would contain the
        hash data corresponding to the next key we found.

Return Value:

    STATUS_SUCCESS if successful.

    STATUS_INVALID_PARAMETER if the specified hash table is invalid or the
        or one of the parameters passed in are incorrect.

    STATUS_NOT_FOUND if an entry does not exist with the specified Key or this
        is the last entry in the hash table.

--*/

{

    PHASH_ENTRY HashEntry;
    PHASH_TABLE HashTable;
    ULONG Index;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceHt);

    //
    // Parameter checking. Check that the hash table identifier and the pointers
    // to the data to be returned are valid.
    //

    if ((HtTableSize <= HashTableId) ||
        (ARGUMENT_PRESENT(HashKey) == FALSE) ||
        (ARGUMENT_PRESENT(HashData) == FALSE)) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlHtGetNextEnd;
    }

    //
    // Parameter checking. If the lookup key is present it must be valid. A
    // valid lookup key contains some sort of data and if the VALUE flag is set,
    // the key's value is a ULONG_PTR.  Ensure that the size of the hashed value
    // accurately reflects this request.
    //
    //

    if ((ARGUMENT_PRESENT(PreviousKey) == TRUE) &&
        ((PreviousKey->Size == 0) ||
         (CHECK_FLAG(PreviousKey->Flags, HASH_KEY_FLAGS_VALUE) &&
          (PreviousKey->Size != sizeof(ULONG_PTR))) ||
         ((CHECK_FLAG(PreviousKey->Flags, HASH_KEY_FLAGS_VALUE) == 0) &&
          (PreviousKey->Value == NULL)))) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlHtGetNextEnd;

    }

    HashTable = HtTableArray[HashTableId];

    //
    // If the previous key is null then start from the first list entry in the
    // first bucket, otherwise start from the given key.
    //

    if (PreviousKey != NULL) {
        Index = HashTable->HashFunction(PreviousKey, HashTable->Size);

        //
        // Walk the collision chain looking for an entry with the specified key.
        // If a match is found, NextEntry will be pointing to the match found.
        //

        Status = STATUS_NOT_FOUND;
        NextEntry = HashTable->Array[Index].Flink;
        while (NextEntry != &(HashTable->Array[Index])) {
            HashEntry = CONTAINING_RECORD(NextEntry, HASH_ENTRY, ListEntry);
            if (HashTable->CompareFunction(&HashEntry->Key,
                                           PreviousKey) != FALSE) {

                Status = STATUS_SUCCESS;
                break;
            }

            NextEntry = NextEntry->Flink;
        }

        if (Status == STATUS_NOT_FOUND) {
            goto BlHtGetNextEnd;
        }

    } else {
        Index = 0;
        NextEntry = &(HashTable->Array[Index]);
    }

    //
    // Check if this bucket has any entries. If not, go to the next list.
    //

    Status = STATUS_NOT_FOUND;
    while (Index < HashTable->Size) {
        if (NextEntry->Flink != &(HashTable->Array[Index])) {
            HashEntry = CONTAINING_RECORD(NextEntry->Flink,
                                          HASH_ENTRY,
                                          ListEntry);

            RtlCopyMemory(HashData, &HashEntry->Data, sizeof(HASH_DATA));
            RtlCopyMemory(HashKey, &HashEntry->Key, sizeof(HASH_KEY));
            Status = STATUS_SUCCESS;
            break;
        }

        Index++;
        NextEntry = &(HashTable->Array[Index]);
    }

BlHtGetNextEnd:

    BlpInterfaceExit(InterfaceHt);
    return Status;
}

VOID
BlHtEnumerateAndRemoveEntries (
    __in HASH_TABLE_ID HashTableId,
    __in PHASH_REMOVE_CALLBACK Callback,
    __in_opt PVOID CallbackContext
    )

/*++

Routine Description:

    This routine will call supplied enumeration function for all entries in the
    hash table, and will remove the entry from the table if requested to do so
    by the callback.

Arguments:

    HashTableId - Supplies the id of the hashtable.

    Callback - Supplies enumeration callback function. The function is used to
        decide if the entry should be removed.

    CallbackContext - Supplies context value which will be passed to the
        enumeration callback.

Return Value:

    None.

--*/

{

    PHASH_ENTRY HashEntry;
    PHASH_TABLE HashTable;
    ULONG Id;
    PLIST_ENTRY NextEntry;
    BOOLEAN Remove;

    BlpInterfaceEnter(InterfaceHt);

    NT_ASSERT(HtTableSize > HashTableId);

    //
    // Enumerate hash table entries, checking if entry should be removed.
    //

    HashTable = HtTableArray[HashTableId];
    for (Id = 0; Id < HashTable->Size; Id += 1) {
        NextEntry = HashTable->Array[Id].Flink;
        while (NextEntry != &(HashTable->Array[Id])) {
            HashEntry = CONTAINING_RECORD(NextEntry, HASH_ENTRY, ListEntry);
            NextEntry = NextEntry->Flink;
            
            //
            // Call the callback to check if the entry is to be removed.
            //
            
            Remove = (*Callback)(&HashEntry->Key,
                                 &HashEntry->Data,
                                 CallbackContext);

            //
            // Remove and free the entry if indicated to do so by the callback.
            //

            if (Remove != FALSE) {
                RemoveEntryList(&HashEntry->ListEntry);
                BlMmFreeHeap(HashEntry);
            }
        }
    }

    BlpInterfaceExit(InterfaceHt);
    return;
}

// ------------------------------------------------------ Default Hash Function

ULONG
DefaultHashFunction (
    __in PHASH_KEY Key,
    __in ULONG HashTableSize
    )

/*++

Routine Description:

    Maps an arbitrary key to a bucket by summing the value of each byte in
    the Key's value and returning the result modulo HashTableSize.

Arguments:

    Key - Supplies the key for the object to hash.

    HashTableSize - Supplies the size of the hash table.

Return Value:

    A value between zero and HashTableSize - 1.

--*/

{

    ULONG Accum;
    ULONG Id;
    PUCHAR UcharPtr;

    ASSERT(Key != NULL);
    ASSERT(Key->Size != 0);
    ASSERT(Key->Value != NULL);

    //
    // Sum up the bytes of Key.
    //

    if (CHECK_FLAG(Key->Flags, HASH_KEY_FLAGS_VALUE)) {
        UcharPtr = (PUCHAR)&Key->Value;

    } else {
        UcharPtr = Key->Value;
    }

    for (Id = 0, Accum = 0; Id < Key->Size; Id += 1) {
        Accum += UcharPtr[Id];
    }

    return (Accum % HashTableSize);
}

// --------------------------------------------------------- Internal Functions

BOOLEAN
HtpCompareKeys (
    __in PHASH_KEY Key1,
    __in PHASH_KEY Key2
    )

/*++

Routine Description:

    This routine determines if two keys are the same. When HASH_KEY_FLAGS is
    set, two keys are the same if their values are the same. Otherwise, two
    keys are identical if a byte comparison of their value pointers are
    identical.

Arguments:

    Key1 - Supplies a pointer to a key that is to be compared.

    Key2 - Supplies a pointer to a second key that is to be compared.

Return Value:

    TRUE if Key1 and Key2 are identical.
    FALSE otherwise.

--*/

{

    ULONG Size;

    ASSERT(Key1 != NULL);
    ASSERT(Key2 != NULL);

    //
    // For two keys to be the same size, they either need to point to the
    // same data (determined by a byte comparison) or they both must
    // have the same value (when HASH_KEY_FLAGS_VALUE is set).
    //

    if ((Key1->Size != Key2->Size) || (Key1->Flags != Key2->Flags)) {
        return FALSE;
    }

    if (CHECK_FLAG(Key1->Flags, HASH_KEY_FLAGS_VALUE)) {
        return (Key1->Value == Key2->Value) ? TRUE : FALSE;
    }

    //
    // Perform a byte comparison of Key.Value pointers.
    //

    Size = (ULONG)RtlCompareMemory(Key1->Value, Key2->Value, Key1->Size);
    return (Size == Key1->Size) ? TRUE : FALSE;
}


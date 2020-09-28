l & PCI_ENABLE_BRIDGE_ISA) {

            State->WorkSpace |= PORT_ARBITER_ISA_BIT_SET;
        }

        State->WorkSpace |= PORT_ARBITER_BRIDGE_WINDOW;
    }

    //
    // Set some attributes
    //
    PciSetAllocationAttributes(State);

    TRACE(ARBITER_TRACE_LEVEL, "Allocation State after preprocessing\n");
    DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);

    return STATUS_SUCCESS;
}

BOOLEAN
PciIoArbiter_GetNextAllocationRange(
    __inout PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This is the GetNextAllocationRange entrypoint for the PCI I/O
    arbiter.  GetNextAllocationRange is the outer loop of finding
    a solution for a requirement.  It loops through entries in the
    arbiter's ordering list, trying to find one that works.  This
    version comprehends the fact that bridges with their ISA bit
    set have a very simple ordering list - they can live either
    in 32 bit space or 16 bit space.  It sets up the ordering list
    for those bridges.

Parameters:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    NT Status code.

--*/
{
    ARBITER_ORDERING_LIST savedOrderingList = {0};
    BOOLEAN rangeFound, doIsaBit;

    PAGED_CODE();

    TRACE(ARBITER_TRACE_LEVEL,
          "\nPciIoArbiter_GetNextAllocationRange for arbiter %p\n",
          Arbiter
          );

    doIsaBit = BITS_SET(State->WorkSpace,
                        PORT_ARBITER_BRIDGE_WINDOW | PORT_ARBITER_ISA_BIT_SET);


    if (doIsaBit) {
        savedOrderingList = Arbiter->OrderingList;
        Arbiter->OrderingList = PciBridgeOrderingList;
    }

    rangeFound = ArbGetNextAllocationRange(Arbiter, State);

    if (doIsaBit) {

        //
        // If we have reached preferred reserved priority then we fail as we
        // have already considered both the 16 and 32 bit IO cases and using
        // the reserved may allow us to stradle the boundry.
        //
        if (rangeFound &&
            (State->CurrentAlternative->Priority > ARBITER_PRIORITY_PREFERRED_RESERVED)) {

            rangeFound = FALSE;
        }

        Arbiter->OrderingList = savedOrderingList;
    }

    //
    // ArbGetNextAllocationRange may have updated the current alternative.
    // This routine updates the attributes for RtlFindRange based on this change.
    //
    PciSetAllocationAttributes(State);

    if (rangeFound) {
        TRACE(ARBITER_TRACE_LEVEL, "Next Allocation range found\n");
        DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);
    }

    return rangeFound;
}

BOOLEAN
PciIoArbiter_FindSuitableRange(
    __in PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This is the FindSuitableRange entrypoint for the PCI I/O arbiter.
    FindSuitableRange is the inner loop of the arbitration.  Within
    every range selected by GetNextAllocationRange, this routine loops
    selecting possibilities.  It tries to find a free range that matches the
    requirements in State while restricting its possible solutions to the range
    State->Start to State->CurrentMaximum.  On success State->Start and
    State->End represent this range.  Conflicts between boot configs are allowed.
    This version detects requirements for bridges with the ISA bit set, which
    relaxes the ranges that must be available for the range to be allowed.
    It also makes sure all aliases are available for ranges that don't do
    full 16 bit decode.












Arguments:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    TRUE if we found a range, FALSE otherwise.

--*/
{
    PIO_RESOURCE_DESCRIPTOR descriptor;
    BOOLEAN returnValue;

    PAGED_CODE();

    TRACE(ARBITER_TRACE_LEVEL, "\nPciIoArbiter_FindSuitableRange for arbiter %p\n", Arbiter);

    //
    // On downlevel OSes, a fixed I/O requirement was treated like a boot config
    // if the parent was a root bus or a bridge on its initial I/O window
    // assignment. In this case treating like a boot config means allowing NULL
    // conflicts. This is now deemed unnecessary and is being removed.
    //

    if (State->CurrentAlternative->Length == 0) {

        returnValue = ArbFindSuitableRange(Arbiter, State);

        if (returnValue) {

            ARB_PRINT(2,
                ("Zero length solution solution for %p = 0x%I64x-0x%I64x, %s\n",
                State->Entry->PhysicalDeviceObject,
                State->Start,
                State->End,
                State->CurrentAlternative->Flags & ARBITER_ALTERNATIVE_FLAG_SHARED ?
                    "shared" : "non-shared"
                ));

            //
            // Set the result in the arbiter appropriately so that we
            // don't try and translate this zero requirement - it won't!
            //
            State->Entry->Result = ArbiterResultNullRequest;
        }

        TRACE(ARBITER_TRACE_LEVEL, "\tarbitrating zero length requirement - returning %d\n", returnValue);
        DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);

        return returnValue;
    }

    //
    // Check if this is a window for a bridge with the ISA bit set.
    // If so, the bridge is only decoding the ISA aliases.  Look
    // for a suitable range given this relaxation.
    //
    if ((State->WorkSpace & PORT_ARBITER_BRIDGE_WINDOW) &&
        (State->WorkSpace & PORT_ARBITER_ISA_BIT_SET) &&
        (State->CurrentMaximum <= PCI_BRIDGE_ISA_BIT_MAX)) {

        returnValue = PciFindSuitableIsaBitRange(Arbiter, State);

        TRACE(ARBITER_TRACE_LEVEL, "\tfinding ISA bit window range - returning %d\n", returnValue);
        DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);

        return returnValue;
    }

    //
    // Otherwise, call the default arbitration routine to find
    // a solution, munging as necessary.
    //
    returnValue = FALSE;
    while (State->CurrentMaximum >= State->CurrentMinimum) {

        if (ArbFindSuitableRange(Arbiter, State)) {

            descriptor = State->CurrentAlternative->Descriptor;
            if (!(descriptor->Flags & CM_RESOURCE_PORT_10_BIT_DECODE) &&
                !(descriptor->Flags & CM_RESOURCE_PORT_12_BIT_DECODE)) {

                PCI_ASSERT((descriptor->Flags & CM_RESOURCE_PORT_POSITIVE_DECODE) ||
                           (descriptor->Flags & CM_RESOURCE_PORT_16_BIT_DECODE));

                //
                // If the 10 or 12 bit flags are not set, there are no aliases.
                // The range is good.
                //
                returnValue = TRUE;
                break;
            }

            //
            // If all of the aliases of the chosen range are available, the range is good.
            //
            if (PciAllAliasesAvailable(Arbiter, State)) {

                returnValue = TRUE;
                break;
            }

            //
            // Otherwise, update the state and try again.
            //
            if ((State->Start - 1) > State->Start) {
                break;
            }

            State->CurrentMaximum = State->Start - 1;

        } else {

            //
            // The default routine failed, so there's no solution.
            //
            TRACE(ARBITER_TRACE_LEVEL, "ArbFindSuitableRange couldn't find solution\n");
            break;
        }
    }

    TRACE(ARBITER_TRACE_LEVEL, "FindSuitableRange returning %d\n", returnValue);
    DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);

    return returnValue;
}

VOID
PciIoArbiter_AddAllocation(
    __in PARBITER_INSTANCE Arbiter,
    __in PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This is the AddAllocation entry point to the PCI I/O arbiter.
    This routine is called from AllocateEntry once we have found
    a possible solution (State->Start - State->End).  It adds the
    ranges that will not be available if we commit to this solution
    to Arbiter->PossibleAllocation.  This version handles bridges
    with the ISA bit, for which only pieces of the allocated range
    actually need to be considered unavailable.  It also handles
    ranges with aliases, for which all the aliases need to be
    added as unavailable.

Arguments:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    None.

--*/
{
    PAGED_CODE();

    PciAddOrBacktrackIoAllocation(Arbiter,
                                  State,
                                  ArbAddAllocation
                                  );
}

VOID
PciIoArbiter_BacktrackAllocation(
     __in PARBITER_INSTANCE Arbiter,
     __in PARBITER_ALLOCATION_STATE State
     )
/*++

Routine Description:

    This is the BacktrackAllocation entry point to the PCI I/O arbiter.
    This routine is called from AllocateEntry if it is determined that
    the range indicated by State->Start will not work (after it was added
    to the range list by AddAllocation).  It removes these ranges
    from the range list. This version handles bridges with the ISA bit,
    for which only pieces of the allocated range actually need to be
    removed.  It also handles ranges with aliases, for which all the
    aliases need to be added as unavailable.

Arguments:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    None.

--*/
{
    PAGED_CODE();

    PciAddOrBacktrackIoAllocation(Arbiter,
                                  State,
                                  ArbBacktrackAllocation
                                  );
}

BOOLEAN
PciIoArbiter_OverrideConflict(
    __in PARBITER_INSTANCE Arbiter,
    __inout PARBITER_ALLOCATION_STATE State
    )
/*++

Routine Description:

    This is the OverrideConflict entrypoint of the PCI I/O arbiter.
    This is called when a conflict between a possible solution for
    a requirement and a solution for another requirement has been found.
    It determines whether the conflict is real or should be allowed
    to stand.  It allows conflicts between fixed requirements from the same
    device.  Conflicts between passively decoded ranges and NULL are also
    allowed.

Arguments:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

Return Value:

    TRUE if the conflict is allowable, FALSE otherwise

--*/
{
    PPCI_DEVICE ownerExtension;
    RTL_RANGE_LIST_ITERATOR iterator;
    PDEVICE_OBJECT owner;
    PRTL_RANGE current;
    BOOLEAN ok;

    PAGED_CODE();

    TRACE(ARBITER_TRACE_LEVEL, "\nPciIoArbiter_OverrideConflict for arbiter %p\n", Arbiter);
    DebugDumpArbiterAllocationState(ARBITER_TRACE_LEVEL, State);

    if (!(State->CurrentAlternative->Flags & ARBITER_ALTERNATIVE_FLAG_FIXED)) {
        return FALSE;
    }

    ok = FALSE;
    FOR_ALL_RANGES(Arbiter->PossibleAllocation, &iterator, current) {

        if (INTERSECT(current->Start, current->End,
                      State->CurrentMinimum, State->CurrentMaximum)) {

            if (current->Attributes & State->RangeAvailableAttributes) {

                //
                // We DON'T set ok to true because RtlFindRange would have allowed
                // this conflict as well, so it can't be the cause of the conflict.
                //
                continue;
            }

            //
            // Allow conflicts between fixed requirements from the same device.
            //
            if ((current->Owner == State->Entry->PhysicalDeviceObject) &&
                (State->CurrentAlternative->Flags & ARBITER_ALTERNATIVE_FLAG_FIXED) &&
                (current->Attributes & ARBITER_RANGE_FIXED_REQUIREMENT)) {

                ARB_PRINT(1,
                    ("PnP Warning: Device reported self-conflicting requirement\n"
                    ));

                State->Start=State->CurrentMinimum;
                State->End=State->CurrentMaximum;

                ok = TRUE;

                TRACE(ARBITER_TRACE_LEVEL, "Possible allowed self-conflicting range:\n");
                TRACE(ARBITER_TRACE_LEVEL, "%I64x-%I64x Flags-0x%x O=%p U=%p\n",
                      current->Start, current->End,
                      current->Flags, current->Owner,
                      current->UserData
                      );
                continue;
            }

            //
            // If the passive decode flag is set and we conflict with a bridge then
            // allow the conflict.  We also allow the conflict if the range never
            // makes it onto the bus (Owner == NULL)
            //










            if (State->CurrentAlternative->Descriptor->Flags & CM_RESOURCE_PORT_PASSIVE_DECODE) {

                owner = (PDEVICE_OBJECT)current->Owner;
                if (owner && PCI_DEVICE_OBJECT(owner)) {

                    ownerExtension = (PPCI_DEVICE)owner->DeviceExtension;
                } else {
                    ownerExtension = NULL;
                }

                if ((owner == NULL) ||
                    (ownerExtension && (ownerExtension->HeaderType != PCI_DEVICE_TYPE))) {

                    State->Start=State->CurrentMinimum;
                    State->End=State->CurrentMaximum;

                    ok = TRUE;

                    TRACE(ARBITER_TRACE_LEVEL, "Possible allowed conflict with passive decode:\n");
                    TRACE(ARBITER_TRACE_LEVEL, "%I64x-%I64x Flags-0x%x O=%p U=%p\n",
                          current->Start, current->End,
                          current->Flags, current->Owner,
                          current->UserData
                          );
                    continue;
                }
            }

            //
            // The conflict is still valid
            //
            return FALSE;
        }
    }
    return ok;
}

// ------------------------------------------------------------------ I/O Arbiter helpers

VOID
PciAddOrBacktrackIoAllocation(
     __in PARBITER_INSTANCE Arbiter,
     __in PARBITER_ALLOCATION_STATE State,
     __in PARBITER_BACKTRACK_ALLOCATION Callback
     )
/*++

Routine Description:

    This relies on the fact that PARBITER_BACKTRACK_ALLOCATION and
    PARBITER_ADD_ALLOCATION are of the same type.  It does the
    work of both AddAllocation and BacktrackAllocation, which
    is the same, except for the work of actually adding or
    backtracking each range.

Arguments:

    Arbiter - The arbiter being used to arbitrate this requirement.

    State - The current allocation state of the requirement.

    Callback - The routine to execute for each range that needs to
        be added/backtracked.  If this is called from AddAllocation,
        it will be ArbAddAllocation, for instance.

Return Value:

    None.

--*/
{
    ARBITER_ALLOCATION_STATE localState;
    PIO_RESOURCE_DESCRIPTOR descriptor;
    ULONG increment;

    PAGED_CODE();

    PCI_ASSERT(Arbiter);
    PCI_ASSERT(State);

    RtlCopyMemory(&localState, State, sizeof(ARBITER_ALLOCATION_STATE));

    if ((State->WorkSpace & PORT_ARBITER_BRIDGE_WINDOW) &&
        (State->WorkSpace & PORT_ARBITER_ISA_BIT_SET) &&
        (State->Start < PCI_BRIDGE_ISA_BIT_MAX)) {

        //
        // We don't support IO windows that straddle the 16/32 bit boundry
        //
        PCI_ASSERT(State->End <= PCI_BRIDGE_ISA_BIT_MAX);

        //
        // This is a bridge window with the ISA bit, so we only have to
        // add/backtrack up to ISA alias ranges or the end of range as
        // the case may be. The latter case is more appropriate for cardbus
        // bridges.
        //
        increment = PCI_BRIDGE_ISA_BIT_WIDTH - 1;

        for (;
             (localState.Start < State->End) && (localState.Start < PCI_BRIDGE_ISA_BIT_MAX);
             localState.Start += PCI_BRIDGE_ISA_BIT_STRIDE) {

            if ((State->End - localState.Start) >= increment) {
                localState.End = localState.Start + increment;
            } else {
                localState.End = State->End;
            }

            Callback(Arbiter, &localState);
        }

    } else {

        //
        // Process the base range
        //
        Callback(Arbiter, State);

        //
        // and any aliases
        //
        descriptor = State->CurrentAlternative->Descriptor;
        if ((descriptor->Flags & CM_RESOURCE_PORT_10_BIT_DECODE) ||
            (descriptor->Flags & CM_RESOURCE_PORT_12_BIT_DECODE)) {

            if (descriptor->F
idge = CurrentBridge->Parent;
                Direction = ToChild;
            }
        }

        if (CurrentBridge->Secondary >= MaxSubordinate) {

            //
            // If the secondary bus has reached the maximum, terminate the
            // search now and don't continue on any child. Set DevicePresent to
            // 1 here to make sure the device will not be ignored.
            //

            CurrentBridge->DevicesPresent = 1;
            goto UpdateParent;
        }

        //
        // Only do the following initialization once. If a bridge has children,
        // at the second time it reaches here, the DevicesPresent count
        // will not be 0. If a bridge has no child, it will not reach here
        // twice because the above code will move the traversal up one level.
        //

        if (CurrentBridge->DevicesPresent == 0) {
            if (CurrentBridge->RootBus == FALSE) {
                CurrentBridge->ExpressCaps = PciFindCapOffsetRaw(
                                                PciIF,
                                                CurrentBridge->Primary,
                                                CurrentBridge->Slot,
                                                PCI_CAPABILITY_ID_PCI_EXPRESS,
                                                NULL,
                                                NULL);

                CurrentBridge->SlotImplemented = PciIsDeviceSlotImplementedRaw(
                                                    PciIF,
                                                    CurrentBridge->Primary,
                                                    CurrentBridge->Slot,
                                                    CurrentBridge->ExpressCaps);

                CurrentBridge->HotplugCapable = PciIsDeviceHotplugCapableRaw(
                                                    PciIF,
                                                    CurrentBridge->Primary,
                                                    CurrentBridge->Slot,
                                                    CurrentBridge->ExpressCaps);
            } else {
                CurrentBridge->HotplugCapable = 0;
            }

            PciResetSlotEnum(CurrentBridge);
        }

        //
        // If DecreaseParentDevice is true, this means the last enumerated child
        // of the current bridge is ignored because it has no slot implemented
        // and is not hotplug capable. In this case, decrease the device count
        // of the current bridge by 1 as the child is ignored.
        //

        if (DecreaseParentDevice != FALSE) {
            CurrentBridge->DevicesPresent -= 1;

            PCI_ASSERT(CurrentBridge->DevicesPresent >= 0);

            DecreaseParentDevice = FALSE;
        }

        //
        // Traverse on children of this bridge looking for other bridges. If no
        // more child is found, go up one level.
        //

        if (PciNextPopulatedSlot(PciIF, CurrentBridge) == FALSE) {

            //
            // If there was nothing under the bridge, see if it supports hotplug.
            // If so, add to the HotPlugCount total. Once actual devices are
            // satisfied, any excess bus numbers will be distributed to bridges that
            // are hotplug capable.
            //
            // N.B. In the interest of conserving bus numbers for ports which
            //      have no devices plugged in, this underfunds bus numbers to ports
            //      which already have a device installed. The risk of this choice is
            //      that one may have no available bus numbers in the scenario where a
            //      user hot-removes a device and replaces it with a device which
            //      requires more bus numbers. If this turns out to be a problem,
            //      one can eliminate it by removing the check for
            //      (Parent->DevicesPresent == 0), so we allocate extra bus numbers
            //      for all hotplug ports.
            //

            if ((CurrentBridge->DevicesPresent == 0) &&
                (CurrentBridge->HotplugCapable != FALSE)) {

                CurrentBridge->HotplugCount += 1;
            }

            //
            // All the bridge resource requirements have now been calculated.
            // Adjust as necessary for alignment here.
            //

            PciAdjustBridgeRequirements(CurrentBridge);

            //
            // Update parent's information.
            //

UpdateParent:

            if (ParentBridge != NULL) {
                PciWriteBusNumsRaw(
                    PciIF,
                    CurrentBridge->Primary,
                    CurrentBridge->Slot,
                    0,
                    0,
                    0);

                //
                // Allocate an auxiliary data structure that remembers the
                // secondary/subordinate bus numbers that are assigned to the
                // current bridge. This will be used later during PciScanBus to
                // determine whether the bridge has child or is hotplug capable,
                // thus determines whether the PDO of the bridge will be created.
                //

                ChildBusNumHint = PciAllocatePool(PagedPool,
                                                  sizeof(PCI_CHILDREN_BUSNUM_HINT));

                if (ChildBusNumHint == NULL) {
                    PCI_ASSERT(FALSE);

                } else {
                    RtlZeroMemory(ChildBusNumHint, sizeof(PCI_CHILDREN_BUSNUM_HINT));
                    ChildBusNumHint->BusNumber = CurrentBridge->ActualPrimary;
                    ChildBusNumHint->Slot.u.AsULONG = CurrentBridge->Slot.u.AsULONG;
                }

                //
                // If the bridge has no child, nor is it hotplug capable, and
                // in addition is a Thunderbolt device, do not enumerate this
                // bridge and do not assign bus number to it.
                //

                if ((CurrentBridge->DevicesPresent == 0) &&
                    (CurrentBridge->SlotImplemented == 0) &&
                    (CurrentBridge->HotplugCount == 0) &&
                    (CurrentBridge->IsThunderBolt != FALSE)) {

                    DecreaseParentDevice = TRUE;
                    if (ChildBusNumHint != NULL) {
                        ChildBusNumHint->Secondary = 0;
                        ChildBusNumHint->Subordinate = 0;
                    }

                } else {
                    if (ChildBusNumHint != NULL) {
                        ChildBusNumHint->Secondary =
                            CurrentBridge->ActualSecondary;

                        ChildBusNumHint->Subordinate =
                            ChildBusNumHint->Secondary + CurrentBridge->BusNumCount - 1;
                    }

                    ParentBridge->BusNumCount += CurrentBridge->BusNumCount;
                    ParentBridge->HotplugCount += CurrentBridge->HotplugCount;
                    ParentBridge->ChildCount += 1;

                    //
                    // Add the child's BAR resource requirements to the
                    // parent requirements.
                    //

                    PciAddBridgeWindowRequirements(CurrentBridge, ParentBridge);
                    InsertTailList(&ParentBridge->Children, &CurrentBridge->Link);
                }

                if (ChildBusNumHint != NULL) {
                    InsertTailList(&Bus->ChildrenBusNumHints,
                                   &ChildBusNumHint->ListEntry);
                }

                CurrentBridge->Primary = 0;
                CurrentBridge->Secondary = 0;
                CurrentBridge->Subordinate = 0;
            }

            Direction = ToParent;
            continue;
        }

        CurrentBridge->DevicesPresent += 1;

        //
        // Add the child's resource requirements to the parent requirements.
        // This adds requirements specified by the BARs in the
        // child (whether bridge or device). It does not look
        // at windows or recurse beneath the child bridge to determine what
        // additional resources are required underneath child bridges.
        // These additional resources will be added later on in the function.
        //

        HeaderType = PciReadHeaderTypeRaw(PciIF,
                                          CurrentBridge->Secondary,
                                          CurrentBridge->EnumSlot);

        HeaderType &= ~PCI_MULTIFUNCTION;
        PciAddChildRequirementsToBridgeRequirements(PciIF,
                                                    CurrentBridge->Secondary,
                                                    CurrentBridge->EnumSlot,
                                                    HeaderType,
                                                    CurrentBridge->Requirements);

        if ((HeaderType != PCI_BRIDGE_TYPE) &&
            (HeaderType != PCI_CARDBUS_BRIDGE_TYPE)) {

            //
            // The child is a device (not a bridge).
            //
            // If the device supports SR-IOV, PciSriovBusNumbersRaw will return
            // the number of additional buses (worst-case) to address all VFs
            // implemented in the device.
            //

            CurrentBridge->BusNumCount += PciSriovBusNumbersRaw(
                                            PciIF,
                                            CurrentBridge->Secondary,
                                            CurrentBridge->EnumSlot,
                                            CurrentBridge->AriEnabled);

            //
            // Continue normal traversal on the current level, keep CurrentBridge
            // unchanged and Direction still ToChild.
            //

            continue;
        }

        if (!PciIsUnconfiguredBridge(PciIF,
                                     CurrentBridge->Secondary,
                                     CurrentBridge->EnumSlot)) {

            PCI_ASSERT(FALSE);

            //
            // If this edge case happens, terminate traversal.
            //

            goto UpdateParent;
        }

        //
        // Allocate a new bridge structure and initialize.
        //

        Child = PciAllocateBridge();
        if (Child == NULL) {

            PCI_ASSERT(FALSE);

            //
            // Terminate searching on this child, continue on other children.
            //

            continue;
        }

        Child->Parent = CurrentBridge;
        Child->ActualPrimary = CurrentBridge->ActualSecondary;
        Child->ActualSecondary = CurrentBridge->ActualSecondary + CurrentBridge->BusNumCount;
        Child->ChildCount = 0;
        Child->BusNumCount = 1;
        Child->HotplugCount = 0;
        Child->DevicesPresent = 0;
        Child->HeaderType = HeaderType;
        Child->Primary = CurrentBridge->Secondary;
        Child->RootBus = FALSE;
        Child->Secondary = Child->Primary + 1;
        Child->Subordinate = MaxSubordinate;
        Child->Slot = CurrentBridge->EnumSlot;
        Child->IsThunderBolt = PciIsDeviceThunderboltRaw(PciIF,
                                                         Child->Primary
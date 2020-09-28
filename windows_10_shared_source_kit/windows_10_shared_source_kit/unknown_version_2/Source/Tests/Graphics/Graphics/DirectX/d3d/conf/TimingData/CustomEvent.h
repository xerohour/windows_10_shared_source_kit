ignment) {
                Window->Alignment = BarRequirement.u.Port.Alignment;
            }

            break;
        }
    }
}

VOID
PciSaveBridgeRequirements(
    _In_ PBRIDGE Bridge,
    _In_ PPCI_SEGMENT Segment
)

/*++

Routine Description:

    This routine updates the requirement override hints. Hints are stored
    in an array attached to the Segment, and are indexed by the secondary
    bus number below the bridge.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Parent - Provides a pointer to the topmost bridge.

Return Value:
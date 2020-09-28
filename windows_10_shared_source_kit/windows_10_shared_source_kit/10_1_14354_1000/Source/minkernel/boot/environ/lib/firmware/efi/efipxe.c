/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efipxe.c

Abstract:

    Wrappers for EFI PXE Base Protocol APIs.

    The EFI PXE Base Protocol provides functional support to read and
    write UDP packets to a network interface as well as manage the network
    interface. This module provides wrappers for these functions to correctly
    handle cases such as whether the caller is in virtual mode, physical mode,
    etc.

--*/

// ------------------------------------------------------------------- Includes

#include "bootlibp.h"

// ------------------------------------------------------------------ Functions

EFI_SIMPLE_NETWORK_MODE*
EfiSnpGetMode (
    __in EFI_SIMPLE_NETWORK *Snp
    )

/*++

Routine Description:

    This function returns the pointer to MODE structure.

Arguments:

    Snp - Supplies pointer to EFI_SIMPLE_NETWORK_PROTOCOL structure.

Return Value:

    Returns pointer to MODE structure.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_SIMPLE_NETWORK_MODE *Mode;
    PHYSICAL_ADDRESS SnpPa = {0};

    Mode = NULL;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(Snp, &SnpPa);
        Snp = (EFI_SIMPLE_NETWORK *)(ULONG_PTR)SnpPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    Mode = Snp->Mode;

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return Mode;
}

NTSTATUS
EfiSnpGetStatus (
    __in EFI_SIMPLE_NETWORK *Snp,
    __out UINT32 *InterruptStatus,
    __out PVOID *TxBuf
    )

/*++

Routine Description:

    This function returns the interrupt status and recycled transmit buffers.

Arguments:

    Snp - Supplies pointer to EFI_SIMPLE_NETWORK_PROTOCOL structure.

    InterruptStatus - Supplies the currently active interrupts as a bitmask on
        return.

    TxBuf - Supplies the transmit buffer that has been processed and can be
        recycled.

Return Value:

    STATUS_SUCCESS if status is queried successfully.

    STATUS_UNSUCCESSFUL otherwise.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterruptStatusPa = {0};
    PHYSICAL_ADDRESS SnpPa = {0};
    PHYSICAL_ADDRESS TxBufPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(Snp, &SnpPa);
        Snp = (EFI_SIMPLE_NETWORK *)(ULONG_PTR)SnpPa.QuadPart;
        BlMmTranslateVirtualAddress(InterruptStatus, &InterruptStatusPa);
        InterruptStatus = (UINT32 *)(ULONG_PTR)InterruptStatusPa.QuadPart;
        BlMmTranslateVirtualAddress(TxBuf, &TxBufPa);
        TxBuf = (PVOID *)(ULONG_PTR)TxBufPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Resolve IP address to MAC address.
    //

    EfiStatus = Snp->GetStatus(Snp,
                               InterruptStatus,
                               TxBuf);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiSnpMCastToMac (
    __in EFI_SIMPLE_NETWORK *Snp,
    __in BOOLEAN IPv6,
    __in EFI_IP_ADDRESS *IpAddress,
    __out EFI_MAC_ADDRESS *MacAddress
    )

/*++

Routine Description:

    This routine translates a multicast ip address to mac address. The mac
    address then can be set of the network interface to receive multicast
    traffic.

Arguments:

    Snp - Supplies pointer to EFI_SIMPLE_NETWORK_PROTOCOL structure.

    IPv6 - This parameter is set to TRUE if provided IP address is IPv6
        otherwise this member is set to FALSE to indicate an IPv4 address.

    IpAddress - Supplies the IP address to translate.

    MacAddress - Supplies the output parameter which is populated with the mac
        address.

Return Value:

    STATUS_SUCCESS when the UDP packet is sent.

    STATUS_UNSUCCESSFUL when the UDP packet fails to be sent.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS IpAddressPa = {0};
    PHYSICAL_ADDRESS MacAddressPa = {0};
    PHYSICAL_ADDRESS SnpPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(Snp, &SnpPa);
        Snp = (EFI_SIMPLE_NETWORK *)(ULONG_PTR)SnpPa.QuadPart;
        BlMmTranslateVirtualAddress(IpAddress, &IpAddressPa);
        IpAddress = (EFI_IP_ADDRESS *)(ULONG_PTR)IpAddressPa.QuadPart;
        BlMmTranslateVirtualAddress(MacAddress, &MacAddressPa);
        MacAddress = (EFI_MAC_ADDRESS *)(ULONG_PTR)MacAddressPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Resolve IP address to MAC address.
    //

    EfiStatus = Snp->MCastIpToMac(Snp, IPv6, IpAddress, MacAddress);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiSnpReceiveFilters (
    __in EFI_SIMPLE_NETWORK *Snp,
    __in UINT32 Enable,
    __in UINT32 Disable,
    __in BOOLEAN ResetMCastFilter,
    __in UINTN MCastFilterCount,
    __in_ecount(MCastFilterCount) EFI_MAC_ADDRESS *MCastFilter
    )

/*++

Routine Description:

    This function updates receive filters for the network interface.

Arguments:

    Snp - Supplies pointer to EFI_SIMPLE_NETWORK structure.

    Enable - Specifies types of packet to start receiving.

    Disable - Specifies types of packet to stop receiving.

    ResetMCastFilter - Specifies if multicast address filter should be reset.

    MCastFilterCount - Specifies total number of new multicast mac addresses to
        start listening on.

    MCastFilter - Specifies the list of multicast mac addresses.

Return Value:

    STATUS_SUCCESS when the UDP packet is sent.

    STATUS_UNSUCCESSFUL when the UDP packet fails to be sent.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS MCastFilterPa = {0};
    PHYSICAL_ADDRESS SnpPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(Snp, &SnpPa);
        Snp = (EFI_SIMPLE_NETWORK *)(ULONG_PTR)SnpPa.QuadPart;
        BlMmTranslateVirtualAddress(MCastFilter, &MCastFilterPa);
        MCastFilter = (EFI_MAC_ADDRESS *)(ULONG_PTR)MCastFilterPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Resolve IP address to MAC address.
    //

    EfiStatus = Snp->ReceiveFilters(Snp,
                                    Enable,
                                    Disable,
                                    ResetMCastFilter,
                                    MCastFilterCount,
                                    MCastFilter);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiSnpReceivePacket (
    __in EFI_SIMPLE_NETWORK *Snp,
    __out_opt UINTN *HeaderSize,
    __inout UINTN *BufferSize,
    __out_bcount(*BufferSize) PVOID Buffer,
    __out_opt EFI_MAC_ADDRESS *SrcAddr,
    __out_opt EFI_MAC_ADDRESS *DestAddr,
    __out_opt UINT16 *Protocol
    )

/*++

Routine Description:

    This function receives a ethernet packet from the network.

Arguments:

    Snp - Supplies pointer to EFI_SIMPLE_NETWORK structure.

    HeaderSize - Supplies the output parameter which is set to the ethernet
        header size of the received packet.

    BufferSize - Supplies the size of buffer on input, and on output this
        parameter is set to the total number of bytes of the received packet.
        The returned size also includes the ethernet header length.

    Buffer - Supplies the buffer where received packet contents should be
        copied.

    SrcAddr - Supplies the output parameter for the source address of the
        received packet.

    DestAddr - Supplies the output parameter for the destination address of the
        received packet.

    Protocol - Supplies the output parameter for the protocol value as specified
        in the ethernet header of the received packet.

Return Value:

    STATUS_SUCCESS when the packet is received.

    STATUS_UNSUCCESSFUL when failed to receive packet.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS BufferPa = {0};
    PHYSICAL_ADDRESS BufferSizePa = {0};
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS DestAddrPa = {0};
    PHYSICAL_ADDRESS HeaderSizePa = {0};
    PHYSICAL_ADDRESS SnpPa = {0};
    PHYSICAL_ADDRESS SrcAddrPa = {0};
    PHYSICAL_ADDRESS ProtocolPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(Snp, &SnpPa);
        Snp = (EFI_SIMPLE_NETWORK *)(ULONG_PTR)SnpPa.QuadPart;
        BlMmTranslateVirtualAddress(HeaderSize, &HeaderSizePa);
        HeaderSize = (UINTN *)(ULONG_PTR)HeaderSizePa.QuadPart;
        BlMmTranslateVirtualAddress(BufferSize, &BufferSizePa);
        BufferSize = (UINTN *)(ULONG_PTR)BufferSizePa.QuadPart;
        BlMmTranslateVirtualAddress(Buffer, &BufferPa);
        Buffer = (PVOID)(ULONG_PTR)BufferPa.QuadPart;
        BlMmTranslateVirtualAddress(SrcAddr, &SrcAddrPa);
        SrcAddr = (EFI_MAC_ADDRESS *)(ULONG_PTR)SrcAddrPa.QuadPart;
        BlMmTranslateVirtualAddress(DestAddr, &DestAddrPa);
        DestAddr = (EFI_MAC_ADDRESS *)(ULONG_PTR)DestAddrPa.QuadPart;
        BlMmTranslateVirtualAddress(Protocol, &ProtocolPa);
        Protocol = (UINT16 *)(ULONG_PTR)ProtocolPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Resolve IP address to MAC address.
    //

    EfiStatus = Snp->Receive(Snp,
                             HeaderSize,
                             BufferSize,
                             Buffer,
                             SrcAddr,
                             DestAddr,
                             Protocol);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiSnpTransmitPacket (
    __in EFI_SIMPLE_NETWORK *Snp,
    __in UINTN HeaderSize,
    __in UINTN BufferSize,
    __in PVOID Buffer,
    __in EFI_MAC_ADDRESS *SrcAddress,
    __in EFI_MAC_ADDRESS *DestAddress,
    __in UINT16 *Protocol,
    __in BOOLEAN AsyncSend
    )

/*++

Routine Description:

    This function transmits the packet over the network.

Arguments:

    Snp - Supplies pointer to EFI_SIMPLE_NETWORK structure.

    HeaderSize - The size of the media header to be filled in by the function.
        If this is zero, the function fills in the media header otherwise caller
        is required to fill in the media header.

    BufferSize - Supplies the total buffer size including the media header.

    Buffer - Supplies the pointer to packet which includes the media header
        followed by the packet.

    SrcAddress - Supplies the source address of sender.

    DestAddress - Supplies the destination address of packet.

    Protocol - Supplies the protocol for the packet.

    AsyncSend - This parameter is set to TRUE if function should not wait for
        packet to be sent over the network.

Return Value:

    STATUS_SUCCESS when the UDP packet is sent.

    STATUS_UNSUCCESSFUL when the UDP packet fails to be sent.

--*/

{

    PHYSICAL_ADDRESS BufferPa = {0};
    PVOID BufferVa;
    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS SrcAddressPa = {0};
    PHYSICAL_ADDRESS DestAddressPa = {0};
    PHYSICAL_ADDRESS ProtocolPa = {0};
    PHYSICAL_ADDRESS SnpPa = {0};
    EFI_SIMPLE_NETWORK *SnpVa;
    NTSTATUS Status;

    BufferVa = Buffer;
    SnpVa = Snp;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(Snp, &SnpPa);
        Snp = (EFI_SIMPLE_NETWORK *)(ULONG_PTR)SnpPa.QuadPart;
        BlMmTranslateVirtualAddress(Buffer, &BufferPa);
        Buffer = (PVOID)(ULONG_PTR)BufferPa.QuadPart;
        BlMmTranslateVirtualAddress(SrcAddress, &SrcAddressPa);
        SrcAddress = (EFI_MAC_ADDRESS *)(ULONG_PTR)SrcAddressPa.QuadPart;
        BlMmTranslateVirtualAddress(DestAddress, &DestAddressPa);
        DestAddress = (EFI_MAC_ADDRESS *)(ULONG_PTR)DestAddressPa.QuadPart;
        BlMmTranslateVirtualAddress(Protocol, &ProtocolPa);
        Protocol = (UINT16 *)(ULONG_PTR)ProtocolPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Transmit packet.
    //

    EfiStatus = Snp->Transmit(Snp,
                              HeaderSize,
                              BufferSize,
                              Buffer,
                              SrcAddress,
                              DestAddress,
                              Protocol);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    Status = EfiGetNtStatusCode(EfiStatus);
    if (!NT_SUCCESS(Status)) {
        goto TransmitPacketEnd;
    }

    if (AsyncSend == FALSE) {
        Status = EfiSnpWaitForTransmitDone(SnpVa, BufferVa);
    }

TransmitPacketEnd:
    return Status;
}

NTSTATUS
EfiSnpWaitForTransmitDone (
    __in EFI_SIMPLE_NETWORK *Snp,
    __in PVOID Packet
    )

/*++

Routine Description:

    This function waits for the packet to be transmitted over the network.

Arguments:

    Snp - Supplies pointer to EFI_SIMPLE_NETWORK structure.

    Packet - Supplies the pointer to the packet that has been sent over the
        network.

Return Value:

    STATUS_SUCCESS if the stop operation completed successfully.

    STATUS_INVALID_PARAMETER if the parameter is invalid.

--*/

{

    PHYSICAL_ADDRESS PacketPa = {0};
    NTSTATUS Status;
    ULONG Time0;
    BOOLEAN TransmitDone;
    PVOID TxBuf;

    TransmitDone = FALSE;
    Status = STATUS_SUCCESS;

    //
    // When a buffer is transmitted using EfiSnpTransmitPacket, the address of
    // the buffer is converted from virtual to physical (if the application is
    // running in the application's context).
    //
    // When this function is called, caller will specify the virtual address of
    // the buffer that was trasmitted and it must be converted to physical
    // address because that is the address SNP protocol will return.
    //

    if (BlpArchQueryCurrentContextType() != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(Packet, &PacketPa);
        Packet = (EFI_SIMPLE_NETWORK *)(ULONG_PTR)PacketPa.QuadPart;
    }

    Time0 = BlTimeGetRelativeTime();
    while((BlTimeGetRelativeTime() - Time0) < 2) {
        Status = EfiSnpGetStatus(Snp, NULL, &TxBuf);
        if (!NT_SUCCESS(Status)) {
            goto WaitForTransmitDoneEnd;
        }

        if (TxBuf == Packet) {
            TransmitDone = TRUE;
            break;
        }
    }

    if (TransmitDone == FALSE) {
        Status = STATUS_TIMEOUT;
        goto WaitForTransmitDoneEnd;
    }

WaitForTransmitDoneEnd:
    return Status;
}

NTSTATUS
EfiPxeGetBinlReplyPacket (
    __in EFI_PXE_BASE_CODE *PxeClient,
    __inout EFI_PXE_BASE_CODE_DHCPV4_PACKET *Packet
    )

/*++

Routine Description:

    This routine is a wrapper for retrieving a PXE reply packet from the PXE
    base code protocol.  This packet may be either an OFFER packet from a proxy
    during the discovery process, or it may be an ACK from the PXE server
    itself.

    For compatibility with prior versions, the OFFER is used if it is
    available, and failing that the ACK is used.  Prior versions always used
    the OFFER and ignored the ACK.

Arguments:

    PxeClient - Supplies a pointer to the PXE base code protocol interface.

    Packet - Supplies a pointer to a buffer that will receive the BINL reply
        packet.

Returns:

    STATUS_NOT_FOUND indicates there is no PXE reply packet available (neither
    the proxy's OFFER nor the PXE server's ACK).

    STATUS_SUCCESS indicates the packet was retrieved successfully.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_PXE_BASE_CODE_DHCPV4_PACKET *BinlReplyPacket;
    PHYSICAL_ADDRESS PacketPa = {0};
    PHYSICAL_ADDRESS PxeClientPa = {0};
    NTSTATUS Status;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(PxeClient, &PxeClientPa);
        PxeClient = (PVOID)(ULONG_PTR)PxeClientPa.QuadPart;
        BlMmTranslateVirtualAddress(Packet, &PacketPa);
        Packet = (PDHCP_PACKET)(ULONG_PTR)PacketPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Determine if the ProxyOffer packet is available.  For compatibility with
    // downlevel boot programs, the ProxyOffer packet is considered first.
    //

    BinlReplyPacket = NULL;
    if (PxeClient->Mode->ProxyOfferReceived != FALSE) {
        BinlReplyPacket = &PxeClient->Mode->ProxyOffer.Dhcpv4;
    }

    //
    // Otherwise, if the PxeReply packet is available, this is used instead.
    // It is possible to get a PxeReply without a ProxyOffer in DHCPv4 PXE when
    // Option 43 is used to control server discovery.
    //

    if (BinlReplyPacket == NULL &&
        PxeClient->Mode->PxeReplyReceived != FALSE) {

        BinlReplyPacket = &PxeClient->Mode->PxeReply.Dhcpv4;
    }

    //
    // Copy the reply packet to caller specified buffer.
    //

    if (BinlReplyPacket != NULL) {
        RtlCopyMemory(Packet, BinlReplyPacket, sizeof(PXE_PACKET));
        Status = STATUS_SUCCESS;

    } else {
        Status = STATUS_NOT_FOUND;
    }


    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return Status;
}

NTSTATUS
EfiPxeGetDhcpAckPacket (
    __in EFI_PXE_BASE_CODE *PxeClient,
    __inout EFI_PXE_BASE_CODE_DHCPV4_PACKET *Packet
    )

/*++

Routine Description:

    This routine is a wrapper for retrieving a DHCP ACK packet from the PXE base
    code protocol.

Arguments:

    PxeClient - Supplies a pointer to the PXE base code protocol interface.

    Packet - Supplies a pointer to a buffer that will receive the DHCP ACK
        packet.

Returns:

    STATUS_NOT_FOUND indicates there is no DHCP ACK packet available.

    STATUS_SUCCESS indicates the packet was retrieved successfully.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS PacketPa = {0};
    PHYSICAL_ADDRESS PxeClientPa = {0};
    NTSTATUS Status;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(PxeClient, &PxeClientPa);
        PxeClient = (PVOID)(ULONG_PTR)PxeClientPa.QuadPart;
        BlMmTranslateVirtualAddress(Packet, &PacketPa);
        Packet = (PDHCP_PACKET)(ULONG_PTR)PacketPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    if (PxeClient->Mode->DhcpAckReceived != FALSE) {
        RtlCopyMemory(Packet, &PxeClient->Mode->DhcpAck, sizeof(PXE_PACKET));
        Status = STATUS_SUCCESS;

    } else {
        Status = STATUS_NOT_FOUND;
    }

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return Status;
}

NTSTATUS
EfiPxeGetDhcpDiscoverPacket (
    __in EFI_PXE_BASE_CODE *PxeClient,
    __inout EFI_PXE_BASE_CODE_DHCPV4_PACKET *Packet
    )

/*++

Routine Description:

    This routine is a wrapper for retreiving the DHCP discover packet from the
    PXE base code protocol.

Arguments:

    PxeClient - Supplies a pointer to the PXE base code protocol interface.

    Packet - Supplies a pointer to a buffer that will receive the DHCP discover
        packet.

Returns:

    STATUS_NOT_FOUND indicates there is no DHCP ACK packet available.

    STATUS_SUCCESS indicates the packet was retrieved successfully.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS PacketPa;
    PHYSICAL_ADDRESS PxeClientPa = {0};
    NTSTATUS Status;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(PxeClient, &PxeClientPa);
        PxeClient = (PVOID)(ULONG_PTR)PxeClientPa.QuadPart;
        BlMmTranslateVirtualAddress(Packet, &PacketPa);
        Packet = (PDHCP_PACKET)(ULONG_PTR)PacketPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    if (PxeClient->Mode->DhcpDiscoverValid != FALSE) {
        RtlCopyMemory(Packet, &PxeClient->Mode->DhcpDiscover, sizeof(PXE_PACKET));
        Status = STATUS_SUCCESS;

    } else {
        Status = STATUS_NOT_FOUND;
    }

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return Status;
}

NTSTATUS
EfiPxeGetDhcpv6PxeReplyPacket (
    __in EFI_PXE_BASE_CODE *PxeClient,
    __inout EFI_PXE_BASE_CODE_PACKET *Packet
    )

/*++

Routine Description:

    This routine is a wrapper for retrieving the DHCPv6 ADVERTISE or DHCPv6
    REPLY packet from the proxy (PXE) server that responded.

    For compatibility with downlevel boot programs, if the ADVERTISE from the
    PXE discovery process is available, this is used first.  If this is not
    present, the REPLY is returned instead.

Arguments:

    PxeClient - Supplies a pointer to the PXE base code protocol interface.

    Packet - Supplies a pointer to a buffer that will receive the DHCPv6
        ADVERTISE or REPLY packet.

Returns:

    STATUS_NOT_FOUND indicates there is no DHCP ACK packet available.

    STATUS_SUCCESS indicates the packet was retrieved successfully.

--*/

{

    //
    // Stored in the same field as the DHCPv4 proxy OFFER.
    //

    return EfiPxeGetBinlReplyPacket(PxeClient, &Packet->Dhcpv4);
}

NTSTATUS
EfiPxeGetDhcpv6ReplyPacket (
    __in EFI_PXE_BASE_CODE *PxeClient,
    __inout EFI_PXE_BASE_CODE_PACKET *Packet
    )

/*++

Routine Description:

    This routine is a wrapper for retrieving a DHCPv6 REPLY packet from the
    PXE base code protocol.

Arguments:

    PxeClient - Supplies a pointer to the PXE base code protocol interface.

    Packet - Supplies a pointer to a buffer that will receive the DHCPv6
        REPLY packet.

Returns:

    STATUS_NOT_FOUND indicates there is no DHCP ACK packet available.

    STATUS_SUCCESS indicates the packet was retrieved successfully.

--*/

{

    //
    // Stored in the same field as the DHCPv4 ACK.
    //

    return EfiPxeGetDhcpAckPacket(PxeClient, &Packet->Dhcpv4);
}

NTSTATUS
EfiPxeGetDhcpv6SolicitPacket (
    __in EFI_PXE_BASE_CODE *PxeClient,
    __inout EFI_PXE_BASE_CODE_PACKET *Packet
    )

/*++

Routine Description:

    This routine is a wrapper for retreiving the DHCPv6 SOLICIT packet from the
    PXE base code protocol.

Arguments:

    PxeClient - Supplies a pointer to the PXE base code protocol interface.

    Packet - Supplies a pointer to a buffer that will receive the DHCPv6
        SOLICIT packet.

Returns:

    STATUS_NOT_FOUND indicates there is no DHCP ACK packet available.

    STATUS_SUCCESS indicates the packet was retrieved successfully.

--*/

{

    //
    // Stored in the same field as the DHCPv4 DISCOVER.
    //

    return EfiPxeGetDhcpDiscoverPacket(PxeClient, &Packet->Dhcpv4);
}

NTSTATUS
EfiPxeSetPackets (
    __in EFI_PXE_BASE_CODE *PxeClient,
    __in_opt BOOLEAN *NewDhcpDiscoverValid,
    __in_opt BOOLEAN *NewDhcpAckReceived,
    __in_opt BOOLEAN *NewProxyOfferReceived,
    __in_opt BOOLEAN *NewPxeDiscoverValid,
    __in_opt BOOLEAN *NewPxeReplyReceived,
    __in_opt BOOLEAN *NewPxeBisReplyReceived,
    __in_opt EFI_PXE_BASE_CODE_PACKET *NewDhcpDiscover,
    __in_opt EFI_PXE_BASE_CODE_PACKET *NewDhcpAck,
    __in_opt EFI_PXE_BASE_CODE_PACKET *NewProxyOffer,
    __in_opt EFI_PXE_BASE_CODE_PACKET *NewPxeDiscover,
    __in_opt EFI_PXE_BASE_CODE_PACKET *NewPxeReply,
    __in_opt EFI_PXE_BASE_CODE_PACKET *NewPxeBisReply
    )

/*++

Routine Description:

    This routine is a wrapper for updating packets that are cached by the
    firmware.

Arguments:

    PxeClient - Supplies a pointer to the PXE client code base.

    NewDhcpDiscoverValid - Supplies a pointer to a value that specifies whether
        to replace the current value of DhcpDiscoverValid field. If NULL, this
        parameter is ignored.

    NewDhcpAckReceived - Supplies a pointer to a value that specifies whether to
        replace the current value of DhcpAckReceived field. If NULL, this
        parameter is ignored.

    NewProxyOfferReceived - Supplies a pointer to a value that specifies whether
        to replace the current value of ProxyOfferReceived field. If NULL, this
        parameter is ignored.

    NewPxeDiscoverValid -  Supplies a pointer to a value that specifies whether
        to replace the current value of PxeDiscoverValid field. If NULL, this
        parameter is ignored.

    NewPxeReplyReceived - Supplies a pointer to a value that specifies whether
        to replace the current value of PxeReplyReceived field. If NULL, this
        parameter is ignored.

    NewPxeBisReplyReceived - Supplies a pointer to a value that specifies
        whether to replace the current value of PxeBisReplyReceived field. If
        NULL, this parameter is ignored.

    NewDhcpDiscover - Supplies pointer to the new cached DHCP Discover packet.

    NewDhcpAck - Supplies pointer to the new cached DHCP Ack packet.

    NewProxyOffer - Supplies pointer to the new cached Proxy Offer packet.

    NewPxeDiscover - Supplies pointer to the new cached PXE Discover packet.

    NewPxeReply - Supplies pointer to the new cached PXE Reply packet.

    NewPxeBisReply - Supplies pointer to the new cached PXE BIS Reply packet.

Return Value:

    STATUS_SUCCESS if the stop operation completed successfully.

    STATUS_INVALID_PARAMETER if the parameter is invalid.

    STATUS_DEVICE_NOT_READY if the network device was not started.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS NewDhcpAckPa;
    PHYSICAL_ADDRESS NewDhcpAckReceivedPa;
    PHYSICAL_ADDRESS NewDhcpDiscoverPa;
    PHYSICAL_ADDRESS NewDhcpDiscoverValidPa;
    PHYSICAL_ADDRESS NewProxyOfferPa;
    PHYSICAL_ADDRESS NewProxyOfferReceivedPa;
    PHYSICAL_ADDRESS NewPxeBisReplyPa;
    PHYSICAL_ADDRESS NewPxeBisReplyReceivedPa;
    PHYSICAL_ADDRESS NewPxeDiscoverPa;
    PHYSICAL_ADDRESS NewPxeDiscoverValidPa;
    PHYSICAL_ADDRESS NewPxeReplyPa;
    PHYSICAL_ADDRESS NewPxeReplyReceivedPa;
    PHYSICAL_ADDRESS PxeClientPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //
    // N.B. PXE client code base pointer is a physical address.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(PxeClient, &PxeClientPa);
        PxeClient = (EFI_PXE_BASE_CODE *)(ULONG_PTR)PxeClientPa.QuadPart;
        if (ARGUMENT_PRESENT(NewDhcpDiscoverValid) != FALSE) {
            BlMmTranslateVirtualAddress(NewDhcpDiscoverValid,
                                        &NewDhcpDiscoverValidPa);

            NewDhcpDiscoverValid =
                (BOOLEAN *)(ULONG_PTR)NewDhcpDiscoverValidPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewDhcpAckReceived) != FALSE) {
            BlMmTranslateVirtualAddress(NewDhcpAckReceived,
                                        &NewDhcpAckReceivedPa);
            NewDhcpAckReceived =
                (BOOLEAN *)(ULONG_PTR)NewDhcpAckReceivedPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewProxyOfferReceived) != FALSE) {
            BlMmTranslateVirtualAddress(NewProxyOfferReceived,
                                        &NewProxyOfferReceivedPa);

            NewProxyOfferReceived =
                (BOOLEAN *)(ULONG_PTR)NewProxyOfferReceivedPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewPxeDiscoverValid) != FALSE) {
            BlMmTranslateVirtualAddress(NewPxeDiscoverValid,
                                        &NewPxeDiscoverValidPa);
            NewPxeDiscoverValid =
                (BOOLEAN *)(ULONG_PTR)NewPxeDiscoverValidPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewPxeReplyReceived) != FALSE) {
            BlMmTranslateVirtualAddress(NewPxeReplyReceived,
                                        &NewPxeReplyReceivedPa);

            NewPxeReplyReceived =
                (BOOLEAN *)(ULONG_PTR)NewPxeReplyReceivedPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewPxeBisReplyReceived) != FALSE) {
            BlMmTranslateVirtualAddress(NewPxeBisReplyReceived,
                                        &NewPxeBisReplyReceivedPa);

            NewPxeBisReplyReceived =
                (BOOLEAN *)(ULONG_PTR)NewPxeBisReplyReceivedPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewDhcpDiscover) != FALSE) {
            BlMmTranslateVirtualAddress(NewDhcpDiscover, &NewDhcpDiscoverPa);
            NewDhcpDiscover = (EFI_PXE_BASE_CODE_PACKET *)
                              (ULONG_PTR)NewDhcpDiscoverPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewDhcpAck) != FALSE) {
            BlMmTranslateVirtualAddress(NewDhcpAck, &NewDhcpAckPa);
            NewDhcpAck =
                (EFI_PXE_BASE_CODE_PACKET *)(ULONG_PTR)NewDhcpAckPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewProxyOffer) != FALSE) {
            BlMmTranslateVirtualAddress(NewProxyOffer, &NewProxyOfferPa);
            NewProxyOffer =
                (EFI_PXE_BASE_CODE_PACKET *)(ULONG_PTR)NewProxyOfferPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewPxeDiscover) != FALSE) {
            BlMmTranslateVirtualAddress(NewPxeDiscover, &NewPxeDiscoverPa);
            NewPxeDiscover = (EFI_PXE_BASE_CODE_PACKET *)
                             (ULONG_PTR)NewPxeDiscoverPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewPxeReply) != FALSE) {
            BlMmTranslateVirtualAddress(NewPxeReply, &NewPxeReplyPa);
            NewPxeReply =
                (EFI_PXE_BASE_CODE_PACKET *)(ULONG_PTR)NewPxeReplyPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewPxeBisReply) != FALSE) {
            BlMmTranslateVirtualAddress(NewPxeBisReply, &NewPxeBisReplyPa);
            NewPxeBisReply = (EFI_PXE_BASE_CODE_PACKET *)
                             (ULONG_PTR)NewPxeBisReplyPa.QuadPart;
        }

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = PxeClient->SetPackets(PxeClient,
                                      NewDhcpDiscoverValid,
                                      NewDhcpAckReceived,
                                      NewProxyOfferReceived,
                                      NewPxeDiscoverValid,
                                      NewPxeReplyReceived,
                                      NewPxeBisReplyReceived,
                                      NewDhcpDiscover,
                                      NewDhcpAck,
                                      NewProxyOffer,
                                      NewPxeDiscover,
                                      NewPxeReply,
                                      NewPxeBisReply);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiPxeSetParameters (
    __in EFI_PXE_BASE_CODE *PxeClient,
    __in_opt BOOLEAN *NewAutoArp,
    __in_opt BOOLEAN *NewSendGUID,
    __in_opt UINT8 *NewTTL,
    __in_opt UINT8 *NewToS,
    __in_opt BOOLEAN *NewMakeCallback
    )

/*++

Routine Description:

    This routine is a wrapper for updating parameters that affect the operation
    of the PXE Base Code Protocol.

Arguments:

    PxeClient - Supplies a pointer to the PXE client code base.

    NewAutoArp - Supplies a pointer to a value that specifies whether
        to replace the current value of AutoArp field. If NULL, this
        parameter is ignored.

    NewSendGUID - Supplies a pointer to a value that specified whether to
        replace the current value of SendGUID. If NULL, this parameter is
        ignored.

    NewTTL - Supplies a pointer to a value containing tew TTL value. If NULL,
        this parameter is ignored.

    NewToS - Supplies a pointer to a value containing new ToS value. If NULL,
        this parameter is ignored.

    NewMakeCallback - Supplies a pointer to a value that specified whether to
        replace the current value of MakeCallback field of the Mode structure.
        If NULL, this parameter is ignored.

Return Value:

    STATUS_SUCCESS if the stop operation completed successfully.

    STATUS_INVALID_PARAMETER if the parameter is invalid.

    STATUS_DEVICE_NOT_READY if the network device was not started.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS NewAutoArpPa;
    PHYSICAL_ADDRESS NewMakeCallbackPa;
    PHYSICAL_ADDRESS NewSendGUIDPa;
    PHYSICAL_ADDRESS NewTTLPa;
    PHYSICAL_ADDRESS NewToSPa;
    PHYSICAL_ADDRESS PxeClientPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //
    // N.B. PXE client code base pointer is a physical address.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(PxeClient, &PxeClientPa);
        PxeClient = (EFI_PXE_BASE_CODE *)(ULONG_PTR)PxeClientPa.QuadPart;
        if (ARGUMENT_PRESENT(NewAutoArp) != FALSE) {
            BlMmTranslateVirtualAddress(NewAutoArp,
                                        &NewAutoArpPa);

            NewAutoArp =
                (BOOLEAN *)(ULONG_PTR)NewAutoArpPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewSendGUID) != FALSE) {
            BlMmTranslateVirtualAddress(NewSendGUID,
                                        &NewSendGUIDPa);

            NewSendGUID =
                (BOOLEAN *)(ULONG_PTR)NewSendGUIDPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewTTL) != FALSE) {
            BlMmTranslateVirtualAddress(NewTTL,
                                        &NewTTLPa);

            NewTTL =
                (UINT8 *)(ULONG_PTR)NewTTLPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewToS) != FALSE) {
            BlMmTranslateVirtualAddress(NewToS,
                                        &NewToSPa);

            NewToS =
                (UINT8 *)(ULONG_PTR)NewToSPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(NewMakeCallback) != FALSE) {
            BlMmTranslateVirtualAddress(NewMakeCallback,
                                        &NewMakeCallbackPa);

            NewMakeCallback =
                (BOOLEAN *)(ULONG_PTR)NewMakeCallbackPa.QuadPart;
        }

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = PxeClient->SetParameters(PxeClient,
                                         NewAutoArp,
                                         NewSendGUID,
                                         NewTTL,
                                         NewToS,
                                         NewMakeCallback);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiPxeStop (
    __in EFI_PXE_BASE_CODE *PxeClient
    )

/*++

Routine Description:

    This routine is a wrapper for stopping the network device and thereby
    disabling the use of PXE Base Code Protocol functions.

Arguments:

    PxeClient - Supplies a pointer to the PXE client code base.

Return Value:

    STATUS_SUCCESS if the stop operation completed successfully.

    STATUS_INVALID_PARAMETER if the parameter is invalid.

    STATUS_IO_DEVICE_ERROR if the network device encountered an error during
        stop.

    STATUS_DEVICE_NOT_READY if the network device was not started.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS PxeClientPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(PxeClient, &PxeClientPa);
        PxeClient = (EFI_PXE_BASE_CODE *)(ULONG_PTR)PxeClientPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = PxeClient->Stop(PxeClient);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiUdpRead (
    __in EFI_PXE_BASE_CODE *PxeClient,
    __in UINT16 Flags,
    __inout_opt EFI_IP_ADDRESS *DestinationIP,
    __inout_opt EFI_PXE_BASE_CODE_UDP_PORT *DestinationPort,
    __inout_opt EFI_IP_ADDRESS *SourceIP,
    __inout_opt EFI_PXE_BASE_CODE_UDP_PORT *SourcePort,
    __in_opt UINTN *HeaderSize,
    __in_bcount_opt(*HeaderSize) PVOID HeaderAddress,
    __inout UINTN *BufferSize,
    __in_bcount(*BufferSize) PVOID Buffer
    )

/*++

Routine Description:

    This routine reads a UDP packet from the network interface.

Arguments:

    PxeClient - Supplies a pointer to the PXE client code base.

    Flags - Supplies the UDP operational flags.

    DestinationIP - Supplies the destination IP address for the packet.

    DestinationPort - Supplies the destination UDP port number for the packet.

    GatewayIP - Supplies the IP address for the gateway. This parameter is
        only required if the source and destination reside in different IP
        subnet.

    SourceIP - Supplies the source IP address for the packet.

    SourcePort - Supplies the source UDP port number for the packet.

    HeaderSize - Supplies a pointer to a variable that contains the size
        (in bytes) of the header to be put in the header buffer.

    HeaderAddress - Supplies a pointer to a buffer that receives the
        specified number of bytes following the UDP header.

    BufferSize - Supplies a pointer to a variable that contains the size
        (in bytes) of buffer on input. On output, receives the number of bytes
        written to the data buffer.

    Buffer - Supplies a pointer to a buffer that receives the data read.

Return Value:

    STATUS_SUCCESS if the UDP write operation completed successfully.

    STATUS_DEVICE_NOT_READY if the PXE Base Code protocol driver is in stopped
        state.

    STATUS_INVALID_PARAMETER if an invalid parameter is supplied.

    STATUS_IO_DEVICE_ERROR if the network device encountered an error.

    STATUS_BUFFER_TOO_SMALL if the buffer is too small.

    STATUS_REQUEST_ABORTED if the write operation was aborted.

    STATUS_TIMEOUT if the UDP write operation timed out.

    STATUS_UNSUCCESSFUL otherwise.

--*/

{

    PHYSICAL_ADDRESS BufferPa;
    PHYSICAL_ADDRESS BufferSizePa;
    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS DestinationIPPa;
    PHYSICAL_ADDRESS DestinationPortPa;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS HeaderAddressPa;
    PHYSICAL_ADDRESS HeaderSizePa;
    PHYSICAL_ADDRESS PxeClientPa = {0};
    PHYSICAL_ADDRESS SourceIPPa;
    PHYSICAL_ADDRESS SourcePortPa;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //
    // N.B. PXE client code base pointer is a physical address.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        if (ARGUMENT_PRESENT(DestinationIP) != FALSE) {
            BlMmTranslateVirtualAddress(DestinationIP, &DestinationIPPa);
            DestinationIP =
                (EFI_IP_ADDRESS *)(ULONG_PTR)DestinationIPPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(DestinationPort) != FALSE) {
            BlMmTranslateVirtualAddress(DestinationPort, &DestinationPortPa);
            DestinationPort = (EFI_PXE_BASE_CODE_UDP_PORT *)
                              (ULONG_PTR)DestinationPortPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(SourceIP) != FALSE) {
            BlMmTranslateVirtualAddress(SourceIP, &SourceIPPa);
            SourceIP = (EFI_IP_ADDRESS *)(ULONG_PTR)SourceIPPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(SourcePort) != FALSE) {
            BlMmTranslateVirtualAddress(SourcePort, &SourcePortPa);
            SourcePort =
                (EFI_PXE_BASE_CODE_UDP_PORT *)(ULONG_PTR)SourcePortPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(HeaderSize) != FALSE) {
            BlMmTranslateVirtualAddress(HeaderSize, &HeaderSizePa);
            HeaderSize = (UINTN *)(ULONG_PTR)HeaderSizePa.QuadPart;
        }

        if (ARGUMENT_PRESENT(HeaderAddress) != FALSE) {
            BlMmTranslateVirtualAddress(HeaderAddress, &HeaderAddressPa);
            HeaderAddress = (PVOID)(ULONG_PTR)HeaderAddressPa.QuadPart;
        }

        BlMmTranslateVirtualAddress(BufferSize, &BufferSizePa);
        BufferSize = (UINTN *)(ULONG_PTR)BufferSizePa.QuadPart;
        BlMmTranslateVirtualAddress(Buffer, &BufferPa);
        Buffer = (PVOID)(ULONG_PTR)BufferPa.QuadPart;
        BlMmTranslateVirtualAddress(PxeClient, &PxeClientPa);
        PxeClient = (EFI_PXE_BASE_CODE *)(ULONG_PTR)PxeClientPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Receive the UDP packet from the network interface.
    //

    EfiStatus = PxeClient->UdpRead(PxeClient,
                                   Flags,
                                   DestinationIP,
                                   DestinationPort,
                                   SourceIP,
                                   SourcePort,
                                   HeaderSize,
                                   HeaderAddress,
                                   BufferSize,
                                   Buffer);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiUdpSetIpFilter (
    __in EFI_PXE_BASE_CODE *PxeClient,
    __in EFI_PXE_BASE_CODE_IP_FILTER *Filter
    )

/*++

Routine Description:

    This routine updates the software filter for IP address that can be
    received.

Arguments:

    PxeClient - Supplies a pointer to the PXE client code base.

    Filter - Supplies the IP filter to set on device.

Return Value:

    STATUS_SUCCESS if the UDP write operation completed successfully.

    STATUS_DEVICE_NOT_READY if the PXE Base Code protocol driver is in stopped
        state.

    STATUS_INVALID_PARAMETER if an invalid parameter is supplied.

    STATUS_UNSUCCESSFUL otherwise.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS FilterPa;
    PHYSICAL_ADDRESS PxeClientPa;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(PxeClient, &PxeClientPa);
        PxeClient = (EFI_PXE_BASE_CODE *)(ULONG_PTR)PxeClientPa.QuadPart;
        BlMmTranslateVirtualAddress(Filter, &FilterPa);
        Filter = (EFI_PXE_BASE_CODE_IP_FILTER *)(ULONG_PTR)FilterPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = PxeClient->SetIpFilter(PxeClient, Filter);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiUdpWrite (
    __in EFI_PXE_BASE_CODE *PxeClient,
    __in UINT16 Flags,
    __in EFI_IP_ADDRESS *DestinationIP,
    __in EFI_PXE_BASE_CODE_UDP_PORT *DestinationPort,
    __in_opt EFI_IP_ADDRESS *GatewayIP,
    __in_opt EFI_IP_ADDRESS *SourceIP,
    __inout_opt EFI_PXE_BASE_CODE_UDP_PORT *SourcePort,
    __in_opt UINTN *HeaderSize,
    __in_bcount_opt(*HeaderSize) PVOID HeaderAddress,
    __in UINTN *BufferSize,
    __in_bcount(*BufferSize) PVOID Buffer
    )

/*++

Routine Description:

    This routine writes a UDP packet to the network interface.

Arguments:

    PxeClient - Supplies a pointer to the PXE client code base.

    Flags - Supplies the UDP operational flags.

    DestinationIP - Supplies the destination IP address for the packet.

    DestinationPort - Supplies the destination UDP port number for the packet.

    GatewayIP - Supplies the IP address for the gateway. This parameter is
        only required if the source and destination reside in different IP
        subnet.

    SourceIP - Supplies the source IP address for the packet.

    SourcePort - Supplies the source UDP port number for the packet.

    HeaderSize - Supplies a pointer to a variable that contains the size
        (in bytes) of the UDP header.

    HeaderAddress - Supplies a pointer to a buffer that contains the header
        to be prepended to the data buffer.

    BufferSize - Supplies a pointer to a variable that contains the size
        (in bytes) of the UDP data packet.

    Buffer - Supplies a pointer to the data to be written.

Return Value:

    STATUS_SUCCESS if the UDP write operation completed successfully.

    STATUS_DEVICE_NOT_READY if the PXE Base Code protocol driver is in stopped
        state.

    STATUS_INVALID_PARAMETER if an invalid parameter is supplied.

    STATUS_IO_DEVICE_ERROR if the network device encountered an error.

    STATUS_INVALID_BUFFER_SIZE if the buffer to send is too large.

    STATUS_REQUEST_ABORTED if the write operation was aborted.

    STATUS_TIMEOUT if the UDP write operation timed out.

    STATUS_UNSUCCESSFUL otherwise.

--*/

{

    PHYSICAL_ADDRESS BufferPa;
    PHYSICAL_ADDRESS BufferSizePa;
    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS DestinationIPPa;
    PHYSICAL_ADDRESS DestinationPortPa;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS GatewayIPPa;
    PHYSICAL_ADDRESS HeaderAddressPa;
    PHYSICAL_ADDRESS HeaderSizePa;
    PHYSICAL_ADDRESS PxeClientPa;
    PHYSICAL_ADDRESS SourceIPPa;
    PHYSICAL_ADDRESS SourcePortPa;

    //
    // Validate the supplied parameters.
    //

    if ((PxeClient == NULL) ||
        (DestinationIP == NULL) ||
        (DestinationPort == NULL) ||
        (BufferSize == NULL) ||
        (Buffer == NULL)) {

        return STATUS_INVALID_PARAMETER;
    }

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //
    // N.B. PXE client code base pointer is a physical address.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(DestinationIP, &DestinationIPPa);
        DestinationIP = (EFI_IP_ADDRESS *)(ULONG_PTR)DestinationIPPa.QuadPart;
        BlMmTranslateVirtualAddress(DestinationPort, &DestinationPortPa);
        DestinationPort =
            (EFI_PXE_BASE_CODE_UDP_PORT *)(ULONG_PTR)DestinationPortPa.QuadPart;

        if (ARGUMENT_PRESENT(GatewayIP) != FALSE) {
            BlMmTranslateVirtualAddress(GatewayIP, &GatewayIPPa);
            GatewayIP = (EFI_IP_ADDRESS *)(ULONG_PTR)GatewayIPPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(SourceIP) != FALSE) {
            BlMmTranslateVirtualAddress(SourceIP, &SourceIPPa);
            SourceIP = (EFI_IP_ADDRESS *)(ULONG_PTR)SourceIPPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(SourcePort) != FALSE) {
            BlMmTranslateVirtualAddress(SourcePort, &SourcePortPa);
            SourcePort =
                (EFI_PXE_BASE_CODE_UDP_PORT *)(ULONG_PTR)SourcePortPa.QuadPart;
        }

        if (ARGUMENT_PRESENT(HeaderSize) != FALSE) {
            BlMmTranslateVirtualAddress(HeaderSize, &HeaderSizePa);
            HeaderSize = (UINTN *)(ULONG_PTR)HeaderSizePa.QuadPart;
        }

        if (ARGUMENT_PRESENT(HeaderAddress) != FALSE) {
            BlMmTranslateVirtualAddress(HeaderAddress, &HeaderAddressPa);
            HeaderAddress = (PVOID)(ULONG_PTR)HeaderAddressPa.QuadPart;
        }

        BlMmTranslateVirtualAddress(BufferSize, &BufferSizePa);
        BufferSize = (UINTN *)(ULONG_PTR)BufferSizePa.QuadPart;
        BlMmTranslateVirtualAddress(Buffer, &BufferPa);
        Buffer = (PVOID)(ULONG_PTR)BufferPa.QuadPart;
        BlMmTranslateVirtualAddress(PxeClient, &PxeClientPa);
        PxeClient = (EFI_PXE_BASE_CODE *)(ULONG_PTR)PxeClientPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Send the UDP packet over the network interface.
    //

    EfiStatus = PxeClient->UdpWrite(PxeClient,
                                    Flags,
                                    DestinationIP,
                                    DestinationPort,
                                    GatewayIP,
                                    SourceIP,
                                    SourcePort,
                                    HeaderSize,
                                    HeaderAddress,
                                    BufferSize,
                                    Buffer);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}


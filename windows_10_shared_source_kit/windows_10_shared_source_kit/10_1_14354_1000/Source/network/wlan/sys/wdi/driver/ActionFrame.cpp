/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    ActionFrame.cpp

Abstract:

    Implementation for Action Frame functions

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "ActionFrame.tmh"


NDIS_STATUS CSendActionFrameRequestJob::Initialize(
    _In_ CAdapter* pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber
    )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    BOOLEAN actionFramesSupported = FALSE;

    // Check if action frames are supported
    status = pAdapter->GetAdapterPropertyCache()->GetPropertyBoolean(WfcAdapterPropertyActionFramesSupported, &actionFramesSupported);
    if (NDIS_STATUS_SUCCESS != status)
    {
        WFCActError("Unable to query property ActionFramesSupported, %d\n", status);
        goto Exit;
    }

    if (!actionFramesSupported)
    {
        status = NDIS_STATUS_NOT_SUPPORTED;
        WFCActError("Action frames not supported.");
        goto Exit;
    }

    if (m_pTlvBuffer != NULL)
    {
        delete[] m_pTlvBuffer;
        m_pTlvBuffer = NULL;
    }
    m_TlvBufferLength = 0;

    if (!m_Initialized)
    {
        m_pAdapter = pAdapter;
        m_NdisPortNumber = NdisPortNumber;

        status = CJobBase::Initialize(
            m_pAdapter->GetEventQueue(),
            m_pAdapter->GetDeviceCommandScheduler(),
            m_pAdapter->GetActiveJobsList(),
            m_pAdapter->GetSerializedJobList(),
            WiFiJobSendActionFrameRequest,
            false,
            false,
            WiFiPriorityScopeAdapter,
            WiFiSerializedJobPriority_6,
            pAdapter,
            pAdapter,
            NdisPortNumber );

        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCActErrorStatus("Couldn't initialize CJobBase.");
            goto Exit;
        }

        m_Initialized = true;
    }

Exit:
    return (status);
}

NDIS_STATUS CSendActionFrameRequestJob::InitializePublicActionFrame(
    _In_ CAdapter *pAdapter,
    _In_ NDIS_PORT_NUMBER NdisPortNumber,
    _In_ DOT11_ACTION_FRAME_PUBLIC_ACTION_TYPE ActionType,
    _In_ UCHAR DialogToken,
    _In_ WDI_MAC_ADDRESS *pDestMacAddress,
    _In_ UINT32 BandId,
    _In_ UINT32 ChannelNumber,
    _In_ USHORT DataLength,
    _In_bytecount_(DataLength) PBYTE ucDataBuffer,
    _In_opt_ PANQP_ACTION_FRAME_PARAMETERS pANQPParams,      // For ANQP Action frames
    _In_opt_ BOOL bIsRequestTask
    )
{
    NDIS_STATUS                     status = NDIS_STATUS_SUCCESS;
    USHORT                          uActionFrameLength = 0;
    PBYTE                           pActionFrame = NULL;

    TraceActEntry();

    status = Initialize (pAdapter, NdisPortNumber);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("InitializeCommon failed <%d>", status);
        goto Exit;
    }

    switch (ActionType)
    {
        case (DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_INITIAL_REQUEST):
        case (DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_INITIAL_RESPONSE):
        case (DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_COMEBACK_REQUEST):
        case (DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_COMEBACK_RESPONSE):
        {
            status = GenerateANQPFrame (ActionType,
                                        DialogToken,
                                        pANQPParams,
                                        DataLength,
                                        ucDataBuffer,
                                        &uActionFrameLength,
                                        &pActionFrame);
            if (status != NDIS_STATUS_SUCCESS)
            {
                goto Exit;
            }

            break;
        }

        default:
        {
            status = NDIS_STATUS_INVALID_PARAMETER;
            WFCActError("Invalid Action frame type = %d", ActionType);
            goto Exit;
        }
    }

    // Remember if this is to be sent as a Request or Response Task
    m_IsRequestTask = bIsRequestTask;

    //
    // Fill in the parameters
    //
    if (m_IsRequestTask)
    {
        RtlZeroMemory (&m_RequestParameters, sizeof(m_RequestParameters));
        m_RequestParameters.RequestParams.ChannelNumber = ChannelNumber;
        m_RequestParameters.RequestParams.BandId = BandId;
        RtlCopyMemory (&m_RequestParameters.RequestParams.DestinationAddress, pDestMacAddress, sizeof(WDI_MAC_ADDRESS));
        status = m_RequestParameters.ActionFrameBody.SimpleSet (pActionFrame, uActionFrameLength, 0);
    }
    else
    {
        RtlZeroMemory (&m_ResponseParameters, sizeof(m_ResponseParameters));
        m_ResponseParameters.ResponseParams.ChannelNumber = ChannelNumber;
        m_ResponseParameters.ResponseParams.BandId = BandId;
        RtlCopyMemory (&m_ResponseParameters.ResponseParams.DestinationAddress,pDestMacAddress,sizeof(WDI_MAC_ADDRESS));
        status = m_ResponseParameters.ActionFrameBody.SimpleSet (pActionFrame, uActionFrameLength, 0);
    }

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to allocate memory for GAS protocol frame body.");
        goto Exit;
    }

Exit:
    if (pActionFrame)
    {
        delete [] pActionFrame;
    }

    TraceActExit(status);
    return (status);
}


NDIS_STATUS CSendActionFrameRequestJob::GenerateANQPFrame(
    _In_ DOT11_ACTION_FRAME_PUBLIC_ACTION_TYPE ActionType,
    _In_ UCHAR DialogToken,
    _In_ PANQP_ACTION_FRAME_PARAMETERS pANQPParams,
    _In_ USHORT DataLength,
    _In_bytecount_(DataLength) PBYTE ucDataBuffer,
    _Out_ USHORT *pActionFrameLength,
    _Outptr_result_bytebuffer_(*pActionFrameLength) PBYTE *ppActionFrame
    )
{
    NDIS_STATUS                         status = NDIS_STATUS_SUCCESS;
    DOT11_ANQP_ACTION_FRAME             ANQPHeader = {0};
    UCHAR                               AdvertisementProtocolID = 0;
    USHORT                              StatusCode = 0;
    DOT11_ANQP_FRAGMENT_ID              FragmentID = {0};
    USHORT                              ComebackDelay = 0;
    USHORT                              HeaderLength;
    DOT11_AVERTISEMENT_PROTOCOL_ELEMENT *pAdvertisementProtocolElement = NULL;
    PBYTE                               pActionFrame = NULL;

    //
    // Override default parameters if provided
    //
    if (pANQPParams)
    {
        AdvertisementProtocolID = pANQPParams->AdvertisementProtocolID;
        StatusCode = pANQPParams->StatusCode;
        FragmentID = pANQPParams->FragmentID;
        ComebackDelay = pANQPParams->ComebackDelay;
    }

    //
    // Fill in Action frame header
    //
    ANQPHeader.Category = WDI_ACTION_FRAME_CATEGORY_PUBLIC;
    ANQPHeader.Action = (UCHAR) ActionType;
    ANQPHeader.DialogToken = DialogToken;

    switch (ActionType)
    {
        case (DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_INITIAL_REQUEST):
        {
            ANQPHeader.InitialRequest.QueryRequestLength = HostToDot11Short (DataLength);

            pAdvertisementProtocolElement = &ANQPHeader.InitialRequest.AdvertisementProtocolElement;
            HeaderLength = FIELD_OFFSET(DOT11_ANQP_ACTION_FRAME, InitialRequest.InfoElements);

            break;
        }

        case (DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_INITIAL_RESPONSE):
        {
            ANQPHeader.InitialResponse.StatusCode = HostToDot11Short (StatusCode);
            ANQPHeader.InitialResponse.ComebackDelay = HostToDot11Short (ComebackDelay);
            ANQPHeader.InitialResponse.QueryResponseLength = HostToDot11Short (DataLength);

            pAdvertisementProtocolElement = &ANQPHeader.InitialResponse.AdvertisementProtocolElement;
            HeaderLength = FIELD_OFFSET(DOT11_ANQP_ACTION_FRAME, InitialResponse.InfoElements);

            break;
        }

        case (DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_COMEBACK_REQUEST):
        {
            HeaderLength = FIELD_OFFSET(DOT11_ANQP_ACTION_FRAME, InitialRequest);

            break;
        }

        case (DOT11_PUBLIC_ACTION_FRAME_TYPE_GAS_COMEBACK_RESPONSE):
        {
            ANQPHeader.ComebackResponse.StatusCode = HostToDot11Short (StatusCode);
            ANQPHeader.ComebackResponse.QueryResponseFragmentID = FragmentID;
            ANQPHeader.ComebackResponse.ComebackDelay = HostToDot11Short (ComebackDelay);
            ANQPHeader.ComebackResponse.QueryResponseLength = HostToDot11Short (DataLength);

            pAdvertisementProtocolElement = &ANQPHeader.ComebackResponse.AdvertisementProtocolElement;
            HeaderLength = FIELD_OFFSET(DOT11_ANQP_ACTION_FRAME, ComebackResponse.InfoElements);

            break;
        }

        default:
        {
            WFCActError("ERROR - Unknown ActionType = %d.", ActionType);
            goto Exit;
        }
    }

    if (pAdvertisementProtocolElement)
    {
        pAdvertisementProtocolElement->ElementID = DOT11_INFO_ELEMENT_ID_ADVERTISEMENT_PROTOCOL;
        pAdvertisementProtocolElement->Length = 2;
        pAdvertisementProtocolElement->AdvertisementProtocol[0].QueryResponseInfo = 0x0;
        pAdvertisementProtocolElement->AdvertisementProtocol[0].AdvertisementProtocolID = AdvertisementProtocolID;
    }

    //
    // Copy the header and body into the frame
    //
    pActionFrame = new UCHAR[HeaderLength + DataLength];
    if (!pActionFrame)
    {
        WFCActError("Couldn't allocate memory for ANQP Action frame buffer.");
        status = NDIS_STATUS_RESOURCES;
        goto Exit;
    }

    RtlCopyMemory (pActionFrame, &ANQPHeader, HeaderLength);
    if (DataLength)
    {
        RtlCopyMemory (pActionFrame+HeaderLength, ucDataBuffer, DataLength);
    }

    *pActionFrameLength = HeaderLength + DataLength;
    *ppActionFrame = pActionFrame;

Exit:
    return (status);
}

void CSendActionFrameRequestJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    status = SendActionFrameRequest();
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't send action frame request.");
        goto Exit;
    }

Exit:
    if ( status != NDIS_STATUS_SUCCESS )
    {
        // Just complete the job
        CompleteJob(status);
    }

    TraceActExit(status);
}


void CSendActionFrameRequestJob::OnJobStepCompleted(
    _In_ NDIS_STATUS OperationStatus,
    _In_ WFC_SENDER_OBJECT SenderObject)
{
    NDIS_STATUS taskStatus;

    TraceActEntry();

    UNREFERENCED_PARAMETER(SenderObject);

    if (NDIS_STATUS_SUCCESS == OperationStatus)
    {
        OperationStatus = CMessageHelper::GetStatusFromTaskOutput(
            &m_TaskCommand,
            &taskStatus );
        if ( OperationStatus == NDIS_STATUS_SUCCESS )
        {
            OperationStatus = taskStatus;
        }
    }

    CompleteJob(OperationStatus);

    TraceActExit(OperationStatus);
}

void CSendActionFrameRequestJob::OnJobCancelled()
{
    TraceActEntry();
    m_TaskCommand.CancelTask();
    TraceActExit(0);
}

NDIS_STATUS CSendActionFrameRequestJob::SendActionFrameRequest()
{
    NDIS_STATUS     status = NDIS_STATUS_SUCCESS;
    WFC_PORT_TYPE   PortType = WfcPortTypeUnspecified;
    ULONG           CommandId;

    TraceActEntry();

    status = m_pAdapter->GetPortTypeFromPortId(get_PortId(), &PortType);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("FAILed to get Port type from PortId=%d failed <%d>", get_PortId(), status);
        goto Exit;
    }

    //
    // Set the SendTimeout and Dwell times
    //
    if (m_IsRequestTask)
    {
        CommandId = WDI_TASK_SEND_REQUEST_ACTION_FRAME;
        m_RequestParameters.RequestParams.PostACKDwellTime = WFC_CONSTANT::DefaultActionFramePostACKDwellTime;
        m_RequestParameters.RequestParams.SendTimeout = PortType == WfcPortTypeExtSTA ?
            WFC_CONSTANT::DefaultActionFrameRequestSendTimeoutForSTA :
            WFC_CONSTANT::DefaultActionFrameRequestSendTimeoutForP2P;
        status = GenerateWdiTaskSendRequestActionFrame(&m_RequestParameters,
            (ULONG)sizeof(WFC_MESSAGE_METADATA),
            m_pAdapter->GetTlvContext(),
            &m_TlvBufferLength,
            &m_pTlvBuffer);
    }
    else
    {
        CommandId = WDI_TASK_SEND_RESPONSE_ACTION_FRAME;
        m_ResponseParameters.ResponseParams.PostACKDwellTime = WFC_CONSTANT::DefaultActionFramePostACKDwellTime;
        m_ResponseParameters.ResponseParams.SendTimeout = PortType == WfcPortTypeExtSTA ?
            WFC_CONSTANT::DefaultActionFrameResponseSendTimeoutForSTA :
            WFC_CONSTANT::DefaultActionFrameResponseSendTimeoutForP2P;
        status = GenerateWdiTaskSendResponseActionFrame(&m_ResponseParameters,
            (ULONG)sizeof(WFC_MESSAGE_METADATA),
            m_pAdapter->GetTlvContext(),
            &m_TlvBufferLength,
            &m_pTlvBuffer);
    }

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Failed to generate TLV for Send Action Frame %s task.",
            m_IsRequestTask ? "Request": "Response");
        goto Exit;
    }

    status = InitializeAndStartTask(
        &m_TaskCommand,
        CommandId,
        m_TlvBufferLength,
        m_pTlvBuffer,
        m_pAdapter->GetNotificationManager());

    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActErrorStatus("Couldn't start send action frame request.");
        goto Exit;
    }

    WFCActInfo("Waiting for action frame request completion.");

Exit:
    TraceActExit(status);
    return status;
}

NDIS_STATUS CSendNeighborReportRequestJob::InitializeNeighborReportRequest()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    PDOT11_NEIGHBOR_REPORT_FRAME pNeighborReportFrame = NULL;
    PUCHAR pFrameBuffer = NULL;
    ULONG frameLength = FIELD_OFFSET(DOT11_NEIGHBOR_REPORT_FRAME, InfoElements);

    TraceActEntry();

    //
    // Fill in action frame body
    //
    if (m_pSsid)
    {
        // Add size of optional SSID
        frameLength += sizeof(DOT11_INFO_ELEMENT);
        frameLength += m_pSsid->uSSIDLength;
    }

    pFrameBuffer = new UCHAR[frameLength];
    if (pFrameBuffer == NULL)
    {
        WFCActError("Couldn't allocate memory for neighbor report frame buffer.");
        status = NDIS_STATUS_RESOURCES;
        goto Exit;
    }

    RtlZeroMemory(pFrameBuffer, frameLength);

    pNeighborReportFrame = (PDOT11_NEIGHBOR_REPORT_FRAME)pFrameBuffer;
    pNeighborReportFrame->Category = WDI_ACTION_FRAME_CATEGORY_RADIO_MEASUREMENT;
    pNeighborReportFrame->Action = WDI_RADIO_MEASUREMENT_ACTION_NEIGHBOR_REPORT_REQUEST;

    // Copy SSID to frame if it exists
    if (m_pSsid)
    {
        pNeighborReportFrame->InfoElements.ElementID = DOT11_INFO_ELEMENT_ID_SSID;
        pNeighborReportFrame->InfoElements.Length = (UCHAR)m_pSsid->uSSIDLength;
        RtlCopyMemory(
            pFrameBuffer + FIELD_OFFSET(DOT11_NEIGHBOR_REPORT_FRAME, InfoElements) + sizeof(DOT11_INFO_ELEMENT),
            m_pSsid->ucSSID,
            m_pSsid->uSSIDLength);
    }

    status = m_RequestParameters.ActionFrameBody.SimpleSet(pFrameBuffer, frameLength, 0);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to allocate memory for neighbor report frame body.");
        goto Exit;
    }

    //
    // Fill in remaining request parameters
    //
    m_RequestParameters.RequestParams.SendTimeout = WFC_CONSTANT::DefaultNeighborReportSendTimeout;
    m_RequestParameters.RequestParams.PostACKDwellTime = WFC_CONSTANT::DefaultNeighborReportPostACKDwellTime;

    CPort* pPort = m_pAdapter->GetPortFromPortId(get_PortId());
    CConnectedPeer* pPeer = pPort->GetConnectedPeerList()
        ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);

    if (pPeer == NULL)
    {
        WFCActErrorStatus("Failed to initialize Neighbor Report Request. Not currently connected to an AP.");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    // Get the current channel and band info
    status = pPort->GetPropertyCache()->GetPropertyT(
        WfcPortPropertyCurrentChannelNumber,
        &m_RequestParameters.RequestParams.ChannelNumber);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Couldn't get Channel Number from property cache");
        goto Exit;
    }

    status = pPort->GetPropertyCache()->GetPropertyT(
        WfcPortPropertyCurrentBandID,
        &m_RequestParameters.RequestParams.BandId);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Couldn't get Band Id from property cache");
        goto  Exit;
    }

    RtlCopyMemory(
        &m_RequestParameters.RequestParams.DestinationAddress,
        pPeer->GetMacAddress(),
        sizeof(WDI_MAC_ADDRESS));

Exit:
    if (pFrameBuffer)
    {
        delete[] pFrameBuffer;
    }

    TraceActExit(status);
    return status;
}

void CSendNeighborReportRequestJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    status = InitializeNeighborReportRequest();
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize neighbor report action frame.");
        goto Exit;
    }

    CSendActionFrameRequestJob::OnJobStarted();

Exit:
    if ( status != NDIS_STATUS_SUCCESS )
    {
        // Just complete the job
        CompleteJob(status);
    }

    TraceActExit(status);
}

NDIS_STATUS CBssTransitionResponseJob::InitializeBSSTransitionResponseFrame()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    PDOT11_BSS_TRANSITION_MANAGEMENT_RESPONSE_FRAME pBSSTransitionManagementResponseFrame = NULL;
    PUCHAR pFrameBuffer = NULL;
    ULONG frameLength = FIELD_OFFSET(DOT11_BSS_TRANSITION_MANAGEMENT_RESPONSE_FRAME, TargetBSSID);

    TraceActEntry();

    //
    // Fill in action frame body
    //
    if (m_pTargetBssid)
    {
        // Add size of optional Target BSSID
        frameLength += sizeof(DOT11_MAC_ADDRESS);
    }

    if (frameLength <= 0)
    {
        WFCActError("Frame length is less than or equal to zero. Error, exiting");
        goto Exit;
    }
    
    pFrameBuffer = new UCHAR[frameLength];
    
    if (pFrameBuffer == NULL)
    {
        WFCActError("Couldn't allocate memory for BSS Transition Management response frame buffer.");
        status = NDIS_STATUS_RESOURCES;
        goto Exit;
    }

    RtlZeroMemory(pFrameBuffer, frameLength);

    pBSSTransitionManagementResponseFrame = (PDOT11_BSS_TRANSITION_MANAGEMENT_RESPONSE_FRAME)pFrameBuffer;
    pBSSTransitionManagementResponseFrame->Category = WDI_ACTION_FRAME_CATEGORY_WNM;
    pBSSTransitionManagementResponseFrame->Action = WDI_WNM_ACTION_BSS_TRANSITION_MANAGEMENT_RESPONSE;
    pBSSTransitionManagementResponseFrame->DialogToken = m_DialogToken;
    pBSSTransitionManagementResponseFrame->Status = (UCHAR)m_StatusCode;
    pBSSTransitionManagementResponseFrame->BSSTerminationDelay = 0; //We do not support delay frame. So filling it out with zero

    // Copy target BSSID to frame if it exists
    if (m_pTargetBssid)
    {
        RtlCopyMemory(
            pFrameBuffer + FIELD_OFFSET(DOT11_BSS_TRANSITION_MANAGEMENT_RESPONSE_FRAME, TargetBSSID),
            m_pTargetBssid,
            sizeof(DOT11_MAC_ADDRESS)
            );
    }

    status = m_ResponseParameters.ActionFrameBody.SimpleSet(pFrameBuffer, frameLength, 0);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to allocate memory for BSS Transition response frame body.");
        goto Exit;
    }

    //
    // Fill in remaining response parameters
    //
    m_ResponseParameters.ResponseParams.SendTimeout = WFC_CONSTANT::DefaultActionFrameResponseSendTimeoutForSTA;
    m_ResponseParameters.ResponseParams.PostACKDwellTime = WFC_CONSTANT::DefaultActionFramePostACKDwellTime;
    m_IsRequestTask = false;

    CPort* pPort = m_pAdapter->GetPortFromPortId(get_PortId());
    CConnectedPeer* pPeer = pPort->GetConnectedPeerList()
        ->FindPeerByAssociationState(dot11_assoc_state_auth_assoc);

    if (pPeer == NULL)
    {
        WFCActErrorStatus("Failed to initialize BSS Transition response. Not currently connected to an AP.");
        status = NDIS_STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    // Get the current channel and band info
    status = pPort->GetPropertyCache()->GetPropertyT(
        WfcPortPropertyCurrentChannelNumber,
        &m_ResponseParameters.ResponseParams.ChannelNumber);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Couldn't get Channel Number from property cache");
        goto Exit;
    }

    status = pPort->GetPropertyCache()->GetPropertyT(
        WfcPortPropertyCurrentBandID,
        &m_ResponseParameters.ResponseParams.BandId);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Couldn't get Band Id from property cache");
        goto  Exit;
    }

    RtlCopyMemory(
        &m_ResponseParameters.ResponseParams.DestinationAddress,
        pPeer->GetMacAddress(),
        sizeof(WDI_MAC_ADDRESS));

Exit:
    if (pFrameBuffer)
    {
        delete[] pFrameBuffer;
    }

    TraceActExit(status);
    return status;
}

void CBssTransitionResponseJob::OnJobStarted()
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    TraceActEntry();

    status = InitializeBSSTransitionResponseFrame();
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCActError("Failed to initialize BSS transition response action frame.");
        goto Exit;
    }

    CSendActionFrameRequestJob::OnJobStarted();

Exit:
    if ( status != NDIS_STATUS_SUCCESS )
    {
        // Just complete the job
        CompleteJob(status);
    }

    TraceActExit(status);
}

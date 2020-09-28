/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    ActionFrame.hpp

Abstract:

    Header file for the Action Frame functions

Environment:

    Kernel mode

Revision History:

--*/
#pragma once

/*++
Issues action frame request to driver
*/

typedef struct
{
    USHORT                  StatusCode;                     // For Initial and Comeback Responses
    USHORT                  ComebackDelay;                  // For Initial and Comeback Responses
    UCHAR                   AdvertisementProtocolID;
    DOT11_ANQP_FRAGMENT_ID  FragmentID;                     // For Comeback Response
} ANQP_ACTION_FRAME_PARAMETERS, *PANQP_ACTION_FRAME_PARAMETERS;


class CSendActionFrameRequestJob : public CJobBase
{
public:
    CSendActionFrameRequestJob(
        _In_ WFC_ACTIVITY_ID ActivityId) :
        CJobBase(ActivityId),
        m_Initialized(false),
        m_TaskCommand(ActivityId),
        m_IsRequestTask(TRUE),
        m_pTlvBuffer(NULL),
        m_TlvBufferLength(0)
    {
        RtlZeroMemory(&m_RequestParameters, sizeof(m_RequestParameters));
        RtlZeroMemory(&m_ResponseParameters, sizeof(m_ResponseParameters));
    }
    
    ~CSendActionFrameRequestJob()
    {
        if (m_pTlvBuffer != NULL)
        {
            delete[] m_pTlvBuffer;
            m_pTlvBuffer = NULL;
        }
    }

    NDIS_STATUS Initialize(
        _In_ CAdapter* pAdapter,
        _In_ NDIS_PORT_NUMBER NdisPortNumber
        );

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
        _In_opt_ PANQP_ACTION_FRAME_PARAMETERS pANQPParams,
        _In_opt_ BOOL bIsRequestTask = TRUE
        );

    NDIS_STATUS GenerateANQPFrame(
        _In_ DOT11_ACTION_FRAME_PUBLIC_ACTION_TYPE ActionType,
        _In_ UCHAR DialogToken,
        _In_ PANQP_ACTION_FRAME_PARAMETERS pANQPParams,
        _In_ USHORT DataLength,
        _In_bytecount_(DataLength) PBYTE ucDataBuffer,
        _Out_ USHORT *pActionFrameLength,
        _Outptr_result_bytebuffer_(*pActionFrameLength) PBYTE *ppActionFrame
        );

    //
    // CJobBase
    //
    void OnJobStarted();

    void OnJobStepCompleted(
        _In_ NDIS_STATUS OperationStatus,
        _In_ WFC_SENDER_OBJECT SenderObject);

    void OnJobCancelled();

protected:
    NDIS_STATUS SendActionFrameRequest();

protected:
    CAdapter*           m_pAdapter;
    NDIS_PORT_NUMBER    m_NdisPortNumber;
    Task                m_TaskCommand;
    BOOL                m_Initialized;
    BOOL                m_IsRequestTask;

    BYTE*      m_pTlvBuffer;
    ULONG      m_TlvBufferLength;
    WDI_TASK_SEND_REQUEST_ACTION_FRAME_PARAMETERS m_RequestParameters;
    WDI_TASK_SEND_RESPONSE_ACTION_FRAME_PARAMETERS m_ResponseParameters;
};

class CSendNeighborReportRequestJob : public CSendActionFrameRequestJob

{
public:
    CSendNeighborReportRequestJob(
        _In_ WFC_ACTIVITY_ID ActivityId) :
        CSendActionFrameRequestJob(ActivityId),
        m_pSsid(NULL)
    {

    }

    ~CSendNeighborReportRequestJob()
    {

    }

    NDIS_STATUS InitializeNeighborReportRequest();

    void OnJobStarted();

private:
    PDOT11_SSID m_pSsid;
};

// The response action frame class for BSS Transition request. 
// This will be sent with the accept/reject status codes

class CBssTransitionResponseJob: public CSendActionFrameRequestJob
{
public:
    CBssTransitionResponseJob(
        _In_ WFC_ACTIVITY_ID ActivityId) :
        CSendActionFrameRequestJob(ActivityId),
        m_StatusCode(WDI_BSS_TRANSITION_MAX),
        m_pTargetBssid(NULL)
    {

    }

    ~CBssTransitionResponseJob()
    {

    }

    void SetTargetBssId(PDOT11_MAC_ADDRESS targetBSSID)
    {
        m_pTargetBssid = targetBSSID;
    }

    void SetResponseFrameParameters(
        _In_ UCHAR DialogToken,
        _In_ WDI_BSS_TRANSITION_RESPONSE_RESULT StatusCode
        )
    {
        m_DialogToken = DialogToken;
        m_StatusCode = StatusCode;
    }

    NDIS_STATUS InitializeBSSTransitionResponseFrame();

    void OnJobStarted();

private:
    WDI_BSS_TRANSITION_RESPONSE_RESULT m_StatusCode;
    PDOT11_MAC_ADDRESS m_pTargetBssid;
    UCHAR m_DialogToken;
};



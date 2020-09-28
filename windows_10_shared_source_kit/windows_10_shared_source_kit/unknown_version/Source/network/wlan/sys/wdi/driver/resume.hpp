/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Resume.hpp

Abstract:

    Header file for Resume functions

Environment:

    Kernel mode

Revision History:

--*/
#pragma once


/*++

    Derive class for resume. To adapt the parent class is not suffucient,  it would need
    to go to parent's parent's parent which disturb too many derived classes.
    Therefore, we derive one for our own use. Benefit is no change in the parent.
    We implement methods in our class.

--*/
class CResumeAdvSettingJob : public CSetWFDAdvertisementSettings
{
public:
    CResumeAdvSettingJob( 
        _In_ WFC_ACTIVITY_ID ActivityId ):
        CSetWFDAdvertisementSettings( ActivityId )
    {};

    ~ CResumeAdvSettingJob()
    {};

    NDIS_STATUS InitializeForResume( 
        _In_ CAdapter * pAdapter,
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _In_ NDIS_OID Oid,
        _In_ ULONG OidInfoBufferSize,
        _In_ BYTE * pbOidInfoBuffer );

private:
    NDIS_OID_REQUEST m_OidRequest;
};


class CResumePacketFilterJob: public CSetPacketFilterJob
{
public:
    CResumePacketFilterJob(
        _In_ WFC_ACTIVITY_ID ActivityId ):
    CSetPacketFilterJob(ActivityId)
    {};

    ~CResumePacketFilterJob()
    {};

    NDIS_STATUS InitializeForResume( 
        _In_ CAdapter * pAdapter,
        _In_ NDIS_PORT_NUMBER NdisPortNumber );

    void OnOidJobCompletedEvent()
    {
        // do nothing, don't complete the OID which is a dummy
        return;
    };

private:
    static const int    c_OidRequestSize = sizeof(NDIS_OID_REQUEST)+sizeof(ULONG);
    BYTE                m_pbOidPacketFilter[c_OidRequestSize];
};


class CResumeMulticastListJob: public CSetMulticastListJob
{
public:
    CResumeMulticastListJob(
        _In_ WFC_PROPERTY_NAME MulticastPropertyName,
        _In_ WFC_ACTIVITY_ID ActivityId):
        CSetMulticastListJob(MulticastPropertyName, ActivityId)
    {};

    ~CResumeMulticastListJob()
    {};

    NDIS_STATUS InitializeForResume( 
        _In_ CAdapter * pAdapter,
        _In_ NDIS_PORT_NUMBER NdisPortNumber,
        _In_ WFC_PROPERTY_NAME MulticastPropertyName,
        _In_ ULONG OidInfoBufferSize,
        _In_ BYTE * pbOidInfoBuffer );


    void OnOidJobCompletedEvent()
    {
        // do nothing, don't complete the OID which is our dummy
        return;
    };

private:
    CAdapter *      m_pAdapter;
    NDIS_PORT_NUMBER    m_NdisPortNumber;
    NDIS_OID_REQUEST    m_OidRequest;
    ULONG               m_OidInfoBufferSize;
    BYTE                *m_pbOidInfoBuffer;
};


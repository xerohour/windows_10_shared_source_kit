#pragma once

/*
  WiFiJobP2PSetGroupJoinParameters
 */
class CSetP2PGroupJoinParameters : public CPropertyCacheSetBufferOidJob
{
public:
    CSetP2PGroupJoinParameters(
        _In_ WFC_ACTIVITY_ID ActivityId ) :
        CPropertyCacheSetBufferOidJob(
            WfcPortPropertyP2PGroupJoinParameters,
            ActivityId ) { }
};

ia = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_145_Desc = "vs_2_0 : pow is NOT allowed in a 4 level loop aL, i0 block";
string VS_145 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				pow r0, v0.x, c0.x "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_146_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_146_Desc = "vs_2_0 : pow is NOT allowed in a 5 level loop aL, i0 block";
string VS_146 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		loop aL, i0 "
	"			loop aL, i0 "
	"				loop aL, i0 "
	"					pow r0, v0.x, c0.x "
	"				endloop "
	"			endloop "
	"		endloop "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_147_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_147_Desc = "vs_2_0 : pow is allowed in a 1 level rep i0 block";
string VS_147 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	pow r0, v0.x, c0.x "
	"endrep ";

VS_CRITERIA VS_148_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_148_Desc = "vs_2_0 : pow is NOT allowed in a 2 level rep i0 block";
string VS_148 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	rep i0 "
	"		pow r0, v0.x, c0.x "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_149_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_149_Desc = "vs_2_0 : pow is NOT allowed in a 3 level rep i0 block";
string VS_149 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			pow r0, v0.x, c0.x "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_150_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_150_Desc = "vs_2_0 : pow is NOT allowed in a 4 level rep i0 block";
string VS_150 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				pow r0, v0.x, c0.x "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_151_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_151_Desc = "vs_2_0 : pow is NOT allowed in a 5 level rep i0 block";
string VS_151 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	rep i0 "
	"		rep i0 "
	"			rep i0 "
	"				rep i0 "
	"					pow r0, v0.x, c0.x "
	"				endrep "
	"			endrep "
	"		endrep "
	"	endrep "
	"endrep ";

VS_CRITERIA VS_152_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_152_Desc = "vs_2_0 : pow is allowed in a if b0 and if b0 block";
string VS_152 = 
	"vs_2_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if b0 "
	"	if b0 "
	"		pow r0, v0.x, c0.x "
	"	endif "
	"endif ";

VS_CRITERIA VS_153_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_153_Desc = "vs_2_0 : pow is allowed in a if b0 and if b0 nop else block";
string VS_153 = 
	"vs_2_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if b0 "
	"	if b0 nop else "
	"		pow r0, v0.x, c0.x "
	"	endif "
	"endif ";

VS_CRITERIA VS_154_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_154_Desc = "vs_2_0 : pow is allowed in a if b0 and loop aL, i0 block";
string VS_154 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if b0 "
	"	loop aL, i0 "
	"		pow r0, v0.x, c0.x "
	"	endloop "
	"endif ";

VS_CRITERIA VS_155_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_155_Desc = "vs_2_0 : pow is allowed in a if b0 and rep i0 block";
string VS_155 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if b0 "
	"	rep i0 "
	"		pow r0, v0.x, c0.x "
	"	endrep "
	"endif ";

VS_CRITERIA VS_156_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_156_Desc = "vs_2_0 : pow is allowed in a if b0 nop else and if b0 block";
string VS_156 = 
	"vs_2_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if b0 nop else "
	"	if b0 "
	"		pow r0, v0.x, c0.x "
	"	endif "
	"endif ";

VS_CRITERIA VS_157_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_157_Desc = "vs_2_0 : pow is allowed in a if b0 nop else and if b0 nop else block";
string VS_157 = 
	"vs_2_0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if b0 nop else "
	"	if b0 nop else "
	"		pow r0, v0.x, c0.x "
	"	endif "
	"endif ";

VS_CRITERIA VS_158_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_158_Desc = "vs_2_0 : pow is allowed in a if b0 nop else and loop aL, i0 block";
string VS_158 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if b0 nop else "
	"	loop aL, i0 "
	"		pow r0, v0.x, c0.x "
	"	endloop "
	"endif ";

VS_CRITERIA VS_159_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_159_Desc = "vs_2_0 : pow is allowed in a if b0 nop else and rep i0 block";
string VS_159 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"defb b0, true "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"if b0 nop else "
	"	rep i0 "
	"		pow r0, v0.x, c0.x "
	"	endrep "
	"endif ";

VS_CRITERIA VS_160_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_160_Desc = "vs_2_0 : pow is allowed in a loop aL, i0 and if b0 block";
string VS_160 = 
	"vs_2_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	if b0 "
	"		pow r0, v0.x, c0.x "
	"	endif "
	"endloop ";

VS_CRITERIA VS_161_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_161_Desc = "vs_2_0 : pow is allowed in a loop aL, i0 and if b0 nop else block";
string VS_161 = 
	"vs_2_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	if b0 nop else "
	"		pow r0, v0.x, c0.x "
	"	endif "
	"endloop ";

VS_CRITERIA VS_162_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_162_Desc = "vs_2_0 : pow is NOT allowed in a loop aL, i0 and loop aL, i0 block";
string VS_162 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	loop aL, i0 "
	"		pow r0, v0.x, c0.x "
	"	endloop "
	"endloop ";

VS_CRITERIA VS_163_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_163_Desc = "vs_2_0 : pow is NOT allowed in a loop aL, i0 and rep i0 block";
string VS_163 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"loop aL, i0 "
	"	rep i0 "
	"		pow r0, v0.x, c0.x "
	"	endrep "
	"endloop ";

VS_CRITERIA VS_164_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_164_Desc = "vs_2_0 : pow is allowed in a rep i0 and if b0 block";
string VS_164 = 
	"vs_2_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	if b0 "
	"		pow r0, v0.x, c0.x "
	"	endif "
	"endrep ";

VS_CRITERIA VS_165_Criteria = { true, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_165_Desc = "vs_2_0 : pow is allowed in a rep i0 and if b0 nop else block";
string VS_165 = 
	"vs_2_0 "
	"defb b0, true "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	if b0 nop else "
	"		pow r0, v0.x, c0.x "
	"	endif "
	"endrep ";

VS_CRITERIA VS_166_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_166_Desc = "vs_2_0 : pow is NOT allowed in a rep i0 and loop aL, i0 block";
string VS_166 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	loop aL, i0 "
	"		pow r0, v0.x, c0.x "
	"	endloop "
	"endrep ";

VS_CRITERIA VS_167_Criteria = { false, false, 0x0200, -1, -1, -1, -1, 0, -1, 0.f };
string VS_167_Desc = "vs_2_0 : pow is NOT allowed in a rep i0 and rep i0 block";
string VS_167 = 
	"vs_2_0 "
	"defi i0, 0, 0, 0, 0 "
	"def c0, 1, 1, 1, 1 "
	"dcl_fog v0 "
	"rep i0 "
	"	rep i0 "
	"		pow r0, v0.x, c0.x "
	"	endrep "
	"endrep ";


#include "MultiScissor.h"

struct color4
{
	color4(FLOAT R, FLOAT G, FLOAT B, FLOAT A) {r=R;g=G;b=B;a=A;}
	color4() {r=0.0f;g=0.0f;b=0.0f;a=0.0f;}
	FLOAT r;
	FLOAT g;
	FLOAT b;
	FLOAT a;
};

class BadIndex : public MultiScissor
{
private:
	UINT m_ViewportArrayIndex;

public:
	void InitTestParameters();
	TEST_RESULT ExecuteTestCase();
};
﻿<?xml version='1.0' encoding='utf-8' standalone='yes'?>
<Content xmlns="http://microsoft.com/schemas/Windows/Kits/Manifest">
  <ContentGroup
      BuildArchitecture="each"
      Destination="$(_BuildArch)"
      >
    <FeatureReference Name="Device.Graphics"/>
    <BinaryFile
        Destination="nttest\windowstest\graphics\d3d\conf"
        Name="WGF11Draw.exe"
        Source="$(OBJ_PATH)\$(O)"
        />
  </ContentGroup>
</Content>
//-------------------------------------------------------------------------
// ENUMDECL related declarations
//-------------------------------------------------------------------------

extern tstring WGFTESTAPI ToString( ENUMDECL ENUMNAME );
extern bool WGFTESTAPI FromString( ENUMDECL *pENUMNAME, const tstring &str );

// ENUMNAMEInfo - Structure that describes the properties of a D3DENUMNAMECAP
struct ENUMNAMEInfo
{
	ENUMDECL ENUMNAME;
	const TCHAR *szName;
};

// CENUMNAMEInfoMap - Static singleton object used to lookup info given a D3DENUMNAME.
// THIS CLASS SHOULD NOT BE DIRECTLY INSTANTIATED !!!
class CENUMNAMEInfoMap
{
public:
	static const ENUMNAMEInfo &GetENUMNAMEInfo( size_t nIndex ) { return m_ENUMNAMEInfoTable[nIndex]; }
		// Returns the D3DENUMNAMEInfo at nIndex in the internal table.
	
	static const ENUMNAMEInfo *Find( ENUMDECL ENUMNAME );
	static const ENUMNAMEInfo *Find( const tstring &Desc );
		// Returns a pointer to the D3DENUMNAMEInfo associated with the given string value.
		
	static const size_t GetENUMNAMEInfoTableSize();
		// Returns number on entries in m_ENUMNAMEInfoTable
		
private:
	CENUMNAMEInfoMap();

	typedef	std::map< ENUMDECL, const ENUMNAMEInfo *> ByValue;
	ByValue m_ByValue;
	
	typedef std::map< tcistring, const ENUMNAMEInfo *> ByDesc;
	ByDesc m_ByDesc;
	
	static CENUMNAMEInfoMap m_Map;	
	friend class CENUMNAMEInfoMap;
	
	static const ENUMNAMEInfo m_ENUMNAMEInfoTable[];
};
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
    NDIS_STATUS                 
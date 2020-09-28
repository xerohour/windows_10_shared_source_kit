/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/


#include <windows.h>
#include <tchar.h>
#include <cfgmgr32.h>
#include <setupapi.h>
#include <atlstr.h>

#define USERMODE
#include <azcodec.h>

#include <shell98.h>
#include <module98.h>
#include <dvmodule.h>
#include <privioct.h>
#include <controller-interface.h>

#include "azverbutils.h"
#include "install.h"
#include "uaatest.h"
#include "aznode.h"
#include "log.h"
#include "power.h"
#include "codec.h"
#include "stringify.h"

//==============================================================================
// ValidateHDMI
//
// Description:
//  Check a codec for compliance with the Intel HD Audio HDMI DCNs
//  (http://www.intel.com/standards/hdaudio/)
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE if the codec's HDMI implementation is either hidden from us
//  or is in compliance with the HDMI DCNs
//
// History:
//  9/12/2008 - MatEer - created
//==============================================================================
DWORD ValidateHDMI()
{
    BOOL fRes = ((CCodec*)(g_pUAATest->m_pCurDevice))->ValidateHDMI();
    return (fRes ? FNS_PASS : FNS_FAIL);
}

//==============================================================================
// CCodec::ValidateHDMI
//
// Description:
//  Check the codec for compliance with the Intel HD Audio HDMI DCNs
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  9/12/2008 - MatEer - created
//==============================================================================
BOOL 
CCodec::ValidateHDMI()
{
    BOOL fRes = TRUE;    
    
    SLOG(eInfo2, _T("Verifying Codec on SDI %u"), m_usSDI);
    
    // loop through all the function groups on the codec
    // (usually 1)
    for (
        TTNode<CFunctionGroup>* pttnodeFG = listFG.GetHead();
        NULL != pttnodeFG;
        pttnodeFG = listFG.GetNext(pttnodeFG)
    )
    {
        CFunctionGroup* pFG = pttnodeFG->pData;
        if (NodeTypeAudioFunctionObject == (pFG->m_NodeType & 0xff))
        {
            CAudioFunctionGroup* pAFG = dynamic_cast<CAudioFunctionGroup*>(pFG);            
            fRes = pAFG->ValidateHDMI() ? fRes : FALSE;
        }
    }

    return fRes;
} // CCodec::ValidateHDMI


//==============================================================================
// CAudioFunctionGroup::ProbeHDMIPerAudio0078
//
// Description:
//  Check if AFG implements any of the verbs listed in WLP Audio-0078
//  If it does, we can hold the AFG to the HDMI HD Audio DCNs
//  If not, we have to ignore the HDMI capability bit
//  Sets m_SupportsHDMIPerAudio0078 to true or false accordingly
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  4/15/09 - MatEer - created
//==============================================================================
BOOL 
CAudioFunctionGroup::ProbeHDMIPerAudio0078()
{
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    SLOG(eInfo1, "Probing audio function group for HDMI support per WLP requirement AUDIO-0078...");

    // does any pin complex claim HDMI or DisplayPort support?
    // 
    bool bAnyPinWidgetHasLocationSetToHDMI = false;
    bool bAnyPinWidgetHasHDMICapableSet = false;
    bool bAnyPinWidgetHasDisplayPortCapableSet = false;

    for (
        TTNode<CPinComplex>* pttnodePC = listPC.GetHead();
        NULL != pttnodePC;
        pttnodePC = listPC.GetNext(pttnodePC)
    )
    {
        CPinComplex* pPC = pttnodePC->pData;
        AzPinCaps capabilities(pPC->m_PinCaps.Response);
        AzPinWidgetConfigDefault configDefault(pPC->m_ConfigDef.Response);

        // if the pin is not connected, don't bother with further verbs
        if (ePcxPortConnNoPhysConn == configDefault.PortConn)
        {
            SLOG(eInfo1, "    Pin complex %u is not physically connected", pPC->m_usNode);
            continue;
        }
        
        // mechanism 1, HD Audio Spec 1.0:
        //     Pin Widget: Configuration Default (7.3.3.31)
        //     Location = Special/Internal implies HDMI
        //     (updated to Digital Display in DCN 36-A)
        if (
            ePcxGenLocInternal == configDefault.GenLoc &&
            ePcxGeoLocSpecial1 == configDefault.GeoLoc
        )
        {
            SLOG(eInfo1,
                "Pin complex %u is HDMI by virtue of having\n"
                "general location \"internal\" and geographic location \"special (8)\".\n"
                "See Configuration Default (7.3.3.31) of original HD Audio specification,\n"
                "and DCN 36-A (DisplayPort.)",
                pPC->m_usNode
            );
            bAnyPinWidgetHasLocationSetToHDMI = true;
        }

        // mechanism 2, HD Audio DCN 34-A2:
        //     Pin Widget: Pin Capabilities (7.3.4.9)
        //     HDMI Capable bit (bit 7)
        if (capabilities.HDMICapable)
        {
            SLOG(eInfo1,
                "Pin complex %u is HDMI by virtue of having the HDMI Capable bit set.\n"
                "See Pin Capabilities (7.3.4.9) of HD Audio DCN 34-A2 (HDMI).",
                pPC->m_usNode
            );
            bAnyPinWidgetHasHDMICapableSet = true;
        }

        // mechanism 3, HD Audio DCN 36-A:
        //     Pin Widget: Pin Capabilities (7.3.4.9)
        //     Display Port Capable bit (bit 24)
        if (capabilities.DPCapable)
        {
            SLOG(eInfo1,
                "Pin complex %u is Display Port by virtue of having the Display Port Capable bit set.\n"
                "See Pin Capabilities (7.3.4.9) of HD Audio DCN 36-A (DisplayPort).",
                pPC->m_usNode
            );
            bAnyPinWidgetHasDisplayPortCapableSet = true;
        }
    }    

    // does AUDIO-0078 escape hatch apply?
    if (
        bAnyPinWidgetHasLocationSetToHDMI ||
        bAnyPinWidgetHasHDMICapableSet ||
        bAnyPinWidgetHasDisplayPortCapableSet
    )
    {
        if (bAnyPinWidgetHasDisplayPortCapableSet)
        {
            SLOG(eInfo1,
                "AUDIO-0078: This codec has a DisplayPort Capable bit set.\n"
                "Full compliance with all Intel HD Audio HDMI DCNs is entailed.\n"
                "Skipping per-verb implementation probe."
            );
            m_SupportsHDMIPerAudio0078 = true;
            goto DONE;
        }
        else
        {
            SLOG(eInfo1,
                "AUDIO-0078: There is a pin that advertises HDMI support via either:\n"
                "(1) Location (in the Configuration Default) or\n"
                "(2) HDMI Capable (in the Pin Capabilities).\n"
                "Full compliance with all Intel HD Audio HDMI DCNs is required IF AND ONLY IF\n"
                "any of the verbs listed in AUDIO-0078 are implemented.\n"
                "Will check for support of each verb listed."
            );
        }
    }
    else
    {
        SLOG(eInfo1,
            "AUDIO-0078: There is no HDMI or DisplayPort on this codec:\n"
            "(1) No pin widgets have the Location set to HDMI in the Configuration Default.\n"
            "(2) No pin widgets have the HDMI Capable bit set in the Pin Capabilities.\n"
            "(3) No pin widgets have the Display Port bit set in the pin capabilities.\n"
            "Skipping AUDIO-0078 verb probing."
        );
        goto DONE;
    }

    // if we get this far need to probe verbs.

    // Check the following verbs on all audio output converters:
    for (
        TTNode<CAudioOutputConverter>* pttnodeAOC = listAOC.GetHead();
        NULL != pttnodeAOC;
        pttnodeAOC = listAOC.GetNext(pttnodeAOC)
    )
    {
        CAudioOutputConverter* pAOC = pttnodeAOC->pData;

        
        // F2Dh (Get Converter Channel Count)
        AzConverterChannelCount azConverterChannelCount;
        UCHAR chOriginal = 0;
        Verb = MakeVerb(m_pCodec->m_usSDI, pAOC->m_usNode, GetCnvChCnt, 0, 0);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
        
        if (azrResponse.Valid)
        {
            azConverterChannelCount = azrResponse.Response;
            chOriginal = azConverterChannelCount.CnvChCnt;

            // AzConverterChannelCount doesn't have a .Reserved0 for the top 24 reserved bits
            if (0 != (azrResponse.Response & 0xffffff00))
            {
                SLOG(eError,
                    "Get Converter Channel Count on node %u has reserved bits set: 0x%08x",
                    pAOC->m_usNode,
                    azrResponse.Response
                );
            }

            // If initial value is non-zero then that counts as support
            SLOG(eInfo1,
                "Initial \"Get Converter Channel Count\" on node %u is %u",
                pAOC->m_usNode,
                azConverterChannelCount.CnvChCnt
            );

            if (0 != azConverterChannelCount.CnvChCnt)
            {
                SLOG(eInfo1, "Interpreting non-zero initial value for Get Converter Channel Count as implementing that verb");
                m_SupportsHDMIPerAudio0078 = true;
            }
        }

        
        // 72Dh (Set Converter Channel Count)

        // if initial value is zero but setting a non-zero value sticks
        // then that counts as support
        // chNew can be anything different than chOriginal
        // picking 1 and 7 because they stand for stereo and 7.1, respectively
        // these are reasonably likely to be supported
        UCHAR chNew = (1 == chOriginal ? 7 : 1);

        // change the value
        Verb = MakeVerb(m_pCodec->m_usSDI, pAOC->m_usNode, SetCnvChCnt, chNew, 0);
        SendVerb(m_pCodec->m_hDriver, Verb); // ignoring response

        // get the new value
        Verb = MakeVerb(m_pCodec->m_usSDI, pAOC->m_usNode, GetCnvChCnt, 0, 0);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);

        if (azrResponse.Valid)
        {
           azConverterChannelCount = azrResponse.Response;

            if (azConverterChannelCount.CnvChCnt == chNew)
            {
                SLOG(eInfo1, "Setting converter channel count to %u stuck", chNew);
                m_SupportsHDMIPerAudio0078 = true;
            }
            else if (azConverterChannelCount.CnvChCnt != chOriginal)
            {
                SLOG(eError,
                    "Setting the converter channel count to %u caused it to change to %u "
                    "(it was originally %u)",
                    chNew,
                    azConverterChannelCount.CnvChCnt,
                    chOriginal
                );
                m_SupportsHDMIPerAudio0078 = true;
            }
            else
            {
                SLOG(eInfo1, "Attempting to change the converter channel count had no effect; it's still %u", chOriginal);
            }
        }

        // put the original value back
        SLOG(eInfo1, "Setting converter channel count back to original value of %u", chOriginal);
        Verb = MakeVerb(m_pCodec->m_usSDI, pAOC->m_usNode, SetCnvChCnt, chOriginal, 0);
        SendVerb(m_pCodec->m_hDriver, Verb);
        // trust to luck that the put-things-back succeeded

        // F34h (Get Converter Channel to HDMI Slot Mapping)        
        // 734h (Set Converter Channel to HDMI Slot Mapping)
        // do these per-slot
        for (USHORT slot = 0; slot < 8; slot++)
        {
            // F34h (Get Converter Channel to HDMI Slot Mapping)
            AzCnvChMap azMap;
            azMap.SlotNum = slot;
            azMap.SpeakerNum = 0; // reserved on "get" requests

            Verb = MakeVerb(m_pCodec->m_usSDI, pAOC->m_usNode, GetCnvChMap, azMap, 0);
            azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);

            // default this to what is supposed to be the original value
            USHORT spkOriginal = slot;
            switch (slot)
            {
                case 2: spkOriginal = 3; break;
                case 3: spkOriginal = 2; break;
            }

            if (azrResponse.Valid)
            {
                azMap = azrResponse.Response;

                spkOriginal = azMap.SpeakerNum;

                // check for reserved bits
                if (0 != (azrResponse.Response & 0xffffff00))
                {
                    SLOG(eError,
                        "Error: reserved bits set in response (0x%08x) to ASP channel mapping for slot %u",
                        azrResponse.Response, slot
                    );
                    m_SupportsHDMIPerAudio0078 = true;
                }

                if (0 == azMap.SlotNum && 0 == azMap.SpeakerNum)
                {
                    SLOG(eInfo1,
                        "Get Converter Channel to HDMI Slot Mapping for slot %u returned "
                        "0 => 0 on node %u",
                        slot, pAOC->m_usNode
                    );
                }
                else
                {
                    SLOG(eInfo1,
                        "Get Converter Channel to HDMI Slot Mapping for slot %u returned "
                        "slot %u => speaker %u on node %u",
                        slot, azMap.SlotNum, azMap.SpeakerNum, pAOC->m_usNode
                    );
                    m_SupportsHDMIPerAudio0078 = true;
                }
            } // if "get converter channel" response was valid

            // 734h (Set Converter Channel to HDMI Slot Mapping)
            // change the value to something different (and non-zero
            USHORT spkNew = (1 == spkOriginal ? 2 : 1);

            azMap.SlotNum = slot;
            azMap.SpeakerNum = spkNew;

            Verb = MakeVerb(m_pCodec->m_usSDI, pAOC->m_usNode, SetCnvChMap, azMap, 0);
            SendVerb(m_pCodec->m_hDriver, Verb);
            // trust to luck

            // see if it stuck
            azMap.SlotNum = slot;
            azMap.SpeakerNum = 0; // reserved on "get" requests

            Verb = MakeVerb(m_pCodec->m_usSDI, pAOC->m_usNode, GetCnvChMap, azMap, 0);
            azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
            if (azrResponse.Valid)
            {
                azMap = AzCnvChMap(azrResponse.Response);

                SLOG(eInfo1,
                    "Started with %u => %u.\n"
                    "Used Set Converter Channel to HDMI Slot Mapping to %u => %u;\n"
                    "End result was %u => %u.",
                    slot, spkOriginal,
                    slot, spkNew,
                    azMap.SlotNum, azMap.SpeakerNum
                );
                if (0 == azMap.SlotNum && 0 == azMap.SpeakerNum)
                {
                    // consistent with non-implementation
                }
                else
                {
                    // we got something nonzero out of this verb
                    m_SupportsHDMIPerAudio0078 = true;
                }
            }

            // put the original value back
            azMap.SlotNum = slot;
            azMap.SpeakerNum = spkOriginal;

            Verb = MakeVerb(m_pCodec->m_usSDI, pAOC->m_usNode, SetCnvChMap, azMap, 0);
            SendVerb(m_pCodec->m_hDriver, Verb);
            // trust to luck that the revert worked

        } // for each slot
    }

    // Check the following verbs on all pin complexes:
    for (
        TTNode<CPinComplex>* pttnodePC = listPC.GetHead();
        NULL != pttnodePC;
        pttnodePC = listPC.GetNext(pttnodePC)
    )
    {
        CPinComplex* pPC = pttnodePC->pData;

        AzPinWidgetConfigDefault configDefault(pPC->m_ConfigDef.Response);

        // if the pin is not connected, don't bother with further verbs
        if (ePcxPortConnNoPhysConn == configDefault.PortConn)
        {
            SLOG(eInfo1, "    Pin complex %u is not physically connected", pPC->m_usNode);
            continue;
        }

        // F2Fh (Get HDMI ELD Data)
        // just get the four bytes that form the ELD header
        // don't bother to check the data island packet size (yet)
        for (UCHAR i = 0; i <= 3; i++)
        {
            AzGetELDDataPayload azELDPayload;
            azELDPayload.Offset = i;
            
            Verb = MakeVerb(m_pCodec->m_usSDI, pPC->m_usNode, GetELDData, azELDPayload, 0);
            azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
            if (azrResponse.Valid)
            {
                AzELDData azELDData = azrResponse.Response;

                if (0 != azrResponse.Response)
                {
                    m_SupportsHDMIPerAudio0078 = true;
                }

                SLOG(eInfo1,
                    "Get ELD Data response for ELD byte %u on node %u is:\n"
                    "Valid (bit 31): %u; Reserved (bits 30:8): 0x%x; Data (bits 7:0): 0x%02x",
                    i, pPC->m_usNode,
                    azELDData.Valid, azELDData.Reserved0, azELDData.Data
                );
            }
        } // for each of the four bytes of the ELD header
        
        // F2Eh (Get HDMI Data Island Packet – Size Info)
        // first try ELD (handled specially)
        AzGetDIPSizePayload azGetDIPSizePayload;
        azGetDIPSizePayload.EldInx = 1;
        Verb = MakeVerb(m_pCodec->m_usSDI, pPC->m_usNode, GetDIPSize, azGetDIPSizePayload, 0);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
        if (azrResponse.Valid)
        {
            if (0 != azrResponse.Response)
            {
                m_SupportsHDMIPerAudio0078 = true;
            }

            AzDIPSize azDIPSize(azrResponse.Response);
            SLOG(eInfo1,
                "Get Data Island Packet Size response for ELD on node %u is 0x%08x (%u bytes, 0-based)",
                pPC->m_usNode,
                azrResponse.Response, azDIPSize.DipSize
            );
        }
        
        // then try each of the seven possible data island packets
        azGetDIPSizePayload.EldInx = 0;
        for (UCHAR dip = 0; dip <= 7; dip++)
        {
            azGetDIPSizePayload.PacketInx = dip;

            Verb = MakeVerb(m_pCodec->m_usSDI, pPC->m_usNode, GetDIPSize, azGetDIPSizePayload, 0);
            azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
            if (azrResponse.Valid)
            {
                if (0 != azrResponse.Response)
                {
                    m_SupportsHDMIPerAudio0078 = true;
                }

                AzDIPSize azDIPSize(azrResponse.Response);
                SLOG(eInfo1,
                    "Get Data Island Packet Size response for data island packet %u on node %u is 0x%08x (%u bytes, 0-based)",
                    pPC->m_usNode, dip,
                    azrResponse.Response, azDIPSize.DipSize
                );
            }
        }

        SLOG(eInfo1, "Not currently checking Set Data Island Packet Size... may test in a future revision.");
        SLOG(eInfo1, "Not currently checking Get/Set Data Island Packet Index... may test in a future revision.");
        SLOG(eInfo1, "Not currently checking Get/Set Data Island Packet Data... may test in a future revision.");
        SLOG(eInfo1, "Not currently checking Get/Set Data Island Packet Transmit Control... may test in a future revision.");

        // F33h (Get Content Protection Control)
        Verb = MakeVerb(m_pCodec->m_usSDI, pPC->m_usNode, GetCPState, 0);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
        if (azrResponse.Valid)
        {
            AzCPState azCPState = azrResponse.Response;

            if (0 != azrResponse.Response)
            {
                m_SupportsHDMIPerAudio0078 = true;
            }

            SLOG(eInfo1,
                "--- CONTENT PROTECTION STATE on node %u ---\n"
                "Current request state: %u\n"
                "Reserved (bit 2): %u\n"
                "Unsolicited Response sub tag: 0x%x\n"
                "Ready: %u\n"
                "Current encryption state: %u\n"
                "Reserved (bits 10 - 31): 0x%x",
                pPC->m_usNode,
                azCPState.RequestedState,
                azCPState.Reserved0,
                azCPState.SubTag,
                azCPState.Ready,
                azCPState.CurrentState,
                azCPState.Reserved1
            );
        }

        SLOG(eInfo1, "Not currently checking Set Content Protection Control... may test in a future revision.");
    }

DONE:    
    SLOG(eInfo1,
        "... done probing HDMI support. Adherence to the HDMI DCNs %s for this codec.",
        (m_SupportsHDMIPerAudio0078 ? "IS required" : "IS NOT required")
    );
    return TRUE;
}


//==============================================================================
// CAudioFunctionGroup::ValidateHDMI
//
// Description:
//  Check the audio function group's pins for compliance with
//  the Intel HD Audio HDMI DCNs
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  9/12/2008 - MatEer - created
//==============================================================================
BOOL CAudioFunctionGroup::ValidateHDMI()
{
    BOOL fRes = TRUE;

    SLOG(eInfo2, _T("Verifying Audio Function Group at node %u"), m_usNode);

    // verify all pin complexes
    for (
        TTNode<CPinComplex>* pttnodePC = listPC.GetHead();
        NULL != pttnodePC;
        pttnodePC = listPC.GetNext(pttnodePC)
    )
    {
        CPinComplex* pPC = pttnodePC->pData;
        fRes = pPC->ValidateHDMI() ? fRes : FALSE;
    }    
    
    return fRes;
} // CAudioFunctionGroup::ValidateHDMI

//==============================================================================
// CPinComplex::ValidateHDMI
//
// Description:
//  Check the pin complex for compliance with
//  the Intel HD Audio HDMI DCNs
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  9/12/2008 - MatEer - created
//==============================================================================
BOOL CPinComplex::ValidateHDMI()
{
    BOOL fRes = TRUE;

    CAudioFunctionGroup *pFG = dynamic_cast<CAudioFunctionGroup*>(m_parentFG);
    AzAudioWidgetCaps azAudioCaps = m_AudWidgetCaps.Response;
    AzPinCaps azPinCaps = m_PinCaps.Response;
    AzPinWidgetConfigDefault azConfig = m_ConfigDef.Response;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;
    AzGetCnvChMapPayload azPayload;
    AzDigitalPinSenseResponse azPinSense;
    AzGetDIPSizePayload azDIPSizePayload;
    AzDIPSize azDIPSize;
    int cbELDSize;
    int cbELDBaselineBlockSize;
    int cbELDVendorBlockSize;
    int vELDVersion;
    BYTE rbELDHeader[4] = {0};
    BYTE *pbELDBaselineBlock = NULL;
    BYTE *pbELDVendorBlock = NULL;

    SLOG(eInfo2, _T("Verifying Pin Complex at node %u"), m_usNode);

    LogAudioWidgetCaps(eInfo2, azAudioCaps);
    LogPinCaps(eInfo2, azPinCaps);
    LogConfigDefault(eInfo2, azConfig);
    
    // if the pin is not connected, don't bother with further verbs
    if (ePcxPortConnNoPhysConn == azConfig.PortConn)
    {
        SLOG(eInfo1, "    Pin complex %u is not physically connected", m_usNode);
        goto DONE;
    }

    // verify obsolete parameter ID 0x20 is not supported
    // (HDMI LPCM CAD in DCN 34-A2)
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, (AzCodecNodeParameter)0xf0020);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "   Obsolete HDMI LPCM CAD: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (azrResponse.Response != 0)
    {
        SLOG(eError, "Error: obsolete parameter 20h for HDMI LPCM CAD used (introduced in DCN 34-A2 but removed in DCN36-A)");
        fRes = FALSE;
    }


    //
    // SANITY CHECKS: Pin Capabilities
    //

    // High Bit Rate only applies to HDMI pin widgets
    if (azPinCaps.HBR && !azPinCaps.HDMICapable)
    {
        SLOG(eError, "Error: Pin claims support for High Bit Rate but this only applies to HDMI Capable pins.");
        fRes = FALSE;
    }

    // Copy protection only applies to digital pins
    if (azAudioCaps.CPCapable && !azAudioCaps.Digital)
    {
        SLOG(eError, "Error: Pin claims support for content protection but this only applies to digital pins");
        fRes = FALSE;
    }

    if (
        // location claims to be "Digital Display"
        ePcxGenLocInternal == azConfig.GenLoc &&
        ePcxGeoLocSpecial1 == azConfig.GeoLoc &&
        // but this is not an HDMI or Display Port pin widget
        !azPinCaps.HDMICapable &&
        !azPinCaps.DPCapable
    )
    {
        XLOG(XWARN, eWarn1,
            _T("Warning: Widget has Gross Location %u (%s) ")
            _T("and Geometric Location %u (%s), implying ")
            _T("per the Location table for section 7.3.3.31 in the HD Audio specification ")
            _T("that this is an HDMI or DisplayPort pin widget; ")
            _T("yet the pin capabilities has zeros for both \"HDMI Capable\" and \"DisplayPort Capable\""),
            ePcxGenLocInternal, Stringify::GrossLocation(ePcxGenLocInternal),
            ePcxGeoLocSpecial1, Stringify::GeometricLocation(ePcxGeoLocSpecial1)
        );
        fRes = FALSE;            
    }

    // Further checks only apply to HDMI or DisplayPort pin widgets
    if (
        !azPinCaps.HDMICapable &&
        !azPinCaps.DPCapable &&
        (
            ePcxGenLocInternal != azConfig.GenLoc ||
            ePcxGeoLocSpecial1 != azConfig.GeoLoc
        )
    )
    {
        SLOG(eInfo1, "This pin complex is not HDMI-capable or DisplayPort-capable");
        goto DONE;
    }

    // Some capabilities do not apply to HDMI or DisplayPort pin widgets
    if (azPinCaps.ImpSense)
    {
        SLOG(eError, "Error: HDMI/DisplayPort pin widgets cannot do impedance sense");
        fRes = FALSE;
    }

    if (azPinCaps.TriggerRequired)
    {
        SLOG(eError, "Error: Setting the \"Trigger Required\" bit is not compatible with HDMI or DisplayPort support");
        fRes = FALSE;
    }

    if (azPinCaps.HpDriveCapable)
    {
        SLOG(eError, "Error: Setting the \"Headphone Drive Capable\" bit is not compatible with HDMI or DisplayPort support");
        fRes = FALSE;
    }

    if (azPinCaps.InputCapable || !azPinCaps.OutputCapable)
    {
        SLOG(eError, "Error: HDMI and DisplayPort pin widgets must be output-only");
        fRes = FALSE;
    }

    if (azPinCaps.BalancedIO)
    {
        SLOG(eError, "Error: HDMI and DisplayPort pin widgets are not compatible with balanced I/O");
        fRes = FALSE;
    }

    if (azPinCaps.HiZ || azPinCaps.Volt50 || azPinCaps.Volt80 || azPinCaps.Volt100)
    {
        SLOG(eError, "Error: HDMI and DisplayPort pin widgets are not compatible with VRef settings");
        fRes = FALSE;
    }

    if (azPinCaps.EAPD)
    {
        SLOG(eError, "Error: HDMI and DisplayPort pin widgets are not compatible with External Amplifier Disable settings");
        fRes = FALSE;
    }

    //
    // SANITY CHECKS: Audio Widget Capabilities
    //

    if (azAudioCaps.ChanCountExt == 0 && azAudioCaps.Stereo == 0) // mono
    {
        SLOG(eError, "Error: HDMI and DisplayPort pin widgets must support at least two channels");
        fRes = FALSE;
    }

    if (azAudioCaps.InAmpPresent)
    {
        SLOG(eError, "Error: HDMI and DisplayPort pin widgets cannot have input amplifiers");
        fRes = FALSE;
    }

    // test output amplifier
    if (azAudioCaps.OutAmpPresent)
    {
        AzAmpProperties amp;

        // this could be specified on the pin complex level or the function group level
        if (azAudioCaps.AmpParamOverride)
        {
            if (m_OutAmpProps.Valid)
            {
                amp = m_OutAmpProps.Response; // got it
            }
            else
            {
                SLOG(eError, "Error: HDMI/DisplayPort pin complex has AmpParamOverride but request for Output Amplifier Properties was invalid");
                fRes = FALSE;
                goto SKIPAMPLIFIER;
            }
        }
        else
        {
            // use the function group's default
            if (NULL == pFG)
            {
                SLOG(eError, _T("Error: HDMI/DisplayPort pin complex's parent function group is not an audio function group!"));
                fRes = FALSE;
                goto SKIPAMPLIFIER;
            }
            else
            {
                if (pFG->m_OutAmpProps.Valid)
                {
                    amp = pFG->m_OutAmpProps.Response; // got it
                }
                else
                {
                    SLOG(eError, _T("Error: Neither the HDMI/DisplayPort pin complex nor the parent function group have valid output amplifier parameters"));
                    fRes = FALSE;
                    goto SKIPAMPLIFIER;
                }
            }
        }

        // OK, got it
        LogAmplifierProperties(eInfo1, amp);
    }
SKIPAMPLIFIER:

    if (!azAudioCaps.Digital)
    {
        SLOG(eError, _T("Error: HDMI/DisplayPort widget doesn't have the Digital bit set in audio capabilities"));
        fRes = FALSE;
    }

    //
    // SANITY CHECKS: Config Default
    //
    if (0 != azConfig.JackDetectOverride)
    {
        SLOG(eError, _T("Error: Jack Detect Override makes sense only for analog pin widgets"));
        fRes = FALSE;
    }

    if (ePcxConnTypeOtherDigital != azConfig.ConnType)
    {
        SLOG(eError,
            _T("Error: HDMI/DisplayPort widget has Connection Type %u (%s); expected %u (%s)"),
            azConfig.ConnType, Stringify::ConnectionType(azConfig.ConnType),
            ePcxConnTypeOtherDigital, Stringify::ConnectionType(ePcxConnTypeOtherDigital)
        );
        fRes = FALSE;
    }

    if (ePcxDefDevDigitalOtherOut != azConfig.DefDev)
    {
        SLOG(eError,
            _T("Error: HDMI/DisplayPort widget has Default Device %u (%s); expected %u (%s)"),
            azConfig.DefDev, Stringify::DefaultDevice(azConfig.DefDev),
            ePcxDefDevDigitalOtherOut, Stringify::ConnectionType(ePcxDefDevDigitalOtherOut)
        );
        fRes = FALSE;            
    }

    if (ePcxGenLocInternal != azConfig.GenLoc || ePcxGeoLocSpecial1 != azConfig.GeoLoc)
    {
        SLOG(eError,
            _T("Error: HDMI/DisplayPort widget has Gross Location %u (%s) ")
            _T("and Geometric Location %u (%s); expected %u (%s) and %u (%s) ")
            _T("per the Location table for section 7.3.3.31 in the HD Audio specification"),
            azConfig.GenLoc, Stringify::GrossLocation(azConfig.GenLoc),
            azConfig.GeoLoc, Stringify::GeometricLocation(azConfig.GeoLoc),
            ePcxGenLocInternal, Stringify::GrossLocation(ePcxGenLocInternal),
            ePcxGeoLocSpecial1, Stringify::GeometricLocation(ePcxGeoLocSpecial1)
        );
        fRes = FALSE;            
    }

    if (ePcxPortConnJack != azConfig.PortConn)
    {
        SLOG(eError,
            _T("Error: HDMI/DisplayPort widget has Port Connectivity %u (%s) - expected %u (%s)"),
            azConfig.PortConn, Stringify::PortConnectivity(azConfig.PortConn),
            ePcxPortConnJack, Stringify::PortConnectivity(ePcxPortConnJack)
        );
        fRes = FALSE;
    }

    // further checks apply only to codecs that meet the criteria for AUDIO-0078
    if (NULL == pFG)
    {
        SLOG(eError, "HDMI pin complex has NULL for a parent function group!");
        fRes = FALSE;
        goto DONE;
    }
    else if (!pFG->m_SupportsHDMIPerAudio0078)
    {
        XLOG(XWARN, eWarn1,
            "HDMI pin complex does not implement any of the verbs in AUDIO-0078. "
            "Skipping further HDMI verification."
        );
        goto DONE;
    }

    //
    // 7.3.3.41 - Audio Sample Packet channel mapping
    //

    bool bAnyNonZero = false;
        
    // get the default mappings
    for (USHORT i = 0; i < 8; i++)
    {
        azPayload.SlotNum = i;
        Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetCnvChMap, azPayload, 0);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);

        if (!azrResponse.Valid)
        {
            SLOG(eError, "Error: getting ASP channel mapping for slot %u resulted in invalid response", i);
            fRes = FALSE;
            continue;
        }

        if (0 != (azrResponse.Response & 0xffffff00))
        {
            SLOG(eError,
                "Error: reserved bits set in response (0x%08x) to ASP channel mapping for slot %u",
                azrResponse.Response, i
            );
            fRes = FALSE;
        }

        if (0 != azrResponse.Response)
        {
            bAnyNonZero = true;
        }

        AzCnvChMap azMap = azrResponse.Response;

        // all-zero responses just mean the verb is not supported, which is OK
        if (bAnyNonZero && (i != azMap.SlotNum))
        {
            SLOG(eError, "Error: Asking for the ASP channel mapping for slot %u got back slot %u", i, azMap.SlotNum);
            fRes = FALSE;
        }

        // ASP slot 2 should map to converter channel 3
        // ASP slot 3 should map to converter channel 2
        USHORT iExpected = i;
        switch (i)
        {
            case 2: iExpected = 3; break;
            case 3: iExpected = 2; break;

            default: break;
        }

        if (azMap.SpeakerNum == iExpected)
        {
            SLOG(eInfo1, "ASP channel mapping slot %u maps to converter channel %u as expected.", i, azMap.SpeakerNum);
        }
        else if (bAnyNonZero)
        {
            SLOG(eError,
                "Error: ASP channel mapping slot %u maps to converter channel %u instead of expected %u",
                i, azMap.SpeakerNum, iExpected
            );
            fRes = FALSE;
        }
        else if (i != 0) // the expected response for channel 0 is indistinguishable from "not supported"
        {
            SLOG(eError, "Error: HDMI/DisplayPort pin widget does not support channel mapping verb (7.3.3.41 in HD Audio DCN)");
        }
    }

    // only bother checking this verb further if we got a sign of life out of it
    if (!bAnyNonZero)
    {
        if (azPinCaps.HBR)
        {
            SLOG(eError,
                "Error: Pin claiming High Bit Rate support did not respond to any converter channel verbs"
            );  
            fRes = FALSE;
        }
        goto SKIPCHANNELMAPPING;
    }

    // verify turning each channel on and off works
    for (USHORT i = 0; i < 8; i++)
    {
        // ASP slot 2 should map to converter channel 3
        // ASP slot 3 should map to converter channel 2
        USHORT iExpected = i;
        switch (i)
        {
            case 2: iExpected = 3; break;
            case 3: iExpected = 2; break;

            default: break;
        }

        // turn it off first
        AzCnvChMap azMap;
        azMap.SlotNum = i;
        azMap.SpeakerNum = 0xf; // that is, "off" (do not play this channel anywhere)

        Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetCnvChMap, azMap, 0);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);

        if (!azrResponse.Valid)
        {
            SLOG(eError, "Error: setting slot %u mapping to 0xF resulted in an invalid response", i);
            fRes = FALSE; // soldier on anyway
        }

        // see if it stuck
        azPayload.SlotNum = i;
        Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetCnvChMap, azPayload, 0);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);

        if (azrResponse.Valid)
        {
            azMap = azrResponse.Response;
            if (bAnyNonZero && (i != azMap.SlotNum || 0xf != azMap.SpeakerNum)) // bAnyNonZero means the verb is supported
            {
                SLOG(eError,
                    "Error: got back slot number %u and converter channel %u; expected %u and %u",
                    azMap.SlotNum, azMap.SpeakerNum,
                    i, 0xf
                );
                fRes = FALSE;
            }
        }
        else
        {
            SLOG(eError, "Error: getting ASP 0xF channel mapping for slot %u resulted in invalid response", i);
            fRes = FALSE; // soldier on
        }

        // now put it back
        azMap.SlotNum = i;
        azMap.SpeakerNum = iExpected;

        Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetCnvChMap, azMap, 0);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);

        if (!azrResponse.Valid)
        {
            SLOG(eError, "Error: setting slot %u mapping to %u resulted in an invalid response", i, iExpected);
            fRes = FALSE; // soldier on anyway
        }

        // see if it stuck
        azPayload.SlotNum = i;
        Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetCnvChMap, azPayload, 0);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);

        if (azrResponse.Valid)
        {
            azMap = azrResponse.Response;
            if (bAnyNonZero && (i != azMap.SlotNum || iExpected != azMap.SpeakerNum))
            {
                SLOG(eError,
                    "Error: got back slot number %u and converter channel %u; expected %u and %u",
                    azMap.SlotNum, azMap.SpeakerNum,
                    i, iExpected
                );
                fRes = FALSE;
            }
        }
        else
        {
            SLOG(eError, "Error: getting ASP expected channel mapping for slot %u resulted in invalid response", i);
            fRes = FALSE; // soldier on
        }
    } // for each channel
SKIPCHANNELMAPPING:

    //
    // 7.3.3.40 - Content Protection
    //
    if (!azAudioCaps.CPCapable)
    {
        goto SKIPCONTENTPROTECTION;
    }

    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetCPState, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    if (azrResponse.Valid)
    {
        AzCPState azCPState = azrResponse.Response;

        SLOG(eInfo1,
            "--- CONTENT PROTECTION STATE ---\n"
            "Current request state: %u\n"
            "Reserved (bit 2): %u\n"
            "Unsolicited Response sub tag: 0x%x\n"
            "Ready: %u\n"
            "Current encryption state: %u\n"
            "Reserved (bits 10 - 31): 0x%x",
            azCPState.RequestedState,
            azCPState.Reserved0,
            azCPState.SubTag,
            azCPState.Ready,
            azCPState.CurrentState,
            azCPState.Reserved1
        );

        if (0 != azCPState.Reserved0 || 0 != azCPState.Reserved1)
        {
            SLOG(eError,
                "Error: Reserved bits set in Content Protection State response: bit 2 = %u, bits 10-31 = 0x%x",
                azCPState.Reserved0, azCPState.Reserved1
            );
            fRes = FALSE;
        }

    }
    else
    {
        SLOG(eError, "Error: getting content protection state resulted in invalid response.");
        fRes = FALSE;
    }
SKIPCONTENTPROTECTION:

    //
    // 7.3.3.34 - EDID-Like Data (ELD) Data
    // 7.3.3.36 - Data Island Packet - Size info
    while(true)
    {
        // see if the "Extended display identification data"-like data is valid
        Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetPinSense, 0);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
        if (azrResponse.Valid)
        {
            azPinSense = azrResponse.Response;
            SLOG(eInfo1,
                "Pin sense response for digital pin widget is: "
                "Presence Detect = %u, ELD Valid = %u, Reserved = 0x%0x",
                azPinSense.PresenceDetect, azPinSense.ELDValid, azPinSense.Reserved0
                );
        }
        else
        {
            SLOG(eError, "Error: sending down a get pin sense verb resulted in an invalid response.");
            fRes = FALSE;
            goto SKIPELD;
        }

        if(!azPinSense.ELDValid)
        {
#ifdef WHQL
            // give the user a chance to plug in the output
            // this might be a system that has more outputs than it can power simultaneously
            // for example, there might be three HDMI outputs
            // but only enough display resources to drive two external displays
            LPSTR szUnplugged = "ELD Valid bit is not set. Please attach an HDMI or DisplayPort Device\n"
                "Please attach it and press RETRY\n"
                "Or press CANCEL to FAIL the test";

            int iRetVal = MessageBox(g_IShell->m_hwndShell, 
                szUnplugged,
                "Unplugged Digital Output",
                MB_RETRYCANCEL);

            if (IDRETRY == iRetVal)
            {
                //user indicated that they wish to retry
                SLOG(eInfo1, "Retrying after user plugged in HDMI or DisplayPort device...");
                continue;
            }
            else if (IDCANCEL == iRetVal)
            {
                //user has pressed cancel, fail the test
                SLOG(eError, "Error: HDMI is unplugged and user canceled test. Failing Test.");
                fRes = FALSE;
                goto SKIPELD;
            }
#else
            // internal build of UAA Test used in automation
            // don't pop up any UI
            SLOG(eError, "Error: HDMI is unplugged. Failing Test.");
            fRes = FALSE;
            goto SKIPELD;
#endif // WHQL
        }
        else
        {
            //ELD is valid
            break;
        }
    }

    // get the size of the ELD
    azDIPSizePayload.EldInx = 1;
    azDIPSizePayload.PacketInx = 0;
    azDIPSizePayload.Reserved0 = 0;

    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetDIPSize, azDIPSizePayload);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    if (azrResponse.Valid)
    {
        azDIPSize = azrResponse.Response;
        if (azrResponse.Response & 0xffffff00)
        {
            SLOG(eError, "Error: reserved bits set in response to query for ELD buffer size (0x%08x)", azrResponse.Response);
            fRes = FALSE;
        }
        cbELDSize = azDIPSize.DipSize + 1; // DIP size is 0-based
    }
    else
    {
        SLOG(eError, "Error: sending down a get ELD buffer size verb resulted in an invalid response.");
        fRes = FALSE;
        goto SKIPELD;
    }

    if (4 > cbELDSize)
    {
        SLOG(eError,
            "Error: the ELD size is reported as %u bytes "
            "which isn't even enough for the ELD header. Will read ELD anyway.",
            cbELDSize
        );
        fRes = FALSE;
    }
    SLOG(eInfo1, "ELD size response is %u bytes (0-based)", azDIPSize.DipSize);

    // while we're at it, get the size of the other DIP slots
    for (int i = 0; i <= 7; i++)
    {
        azDIPSizePayload.EldInx = 0;
        azDIPSizePayload.PacketInx = i;

        Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetDIPSize, azDIPSizePayload);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
        if (azrResponse.Valid)
        {
            azDIPSize = azrResponse.Response;
            if (azrResponse.Response & 0xffffff00)
            {
                SLOG(eError,
                    "Error: reserved bits set in response to query for Data Island Packet %u size (0x%08x)",
                    i, azrResponse.Response
                );
                fRes = FALSE;
            }
            SLOG(eInfo1, "Data Island Packet %u size is %u bytes (0-based)", i, azDIPSize.DipSize);
        }
        else
        {
            SLOG(eError, "Error: sending down a get Data Island Packet size verb (index %u) resulted in an invalid response.", i);
            fRes = FALSE;
        }
    }

    // ELD step one of three; get the header block
    for (int i = 0; i <= 3; i++)
    {
        AzGetELDDataPayload azELDPayload;
        azELDPayload.Offset = i;
        
        Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetELDData, azELDPayload);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
        if (azrResponse.Valid)
        {
            AzELDData azELDData = azrResponse.Response;
            if (azELDData.Reserved0)
            {
                SLOG(eError,
                    "Error: getting ELD header byte %u resulted in reserved bits being set 0x%08x",
                    i, azrResponse.Response
                );
                fRes = FALSE;
            }

            if (!azELDData.Valid)
            {
                SLOG(eError, "Error: ELD is no longer valid getting ELD header byte %u", i);
                fRes = FALSE;
            }

            rbELDHeader[i] = azELDData.Data;
        }
        else
        {
            SLOG(eError, "Error: getting ELD header byte %u resulted in invalid response", i);
            fRes = FALSE;
        }
    }

    // parse the data
    SLOG(eInfo1,
        "--- ELD Header ---\n"
        "Reserved (bits 0-2): 0x%x\n"
        "ELD version: %u\n"
        "Reserved (byte 1): 0x%x\n"
        "Baseline ELD length: %u\n"
        "Reserved (byte 3): 0x%x",
        rbELDHeader[0] & 0x7,
        rbELDHeader[0] >> 3,
        rbELDHeader[1],
        rbELDHeader[2],
        rbELDHeader[3]
    );

    cbELDBaselineBlockSize = 4 * rbELDHeader[2];
    
    if (0 != (rbELDHeader[0] & 0x7))
    {
        SLOG(eError, "Error: first three bits of ELD data must be 0, not 0x%x", rbELDHeader[0] & 0x3);
        fRes = FALSE;
    }

    if (0 != rbELDHeader[1])
    {
        SLOG(eError, "Error: second byte of ELD data must be 0, not 0x%x", rbELDHeader[1]);
        fRes = FALSE;
    }
    
    if (0 != rbELDHeader[3])
    {
        SLOG(eError, "Error: fourth byte of ELD data must be 0, not 0x%x", rbELDHeader[3]);
        fRes = FALSE;
    }

    vELDVersion = rbELDHeader[0] >> 3;

    if (2 != vELDVersion && 0x1f != vELDVersion)
    {
        SLOG(eError, "Error: ELD version (0x%x) not supported; needs to be 2 or 0x1f", vELDVersion);
        fRes = FALSE;
        goto SKIPELD;
    }

    // ELD step two of three; get the baseline block
    if (0 == cbELDBaselineBlockSize)
    {
        SLOG(eError, "Error: baseline ELD length is 0. Skipping the rest of the ELD");
        fRes = FALSE;
        goto SKIPELD; // can't proceed
    }
    if (4 + cbELDBaselineBlockSize > cbELDSize)
    {
        SLOG(eError,
            "Error: header size (4) plus ELD baseline block size (%u) is bigger than purported ELD size (%u)",
            cbELDBaselineBlockSize, cbELDSize
        );
        fRes = FALSE;
    }
    if (2 == vELDVersion && 80 < cbELDBaselineBlockSize)
    {
        SLOG(eError,
            "Error: ELD version 2 allows for a baseline ELD block of 80 bytes, but this is %u bytes",
            cbELDBaselineBlockSize
        );
        fRes = FALSE;
    }
    pbELDBaselineBlock = new BYTE[ 4 * rbELDHeader[2] ];
    if (NULL == pbELDBaselineBlock)
    {
        SLOG(eError, "Error: out of memory allocating %u bytes for ELD baseline block", rbELDHeader[2]);
        fRes = FALSE;
        goto SKIPELD;
    }
    // initialize
    for (int i = 0; i < 4 * rbELDHeader[2]; i++) { pbELDBaselineBlock[i] = 0; }
    // pull from device
    for (int i = 0; i < cbELDBaselineBlockSize; i++)
    {
        AzGetELDDataPayload azELDPayload;
        azELDPayload.Offset = 4 + i;
        
        Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetELDData, azELDPayload);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
        if (azrResponse.Valid)
        {
            AzELDData azELDData = azrResponse.Response;
            if (azELDData.Reserved0)
            {
                SLOG(eError,
                    "Error: getting ELD baseline block byte %u (ELD byte %u) "
                    "resulted in reserved bits being set 0x%08x",
                    i, 4 + i,
                    azrResponse.Response
                );
                fRes = FALSE;
            }

            if (!azELDData.Valid)
            {
                SLOG(eError,
                    "Error: ELD is no longer valid getting ELD baseline byte %u (ELD byte %u)",
                    i, 4 + i
                );
                fRes = FALSE;
            }

            pbELDBaselineBlock[i] = azELDData.Data;
        }
        else
        {
            SLOG(eError,
                "Error: getting ELD header byte %u (ELD byte %u) resulted in invalid response",
                i, 4 + i
            );
            fRes = FALSE;
        }
    }
    
    // log the raw bytes for posterity
    LogBytes(eInfo1, "--- ELD Baseline Data ---\n", pbELDBaselineBlock, cbELDBaselineBlockSize);

    // TODO: once there is hardware available that actually sends ELDs,
    // do further checks
    
    // ELD step three of three; get the vendor block
    if (cbELDSize > 4 + cbELDBaselineBlockSize)
    {
        cbELDVendorBlockSize = cbELDSize - 4 - cbELDBaselineBlockSize;

        pbELDVendorBlock = new BYTE[ cbELDVendorBlockSize ];
        if (NULL == pbELDVendorBlock)
        {
            SLOG(eError, "Error: ran out of memory assigning %u bytes for the vendor block", cbELDVendorBlockSize);
            fRes = FALSE;
            goto SKIPELD;
        }

        for (int i = 0; i < cbELDVendorBlockSize; i++) { pbELDVendorBlock[i] = 0; }
        for (int i = 0; i < cbELDVendorBlockSize; i++)
        {
            AzGetELDDataPayload azELDPayload;
            azELDPayload.Offset = 4 + cbELDBaselineBlockSize + i;
            
            Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetELDData, azELDPayload);
            azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
            if (azrResponse.Valid)
            {
                AzELDData azELDData = azrResponse.Response;
                if (azELDData.Reserved0)
                {
                    SLOG(eError,
                        "Error: getting ELD vendor block byte %u (ELD byte %u) "
                        "resulted in reserved bits being set 0x%08x",
                        i, 4 + cbELDBaselineBlockSize + i,
                        azrResponse.Response
                    );
                    fRes = FALSE;
                }

                if (!azELDData.Valid)
                {
                    SLOG(eError,
                        "Error: ELD is no longer valid getting ELD vendor byte %u (ELD byte %u)",
                        i, 4 + cbELDBaselineBlockSize + i
                    );
                    fRes = FALSE;
                }

                pbELDVendorBlock[i] = azELDData.Data;
            }
            else
            {
                SLOG(eError,
                    "Error: getting ELD vendor byte %u (ELD byte %u) resulted in invalid response",
                    i, 4 + cbELDBaselineBlockSize + i
                );
                fRes = FALSE;
            }
        }
        // log it for posterity
        LogBytes(eInfo1, "--- ELD Vendor Block ---", pbELDVendorBlock, cbELDVendorBlockSize);
    }
SKIPELD:

DONE:
    delete [] pbELDBaselineBlock;
    delete [] pbELDVendorBlock;
    
    return fRes;
}

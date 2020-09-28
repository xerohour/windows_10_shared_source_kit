/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#include <windows.h>
#include <math.h>
#include <tchar.h>
#include <atlstr.h>

#include <shell98.h>
#include <module98.h>
#include <dvmodule.h>
#include <cfgmgr32.h>
#include <setupapi.h>
#include <list>

#define USERMODE // or azcodec.h includes wdm.h (kernel mode version of windows.h)
#include <azcodec.h>
#include <privioct.h>
#include <controller-interface.h>

#include "aznode.h"
#include "install.h"
#include "uaatest.h"
#include "power.h"
#include "codec.h"
#include "buffer.h"
#include "stringify.h"
#include "log.h"

#define USERMODE
#include "privioct.h"
#include "azverbutils.h"

#define MAXSTREAMS 16
#define MAXCHANNELS 16

const UINT64 VREF_CAPS_MASK = 0xff00; // VRef bits from PinCaps: HD Audio 7.3.4.9

//
// Helper classes
//
BOOL 
CAssoc::Add(CPinComplex* pPC)
{
    m_usNumPins++;
    m_listPC.AddTail(pPC);

    return TRUE;
}

BOOL 
CAssoc::Validate()
{
    BOOL fRes = TRUE;

    SLOG(eInfo1, "Validating Association %01Xh", m_usAssoc);

    TTNode<CPinComplex>* pttnodePC = m_listPC.GetHead();

    CPinComplex* pPinComplex = pttnodePC->pData;
    AzPinWidgetConfigDefault azConfigDefault = pPinComplex->m_ConfigDef.Response;
    AzPinCaps azPinCaps = pPinComplex->m_PinCaps.Response;
    AzAudioWidgetCaps azWidgetCaps = pPinComplex->m_AudWidgetCaps.Response;

    // single pin device
    if (1 == m_usNumPins)
    {
        if (0xF != m_usAssoc)
        {
            XLOG(XWARN, eWarn1, "Warning: Association 0xF instead of %08Xh is recommended for single-pin devices", m_usAssoc);
        }

        if (0 != azConfigDefault.Sequence)
        {
            SLOG(eError, "Error: Associations with only one pin should set Sequence == 0, not %u", azConfigDefault.Sequence);
            fRes = FALSE;
        }
        switch(azConfigDefault.DefDev)
        {
            case ePcxDefDevLineOut:
            case ePcxDefDevSpeaker:
            case ePcxDefDevHPOut:
            case ePcxDefDevSpdifOut:
                // validate single-pin render
                SLOG(eInfo1, "Known single pin render device type %01Xh", azConfigDefault.DefDev);
            break;

            case ePcxDefDevDigitalOtherOut:
                if (
                    ePcxGenLocInternal == azConfigDefault.GenLoc &&
                    ePcxGeoLocSpecial1 == azConfigDefault.GeoLoc
                )
                {
                    // perfectly good HDMI or displayport device.
                    SLOG(eInfo1, "Known single-pin render device type: digital display pin.");
                    
                    if (azPinCaps.HDMICapable)
                    {
                        SLOG(eInfo1, "Digital display pin can do HDMI.");
                    }

                    if (azPinCaps.DPCapable)
                    {
                        SLOG(eInfo1, "Digital display pin can do DisplayPort.");
                        if (!azPinCaps.HDMICapable)
                        {
                            XLOG(XWARN, eWarn1, "Digital display pin can do DisplayPort but not HDMI.\n");
                        }
                    }
                }
                else if (
                    azPinCaps.HDMICapable ||
                    azPinCaps.DPCapable
                )
                {
                    // display pin or not?
                    SLOG(eError,
                        "Configuration Default \"default device\" for this pin widget is\n"
                        "Digital Other Out, but the location bits are not set to\n"
                        "internal / \"special (8)\" so this is not a digital display.\n"
                        "Instead, they are %u (%s) / %u (%s).\n"
                        "However, this is an HDMI capable (%u) or DisplayPort capable (%u) pin.",
                        azConfigDefault.GenLoc,
                            Stringify::GrossLocation(azConfigDefault.GenLoc),
                        azConfigDefault.GeoLoc,
                            Stringify::GeometricLocation(azConfigDefault.GeoLoc),
                        azPinCaps.HDMICapable, azPinCaps.DPCapable
                    );
                    fRes = FALSE;
                }
                else
                {
                    // unsupported single-pin device
                    SLOG(eError,
                        "Error: unknown single single pin device type: "
                        " default device is %01Xh (%s) but "
                        "\"HDMI Capable\" bit is %u, "
                        "\"DisplayPort Capable\" bit is %u, and "
                        "the general and geographic locations are %u (%s) and %u (%s)",
                        ePcxDefDevDigitalOtherOut, Stringify::DefaultDevice(ePcxDefDevDigitalOtherOut),
                        azPinCaps.HDMICapable, azPinCaps.DPCapable, 
                        azConfigDefault.GenLoc,
                            Stringify::GrossLocation(azConfigDefault.GenLoc),
                        azConfigDefault.GeoLoc,
                            Stringify::GeometricLocation(azConfigDefault.GeoLoc)
                    );
                    fRes = FALSE;
                }
            break;

            case ePcxDefDevLineIn:
            case ePcxDefDevAUX:
            case ePcxDefDevCD: // CD in - capture from CD player
            case ePcxDefDevMicIn:
            case ePcxDefDevSpdifIn:
                // validate single-pin capture
                SLOG(eInfo1, "Known single pin capture device type %01Xh", azConfigDefault.DefDev);                
            break;

            case ePcxDefDevOther:
                // can't fail - could be a PC Beep capture pin.
                XLOG(XWARN, eWarn1, "Warning: single-pin capture device has default device \"Other\" and "
                             "will not be supported by the Microsoft class driver.");
            break;

            default:
                // unsupported single-pin device
                SLOG(eError, "Error: Unsupported single pin device type %01Xh (%s)",
                    azConfigDefault.DefDev, Stringify::DefaultDevice(azConfigDefault.DefDev)
                );
                fRes = FALSE;
        }
        // all single-pin 1/8" analog jacks must provide jack-presence detection
        if (ePcxPortConnJack == azConfigDefault.PortConn)
        {            
            if (ePcxConnTypeEighth == azConfigDefault.ConnType)
            {                
                if ((1 != azPinCaps.PresenceDetect) || 
                    (0 != azConfigDefault.JackDetectOverride) || 
                    (1 != azWidgetCaps.UnSolCapable))
                {
                    SLOG(eError, "Error: Pin complex at node %u does not provide jack-presence detection\n"
                                 "All single-pin 1/8\" analog jacks must provide jack-presence detection\n"
                                 "  Presence Detect bit == 1  (Current value == %u)\n"
                                 "  JackDetectOverride bit == 0  (Current value == %u)\n"
                                 "  UnSolCapable bit == 1  (Current value == %u)", 
                                 pPinComplex->m_usNode, 
                                 azPinCaps.PresenceDetect, 
                                 azConfigDefault.JackDetectOverride, 
                                 azWidgetCaps.UnSolCapable);
                    fRes = FALSE;
                }
            }
        }
    }
    // multi-pin device
    else
    {
        switch(azConfigDefault.DefDev)
        {
            // SPDIF In/Out
            case ePcxDefDevSpdifOut:
            case ePcxDefDevSpdifIn:
                // SPDIF In/Out must be an independent device
                SLOG(eError, "Error: SPDIF In or SPDIF Out must not be associated with any other pins");
                fRes = FALSE;
            break;

            // Multichannel render (line-out, speakers, or redirected HP)
            case ePcxDefDevHPOut:
            case ePcxDefDevLineOut:
            case ePcxDefDevSpeaker:
                fRes = ValidateMultiRender() ? fRes : FALSE;
            break;

            // Multichannel capture (Mix or mux)
            case ePcxDefDevLineIn:
            case ePcxDefDevAUX:
            case ePcxDefDevMicIn:
            case ePcxDefDevCD:
                fRes = ValidateMultiCapture() ? fRes : FALSE;
            break;

            default:
                // unsupported multi-pin device
                SLOG(eError,
                    "Error: Unsupported multi-pin default device type %01Xh (%s)\r\n"
                    "This device will be unrecognized by the Microsoft HD Audio class driver",
                    azConfigDefault.DefDev, Stringify::DefaultDevice(azConfigDefault.DefDev)
                );
                fRes = FALSE;
        }
    }

    return fRes;
}

BOOL 
CAssoc::ValidateMultiRender()
{
    BOOL fRes = TRUE;
    BOOL fLineOut = FALSE;    
    BOOL fRedirHP = FALSE;
    BOOL fRedirIntSpeaker = FALSE;
    BOOL fExtSpeaker = FALSE;

    // using this because we need the jacks in order by sequence number and 
    //  TTList <CPinComplex>m_listPC does not support sorting or inserting
    using namespace std;
    list<USHORT> listSequence;  // sorted list of sequence numbers, used to index into rgJacks[]
    CPinComplex* rgJacks[16] = {0};   // 4-bit sequence number used for direct addressing: Hence, array size == 16


    SLOG(eInfo1, "Validating multi-pin render device");

    // input and output widgets can not be combined in the same association
    for (
        TTNode<CPinComplex>* pttnodePC = m_listPC.GetHead();
        NULL != pttnodePC;
        pttnodePC = m_listPC.GetNext(pttnodePC)
    )
    {        
        CPinComplex *pPinComplex = pttnodePC->pData;
        AzPinWidgetConfigDefault azConfigDefault = pPinComplex->m_ConfigDef.Response;
        AzPinCaps azPinCaps = pPinComplex->m_PinCaps.Response;
        AzAudioWidgetCaps azWidgetCaps = pPinComplex->m_AudWidgetCaps.Response;

        // keep track of connectors and associated list for connector ordering
        listSequence.push_back(azConfigDefault.Sequence);
#pragma prefast( suppress: __WARNING_POTENTIAL_BUFFER_OVERFLOW_HIGH_PRIORITY, "PREFast doesn't realize a 4-bit member can't be more than 0b1111 == 15")
        rgJacks[azConfigDefault.Sequence] = pPinComplex;
        
        switch (azConfigDefault.DefDev)
        {
            case ePcxDefDevLineIn:
            case ePcxDefDevCD:
            case ePcxDefDevAUX:
            case ePcxDefDevMicIn:
            case ePcxDefDevSpdifIn:
            case ePcxDefDevDigitalOtherIn:
                SLOG(eError, "Error: Pin complex at node %u is an input pin", pPinComplex->m_usNode);
                SLOG(eError, "Error: Input and Output pins can not be combined in the same association");
                fRes = FALSE;
            break;

            case ePcxDefDevHPOut:
                if (fRedirHP)
                {
                    SLOG(eError, "Error: Association can only have one pin of type HPOut");
                    fRes = FALSE;
                }
                fRedirHP = TRUE;
                if (0xF != azConfigDefault.Sequence)
                {
                    SLOG(eError, "Error: HPOut pin should have Sequence=0xF in a Redirected HP Device");
                    fRes = FALSE;
                }
                if ((1 != azPinCaps.PresenceDetect) || 
                    (0 != azConfigDefault.JackDetectOverride) || 
                    (1 != azWidgetCaps.UnSolCapable))
                {
                    SLOG(eError, "Error: Pin complex at node %u does not provide jack-presence detection\n"
                                 "All redirected headphone jacks must provide jack-presence detection\n"
                                 "  Presence Detect bit == 1  (Current value == %u)\n"
                                 "  JackDetectOverride bit == 0  (Current value == %u)\n"
                                 "  UnSolCapable bit == 1  (Current value == %u)", 
                                 pPinComplex->m_usNode, 
                                 azPinCaps.PresenceDetect, 
                                 azConfigDefault.JackDetectOverride, 
                                 azWidgetCaps.UnSolCapable);
                    fRes = FALSE;
                }
            break;

            case ePcxDefDevLineOut:
                fLineOut = TRUE;
                if (fExtSpeaker)
                {
                    SLOG(eError, "Error: Can not mix Line-out and external Speaker pins within the same association");
                    fRes = FALSE;
                }
            break;

            case ePcxDefDevSpeaker:
                if (fRedirIntSpeaker)
                {
                    SLOG(eError, "Error: Association can only have one Internal Speaker)");
                    fRes = FALSE;
                }
                if (ePcxGenLocInternal == azConfigDefault.GenLoc)
                {
                    fRedirIntSpeaker = TRUE;
                }
                else
                {
                    fExtSpeaker = TRUE;
                }
                if (fLineOut && !fRedirIntSpeaker)
                {                    
                    SLOG(eError, "Error: Can not mix Line-out and external Speaker pins within the same association");
                    fRes = FALSE;
                }
            break;

            default:
                // unsupported multi-pin render device
                SLOG(eError,
                    "Error: Unsupported default device type %01Xh (%s) in a multi-pin render association\r\n"
                    "This device will be unrecognized by the Microsoft HD Audio class driver",
                    azConfigDefault.DefDev, Stringify::DefaultDevice(azConfigDefault.DefDev)
                );
                fRes = FALSE;
        }
    } // next pin complex

    if ((fRedirHP && fLineOut && (fExtSpeaker || fRedirIntSpeaker)) ||
        ((fExtSpeaker && fRedirIntSpeaker) && (fRedirHP || fLineOut)))
    {
        SLOG(eError, "Error: Redirected Outputs can only have one alternate destination");
        fRes = FALSE;
    }
    
    listSequence.sort();
    list<USHORT>::const_iterator cIter;
    cIter = listSequence.begin();
    CPinComplex *pPinComplex = rgJacks[*cIter];
    AzPinWidgetConfigDefault azConfigDefault = pPinComplex->m_ConfigDef.Response;

    // only for 1/8" analog jacks
    if (ePcxConnTypeEighth == azConfigDefault.ConnType)
    {
        // only on the rear panel
        // ensure color-coding is consistent with WLP requirement SYSFUND-0041
        if ((ePcxGenLocPrimBox != azConfigDefault.GenLoc) && 
            (ePcxGeoLocRear == azConfigDefault.GeoLoc))
        {            
            // >=5.1 (+1 for Redirected HP)
            if (3 + (fRedirHP ? 1 : 0) == m_usNumPins)
            {
                pPinComplex = rgJacks[*cIter];
                azConfigDefault = pPinComplex->m_ConfigDef.Response;
                if (ePcxColorGreen != azConfigDefault.Color)
                {
                    SLOG(eError, "Error: Front L/R output connector must be color-coded lime green");
                    fRes = FALSE;
                }
                cIter++;
                pPinComplex = rgJacks[*cIter];
                azConfigDefault = pPinComplex->m_ConfigDef.Response;
                if (ePcxColorOrange != azConfigDefault.Color)
                {
                    SLOG(eError, "Error: Center/suboutput connector must be color-coded orange");
                    fRes = FALSE;
                }
                cIter++;
                pPinComplex = rgJacks[*cIter];
                azConfigDefault = pPinComplex->m_ConfigDef.Response;
                if (2 == *cIter)
                {
                    XLOG(XWARN, eWarn1, "Warning: 5.1 configurations should use Side L/R instead of Rear L/R");
                    if (ePcxColorBlack != azConfigDefault.Color)
                    {
                        SLOG(eError, "Error: Rear L/R connector must be color-coded black");
                        fRes = FALSE;
                    }
                }
                else if (4 == *cIter)
                {
                    if (ePcxColorGrey != azConfigDefault.Color)
                    {
                        SLOG(eError, "Error: Side L/R connector must be color-coded grey");
                        fRes = FALSE;
                    }
                }
                else
                {
                    SLOG(eError, "Error: Unsupported speaker configuration sequence number %u", *cIter);
                    fRes = FALSE;
                }
            }

            // 7.1 (+1 for Redirected HP)
            if (4 + (fRedirHP ? 1 : 0) == m_usNumPins)
            {
                pPinComplex = rgJacks[*cIter];
                azConfigDefault = pPinComplex->m_ConfigDef.Response;
                if (ePcxColorGreen != azConfigDefault.Color)
                {
                    SLOG(eError, "Error: Front L/R output connector must be color-coded lime green");
                    fRes = FALSE;
                }
                cIter++;
                pPinComplex = rgJacks[*cIter];
                azConfigDefault = pPinComplex->m_ConfigDef.Response;
                if (ePcxColorOrange != azConfigDefault.Color)
                {
                    SLOG(eError, "Error: Center/suboutput connector must be color-coded orange");
                    fRes = FALSE;
                }
                cIter++;
                pPinComplex = rgJacks[*cIter];
                azConfigDefault = pPinComplex->m_ConfigDef.Response;
                if (2 == *cIter)
                {
                    if (ePcxColorBlack != azConfigDefault.Color)
                    {
                        SLOG(eError, "Error: Rear L/R connector must be color-coded black");
                        fRes = FALSE;
                    }
                    cIter++;
                    pPinComplex = rgJacks[*cIter];
                    azConfigDefault = pPinComplex->m_ConfigDef.Response;
                    // 7.1 wide config (obsolete)
                    if (3 == *cIter)
                    {
                        XLOG(XWARN, eWarn1, "Warning: Obsolete 7.1 wide configuration");                        
                    }
                    // 7.1 home theater
                    else if (4 == *cIter)
                    {
                        if (ePcxColorGrey != azConfigDefault.Color)
                        {
                            SLOG(eError, "Error: Side L/R connector must be color-coded grey");
                            fRes = FALSE;
                        }
                    }
                    else
                    {
                        SLOG(eError, "Error: Unsupported speaker configuration sequence number %u", *cIter);
                        fRes = FALSE;
                    }
                }
                // 7.1 wide config (obsolete)
                else if (3 == *cIter)
                {                    
                        XLOG(XWARN, eWarn1, "Warning: Obsolete 7.1 wide configuration");
                }
                else
                {
                    SLOG(eError, "Error: Unsupported speaker configuration sequence number %u", *cIter);
                    fRes = FALSE;
                }
            }        
        }
    }    

    // first 2 channels must have jack-presence detect if PortConn is external
    cIter = listSequence.begin();
    pPinComplex = rgJacks[*cIter];
    azConfigDefault = pPinComplex->m_ConfigDef.Response;
    AzPinCaps azPinCaps = pPinComplex->m_PinCaps.Response;
    AzAudioWidgetCaps azWidgetCaps = pPinComplex->m_AudWidgetCaps.Response;

    if (ePcxPortConnJack == azConfigDefault.PortConn)
    {
        // only for 1/8" analog jacks
        if (ePcxConnTypeEighth == azConfigDefault.ConnType)
        {            
            if ((1 != azPinCaps.PresenceDetect) || 
                (0 != azConfigDefault.JackDetectOverride) || 
                (1 != azWidgetCaps.UnSolCapable))
            {
                SLOG(eError, "Error: Pin complex at node %u does not provide jack-presence detection\n"
                             "First 2 channels in a multi-pin line-out or speaker association must support Presence Detect\n"
                             "  Presence Detect bit == 1  (Current value == %u)\n"
                             "  JackDetectOverride bit == 0  (Current value == %u)\n"
                             "  UnSolCapable bit == 1  (Current value == %u)", 
                             pPinComplex->m_usNode, 
                             azPinCaps.PresenceDetect, 
                             azConfigDefault.JackDetectOverride, 
                             azWidgetCaps.UnSolCapable);
                fRes = FALSE;
            }
        }
    }

    return fRes;
}

BOOL 
CAssoc::ValidateMultiCapture()
{
    BOOL fRes = TRUE;
    BOOL fMux = FALSE;
    BOOL fMix = FALSE;

    SLOG(eInfo1, "Validating multi-pin capture device");
    
    // input and output widgets can not be combined in the same association
    for (
        TTNode<CPinComplex>* pttnodePC = m_listPC.GetHead();
        NULL != pttnodePC;
        pttnodePC = m_listPC.GetNext(pttnodePC)
    )
    {
        CPinComplex* pPinComplex = pttnodePC->pData;
        AzPinWidgetConfigDefault azConfigDefault = pPinComplex->m_ConfigDef.Response;
        AzPinCaps azPinCaps = pPinComplex->m_PinCaps.Response;
        AzAudioWidgetCaps azWidgetCaps = pPinComplex->m_AudWidgetCaps.Response;

        switch (azConfigDefault.DefDev)
        {
            case ePcxDefDevLineOut:
            case ePcxDefDevSpeaker:
            case ePcxDefDevHPOut:
            case ePcxDefDevSpdifOut:
            case ePcxDefDevDigitalOtherOut:
                SLOG(eError, "Error: Pin complex at node %u is an output pin", pPinComplex->m_usNode);
                SLOG(eError, "Error: Input and Output pins can not be combined in the same association");
                fRes = FALSE;
            break;

            case ePcxDefDevLineIn:
            case ePcxDefDevAUX:
            case ePcxDefDevMicIn:
            case ePcxDefDevCD:
                if (0xF == azConfigDefault.Sequence)
                {
                    fMix = TRUE;
                }
                else if (0xE == azConfigDefault.Sequence)
                {
                    fMux = TRUE;
                }
            break;

            default:
                // unsupported multi-pin capture device
                SLOG(eError,
                    "Error: Unsupported default device type %01Xh (%s) in a multi-pin capture association"
                    "This device will be unrecognized by the Microsoft HDAudio class driver",
                    azConfigDefault.DefDev, Stringify::DefaultDevice(azConfigDefault.DefDev)
                );
                fRes = FALSE;
        }

        // all multi-pin 1/8" analog inputs must provide jack-presence detection
        if (ePcxPortConnJack == azConfigDefault.PortConn)
        {            
            if (ePcxConnTypeEighth == azConfigDefault.ConnType)
            {                
                if ((1 != azPinCaps.PresenceDetect) || 
                    (0 != azConfigDefault.JackDetectOverride) || 
                    (1 != azWidgetCaps.UnSolCapable))
                {
                    SLOG(eError, "Error: Pin complex at node %u does not provide jack-presence detection\n"
                                 "All 1/8\" analog inputs in mixed or muxed capture devices must provide jack-presence detection\n"
                                 "  Presence Detect bit == 1  (Current value == %u)\n"
                                 "  JackDetectOverride bit == 0  (Current value == %u)\n"
                                 "  UnSolCapable bit == 1  (Current value == %u)", 
                                 pPinComplex->m_usNode, 
                                 azPinCaps.PresenceDetect, 
                                 azConfigDefault.JackDetectOverride, 
                                 azWidgetCaps.UnSolCapable);
                    fRes = FALSE;
                }
            }
        }
    } // next pin complex

    if (fMix && fMux) {
        SLOG(eError, "Error: Device is marked as both mixed and muxed capture");
        fRes = FALSE;
    }
    else if (fMix)
    {
        SLOG(eInfo1, "Device is mixed-capture");
    }
    else if (fMux)
    {
        SLOG(eInfo1, "Device is muxed-capture");
    }
    else
    {
        SLOG(eError, "Error: Device is neither mixed nor muxed capture and will be unrecognized by the Microsoft HDAudio Class Driver");
        fRes = FALSE;
    }

    return fRes;
}


BOOL 
CAssociations::Add(CPinComplex* pPC)
{
    BOOL fRes = FALSE;
    CAssoc* pAssoc = NULL;
    USHORT usAssoc = ((AzPinWidgetConfigDefault)(pPC->m_ConfigDef.Response)).DefAssoc;

    // 0xF always indicates an independent device, so create a new association
    if (0xF == usAssoc)
    {
        pAssoc = new CAssoc(usAssoc);
        m_listAssoc.AddTail(pAssoc);
    }
    else
    {
        // does association group already exist?
        BOOL fFound = FALSE;
        TTNode<CAssoc>* pttnodeAssoc = m_listAssoc.GetHead();
        while (!fFound && pttnodeAssoc)
        {
            pAssoc = pttnodeAssoc->pData;
            if (usAssoc == pAssoc->m_usAssoc)
            {
                fFound = TRUE;
            }
            pttnodeAssoc = m_listAssoc.GetNext(pttnodeAssoc);
        }
        if (!fFound)
        {
            pAssoc = new CAssoc(usAssoc);
            m_listAssoc.AddTail(pAssoc);
        }
    }
    fRes = pAssoc->Add(pPC);

    return fRes;
}

BOOL 
CAssociations::Validate()
{
    BOOL fRes = TRUE;

    TTNode<CAssoc>* pttnodeAssoc = m_listAssoc.GetHead();
    while (pttnodeAssoc)
    {
        CAssoc* pAssoc = pttnodeAssoc->pData;
        fRes = pAssoc->Validate() ? fRes : FALSE;
        pttnodeAssoc = m_listAssoc.GetNext(pttnodeAssoc);
    }

    return fRes;
}

//==============================================================================
// CCodec::Parse
//
// Description:
//  Parse the codec, populate internal structures with support information, 
//  and verify that required parameters are supported
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/08/03 - jwexler - created
//==============================================================================
BOOL 
CCodec::Parse()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;
    CFunctionGroup* pFG = NULL;

    m_rootnode.m_pCodec = this;

    Verb = MakeVerb(m_usSDI, 0, VenDevID);
    azrResponse = SendVerb(m_hDriver, Verb);
    m_rootnode.m_VenDevID = azrResponse;
    SLOG(eInfo3, "  VenDevID: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    
    Verb = MakeVerb(m_usSDI, 0, RevisionID);
    azrResponse = SendVerb(m_hDriver, Verb);
    m_rootnode.m_RevisionID = azrResponse;
    SLOG(eInfo3, "  RevisionID: Response == 0x%016I64X", (ULONGLONG)azrResponse);

    // ask root node for number of function groups / first NID
    Verb = MakeVerb(m_usSDI, 0, SubNodeCount);
    azrResponse = SendVerb(m_hDriver, Verb);
    m_rootnode.m_SubNodeCount = azrResponse;
    SLOG(eInfo3, "  SubNodeCount: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: Root node does not support required parameter SubNodeCount");
        fRes = FALSE;
        goto DONE;
    }

    USHORT usStartNode = (USHORT)(azrResponse.Response >> 16);
    USHORT usNumNodes = (USHORT)(azrResponse.Response & 0xff);
    // step through
    // if something goes wrong on any given node,
    // log an error and try the next node
    for (USHORT usNode = usStartNode; usNode < (usStartNode+usNumNodes); usNode++)
    {
        Verb = MakeVerb(m_usSDI, usNode, NodeType);
        azrResponse = SendVerb(m_hDriver, Verb);
        SLOG(eInfo3, "   NodeType: Response == 0x%016I64X", (ULONGLONG)azrResponse);
        if (!azrResponse.Valid)
        {
            SLOG(eError, "Error: Node does not support required parameter NodeType");
            fRes = FALSE;
            continue; // try the next node
        }
        
        switch (azrResponse.Response & 0xff)
        {
            case NodeTypeAudioFunctionObject:
                SLOG(eInfo2, "Node %u: Audio Function Group", usNode);
                pFG = new CAudioFunctionGroup(usNode, this, (NodeType >> 8) & 0x1);                
            break;
        
            case NodeTypeModemFunctionObject:
                SLOG(eInfo2, "Node %u: Modem Function Group", usNode);
                pFG = new CModemFunctionGroup(usNode, this, (NodeType >> 8) & 0x1);
            break;

            case NodeTypeOther:
                SLOG(eError, "Error: Node %u: HDMI Function Groups no longer exist as of Intel HD Audio DCN 36-A", usNode);
                fRes = FALSE;
                continue; // try the next node
            break;

            default:
                if ((azrResponse.Response & 0xff) >= 0x80)
                {
                    SLOG(eInfo2, "Node %u: Vendor-defined Function Group", usNode);
                    pFG = new CVendorFunctionGroup(usNode, this, (NodeType >> 8) & 0x1);
                }
                else
                {
                    SLOG(eError, "Error: Node %u: Unknown node type: %08Xh", usNode, azrResponse.Response);
                    fRes = FALSE;
                    continue; // try the next node
                }
        }
        pFG->m_NodeType = azrResponse;
        fRes = pFG->Parse();
        if (!fRes)
        {
            SLOG(eError, "Error: Could not parse FunctionGroup node %u attached to SDI# %u", usNode, m_usSDI);
            fRes = FALSE;
            continue; // try the next node
        }
        listFG.AddTail(pFG);
    }

DONE:    
    return fRes;
} // CCodec::Parse

//==============================================================================
// CCodec::RequiredParameterSupport
//
// Description:
//  Check all codec nodes for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/11/03 - jwexler - created
//==============================================================================
BOOL 
CCodec::RequiredParameterSupport()
{
    BOOL fRes = TRUE;
    
    SLOG(eInfo2, "Verifying Codec on SDI %u", m_usSDI);

    // verify the root node    
    fRes = m_rootnode.RequiredParameterSupport();
    
    // verify all function groups
    TTNode<CFunctionGroup>* pttnodeFG = listFG.GetHead();
    while (pttnodeFG)
    {
        CFunctionGroup* pFG = pttnodeFG->pData;        
        fRes = pFG->RequiredParameterSupport() ? fRes : FALSE;
        pttnodeFG = listFG.GetNext(pttnodeFG);
    }

//DONE:    
    return fRes;
} // CCodec::RequiredParameterSupport

//==============================================================================
// CCodec::RequiredVerbSupport
//
// Description:
//  Check all codec nodes for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/15/03 - jwexler - created
//==============================================================================
BOOL 
CCodec::RequiredVerbSupport()
{
    BOOL fRes = TRUE;    
    
    SLOG(eInfo2, "Verifying Codec on SDI %u", m_usSDI);

    // verify the root node    
    fRes = m_rootnode.RequiredVerbSupport();    
    
    // verify all function groups
    TTNode<CFunctionGroup>* pttnodeFG = listFG.GetHead();
    while (pttnodeFG)
    {
        CFunctionGroup* pFG = pttnodeFG->pData;        
        fRes = pFG->RequiredVerbSupport() ? fRes : FALSE;        
        pttnodeFG = listFG.GetNext(pttnodeFG);
    }

//DONE:    
    return fRes;
} // CCodec::RequiredVerbSupport

//==============================================================================
// CCodec::FGReset
//
// Description:
//  Check the codec's Function Groups for proper RESET support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  11/10/04 - jwexler - created
//==============================================================================
BOOL 
CCodec::FGReset()
{
    BOOL fRes = TRUE;    
    
    SLOG(eInfo2, "Verifying Codec on SDI %u", m_usSDI);
    
    // verify all function groups
    TTNode<CFunctionGroup>* pttnodeFG = listFG.GetHead();
    while (pttnodeFG)
    {
        CFunctionGroup* pFG = pttnodeFG->pData;        
        fRes = pFG->FGReset() ? fRes : FALSE;
        pttnodeFG = listFG.GetNext(pttnodeFG);
    }

    return fRes;
} // CCodec::FGReset

//==============================================================================
// CCodec::ValidatePinConfigs
//
// Description:
//  Check the codec for compliance with our pin configuration requirements
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  02/24/06 - jwexler - created
//==============================================================================
BOOL 
CCodec::ValidatePinConfigs()
{
    BOOL fRes = TRUE;    
    
    SLOG(eInfo2, "Verifying Codec on SDI %u", m_usSDI);
    
    // BUGBUG - might need to write 0s and reboot to ensure that BIOS is doing the right thing?

    // verify all function groups
    TTNode<CFunctionGroup>* pttnodeFG = listFG.GetHead();
    while (pttnodeFG)
    {
        CFunctionGroup* pFG = pttnodeFG->pData;
        if (NodeTypeAudioFunctionObject==(pFG->m_NodeType & 0xff))
        {
            CAudioFunctionGroup* pAFG = dynamic_cast<CAudioFunctionGroup*>(pFG);            
            fRes = pAFG->ValidatePinConfigs() ? fRes : FALSE;
        }
        pttnodeFG = listFG.GetNext(pttnodeFG);
    }

    return fRes;
} // CCodec::ValidatePinConfigs

//==============================================================================
// CRootNode::RequiredParameterSupport
//
// Description:
//  Check root node for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/11/03 - jwexler - created
//==============================================================================
BOOL 
CRootNode::RequiredParameterSupport()
{
    BOOL fRes = TRUE;
    
    SLOG(eInfo2, "Verifying root node at node %u", m_usNode);

    SLOG(eInfo2, " Verifying VenDevID");
    if (!m_VenDevID.Valid)
    {
        SLOG(eError, "Error: Root Node does not support required parameter VenDevID");
        fRes = FALSE;
    }
    
    SLOG(eInfo2, " Verifying RevisionID");
    if (!m_RevisionID.Valid)
    {
        SLOG(eError, "Error: Root Node does not support required parameter RevisionID");
        fRes = FALSE;
    }
    SLOG(eInfo2, " Verifying SubNodeCount");
    if (!m_SubNodeCount.Valid)
    {
        SLOG(eError, "Error: Root Node does not support required parameter SubNodeCount");
        fRes = FALSE;
    }

//DONE:    
    return fRes;
} // CRootNode::RequiredParameterSupport

//==============================================================================
// CRootNode::RequiredVerbSupport
//
// Description:
//  Check root node for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/15/03 - jwexler - created
//==============================================================================
BOOL 
CRootNode::RequiredVerbSupport()
{
    BOOL fRes = TRUE;

    SLOG(eInfo2, "Verifying root node at node %u", m_usNode);

    // GetParameter is covered by RequiredParameterSupport test case    
    // so, nothing is necessary here
    
    return fRes;
} // CRootNode::RequiredVerbSupport

//==============================================================================
// CAudioFunctionGroup::Parse
//
// Description:
//  Parse the AudioFunctionGroup node and populate internal structures 
//  with support information
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/09/03 - jwexler - created
//==============================================================================
BOOL 
CAudioFunctionGroup::Parse()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;    
    
    // get AFG caps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, AudFnGrpCaps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    AudFnGrpCaps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_AudFnGrpCaps = azrResponse;

    // get AFG SupBitsAndSRs
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupBitsAndSRs);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    SupBitsAndSRs: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupBitsAndSRs = azrResponse;

    // get AFG SupStrFormats
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupStrFormats);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    SupStrFormats: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupStrFormats = azrResponse;

    // get AFG InAmpProps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, InAmpProps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    InAmpProps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_InAmpProps = azrResponse;

    // get AFG OutAmpProps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, OutAmpProps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    OutAmpProps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_OutAmpProps = azrResponse;

    // get AFG SupPwrStates
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupPwrStates);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    SupPwrStates: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupPwrStates = azrResponse;

    // get AFG NumGPIOs
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, NumGPIOs);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    NumGPIOs: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_NumGPIOs = azrResponse;

    // get AFG VolKnobCaps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, VolKnobCaps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    VolKnobCaps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_VolKnobCaps = azrResponse;
    
    // ask AFG for number of widgets / first NID
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SubNodeCount);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    SubNodeCount: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SubNodeCount = azrResponse;
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: AFG does not support required parameter SubNodeCount");
        fRes = FALSE;
        goto DONE;
    }

    USHORT usStartNode = (USHORT)(azrResponse.Response >> 16);
    USHORT usNumNodes = (USHORT)(azrResponse.Response & 0xff);
    // step through widgets
    for (USHORT usNode = usStartNode; usNode < (usStartNode+usNumNodes); usNode++)
    {
        // Get widget caps
        Verb = MakeVerb(m_pCodec->m_usSDI, usNode, AudWidgetCaps);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
        SLOG(eInfo3, " AudWidgetCaps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
        if (!azrResponse.Valid)
        {
            SLOG(eError, "Error: Widget does not support required parameter AudWidgetCaps");
            fRes = FALSE;
            goto DONE;
        }
                
        // add to appropriate list
        ULONG ulWidgetType = (azrResponse.Response >> 20) & 0xf;
        CAudioWidget* pWidget = NULL;
        switch (ulWidgetType)
        {
            case AzAudNodeOutput:
                SLOG(eInfo2, " Node %u: AudioOutputConverter", usNode);
                pWidget = new CAudioOutputConverter(usNode, m_pCodec, this, azrResponse);
                pWidget->Parse();
                listAOC.AddTail(dynamic_cast<CAudioOutputConverter*>(pWidget));
                listWidgets.AddTail(pWidget);
            break;
        
            case AzAudNodeInput:               
                SLOG(eInfo2, " Node %u: AudioInputConverter", usNode);
                pWidget = new CAudioInputConverter(usNode, m_pCodec, this, azrResponse);
                pWidget->Parse();
                listAIC.AddTail(dynamic_cast<CAudioInputConverter*>(pWidget));
                listWidgets.AddTail(pWidget);
            break;

            case AzAudNodeMixer:
                SLOG(eInfo2, " Node %u: Mixer", usNode);
                pWidget = new CMixer(usNode, m_pCodec, this, azrResponse);
                pWidget->Parse();
                listMix.AddTail(dynamic_cast<CMixer*>(pWidget));
                listWidgets.AddTail(pWidget);
            break;

            case AzAudNodeSelector:                
                SLOG(eInfo2, " Node %u: InputSelector", usNode);
                pWidget = new CInputSelector(usNode, m_pCodec, this, azrResponse);
                pWidget->Parse();
                listIS.AddTail(dynamic_cast<CInputSelector*>(pWidget));
                listWidgets.AddTail(pWidget);
            break;

            case AzAudNodePinComplex:                
                SLOG(eInfo2, " Node %u: PinComplex", usNode);
                pWidget = new CPinComplex(usNode, m_pCodec, this, azrResponse);
                pWidget->Parse();
                listPC.AddTail(dynamic_cast<CPinComplex*>(pWidget));
                listWidgets.AddTail(pWidget);
            break;

            case AzAudNodePower:                
                SLOG(eInfo2, " Node %u: PowerWidget", usNode);
                pWidget = new CPowerWidget(usNode, m_pCodec, this, azrResponse);
                pWidget->Parse();
                listPW.AddTail(dynamic_cast<CPowerWidget*>(pWidget));
                listWidgets.AddTail(pWidget);
            break;

            case AzAudVolumeKnob:                
                SLOG(eInfo2, " Node %u: VolumeKnob", usNode);
                pWidget = new CVolumeKnob(usNode, m_pCodec, this, azrResponse);
                pWidget->Parse();
                listVK.AddTail(dynamic_cast<CVolumeKnob*>(pWidget));
                listWidgets.AddTail(pWidget);
            break;

            case AzAudBeepGen:                
                SLOG(eInfo2, " Node %u: BeepGenerator", usNode);
                pWidget = new CBeepGen(usNode, m_pCodec, this, azrResponse);
                pWidget->Parse();
                listBG.AddTail((CBeepGen*)pWidget);
                listWidgets.AddTail(pWidget);
            break;

            case AzAudNodeVendor:
                SLOG(eInfo2, " Node %u: Vendor-defined", usNode);
                pWidget = new CVendorWidget(usNode, m_pCodec, this, azrResponse);
                pWidget->Parse();
                listVW.AddTail((CVendorWidget*)pWidget);
                listWidgets.AddTail(pWidget);
            break;

            default:
                SLOG(eError, "Error: Node %u: Unknown audio widget type: 0x%01x", usNode, ulWidgetType);
                fRes = FALSE;
                goto DONE;
        }
    }

    fRes = ProbeHDMIPerAudio0078() ? fRes : FALSE;

DONE:
    return fRes;
} // CAudioFunctionGroup::Parse


//==============================================================================
// CAudioFunctionGroup::RequiredParameterSupport
//
// Description:
//  Check AFG and all sub-nodes for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/12/03 - jwexler - created
//==============================================================================
BOOL 
CAudioFunctionGroup::RequiredParameterSupport()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;    
    
    SLOG(eInfo2, "Verifying Audio Function Group at node %u", m_usNode);

    SLOG(eInfo2, " Verifying SubNodeCount");
    if (!m_SubNodeCount.Valid)
    {
        SLOG(eError, "Error: AudioFunctionGroup at node %u does not support required parameter SubNodeCount", m_usNode);
        fRes = FALSE;
    }
    SLOG(eInfo2, " Verifying NodeType");
    if (!m_NodeType.Valid)
    {
        SLOG(eError, "Error: AudioFunctionGroup at node %u does not support required parameter NodeType", m_usNode);
        fRes = FALSE;
    }
    // BUGBUG - need to determine if delay reporting really MUST be supported in afg or ALL(some?) sub-widgets
    SLOG(eInfo2, " Verifying AudFnGrpCaps");
    if (!m_AudFnGrpCaps.Valid)
    {
        XLOG(XWARN, eWarn1,
            "Warning: AudioFunctionGroup at node %u does not support parameter AudFnGrpCaps. "
            "This parameter is required if one or more widgets in the AudioFunctionGroup opts to "
            "not report a correct delay in its AudWidgetCaps parameter.",
            m_usNode
        );
    }
    else
    {
        AzAudioFunctionParameters afgcaps(m_AudFnGrpCaps.Response);
        if (0 == afgcaps.InputDelay)
        {
            XLOG(XWARN, eWarn1,
                "Warning: AudioFunctionGroup at node %u specifies 0 for its Input Delay. "
                "This value must be non-zero if one or more widgets in the AudioFunctionGroup opts to "
                "not report a correct delay in its AudWidgetCaps parameter.",
                m_usNode
            );
        }
        if (0 == afgcaps.OutputDelay)
        {
            XLOG(XWARN, eWarn1,
                "Warning: AudioFunctionGroup at node %u specifies 0 for its Output Delay. "
                "This value must be non-zero if one or more widgets in the AudioFunctionGroup opts to "
                "not report a correct delay in its AudWidgetCaps parameter.",
                m_usNode
            );
        }
    }

    SLOG(eInfo2, " Verifying SupBitsAndSRs");
    fRes = RPS_SupBitsAndSRs() ? fRes : FALSE;

    SLOG(eInfo2, " Verifying SupStrFormats");
    fRes = RPS_SupStrFormats() ? fRes : FALSE;

    SLOG(eInfo2, " Verifying InAmpProps");
    fRes = RPS_InAmpProps() ? fRes : FALSE;

    SLOG(eInfo2, " Verifying OutAmpProps");
    fRes = RPS_OutAmpProps() ? fRes : FALSE;

    SLOG(eInfo2, " Verifying SupPwrStates");
    if (!m_SupPwrStates.Valid)
    {
        SLOG(eError, "Error: AudioFunctionGroup at node %u does not support required parameter SupPwrStates", m_usNode);
        fRes = FALSE;
    }    

    TTNode<CAudioWidget>* pttnodeWidget = listWidgets.GetHead();
    while (pttnodeWidget)
    {
        CAudioWidget* pWidget = pttnodeWidget->pData;        
        fRes = pWidget->RequiredParameterSupport() ? fRes : FALSE;
        pttnodeWidget = listWidgets.GetNext(pttnodeWidget);
    }

//DONE:    
    return fRes;
} // CAudioFunctionGroup::RequiredParameterSupport

//==============================================================================
// CAudioFunctionGroup::RPS_SupBitsAndSRs
//
// Description:
//  Check AFG for required support of SupBitsAndSRs parameter
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  04/23/04 - jwexler - created
//==============================================================================
BOOL 
CAudioFunctionGroup::RPS_SupBitsAndSRs()
{
    BOOL fRes = TRUE;

    // SupStrFormats parameter is required either in the AFG, to be used as a default, or else in all of the AOC and AIC widgets
    if (!m_SupBitsAndSRs.Valid)
    {        
        TTNode<CAudioOutputConverter>* pttnodeAOC = listAOC.GetHead();
        while (pttnodeAOC)
        {
            CAudioOutputConverter* pAOC = pttnodeAOC->pData;
            SLOG(eInfo2, "  Verifying AOC at node %u", pAOC->m_usNode);
            if (!pAOC->m_AudWidgetCaps.Valid)
            {
                SLOG(eError, "Error: AudWidgetCaps parameter is required in Audio Output Converter at node %u if "
                             "SupBitsAndSRs is not specified in the Audio Function Group at node %u.", 
                             pAOC->m_usNode, m_usNode);
                fRes = FALSE;
            }
            else if (!(AzAudioWidgetCaps(pAOC->m_AudWidgetCaps.Response)).FormatOverride)
            {
                SLOG(eError, "Error: FormatOverride must be 1 in AudWidgetCaps for Audio Output Converter at node %u if "
                             "SupBitsAndSRs is not specified in the Audio Function Group at node %u.", 
                             pAOC->m_usNode, m_usNode);
                fRes = FALSE;
            }
            if (!pAOC->m_SupBitsAndSRs.Valid)
            {
                SLOG(eError, "Error: SupBitsAndSRs parameter is required either in the Audio Function Group at node %u, "
                             "to be used as a default or else in all of its Audio Output and Input Converter widgets.", 
                             m_usNode);
                fRes = FALSE;
            }
            pttnodeAOC = listAOC.GetNext(pttnodeAOC);
        }
        TTNode<CAudioInputConverter>* pttnodeAIC = listAIC.GetHead();
        while (pttnodeAIC)
        {
            CAudioInputConverter* pAIC = pttnodeAIC->pData;
            SLOG(eInfo2, "  Verifying AIC at node %u", pAIC->m_usNode);
            if (!pAIC->m_AudWidgetCaps.Valid)
            {
                SLOG(eError, "Error: AudWidgetCaps parameter is required in Audio Input Converter at node %u if "
                             "SupBitsAndSRs is not specified in the Audio Function Group at node %u.", 
                             pAIC->m_usNode, m_usNode);
                fRes = FALSE;
            }
            else if (!(AzAudioWidgetCaps(pAIC->m_AudWidgetCaps.Response)).FormatOverride)
            {
                SLOG(eError, "Error: FormatOverride must be 1 in AudWidgetCaps for Audio Input Converter at node %u if "
                             "SupBitsAndSRs is not specified in the Audio Function Group at node %u.", 
                             pAIC->m_usNode, m_usNode);
                fRes = FALSE;
            }
            if (!pAIC->m_SupBitsAndSRs.Valid)
            {
                SLOG(eError, "Error: SupBitsAndSRs parameter is required either in the Audio Function Group at node %u, "
                             "to be used as a default or else in all of its Audio Output and Input Converter widgets.", 
                             m_usNode);
                fRes = FALSE;
            }
            pttnodeAIC = listAIC.GetNext(pttnodeAIC);
        }
    }

    return fRes;
} // CAudioFunctionGroup::RPS_SupBitsAndSRs

//==============================================================================
// CAudioFunctionGroup::RPS_SupStrFormats
//
// Description:
//  Check AFG for required support of SupStrFormats parameter
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  04/23/04 - jwexler - created
//==============================================================================
BOOL 
CAudioFunctionGroup::RPS_SupStrFormats()
{
    BOOL fRes = TRUE;

    // SupStrFormats parameter is required either in the AFG, to be used as a default, or else in all of the AOC and AIC widgets    
    if (!m_SupStrFormats.Valid)
    {        
        TTNode<CAudioOutputConverter>* pttnodeAOC = listAOC.GetHead();
        while (pttnodeAOC)
        {
            CAudioOutputConverter* pAOC = pttnodeAOC->pData;
            SLOG(eInfo2, "  Verifying AOC at node %u", pAOC->m_usNode);
            if (!pAOC->m_AudWidgetCaps.Valid)
            {
                SLOG(eError, "Error: AudWidgetCaps parameter is required in Audio Output Converter at node %u if "
                             "SupStrFormats is not specified in the Audio Function Group at node %u.", 
                             pAOC->m_usNode, m_usNode);
                fRes = FALSE;
            }
            else if (!(AzAudioWidgetCaps(pAOC->m_AudWidgetCaps.Response)).FormatOverride)
            {
                SLOG(eError, "Error: FormatOverride must be 1 in AudWidgetCaps for Audio Output Converter at node %u if "
                             "SupStrFormats is not specified in the Audio Function Group at node %u.", 
                             pAOC->m_usNode, m_usNode);
                fRes = FALSE;
            }
            if (!pAOC->m_SupStrFormats.Valid)
            {
                SLOG(eError, "Error: SupStrFormats parameter is required either in the Audio Function Group at node %u, "
                             "to be used as a default or else in all of its Audio Output and Input Converter widgets.", 
                             m_usNode);
                fRes = FALSE;
            }
            pttnodeAOC = listAOC.GetNext(pttnodeAOC);
        }
        TTNode<CAudioInputConverter>* pttnodeAIC = listAIC.GetHead();
        while (pttnodeAIC)
        {
            CAudioInputConverter* pAIC = pttnodeAIC->pData;
            SLOG(eInfo2, "  Verifying AIC at node %u", pAIC->m_usNode);
            if (!pAIC->m_AudWidgetCaps.Valid)
            {
                SLOG(eError, "Error: AudWidgetCaps parameter is required in Audio Input Converter at node %u if "
                             "SupStrFormats is not specified in the Audio Function Group at node %u.", 
                             pAIC->m_usNode, m_usNode);
                fRes = FALSE;
            }
            else if (!(AzAudioWidgetCaps(pAIC->m_AudWidgetCaps.Response)).FormatOverride)
            {
                SLOG(eError, "Error: FormatOverride must be 1 in AudWidgetCaps for Audio Input Converter at node %u if "
                             "SupStrFormats is not specified in the Audio Function Group at node %u.", 
                             pAIC->m_usNode, m_usNode);
                fRes = FALSE;
            }
            if (!pAIC->m_SupStrFormats.Valid)
            {
                SLOG(eError, "Error: SupStrFormats parameter is required either in the Audio Function Group at node %u, "
                             "to be used as a default or else in all of its Audio Output and Input Converter widgets.", 
                             m_usNode);
                fRes = FALSE;
            }
            pttnodeAIC = listAIC.GetNext(pttnodeAIC);
        }
    }

    return fRes;
} // CAudioFunctionGroup::RPS_SupStrFormats

//==============================================================================
// CAudioFunctionGroup::RPS_InAmpProps
//
// Description:
//  Check AFG for required support of InAmpProps parameter
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  04/23/04 - jwexler - created
//==============================================================================
BOOL 
CAudioFunctionGroup::RPS_InAmpProps()
{
    BOOL fRes = TRUE;

    // InAmpProps parameter is required either in the AFG, to be used as a default, or else in all of the 
    //  AIC, PinComplex, Mixer, and InputSelector widgets
    if (!m_InAmpProps.Valid)
    {
        TTNode<CAudioInputConverter>* pttnodeAIC = listAIC.GetHead();
        while (pttnodeAIC)
        {
            CAudioInputConverter* pAIC = pttnodeAIC->pData;
            SLOG(eInfo2, "  Verifying AIC at node %u", pAIC->m_usNode);
            if (!pAIC->m_InAmpProps.Valid)
            {
                SLOG(eError, "Error: InAmpProps parameter is required either in the AFG at node %u, to be used as a default, ", m_usNode);
                SLOG(eError, "or else in all of the AIC, PinComplex, Mixer, and InputSelector widgets");
                fRes = FALSE;
            }
            pttnodeAIC = listAIC.GetNext(pttnodeAIC);
        }
        TTNode<CPinComplex>* pttnodePC = listPC.GetHead();
        while (pttnodePC)
        {
            CPinComplex* pPC = pttnodePC->pData;
            SLOG(eInfo2, "  Verifying PinComplex at node %u", pPC->m_usNode);
            if (!pPC->m_InAmpProps.Valid)
            {
                SLOG(eError, "Error: InAmpProps parameter is required either in the AFG at node %u, to be used as a default, ", m_usNode);
                SLOG(eError, "or else in all of the AIC, PinComplex, Mixer, and InputSelector widgets");
                fRes = FALSE;
            }
            pttnodePC = listPC.GetNext(pttnodePC);
        }
        TTNode<CMixer>* pttnodeMix = listMix.GetHead();
        while (pttnodeMix)
        {
            CMixer* pMix = pttnodeMix->pData;
            SLOG(eInfo2, "  Verifying Mixer at node %u", pMix->m_usNode);
            if (!pMix->m_InAmpProps.Valid)
            {
                SLOG(eError, "Error: InAmpProps parameter is required either in the AFG at node %u, to be used as a default, ", m_usNode);
                SLOG(eError, "or else in all of the AIC, PinComplex, Mixer, and InputSelector widgets");
                fRes = FALSE;
            }
            pttnodeMix = listMix.GetNext(pttnodeMix);
        }
        TTNode<CInputSelector>* pttnodeIS = listIS.GetHead();
        while (pttnodeIS)
        {
            CInputSelector* pIS = pttnodeIS->pData;
            SLOG(eInfo2, "  Verifying InputSelector at node %u", pIS->m_usNode);
            if (!pIS->m_InAmpProps.Valid)
            {
                SLOG(eError, "Error: InAmpProps parameter is required either in the AFG at node %u, to be used as a default, ", m_usNode);
                SLOG(eError, "or else in all of the AIC, PinComplex, Mixer, and InputSelector widgets");
                fRes = FALSE;
            }
            pttnodeIS = listIS.GetNext(pttnodeIS);
        }
    }
    else
    {
        AzAmpProperties azAmpProp = (AzAmpProperties)m_InAmpProps.Response;
        if (azAmpProp.Offset > azAmpProp.NumSteps)
        {
            SLOG(eError, "Error: FG at node %u has InAmpProps.Offset > InAmpProps.NumSteps", m_usNode);
            fRes = FALSE;
        }

        if (azAmpProp.Reserved0 || azAmpProp.Reserved1 || azAmpProp.Reserved2)
        {
            SLOG(eError,
                "Error: FG at node %u has reserved bits set in Input Amplifier Properties; "
                "Reserved0 (7) = %u; Reserved1 (15) = %u; Reserved2 (23-30) = 0x%x",
                m_usNode, azAmpProp.Reserved0, azAmpProp.Reserved1, azAmpProp.Reserved2
            );
            fRes = FALSE;
        }
    }

    return fRes;
} // CAudioFunctionGroup::RPS_InAmpProps

//==============================================================================
// CAudioFunctionGroup::RPS_OutAmpProps
//
// Description:
//  Check AFG for required support of OutAmpProps parameter
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  04/23/04 - jwexler - created
//==============================================================================
BOOL 
CAudioFunctionGroup::RPS_OutAmpProps()
{
    BOOL fRes = TRUE;

    // OutAmpProps parameter is required either in the AFG, to be used as a default, or else in all of the 
    //  AOC, PinComplex, Mixer, and InputSelector widgets
    if (!m_OutAmpProps.Valid)
    {
        TTNode<CAudioOutputConverter>* pttnodeAOC = listAOC.GetHead();
        while (pttnodeAOC)
        {
            CAudioOutputConverter* pAOC = pttnodeAOC->pData;
            SLOG(eInfo2, "  Verifying AOC at node %u", pAOC->m_usNode);
            if (!pAOC->m_OutAmpProps.Valid)
            {
                SLOG(eError, "Error: OutAmpProps parameter is required either in the AFG at node %u, to be used as a default, ", m_usNode);
                SLOG(eError, "or else in all of the AOC, PinComplex, Mixer, and InputSelector widgets");
                fRes = FALSE;
            }
            pttnodeAOC = listAOC.GetNext(pttnodeAOC);
        }
        TTNode<CPinComplex>* pttnodePC = listPC.GetHead();
        while (pttnodePC)
        {
            CPinComplex* pPC = pttnodePC->pData;
            SLOG(eInfo2, "  Verifying PinComplex at node %u", pPC->m_usNode);
            if (!pPC->m_OutAmpProps.Valid)
            {
                SLOG(eError, "Error: OutAmpProps parameter is required either in the AFG at node %u, to be used as a default, ", m_usNode);
                SLOG(eError, "or else in all of the AIC, PinComplex, Mixer, and InputSelector widgets");
                fRes = FALSE;
            }
            pttnodePC = listPC.GetNext(pttnodePC);
        }
        TTNode<CMixer>* pttnodeMix = listMix.GetHead();
        while (pttnodeMix)
        {
            CMixer* pMix = pttnodeMix->pData;
            SLOG(eInfo2, "  Verifying Mixer at node %u", pMix->m_usNode);
            if (!pMix->m_OutAmpProps.Valid)
            {
                SLOG(eError, "Error: OutAmpProps parameter is required either in the AFG at node %u, to be used as a default, ", m_usNode);
                SLOG(eError, "or else in all of the AIC, PinComplex, Mixer, and InputSelector widgets");
                fRes = FALSE;
            }
            pttnodeMix = listMix.GetNext(pttnodeMix);
        }
        TTNode<CInputSelector>* pttnodeIS = listIS.GetHead();
        while (pttnodeIS)
        {
            CInputSelector* pIS = pttnodeIS->pData;
            SLOG(eInfo2, "  Verifying InputSelector at node %u", pIS->m_usNode);
            if (!pIS->m_OutAmpProps.Valid)
            {
                SLOG(eError, "Error: OutAmpProps parameter is required either in the AFG at node %u, to be used as a default, ", m_usNode);
                SLOG(eError, "or else in all of the AIC, PinComplex, Mixer, and InputSelector widgets");
                fRes = FALSE;
            }
            pttnodeIS = listIS.GetNext(pttnodeIS);
        }
    }    
    else
    {
        AzAmpProperties azAmpProp = (AzAmpProperties)m_OutAmpProps.Response;
        if (azAmpProp.Offset > azAmpProp.NumSteps)
        {
            SLOG(eError, "Error: FG at node %u has OutAmpProps.Offset > OutAmpProps.NumSteps", m_usNode);
            fRes = FALSE;
        }

        if (azAmpProp.Reserved0 || azAmpProp.Reserved1 || azAmpProp.Reserved2)
        {
            SLOG(eError,
                "Error: FG at node %u has reserved bits set in Output Amplifier Properties; "
                "Reserved0 (7) = %u; Reserved1 (15) = %u; Reserved2 (23-30) = 0x%x",
                m_usNode, azAmpProp.Reserved0, azAmpProp.Reserved1, azAmpProp.Reserved2
            );
            fRes = FALSE;
        }
    }

    return fRes;
} // CAudioFunctionGroup::RPS_OutAmpProps

//==============================================================================
// CAudioFunctionGroup::RequiredVerbSupport
//
// Description:
//  Check audio function group node for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/15/03 - jwexler - created
//==============================================================================
BOOL 
CAudioFunctionGroup::RequiredVerbSupport()
{
    BOOL fRes = TRUE;

    SLOG(eInfo2, "Verifying Audio Function Group at node %u", m_usNode);

    // GetParameter is covered by RequiredParameterSupport test case

    // SubSystemID
    fRes = CheckSubsystemID() ? fRes : FALSE;

    // PowerState    
    fRes = m_PowerControl.Validate(this, eNodeType_FunctionGroup, TRUE) ? fRes : FALSE;

    // GPIO controls f10-f1a and 710-71a
    fRes = CheckGPIO() ? fRes : FALSE;
    
    // TODO - BeepGenerationControl

    // TODO - RESET

    // TODO - I could/should probably make this common in CFunctionGroup
    TTNode<CAudioWidget>* pttnodeWidget = listWidgets.GetHead();
    while (pttnodeWidget)
    {
        CAudioWidget* pWidget = pttnodeWidget->pData;
        fRes = pWidget->RequiredVerbSupport() ? fRes : FALSE;
        pttnodeWidget = listWidgets.GetNext(pttnodeWidget);
    }
    
    return fRes;
} // CAudioFunctionGroup::RequiredVerbSupport

//==============================================================================
// CAudioFunctionGroup::ValidatePinConfigs
//
// Description:
//  Check the function group's pins for compliance with our pin configuration 
//  requirements
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  02/24/06 - jwexler - created
//==============================================================================
BOOL 
CAudioFunctionGroup::ValidatePinConfigs()
{
    BOOL fRes = TRUE;

    SLOG(eInfo2, "Verifying Audio Function Group at node %u", m_usNode);

    // verify all pin complexes and build associations
    TTNode<CPinComplex>* pttnodePC = listPC.GetHead();
    while (pttnodePC)
    {
        CPinComplex* pPC = pttnodePC->pData;
        fRes = pPC->ValidatePinConfig() ? fRes : FALSE;
        // add to list of associations only if the pin is actually connected
        if (ePcxPortConnNoPhysConn != ((AzPinWidgetConfigDefault)(pPC->m_ConfigDef.Response)).PortConn)
        {
            fRes = m_Assoc.Add(pPC) ? fRes: FALSE;
        }
        pttnodePC = listPC.GetNext(pttnodePC);
    }    

    // verify all associations
    fRes = m_Assoc.Validate() ? fRes : FALSE;    
    
    return fRes;
} // CAudioFunctionGroup::ValidatePinConfigs


//==============================================================================
// CModemFunctionGroup::Parse
//
// Description:
//  Parse the ModemFunctionGroup node and populate internal structures 
//  with support information
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/15/03 - jwexler - created
//==============================================================================
BOOL 
CModemFunctionGroup::Parse()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    // get MFG SupPwrStates
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupPwrStates);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    SupPwrStates: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupPwrStates = azrResponse;

    // get MFG NumGPIOs
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, NumGPIOs);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    NumGPIOs: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_NumGPIOs = azrResponse;    
    
    // ask MFG for number of widgets / first NID
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SubNodeCount);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    SubNodeCount: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SubNodeCount = azrResponse;
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: MFG does not support required parameter SubNodeCount");
        //fRes = FALSE;
        goto DONE;
    }

    // BUGBUG - don't know how to handle modems right now
    // step through widgets
    /*
    USHORT usStartNode = (USHORT)(azrResponse.Response >> 16);
    USHORT usNumNodes = (USHORT)(azrResponse.Response & 0xff);
    for (USHORT usNode = usStartNode; usNode < (usStartNode+usNumNodes); usNode++)
    {
        SLOG(eInfo2, "    Node %u:", usNode);
        // parse modem widgets???
    }
    */

DONE:
    return fRes;
} // CModemFunctionGroup::Parse

//==============================================================================
// CModemFunctionGroup::RequiredParameterSupport
//
// Description:
//  Check MFG and all sub-nodes for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/15/03 - jwexler - created
//==============================================================================
BOOL 
CModemFunctionGroup::RequiredParameterSupport()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    SLOG(eInfo2, "Verifying Modem Function Group at node %u", m_usNode);    
    
    SLOG(eInfo2, " Verifying SubNodeCount");
    if (!m_SubNodeCount.Valid)
    {
        SLOG(eError, "Error: ModemFunctionGroup at node %u does not support required parameter SubNodeCount", m_usNode);
        fRes = FALSE;
    }
    SLOG(eInfo2, " Verifying NodeType");
    if (!m_NodeType.Valid)
    {
        SLOG(eError, "Error: ModemFunctionGroup at node %u does not support required parameter NodeType", m_usNode);
        fRes = FALSE;
    }    

    SLOG(eInfo2, " Verifying SupPwrStates");
    if (!m_SupPwrStates.Valid)
    {
        SLOG(eError, "Error: ModemFunctionGroup at node %u does not support required parameter SupPwrStates", m_usNode);
        fRes = FALSE;
    }    

    // BUGBUG - Check all widgets
    /*
    TTNode<CAudioWidget>* pttnodeWidget = listWidgets.GetHead();
    while (pttnodeWidget)
    {
        CAudioWidget* pWidget = pttnodeWidget->pData;
        SLOG(eInfo2, " Verifying Audio Widget at node %u", pWidget->m_usNode);
        fRes = pWidget->RequiredParameterSupport() ? fRes : FALSE;
        pttnodeWidget = listWidgets.GetNext(pttnodeWidget);
    }
    */

//DONE:    
    return fRes;
} // CModemFunctionGroup::RequiredParameterSupport

//==============================================================================
// CModemFunctionGroup::RequiredVerbSupport
//
// Description:
//  Check Modem function group node for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  04/23/04 - jwexler - created
//==============================================================================
BOOL 
CModemFunctionGroup::RequiredVerbSupport()
{
    BOOL fRes = TRUE;

    SLOG(eInfo2, "Verifying Modem Function Group at node %u", m_usNode);

    // GetParameter is covered by RequiredParameterSupport test case

    // SubSystemID
    fRes = CheckSubsystemID() ? fRes : FALSE;

    // PowerState
    fRes = m_PowerControl.Validate(this, eNodeType_FunctionGroup, TRUE) ? fRes : FALSE;

    // GPIO controls f10-f1a and 710-71a
    fRes = CheckGPIO() ? fRes : FALSE;

    // TODO - RESET
    
    return fRes;
} // CModemFunctionGroup::RequiredVerbSupport

//==============================================================================
// CVendorFunctionGroup::Parse
//
// Description:
//  Parse the VendorFunctionGroup node and populate internal structures 
//  with support information
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/15/03 - jwexler - created
//==============================================================================
BOOL 
CVendorFunctionGroup::Parse()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;
    
    // get VFG SupPwrStates
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupPwrStates);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    SupPwrStates: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupPwrStates = azrResponse;

    // get VFG NumGPIOs
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, NumGPIOs);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    NumGPIOs: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_NumGPIOs = azrResponse;    
    
    // ask VFG for number of widgets / first NID
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SubNodeCount);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "    SubNodeCount: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SubNodeCount = azrResponse;
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: VFG does not support required parameter SubNodeCount");
        fRes = FALSE;
        goto DONE;
    }

    /*
    // step through widgets
    USHORT usStartNode = (USHORT)(azrResponse.Response >> 16);
    USHORT usNumNodes = (USHORT)(azrResponse.Response & 0xff);
    for (USHORT usNode = usStartNode; usNode < (usStartNode+usNumNodes); usNode++)
    {
        SLOG(eInfo2, "    Node %u:", usNode);
        // BUGBUG - parse vendor widgets???
        // maybe just look for widget types i know about?
        // what would i check for?
    }
    */

DONE:
    return fRes;
} // CVendorFunctionGroup::Parse

//==============================================================================
// CVendorFunctionGroup::RequiredParameterSupport
//
// Description:
//  Check VFG and all sub-nodes for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/15/03 - jwexler - created
//==============================================================================
BOOL 
CVendorFunctionGroup::RequiredParameterSupport()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    SLOG(eInfo2, "Verifying Vendor-defined Function Group at node %u", m_usNode);    

    SLOG(eInfo2, " Verifying SubNodeCount");
    if (!m_SubNodeCount.Valid)
    {
        SLOG(eError, "Error: VendorFunctionGroup at node %u does not support required parameter SubNodeCount", m_usNode);
        fRes = FALSE;
    }
    SLOG(eInfo2, " Verifying NodeType");
    if (!m_NodeType.Valid)
    {
        SLOG(eError, "Error: VendorFunctionGroup at node %u does not support required parameter NodeType", m_usNode);
        fRes = FALSE;
    }    

    // BUGBUG - not sure if I need to check all widgets if not supported here
    /*
    SLOG(eInfo2, " Verifying SupPwrStates");
    if (!m_SupPwrStates.Valid)
    {
        SLOG(eError, "Error: VendorFunctionGroup at node %u does not support required parameter SupPwrStates", m_usNode);
        fRes = FALSE;
    }
    */

    // BUGBUG - Check all widgets
    /*
    TTNode<CAudioWidget>* pttnodeWidget = listWidgets.GetHead();
    while (pttnodeWidget)
    {
        CAudioWidget* pWidget = pttnodeWidget->pData;
        SLOG(eInfo2, " Verifying Audio Widget at node %u", pWidget->m_usNode);
        fRes = pWidget->RequiredParameterSupport() ? fRes : FALSE;
        pttnodeWidget = listWidgets.GetNext(pttnodeWidget);
    }
    */

//DONE:    
    return fRes;
} // CVendorFunctionGroup::RequiredParameterSupport

//==============================================================================
// CVendorFunctionGroup::RequiredVerbSupport
//
// Description:
//  Check Vendor function group node for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  04/23/04 - jwexler - created
//==============================================================================
BOOL 
CVendorFunctionGroup::RequiredVerbSupport()
{
    BOOL fRes = TRUE;

    SLOG(eInfo2, "Verifying Vendor-defined Function Group at node %u", m_usNode);
    
    // GetParameter is covered by RequiredParameterSupport test case

    // SubSystemID
    fRes = CheckSubsystemID() ? fRes : FALSE; 

    // PowerState
    fRes = m_PowerControl.Validate(this, eNodeType_FunctionGroup, FALSE) ? fRes : FALSE;

    // GPIO controls f10-f1a and 710-71a
    //fRes = CheckGPIO() ? fRes : FALSE;

    // BUGBUG - RESET
    
    return fRes;
} // CVendorFunctionGroup::RequiredVerbSupport

//==============================================================================
// CFunctionGroup::CheckSubsystemID
//
// Description:
//  Verify that the function group properly supports the SubsystemID control
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  04/23/04 - jwexler - created
//==============================================================================
BOOL 
CFunctionGroup::CheckSubsystemID()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    SLOG(eInfo2, " Verifying SubsystemID");    

    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SubsystemID);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);    
    if (azrResponse.Response)
    {
        SLOG(eInfo3, "  SubsystemID(Param): Response == 0x%016I64X", (ULONGLONG)azrResponse);
        // HACKHACK - for pre-1.0 we allow codecs to support this as a parameter    
        //if (1 > ((m_pCodec->m_rootnode.m_RevisionID>>20) & 0xF))
        //{
        //    XLOG(XWARN, eWarn1, "Warning: FunctionGroup at node %u should implement "
        //        "SubsystemID as a control and not as a parameter!", m_usNode);
        //    XLOG(XWARN, eWarn1, "Warning: This will be considered a failure in the future.");
        //}
        //else
        //{
            SLOG(eError, "Error: FunctionGroup at node %u must implement "
                "SubsystemID as a control and not as a parameter!", m_usNode);
            fRes = FALSE;
        //}
    }

    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetSubsystemID, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    m_SubSystemID = azrResponse;
    SLOG(eInfo3, "  GetSubsystemID: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!m_SubSystemID.Valid)
    {
        SLOG(eError, "Error: FunctionGroup at node %u does not support required control GetSubsystemID", m_usNode);
        fRes = FALSE;
    }
    if (0 == m_SubSystemID.Response)
    {
        // HACKHACK - for pre-1.0 we allow codecs to support this as a parameter    
        //if (1 > ((m_pCodec->m_rootnode.m_RevisionID>>20) & 0xF))
        //{
        //    XLOG(XWARN, eWarn1, "Warning: FunctionGroup at node %u should not return 0 for GetSubsystemID", m_usNode);
        //    XLOG(XWARN, eWarn1, "Warning: This will be considered a failure in the future.");
        //}
        //else
        //{
            SLOG(eError, "Error: FunctionGroup at node %u must not return 0 for GetSubsystemID", m_usNode);
            fRes = FALSE;
        //}
    }
        
    // write the same subsystem id back in
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetSubsystemID0, (USHORT)(m_SubSystemID.Response) & 0xFF, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "  SetSubsystemID0: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: FunctionGroup at node %u does not support required control SetSubsystemID0", m_usNode);
        fRes = FALSE;
    }
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetSubsystemID1, (USHORT)(m_SubSystemID.Response>>8) & 0xFF, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "  SetSubsystemID1: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: FunctionGroup at node %u does not support required control SetSubsystemID1", m_usNode);
        fRes = FALSE;
    }
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetSubsystemID2, (USHORT)(m_SubSystemID.Response>>16) & 0xFF, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "  SetSubsystemID2: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: FunctionGroup at node %u does not support required control SetSubsystemID2", m_usNode);
        fRes = FALSE;
    }
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetSubsystemID3, (USHORT)(m_SubSystemID.Response>>24) & 0xFF, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "  SetSubsystemID3: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: FunctionGroup at node %u does not support required control SetSubsystemID3", m_usNode);
        fRes = FALSE;
    }
    // verify that it's still the same
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetSubsystemID, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "  GetSubsystemID: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (m_SubSystemID != azrResponse)
    {
        SLOG(eError, "Error: SubsystemID was not written back correctly!");
        fRes = FALSE;
    }

    // verify reserved controls f21, f22, and f23
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetSubsystemID_Reserved0, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "  GetSubsystemID(F21): Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid || (azrResponse.Response && (m_SubSystemID != azrResponse)))
    {
        SLOG(eError, "Error: Codec must either respond to F21 control identically to F20 "
                     "or respond with 0");
        fRes = FALSE;
    }
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetSubsystemID_Reserved1, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "  GetSubsystemID(F22): Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid || (azrResponse.Response && (m_SubSystemID != azrResponse)))
    {
        SLOG(eError, "Error: Codec must either respond to F22 control identically to F20 "
                     "or respond with 0");
        fRes = FALSE;
    }
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetSubsystemID_Reserved2, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "  GetSubsystemID(F23): Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid || (azrResponse.Response && (m_SubSystemID != azrResponse)))
    {
        SLOG(eError, "Error: Codec must either respond to F23 control identically to F20 "
                     "or respond with 0");
        fRes = FALSE;
    }

    // verify that subsystem persists across power transitions
    //  This will be done in the power state validation routine

    return fRes;
} // CFunctionGroup::CheckSubsystemID


//==============================================================================
// CFunctionGroup::CheckGPIO
//
// Description:
//  Verify that the function group properly supports the GPIO control
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  04/23/04 - jwexler - created
//==============================================================================
BOOL 
CFunctionGroup::CheckGPIO()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;
    AzNumGPIO azNumGPIO;

    // BUGBUG - Need to understand better what I can test here
    /*
    if (azNumGPIO.NumGPI)
    {
        Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetFnPwrState, azwPowerState, 0);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
        SLOG(eInfo3, " SetFnPwrState(original): Response == 0x%016I64X", i, azrResponse);
    }
    azNumGPIO = m_NumGPIOs.Response;
    azNumGPIO.GPIWake;
    azNumGPIO.GPIUnsol;
    
    azNumGPIO.NumGPO;
    azNumGPIO.NumGPIO;
    */

    return fRes;
} // CFunctionGroup::CheckGPIO

//==============================================================================
// CFunctionGroup::FGReset
//
// Description:
//  Verify that the function group properly supports the RESET control
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  11/10/04 - jwexler - created
//==============================================================================
BOOL 
CFunctionGroup::FGReset()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry arzResponse;

    SLOG(eInfo2, "Verifying RESET");    
    
    const ULONG culFirstVerb = 0xf0100;
    const ULONG culLastVerb = 0xf2400;
    const ULONG culVerbInterval = 0x00100;

    // Number of verbs, plus 2 resets
    ULONG ulNumCommands = (culLastVerb-culFirstVerb)/culVerbInterval + 2;
    SLOG(eInfo3, "ulNumCommands == %u", ulNumCommands);

    ULONG ulCPSize = sizeof(UserModeCodecCommandPacket) + sizeof(AzCorbEntry) * ulNumCommands;
    ULONG ulRPSize = sizeof(UserModeCodecResponsePacket) + sizeof(AzRirbEntry) * ulNumCommands;
    CBuffer<BYTE> pCPacketBuffer(ulCPSize);
    CBuffer<BYTE> pRPacketBuffer(ulRPSize);
    UserModeCodecCommandPacket* pCPacket = (UserModeCodecCommandPacket*)(BYTE*)pCPacketBuffer;
    UserModeCodecResponsePacket* pRPacket = (UserModeCodecResponsePacket*)(BYTE*)pRPacketBuffer;
    DWORD dwBytesReturned = 0;

    ZeroMemory(pCPacket, ulCPSize);
    ZeroMemory(pRPacket, ulRPSize);
    pCPacket->NumCommands = ulNumCommands;

    // pack in verbs, starting with double RESET
    UINT i = 0;
    pCPacket->Command[i++] = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetFuncReset, 0, 0);
    pCPacket->Command[i++] = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetFuncReset, 0, 0);
    ULONG ulVerb = culFirstVerb;
    for (; i < ulNumCommands; i++)
    {        
        pCPacket->Command[i] = MakeVerb(m_pCodec->m_usSDI, m_usNode, (AzCodecNodeControl)(ulVerb), 0, 0);
        ulVerb += culVerbInterval;
    }

    for (i = 0; i < ulNumCommands; i++)
    {
        SLOG(eInfo1, "Verb[%u]: 0x%08X", i, (ULONG)(pCPacket->Command[i]));
    }
    
    
    fRes = DeviceIoControl(m_pCodec->m_hDriver, IOCTL_AZALIABUS_SENDVERBS, 
                           pCPacket, ulCPSize, 
                           pRPacket, ulRPSize, 
                           &dwBytesReturned, 0);
    if (!fRes)
    {
        SLOG(eError, "    [DeviceIoControl:SendVerb] GetLastError() == %u, failing reset", GetLastError());
        goto DONE;
    }
    // check responses - this is the bus driver's job and does not indicate *valid* responses
    //SLOG(eInfo3, "NumResponses == %u", pRPacket->NumResponses);
    if (pRPacket->NumResponses != ulNumCommands)
    {
        SLOG(eError, "Error: Number of responses is different from the number of commands sent.");
        fRes = FALSE;
    }
    SLOG(eInfo3, "SetFuncReset: Response == 0x%016I64X", (ULONGLONG)(pRPacket->Response[0]));
    if (!pRPacket->Response[0].Valid)
    {
        SLOG(eError, "Error: FunctionGroup at node %u does not support required control SetFuncReset", m_usNode);
        fRes = FALSE;
    }

    for (UINT i = 0; i < pRPacket->NumResponses; i++)
    {
        SLOG(eInfo1, "Response[%u]: 0x%016I64X", i, (ULONGLONG)(pRPacket->Response[i]));
    }

    ULONG ulValidResponses = 0;
    for (UINT i = 1; i < pRPacket->NumResponses; i++)
    {
        if (pRPacket->Response[i].Valid)
        {
            ulValidResponses++;
        }
        else
        {
            SLOG(eError,
                "Error: codec response #%u to verb 0x%x was invalid.",
                i, culFirstVerb + (i - 2) * culVerbInterval
            );
            
            fRes = FALSE;
        }
    }
    if (ulValidResponses != ulNumCommands-1)
    {
        SLOG(eError, "Error: Codec failed to respond properly after Reset.");
        SLOG(eError, "  Number of verbs sent after Reset == %u", ulNumCommands - 1);
        SLOG(eError, "  Number of valid responses returned after Reset == %u", ulValidResponses);
        SLOG(eError, "  This suggests that the first %u verbs after Reset were dropped.", ulNumCommands - 1 - ulValidResponses);

        fRes = FALSE;
    }

DONE:
    return fRes;
} // CFunctionGroup::FGReset

//==============================================================================
// CAudioOutputConverter::Parse
//
// Description:
//  Parse the AudioOutputConverter widget and populate internal structures 
//  with support information
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/09/03 - jwexler - created
//==============================================================================
BOOL 
CAudioOutputConverter::Parse()
{
    BOOL fRes = FALSE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    // get AOC SupBitsAndSRs
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupBitsAndSRs);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     SupBitsAndSRs: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupBitsAndSRs = azrResponse;

    // get AOC SupStrFormats
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupStrFormats);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     SupStrFormats: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupStrFormats = azrResponse;

    // get AOC OutAmpProps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, OutAmpProps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     OutAmpProps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_OutAmpProps = azrResponse;

    // get AOC SupPwrStates
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupPwrStates);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     SupPwrStates: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupPwrStates = azrResponse;

    // get AOC ProcParam
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, ProcParam);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     ProcParam: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_ProcParam = azrResponse;

    fRes = TRUE;

//DONE:
    return fRes;
} // CAudioOutputConverter::Parse

//==============================================================================
// CAudioOutputConverter::RequiredParameterSupport
//
// Description:
//  Check AOC for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/12/03 - jwexler - created
//==============================================================================
BOOL 
CAudioOutputConverter::RequiredParameterSupport()
{
    BOOL fRes = TRUE;    

    SLOG(eInfo2, " Verifying Audio Output Converter at node %u", m_usNode);

    // AudWidgetCaps parameter covered by CAudioFunctionGroup::Parse

    SLOG(eInfo2, "  Verifying AOC for OutAmpProps");
    if (m_OutAmpProps.Valid)
    {
        AzAmpProperties azAmpProp = (AzAmpProperties)m_OutAmpProps.Response;
        if (azAmpProp.Offset > azAmpProp.NumSteps)
        {
            SLOG(eError, "Error: AOC at node %u has OutAmpProps.Offset > OutAmpProps.NumSteps", m_usNode);
            fRes = FALSE;
        }

        if (azAmpProp.Reserved0 || azAmpProp.Reserved1 || azAmpProp.Reserved2)
        {
            SLOG(eError,
                "Error: AOC at node %u has reserved bits set in Output Amplifier Properties; "
                "Reserved0 (7) = %u; Reserved1 (15) = %u; Reserved2 (23-30) = 0x%x",
                m_usNode, azAmpProp.Reserved0, azAmpProp.Reserved1, azAmpProp.Reserved2
            );
            fRes = FALSE;
        }
    }

    return fRes;
} // CAudioOutputConverter::RequiredParameterSupport

//==============================================================================
// CAudioOutputConverter::RequiredVerbSupport
//
// Description:
//  Check Audio Output Converter node for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  04/23/04 - jwexler - created
//==============================================================================
BOOL 
CAudioOutputConverter::RequiredVerbSupport()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;
    
    AzAudioWidgetCaps awc = m_AudWidgetCaps.Response;

    // this is a little weird so I'll break it out
    // the way to calculate the number of channels is as follows
    // awc.Stereo is the least significant bit of a number
    // awc.ChanCountExt is the higher bits of that number
    // this number is the 0-based index of the highest channel.
    //
    // e.g., if Stereo = 1, and ChanCountExt = 0b011,
    // then the number ((ChanCountExt << 1) | Stereo) is 0b0111
    // so the highest channel index is 7
    // so there are eight channels
    //
    // ew
    //
    // but this preserves backcompat - awc.ChanCountExt used to be reserved
    // so it preserves the pre-ChanCountExt behavior:
    //     Stereo = 0; ChanCountExt = 0; highest index = 0; mono
    //     Stereo = 1; ChanCountExt = 0; highest index = 1; stereo)
    
    USHORT cChannels = (USHORT)(
        (awc.ChanCountExt << 1) | // higher-order bits
        awc.Stereo // least-significant bit
    ) + 1; // plus one to go from highest index to count

    SLOG(eInfo2, "Verifying Audio Output Converter at node %u with channel count %u", m_usNode, cChannels);

    // GetParameter is covered by RequiredParameterSupport test case

    // BUGBUG
    // c: proc state
    // c: coeff index
    // c: proc coeff

    // c: amp gain/mute - covered by AudioFunctionGroup::RequiredVerbSupport

    // stream format
    fRes = CheckStreamFormat() ? fRes : FALSE;    

    // c: digital converter
    if (awc.Digital)
    {
        SLOG(eInfo1, "Audio Output Converter at node %u is a Digital Converter", m_usNode);
        // c: digital converter 1
        // c: digital converter 2
    }

    if (awc.Reserved0)
    {
        SLOG(eError,
            "Error: Audio Widget Capabilities reserved bits should not be set.\n"
            "Reserved0 is 0x%x.",
            awc.Reserved0
        );
        fRes = FALSE;
    }

    if (cChannels > 2 && cChannels % 2 == 1) // odd number of channels > 2
    {
        SLOG(eError,
            "Error: Audio Widget reports it supports an odd number of channels greater than two (%u)",
            cChannels
        );
        fRes = FALSE;
    }

    if (cChannels > 2 && !awc.Digital)
    {
        SLOG(eError, "Error: Non-digital Audio Output Converter handles more than 2 channels (%u)", cChannels);
        fRes = FALSE;
    }

    // converter channel count (7.3.3.35 in DCN 35-A)
    // only required for > 2 channels
    if (cChannels > 2)
    {
        Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetCnvChCnt, 0, 0);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
        if (azrResponse.Valid)
        {
            // check for reserved bits
            if (azrResponse.Response & 0xffffff00)
            {
                SLOG(eError, "Error: reserved bits set in response to GetCnvChCnt: 0x%08x", azrResponse.Response);
                fRes = FALSE;
            }
            AzConverterChannelCount azOriginalCount = azrResponse.Response & 0xff;
            AzConverterChannelCount azNewCount;

            SLOG(eInfo1,
                "Converter channel count original value is %u (%u channels)",
                azOriginalCount.CnvChCnt, azOriginalCount.CnvChCnt + 1
            );

            // try setting each channel value in turn
            for (USHORT i = 0; i < cChannels; i++)
            {
                Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetCnvChCnt, i, 0);
                azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);

                if (azrResponse.Valid)
                {
                    // read it back and make sure it's the same
                    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetCnvChCnt, 0, 0);
                    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
                    if (azrResponse.Valid)
                    {
                        if (azrResponse.Response & 0xffffff00)
                        {
                            SLOG(eError, "Error: reserved bits set in response to GetCnvChCnt : 0x%08x", azrResponse.Response);
                            fRes = FALSE;
                        }

                        azNewCount = azrResponse.Response & 0xff;
                        if (i == azNewCount.CnvChCnt)
                        {
                            SLOG(eInfo1, "Setting the converter channel count to %u worked", i);
                        }
                        else
                        {
                            SLOG(eError,
                                "Error: setting converter channel count didn't stick, got %u back instead of %u",
                                azNewCount.CnvChCnt, i
                            );
                            fRes = FALSE;
                        }
                    }
                    else
                    {
                        SLOG(eError, "Error: Couldn't read back channel count after setting to %u", i);
                        fRes = FALSE;
                    }
                }
                else
                {
                    SLOG(eError, "Error: Setting converter channel count to %u yielded invalid response", i);
                    fRes = FALSE;
                }
            }

            // set the original value back
            Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetCnvChCnt, azOriginalCount.CnvChCnt, 0);
            azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);

            if (azrResponse.Valid)
            {
                // read it back and make sure it's the original value
                Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetCnvChCnt, 0, 0);
                azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
                if (azrResponse.Valid)
                {
                    if (azrResponse.Response & 0xffffff00)
                    {
                        SLOG(eError, "Error: reserved bits set in response to GetCnvChCnt : 0x%08x", azrResponse.Response);
                        fRes = FALSE;
                    }

                    azNewCount = azrResponse.Response & 0xff;
                    if (azOriginalCount.CnvChCnt == azNewCount.CnvChCnt)
                    {
                        SLOG(eInfo1, "Setting converter channel count back to the original value (%u) worked", azOriginalCount.CnvChCnt);
                    }
                    else
                    {
                        SLOG(eError,
                            "Error: setting converter channel count to original value didn't stick, got %u back instead of %u",
                            azNewCount.CnvChCnt, azOriginalCount.CnvChCnt
                        );
                        fRes = FALSE;
                    }
                }
                else
                {
                    SLOG(eError, "Error: Couldn't read back channel count after setting to the original value");
                    fRes = FALSE;
                }
            }
            else
            {
                SLOG(eError, "Error: Setting converter channel count to the original value yielded invalid response");
                fRes = FALSE;
            }
            
        }
        else
        {
            SLOG(eError,
                "Error: Audio Output Converter handles more than two channels "
                "but doesn't support \"Get Converter Channel Count\" verb "
                "(see 7.3.3.35 of HD Audio DCN 35-A)"
            );
            fRes = FALSE;
        }
    }
    
    // Audio Sample Packet channel mapping (7.3.3.41 in DCN 36-A)
    // make a trial to see if the verb is supported
    // it shouldn't be - this verb should apply to pin widgets, /not/ converters
    // but there was an error in DCN 34-A2
    AzGetCnvChMapPayload azPayload;
    // need to ask for a non-zero slot so we can distinguish supported from not supported
    // in particular, the response should have SlotNum = 1
    azPayload.SlotNum = 1;
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetCnvChMap, azPayload, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    if (azrResponse.Valid && azrResponse.Response != 0)
    {
        // D'oh - DCN 34-A2 claims another victim :-(
        SLOG(eError,
            "Error: handling Audio Sample Packet channel mapping verbs "
            "on an output converter is incorrect. "
            "Section 7.3.3.41 incorrectly lists \"output converter\" in DCN 34-A2 "
            "but correctly lists \"pin widget\" in DCN 36-A."
        );
        fRes = FALSE;
    }

    // c: Power State
    fRes = m_PowerControl.Validate(this, eNodeType_AudioWidget, FALSE) ? fRes : FALSE;

    // Channel / Stream ID
    fRes = CheckCnvStrCh() ? fRes : FALSE;

    // BUGBUG - X: SDI Select

    // BUGBUG - c: Unsolicted Enable
    
    return fRes;
} // CAudioOutputConverter::RequiredVerbSupport

//==============================================================================
// CAudioInputConverter::Parse
//
// Description:
//  Parse the AudioInputConverter widget and populate internal structures 
//  with support information
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/10/03 - jwexler - created
//==============================================================================
BOOL 
CAudioInputConverter::Parse()
{
    BOOL fRes = FALSE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    // get AIC SupBitsAndSRs
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupBitsAndSRs);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     SupBitsAndSRs: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupBitsAndSRs = azrResponse;

    // get AIC SupStrFormats
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupStrFormats);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     SupStrFormats: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupStrFormats = azrResponse;

    // get AIC InAmpProps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, InAmpProps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     InAmpProps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_InAmpProps = azrResponse;

    // get AIC ConnListLength
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, ConnListLength);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     ConnListLength: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_ConnListLength = azrResponse;

    // get AIC SupPwrStates
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupPwrStates);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     SupPwrStates: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupPwrStates = azrResponse;

    // get AIC ProcParam
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, ProcParam);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     ProcParam: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_ProcParam = azrResponse;

    fRes = TRUE;

//DONE:
    return fRes;
} // CAudioInputConverter::Parse

//==============================================================================
// CAudioInputConverter::RequiredParameterSupport
//
// Description:
//  Check AIC for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/12/03 - jwexler - created
//==============================================================================
BOOL 
CAudioInputConverter::RequiredParameterSupport()
{
    BOOL fRes = TRUE;    

    SLOG(eInfo2, " Verifying Audio Input Converter at node %u", m_usNode);

    // AudWidgetCaps parameter covered by CAudioFunctionGroup::Parse

    SLOG(eInfo2, "  Verifying AIC for ConnListLength");
    if (!m_ConnListLength.Valid)
    {
        SLOG(eError, "Error: AudioInputConverter at node %u does not support required parameter ConnListLength", m_usNode);
        fRes = FALSE;
    }

    SLOG(eInfo2, "  Verifying AIC for InAmpProps");
    if (m_InAmpProps.Valid)
    {
        AzAmpProperties azAmpProp = (AzAmpProperties)m_InAmpProps.Response;
        if (azAmpProp.Offset > azAmpProp.NumSteps)
        {
            SLOG(eError, "Error: AIC at node %u has InAmpProps.Offset > InAmpProps.NumSteps", m_usNode);
            fRes = FALSE;
        }

        if (azAmpProp.Reserved0 || azAmpProp.Reserved1 || azAmpProp.Reserved2)
        {
            SLOG(eError,
                "Error: AIC at node %u has reserved bits set in Input Amplifier Properties; "
                "Reserved0 (7) = %u; Reserved1 (15) = %u; Reserved2 (23-30) = 0x%x",
                m_usNode, azAmpProp.Reserved0, azAmpProp.Reserved1, azAmpProp.Reserved2
            );
            fRes = FALSE;
        }
    }
    
//DONE:    
    return fRes;
} // CAudioInputConverter::RequiredParameterSupport

//==============================================================================
// CAudioInputConverter::RequiredVerbSupport
//
// Description:
//  Check Audio Input Converter node for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  04/30/04 - jwexler - created
//==============================================================================
BOOL 
CAudioInputConverter::RequiredVerbSupport()
{
    BOOL fRes = TRUE;    
    AzAudioWidgetCaps awc = m_AudWidgetCaps.Response;

    SLOG(eInfo2, "Verifying Audio Input Converter at node %u", m_usNode);
    
    // GetParameter is covered by RequiredParameterSupport test case

    // R: GetConnListEntry
    // c: ConnSelect
    fRes = CheckConn() ? fRes : FALSE;

    // BUGBUG
    // c: proc state
    // c: coeff index
    // c: proc coeff

    // c: amp gain/mute - covered by AudioFunctionGroup::RequiredVerbSupport

    // R: stream format
    fRes = CheckStreamFormat() ? fRes : FALSE;    

    // c: digital converter
    if (awc.Digital)
    {
        SLOG(eInfo1, "Audio Input Converter at node %u is a Digital Converter", m_usNode);
        // c: digital converter 1
        // c: digital converter 2
    }

    if (awc.Reserved0)
    {
        SLOG(eError,
            "Error: Audio Widget Capabilities reserved bits should not be set.\n"
            "Reserved0 is 0x%x.",
            awc.Reserved0
        );
        fRes = FALSE;
    }

    if (0 < awc.ChanCountExt && 0 == awc.Stereo) // odd number of channels > 2
    {
        SLOG(eError,
            "Error: Audio Widget reports it supports an odd number of channels greater than two (%u)",
            ((awc.ChanCountExt << 1) | awc.Stereo) + 1
        );
        fRes = FALSE;
    }

    // c: Power State
    fRes = m_PowerControl.Validate(this, eNodeType_AudioWidget, FALSE) ? fRes : FALSE;

    // Channel / Stream ID
    fRes = CheckCnvStrCh() ? fRes : FALSE;    

    // X: SDI Select

    // c: Unsolicted Enable
    
    return fRes;
} // CAudioInputConverter::RequiredVerbSupport

//==============================================================================
// CAudioConverter::CheckStreamFormat
//
// Description:
//  Verify that the Audio Converter properly supports the CnvBitsSR controls
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  04/26/04 - jwexler - created
//==============================================================================
BOOL 
CAudioConverter::CheckStreamFormat()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;
    AzCnvBitsSRPayload azCnvBitsSRPayload;

    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetCnvBitsSR, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    m_CnvBitsSR = azrResponse;
    SLOG(eInfo3, " GetCnvBitsSR: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!m_CnvBitsSR.Valid)
    {
        SLOG(eError, "Error: Audio Converter at node %u does not support required control GetCnvBitsSR", m_usNode);
        fRes = FALSE;
    }

    // TODO - Go through all claimed supported formats and attempt set/get
    //       - might want to do this in a separate test case
    //
    //  for now, we'll just check for basic support
    // set
    azCnvBitsSRPayload = (AzCnvBitsSRPayload)((AzCnvBitsSR)(m_CnvBitsSR.Response));
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetCnvBitsSR, azCnvBitsSRPayload, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, " SetCnvBitsSR: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: Audio Converter at node %u does not support required control SetCnvBitsSR", m_usNode);
        fRes = FALSE;
    }
    // get
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetCnvBitsSR, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, " GetCnvBitsSR: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid || (azrResponse != m_CnvBitsSR))
    {
        SLOG(eError, "Error: Audio Converter at node %u failed to set CnvBitsSR to the requested format.", m_usNode);
        fRes = FALSE;
    }

    return fRes;
} // CAudioConverter::CheckStreamFormat

//==============================================================================
// CAudioConverter::CheckCnvStrCh
//
// Description:
//  Verify that the Audio Converter properly supports the CnvStrCh controls
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  04/30/04 - jwexler - created
//==============================================================================
BOOL 
CAudioConverter::CheckCnvStrCh()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;    
    AzAudioWidgetCaps awc = m_AudWidgetCaps.Response;

    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetCnvStrCh, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    m_CnvStrCh = azrResponse;
    SLOG(eInfo3, "   GetCnvStrCh: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!m_CnvStrCh.Valid)
    {
        SLOG(eError, "Error: Audio Converter at node %u does not support required control GetCnvStrCh", m_usNode);
        fRes = FALSE;
    }

    // Go through all stream/channel combinations
    //  BUGBUG - What if different converters overlap stream/channel settings??
    SLOG(eInfo2, "Checking all stream/channel combinations; will log only those that fail");
    AzCnvStreamChan azcStreamChan;
    for (UINT i = 0; i < MAXSTREAMS; i++)
    {
        azcStreamChan.StreamId = i;
        for (UINT j = 0; j < MAXCHANNELS; j++)
        {
            // BUGBUG - channel 16 won't work for stereo will it?            
            //    I should set it only if dealing with a mono converter, else we stop at 15.
            azcStreamChan.Channel = j;            

            // set
            Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetCnvStrCh, azcStreamChan, 0);
            azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);            
            if (!azrResponse.Valid)
            {
                SLOG(eBlab1, "   SetCnvStrCh(%08Xh): Response == 0x%016I64X", (UCHAR)azcStreamChan, (ULONGLONG)azrResponse);
                SLOG(eError, "Error: Audio Converter at node %u does not support required control SetCnvStrCh", m_usNode);
                fRes = FALSE;
            }
            // get
            Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetCnvStrCh, 0, 0);
            azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
            if (azrResponse.Response != azcStreamChan)
            {
                SLOG(eBlab1, "   GetCnvStrCh(%08Xh expected): Response == 0x%016I64X", (UCHAR)azcStreamChan, (ULONGLONG)azrResponse);
                SLOG(eError, "Error: Audio Converter at node %u did not set Stream/Channel as requested.", m_usNode);
                fRes = FALSE;
            }
            // Only check for channel 0 on Digital Input Converters
            ULONG ulWidgetType = (m_AudWidgetCaps.Response >> 20) & 0xf;
            if ((AzAudNodeInput == ulWidgetType) && 
                (awc.Digital))
            {
                break;
            }
#ifdef ADI_AD1986_CONTINGENCY
            // Only do channel 0 on node 6 for the ADI AD1986A - contingency
            //  We make this code exception because on the nVidia MCP51 controller, there is another issue which causes
            //  all further commands to be ignored.  (State machine is not reset on Frame Sync)
            //
            if ((0x11D41986==m_pCodec->m_rootnode.m_VenDevID.Response) && 
                (6==m_usNode))
            {
                break;
            }
#endif // #ifdef ADI_AD1986_CONTINGENCY
        }
    }
    // set it back to original state
    azcStreamChan = (AzCnvStreamChan)(m_CnvStrCh.Response);
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetCnvStrCh, azcStreamChan, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, " SetCnvStrCh: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: Audio Converter at node %u does not support required control SetCnvStrCh", m_usNode);
        fRes = FALSE;
    }

    return fRes;
} // CAudioConverter::CheckCnvStrCh


//==============================================================================
// CConnChecker::Check
//
// Description:
//  Verify that the passed in Audio Widget obeys spec'd Interconnection Rules
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  09/30/04 - jwexler - created
//==============================================================================
BOOL 
CConnChecker::CheckConn()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;
    AzAudioWidgetCaps awc = m_AudWidgetCaps.Response;

    SLOG(eInfo2, "Checking Audio Widget's connections.");
    // each widget's class declaration defines whether the ConnList bit is required or not 
    if (m_fConnListReq && !awc.ConnList)
    {
        SLOG(eError, "Error: Audio Widget at node %u does not specify ConnList == 1 in its capabilities!", 
            m_usNode);
        fRes = FALSE;            
    }

    if (0x7F == ((AzConnectionListLength)m_ConnListLength.Response).Length)
    {
        SLOG(eError, "Error: Invalid Connection List Length");
        fRes = FALSE;
        goto DONE;
    }

    // verify all list entries are non-zero
    // BUGBUG - should only check this if response was valid
    // response should be valid if ConnList is set, are we checking this in RPS already?
    ULONG ulNumPer = ((AzConnectionListLength)m_ConnListLength.Response).LongForm ? 2 : 4;
    ULONG ulNumEntries = ((AzConnectionListLength)m_ConnListLength.Response).Length;
    SLOG(eInfo2, "  Verifying that all %u connection list entries are non-zero.", ulNumEntries);
    SLOG(eInfo2, "  LongForm == %u", ((AzConnectionListLength)m_ConnListLength.Response).LongForm);
    SLOG(eInfo2, "  ulNumEntries == %u", ulNumEntries);
    for (ULONG i = 0; i < ulNumEntries; i += ulNumPer)
    {
        Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetConnListEntry, (USHORT)i, 0);
        azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);            
        SLOG(eInfo3, " GetConnListEntry(%u): Response == 0x%016I64X", i, (ULONGLONG)azrResponse);
        if (!azrResponse.Valid)
        {
            SLOG(eError, "Error: Audio Widget at node %u does not support required control GetConnListEntry", 
                m_usNode);
            fRes = FALSE;
        }
        ULONG ulEntries = azrResponse.Response;
        // check this block of entries
        for (UINT j = i; j < i+ulNumPer; j++)
        {
            // shift/mask out each entry, 8 or 16 bits depending on short or long form
            ULONG ulNumBits = sizeof(ULONG)*8 / ulNumPer;            
            USHORT usEntry = (USHORT)((ulEntries >> (ulNumBits*(j-i))) & ((1 << ulNumBits)-1));
            SLOG(eBlab1, "   usEntry == %04X", usEntry);

            if (j < ulNumEntries)
            {
                if (0 == usEntry)
                {
                    SLOG(eError, "Error: Entry %u in the Audio Widget's Connection List == 0 and should not be.", j);
                    fRes = FALSE;
                }
                SLOG(eBlab1, "  Checking node");
                if (!CheckNode(usEntry))
                {
                    SLOG(eError, "Error: Audio Widget at node %u has an invalid connection list entry.", m_usNode);
                    fRes = FALSE;
                }
            }
            else if (0 != usEntry)
            {
                SLOG(eError, "Error: Out of range entry %u is non-zero!", j);
            }
            else
                continue;
            
            // each widget's class declaration defines whether the ConnSelects verbs are required or not            
            if (m_fConnSelReq && ulNumEntries > 1)
            {                
                // BUGBUG - don't forget to stash and restore the original connection - i guess?
                Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetConnSelect, (USHORT)j, 0);
                azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);            
                SLOG(eBlab1, " SetConnSelect(%u): Response == 0x%016I64X", j, (ULONGLONG)azrResponse);
                if (!azrResponse.Valid)
                {
                    SLOG(eError, "Error: Audio Widget at node %u does not support required control SetConnSelect", 
                        m_usNode);
                    fRes = FALSE;
                }
                Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetConnSelect, 0, 0);
                azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);            
                SLOG(eBlab1, " GetConnSelect: Response == 0x%016I64X", (ULONGLONG)azrResponse);
                if (!azrResponse.Valid)
                {
                    SLOG(eError, "Error: Audio Widget at node %u does not support required control GetConnSelect", 
                        m_usNode);
                    fRes = FALSE;
                }
                if (azrResponse.Response != j)
                {
                    SLOG(eError, "Error: Failed to set Connection to requested entry %u!", j);
                    fRes = FALSE;
                }                
            }
        }
    }
DONE:
    return fRes;
} // CConnChecker::Check


//==============================================================================
// CConnChecker::CheckNode
//
// Description:
//  Verify that the node specified by usEntry is a valid connection list entry
//
// Arguments:
//  USHORT usEntry - the node id to check
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  11/03/04 - jwexler - created
//==============================================================================
BOOL 
CConnChecker::CheckNode(USHORT usEntry)
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    // verify that entry corresponds to actual node
    // Get widget caps
    Verb = MakeVerb(m_pCodec->m_usSDI, usEntry, AudWidgetCaps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eBlab1, "     AudWidgetCaps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: Widget does not support required parameter AudWidgetCaps");
        fRes = FALSE;
        goto DONE;
    }
    
    // beepgen's now are allowed in connection lists
    /*
    // Verify that it's not a BeepGen
    ULONG ulWidgetType = (azrResponse.Response >> 20) & 0xf;
    if (AzAudBeepGen == ulWidgetType)
    {
        SLOG(eError, "Error: Beep Generators found in connection list.");
        fRes = FALSE;
    }
    */

    // TODO - verify that it's within bounds specified by function group
    // TODO - Verify that it's not shared across function groups?? Can I maintain a global list of Node IDs?
    // TODO - Verify connection is not orphaned.
DONE:
    return fRes;
} // CConnChecker::CheckNode

//==============================================================================
// CMixer::Parse
//
// Description:
//  Parse the Mixer widget and populate internal structures 
//  with support information
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/10/03 - jwexler - created
//==============================================================================
BOOL 
CMixer::Parse()
{
    BOOL fRes = FALSE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    // get Mixer InAmpProps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, InAmpProps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     InAmpProps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_InAmpProps = azrResponse;

    // get Mixer OutAmpProps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, OutAmpProps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     OutAmpProps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_OutAmpProps = azrResponse;

    // get Mixer ConnListLength
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, ConnListLength);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     ConnListLength: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_ConnListLength = azrResponse;

    // get Mixer SupPwrStates
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupPwrStates);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     SupPwrStates: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupPwrStates = azrResponse;

    fRes = TRUE;

//DONE:
    return fRes;
} // CMixer::Parse

//==============================================================================
// CMixer::RequiredParameterSupport
//
// Description:
//  Check Mixer for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/12/03 - jwexler - created
//==============================================================================
BOOL 
CMixer::RequiredParameterSupport()
{
    BOOL fRes = TRUE;    

    SLOG(eInfo2, " Verifying Mixer at node %u", m_usNode);

    // AudWidgetCaps parameter covered by CAudioFunctionGroup::Parse

    SLOG(eInfo2, "  Verifying Mixer for ConnListLength");
    if (!m_ConnListLength.Valid)
    {
        SLOG(eError, "Error: Mixer at node %u does not support required parameter ConnListLength", m_usNode);
        fRes = FALSE;
    }

    SLOG(eInfo2, "  Verifying Mixer for InAmpProps");
    if (m_InAmpProps.Valid)
    {
        AzAmpProperties azAmpProp = (AzAmpProperties)m_InAmpProps.Response;
        if (azAmpProp.Offset > azAmpProp.NumSteps)
        {
            SLOG(eError, "Error: Mixer at node %u has InAmpProps.Offset > InAmpProps.NumSteps", m_usNode);
            fRes = FALSE;
        }

        if (azAmpProp.Reserved0 || azAmpProp.Reserved1 || azAmpProp.Reserved2)
        {
            SLOG(eError,
                "Error: Mixer at node %u has reserved bits set in Input Amplifier Properties; "
                "Reserved0 (7) = %u; Reserved1 (15) = %u; Reserved2 (23-30) = 0x%x",
                m_usNode, azAmpProp.Reserved0, azAmpProp.Reserved1, azAmpProp.Reserved2
            );
            fRes = FALSE;
        }
    }

    SLOG(eInfo2, "  Verifying Mixer for OutAmpProps");
    if (m_OutAmpProps.Valid)
    {
        AzAmpProperties azAmpProp = (AzAmpProperties)m_OutAmpProps.Response;
        if (azAmpProp.Offset > azAmpProp.NumSteps)
        {
            SLOG(eError, "Error: Mixer at node %u has OutAmpProps.Offset > OutAmpProps.NumSteps", m_usNode);
            fRes = FALSE;
        }

        if (azAmpProp.Reserved0 || azAmpProp.Reserved1 || azAmpProp.Reserved2)
        {
            SLOG(eError,
                "Error: Mixer at node %u has reserved bits set in Output Amplifier Properties; "
                "Reserved0 (7) = %u; Reserved1 (15) = %u; Reserved2 (23-30) = 0x%x",
                m_usNode, azAmpProp.Reserved0, azAmpProp.Reserved1, azAmpProp.Reserved2
            );
            fRes = FALSE;
        }
    }
    
//DONE:    
    return fRes;
} // CMixer::RequiredParameterSupport

//==============================================================================
// CMixer::RequiredVerbSupport
//
// Description:
//  Check Mixer node for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  07/28/04 - jwexler - created
//==============================================================================
BOOL 
CMixer::RequiredVerbSupport()
{
    BOOL fRes = TRUE;    
    
    SLOG(eInfo2, "Verifying Mixer at node %u", m_usNode);
    
    // R: GetConnListEntry
    fRes = CheckConn() ? fRes : FALSE;

    // c: amp gain/mute
    //fRes = CheckAmp() ? fRes : FALSE;

    // c: PowerState
    fRes = m_PowerControl.Validate(this, eNodeType_AudioWidget, FALSE) ? fRes : FALSE;

    // TODO - c: Unsol Enable
    
    return fRes;
} // CMixer::RequiredVerbSupport

//==============================================================================
// CMixer::CheckAmp
//
// Description:
//  Verify that the Audio Widget properly supports Amplifier Gain/Mute verb
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/30/04 - jwexler - created
//==============================================================================
/*
BOOL 
CMixer::CheckAmp()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;
    AzGetAmpGainMuteVerbPayload azGetAmpPayload;
    AzGetAmpGainMuteVerbResponse azGetAmpResponse;
    AzSetAmpGainMuteVerbPayload azSetAmpPayload;
    AzAudioWidgetCaps azCaps = m_AudWidgetCaps.Response;

    SLOG(eInfo2, "Checking Mixer's AmpGainMute support.");    
    
    azGetAmpPayload.GetOutput = 0;
    do
    {
        SLOG(eBlab1, " azGetAmpPayload.GetOutput == %u", azGetAmpPayload.GetOutput);
        azGetAmpPayload.GetLeft = 0;
        do
        {
            SLOG(eBlab1, "  azGetAmpPayload.GetLeft == %u", azGetAmpPayload.GetLeft);
            ULONG ulNumEntries = ((AzConnectionListLength)m_ConnListLength.Response).Length;
            azGetAmpPayload.Index = 0;
            do
            {
                SLOG(eBlab1, "  azGetAmpPayload.Index == %u", azGetAmpPayload.Index);
                // get/set mute bit        
                azGetAmpPayload.Reserved0 = 0;
                azGetAmpPayload.Reserved1 = 0;
                
                Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetAmpGainMute, azGetAmpPayload, 0);
                azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);    
                SLOG(eInfo3, "   GetAmpGainMute: Response == 0x%016I64X", (ULONGLONG)azrResponse);
                if (!azrResponse.Valid)
                {
                    SLOG(eError, "Error: Mixer at node %u does not support required control GetAmpGainMute!", m_usNode);
                    fRes = FALSE;
                }
                azGetAmpResponse = azrResponse.Response;
                // If we request a non-existant amp,the response should be 0.
                if (((!azGetAmpPayload.GetOutput && !azCaps.InAmpPresent) || 
                    (azGetAmpPayload.GetOutput && !azCaps.OutAmpPresent)) && 
                    (0 != azGetAmpResponse))
                {
                    SLOG(eError, "Error: Mixer at node %u returned a non-zero response for a non-existant amp.", m_usNode);
                    fRes = FALSE;
                }

                // HEREHERE
                // TODO - need to check caps to determine if In/Out AmpPresent (7.3.4.6)
                //  amp params (stepsize/numsteps/offset/mutecap) are same but values (Gain/Mute) can be different
                //  set/get/verify all valid values/combinations based on params/caps
                //  set/get/verify all invalud values/combinations with defined behavior based on params/caps
                    
                
                azSetAmpPayload.Gain = azGetAmpResponse.Gain;
                azSetAmpPayload.Mute = !azGetAmpResponse.Mute;
                azSetAmpPayload.Index = azGetAmpPayload.Index;
                azSetAmpPayload.SetRightAmp = !azGetAmpPayload.GetLeft;
                azSetAmpPayload.SetLeftAmp = azGetAmpPayload.GetLeft;
                azSetAmpPayload.SetInputAmp = !azGetAmpPayload.GetOutput;
                azSetAmpPayload.SetOutputAmp = azGetAmpPayload.GetOutput;
                Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetAmpGainMute, azSetAmpPayload, 0);
                azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
                SLOG(eInfo3, "  SetAmpGainMute(original): Response == 0x%016I64X", (ULONGLONG)azrResponse);
                if (!azrResponse.Valid)
                {
                    SLOG(eError, "Error: Mixer at node %u does not support required control SetAmpGainMute", m_usNode);
                    fRes = FALSE;
                }

                //azGetAmpResponse.Gain;
                azGetAmpPayload.Index++;
            } while (azGetAmpPayload.Index && (azGetAmpPayload.Index < ulNumEntries));
            azGetAmpPayload.GetLeft = !azGetAmpPayload.GetLeft;
        } while (azGetAmpPayload.GetLeft);
        azGetAmpPayload.GetOutput = !azGetAmpPayload.GetOutput;
    } while (azGetAmpPayload.GetOutput);
    

    // set gain values based on AmpCaps
    
    // Get output, left, right, & both
        // verify that index bits are ignored
        // Verify all 0s are returned if there is no output amplifier.
    // Get input, left, right & both, index based on Connection List.
        // Verify that index bits are ignored if only 1 input
        // Verify all 0s returned if index out of range or if there is no input amplifier. 
    // Set output only, left, right, & both.
        // Verify that channel bits are ignored if there is only 1 channel.  
        // Verify that Set is ignored (ie: does not affect input amps) if there is no output amplifier.  
        // Verify that index bits are ignored.
    // Set input only, left, right & both.
        // Verify that channel bits are ignored if there is only 1 channel.
        // Verify that index is ignored if not a selector nor a mixer widget.  
        // If it is, then verify that index is respected.
    // Set input and output together, left, right, & both channels.  Use index where appropriate.  
    // Set neither – verify that nothing changed.

    // TODO - use loopback to determine that mute and gains take affect

    //
    return fRes;
} // CMixer::CheckAmp
*/
//==============================================================================
// CInputSelector::Parse
//
// Description:
//  Parse the InputSelector widget and populate internal structures 
//  with support information
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/10/03 - jwexler - created
//==============================================================================
BOOL 
CInputSelector::Parse()
{
    BOOL fRes = FALSE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    // get InputSelector InAmpProps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, InAmpProps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     InAmpProps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_InAmpProps = azrResponse;

    // get InputSelector OutAmpProps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, OutAmpProps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     OutAmpProps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_OutAmpProps = azrResponse;

    // get InputSelector ConnListLength
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, ConnListLength);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     ConnListLength: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_ConnListLength = azrResponse;

    // get InputSelector SupPwrStates
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupPwrStates);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     SupPwrStates: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupPwrStates = azrResponse;

    // get InputSelector ProcParam
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, ProcParam);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     ProcParam: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_ProcParam = azrResponse;

    fRes = TRUE;

//DONE:
    return fRes;
} // CInputSelector::Parse

//==============================================================================
// CInputSelector::RequiredParameterSupport
//
// Description:
//  Check InputSelector for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/12/03 - jwexler - created
//==============================================================================
BOOL 
CInputSelector::RequiredParameterSupport()
{
    BOOL fRes = TRUE;    

    SLOG(eInfo2, " Verifying Input Selector at node %u", m_usNode);

    // AudWidgetCaps parameter covered by CAudioFunctionGroup::Parse

    SLOG(eInfo2, "  Verifying InputSelector for ConnListLength");
    if (!m_ConnListLength.Valid)
    {
        SLOG(eError, "Error: InputSelector at node %u does not support required parameter ConnListLength", m_usNode);
        fRes = FALSE;
    }

    SLOG(eInfo2, "  Verifying InputSelector for InAmpProps");
    if (m_InAmpProps.Valid)
    {
        AzAmpProperties azAmpProp = (AzAmpProperties)m_InAmpProps.Response;
        if (azAmpProp.Offset > azAmpProp.NumSteps)
        {
            SLOG(eError, "Error: InputSelector at node %u has InAmpProps.Offset > InAmpProps.NumSteps", m_usNode);
            fRes = FALSE;
        }

        if (azAmpProp.Reserved0 || azAmpProp.Reserved1 || azAmpProp.Reserved2)
        {
            SLOG(eError,
                "Error: InputSelector at node %u has reserved bits set in Input Amplifier Properties; "
                "Reserved0 (7) = %u; Reserved1 (15) = %u; Reserved2 (23-30) = 0x%x",
                m_usNode, azAmpProp.Reserved0, azAmpProp.Reserved1, azAmpProp.Reserved2
            );
            fRes = FALSE;
        }
    }
    
    SLOG(eInfo2, "  Verifying InputSelector for OutAmpProps");
    if (m_OutAmpProps.Valid)
    {
        AzAmpProperties azAmpProp = (AzAmpProperties)m_OutAmpProps.Response;
        if (azAmpProp.Offset > azAmpProp.NumSteps)
        {
            SLOG(eError, "Error: InputSelector at node %u has OutAmpProps.Offset > OutAmpProps.NumSteps", m_usNode);
            fRes = FALSE;
        }

        if (azAmpProp.Reserved0 || azAmpProp.Reserved1 || azAmpProp.Reserved2)
        {
            SLOG(eError,
                "Error: InputSelector at node %u has reserved bits set in Output Amplifier Properties; "
                "Reserved0 (7) = %u; Reserved1 (15) = %u; Reserved2 (23-30) = 0x%x",
                m_usNode, azAmpProp.Reserved0, azAmpProp.Reserved1, azAmpProp.Reserved2
            );
            fRes = FALSE;
        }
    }
    
//DONE:    
    return fRes;
} // CInputSelector::RequiredParameterSupport

//==============================================================================
// CInputSelector::RequiredVerbSupport
//
// Description:
//  Check InputSelector node for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  07/28/04 - jwexler - created
//==============================================================================
BOOL 
CInputSelector::RequiredVerbSupport()
{
    BOOL fRes = TRUE;
    
    SLOG(eInfo2, "Verifying Input Selector at node %u", m_usNode);
    
    // c: ConnectionSelect
    // R: GetConnListEntry
    fRes = CheckConn() ? fRes : FALSE;

    // TODO
    // c: ProcState
    // c: CoeffIndex
    // c: ProcCoeff
    // c: AmpGainMute

    // c:PowerState
    fRes = m_PowerControl.Validate(this, eNodeType_AudioWidget, FALSE) ? fRes : FALSE;

    // TODO
    // c: Unsol Enable

    return fRes;
} // CInputSelector::RequiredVerbSupport

//==============================================================================
// CPinComplex::Parse
//
// Description:
//  Parse the PinComplex widget and populate internal structures 
//  with support information
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/10/03 - jwexler - created
//==============================================================================
BOOL 
CPinComplex::Parse()
{
    BOOL fRes = FALSE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    // get PinComplex PinCaps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, PinCaps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     PinCaps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_PinCaps = azrResponse;

    // get PinComplex ConfigDefault
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetConfigDefault, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     GetConfigDefault: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_ConfigDef = azrResponse;

    // get PinComplex InAmpProps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, InAmpProps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     InAmpProps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_InAmpProps = azrResponse;

    // get PinComplex OutAmpProps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, OutAmpProps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     OutAmpProps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_OutAmpProps = azrResponse;

    // get PinComplex ConnListLength
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, ConnListLength);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     ConnListLength: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_ConnListLength = azrResponse;

    // get PinComplex SupPwrStates
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupPwrStates);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     SupPwrStates: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupPwrStates = azrResponse;

    // get PinComplex ProcParam
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, ProcParam);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     ProcParam: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_ProcParam = azrResponse;

    fRes = TRUE;

//DONE:
    return fRes;
} // CPinComplex::Parse

//==============================================================================
// CPinComplex::RequiredParameterSupport
//
// Description:
//  Check PinComplex for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/12/03 - jwexler - created
//==============================================================================
BOOL 
CPinComplex::RequiredParameterSupport()
{
    BOOL fRes = TRUE;
    AzAudioWidgetCaps azAudioWidgetCaps = m_AudWidgetCaps.Response;

    SLOG(eInfo2, " Verifying Pin Complex at node %u", m_usNode);

    // AudWidgetCaps parameter covered by CAudioFunctionGroup::Parse

    SLOG(eInfo2, "  Verifying PinComplex for PinCaps");
    if (!m_PinCaps.Valid)
    {
        SLOG(eError, "Error: PinComplex at node %u does not support required parameter PinCaps", m_usNode);
        fRes = FALSE;
    }    
    AzPinCaps azPinCaps = (AzPinCaps)m_PinCaps.Response;

    //RPS: if outputcapable and any vref, then hi-z must be set
    if ((azPinCaps.OutputCapable) && 
        (0xFF00 & azPinCaps))
    {
        if (!azPinCaps.HiZ)
        {
            SLOG(eError, "Error: If the Output Capable bit and any bits in the VRef field are set, then bit 0 (Hi-Z) "
                         "must also be set to indicate that the VRef signal can be turned off to support output devices.");
            fRes = FALSE;
        }
    }
    
    //RPS: pincaps.reservedX must == 0
    //AzPinCaps.Reserved0 became AzPinCaps.HDMICapable
    if (
        azPinCaps.Reserved1 ||
        azPinCaps.Reserved2 ||
        azPinCaps.Reserved3 ||
        azPinCaps.Reserved4 ||
        azPinCaps.Reserved5
    )
    {
        SLOG(eError,
            "Error: All PinCaps.ReservedX fields must be 0\n"
            "Reserved1 (bit 11) is 0x%x\n"
            "Reserved2 (bits 14 and 15) is 0x%x\n"
            "Reserved3 (bits 17-23) is 0x%x\n"
            "Reserved4 (bits 25 and 26) is 0x%x\n"
            "Reserved5 (bits 28-31) is 0x%x",
            azPinCaps.Reserved1,
            azPinCaps.Reserved2,
            azPinCaps.Reserved3,
            azPinCaps.Reserved4,
            azPinCaps.Reserved5
        );
        fRes = FALSE;
    }

    //RPS: if hpcap then outcap
    if ((azPinCaps.HpDriveCapable) && (!azPinCaps.OutputCapable))
    {
        SLOG(eError, "Error: If a pin is Headphone Drive Capable, the Output Capable bit must also be set.");
        fRes = FALSE;
    }

    if (azAudioWidgetCaps.Digital)
    {
        // certain properties do not apply to digital pins
        if (azPinCaps.HpDriveCapable)
        {
            SLOG(eError, "Error: If a pin is digital, the Headphone Drive Capable bit cannot be set.");
            fRes = FALSE;
        }

        if (azPinCaps & VREF_CAPS_MASK)
        {
            SLOG(eError, "Error: If a pin is digital, the VRef capabilities bits must be 0.");
            fRes = FALSE;
        }

        if (azPinCaps.HBR && !azPinCaps.HDMICapable)
        {
            SLOG(eError, "Error: If the High Bit Rate bit is set, the HDMI bit must also be set.");
            fRes = FALSE;
        }
    }
    else
    {
        // certain properties do not apply to analog pins
        if (azPinCaps.HBR)
        {
            SLOG(eError, "Error: If a pin is analog, the High Bit Rate bit cannot be set.");
            fRes = FALSE;
        }

        if (azPinCaps.HDMICapable)
        {
            SLOG(eError, "Error: If a pin is analog, the HDMI bit cannot be set.");
            fRes = FALSE;
        }

        if (azPinCaps.DPCapable)
        {
            SLOG(eError, "Error: If a pin is analog, the DisplayPort bit cannot be set.");
            fRes = FALSE;
        }
    }

    SLOG(eInfo2, "  Verifying PinComplex for ConnListLength");
    if (!m_ConnListLength.Valid)
    {
        SLOG(eError, "Error: PinComplex at node %u does not support required parameter ConnListLength", m_usNode);
        fRes = FALSE;
    }

    SLOG(eInfo2, "  Verifying PinComplex for InAmpProps");
    if (m_InAmpProps.Valid)
    {
        AzAmpProperties azAmpProp = (AzAmpProperties)m_InAmpProps.Response;
        if (azAmpProp.Offset > azAmpProp.NumSteps)
        {
            SLOG(eError, "Error: PinComplex at node %u has InAmpProps.Offset > InAmpProps.NumSteps", m_usNode);
            fRes = FALSE;
        }

        if (azAmpProp.Reserved0 || azAmpProp.Reserved1 || azAmpProp.Reserved2)
        {
            SLOG(eError,
                "Error: Pin Complex at node %u has reserved bits set in Input Amplifier Properties; "
                "Reserved0 (7) = %u; Reserved1 (15) = %u; Reserved2 (23-30) = 0x%x",
                m_usNode, azAmpProp.Reserved0, azAmpProp.Reserved1, azAmpProp.Reserved2
            );
            fRes = FALSE;
        }
    }

    SLOG(eInfo2, "  Verifying PinComplex for OutAmpProps");
    if (m_OutAmpProps.Valid)
    {
        AzAmpProperties azAmpProp = (AzAmpProperties)m_OutAmpProps.Response;
        if (azAmpProp.Offset > azAmpProp.NumSteps)
        {
            SLOG(eError, "Error: PinComplex at node %u has OutAmpProps.Offset > OutAmpProps.NumSteps", m_usNode);
            fRes = FALSE;
        }

        if (azAmpProp.Reserved0 || azAmpProp.Reserved1 || azAmpProp.Reserved2)
        {
            SLOG(eError,
                "Error: Pin Complex at node %u has reserved bits set in Output Amplifier Properties; "
                "Reserved0 (7) = %u; Reserved1 (15) = %u; Reserved2 (23-30) = 0x%x",
                m_usNode, azAmpProp.Reserved0, azAmpProp.Reserved1, azAmpProp.Reserved2
            );
            fRes = FALSE;
        }
    }
    
//DONE:    
    return fRes;
} // CPinComplex::RequiredParameterSupport

//==============================================================================
// CPinComplex::RequiredVerbSupport
//
// Description:
//  Check PinComplex node for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  07/28/04 - jwexler - created
//==============================================================================
BOOL 
CPinComplex::RequiredVerbSupport()
{
    BOOL fRes = TRUE;    

    SLOG(eInfo2, "Verifying Pin Complex at node %u", m_usNode);
    
    // R: ConfigDefault
    // NOTE: This has to happen first, since later verification routines may rely
    //  on the configuration defaults data
    fRes = CheckConfigDef() ? fRes : FALSE;

    // if the pin is not connected, don't bother with further verbs
    if (ePcxPortConnNoPhysConn == AzPinWidgetConfigDefault(m_ConfigDef.Response).PortConn)
    {
        SLOG(eInfo1, "    Pin is not physically connected; skipping further verb validation.");
        goto DONE;
    }
    
    // c: ConnectionSelect
    // R: GetConnListEntry
    fRes = CheckConn() ? fRes : FALSE;

    // TODO
    // c: ProcState
    // c: CoeffIndex
    // c: ProcCoeff
    // c: AmpGainMute

    // c:PowerState
    fRes = m_PowerControl.Validate(this, eNodeType_AudioWidget, FALSE) ? fRes : FALSE;
    
    // TODO
    // c: Unsol Enable
    
    // c: PinSense
    fRes = CheckPinSense() ? fRes : FALSE;
    
    // c: EAPD/BTL Enable
    
    // R: PinWidget Control
    fRes = CheckPinControl() ? fRes : FALSE;    

DONE:
    return fRes;
} // CPinComplex::RequiredVerbSupport

//==============================================================================
// CPinComplex::CheckNode
//
// Description:
//  Verify that the node specified by usEntry is a valid connection list entry
//
// Arguments:
//  USHORT usEntry - the node id to check
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  11/03/04 - jwexler - created
//==============================================================================
BOOL 
CPinComplex::CheckNode(USHORT usEntry)
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;
    
    // verify that entry corresponds to actual node
    // Get widget caps
    Verb = MakeVerb(m_pCodec->m_usSDI, usEntry, AudWidgetCaps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     AudWidgetCaps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: Widget does not support required parameter AudWidgetCaps");
        fRes = FALSE;
        goto DONE;
    }
    // Verify that it's not a BeepGen
    ULONG ulWidgetType = (azrResponse.Response >> 20) & 0xf;
    if (AzAudBeepGen == ulWidgetType)
    {
        SLOG(eError, "Error: Beep Generators found in connection list.");
        fRes = FALSE;
    }    
    // verify that pin widgets don't have other pin widgets in their list
    if (AzAudNodePinComplex == ulWidgetType)
    {
        // SPDIF_In->SPDIF_Out PassThru is acceptable
        if (ePcxDefDevSpdifIn == ((AzPinWidgetConfigDefault)(m_ConfigDef.Response)).DefDev &&
            (ePcxDefDevSpdifOut == ((AzPinWidgetConfigDefault)(azrResponse.Response)).DefDev))
        {
            SLOG(eInfo3, "      SPDIF_In->SPDIF_Out PassThru");
        }
        else
        {
            SLOG(eError, "Error: Pin Widgets can not include other Pin Widgets in their connection list.");
            fRes = FALSE;
        }
    }

    // BUGBUG - can/should i neaten this function up to make use of CConnChecker::CheckNode?

    // TODO - verify that it's within bounds specified by function group
    // TODO - Verify that it's not shared across function groups?? Can I maintain a global list of Node IDs?
    // TODO - Verify connection is not orphaned.
DONE:
    return fRes;
} // CPinComplex::CheckNode

BOOL 
CPinComplex::PCTest(PinControlTest pincontroltest)
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    SLOG(eInfo1, " Testing %s", pincontroltest.szTestCase);        
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetPinControls, pincontroltest.azpc, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "  SetPinControls(%02Xh): Response == 0x%016I64X", (UCHAR)pincontroltest.azpc, (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: Pin Complex at node %u does not support required control SetPinControls", m_usNode);
        fRes = FALSE;
    }        

    // verify that it was set as expected
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetPinControls, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);    
    SLOG(eInfo3, "  GetPinControls: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (pincontroltest.fSupported)
    {
        if (pincontroltest.azpc != (UCHAR)(azrResponse.Response))
        {
            SLOG(eError,
                "Error: attempted to set PinControl 0x%02X; got 0x%02X.",
                pincontroltest.azpc, (UCHAR)(azrResponse.Response)
            );
            fRes = FALSE;
        }
    }
    else if (pincontroltest.fDigital)
    {
        AzDigitalPinControl azpcSent;
        AzDigitalPinControl azpcReceived;
        AzDigitalPinControl azpcOriginal;
        
        azpcSent = pincontroltest.azpc;
        azpcReceived = azrResponse.Response;
        azpcOriginal = m_PinControl;
    
        // for unsupported EPT values, spec says to either
        if (
            // reset to 0 (Native) or
            0 != azpcReceived.EPT &&
            // retain previous EPT value
            azpcOriginal.EPT != azpcReceived.EPT
        )
        {
            SLOG(eError,
                "Error: attempted to set unsupported EPT 0b%u%u (%s); "
                "previous value was 0b%u%u (%s); response was 0b%u%u (%s)\r\n"
                "For unsupported EPTs, the pin must either retain the previous value "
                "or take the value of 0b00 (Native).\r\n"
                "See HD Audio specification section 7.3.3.13.",
                (azpcSent.EPT >> 1) & 1,
                    azpcSent.EPT & 1,
                    Stringify::EPT(azpcSent.EPT), // command sent
                (azpcOriginal.EPT >> 1) & 1,
                    azpcOriginal.EPT & 1,
                    Stringify::EPT(azpcOriginal.EPT), // previous value
                (azpcReceived.EPT >> 1) & 1,
                    azpcReceived.EPT & 1,
                    Stringify::EPT(azpcReceived.EPT) // value retrieved
            );
            fRes = FALSE;
        }
    }
    else
    {
        AzPinControl azpcSent;
        AzPinControl azpcReceived;
        AzPinControl azpcOriginal;

        azpcSent = pincontroltest.azpc;
        azpcReceived = azrResponse.Response;
        azpcOriginal = m_PinControl;
        
        // for unsupported VRefs, spec says to either
        if (
            //  reset to 0 (Hi-Z) or
            0 != azpcReceived.VRefEn &&
            //  retain previous VRef value
            azpcOriginal.VRefEn != azpcReceived.VRefEn
        )
        {
            SLOG(eError,
                "Error: attempted to set unsupported VRef 0b%u%u%u (%s); "
                "previous value was 0b%u%u%u (%s); response was 0b%u%u%u (%s)\r\n"
                "For unsupported VRefs, the pin must either retain the previous value "
                "or take the value of 0b000 (Hi-Z).\r\n"
                "See HD Audio specification section 7.3.3.13.",
                (azpcSent.VRefEn >> 2) & 1,
                    (azpcSent.VRefEn >> 1) & 1,
                    azpcSent.VRefEn & 1,
                    Stringify::VRefEn(azpcSent.VRefEn), // command sent
                (azpcOriginal.VRefEn >> 2) & 1,
                    (azpcOriginal.VRefEn >> 1) & 1,
                    azpcOriginal.VRefEn & 1,
                    Stringify::VRefEn(azpcOriginal.VRefEn), // previous value
                (azpcReceived.VRefEn >> 2) & 1,
                    (azpcReceived.VRefEn >> 1) & 1,
                    azpcReceived.VRefEn & 1,
                    Stringify::VRefEn(azpcReceived.VRefEn) // value retrieved
            );
            fRes = FALSE;
        }
    }
    m_PinControl = (UCHAR)(azrResponse.Response); // keep track of current state for reference

    return fRes;
}

//==============================================================================
// CPinComplex::CheckPinControl
//
// Description:
//  Verify that PinComplex supports the PinControl verbs correctly
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  11/05/04 - jwexler - created
//==============================================================================
BOOL 
CPinComplex::CheckPinControl()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;
    UCHAR azpcOriginal;
    AzPinCaps azPinCaps = m_PinCaps.Response;
    AzAudioWidgetCaps azAudioWidgetCaps = (AzAudioWidgetCaps)m_AudWidgetCaps.Response;
    PinControlTest pincontroltest;
    AzPinControl analog = 0;
    AzDigitalPinControl digital = 0;

    SLOG(eInfo2, "Checking Pin Complex's PinControl support.");    
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetPinControls, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);    
    SLOG(eInfo3, " GetPinControls: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: Pin Complex at node %u does not support required control GetPinControls!", m_usNode);
        fRes = FALSE;
        goto DONE;
    }    
    m_PinControl = (UCHAR)(azrResponse.Response);
    azpcOriginal = m_PinControl;

    // digital pins have EPT; analog pins have VRef
    pincontroltest.fDigital = (azAudioWidgetCaps.Digital);

    // all off / Hi-Z / native EPT
    pincontroltest.azpc = 0;
    pincontroltest.fSupported = TRUE;
    pincontroltest.szTestCase = "all 0 / Hi-Z / Native EPT";
    fRes = PCTest(pincontroltest) ? fRes : FALSE;
    
    if (azPinCaps.OutputCapable)
    {
        // Out Enable = 1
        analog = 0;
        analog.OutEnable = 1;
        digital = 0;
        digital.OutEnable = 1;
        
        pincontroltest.azpc = (pincontroltest.fDigital ? (UCHAR)digital : (UCHAR)analog);
        pincontroltest.fSupported = TRUE;
        pincontroltest.szTestCase = "set OutEnable = 1";
        fRes = PCTest(pincontroltest) ? fRes : FALSE;

        // Out Enable = 0
        pincontroltest.azpc = 0;
        pincontroltest.fSupported = TRUE;
        pincontroltest.szTestCase = "set OutEnable = 0";
        fRes = PCTest(pincontroltest) ? fRes : FALSE;

        if (azPinCaps.HpDriveCapable)
        {            
            // H-Phn Enable = 1
            analog = 0;
            analog.OutEnable = 1;
            analog.HPhnEnable = 1;
            digital = 0;
            digital.OutEnable = 1;
            digital.HPhnEnable = 1;
        
            pincontroltest.azpc = (pincontroltest.fDigital ? (UCHAR)digital : (UCHAR)analog);
            pincontroltest.fSupported = TRUE;
            pincontroltest.szTestCase = "set H-Phn Enable = 1";
            fRes = PCTest(pincontroltest) ? fRes : FALSE;
            
            // H-Phn Enable = 0            
            analog = 0;
            analog.OutEnable = 1;
            analog.HPhnEnable = 0;
            digital = 0;
            digital.OutEnable = 1;
            digital.HPhnEnable = 0;
        
            pincontroltest.azpc = (pincontroltest.fDigital ? (UCHAR)digital : (UCHAR)analog);
            pincontroltest.fSupported = TRUE;
            pincontroltest.szTestCase = "set H-Phn Enable = 0";
            fRes = PCTest(pincontroltest) ? fRes : FALSE;            
        }
    }

    if (azPinCaps.InputCapable)
    {        
        // In Enable = 1
        analog = 0;
        analog.InEnable = 1;
        digital = 0;
        digital.InEnable = 1;
    
        pincontroltest.azpc = (pincontroltest.fDigital ? (UCHAR)digital : (UCHAR)analog);
        pincontroltest.fSupported = TRUE;
        pincontroltest.szTestCase = "set InEnable = 1";
        fRes = PCTest(pincontroltest) ? fRes : FALSE;
        // In Enable = 0
        pincontroltest.azpc = 0;
        pincontroltest.fSupported = TRUE;
        pincontroltest.szTestCase = "set InEnable = 0";
        fRes = PCTest(pincontroltest) ? fRes : FALSE;
    } else { // if InputCapable
        // not InputCapable
        if (0 != (azPinCaps & VREF_CAPS_MASK)) {
            // pins that are not input capable must have VRef caps bits set to 0
            SLOG(eError,
                "Error: Pin Complex at node %u is not input capable, but claims support for VRefs: PinCaps = 0x%08X. See HD Audio spec 7.3.4.9",
                m_usNode,
                (ULONG)azPinCaps
            );
            fRes = FALSE;
        }
    }

    // Encoded Packet Type testing - digital outputs only
    if (azAudioWidgetCaps.Digital && azPinCaps.OutputCapable)
    {
        // ept (digital only)
        for (UCHAR ept = 0; ept <= 3; ept++) // two-bit value
        {
            digital = 0;
            digital.OutEnable = 1;
            digital.EPT = ept;
            pincontroltest.azpc = digital;
            
            switch (ept)
            {
                case 0: // Native (00b)
                    pincontroltest.fSupported = TRUE; // native is always supported
                    pincontroltest.szTestCase = "set Encoded Packet Type = Native (00b)";
                break;
                case 1:
                    pincontroltest.fSupported = FALSE; // reserved is never supported
                    pincontroltest.szTestCase = "set Encoded Packet Type = Reserved (01b)";
                break;
                case 2:
                    pincontroltest.fSupported = FALSE; // reserved is never supported
                    pincontroltest.szTestCase = "set Encoded Packet Type = Reserved (10b)";
                break;
                case 3:
                    pincontroltest.fSupported = azPinCaps.HBR; // high bit rate
                    pincontroltest.szTestCase = "set Encoded Packet Type = High Bit Rate (11b)";
                break;
            }
            fRes = PCTest(pincontroltest) ? fRes : FALSE;
        }
    }

    // Voltage Reference Enable testing - analog inputs only
    if (!azAudioWidgetCaps.Digital && azPinCaps.InputCapable)
    {
        // vref (analog only)
        for (UCHAR vref = 0; vref <= 7; vref++) // 3 bit value from 000b->111b
        {
            analog = 0;
            analog.InEnable = 1;
            analog.VRefEn = vref;
            pincontroltest.azpc = analog;

            switch (vref)
            {
                case 0: // Hi-Z (000b)
                    pincontroltest.fSupported = azPinCaps.HiZ;
                    pincontroltest.szTestCase = "set VRefEn = HiZ (000b)";
                break;
                case 1:  // 50%(001b)
                    pincontroltest.fSupported = azPinCaps.Volt50;
                    pincontroltest.szTestCase = "set VRefEn = Volt50 (001b)";
                break;
                case 2:  // Ground (0V) (010b)
                    pincontroltest.fSupported = azPinCaps.Ground;
                    pincontroltest.szTestCase = "set VRefEn = Ground (010b)";
                break;
                case 3:  // Reserved(011b)
                    pincontroltest.fSupported = FALSE;//azPinCaps.Reserved1;
                    pincontroltest.szTestCase = "set VRefEn = Reserved (011b)";
                break;
                case 4:  // 80%(100b)
                    pincontroltest.fSupported = azPinCaps.Volt80;
                    pincontroltest.szTestCase = "set VRefEn = Volt80 (100b)";
                break;
                case 5:  // 100%(101b)
                    pincontroltest.fSupported = azPinCaps.Volt100;
                    pincontroltest.szTestCase = "set VRefEn = Volt100 (101b)";
                break;
                case 6:  // Reserved(110b)
                    pincontroltest.fSupported = FALSE;//azPinCaps.Reserved2;
                    pincontroltest.szTestCase = "set VRefEn = Reserved (110b)";
                break;
                case 7:  // Reserved(111b)
                    pincontroltest.fSupported = FALSE;//azPinCaps.Reserved2;
                    pincontroltest.szTestCase = "set VRefEn = Reserved (111b)";
                break;
            } // switch vref
            fRes = PCTest(pincontroltest) ? fRes : FALSE;
        } // for vref
    } // analog

    // restore original settings    
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetPinControls, azpcOriginal, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, " SetPinControls(original): Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: Pin Complex at node %u does not support required control SetPinControls", m_usNode);
        fRes = FALSE;
    }

DONE:
    return fRes;
} // CPinComplex::CheckPinControl

//==============================================================================
// CPinComplex::CheckConfigDef
//
// Description:
//  Verify that PinComplex supports the ConfigDefault verbs correctly
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  11/05/04 - jwexler - created
//==============================================================================
BOOL 
CPinComplex::CheckConfigDef()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;
    AzPinWidgetConfigDefault azpwConfigDef;

    SLOG(eInfo2, "Checking Pin Complex's ConfigDefault support.");    
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetConfigDefault, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);    
    SLOG(eInfo3, " GetConfigDefault: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_ConfigDef = azrResponse;
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: Pin Complex at node %u does not support required control GetConfigDefault!", m_usNode);
        fRes = FALSE;
    }

    azpwConfigDef = azrResponse.Response;
    if (0 == azpwConfigDef)
    {
        SLOG(eError, "Error: Pin Complex at node %u must not return 0 for GetConfigDefault", m_usNode);
        fRes = FALSE;
    }

    // write the same defaults back in
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetConfigDefault0, (USHORT)azpwConfigDef & 0xFF, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, " SetConfigDefault0(original): Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: Pin Complex at node %u does not support required control SetConfigDefault0", m_usNode);
        fRes = FALSE;
    }
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetConfigDefault1, (USHORT)(azpwConfigDef>>8) & 0xFF, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, " SetConfigDefault1(original): Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: Pin Complex at node %u does not support required control SetConfigDefault1", m_usNode);
        fRes = FALSE;
    }
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetConfigDefault2, (USHORT)(azpwConfigDef>>16) & 0xFF, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, " SetConfigDefault2(original): Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: Pin Complex at node %u does not support required control SetConfigDefault2", m_usNode);
        fRes = FALSE;
    }
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SetConfigDefault3, (USHORT)(azpwConfigDef>>24) & 0xFF, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, " SetConfigDefault3(original): Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: Pin Complex at node %u does not support required control SetConfigDefault3", m_usNode);
        fRes = FALSE;
    }

    // verify that it's still the same
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetConfigDefault, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);    
    SLOG(eInfo3, " GetConfigDefault: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (azpwConfigDef != azrResponse.Response)
    {
        SLOG(eError, "Error: ConfigDefault was not written back correctly!");
        fRes = FALSE;
    }

    // verify reserved controls F1D, F1E, and F1F
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetConfigDefault_Reserved0, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "  GetConfigDefault(F1D): Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid || (azrResponse.Response && (azpwConfigDef != azrResponse.Response)))
    {
        SLOG(eError, "Error: Codec must either respond to F1D control identically to F1C "
                     "or respond with 0");
        fRes = FALSE;
    }
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetConfigDefault_Reserved1, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "  GetConfigDefault(F1E): Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid || (azrResponse.Response && (azpwConfigDef != azrResponse.Response)))
    {
        SLOG(eError, "Error: Codec must either respond to F1E control identically to F1C "
                     "or respond with 0");
        fRes = FALSE;
    }
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetConfigDefault_Reserved2, 0, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "  GetConfigDefault(F1F): Response == 0x%016I64X", (ULONGLONG)azrResponse);
    if (!azrResponse.Valid || (azrResponse.Response && (azpwConfigDef != azrResponse.Response)))
    {
        SLOG(eError, "Error: Codec must either respond to F1F control identically to F1C "
                     "or respond with 0");
        fRes = FALSE;
    }

    return fRes;
} // CPinComplex::CheckConfigDef

//==============================================================================
// CPinComplex::CheckPinSense
//
// Description:
//  Check the pin complex response to the PinSense verb
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  09/23/2008 - MatEer - created 
//==============================================================================
BOOL 
CPinComplex::CheckPinSense()
{
    BOOL fRes = TRUE;

    AzAudioWidgetCaps azAudioWidgetCaps = m_AudWidgetCaps.Response;
    AzPinCaps azPinCaps = m_PinCaps.Response;
    AzPinWidgetConfigDefault azpwConfigDef = m_ConfigDef.Response;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    // see if this is pin-sense-capable
    if (!azPinCaps.PresenceDetect) { return TRUE; }
    if (!azAudioWidgetCaps.Digital && azpwConfigDef.JackDetectOverride) { return TRUE; } 

    if (azAudioWidgetCaps.Digital)
    {
        // digital
        //SPDIF is not required to have jack detection
        if (azpwConfigDef.JackDetectOverride && azpwConfigDef.DefDev != ePcxDefDevSpdifOut && azpwConfigDef.DefDev != ePcxDefDevSpdifIn)
        {
            SLOG(eError, "Error: Analog connections and digital connections (other than S/PDIF) are required to support jack detection");
            fRes = FALSE;
        }

        if (azPinCaps.ImpSense)
        {
            SLOG(eError, "Error: Impedance Sense only applies to analog pins");
            fRes = FALSE;
        }

        if (azPinCaps.TriggerRequired)
        {
            SLOG(eError, "Error: Trigger Required only applies to analog pins");
            fRes = FALSE;
        }
    }
    else
    {
        // analog
        if (azPinCaps.TriggerRequired)
        {
            XLOG(XWARN, eWarn1, "Warning: impedance measurement requires a trigger, which we're not doing in this test");
        }        
    }

    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, GetPinSense, 0);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    if (azrResponse.Valid)
    {
        if (azAudioWidgetCaps.Digital)
        {
            AzDigitalPinSenseResponse azPinSense = azrResponse.Response;
            SLOG(eInfo1,
                "Pin sense response for digital pin widget is: "
                "Presence Detect = %u, ELD Valid = %u, Reserved = 0x%0x",
                azPinSense.PresenceDetect, azPinSense.ELDValid, azPinSense.Reserved0
            );

            if (0 != azPinSense.Reserved0)
            {
                // escape hatch for non-DCN compliant HDMI codecs
                if (
                    (0x7fffffff & azrResponse.Response) == 0x7fffffff // Impedance "N/A" sentinel value
                )
                {
                    CAudioFunctionGroup *pFG = (CAudioFunctionGroup *)m_parentFG;

                    // if this is an HDMI pin, and this is pre-DCN HDMI,
                    // then this is OK
                    if (
                        ePcxGenLocInternal == azpwConfigDef.GenLoc &&
                        ePcxGeoLocSpecial1 == azpwConfigDef.GeoLoc &&
                        NULL != pFG &&
                        !pFG->m_SupportsHDMIPerAudio0078
                    )
                    {
                        XLOG(XWARN, eWarn1,
                            "Pin sense response for pre-DCN HDMI digital pin widget has lower 31 bits set to 1. "
                            "This is allowable per AUDIO-0078."
                        );
                    }

                    // unfortunately there's no way to distinguish pre-DCN S/PDIF
                    // from post-DCN S/PDIF
                    // so we maintain the higher bar
                }
                else
                {
                    SLOG(eError, "Error: GetPinSense response has reserved bits set: 0x%08x", (ULONG)azPinSense);
                    fRes = FALSE;
                }
            } // 0 != Reserved0
            else if (
                0 != azPinSense.ELDValid &&
                0 == azPinCaps.HDMICapable &&
                0 == azPinCaps.DPCapable
            )
            {
                SLOG(eError, "Error: ELD Valid bit is set but this is not an HDMI or DisplayPort pin widget");
                fRes = FALSE;
            }
        }
        else
        {
            AzPinSenseResponse azPinSense = azrResponse.Response;

            SLOG(eInfo1,
                "Pin sense response for analog pin widget is: Presence Detect = %u, Impedance = 0x%0x",
                azPinSense.PresenceDetect, azPinSense.Impedance
            );
        }
    }
    else
    {
        SLOG(eError, "Error: GetPinSense verb resulted in invalid response");
        fRes = FALSE;
    }
    
    return fRes;
}


//==============================================================================
// CPinComplex::ValidatePinConfig
//
// Description:
//  Check the pin complex for compliance with our pin configuration requirements
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  02/24/06 - jwexler - created
//==============================================================================
BOOL 
CPinComplex::ValidatePinConfig()
{
    BOOL fRes = TRUE;
    AzPinWidgetConfigDefault azpwConfigDef = m_ConfigDef.Response;
    AzPinCaps azpCaps = m_PinCaps.Response;

    SLOG(eInfo2, "Verifying Pin Complex at node %u", m_usNode);

    /*
    // HACKHACK - For Device Kit submissions we allow codecs to not have proper config defaults
    //      For Systems Kit submissions they must be correct, unless the codec is pre-1.0 (for HCT 12.1)
    //      These exceptions will no longer be granted in the LH timeframe.
    //#############################################################################
    ELogLevel elogLevel;
    TCHAR* szLogPrefix;
    BOOL fPre1_0 = (1 > ((m_pCodec->m_rootnode.m_RevisionID>>20) & 0xF));
    BOOL fException = FALSE;
    if (g_pShell->m_pszHwIdTestDeviceSelect || fPre1_0) // device kit submissions always specify pnpid on cmdline (-h param)
    {
        elogLevel = eWarn1;
        szLogPrefix = "Warning:";
        fException = TRUE;
    }
    else
    {
        elogLevel = eError;
        szLogPrefix = "Error:";
        fException = FALSE;
    }
    */    

    //
    // Check the rules    
    //    
    // TODO - can i offer warnings about defaults we might not understand or think are weird.
    
    // PortConnectivity[31:30]
    //  A pin’s port connectivity value can only be set to 0x01 when a system that the HD Audio codec is in does not 
    //      have a physical jack connection for that pin. 
    if (ePcxPortConnNoPhysConn == azpwConfigDef.PortConn)
    {        
        XLOG(XWARN, eWarn1, "Warning: A pin's port connectivity value can only be set to 0x01 when a system that the "
                     "HD Audio codec is in does not have a physical jack connection for that pin.");
        goto DONE; // Pin not connected, so ignore the rest of the pinconfig
    }

    // Location[29:24]
    if (ePcxGenLocInternal != azpwConfigDef.GenLoc)
    {
        //  0h and Ah-Fh is reserved for bits[27:24] - GeoLoc
        if (ePcxGeoLocReserved0 == azpwConfigDef.GeoLoc)
        {
            SLOG(eError, "Error: Location field can not be 0.");
            fRes = FALSE;
        }
    }
    
    if ((ePcxGeoLocReserved1 <= azpwConfigDef.GeoLoc) && (ePcxGeoLocReserved6 >= azpwConfigDef.GeoLoc))
    {
        SLOG(eError, "Error: Ah-Fh are reserved values for Location");
        fRes = FALSE;
    }

    // DefaultDevice[23:20]
    // Eh is reserved and should not be allowed
    if (ePcxDefDevReserved0 == azpwConfigDef.DefDev)
    {
        SLOG(eError, "Error: Eh is a reserved value for DefaultDevice.");
        fRes = FALSE;
    }

    // ConnectionType[19:16]
    //  can't be 0
    //  Ch-Fh are reserved
    if (ePcxConnTypeUnknown == azpwConfigDef.ConnType)
    {
        SLOG(eError, "Error: ConnectionType field can not be 0.");
        fRes = FALSE;
    }
    if ((ePcxConnTypeReserved0 <= azpwConfigDef.ConnType) && (ePcxConnTypeReserved3 >= azpwConfigDef.ConnType))
    {
        SLOG(eError, "Error: 0h and Ah-Fh are reserved values for ConnectionType");
        fRes = FALSE;
    }

    switch (azpwConfigDef.DefDev)
    {
        case ePcxDefDevLineOut:
        case ePcxDefDevSpeaker:
        case ePcxDefDevHPOut:
        case ePcxDefDevSpdifOut:
        case ePcxDefDevDigitalOtherOut:
        case ePcxDefDevLineIn:
        case ePcxDefDevAUX:
        case ePcxDefDevMicIn:
        case ePcxDefDevSpdifIn:
        break;
        
        default:
            XLOG(XWARN, eWarn1, "Warning: Default Device value of %01Xh not supported by Microsoft HDAudio Class Driver.", azpwConfigDef.DefDev);
            //fRes = FALSE;
    }

    if (ePcxDefDevAUX == azpwConfigDef.DefDev)
    {
        if ((ePcxGenLocInternal != azpwConfigDef.GenLoc) || 
            (ePcxPortConnIntegratedDevice != azpwConfigDef.PortConn))
        {
            SLOG(eError, "Error: AUX Device must be internal and fixed-function.");
            fRes = FALSE;
        }
    }

    // Color[15:12]
    //  can't be 0
    // Ah-Dh are reserved and should not be allowed
    if (ePcxGenLocInternal == azpwConfigDef.GenLoc)
    {
        // fine, internal jack
    }
    else if (
        ePcxGenLocOther == azpwConfigDef.GenLoc && // 3
        ePcxGeoLocSpecial0 == azpwConfigDef.GeoLoc // 7; for GenLoc = Other, this means "inside mobile lid"
    )
    {
        // fine, this is "inside mobile lid"
        // per the chart in the HD Audio spec 7.3.3.31
    }
    else
    {
        if (ePcxColorUnknown == azpwConfigDef.Color)
        {
            SLOG(eError, "Error: Color field cannot be 0.");
            fRes = FALSE;
        }
    }
    
    if ((ePcxColorReserved0 <= azpwConfigDef.Color) && (ePcxColorReserved3 >= azpwConfigDef.Color))
    {
        SLOG(eError, "Error: Ah-Fh are reserved values for Color");
        fRes = FALSE;
    }
    
    //  In order to avoid end-user confusion and to increase usability no two jacks in the same location should 
    //    have the same color.  
    //      This gives the Microsoft UAA HD Audio class driver power to unambiguously instruct end-users in how to 
    //      setup speakers and other peripheral devices.
    if (ePcxGenLocInternal != azpwConfigDef.GenLoc)
    {

#pragma prefast( suppress: __WARNING_BUFFER_OVERFLOW, "PREFast doesn't realize a 4-bit member can't be more than 0b1111 == 15")
        if (m_pCodec->m_rgusColors[azpwConfigDef.GenLoc][azpwConfigDef.GeoLoc] & (1 << azpwConfigDef.Color))
        {
            XLOG(XWARN, eWarn1, "Warning: This color has already been used by another jack in this location.");
            //fRes = FALSE;
        }
#pragma prefast( suppress: __WARNING_BUFFER_OVERFLOW, "PREFast doesn't realize a 4-bit member can't be more than 0b1111 == 15")
        m_pCodec->m_rgusColors[azpwConfigDef.GenLoc][azpwConfigDef.GeoLoc] |= (1 << azpwConfigDef.Color);
    }
    // Misc[11:8]
    // bit 8 is "jack detect override"
    // bits 9 through 11 are reserved and must not be set as future DCNs may assign them a meaning
    if (0 != azpwConfigDef.Reserved)
    {
        SLOG(eError,
            "Error: Bits 9 through 11 (bits 1-3 in the Misc field) are reserved and must be set to 0, not 0x%x. "
            "Setting reserved bits to 0 is necessary as future DCNs may assign new meanings to reserved bits.",
            azpwConfigDef.Reserved
        );
        fRes = FALSE;
    }
    
    if (
        0 == azpCaps.PresenceDetect &&
        1 == azpwConfigDef.JackDetectOverride
    ) {
        XLOG(XWARN, eWarn1, "Warning: Jack Detect Override set, but pin doesn't support presence detect anyway.");
    }

    // sanity-check jack detection against connection type, device type, and pin capabilities
    if (
        1 == azpCaps.PresenceDetect &&
        0 == azpwConfigDef.JackDetectOverride
    ) // pin claims to support jack detection
    {
        if (
            ePcxConnTypeEighth == azpwConfigDef.ConnType ||
            ePcxConnTypeQuarter == azpwConfigDef.ConnType
        )
        {
            // fine
        } else if (
            ePcxConnTypeOtherDigital == azpwConfigDef.ConnType &&
            ePcxDefDevDigitalOtherOut == azpwConfigDef.DefDev
            // note that setting HDMI Capable or DisplayPort capable is optional
            // per AUDIO-0078
        )
        {
            // fine, digital display (HDMI or DisplayPort)
        } else if (
            ePcxDefDevSpdifIn  == azpwConfigDef.DefDev ||
            ePcxDefDevSpdifOut == azpwConfigDef.DefDev
        )
        {
            // fine, "jack-presence-detecting" S/PDIF
            // can be done on any connection type
            // by looking for a digital handshake
        } else
        {
            // not any kind of jack-detecting connection I've ever seen...
            SLOG(eError,
                "Error: Pin claims support for Presence Detect with\n"
                "connection type %01Xh (%s), default device %01Xh (%s),\n"
                "location %u (%s) / %u (%s),\n"
                "HDMI Capable = %u, and DisplayPort Capable = %u",
                azpwConfigDef.ConnType, Stringify::ConnectionType(azpwConfigDef.ConnType),
                azpwConfigDef.DefDev, Stringify::DefaultDevice(azpwConfigDef.DefDev),
                azpwConfigDef.GenLoc, Stringify::GrossLocation(azpwConfigDef.GenLoc),
                azpwConfigDef.GeoLoc, Stringify::GeometricLocation(azpwConfigDef.GeoLoc),
                azpCaps.HDMICapable, azpCaps.DPCapable
            );
            fRes = FALSE;
        }
    }

    // DefaultAssociation[7:4]    
    //  a value of 0000b is reserved and should not be used.
    //  The default association value 0x0F has special meaning and caution must be used when setting this value. 
    //      Audio devices with an audio data path that ends with a pin widget that has a value of 0x0F as its 
    //      default association value will be exposed as an independent logical device in which case no grouping 
    //      will occur. If several pin widgets share this value then each widget is exposed as a separate and 
    //      independent device. If the default association is set to 0x0F then the sequence number must be 0.
    // Sequence[3:0]
    if (0 == azpwConfigDef.DefAssoc)
    {
        SLOG(eError, "Error: DefaultAssociation field can not be 0.");
        fRes = FALSE;
    }
    if ((0x0F == azpwConfigDef.DefAssoc) && (0 != azpwConfigDef.Sequence))
    {
        SLOG(eError, "Error: If the DefaultAssociation is set to 0x0F, then the Sequence number must be 0.");
        fRes = FALSE;
    }
    
    //  The sequence numbers within the pin configuration register’s default association must be unique within the 
    //      same association.
    //  The lowest numbered jack in the association group should be assigned the lowest numbered channels in the stream, etc.,
    //  The numbers need not be sequential within the group, only the order matters.
    // Does not apply to special-case Association group 0xF.
    SLOG(eInfo1, " Association: %01Xh", azpwConfigDef.DefAssoc);
    SLOG(eInfo1, " Sequence: %01Xh", azpwConfigDef.Sequence);
    if (0xF != azpwConfigDef.DefAssoc)
    {
        if (m_pCodec->m_rgSequences[azpwConfigDef.DefAssoc] & (1 << azpwConfigDef.Sequence))
        {
            SLOG(eError, "Error: Sequence number %01Xh has already been used by another pin "
                            "in the same Association group %01Xh.", azpwConfigDef.Sequence, azpwConfigDef.DefAssoc);
            fRes = FALSE;
        }
        m_pCodec->m_rgSequences[azpwConfigDef.DefAssoc] |= (1 << azpwConfigDef.Sequence);
    }

DONE:
    return fRes;
} // CPinComplex::ValidatePinConfig


//==============================================================================
// CPowerWidget::Parse
//
// Description:
//  Parse the PowerWidget widget and populate internal structures 
//  with support information
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/10/03 - jwexler - created
//==============================================================================
BOOL 
CPowerWidget::Parse()
{
    BOOL fRes = FALSE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    // get PowerWidget ConnListLength
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, ConnListLength);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     ConnListLength: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_ConnListLength = azrResponse;

    // get PowerWidget SupPwrStates
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, SupPwrStates);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     SupPwrStates: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_SupPwrStates = azrResponse;

    fRes = TRUE;

//DONE:
    return fRes;
} // CPowerWidget::Parse

//==============================================================================
// CPowerWidget::RequiredParameterSupport
//
// Description:
//  Check PowerWidget for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/12/03 - jwexler - created
//==============================================================================
BOOL 
CPowerWidget::RequiredParameterSupport()
{
    BOOL fRes = TRUE;    

    SLOG(eInfo2, " Verifying Power Widget at node %u", m_usNode);

    // AudWidgetCaps parameter covered by CAudioFunctionGroup::Parse

    SLOG(eInfo2, "  Verifying PowerWidget for ConnListLength");
    if (!m_ConnListLength.Valid)
    {
        SLOG(eError, "Error: PowerWidget at node %u does not support required parameter ConnListLength", m_usNode);
        fRes = FALSE;
    }

    SLOG(eInfo2, "  Verifying PowerWidget for SupPwrStates");
    if (!m_SupPwrStates.Valid)
    {
        SLOG(eError, "Error: PowerWidget at node %u does not support required parameter SupPwrStatest", m_usNode);
        fRes = FALSE;
    }
    
//DONE:    
    return fRes;
} // CPowerWidget::RequiredParameterSupport

//==============================================================================
// CPowerWidget::RequiredVerbSupport
//
// Description:
//  Check PowerWidget node for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  07/28/04 - jwexler - created
//==============================================================================
BOOL 
CPowerWidget::RequiredVerbSupport()
{
    BOOL fRes = TRUE;

    SLOG(eInfo2, "Verifying Power Widget at node %u", m_usNode);
    
    // R: GetConnListEntry
    fRes = CheckConn() ? fRes : FALSE;

    // R: PowerState
    fRes = m_PowerControl.Validate(this, eNodeType_AudioWidget, TRUE) ? fRes : FALSE;

    // TODO
    // c: Unsol Enable
    
    return fRes;
} // CPowerWidget::RequiredVerbSupport

//==============================================================================
// CVolumeKnob::Parse
//
// Description:
//  Parse the VolumeKnob widget and populate internal structures 
//  with support information
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/10/03 - jwexler - created
//==============================================================================
BOOL 
CVolumeKnob::Parse()
{
    BOOL fRes = FALSE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    // get VolumeKnob VolKnobCaps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, VolKnobCaps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     VolKnobCaps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_VolKnobCaps = azrResponse;

    fRes = TRUE;

//DONE:
    return fRes;
} // CVolumeKnob::Parse

//==============================================================================
// CVolumeKnob::RequiredParameterSupport
//
// Description:
//  Check VolumeKnob for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/12/03 - jwexler - created
//==============================================================================
BOOL 
CVolumeKnob::RequiredParameterSupport()
{
    BOOL fRes = TRUE;    

    SLOG(eInfo2, " Verifying Volume Knob at node %u", m_usNode);

    // AudWidgetCaps parameter covered by CAudioFunctionGroup::Parse

    return fRes;
} // CVolumeKnob::RequiredParameterSupport

//==============================================================================
// CVolumeKnob::RequiredVerbSupport
//
// Description:
//  Check VolumeKnob node for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  07/28/04 - jwexler - created
//==============================================================================
BOOL 
CVolumeKnob::RequiredVerbSupport()
{
    BOOL fRes = TRUE;

    SLOG(eInfo2, "Verifying Volume Knob at node %u", m_usNode);
    
    // R: GetConnListEntry
    fRes = CheckConn() ? fRes : FALSE;

    // TODO
    // c: Unsol Enable

    // TODO
    // R: Volume Knob Control
    
    return fRes;
} // CVolumeKnob::RequiredVerbSupport

//==============================================================================
// CBeepGen::Parse
//
// Description:
//  Parse the BeepGen widget and populate internal structures 
//  with support information
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  07/06/04 - jwexler - created
//==============================================================================
BOOL 
CBeepGen::Parse()
{
    BOOL fRes = FALSE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;    

    fRes = TRUE;

//DONE:
    return fRes;
} // CBeepGen::Parse

//==============================================================================
// CBeepGen::RequiredParameterSupport
//
// Description:
//  Check VolumeKnob for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  07/06/04 - jwexler - created
//==============================================================================
BOOL 
CBeepGen::RequiredParameterSupport()
{
    BOOL fRes = TRUE;    

    SLOG(eInfo2, " Verifying Beep Generator at node %u", m_usNode);

    // AudWidgetCaps parameter covered by CAudioFunctionGroup::Parse

    return fRes;
} // CBeepGen::RequiredParameterSupport

//==============================================================================
// CBeepGen::RequiredVerbSupport
//
// Description:
//  Check BeepGen node for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  07/28/04 - jwexler - created
//==============================================================================
BOOL 
CBeepGen::RequiredVerbSupport()
{
    BOOL fRes = TRUE;

    SLOG(eInfo2, "Verifying Beep Generator at node %u", m_usNode);
    
    // TODO
    // c: amp gain/mute
    // R: Beep Generation Control
    
    return fRes;
} // CBeepGen::RequiredVerbSupport

//==============================================================================
// CVendorWidget::Parse
//
// Description:
//  Parse the VendorWidget widget and populate internal structures 
//  with support information
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/12/03 - jwexler - created
//==============================================================================
BOOL 
CVendorWidget::Parse()
{
    BOOL fRes = FALSE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;

    // BUGBUG - should probably check all known parameters
    //          since spec suggests to use known parameters where appropriate
    /*
    // get VendorWidget VolKnobCaps
    Verb = MakeVerb(m_pCodec->m_usSDI, m_usNode, VolKnobCaps);
    azrResponse = SendVerb(m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "     VolKnobCaps: Response == 0x%016I64X", (ULONGLONG)azrResponse);
    m_VolKnobCaps = azrResponse;
    */

    fRes = TRUE;

//DONE:
    return fRes;
} // CVendorWidget::Parse

//==============================================================================
// CVendorWidget::RequiredParameterSupport
//
// Description:
//  Check VendorWidget for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  12/12/03 - jwexler - created
//==============================================================================
BOOL 
CVendorWidget::RequiredParameterSupport()
{
    BOOL fRes = TRUE;

    SLOG(eInfo2, " Verifying Vendor-defined Widget at node %u", m_usNode);

    // AudWidgetCaps parameter covered by CAudioFunctionGroup::Parse

    return fRes;
} // CVendorWidget::RequiredParameterSupport

//==============================================================================
// CVendorWidget::RequiredVerbSupport
//
// Description:
//  Check VendorWidget node for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  07/28/04 - jwexler - created
//==============================================================================
BOOL 
CVendorWidget::RequiredVerbSupport()
{
    BOOL fRes = TRUE;

    SLOG(eInfo2, "Verifying Vendor-defined Widget at node %u", m_usNode);
    
    // TODO(*) - is there anything I can check here?
    
    return fRes;
} // CVendorWidget::RequiredVerbSupport

//==============================================================================
//                              Test cases
//==============================================================================

//==============================================================================
// RequiredParameterSupport
//
// Description:
//  Check all codec nodes for required parameter support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE if all required parameters are supported, FALSE if not (or error)
//
// History:
//  12/11/03 - jwexler - created
//==============================================================================
DWORD RequiredParameterSupport()
{    
    BOOL fRes = ((CCodec*)(g_pUAATest->m_pCurDevice))->RequiredParameterSupport();
    return (fRes ? FNS_PASS : FNS_FAIL);
} // RequiredParameterSupport

//==============================================================================
// RequiredVerbSupport
//
// Description:
//  Check all codec nodes for required Verb support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE if all required Verbs are supported, FALSE if not (or error)
//
// History:
//  12/15/03 - jwexler - created
//==============================================================================
DWORD RequiredVerbSupport()
{
    BOOL fRes = ((CCodec*)(g_pUAATest->m_pCurDevice))->RequiredVerbSupport();
    return (fRes ? FNS_PASS : FNS_FAIL);
} // RequiredVerbSupport


//==============================================================================
// FGReset
//
// Description:
//  Check all function groups for proper RESET support
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE if RESET is supported correctly, FALSE if not (or error)
//
// History:
//  11/10/04 - jwexler - created
//==============================================================================
DWORD FGReset()
{
    BOOL fRes = ((CCodec*)(g_pUAATest->m_pCurDevice))->FGReset();
    return (fRes ? FNS_PASS : FNS_FAIL);
} // FGReset


//==============================================================================
// ValidatePinConfigs
//
// Description:
//  Check a codec for compliance with our pin configuration requirements
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE if the codec's pin configurations are compatible with 
//          our requirements, else FALSE
//
// History:
//  02/24/06 - jwexler - created
//==============================================================================
DWORD ValidatePinConfigs()
{
    BOOL fRes = ((CCodec*)(g_pUAATest->m_pCurDevice))->ValidatePinConfigs();
    return (fRes ? FNS_PASS : FNS_FAIL);
} // ValidatePinConfigs

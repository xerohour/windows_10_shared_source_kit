/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#include <windows.h>
#include <tchar.h>
#include <mmsystem.h>
#include <shell98.h>
#include <module98.h>
#include <dvmodule.h>
#include <cfgmgr32.h>
#include <setupapi.h>
#include <atlstr.h>
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
#include "azverbutils.h"
#include "log.h"

//#define POWER_TIMEOUT 10
//#define D3toD0_POWER_TIMEOUT 1000

enum DevicePowerState
{
    D0 = 0,
    D1 = 1,
    D2 = 2,
    D3Hot = 3,
    D3Cold = 4
};

//==============================================================================
// PowerTransitionTimeout
//
// Description:
//  Returns requirements from table lookup for the maximum length of time a codec
//  has to make a power transition, in milliseconds
//
// Arguments:
//  from    - Original power state
//  to      - New power state
//  epss    - Requirements are different before / after low-power DCN
//
// Returns:
//  Maximum timeout in milliseconds if transition has a maximum timeout set by HD
//   Audio spec, or by logo requirements
//  Arbitrarily high number if transition time is not defined.
//
//==============================================================================
static DWORD PowerTransitionTimeout(DevicePowerState from, DevicePowerState to, bool epss)
{
    // powering down
    if (D0 == from) {
        switch (to) {
            case D0:        return 0;       // No transition
            case D1:                        // fallthrough to D2
            case D2:        return 100;     // Per MS Power Mgmt Requirements
            case D3Hot:                     // No stated requirement.  Fallthrough (assume no worse than D3Cold)
            case D3Cold:    return epss ?
                                   200 :    // Per HD Audio Low-power DCN
                                   10000;   // No requirement.  Use 10 seconds
        }
    }
    // powering up
    if (D0 == to) {
        switch (from) {
            case D1:        return epss ? 1  : 10;      // 1 per DCN, 100 per MS requirements
            case D2:        return epss ? 2  : 100;     // 2 per DCN, 100 per MS requirements
            case D3Hot:     return epss ? 10 : 10000;   // 10 per DCN, no MS requirements
            case D3Cold:    return 200;                 // 200 per DCN.  Should never be going to D3Cold without EPSS
        }
    }

    // Transitions not involving D0 are not defined, so let's just say 20 seconds.
    return 20000;
}

//==============================================================================
// CPowerControl::Validate
//
// Description:
//  Verify that the node properly supports the PowerState control
//
// Arguments:
//  CAzNode* pNode - pointer to the node to validate
//  eNodeType nodetype - we need to have conditional logic based on nodetype
//  BOOL fRequired - TRUE if this node must support Get/Set PowerState
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  04/23/04 - jwexler - created
//  07/05/06 - jwexler - migrated into separate CPowerControl class
//==============================================================================
BOOL 
CPowerControl::Validate(CAzNode* pNode, eNodeType nodetype, BOOL fRequired)
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;
    AzWidgetPowerState azwPowerState;
    CAudioWidget* pAW = NULL;
    CFunctionGroup* pFG = NULL;

    SLOG(eInfo2, " Verifying FnPwrState");
    m_pNode = pNode;
    m_nodetype = nodetype;
    m_fRequired = fRequired;

    // EPSS bit determines digital keepalive capabilities, and which spec to use for power transition timeout.
    bool epss = false;

    switch(nodetype)
    {
        case eNodeType_AudioWidget:        
            SLOG(eInfo2, "  Checking Audio Widget's PowerState support.");
            // check if widget even claims to support PowerManagement
            pAW = dynamic_cast<CAudioWidget*>(pNode);
            pFG = pAW->m_parentFG;
            if (NULL == pFG)
            {
                SLOG(eError, "Error: Audio Widget at node %u has no parent function group!", m_pNode->m_usNode);
                fRes = FALSE;
                goto DONE;
            }
            m_SupPwrStates = pAW->m_SupPwrStates;
            if (!(AzAudioWidgetCaps(pAW->m_AudWidgetCaps.Response)).PowerCntrl)
                return TRUE;

            if (!m_SupPwrStates.Valid)
            {
                SLOG(eError, "Error: Audio Widget at node %u claims support for PowerManagement in its capabilities, "
                    "but does not support required parameter SupPwrStates!", m_pNode->m_usNode);
                fRes = FALSE;
            }

            epss =
                (
                    // the widget can opt-in
                    AzCodecPowerStateSupported(m_SupPwrStates.Response).EPSS ||

                    // or the widget can be part of a function group that opts-in
                    (
                        AzCodecPowerStateSupported(pFG->m_SupPwrStates.Response).EPSS
                    )
                );
        break;
        
        case eNodeType_FunctionGroup:
            pFG = dynamic_cast<CFunctionGroup*>(m_pNode);
            m_SupPwrStates = pFG->m_SupPwrStates;
            epss = AzCodecPowerStateSupported(m_SupPwrStates.Response).EPSS;
        break;

        default:
            SLOG(eError, "Error: Unsupported node type in CPowerControl!");
            fRes = FALSE;
            goto DONE;
    }

    // explicitly set the codec into D0 before we start
    azwPowerState.Act = 0; // reserved on "set" verbs
    azwPowerState.Set = 0;
    Verb = MakeVerb(m_pNode->m_pCodec->m_usSDI, m_pNode->m_usNode, SetFnPwrState, azwPowerState, 0);
    azrResponse = SendVerb(m_pNode->m_pCodec->m_hDriver, Verb);
    SLOG(eInfo3, "  First SetFnPwrState(D0) on node %u: Response == 0x%016I64X", m_pNode->m_usNode, azrResponse);
    if (!azrResponse.Valid)
    {
        SLOG(eError, "Error: initial SetFnPwrState(D0) response on node %u was invalid.", 
            m_pNode->m_usNode);
        fRes = FALSE;
        goto DONE;
    }

    // give it... oh... let's say... 10 seconds to come up into D0
    fRes = Validate_Get(10000);
    if (!fRes)
    {
        SLOG(eError, "Error: Could not set node %u to D0 initially.", 
            m_pNode->m_usNode);
        fRes = FALSE;
        goto DONE;
    }

    // BUGBUG - Verify that Act is the lesser of Set or the Function Group's Set value
    // BUGBUG - Verify that we can't set a power state higher than its function group node
    // NOTE: We currently are assuming that the FG is in D0 when testing audio widgets

    // BUGBUG - Do we require certain power states to absolutely be supported??

    // check for each supported power state
    for (DevicePowerState d = D1; d <= D3Cold; d = (DevicePowerState)(d + 1))
    {
        if ((m_SupPwrStates.Response >> d) & 0x1)
        {
            // sanity check - D3 Cold should only be advertised on epss codecs
            if (D3Cold == d && !epss)
            {
                SLOG(eError, "Error: Node %u claims support for D3 Cold but EPSS support is not advertised",
                    m_pNode->m_usNode);
                fRes = FALSE;
                continue;
            }

            // D3 Cold behavior is specified only on function groups, not on widgets
            if (D3Cold == d && eNodeType_FunctionGroup != nodetype)
            {
                XLOG(XWARN, eWarn1, "Warning: Node %u claims support for D3 Cold but it is not a function group",
                    m_pNode->m_usNode);
                continue;
            }
        
            // set to power state d
            azwPowerState.Act = 0; // reserved on "set" verbs
            azwPowerState.Set = d;
            Verb = MakeVerb(m_pNode->m_pCodec->m_usSDI, m_pNode->m_usNode, SetFnPwrState, azwPowerState, 0);
            azrResponse = SendVerb(m_pNode->m_pCodec->m_hDriver, Verb);
            SLOG(eInfo3, "  SetFnPwrState(D%u): Response == 0x%016I64X", d, azrResponse);
            if (!azrResponse.Valid)
            {
                SLOG(eError, "Error: Node %u claims support for D%u but fails SetFnPwrState for it.", 
                    m_pNode->m_usNode, d);
                fRes = FALSE;
            }

            // get
            fRes = Validate_Get(PowerTransitionTimeout(D0, d, epss)) ? fRes : FALSE;
            if (m_PowerState.Act != d)
            {
                SLOG(eError, "Error: Node %u did not set power to the requested D%u state.", 
                    m_pNode->m_usNode, d);
                fRes = FALSE;
            }

            // Bringing a codec out of D3 Cold requires a double-function-group reset
            // per the Intel HD Audio Low-Power ECR
            if (D3Cold == d)
            {
                // give it 200 ms to finish up going into D3 Cold
                g_pUAATest->SleepAtLeast(PowerTransitionTimeout(D0, D3Cold, true));

                SLOG(eInfo1, "Doing a double-function-group reset to get the function group out of D3 Cold.");

                ULONG ulCPSize = sizeof(UserModeCodecCommandPacket) + sizeof(AzCorbEntry) * 2;
                ULONG ulRPSize = sizeof(UserModeCodecResponsePacket) + sizeof(AzRirbEntry) * 2;
                CBuffer<BYTE> pCPacketBuffer(ulCPSize);
                CBuffer<BYTE> pRPacketBuffer(ulRPSize);
                UserModeCodecCommandPacket* pCPacket = (UserModeCodecCommandPacket*)(BYTE*)pCPacketBuffer;
                UserModeCodecResponsePacket* pRPacket = (UserModeCodecResponsePacket*)(BYTE*)pRPacketBuffer;
                DWORD dwBytesReturned = 0;

                ZeroMemory(pCPacket, ulCPSize);
                ZeroMemory(pRPacket, ulRPSize);
                pCPacket->NumCommands = 2;

                pCPacket->Command[0] = MakeVerb(m_pNode->m_pCodec->m_usSDI, m_pNode->m_usNode, SetFuncReset, 0, 0);
                pCPacket->Command[1] = MakeVerb(m_pNode->m_pCodec->m_usSDI, m_pNode->m_usNode, SetFuncReset, 0, 0);

                if (
                    !DeviceIoControl(m_pNode->m_pCodec->m_hDriver, IOCTL_AZALIABUS_SENDVERBS, 
                       pCPacket, ulCPSize,
                       pRPacket, ulRPSize, 
                       &dwBytesReturned, 0
                    )
                )
                {
                    fRes = FALSE;
                    SLOG(eError, "Error: Double-function-group reset failed. GetLastError() = %u", GetLastError());
                    goto DONE;
                }

                if (2 != pRPacket->NumResponses)
                {
                    SLOG(eError, "Error: Expected 2 responses, got %u.", pRPacket->NumResponses);
                    fRes = FALSE;
                    goto DONE;
                }

                SLOG(eInfo1,
                    "Responses to double-function-group reset: 0x%016I64X, 0x%016I64X",
                    pRPacket->Response[0],
                    pRPacket->Response[1]
                );
                
                if (!pRPacket->Response[0].Valid && !pRPacket->Response[1].Valid)
                {
                    SLOG(eError, "Error: at least one of the responses to the double-function-group reset on node %u must be valid.", m_pNode->m_usNode);
                    fRes = FALSE;
                    // soldier on anyway
                }

                SLOG(eInfo1, "Double-function-group reset complete.");

                // give it 200 ms to enter a controllable state again
                g_pUAATest->SleepAtLeast(PowerTransitionTimeout(D3Cold, D0, true));

                // not done yet; after a double-function-group reset the codec can now be controlled
                // but it's still not guaranteed to be in D0
                // so put it in D0

                // fall-through to normal (non-D3 Cold) recovery
            }

            // can get back to D0 by a simple SetFnPwrState verb
            // verify that subsystem ID has not changed
            if (eNodeType_FunctionGroup == nodetype)
            {
                fRes = CheckSubSys() ? fRes : FALSE;
            }

            // return power to D0 and verify
            azwPowerState.Act = 0; // reserved on "set" verbs
            azwPowerState.Set = D0;
            Verb = MakeVerb(m_pNode->m_pCodec->m_usSDI, m_pNode->m_usNode, SetFnPwrState, azwPowerState, 0);
            azrResponse = SendVerb(m_pNode->m_pCodec->m_hDriver, Verb);

            SLOG(eInfo3, "  SetFnPwrState(original D0): Response == 0x%016I64X", azrResponse);

            // see if the attempt to bring the codec back to D0 was successful
            fRes = Validate_Get(PowerTransitionTimeout(d, D0, epss)) ? fRes : FALSE;    
            if (D0 != m_PowerState.Act)
            {
                SLOG(eError, "Error: Node %u was not able to restore power to D0.", m_pNode->m_usNode);
                fRes = FALSE;
            }
            
            // verify that subsystem ID has not changed
            if (eNodeType_FunctionGroup == nodetype)
            {
                fRes = CheckSubSys() ? fRes : FALSE;
            }

        } // if power state supported
    } // for each power state


    // If widget is a digital output
    // and function group supports EPSS
    // then verify digital keepalive is R/W
    if (
        epss &&
        eNodeType_AudioWidget == nodetype && 
        AzAudNodeOutput == AzAudioWidgetCaps(pAW->m_AudWidgetCaps.Response).Type &&
        AzAudioWidgetCaps(pAW->m_AudWidgetCaps.Response).Digital
    )
    {
        SLOG(eInfo1, "This is a digital output that supports EPSS; testing Keepalive.");
        
        AzSetDigCnvControl3 azwSetDigCnvControl3;
        AzGetDigCnvControlResponse azwGetDigCnvControl;

        // Get current keepalive
        Verb = MakeVerb(m_pNode->m_pCodec->m_usSDI, m_pNode->m_usNode, GetDigitalConvControl, 0, 0);
        azrResponse = SendVerb(m_pNode->m_pCodec->m_hDriver, Verb);
        if (!azrResponse.Valid)
        {
            SLOG(eError,
                "Error: Unable to read keepalive value on digital output node %u "
                "even though support for EPSS is claimed.",
                m_pNode->m_usNode
            );
            fRes = FALSE;
        }
        UCHAR originalKeepaliveValue = AzGetDigCnvControlResponse(azrResponse.Response).KeepAlive;

        // Change keepalive value
        azwSetDigCnvControl3.KeepAlive = !originalKeepaliveValue;
        Verb = MakeVerb(m_pNode->m_pCodec->m_usSDI, m_pNode->m_usNode, SetDigitalConvControl3, azwSetDigCnvControl3, 0);
        azrResponse = SendVerb(m_pNode->m_pCodec->m_hDriver, Verb);
        if (!azrResponse.Valid)
        {
            SLOG(eError,
                "Error: Unable to change keepalive value on digital output node %u to %u "
                "even though support for EPSS is claimed.",
                m_pNode->m_usNode, azwSetDigCnvControl3.KeepAlive
            );
            fRes = FALSE;
        }

        // Get keepalive bit
        Verb = MakeVerb(m_pNode->m_pCodec->m_usSDI, m_pNode->m_usNode, GetDigitalConvControl, 0, 0);
        azrResponse = SendVerb(m_pNode->m_pCodec->m_hDriver, Verb);
        if (!azrResponse.Valid || (originalKeepaliveValue == AzGetDigCnvControlResponse(azrResponse.Response).KeepAlive))
        {
            SLOG(eError,
                "Error: Unable to %s keepalive on digital output node %u "
                "even though support for EPSS is claimed.",
                originalKeepaliveValue ? "clear" : "set",
                m_pNode->m_usNode
            );
            fRes = FALSE;
        }

        // Restore previous value
        azwSetDigCnvControl3.KeepAlive = originalKeepaliveValue;
        Verb = MakeVerb(m_pNode->m_pCodec->m_usSDI, m_pNode->m_usNode, SetDigitalConvControl3, azwSetDigCnvControl3, 0);
        azrResponse = SendVerb(m_pNode->m_pCodec->m_hDriver, Verb);
        if (!azrResponse.Valid)
        {
            SLOG(eError,
                "Error: Unable to restore keepalive value on digital output node %u to %u "
                "even though support for EPSS is claimed.",
                m_pNode->m_usNode, azwSetDigCnvControl3.KeepAlive
            );
            fRes = FALSE;
        }        

        // Get keepalive bit
        Verb = MakeVerb(m_pNode->m_pCodec->m_usSDI, m_pNode->m_usNode, GetDigitalConvControl, 0, 0);
        azrResponse = SendVerb(m_pNode->m_pCodec->m_hDriver, Verb);
        if (!azrResponse.Valid || (originalKeepaliveValue != AzGetDigCnvControlResponse(azrResponse.Response).KeepAlive))
        {
            SLOG(eError,
                "Error: Unable to %s keepalive on digital output node %u "
                "even though support for EPSS is claimed.",
                originalKeepaliveValue ? "set" : "clear",
                m_pNode->m_usNode
            );
            fRes = FALSE;
        }
    }

DONE:
    return fRes;
} // CPowerControl::Validate

//==============================================================================
// CPowerControl::Validate_Get
//
// Description:
//  Verify that the node properly supports the PowerState(Get) control
//
// Arguments:
//  DWORD dwTimeout - timeout threshold for power state transitions
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  11/19/04 - jwexler - created
//  07/05/06 - jwexler - migrated into separate CPowerControl class
//==============================================================================
BOOL 
CPowerControl::Validate_Get(DWORD dwTimeout)
{
    BOOL fRes = TRUE;
    AzRirbEntry azrResponse;
    AzCorbEntry Verb;

    Verb = MakeVerb(m_pNode->m_pCodec->m_usSDI, m_pNode->m_usNode, GetFnPwrState, 0, 0);
    azrResponse = SendVerb(m_pNode->m_pCodec->m_hDriver, Verb);    
    SLOG(eInfo3, "  GetFnPwrState: Response == 0x%016I64X", azrResponse);
    if (!azrResponse.Valid && (m_fRequired || m_SupPwrStates.Valid))
    {
        SLOG(eError, "Error: Node %u does not support required control GetFnPwrState", m_pNode->m_usNode);
        fRes = FALSE;
        goto DONE;
    }
    m_PowerState = azrResponse.Response;

    if (m_PowerState.Set != m_PowerState.Act)
    {
        XLOG(XWARN, eWarn1, "Warning: The Set and Act power setting of the node are different! Set=%d Act=%d", m_PowerState.Set, m_PowerState.Act);
        XLOG(XWARN, eWarn1, "Allowing %ums for power change.", dwTimeout);

        g_pUAATest->SleepAtLeast(dwTimeout);

        Verb = MakeVerb(m_pNode->m_pCodec->m_usSDI, m_pNode->m_usNode, GetFnPwrState, 0, 0);
        azrResponse = SendVerb(m_pNode->m_pCodec->m_hDriver, Verb);
        SLOG(eInfo3, "  GetFnPwrState: Response == 0x%016I64X", azrResponse);
        m_PowerState = azrResponse.Response;

        if (m_PowerState.Set != m_PowerState.Act)
        {
            SLOG(eError, "Error: The Set and Act power setting of the Function Group are still different! Set=%d Act=%d", m_PowerState.Set, m_PowerState.Act);
            fRes = FALSE;
            goto DONE;
        }
    }   
    
DONE:
    return fRes;
} // CPowerControl::Validate_Get

//==============================================================================
// CPowerControl::CheckSubSys
//
// Description:
//  Verify that the subsystem ID is persisted across power states
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE on success, FALSE on failure
//
// History:
//  09/12/06 - jwexler - created
//==============================================================================
BOOL 
CPowerControl::CheckSubSys()
{
    BOOL fRes = TRUE;
    AzCorbEntry Verb;
    AzRirbEntry azrResponse;
    CFunctionGroup* pFG = NULL;

    SLOG(eInfo2, "  Verifying Subsystem ID has not changed across power states");
    pFG = dynamic_cast<CFunctionGroup*>(m_pNode);

    Verb = MakeVerb(m_pNode->m_pCodec->m_usSDI, m_pNode->m_usNode, GetSubsystemID, 0, 0);
    azrResponse = SendVerb(m_pNode->m_pCodec->m_hDriver, Verb);    
    SLOG(eInfo3, "    GetSubsystemID: Response == 0x%016I64X", azrResponse);    
    if (azrResponse.Response != pFG->m_SubSystemID.Response)
    {
        SLOG(eError, "Error: Subsystem ID changed across power states in FunctionGroup at node %u", m_pNode->m_usNode);
        SLOG(eError, "Error: Subsystem ID changed from 0x%016I64X to 0x%016I64X across power states in FunctionGroup at node %u", 
            pFG->m_SubSystemID.Response, azrResponse.Response, m_pNode->m_usNode);
        fRes = FALSE;
    }

    return fRes;
}  // CPowerControl::CheckSubSys


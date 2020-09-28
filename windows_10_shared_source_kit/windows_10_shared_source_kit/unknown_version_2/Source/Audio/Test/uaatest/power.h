/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#ifndef __POWER_H__
#define __POWER_H__

#ifndef __AZNODE_H__
#error power.h requires aznode.h
#endif

enum eNodeType 
{ 
  eNodeType_FunctionGroup   = 0x00,
  eNodeType_AudioWidget     = 0x01
};

class CPowerControl
{
public:
    BOOL Validate(CAzNode* pNode, eNodeType nodetype, BOOL fRequired);
private:
    BOOL Validate_Get(DWORD dwTimeout);
    BOOL CheckSubSys();
private:
    CAzNode* m_pNode;
    eNodeType m_nodetype;
    BOOL m_fRequired;
    AzRirbEntry m_SupPwrStates; // AzCodecPowerStateSupported
    AzWidgetPowerState m_PowerState; // AzWidgetPowerState
};

#endif // __POWER_H__
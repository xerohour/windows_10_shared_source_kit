/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#ifndef CODEC_H
#define CODEC_H

#ifndef __POWER_H__
#error codec.h requires power.h
#endif

#ifndef _TLIST_7750ebf3_e73c_41bd_bf7f_7fb16042e440
#error codec.h requires ttlist.h
#endif

#define NUMASSOCIATIONS 16
#define NUMGENLOCS 4
#define NUMGEOLOCS 15

// forwards
class CCodec;
class CPinComplex;
class CFunctionGroup;

//
// Helper classes
//
class CAssoc
{
public:
    CAssoc(USHORT usAssoc) : m_usAssoc(usAssoc), m_usNumPins(0) {};
    BOOL Add(CPinComplex* pPC);
    BOOL Validate();
private:
    BOOL ValidateMultiRender();
    BOOL ValidateMultiCapture();
public:
    USHORT m_usAssoc;
private:
    USHORT m_usNumPins;
    TTList<CPinComplex> m_listPC;
    // BUGBUG - not currently used
    USHORT m_rgSequences;  // Bit field, denoting which sequence numbers are being used
};

class CAssociations
{
public:    
    BOOL Add(CPinComplex* pPC);
    BOOL Validate();
private:
    TTList<CAssoc> m_listAssoc;
};

class CRootNode : public CAzNode
{
    friend class CCodec;
public:
    CRootNode(CCodec* pCodec) : 
      CAzNode(0, pCodec), 
      m_VenDevID(AzInvalidResponse),       
      m_RevisionID(AzInvalidResponse)
      {};
public:
    BOOL RequiredParameterSupport();
    BOOL RequiredVerbSupport();
public:
    AzRirbEntry m_RevisionID;
    AzRirbEntry m_VenDevID;
private:    
    AzRirbEntry m_SubNodeCount; // AzCodecNodeInfo
};

class CAudioWidget : public CAzNode
{
    friend class CPowerControl;
    friend class CAssoc;
public:
    CAudioWidget(USHORT usNode, CCodec* pCodec, CFunctionGroup* parentFG, AzRirbEntry azrAudWidgetCaps) : 
      CAzNode(usNode, pCodec), 
      m_AudWidgetCaps(azrAudWidgetCaps),
      m_parentFG(parentFG)
      {};
    CAudioWidget() {};  // should never get called - exists only to make CConnChecker happy
public:
    virtual BOOL Parse() = 0;
    virtual BOOL RequiredParameterSupport() = 0;
    virtual BOOL RequiredVerbSupport() = 0;
protected:    
    AzRirbEntry m_AudWidgetCaps; // AzAudioWidgetCaps
    AzRirbEntry m_SupPwrStates; // AzCodecPowerStateSupported
    CPowerControl m_PowerControl;
    CFunctionGroup* m_parentFG; // Parent function group
};

class CConnChecker : virtual public CAudioWidget
{
public:
    CConnChecker(BOOL fConnListReq, BOOL fConnSelReq) :
      m_fConnListReq(fConnListReq), 
      m_fConnSelReq(fConnSelReq), 
      m_ConnListLength(0) {};
protected:
    BOOL CheckConn();
protected:
    AzRirbEntry m_ConnListLength; // AzConnectionListLength
private:
    virtual BOOL CheckNode(USHORT usEntry);
private:
    BOOL m_fConnListReq;
    BOOL m_fConnSelReq;
};

class CAudioConverter : virtual public CAudioWidget
{
public:
    CAudioConverter(USHORT usNode, CCodec* pCodec, CFunctionGroup* parentFG, AzRirbEntry azrAudWidgetCaps) : 
      CAudioWidget(usNode, pCodec, parentFG, azrAudWidgetCaps) {};
protected:
    BOOL CheckStreamFormat();
    BOOL CheckCnvStrCh();
private:
    AzRirbEntry m_CnvBitsSR; // AzCnvBitsSR
    AzRirbEntry m_CnvStrCh; // AzCnvStreamChan
};

class CAudioOutputConverter : virtual private CAudioWidget, public CAudioConverter
{
    friend class CAudioFunctionGroup;
public:
    CAudioOutputConverter(USHORT usNode, CCodec* pCodec, CFunctionGroup* parentFG, AzRirbEntry azrAudWidgetCaps) : 
      CAudioWidget(usNode, pCodec, parentFG, azrAudWidgetCaps), 
      CAudioConverter(usNode, pCodec, parentFG, azrAudWidgetCaps) {};
public:
    BOOL Parse();
    BOOL RequiredParameterSupport();
    BOOL RequiredVerbSupport();
private:
    AzRirbEntry m_SupBitsAndSRs; // AzSupportedPcmBitSampleRates
    AzRirbEntry m_SupStrFormats; // AzSupportedStreamFormats
    AzRirbEntry m_OutAmpProps; // AzAmpProperties    
    AzRirbEntry m_ProcParam; // AzProcParams
};

class CAudioInputConverter : virtual private CAudioWidget, public CAudioConverter, public CConnChecker
{
    friend class CAudioFunctionGroup;
public:
    CAudioInputConverter(USHORT usNode, CCodec* pCodec, CFunctionGroup* parentFG, AzRirbEntry azrAudWidgetCaps) : 
      CAudioWidget(usNode, pCodec, parentFG, azrAudWidgetCaps), 
      CAudioConverter(usNode, pCodec, parentFG, azrAudWidgetCaps), 
      CConnChecker(TRUE, TRUE) 
      {};
public:
    BOOL Parse();
    BOOL RequiredParameterSupport();
    BOOL RequiredVerbSupport();
private:
    AzRirbEntry m_SupBitsAndSRs; // AzSupportedPcmBitSampleRates
    AzRirbEntry m_SupStrFormats; // AzSupportedStreamFormats
    AzRirbEntry m_InAmpProps; // AzAmpProperties    
    AzRirbEntry m_ProcParam; // AzProcParams
};

class CMixer : virtual public CAudioWidget, public CConnChecker
{
    friend class CAudioFunctionGroup;
public:
    CMixer(USHORT usNode, CCodec* pCodec, CFunctionGroup *parentFG, AzRirbEntry azrAudWidgetCaps) : 
      CAudioWidget(usNode, pCodec, parentFG, azrAudWidgetCaps), 
      CConnChecker(TRUE, FALSE) 
      {};
public:
    BOOL Parse();
    BOOL RequiredParameterSupport();
    BOOL RequiredVerbSupport();
private:
    AzRirbEntry m_InAmpProps; // AzAmpProperties
    AzRirbEntry m_OutAmpProps; // AzAmpProperties
};

class CInputSelector : virtual public CAudioWidget, public CConnChecker
{
    friend class CAudioFunctionGroup;
public:
    CInputSelector(USHORT usNode, CCodec* pCodec, CFunctionGroup *parentFG, AzRirbEntry azrAudWidgetCaps) : 
      CAudioWidget(usNode, pCodec, parentFG, azrAudWidgetCaps), 
      CConnChecker(TRUE, TRUE) 
      {};
public:
    BOOL Parse();
    BOOL RequiredParameterSupport();
    BOOL RequiredVerbSupport();
private:
    AzRirbEntry m_InAmpProps; // AzAmpProperties
    AzRirbEntry m_OutAmpProps; // AzAmpProperties
    AzRirbEntry m_ProcParam; // AzProcParams
};

struct PinControlTest
{
    BOOL  fDigital;
    UCHAR azpc;
    BOOL  fSupported;
    char* szTestCase;
};
class CPinComplex : virtual public CAudioWidget, public CConnChecker
{
    friend class CAudioFunctionGroup;
    friend class CAssociations;
    friend class CAssoc;
public:
    CPinComplex(USHORT usNode, CCodec* pCodec, CFunctionGroup *parentFG, AzRirbEntry azrAudWidgetCaps) : 
      CAudioWidget(usNode, pCodec, parentFG, azrAudWidgetCaps), 
      CConnChecker(FALSE, FALSE),
      m_PinCaps(0),
      m_InAmpProps(0),
      m_OutAmpProps(0),
      m_ProcParam(0),
      m_ConfigDef(0),
      m_PinControl(0)
      {};
public:
    BOOL Parse();
    BOOL RequiredParameterSupport();
    BOOL RequiredVerbSupport();
    BOOL ValidatePinConfig();
    BOOL ValidateHDMI(); // in hdmi.cpp
private:
    virtual BOOL CheckNode(USHORT usEntry);
    BOOL CheckPinControl();
    BOOL CheckConfigDef();
    BOOL CheckPinSense();
    BOOL PCTest(PinControlTest pincontroltest);
private:
    AzRirbEntry m_PinCaps; // AzPinCaps
    AzRirbEntry m_InAmpProps; // AzAmpProperties
    AzRirbEntry m_OutAmpProps; // AzAmpProperties
    AzRirbEntry m_ProcParam; // AzProcParams
    AzRirbEntry m_ConfigDef; // AzPinWidgetConfigDefault
    UCHAR m_PinControl; // could be AzPinControl or AzAnalogPinControl
};

class CPowerWidget : virtual public CAudioWidget, public CConnChecker
{
public:
    CPowerWidget(USHORT usNode, CCodec* pCodec, CFunctionGroup *parentFG, AzRirbEntry azrAudWidgetCaps) : 
      CAudioWidget(usNode, pCodec, parentFG, azrAudWidgetCaps), 
      CConnChecker(TRUE, FALSE) 
      {};
public:
    BOOL Parse();
    BOOL RequiredParameterSupport();
    BOOL RequiredVerbSupport();
};

class CVolumeKnob : virtual public CAudioWidget, public CConnChecker
{
public:
    CVolumeKnob(USHORT usNode, CCodec* pCodec, CFunctionGroup *parentFG, AzRirbEntry azrAudWidgetCaps) : 
      CAudioWidget(usNode, pCodec, parentFG, azrAudWidgetCaps), 
      CConnChecker(FALSE, FALSE) 
      {};
public:
    BOOL Parse();
    BOOL RequiredParameterSupport();
    BOOL RequiredVerbSupport();
private:
    AzRirbEntry m_VolKnobCaps; // AzVolKnobCapabilities
};

class CBeepGen : public CAudioWidget
{
public:
    CBeepGen(USHORT usNode, CCodec* pCodec, CFunctionGroup *parentFG, AzRirbEntry azrAudWidgetCaps) : 
      CAudioWidget(usNode, pCodec, parentFG, azrAudWidgetCaps) {};
public:
    BOOL Parse();
    BOOL RequiredParameterSupport();
    BOOL RequiredVerbSupport();
};

class CVendorWidget : public CAudioWidget
{
public:
    CVendorWidget(USHORT usNode, CCodec* pCodec, CFunctionGroup *parentFG, AzRirbEntry azrAudWidgetCaps) : 
      CAudioWidget(usNode, pCodec, parentFG, azrAudWidgetCaps) {};
public:
    BOOL Parse();
    BOOL RequiredParameterSupport();
    BOOL RequiredVerbSupport();
};

class CFunctionGroup : public CAzNode
{
    friend class CCodec;
    friend class CPowerControl;
public:
    CFunctionGroup(USHORT usNode, CCodec* pCodec, AzRirbEntry azrNodeType) : 
      CAzNode(usNode, pCodec), 
      m_SubSystemID(AzInvalidResponse), 
      m_SubNodeCount(AzInvalidResponse), 
      m_NodeType(azrNodeType)
      {};
    virtual BOOL Parse() = 0;
    virtual BOOL RequiredParameterSupport() = 0;
    virtual BOOL RequiredVerbSupport() = 0;
    BOOL CheckSubsystemID();
    BOOL CheckGPIO();
    BOOL FGReset();
public:
    AzRirbEntry m_NodeType; // AzAudioFuncGroupType
protected:
    AzRirbEntry m_SubSystemID;
    AzRirbEntry m_SubNodeCount; // AzCodecNodeInfo    
    AzRirbEntry m_SupPwrStates; // AzCodecPowerStateSupported    
    AzRirbEntry m_NumGPIOs; // AzNumGPIO
    CPowerControl m_PowerControl;
};

class CAudioFunctionGroup : public CFunctionGroup
{
    friend class CPinComplex;

public:
    CAudioFunctionGroup(USHORT usNode, CCodec* pCodec, AzRirbEntry azrNodeType) : 
      CFunctionGroup(usNode, pCodec, azrNodeType),
      m_SupportsHDMIPerAudio0078(false) {};
    BOOL Parse();
    BOOL ProbeHDMIPerAudio0078();
    BOOL RequiredParameterSupport();
    BOOL RequiredVerbSupport();
    BOOL ValidatePinConfigs();
    BOOL ValidateHDMI(); // in hdmi.cpp
private:
    BOOL RPS_SupBitsAndSRs();
    BOOL RPS_SupStrFormats();
    BOOL RPS_InAmpProps();
    BOOL RPS_OutAmpProps();
private:
    AzRirbEntry m_AudFnGrpCaps; // AzAudioFunctionParameters
    AzRirbEntry m_SupBitsAndSRs; // AzSupportedPcmBitSampleRates
    AzRirbEntry m_SupStrFormats; // AzSupportedStreamFormats
    AzRirbEntry m_InAmpProps; // AzAmpProperties
    AzRirbEntry m_OutAmpProps; // AzAmpProperties
    AzRirbEntry m_VolKnobCaps; // AzVolKnobCapabilities
private:
    TTList<CAudioWidget> listWidgets;
    TTList<CAudioOutputConverter> listAOC;
    TTList<CAudioInputConverter> listAIC;
    TTList<CMixer> listMix;
    TTList<CInputSelector> listIS;
    TTList<CPinComplex> listPC;
    TTList<CPowerWidget> listPW;
    TTList<CVolumeKnob> listVK;
    TTList<CBeepGen> listBG;
    TTList<CVendorWidget> listVW;
private:
    CAssociations m_Assoc;
    bool m_SupportsHDMIPerAudio0078;
};

class CModemFunctionGroup : public CFunctionGroup
{
public:
    CModemFunctionGroup(USHORT usNode, CCodec* pCodec, AzRirbEntry azrNodeType) : 
      CFunctionGroup(usNode, pCodec, azrNodeType) {};
    BOOL Parse();
    BOOL RequiredParameterSupport();
    BOOL RequiredVerbSupport();
private:    
    AzRirbEntry m_VolKnobCaps; // AzVolKnobCapabilities
};

class CVendorFunctionGroup : public CFunctionGroup
{
public:
    CVendorFunctionGroup(USHORT usNode, CCodec* pCodec, AzRirbEntry azrNodeType) : 
      CFunctionGroup(usNode, pCodec, azrNodeType) {};
    BOOL Parse();
    BOOL RequiredParameterSupport();
    BOOL RequiredVerbSupport();
};

class CCodec : public CDeviceDescriptor
{    
public:
    CCodec(LPCSTR szDevicePath, DWORD dwControllerDevInst, USHORT usSDI, HANDLE hFilter) :
        m_dwControllerDevInst(dwControllerDevInst), m_usSDI(usSDI), m_rootnode(NULL), m_hDriver(hFilter)
    {
        strncpy(m_szDevicePath, szDevicePath, MAX_PATH);
        ZeroMemory(m_rgSequences, sizeof(USHORT)*NUMASSOCIATIONS);
        ZeroMemory(m_rgusColors, sizeof(USHORT)*NUMGENLOCS*NUMGEOLOCS);
    };
    BOOL Parse();
    BOOL RequiredParameterSupport();
    BOOL RequiredVerbSupport();
    BOOL FGReset();
    BOOL ValidatePinConfigs();
    BOOL ValidateHDMI(); // in hdmi.cpp
public:
    HANDLE m_hDriver;
    DWORD  m_dwControllerDevInst;
    USHORT m_usSDI;
    CRootNode m_rootnode;
// BUGBUG - the below really belongs in CAudioFunctionGroup or CAssoc::ValidatePinConfig
// also, audio widgets might not need m_pCodec and should maybe just have an m_pAFG instead??
public:
    USHORT m_rgusColors[NUMGENLOCS][NUMGEOLOCS]; // Bit field, denoting which colors have been used for jacks
    USHORT m_rgSequences[NUMASSOCIATIONS];  // Bit fields, denoting which sequence numbers have 
                                            // been used for a particular Association group.
private:
    CHAR m_szDevicePath[MAX_PATH];
    TTList<CFunctionGroup> listFG;
};


//==============================================================================
//                              Test cases
//==============================================================================
DWORD RequiredParameterSupport();
DWORD RequiredVerbSupport();
DWORD FGReset();
DWORD ValidatePinConfigs();
DWORD ValidateHDMI(); // in hdmi.cpp

#endif // CODEC_H

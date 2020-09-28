// Copyright(C) Microsoft. All rights reserved.

#pragma once
#pragma warning(disable : 4995)


DWORD Tc_APOMatrixCreate();
DWORD Tc_APOMatrixInitialize();
DWORD Tc_APOMatrixSetMatrixCoeffns();
DWORD Tc_APOMatrixGetMatrixCoeffns();
DWORD Tc_APOMatrixProcess();
#ifdef TEST_LEAP_MATRIX
DWORD Tc_LEAPMatrixProcess();
#endif

class CCheck
{
public:
    CCheck(){};
    virtual ~CCheck() {};
    static BOOL InRange(double Value1, double Value2, double LowerBound, double UpperBound)
    {
        _ASSERT(LowerBound <= UpperBound);
        Value1 = fabs(Value1);
        Value2 = fabs(Value2);
        return (Value1 >= (LowerBound * Value2)) && (Value1 <= (UpperBound * Value2));
    };
    static BOOL InRange(double Value1, double Value2, double Tolerance)
    {       
        Value1 = fabs(Value1);
        Value2 = fabs(Value2);
        return (Value1 >= (Value2 - Tolerance)) && (Value1 <= (Value2 + Tolerance));
    }; 
};


class CAPOMatrixTest : public CComBase
{
public:
    CAPOMatrixTest()
    {
        HRESULT hr = S_OK;
        SLOG(eInfo1, "Creating Matrix Object");
        hr = m_pMatrix.CreateInstance(__uuidof(AudioMatrix));
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating Matrix Object, 0x%X", hr);
            return;
        }
        SLOG(eInfo1, "Creating APO Object");
        hr = m_pMatrix.QueryInterface(__uuidof(IAudioProcessingObjectConfiguration), &m_pBaseAPO);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating APO Object, 0x%X", hr);
        }

        m_pAPO = NULL;
        SLOG(eInfo1, "Creating APO Object");
        hr = m_pMatrix.QueryInterface(__uuidof(IAudioProcessingObject), &m_pAPO);
        if(FAILED(hr))
        {
            SLOG(eError, "ERROR: Creating APO Object, 0x%X", hr);            
        }

		// common fields in the apo init
		apoParams.APOInit.cbSize = sizeof( apoParams );
		apoParams.APOInit.clsid  = __uuidof( AudioMatrix );

    };
    virtual ~CAPOMatrixTest()
    {    
        m_pBaseAPO = NULL;
        m_pMatrix = NULL;
    };     
    
    // Basic Object creation
    DWORD APOMatrixCreate();
    // IAudioVolume methods 
    DWORD APOSetMatrixCoeffns();
    DWORD APOGetMatrixCoeffns();
    DWORD APOInitialize(); 
    HRESULT Initialize();
    DWORD APOTestMatrix();
#ifdef TEST_LEAP_MATRIX
    DWORD LEAPTestMatrix();
#endif
    
private:    
    IAudioMatrixPtr                         m_pMatrix;
    IAudioProcessingObjectConfigurationPtr  m_pBaseAPO;
    IAudioProcessingObjectPtr				m_pAPO;

	APO_MATRIX_PARAMS						apoParams;
  
	HRESULT CreateObjects();

	HRESULT LockAPO( UINT32 u32InChannels = 1
                     ,UINT32 u32OutChannels = 2
                     ,FLOAT32 f32FrameRate = 48000.00
                     ,FLOAT32* pfMatrixCoeffns = NULL
                     ,MFTIME* pMftime = NULL
                     ,AUDIO_CURVE_TYPE = AUDIO_CURVE_TYPE_NONE
                     ,MFTIME* pCurveDuration = NULL
                     ,PLOCK_PARAMS pLockParams = NULL
                     );

	HRESULT TestFunctional( UINT32 u32InChannels
                            ,UINT32 u32OutChannels                            
                            ,FLOAT32* pfMatrixCoeffns
                            ,UINT32 u32NumInConn
                            ,APO_CONNECTION_PROPERTY* pInConn
                            ,UINT32 u32NumOutConn
                            ,APO_CONNECTION_PROPERTY* pOutConn
                            );
};

#pragma once

#include "DXGIPresent.h"

typedef HRESULT (WINAPI *PDWMENABLECOMPOSITION)  
(  
	UINT CompositionAction  
); 

//////////////////////////////////////////////////////////////////////////
// CMyTestApp - An instance of a single object derived from CWGFTestFramework
// is required by the framework.
//////////////////////////////////////////////////////////////////////////
class CMyTestApp
    :
    public CDXGITestFramework
{
    //
    // Construction/destruction
    //

public:
    CMyTestApp():
      m_saveBMP( false ),
          m_saveDDS( false ),
          m_debugPause( 0 ),
          m_rtl( false ),
          m_noLowResolution( false ),
          m_CapMultiSampleQuality( false ),
          m_cDisplayDevices( 0 ),
          m_bDFlip(false),
          m_pDWMEnableComposition( NULL ), 
          m_hDWMAPI( NULL ) 
      {
      };

      virtual ~CMyTestApp()
      {
          if( m_hDWMAPI ) 
          {
              FreeLibrary( m_hDWMAPI );
              m_hDWMAPI = NULL;
          }
          m_pDWMEnableComposition = NULL;
      };

      bool m_bDFlip;


      //
      // Framework override methods
      //

public:
    virtual TESTTYPE GetTestType() const
    {
        return TESTTYPE_CONF;
    };

    virtual bool InitTestGroups();

    virtual void InitOptionVariables();

    virtual bool Setup();
    virtual void Cleanup();


    //
    // Public methods
    //

public:
    bool IsSaveBMP(){return m_saveBMP;}
    bool IsSaveDDS(){return m_saveDDS;}
    UINT IsCapMultiSampleQuality(){return m_CapMultiSampleQuality;}
    bool IsMinResolutionSet(){return m_MinResolutionSet;}
    bool IsMinMultiSampleSet(){return m_MinMultiSampleSet;}
    UINT GetDebugPause(){return m_debugPause;}
    bool IsRTL(){return m_rtl;}
    bool IsNoLowResolution(){return m_noLowResolution;}
    bool IsUseDesktopResolution(){return m_useDesktopResolution;}
	bool IsAdapterOfType( IDXGIAdapter1* pAdapter, UINT AdapterType )
	{ 
		return CGraphicsTestFramework::IsAdapterOfType( pAdapter, AdapterType);
	}
	
	HRESULT DwmEnableComposition( UINT uCompositionAction )
	{
		HRESULT hr = E_FAIL;
		if( !m_hDWMAPI )
		{
			m_hDWMAPI = LoadLibraryExW
			(
				L"DWMAPI.dll",
				NULL,
				0
			);
		}

		if( m_hDWMAPI && !m_pDWMEnableComposition )
		{
			OSVERSIONINFO versionInfo;  
			versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);  
			GetVersionEx(&versionInfo);  
			if(versionInfo.dwMajorVersion < 6 || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion < 2) )
			{
				m_pDWMEnableComposition = (PDWMENABLECOMPOSITION) GetProcAddress  
				(  
					m_hDWMAPI,  
					"DwmEnableComposition"  							
				);  
			}
			else
			{
				m_pDWMEnableComposition = (PDWMENABLECOMPOSITION) GetProcAddress  
				(  
					m_hDWMAPI,  
					"DwmEnableComposition"  							
				);  
			}
		}

		if( m_pDWMEnableComposition )
			hr = (*m_pDWMEnableComposition)( uCompositionAction );
		
		if ( FAILED( hr ) )  
		{  
			if ( m_pDWMEnableComposition )
				WriteToLogF
				(  
					_T( "DwmEnableComposition failed with error 0x%x." ),  
					hr  
				);  
			else
				WriteToLog(_T( "Unable to get DwmEnableComposition entry point" ));  
		};

		return hr;
	}


    //
    // Member data
    //

private:
    bool m_saveBMP;
    bool m_saveDDS;
    UINT m_debugPause;
    bool m_rtl;
    bool m_noLowResolution;
    bool m_useDesktopResolution;
    UINT m_CapMultiSampleQuality;
    bool m_MinResolutionSet;
    bool m_MinMultiSampleSet;
    DWORD m_cDisplayDevices;
    DISPLAY_DEVICE m_DisplayDevices[ 10 ];
    DEVMODE m_DisplayDeviceModes[ 10 ];
	PDWMENABLECOMPOSITION m_pDWMEnableComposition;
	HMODULE m_hDWMAPI;
};


extern CMyTestApp g_Application;

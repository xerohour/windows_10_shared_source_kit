//////////////////////////////////////////////////////////////////////////
//  SharedResources.h
//  created:	2007/02/05
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include <dxgi.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <d3d10_1.h>
#include <d3d11_4.h>

#include <stdio.h>
#include <tchar.h>

#include <D3D11Test.h>
#include <WGFTestCore.h>

#include <strsafe.h>
#include <DXGIFloatTypes.h>
#include <string>

#include "d3d11partner.h" // Internal interfaces for CompositionBuffers
#include "d3d11videocapture.h"
#include "d3d11internaltest.h" // Internal interfaces for ID3D11DeviceTest

#define MAX_RES_WIDTH 256
#define MAX_RES_HEIGHT 480
#define MAX_DATA_BYTES (MAX_RES_WIDTH * MAX_RES_HEIGHT * 16) //16 is the max number of bytes per pixel
#define SR_ARRAY_SIZE 1
#define SR_RESULT_STRING_LENGTH 1024

#define HANDLE_NAME_VALID L"SharedResources"

//resource types to test
enum D3D_RESOURCE_TYPE
{
	D3D_RESOURCE_TYPE_BUFFER,
	D3D_RESOURCE_TYPE_TEXTURE1D,
	D3D_RESOURCE_TYPE_TEXTURE1D_ARRAY,
	D3D_RESOURCE_TYPE_TEXTURE2D,
	D3D_RESOURCE_TYPE_TEXTURE2D_ARRAY,
	D3D_RESOURCE_TYPE_TEXTURE2D_MS,
	D3D_RESOURCE_TYPE_TEXTURE2D_MS_ARRAY,
	D3D_RESOURCE_TYPE_TEXTURECUBE,
	D3D_RESOURCE_TYPE_TEXTURE3D,
};
DECLARE_NAMED_VALUES( D3D_RESOURCE_TYPE );

enum SR_WRITE_METHOD
{
	SR_COPY_RESOURCE,
	SR_INIT_DATA,
	SR_UPDATE_RESOURCE,
	SR_DRAWTO_RESOURCE
};
DECLARE_NAMED_VALUES( SR_WRITE_METHOD );

enum SR_SYNC_METHOD
{
	SR_NO_SYNC,
	SR_SYS_EVENT,
	SR_KEYED_MUTEX
};
DECLARE_NAMED_VALUES( SR_SYNC_METHOD );

enum SR_TEST_TYPE
{
	SR_SAME_DEVICE,
	SR_SAME_PROCESS,
	SR_OTHER_PROCESS
};
DECLARE_NAMED_VALUES( SR_TEST_TYPE );

//struct used for inter-proces communication
struct RESOURCE_CASE
{
	//to child process
	UINT			   Device; //type of device to open resource on
	D3D_FEATURE_LEVEL  DeviceFeatureLevel;
	D3D_FEATURE_LEVEL  SrcFeatureLevel;
	DXGI_FORMAT		   Format;
	HANDLE			   SharedHandle;
	BYTE			   TestData[MAX_DATA_BYTES]; 
	BYTE			   TestData2[MAX_DATA_BYTES]; 
	SR_SYNC_METHOD	   SyncMethod; // The method used to sync the accesses of the two processes to the shared resource	
	SR_WRITE_METHOD	   WriteMethod; // the method the parent process uses to access the shared resource
	SR_WRITE_METHOD	   ChildAccessMethod; // the method the child process uses to access the shared resource
	UINT			   Width;
	UINT			   Height;
	UINT			   ArraySize;
	UINT			   MipLevels;
	bool			   isNTHandle;
	bool			   isGDICompatible;
	bool			   isShaderResource;
	bool			   isRenderTarget;
	bool			   isCapture;
	bool			   isMiscGenMips;

	//to parent process
	HRESULT			   Result;
	CHAR			   ResultString[SR_RESULT_STRING_LENGTH];
};

// struct used for inter-process communication for shared tile pool test
struct TILE_POOL_SHARED_MEMORY
{
	HANDLE SharedHandle;
	D3D_FEATURE_LEVEL  FeatureLevel;
	D3D_DRIVER_TYPE    DriverType;
	bool               isNTHandle;
	SR_SYNC_METHOD     SyncMethod;

	//to parent process
	HRESULT			   Result;
	CHAR			   ResultString[SR_RESULT_STRING_LENGTH];
};

class SharedResourcesTest;

class DestObjects
{
public:
	DestObjects(HANDLE h = NULL) : m_SharedHandle(h), m_SharedHandleArray(NULL) {} 
	virtual ~DestObjects() {}
	virtual TEST_RESULT SetupTestCase(SharedResourcesTest*) = 0;
	virtual TEST_RESULT ExecuteResourceOpen(SharedResourcesTest*, HANDLE) = 0;
	virtual TEST_RESULT ExecuteResourceAccess(SharedResourcesTest*, BYTE*, BYTE*) = 0;
	virtual HRESULT		QIKeyedMutex(IDXGIKeyedMutex** ppDXGIKeyedMutex) = 0;

	// Functions only used by "SrcObjects", when available:
	virtual TEST_RESULT SetupResources(SharedResourcesTest*, BYTE*, SR_WRITE_METHOD) { return RESULT_BLOCKED; }
	virtual TEST_RESULT UpdateSharedResource(SharedResourcesTest *pTest, BYTE*, SR_WRITE_METHOD) { return RESULT_BLOCKED; }
	virtual TEST_RESULT ExecuteVerifyTestCase(SharedResourcesTest*, BYTE*, BYTE*) { return RESULT_BLOCKED; }

	HANDLE					m_SharedHandle;  //handle to shared resource
	HANDLE					*m_SharedHandleArray;  //handles to shared resources
};

typedef DestObjects SrcObjects;

class DestObjects9 :
	public DestObjects
{
public:
	explicit DestObjects9(SharedResourcesTest*);
	virtual ~DestObjects9();
	virtual TEST_RESULT SetupTestCase(SharedResourcesTest*);
	virtual TEST_RESULT ExecuteResourceOpen(SharedResourcesTest*, HANDLE);
	virtual TEST_RESULT ExecuteResourceAccess(SharedResourcesTest*, BYTE*, BYTE*);
	virtual HRESULT		QIKeyedMutex(IDXGIKeyedMutex** ppDXGIKeyedMutex)
	{
		// sync'ed shared resource is not supported by DX9ex
		return E_FAIL;
	}

	IDirect3DDevice9Ex		*m_pDevice;
	DXGI_FORMAT				m_DXGIFormat;
	D3DFORMAT				m_D3DFormat;
	UINT					m_Width;
	UINT					m_Height;
	IDirect3DTexture9		*m_pTexShared;
};

class SrcObjects9 :
	public DestObjects9
{
public:
	explicit SrcObjects9(SharedResourcesTest*);
	virtual ~SrcObjects9();
	virtual TEST_RESULT SetupResources(SharedResourcesTest*, BYTE*, SR_WRITE_METHOD);
	virtual TEST_RESULT UpdateSharedResource(SharedResourcesTest *pTest, BYTE*, SR_WRITE_METHOD);
	virtual TEST_RESULT ExecuteVerifyTestCase(SharedResourcesTest*, BYTE*, BYTE*);
	virtual HRESULT		QIKeyedMutex(IDXGIKeyedMutex** ppDXGIKeyedMutex)
	{
		// sync'ed shared resource is not supported by DX9ex
		return E_FAIL;
	}


	IDirect3DTexture9		*m_pSrcTex;
	IDirect3DTexture9		*m_pTex;
};

class DestObjects10 :
	public DestObjects
{
public:
	explicit DestObjects10(SharedResourcesTest*);
	explicit DestObjects10(SharedResourcesTest*, ID3D10Device*);
	virtual ~DestObjects10();
	virtual TEST_RESULT SetupTestCase(SharedResourcesTest*);
	virtual TEST_RESULT ExecuteResourceOpen(SharedResourcesTest*, HANDLE);
	virtual TEST_RESULT ExecuteResourceAccess(SharedResourcesTest*, BYTE*, BYTE*);
	virtual HRESULT		QIKeyedMutex(IDXGIKeyedMutex** ppDXGIKeyedMutex)
	{
		if ( !m_pTexShared )
			return E_FAIL;
		return m_pTexShared->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)ppDXGIKeyedMutex);
	}

	TEST_RESULT DrawToResource( SharedResourcesTest *pTest, UINT arrayIndex, UINT mipIndex, void *pData,  ID3D10Texture2D *pRTVTex);

	ID3D10Device			*m_pDevice;
	ID3D11Device1			*m_pDevice11_1;
	DXGI_FORMAT				m_DXGIFormat;
	UINT					m_Width;
	UINT					m_Height;
	UINT					m_ArraySize;
	UINT					m_MipLevels;
	ID3D10Texture2D 		*m_pTexShared;
	ID3D10VertexShader		*m_pVertexShader;
	ID3D10PixelShader		*m_pPixelShader;
	ID3D10InputLayout		*m_pInputLayout;
	ID3D10Buffer			*m_pVertexBuffer;
	ID3D10Texture2D			*m_pSRVTex;
	ID3D10ShaderResourceView	*m_pSRV;
	ID3D10RenderTargetView		*m_pRTV;
};

class SrcObjects10 :
	public DestObjects10
{
public:
	explicit SrcObjects10(SharedResourcesTest*);
	explicit SrcObjects10(SharedResourcesTest*, ID3D10Device*);
	virtual ~SrcObjects10();
	virtual TEST_RESULT SetupResources(SharedResourcesTest*, BYTE*, SR_WRITE_METHOD);
	virtual TEST_RESULT UpdateSharedResource(SharedResourcesTest *pTest, BYTE*, SR_WRITE_METHOD);
	virtual TEST_RESULT ExecuteVerifyTestCase(SharedResourcesTest*, BYTE*, BYTE*);
	virtual HRESULT		QIKeyedMutex(IDXGIKeyedMutex** ppDXGIKeyedMutex)
	{
		if ( !m_pTex )
			return E_FAIL;
		return m_pTex->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)ppDXGIKeyedMutex);
	}

	ID3D10Texture2D			*m_pSrcTex;
	ID3D10Texture2D			*m_pTex;
};

class DestObjects10_1 :
	public DestObjects10
{
public:
	explicit DestObjects10_1(SharedResourcesTest*, D3D_FEATURE_LEVEL);
};

class SrcObjects10_1 :
	public SrcObjects10
{
public:
	explicit SrcObjects10_1(SharedResourcesTest*, D3D_FEATURE_LEVEL);
};

class DestObjects11 :
	public DestObjects
{
public:
	explicit DestObjects11(SharedResourcesTest*, D3D_FEATURE_LEVEL);
	virtual ~DestObjects11();
	virtual TEST_RESULT SetupTestCase(SharedResourcesTest*);
	virtual TEST_RESULT ExecuteResourceOpen(SharedResourcesTest*, HANDLE);
	virtual TEST_RESULT ExecuteResourceAccess(SharedResourcesTest*, BYTE*, BYTE*);
	virtual HRESULT		QIKeyedMutex(IDXGIKeyedMutex** ppDXGIKeyedMutex)
	{
		if ( !m_pTexShared )
			return E_FAIL;
		return m_pTexShared->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)ppDXGIKeyedMutex);
	}

	TEST_RESULT DrawToResource( SharedResourcesTest *pTest, UINT arrayIndex, UINT mipIndex, void *pData,  ID3D11Texture2D *pSharedTex, bool bSampleFromShared = false);

	ID3D11Device			*m_pDevice;
	ID3D11Device1			*m_pDevice11_1;
	ID3D11PartnerDevice		*m_pPartnerDevice;
	ID3D11DeviceContext		*m_pImmediateContext;
	ID3D11DeviceContext		*m_pEffectiveContext;
	ID3D11DeviceTest		*m_pDevTest;
	DXGI_FORMAT				m_DXGIFormat;
	UINT					m_Width;
	UINT					m_Height;
	UINT					m_ArraySize;
	UINT					m_MipLevels;
	ID3D11Texture2D 		*m_pTexShared;
	ID3D11VertexShader		*m_pVertexShader;
	ID3D11PixelShader		*m_pPixelShader;
	ID3D11InputLayout		*m_pInputLayout;
	ID3D11Buffer			*m_pVertexBuffer;
	ID3D11Texture2D			*m_pSRRTTex; // used as shader resource for Draw-To or render target for Sample-from
	ID3D11ShaderResourceView	*m_pSRV;
	ID3D11RenderTargetView		*m_pRTV;

	HANDLE					m_hCompositionSurface;
};

class SrcObjects11 :
	public DestObjects11
{
public:
	explicit SrcObjects11(SharedResourcesTest*, D3D_FEATURE_LEVEL);
	virtual ~SrcObjects11();
	virtual TEST_RESULT SetupResources(SharedResourcesTest*, BYTE*, SR_WRITE_METHOD);
	virtual TEST_RESULT UpdateSharedResource(SharedResourcesTest *pTest, BYTE*, SR_WRITE_METHOD);
	virtual TEST_RESULT ExecuteVerifyTestCase(SharedResourcesTest*, BYTE*, BYTE*);
	virtual HRESULT		QIKeyedMutex(IDXGIKeyedMutex** ppDXGIKeyedMutex)
	{
		if ( !m_pTex )
			return E_FAIL;
		return m_pTex->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)ppDXGIKeyedMutex);
	}

	ID3D11Texture2D			*m_pTex;
	ID3D11Texture2D			*m_pSrcTex;
	ID3D11Texture2D			**m_pTexArray;
	UINT					m_TexArrayCount;
};


class DestObjectsVideo :
	public DestObjects
{
public:
	explicit DestObjectsVideo(SharedResourcesTest*, D3D_FEATURE_LEVEL);
	virtual ~DestObjectsVideo();
	virtual TEST_RESULT SetupTestCase(SharedResourcesTest*);
	virtual TEST_RESULT ExecuteResourceOpen(SharedResourcesTest*, HANDLE);
	virtual TEST_RESULT ExecuteResourceAccess(SharedResourcesTest*, BYTE*, BYTE*);
	virtual HRESULT		QIKeyedMutex(IDXGIKeyedMutex** ppDXGIKeyedMutex)
	{
		if ( !m_pTexShared )
			return E_FAIL;
		return m_pTexShared->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)ppDXGIKeyedMutex);
	}

protected:
	TEST_RESULT UpdateDataWithVideoProcessor(SharedResourcesTest *pTest, ID3D11Texture2D* pTex2D, BYTE* pDataByte);

public:
	ID3D11Device			*m_pDevice;
	ID3D11Device1			*m_pDevice11_1;
	ID3D11DeviceContext		*m_pImmediateContext;
	ID3D11VideoDevice		*m_pVideoDevice;
	ID3D11VideoContext		*m_pVideoContext;
	ID3D11VideoProcessorEnumerator	*m_pVideoProcessorEnum;
	ID3D11VideoProcessor	*m_pVideoProcessor;

	DXGI_FORMAT				m_DXGIFormat;
	UINT					m_Width;
	UINT					m_Height;
	ID3D11Texture2D 		*m_pTexShared;
};

class SrcObjectsVideo :
	public DestObjectsVideo
{
public:
	explicit SrcObjectsVideo(SharedResourcesTest*, D3D_FEATURE_LEVEL);
	virtual ~SrcObjectsVideo();
	virtual TEST_RESULT SetupResources(SharedResourcesTest*, BYTE*, SR_WRITE_METHOD);
	virtual TEST_RESULT UpdateSharedResource(SharedResourcesTest *pTest, BYTE*, SR_WRITE_METHOD);
	virtual TEST_RESULT ExecuteVerifyTestCase(SharedResourcesTest*, BYTE*, BYTE*);
	virtual HRESULT		QIKeyedMutex(IDXGIKeyedMutex** ppDXGIKeyedMutex)
	{
		if ( !m_pTex )
			return E_FAIL;
		return m_pTex->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)ppDXGIKeyedMutex);
	}


	ID3D11Texture2D			*m_pTex;
};

class SrcObjectsGDI:
	public SrcObjects11
{
public:
	explicit SrcObjectsGDI(SharedResourcesTest*, D3D_FEATURE_LEVEL );
	virtual ~SrcObjectsGDI();

	virtual TEST_RESULT UpdateSharedResource(SharedResourcesTest *pTest, BYTE*, SR_WRITE_METHOD);

protected:
	TEST_RESULT UpdateDataWithGDI(SharedResourcesTest *pTest, ID3D11Texture2D* pTex2D, BYTE* pDataByte);

};

class DestObjectsGDI:
	public DestObjects11
{
public:
	explicit DestObjectsGDI(SharedResourcesTest*, D3D_FEATURE_LEVEL );
	virtual ~DestObjectsGDI();

	virtual TEST_RESULT ExecuteResourceAccess(SharedResourcesTest*, BYTE*, BYTE*);
};

class SrcObjectsCompBuffer:
	public SrcObjects11
{
public:
	explicit SrcObjectsCompBuffer(SharedResourcesTest*, D3D_FEATURE_LEVEL);
	virtual ~SrcObjectsCompBuffer();

	virtual TEST_RESULT SetupResources(SharedResourcesTest*, BYTE*, SR_WRITE_METHOD);

};


class SharedResourcesTest : public CD3D11Test
{
public:
	SharedResourcesTest() :
		m_hFileMap(NULL),
		m_exitChildProcessEvent(NULL),
		m_finishUpdateSharedMemEvent(NULL),
		m_startChildResAccessEvent(NULL),
		m_endChildResAccessEvent(NULL),
		m_endChildCaseWithError(NULL),
		m_pD3D9(NULL),
		m_CurrCompBuffer(0),
		m_pSrcObjs(NULL),
		m_pDestObjs(NULL),
		m_pDataByte1(NULL),
		m_pDataByte2(NULL),
		m_isChildProcess(FALSE),
		m_sharedMemory(NULL),
		m_hWindow(NULL),
		m_ResourceType(D3D_RESOURCE_TYPE_TEXTURE2D),
		m_WriteMethod(SR_INIT_DATA),
		m_ChildAccessMethod(SR_UPDATE_RESOURCE),
		m_testType(SR_SAME_PROCESS),
		m_SyncMethod(SR_NO_SYNC),
		m_Format(DXGI_FORMAT_R8G8B8A8_UNORM),
		m_ResWidth(0),
		m_ResHeight(0),
		m_ArraySize(0),
		m_MipLevels(0),
		m_NumCompBuffers(0),
		m_bBindRenderTarget(false),
		m_bBindShaderResource(false),
		m_bBindCapture(false),
		m_bMiscGenMips(false),
		m_bMiscSharedNTHandle(false),
		m_bMiscGDICompatible(false),
		m_bCompBuffer(false),	
		m_bStereo(false),
		m_srcDeviceFL(D3D_FEATURE_LEVEL_9_1),
		m_destDeviceFL(D3D_FEATURE_LEVEL_9_1),
		m_srcDevice(0),
		m_destDevice(0),
		m_FormatBlockSize(1)
	{
		 ZeroMemory( &m_ProcInf, sizeof(m_ProcInf) );
	}

	void InitTestParameters();
	virtual TEST_RESULT Setup();
	void Cleanup();

	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();

	LPDIRECT3DDEVICE9EX		Create9Device();
	ID3D10Device*	Create10Device();
	ID3D10Device*	Create10_1Device( D3D_FEATURE_LEVEL fl);

	TEST_RESULT CompileVsPsShaders(ID3D10Blob **ppVertexShaderBlob, ID3D10Blob **ppPixelShaderBlob, DWORD dxgiFormatFlags, bool bLevel9 = false);

	// create data for m_pDataByte1 and m_pDataByte2, output the value for BytesPerPixel
	TEST_RESULT CreateDataBytes(UINT width, UINT height, DXGI_FORMAT format);
	TEST_RESULT CreateDataBytesGDI(UINT width, UINT height, DXGI_FORMAT format);
	TEST_RESULT CreateDataBytesVideo(UINT width, UINT height, DXGI_FORMAT format);
	// for video formats, also create the staging resource at the same time, since the planar formats can't be initialized
	TEST_RESULT WriteStagingVideoResource(UINT width, UINT height, DXGI_FORMAT format, 
		ID3D11Device *pDevice, 
		ID3D11DeviceContext* pImmediateContext, 
		ID3D11Texture2D *pTexStaging,
		BYTE *pData);

	HRESULT		VerifyIDirect3DTexture9(IDirect3DTexture9 *tex, BYTE *data, D3DFORMAT D3DFormat, tstring* failureString = NULL);
	HRESULT		VerifyTex2D(ID3D10Device *device, 
							UINT arrayIndex, 
							UINT mipIndex,
							ID3D10Texture2D *tex, 
							BYTE *data, 
							tstring* failureString = NULL);
	HRESULT		VerifyTex2D(ID3D11Device *device, 
							ID3D11DeviceContext* immediatecontext, 
							UINT arrayIndex, 
							UINT mipIndex,
							ID3D11Texture2D *tex, 
							BYTE *data,  
							tstring* failureString = NULL);
	//For debugging uses
	HRESULT  VerifyTex2D_D(ID3D11Device *device, 
										 ID3D11DeviceContext *immediatecontext, 									
										 ID3D11Texture2D *tex, 
										 BYTE *data);

	HRESULT		WaitForEvent(ID3D11Device* pDevice11, ID3D10Device* pDevice10 = NULL, IDirect3DDevice9* pDevice9 = NULL);
	HRESULT		SyncCompBuffer( ID3D11Device* pDevice11 );

	// Get methods
	SR_SYNC_METHOD			GetSyncMethod() const { return m_SyncMethod; }
	SR_WRITE_METHOD			GetChildAccessMethod() const { return m_ChildAccessMethod; }
	SR_WRITE_METHOD			GetWriteMethod() const { return m_WriteMethod; }
	DXGI_FORMAT				GetFormat() const { return m_Format; }
	UINT					GetWidth() const {return m_ResWidth; }
	UINT					GetHeight() const {return m_ResHeight; }
	UINT					GetArraySize() const {return m_ArraySize; }
	UINT					GetMipLevels() const {return m_MipLevels; }
	DXGI_FORMAT				GetViewFormat( DXGI_FORMAT formatTypeless );
	bool					GetBindRenderTarget() const { return m_bBindRenderTarget; }
	bool					GetBindShaderResource() const { return m_bBindShaderResource; }
	bool					GetBindCapture() const { return m_bBindCapture; }
	bool					GetMiscGenMips() const { return m_bMiscGenMips; }
	bool					GetMiscSharedNTHandle() const { return m_bMiscSharedNTHandle; }
	bool					GetMiscGDICompatible() const { return m_bMiscGDICompatible; }
	bool					GetCompBuffer() const { return m_bCompBuffer; }
	bool					GetStereo() const { return m_bStereo; }
	UINT					GetNumCompBuffers() const { return m_NumCompBuffers; }
	UINT					GetCurrentCompBuffer() const { return m_CurrCompBuffer; }
	UINT					GetFormatBlockSize() const { return m_FormatBlockSize; }
	D3D_FEATURE_LEVEL		GetSourceDeviceFeatureLevel() const { return m_srcDeviceFL; }
	D3D_FEATURE_LEVEL		GetDestinationDeviceFeatureLevel() const { return m_destDeviceFL; }

	RESOURCE_CASE			*m_sharedMemory;

protected:
	//inter-process
	HANDLE					m_hFileMap;
	HANDLE					m_exitChildProcessEvent;
	HANDLE					m_finishUpdateSharedMemEvent;
	HANDLE					m_startChildResAccessEvent;
	HANDLE					m_endChildResAccessEvent;
	HANDLE					m_endChildCaseWithError;
	BOOL					m_isChildProcess;
	PROCESS_INFORMATION		m_ProcInf;

	// Window class
	WNDCLASSEXW				m_WindowClass;
	// Window handle
	HWND					m_hWindow;

	//devices
	IDirect3D9Ex			*m_pD3D9;

	//parameters to manipulate
	D3D_RESOURCE_TYPE		m_ResourceType; //type of resource being shared
	SR_WRITE_METHOD			m_WriteMethod;
	SR_WRITE_METHOD			m_ChildAccessMethod;
	SR_TEST_TYPE			m_testType;
	SR_SYNC_METHOD			m_SyncMethod;
	DXGI_FORMAT				m_Format;
	UINT					m_ResWidth;
	UINT					m_ResHeight;
	UINT					m_ArraySize;
	UINT					m_MipLevels;
	UINT					m_NumCompBuffers;
	UINT					m_CurrCompBuffer;
	bool					m_bBindRenderTarget;
	bool					m_bBindShaderResource;
	bool					m_bBindCapture;
	bool					m_bMiscGenMips;
	bool					m_bMiscSharedNTHandle;
	bool					m_bMiscGDICompatible;
	bool					m_bCompBuffer;
	bool					m_bStereo;

	D3D_FEATURE_LEVEL		m_srcDeviceFL;
	D3D_FEATURE_LEVEL		m_destDeviceFL;

	UINT					m_srcDevice;
	SrcObjects				*m_pSrcObjs;

	UINT					m_destDevice;
	DestObjects				*m_pDestObjs;

	BYTE					*m_pDataByte1;	//data used to fill resources
	BYTE					*m_pDataByte2;
	UINT					m_FormatBlockSize;

	// test factors used by derived classes
	testfactor::RFactor m_rResTex2D;
	testfactor::RFactor m_rResTex2DArray;
	testfactor::RFactor m_rResTex2DBC;
	testfactor::RFactor m_rFormats_NoBGR8;
	testfactor::RFactor m_rFormatsBasic_NoBGR8;
	testfactor::RFactor m_rFormatsBasic;
	testfactor::RFactor m_rFormats_All;
	testfactor::RFactor m_rFormats_Extended;
	testfactor::RFactor m_rFormats_NV12;
	testfactor::RFactor m_rFormats_Video;
	testfactor::RFactor m_rFormats_GDI;
	testfactor::RFactor m_rFormats_CompBuffer;
	testfactor::RFactor m_rFormats_NewCompBuffer;
	testfactor::RFactor m_rFormats_BC;
	testfactor::RFactor m_rResWidth;
	testfactor::RFactor m_rResHeight;
	testfactor::RFactor m_rResSize;
	testfactor::RFactor m_rResWidthBC;
	testfactor::RFactor m_rResHeightBC;
	testfactor::RFactor m_rResSizeBC;
	testfactor::RFactor m_rResWidth1;
	testfactor::RFactor m_rResHeight1;
	testfactor::RFactor m_rResSize1;
	testfactor::RFactor m_rResCreationSize1;
	testfactor::RFactor m_rSyncMethod_All;
	testfactor::RFactor m_rSyncMethod_AllWithNTHandle;
	testfactor::RFactor m_rSyncMethod_Event;
	testfactor::RFactor m_rSyncMethod_KeyedMutex;
	testfactor::RFactor m_rSyncMethod_KeyedMutexWithNTHandle;
	testfactor::RFactor m_rSyncMethod_CompBuff;
	testfactor::RFactor m_rWriteMethod_All;
	testfactor::RFactor m_rWriteMethod_UpdateALL;
	testfactor::RFactor m_rWriteMethod_Copy;
	testfactor::RFactor m_rWriteMethod_Update;
	testfactor::RFactor m_rWriteMethod_Draw;
	testfactor::RFactor m_rChildMethod_All;
	testfactor::RFactor m_rChildMethod_Update;
	testfactor::RFactor m_rChildMethod_Copy;
	testfactor::RFactor m_rResourceAccessMethod_All;
	testfactor::RFactor m_rFlagsNV12;
	testfactor::RFactor m_rFlagsVideo;
	testfactor::RFactor m_rFlagsGDI;
	testfactor::RFactor m_rFlagsCompBuff;
	testfactor::RFactor m_rFlagsBC;
	testfactor::RFactor m_rFlagsDX9;
	testfactor::RFactor m_rDevice_SameProc;
	testfactor::RFactor m_rDevice_CrossProc;
	testfactor::RFactor m_rDevice_SameDevice;
	testfactor::RFactor m_rSrcFL_All;
	testfactor::RFactor m_rSrcFL_10x;
	testfactor::RFactor m_rSrcFL_10_0;
	testfactor::RFactor m_rSrcFL_11_0;
	testfactor::RFactor m_rDestFL_All;
	testfactor::RFactor m_rDestFL_10x;
	testfactor::RFactor m_rDestFL_10_0;
	testfactor::RFactor m_rDestFL_11_0;
	testfactor::RFactor m_rSrcDescFL9_Mix;

	// helper functions
	TEST_RESULT CreateAndVerifyEvent(HANDLE &hEvent, tstring EventName);
	
};

class ShareFromDX11ToDX11Test : public SharedResourcesTest
{
	void InitTestParameters();
};
						  
class ShareFromDX11ToDX10_1Test : public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX11ToDX10_0Test : public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX11ToDX9Test : public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX10_1ToDX11Test: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX10_1ToDX10_1Test: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX10_1ToDX10_0Test: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX10_1ToDX9Test: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX10_0ToDX11Test: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX10_0ToDX10_1Test: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX10_0ToDX10_0Test: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX10_0ToDX9Test: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX9ToDX11Test : public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX9ToDX10_1Test: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX9ToDX10_0Test: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX9ToDX9Test: public SharedResourcesTest
{
	void InitTestParameters();
};


class ShareFromDX11ToVideoTest: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromVideoToDX11Test: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromDX11ToGDITest: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromGDIToDX11Test: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareFromCompBuffToDX11Test: public SharedResourcesTest
{
	void InitTestParameters();
	virtual TEST_RESULT Setup();
	void Cleanup();
};

class ShareDX11FL9Test: public SharedResourcesTest
{
	void InitTestParameters();
};

class ShareTilePoolTest: public CD3D11Test
{
public:
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();
	void CleanupD3DObjects();
	SR_SYNC_METHOD	GetSyncMethod() const { return m_SyncMethod; }
	void InitTestParameters();

	TEST_RESULT VerifySharedTilePool(UINT pass);

	ShareTilePoolTest() :
		m_hFileMap(NULL),
		m_exitChildProcessEvent(NULL),
		m_finishUpdateSharedMemEvent(NULL),
		m_startChildResAccessEvent(NULL),
		m_endChildResAccessEvent(NULL),
		m_endChildCaseWithError(NULL),
		m_isChildProcess(FALSE),
		m_sharedMemory(NULL),
		m_pDevice(NULL),
		m_pContext(NULL),
		m_pTilePool(NULL),
		m_pStagingBuffer(NULL),
		m_pTiledTexture(NULL),
		m_SyncMethod(SR_KEYED_MUTEX),
		m_bMiscSharedNTHandle(true)
	{
		 ZeroMemory( &m_ProcInf, sizeof(m_ProcInf) );
	}


protected:
	//inter-process
	HANDLE					m_hFileMap;
	HANDLE					m_exitChildProcessEvent;
	HANDLE					m_finishUpdateSharedMemEvent;
	HANDLE					m_startChildResAccessEvent;
	HANDLE					m_endChildResAccessEvent;
	HANDLE					m_endChildCaseWithError;
	BOOL					m_isChildProcess;
	PROCESS_INFORMATION		m_ProcInf;
	TILE_POOL_SHARED_MEMORY*    m_sharedMemory; 

	SR_SYNC_METHOD			m_SyncMethod;
	bool                    m_bMiscSharedNTHandle;

	ID3D11Device2*          m_pDevice;
	ID3D11DeviceContext2*   m_pContext;
	ID3D11Buffer*           m_pTilePool;
	HANDLE                  m_SharedHandle;
	ID3D11Buffer*           m_pStagingBuffer;
	ID3D11Texture2D*        m_pTiledTexture;

	testfactor::RFactor     m_rSyncMethod_KeyedMutex;
	testfactor::RFactor     m_rSyncMethod_Event;
	testfactor::RFactor     m_rSyncMethod_All;

	// helper functions
	TEST_RESULT CreateAndVerifyEvent(HANDLE &hEvent, tstring EventName);

};

class CDummyTest : public CD3D11Test
{
private:
	void InitTestParameters() {};
};

	
class SharedResourcesTestApp : public CD3D11TestFramework
{
public:
	SharedResourcesTestApp() : 
		m_bPresent( false )
		,m_bSupportExtendedShare(false)
		,m_bSupportExtendedNV12Share(false)
		,m_bLessThanWin8(false)
		,m_bSkipLevel9Cases(false)
		,m_bIsWARP(false)
		,m_bIs11On12(false)
		,m_bEmulateR8WithL8(false)
		,m_bIsAtLeastWDDM1_3(false)
	{}
	void Present() { if ( m_bPresent ) CD3D11TestFramework::Present( 0, 0, 0, 0, 0, 0 );	}
	bool getPresent() { return m_bPresent; }
	bool getSupportExtendedShare() { return m_bSupportExtendedShare; }
	bool getSupportExtendedNV12Share() { return m_bSupportExtendedNV12Share; }
	bool getIsLessThanWin8() { return m_bLessThanWin8; }
	bool getSkipLevel9Cases() { return m_bSkipLevel9Cases; }
	bool getIsWARP() { return m_bIsWARP; }
	bool IsEmulatingR8WithL8( ) const { return m_bEmulateR8WithL8; }
	bool IsAtLeastWDDM1_3( ) const { return m_bIsAtLeastWDDM1_3; }

protected:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }

	void InitOptionVariables();
	bool InitTestGroups();

	bool m_bPresent;
	bool m_bSupportExtendedShare;
	bool m_bSupportExtendedNV12Share;
	bool m_bLessThanWin8;
	bool m_bSkipLevel9Cases;
	bool m_bIsWARP;
	bool m_bIs11On12;
	bool m_bEmulateR8WithL8;
	bool m_bIsAtLeastWDDM1_3;
};
	

//helper methods
D3DFORMAT GetD3DFormat(DXGI_FORMAT pFormat);

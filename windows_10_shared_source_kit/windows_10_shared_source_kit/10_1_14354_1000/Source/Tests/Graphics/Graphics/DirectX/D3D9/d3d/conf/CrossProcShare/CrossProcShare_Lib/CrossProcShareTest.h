#ifndef _CROSSPROCSHAREAPI_H_
#define _CROSSPROCSHAREAPI_H_

#include "Communication.h"
#include "CD3DTest.h"
#include "CTestFactor.h"
#include "cd3d.h"
#include "cd3dx.h"
#include "d3d9.h"
#include "EnumResource.h"
#include <vector>

// Conditionally #define'd, because this could come in from
// the build command line.  See the D3DHELPER magic in dxg.mk
// for details.
#ifndef D3D_DEBUG_INFO
#define D3D_DEBUG_INFO
#endif

// Test Actions messages
const UINT MSG_ACT_LOGCASE               		= 0x00000001;

// Resource Create/Open Messages
const UINT MSG_RES_CREATE               		= 0x01000001;
const UINT MSG_RES_CREATENONPRO                 = 0x01000002;
const UINT MSG_RES_RELEASE            		    = 0x01000004;
const UINT MSG_RES_OPEN                   		= 0x01000008;

//  Resource Use Messages
const UINT MSG_RES_LOCK                   		= 0x02000001;
const UINT MSG_RES_UNLOCK                		= 0x02000002;
const UINT MSG_RES_USE                 		    = 0x02000004;
const UINT MSG_RES_FILLZERO               		= 0x02000008;
const UINT MSG_RES_FILLONE               		= 0x02000010;
const UINT MSG_RES_DRAW               		    = 0x02000020;

// Resource Verify Messages
const UINT MSG_RES_VERIFYZERO             		= 0x04000001;
const UINT MSG_RES_VERIFYONE              		= 0x04000002;

struct TESTCASE
{
	DWORD dwTestID;
	TCHAR szTestName[128];
};

struct MYDISPLAYVERTEX 
{
	FLOAT     x;             
	FLOAT     y;
	FLOAT     z;
	FLOAT	  rhw;
	FLOAT     tu;         
	FLOAT     tv;
	FLOAT     tw;
};

// Flag to make client not try to run every test case.
// Client need only to wait for message from server and quit.
static bool g_bClientDone = false;

// ----------------------------------------------------------------------------
//class CCrossProcShareTest
// ----------------------------------------------------------------------------
class CCrossProcShareTest : public CCommunication
{
protected:

    char			m_szFullName[255];

	float			m_fDXVersion;

	UINT			m_nAdapters;
	UINT			m_nClients;
	bool			m_bMultiheadAdapter;
	bool			m_bReadOnlyCase;
	CObject			*m_pResource;
	CObject			*m_pNonSharedResource;
	HANDLE			m_hShared;
	DWORD			m_dwRTYPE;
    DWORD			m_dwClientOpenedRTYPE;

	RESOURCE_CASE	*m_pSysMemResCase;
	RESOURCE_CASE	*m_pDefaultResCase;
	UINT			m_uCurrentDefaultCase;
	UINT			m_uCurrentSysMemCase;

	RFactor			m_rCasesManager;

    RFactor         m_rCasesDefault[TYPE_MAXTYPE];
    RFactor         m_rCasesSysmem[TYPE_MAXTYPE];

	CEnumResource	m_EnumResource;

    RFactor			                m_rTestCases;
	std::vector <TESTCASE>			m_listTestCases;


	// Methods that needs to be overriden in each tests
    virtual UINT ServerDispatchTest(DWORD dwTestID) = 0;

    virtual void SetTestFactors();
    virtual bool CreateTestFactors();
    virtual bool SetTestCase(UINT uTestNumber);
    virtual bool StartClients();
    
    bool GetNextCase();
	bool GetCurrentCase();
	bool GetCase(UINT nTestNum);
    void DumpCaseInfo(const TCHAR *szTestName, RESOURCE_CASE *pResCase, HANDLE hShared, bool bReadOnly);
    UINT OnMessageFromServer(DWORD dwMessage);

    void SetTestOptions();
    
	//Utilities
 	bool FillSurface(SURFACEDESC *pDesc, LOCKED_RECT *pLockedRect, bool bUseSubRectLock, UINT uPattern = 0);
 	bool FillSurface(LPSURFACE pSurface, bool bUseSubRectLock, UINT uPattern = 0);
 	bool FillSurface(LPTEXTURE pTexture, bool bUseSubRectLock, UINT uPattern = 0);
	bool FillVolumeTexture(RESOURCE_CASE *pResCase, LPVOLUMETEXTURE pTexture, bool bUseSubRectLock, UINT uPattern = 0);
    bool FillCubeTexture(RESOURCE_CASE *pResCase, LPCUBETEXTURE pTexture, UINT uPattern = 0);
	bool FillVertexBuffer(RESOURCE_CASE *pResCase, LPVERTEXBUFFER pVB, UINT uPattern = 0);
	bool FillIndexBuffer(RESOURCE_CASE *pResCase, LPINDEXBUFFER pIB, UINT uPattern = 0);
	bool FillResource(RESOURCE_CASE *pResCase, CObject* pResource, UINT uPattern = 0);
	bool DrawResource(RESOURCE_CASE *pResCase, CObject* pResource, LPSURFACE pRT = NULL);
	bool DrawTexture(LPTEXTURE pTexture, LPSURFACE pRT, float x, float y, UINT size);
	bool DrawVolumeTextureSlice(LPVOLUMETEXTURE pVolumeTexture, LPSURFACE pRT, int iSlice, UINT depth, float x, float y, UINT size);
    bool DrawCubeTexture(LPCUBETEXTURE pCubeTexture, LPSURFACE pRT, float x, float y, UINT size);
	bool DrawVertexBuffer(LPVERTEXBUFFER pVB, LPSURFACE pRT = NULL);
	bool DrawIndexBuffer(LPINDEXBUFFER pIB, LPSURFACE pRT = NULL);
	bool DrawSurface(LPSURFACE pSurface, LPSURFACE pRT, float x, float y, UINT size);
	bool UseResource(DWORD dwRTYPE, RESOURCE_CASE *pResCase, CObject *pResource, bool bReadOnlyResource = false);
	bool UseSurface(RESOURCE_CASE *pResCase, LPSURFACE pSurf, bool bReadOnlyResource);
	bool UseTexture(RESOURCE_CASE *pResCase, LPTEXTURE pTexture, bool bReadOnlyResource);
	bool UseVertexBuffer(RESOURCE_CASE *pResCase, LPVERTEXBUFFER pVB, bool bReadOnlyResource);
	bool UseIndexBuffer(RESOURCE_CASE *pResCase, LPINDEXBUFFER pVB, bool bReadOnlyResource);
    bool UseVolumeTexture(RESOURCE_CASE *pResCase, LPVOLUMETEXTURE pVolTexture, bool bReadOnlyResource);
    bool UseCubeTexture(RESOURCE_CASE *pResCase, LPCUBETEXTURE pCubeTexture, bool bReadOnlyResource);
	bool VerifyContent(RESOURCE_CASE *pResCase, CObject* pResource, UINT uPattern = 0);
	bool LockResource(DWORD dwRTYPE, CObject *pResource);
	bool UnLockResource(DWORD dwRTYPE, CObject *pResource);

    UINT FillResource_FromSharedMem(UINT uPattern = 0);
    UINT VerifyResource_FromSharedMem(UINT uPattern = 0);
	bool FillVerifyNonShared_FromSharedMem(UINT uPattern = 0);
    UINT CreateResource_FromSharedMem(bool bSecure = true); // if bSecure = true process is only secure if created on a secure proccess.
    UINT OpenResource_FromSharedMem();
    UINT OpenResource_Test();
    UINT LockResource_FromSharedMem();
    UINT UnLockResource_FromSharedMem();
    UINT UseResource_FromSharedMem();

    BOOL ResetBind();

    // Client
    UINT Client_CreateResource(bool bSecure = true);
    void Client_ReleaseResource();
    UINT Client_FillResource(UINT uPattern = 0);
    UINT Client_VerifyResource(UINT uPattern = 0);
    UINT Client_OpenResource();
    UINT Client_LockResource();
    UINT Client_UnLockResource();
    UINT Client_UseResource();
    UINT Client_DrawResource();



public: // class functions
	CCrossProcShareTest();
	~CCrossProcShareTest();

	// Framework overloads
    virtual UINT TestInitialize(void);
    virtual bool ExecuteTest(UINT);
	virtual bool CapsCheck();
	virtual bool TestTerminate(void);
	virtual void CommandLineHelp(void) {};
    virtual bool Setup(void);
	bool HandleDevice() { return true; };
    void ReleaseTest();
	void ProcessError(HRESULT hr);
};

#endif

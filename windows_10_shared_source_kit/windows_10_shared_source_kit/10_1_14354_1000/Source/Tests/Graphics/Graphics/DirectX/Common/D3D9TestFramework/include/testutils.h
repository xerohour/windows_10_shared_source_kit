//=======================================================================================
//
//  TestUtils.h
//
//  Test Utilities
//
// --------------------------------------------------------------------------------------
//
//  Copyright (c) 1999 Microsoft Corporation
//  All Rights Reserved
//
//  Original Author: Eron Hennessey
//
//=======================================================================================

#ifndef _TESTUTILS_H_
#define _TESTUTILS_H_

#include <windows.h>
#include <tchar.h>
#include "CD3DDef.h"
#include "HelperFunctions.h"

// For compatibility with those that used shader op counting from here:
#include "ShaderUtils.h"

//Declaration of copies of the guid for all versions of DX

///
/// D3D8
///

/* IID_IDirect3D8 */
/* {1DD9E8DA-1C77-4d40-B0CF-98FEFDFF9512} */
DEFINE_GUID(IID_D3D8, 0x1dd9e8da, 0x1c77, 0x4d40, 0xb0, 0xcf, 0x98, 0xfe, 0xfd, 0xff, 0x95, 0x12);

/* IID_IDirect3DDevice8 */
/* {7385E5DF-8FE8-41D5-86B6-D7B48547B6CF} */
DEFINE_GUID(IID_Device8, 0x7385e5df, 0x8fe8, 0x41d5, 0x86, 0xb6, 0xd7, 0xb4, 0x85, 0x47, 0xb6, 0xcf);

/* IID_IDirect3DResource8 */
/* {1B36BB7B-09B7-410a-B445-7D1430D7B33F} */
DEFINE_GUID(IID_Resource8, 0x1b36bb7b, 0x9b7, 0x410a, 0xb4, 0x45, 0x7d, 0x14, 0x30, 0xd7, 0xb3, 0x3f);

/* IID_IDirect3DBaseTexture8 */
/* {B4211CFA-51B9-4a9f-AB78-DB99B2BB678E} */
DEFINE_GUID(IID_BaseTexture8, 0xb4211cfa, 0x51b9, 0x4a9f, 0xab, 0x78, 0xdb, 0x99, 0xb2, 0xbb, 0x67, 0x8e);

/* IID_IDirect3DTexture8 */
/* {E4CDD575-2866-4f01-B12E-7EECE1EC9358} */
DEFINE_GUID(IID_Texture8, 0xe4cdd575, 0x2866, 0x4f01, 0xb1, 0x2e, 0x7e, 0xec, 0xe1, 0xec, 0x93, 0x58);

/* IID_IDirect3DCubeTexture8 */
/* {3EE5B968-2ACA-4c34-8BB5-7E0C3D19B750} */
DEFINE_GUID(IID_CubeTexture8, 0x3ee5b968, 0x2aca, 0x4c34, 0x8b, 0xb5, 0x7e, 0x0c, 0x3d, 0x19, 0xb7, 0x50);

/* IID_IDirect3DVolumeTexture8 */
/* {4B8AAAFA-140F-42ba-9131-597EAFAA2EAD} */
DEFINE_GUID(IID_VolumeTexture8, 0x4b8aaafa, 0x140f, 0x42ba, 0x91, 0x31, 0x59, 0x7e, 0xaf, 0xaa, 0x2e, 0xad);

/* IID_IDirect3DVertexBuffer8 */
/* {8AEEEAC7-05F9-44d4-B591-000B0DF1CB95} */
DEFINE_GUID(IID_VertexBuffer8, 0x8aeeeac7, 0x05f9, 0x44d4, 0xb5, 0x91, 0x00, 0x0b, 0x0d, 0xf1, 0xcb, 0x95);

/* IID_IDirect3DIndexBuffer8 */
/* {0E689C9A-053D-44a0-9D92-DB0E3D750F86} */
DEFINE_GUID(IID_IndexBuffer8, 0x0e689c9a, 0x053d, 0x44a0, 0x9d, 0x92, 0xdb, 0x0e, 0x3d, 0x75, 0x0f, 0x86);

/* IID_IDirect3DSurface8 */
/* {B96EEBCA-B326-4ea5-882F-2FF5BAE021DD} */
DEFINE_GUID(IID_Surface8, 0xb96eebca, 0xb326, 0x4ea5, 0x88, 0x2f, 0x2f, 0xf5, 0xba, 0xe0, 0x21, 0xdd);

/* IID_IDirect3DVolume8 */
/* {BD7349F5-14F1-42e4-9C79-972380DB40C0} */
DEFINE_GUID(IID_Volume8, 0xbd7349f5, 0x14f1, 0x42e4, 0x9c, 0x79, 0x97, 0x23, 0x80, 0xdb, 0x40, 0xc0);

/* IID_IDirect3DSwapChain8 */
/* {928C088B-76B9-4C6B-A536-A590853876CD} */
DEFINE_GUID(IID_SwapChain8, 0x928c088b, 0x76b9, 0x4c6b, 0xa5, 0x36, 0xa5, 0x90, 0x85, 0x38, 0x76, 0xcd);



///
/// D3D9
///


/* IID_IDirect3D9 */
/* {81BDCBCA-64D4-426d-AE8D-AD0147F4275C} */
DEFINE_GUID(IID_D3D9, 0x81bdcbca, 0x64d4, 0x426d, 0xae, 0x8d, 0xad, 0x1, 0x47, 0xf4, 0x27, 0x5c);

/* IID_IDirect3DDevice9 */
// {D0223B96-BF7A-43fd-92BD-A43B0D82B9EB} */
DEFINE_GUID(IID_Device9, 0xd0223b96, 0xbf7a, 0x43fd, 0x92, 0xbd, 0xa4, 0x3b, 0xd, 0x82, 0xb9, 0xeb);

/* IID_IDirect3DResource9 */
// {05EEC05D-8F7D-4362-B999-D1BAF357C704}
DEFINE_GUID(IID_Resource9, 0x5eec05d, 0x8f7d, 0x4362, 0xb9, 0x99, 0xd1, 0xba, 0xf3, 0x57, 0xc7, 0x4);

/* IID_IDirect3DBaseTexture9 */
/* {580CA87E-1D3C-4d54-991D-B7D3E3C298CE} */
DEFINE_GUID(IID_BaseTexture9, 0x580ca87e, 0x1d3c, 0x4d54, 0x99, 0x1d, 0xb7, 0xd3, 0xe3, 0xc2, 0x98, 0xce);

/* IID_IDirect3DTexture9 */
/* {85C31227-3DE5-4f00-9B3A-F11AC38C18B5} */
DEFINE_GUID(IID_Texture9, 0x85c31227, 0x3de5, 0x4f00, 0x9b, 0x3a, 0xf1, 0x1a, 0xc3, 0x8c, 0x18, 0xb5);

/* IID_IDirect3DCubeTexture9 */
/* {FFF32F81-D953-473a-9223-93D652ABA93F} */
DEFINE_GUID(IID_CubeTexture9, 0xfff32f81, 0xd953, 0x473a, 0x92, 0x23, 0x93, 0xd6, 0x52, 0xab, 0xa9, 0x3f);

/* IID_IDirect3DVolumeTexture9 */
/* {2518526C-E789-4111-A7B9-47EF328D13E6} */
DEFINE_GUID(IID_VolumeTexture9, 0x2518526c, 0xe789, 0x4111, 0xa7, 0xb9, 0x47, 0xef, 0x32, 0x8d, 0x13, 0xe6);

/* IID_IDirect3DVertexBuffer9 */
/* {B64BB1B5-FD70-4df6-BF91-19D0A12455E3} */
DEFINE_GUID(IID_VertexBuffer9, 0xb64bb1b5, 0xfd70, 0x4df6, 0xbf, 0x91, 0x19, 0xd0, 0xa1, 0x24, 0x55, 0xe3);

/* IID_IDirect3DIndexBuffer9 */
/* {7C9DD65E-D3F7-4529-ACEE-785830ACDE35} */
DEFINE_GUID(IID_IndexBuffer9, 0x7c9dd65e, 0xd3f7, 0x4529, 0xac, 0xee, 0x78, 0x58, 0x30, 0xac, 0xde, 0x35);

/* IID_IDirect3DSurface9 */
/* {0CFBAF3A-9FF6-429a-99B3-A2796AF8B89B} */
DEFINE_GUID(IID_Surface9, 0xcfbaf3a, 0x9ff6, 0x429a, 0x99, 0xb3, 0xa2, 0x79, 0x6a, 0xf8, 0xb8, 0x9b);

/* IID_IDirect3DVolume9 */
/* {24F416E6-1F67-4aa7-B88E-D33F6F3128A1} */
DEFINE_GUID(IID_Volume9, 0x24f416e6, 0x1f67, 0x4aa7, 0xb8, 0x8e, 0xd3, 0x3f, 0x6f, 0x31, 0x28, 0xa1);

/* IID_IDirect3DSwapChain9 */
/* {794950F2-ADFC-458a-905E-10A10B0B503B} */
DEFINE_GUID(IID_SwapChain9, 0x794950f2, 0xadfc, 0x458a, 0x90, 0x5e, 0x10, 0xa1, 0xb, 0xb, 0x50, 0x3b);

/* IID_IDirect3DVDecl9 */
/* {DD13C59C-36FA-4098-A8FB-C7ED39DC8546} */
DEFINE_GUID(IID_VDecl9, 0xdd13c59c, 0x36fa, 0x4098, 0xa8, 0xfb, 0xc7, 0xed, 0x39, 0xdc, 0x85, 0x46);

/* IID_IDirect3DVShader9 */
/* {EFC5557E-6265-4613-8A94-43857889EB36} */
DEFINE_GUID(IID_VShader9, 0xefc5557e, 0x6265, 0x4613, 0x8a, 0x94, 0x43, 0x85, 0x78, 0x89, 0xeb, 0x36);

/* IID_IDirect3DPShader9 */
/* {6D3BDBDC-5B02-4415-B852-CE5E8BCCB289} */
DEFINE_GUID(IID_PShader9, 0x6d3bdbdc, 0x5b02, 0x4415, 0xb8, 0x52, 0xce, 0x5e, 0x8b, 0xcc, 0xb2, 0x89);

/* IID_IDirect3DStateBlock9 */
/* {B07C4FE5-310D-4ba8-A23C-4F0F206F218B} */
DEFINE_GUID(IID_StateBlock9, 0xb07c4fe5, 0x310d, 0x4ba8, 0xa2, 0x3c, 0x4f, 0xf, 0x20, 0x6f, 0x21, 0x8b);

/* IID_IDirect3DQuery9 */
/* {d9771460-a695-4f26-bbd3-27b840b541cc} */
DEFINE_GUID(IID_Query9, 0xd9771460, 0xa695, 0x4f26, 0xbb, 0xd3, 0x27, 0xb8, 0x40, 0xb5, 0x41, 0xcc);

enum D3DGUIDS
{
	IID_D3D = 0,
	IID_DEVICE = 1,
	IID_RESOURCE = 2,
	IID_BASETEXTURE = 3,
	IID_TEXTURE = 4,
	IID_CUBETEXTURE = 5,
	IID_VOLUMETEXTURE = 6,
	IID_VERTEXBUFFER = 7,
	IID_INDEXBUFFER = 8,
	IID_SURFACE = 9,
	IID_VOLUME = 10,
	IID_SWAPCHAIN = 11,
	IID_VDECL = 12,
	IID_VSHADER = 13,
	IID_PSHADER = 14,
	IID_STATEBLOCK = 15,
    IID_QUERY = 16
};

// ----------------------------------------------------------------------------
// Performs a compare of two floats or doubles to a certain precision.
// returns an integer value <0 if fInput1 < fInput2,
//                          =0 if fInput1 == finput2,
//                          >0 if fInput1 > fInput2.

int fCompare(double fInput1, double fInput2, int nPrecision);

//
//	D3D Utilities
//

struct VS_DCL_INFO
{
    BOOL      bIsUsed;
    DECLUSAGE Usage;
    DWORD     dwUsageIndex;
    DWORD     dwRegNum;
};

HRESULT VS8ToVS9(TCHAR *sShader, TCHAR** sShaderOut);
HRESULT VS9ToVS8(TCHAR *sShader, TCHAR** sShaderOut);
HRESULT VDecl8ToVDecl9(DWORD *pDecl, VERTEXELEMENT **ppVE);
HRESULT GetVSDCLInfo(TCHAR *sShaderIn, VS_DCL_INFO pDCLInfo[16], DWORD *pdwDCLCount);
BOOL FindVertexElementOffset(VERTEXELEMENT *pDecl, DECLUSAGE Usage, UINT uUsageIndex, UINT *pOffset);
UINT GetSizeOfVertexElement(DECLTYPE Type);

void StringToFmt(LPTSTR szFmt, FMT *pFormat);
void StringToFmt(LPTSTR szFmt, MULTISAMPLE_TYPE *pFormat);
void FmtToString(FORMATTYPE Type, FORMAT *pFormat, LPTSTR szOut);
void ExpandDDPF(FMT d3dFormat, PIXELFORMAT *pddpfFormat);

// ----------------------------------------------------------------------------
// These open modal dialogs for the user to edit a vertex declaration or FVF flags:
// returns true for Ok, false for Cancel

// output decl must be an array of MAXD3DDECLLENGTH elements:
// TODO: Implement these:
// bool EditDeclDialog(const VERTEXELEMENT* pDeclInitial, VERTEXELEMENT* pDeclOut);
// bool EditFVFDialog(DWORD dwFVFInitial, DWORD* pdwFVFOut);

// ----------------------------------------------------------------------------
// This will return the GUID of the interface for the DX version

REFIID GetD3DGUID(D3DGUIDS guidIndex, float fDXVersion);


//-----------------------------------------------------------------------------
// Name: class CD3DArcBall
// Desc:
//-----------------------------------------------------------------------------
class CD3DArcBall
{
    INT            m_iWidth;   // ArcBall's window width
    INT            m_iHeight;  // ArcBall's window height
    FLOAT          m_fRadius;  // ArcBall's radius in screen coords
    FLOAT          m_fRadiusTranslation; // ArcBall's radius for translating the target

    D3DXQUATERNION* m_qDown;               // Quaternion before button down
    D3DXQUATERNION* m_qNow;                // Composite quaternion for current drag
    D3DXMATRIX*     m_matRotation;         // Matrix for arcball's orientation
    D3DXMATRIX*     m_matRotationDelta;    // Matrix for arcball's orientation
    D3DXMATRIX*     m_matTranslation;      // Matrix for arcball's position
    D3DXMATRIX*     m_matTranslationDelta; // Matrix for arcball's position
    BOOL           m_bDrag;               // Whether user is dragging arcball
    BOOL           m_bRightHanded;        // Whether to use RH coordinate system

public:
    void ScreenToVector( int sx, int sy, D3DXVECTOR3 *pVecOut );
    LRESULT     HandleMouseMessages( UINT, WPARAM, LPARAM );

    D3DXMATRIX* GetRotationMatrix()         { return m_matRotation; }
    D3DXMATRIX* GetRotationDeltaMatrix()    { return m_matRotationDelta; }
    D3DXMATRIX* GetTranslationMatrix()      { return m_matTranslation; }
    D3DXMATRIX* GetTranslationDeltaMatrix() { return m_matTranslationDelta; }
    BOOL        IsBeingDragged()            { return m_bDrag; }

    void SetRotationMatrix(D3DXMATRIX *pMat);
    void SetTranslationMatrix(D3DXMATRIX *pMat);
    VOID        SetRadius( FLOAT fRadius );
    VOID        SetWindow( INT w, INT h, FLOAT r=0.9 );
    VOID        SetRightHanded( BOOL bRightHanded ) { m_bRightHanded = bRightHanded; }
	VOID		Reset();

    CD3DArcBall();
    ~CD3DArcBall();
};


#endif //_TESTUTILS_H_

//=======================================================================================
// EOFF

#include "D3D10SwapChain.h"
#include "DeviceWrapper10.h"

#include <StrSafe.h>

#include <Handle.h>

using WindowsTest::CHandle;

#undef MSG_LEN
#define MSG_LEN     1024


////////////////////////////////////////////////////////////////////////////////
// Definitions of vector structures.

typedef struct _Vec4
{
    float x;
    float y;
    float z;
    float w;
}
Vec4;

typedef struct _Vec2
{
    float u;
    float v;
}
Vec2;

typedef struct _Vec3
{
    float u;
    float v;
    float w;
}
Vec3;

typedef struct _Vtx
{
    Vec4 Position;
    Vec2 Texcoord;
    //Vec3 Texcoord2;
}
Vtx;

// maps unsigned 8 bits/channel to D3DCOLOR
/*#define D3DCOLOR_ARGB(a,r,g,b) \
((DWORD)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))
#define D3DCOLOR_RGBA(r,g,b,a) D3DCOLOR_ARGB(a,r,g,b)
#define D3DCOLOR_XRGB(r,g,b)   D3DCOLOR_ARGB(0xff,r,g,b)*/


////////////////////////////////////////////////////////////////////////////////
// CDeviceWrapper10 methods

CDeviceWrapper10::CDeviceWrapper10(CTest *pInitLoggingGroup)
    :
pLoggingGroup( pInitLoggingGroup ),
    pD3DDevice( NULL ),
    m_RotationMode( DXGI_MODE_ROTATION_UNSPECIFIED ),
    m_bAlternateRender( false ),
    m_nxDivisions( 64 ),
    m_nyDivisions( 8 ),
    m_nNumVertices( 0 ),
    m_nNumIndixes( 0 ),
    m_nStride( 0 ),
    bReadyForTexRender( false ),
    m_pVB( NULL ),
    m_pIB( NULL ),
    m_pVSBuffer( NULL ),
    m_pEL( NULL ),
    m_pPixelShader( NULL ),
    m_pPixelShaderRemoveXR_BIAS( NULL ),
    m_pVertexShader( NULL ),
    m_pSampler( NULL ),
    m_pRastState( NULL ),
    m_pBlendState( NULL ),
    m_pDepthState( NULL ),
	m_hrLastError( S_OK )
{
};


CDeviceWrapper10::~CDeviceWrapper10()
{
    ReleaseAllObjects();
};


HRESULT CDeviceWrapper10::WrapDevice
    (
    __in ID3D10Device * pDevice
    )
{
    if ( pD3DDevice != NULL )
    {
        ReleaseAllObjects();
    };


    pD3DDevice = pDevice;

    if ( pD3DDevice != NULL )
    {
        pD3DDevice->AddRef();
    };

    return S_OK;
};


HRESULT CDeviceWrapper10::LoadShader
    (
    LPCSTR ShaderCode,
    __out void ** ppShader,
    LPCSTR pFuncName,
    LPCSTR pProfile,
    __out_opt ID3D10Blob ** ppRetainCompiledShader
    )
{
    //
    // Initialize locals.
    //

    HRESULT hr = E_FAIL;
    ID3D10Blob * pBuf = NULL;
    ID3D10Blob * pErrorMessages = NULL;

    DWORD dwFlags = 0;


    //
    // Compile the shader.
    //

    HRESULT hDummyResult = S_OK;
    hr = D3DX10CompileFromMemory
        (
        ShaderCode,
        strlen( ShaderCode ),
        NULL,
        NULL,
        NULL,
        pFuncName,
        pProfile,
        dwFlags,
        0,
        NULL,
        &pBuf,
        &pErrorMessages,
        &hDummyResult
        );

    if ( FAILED( hr ) )
    {
        if ( pLoggingGroup )
        {
            pLoggingGroup->WriteToLog
                (
                _T( "LoadShader:  D3D10CompileShader failed with error 0x%x and reported the following error:  %s." ),
                hr,
                (
                pErrorMessages != NULL &&
                pErrorMessages->GetBufferSize() > 0
                )
                ? pErrorMessages->GetBufferPointer()
                : _T( "[none]" )
                );
        };

        if
            (
            pErrorMessages != NULL &&
            pErrorMessages->GetBufferSize() > 0
            )
        {
            DebugOut.Write( "D3D10CompileShader reports the following error:\n" );
            DebugOut.Write( (LPCTSTR) pErrorMessages->GetBufferPointer() );
            DebugOut.Write( "\n" );
        };

        goto Cleanup;
    };


    if ( pErrorMessages != NULL )
    {
        pErrorMessages->Release();
        pErrorMessages = NULL;
    };


    if
        (
        strcmp(pProfile, "vs_4_0") == 0 ||
        strcmp(pProfile, "vs_4_0_level_9_1") == 0 ||
        strcmp(pProfile, "vs_4_0_level_9_3") == 0
        )
    {
        hr = pD3DDevice->CreateVertexShader
            (
            (DWORD *) pBuf->GetBufferPointer(),
            pBuf->GetBufferSize(),
            (ID3D10VertexShader**)ppShader
            );
    }
    else if
        (
        strcmp(pProfile, "gs_4_0") == 0
        )
    {
        hr = pD3DDevice->CreateGeometryShader
            (
            (DWORD *) pBuf->GetBufferPointer(),
            pBuf->GetBufferSize(),
            (ID3D10GeometryShader**) ppShader
            );
    }
    else if
        (
        strcmp(pProfile, "ps_4_0") == 0 ||
        strcmp(pProfile, "ps_4_0_level_9_1") == 0 ||
        strcmp(pProfile, "ps_4_0_level_9_3") == 0
        )
    {
        hr = pD3DDevice->CreatePixelShader
            (
            (DWORD *) pBuf->GetBufferPointer(),
            pBuf->GetBufferSize(),
            (ID3D10PixelShader**) ppShader
            );
    }
    else
    {
        hr = E_INVALIDARG;
    };


Cleanup:
    if ( ppRetainCompiledShader )
    {
        *ppRetainCompiledShader = pBuf;
    }
    else
    {
        if ( pBuf != NULL )
        {
            pBuf->Release();
            pBuf = NULL;
        };
    };

    if ( pErrorMessages != NULL )
    {
        pErrorMessages->Release();
        pErrorMessages = NULL;
    };

	if( FAILED( hr ) )
		m_hrLastError = hr;

    return hr;
};


bool CDeviceWrapper10::CreateTexRenderResources
    (
    LPCSTR VertexShaderModel,
    LPCSTR PixelShaderModel
    )
{
    //
    // Initialize locals.
    //

    bool bOverallResult = false;
    Vtx verts[ 4 ];
    WORD indices[ 4 ];
    DXGI_RESULT hr(this, S_OK);
    D3DXMATRIX matrix;
    ID3D10Blob * pCode = NULL;


    //
    // Release any currently-held resources.
    //

    ReleaseTexRenderResources();


    if ( m_bAlternateRender )
    {
        //
        // Calculate number of vertixes and indixes.
        //

        m_nNumVertices = 6 * m_nxDivisions * m_nyDivisions;
        m_nNumIndixes = 6 * m_nxDivisions * m_nyDivisions;
        m_nStride = 12 + 4;


        //
        // Create the vertex buffer.
        //

        D3D10_BUFFER_DESC DescBuffer;
        ZeroMemory( &DescBuffer, sizeof( D3D10_BUFFER_DESC ) );

        DescBuffer.ByteWidth = m_nStride * m_nNumVertices;
        DescBuffer.Usage = D3D10_USAGE_DEFAULT;
        DescBuffer.BindFlags = D3D10_BIND_VERTEX_BUFFER;

        hr = pD3DDevice->CreateBuffer
            (
            &DescBuffer,
            NULL,
            &m_pVB
            );

        if ( FAILED( hr ) )
        {
            if ( pLoggingGroup )
            {
                pLoggingGroup->WriteToLog
                    (
                    "CreateBuffer on vertex buffer failed with error 0x%x.",
                    hr
                    );
            };

            goto Cleanup;
        };


        //
        // Create the index buffer.
        //

        DescBuffer.ByteWidth = m_nNumIndixes * sizeof( WORD );
        DescBuffer.BindFlags = D3D10_BIND_INDEX_BUFFER;

        hr = pD3DDevice->CreateBuffer
            (
            &DescBuffer,
            NULL,
            &m_pIB
            );

        if ( FAILED( hr ) )
        {
            if ( pLoggingGroup )
            {
                pLoggingGroup->WriteToLog
                    (
                    "CreateBuffer on index buffer failed with error 0x%x.",
                    hr
                    );
            };

            goto Cleanup;
        };


        //
        // Create the pixel shader.
        //

        char PixelShader[] =
            "struct PS_INPUT								\n"
            "{												\n"
            "    float4 Position : SV_Position;				\n"
            "    linear float4 Color : Color;				\n"
            "};												\n"
            "float4 PS( PS_INPUT In ) : SV_Target			\n"
            "{												\n"
            "	return In.Color;							\n"
            "}												\n";

        hr = LoadShader
            (
            PixelShader,
            (void**) &m_pPixelShader,
            "PS",
            PixelShaderModel,
            NULL
            );

        if ( FAILED( hr ) )
        {
            if ( pLoggingGroup )
            {
                pLoggingGroup->WriteToLog
                    (
                    "CDeviceWrapper10::CreateTexRenderResources:  LoadShader on pixel shader failed with error 0x%x.",
                    hr
                    );
            };

            goto Cleanup;
        };


        //
        // Create the vertex shader.
        //

        char VertexShader[] =
            "cbuffer cb0 : register(b0)						\n"
            "{												\n"
            "	float4x4 modelViewProj : WORLDVIEWPROJ;		\n"
            "};												\n"
            "struct VS_INPUT								\n"
            "{												\n"
            "	float3 Position : Position;					\n"
            "	float4 Color : Color;						\n"
            "};												\n"
            "struct VS_OUTPUT								\n"
            "{												\n"
            "	float4 Position : SV_Position;				\n"
            "	linear float4 Color : Color;				\n"
            "};												\n"
            "VS_OUTPUT VS( in VS_INPUT In )					\n"
            "{												\n"
            "	VS_OUTPUT Out = (VS_OUTPUT)0;				\n"
            "	Out.Position = mul( modelViewProj, float4( In.Position, 1.0f ) );	\n"
            "	Out.Color = In.Color;						\n"
            "	return Out;									\n"
            "}												\n";

        hr = LoadShader
            (
            VertexShader,
            (void**) &m_pVertexShader,
            "VS",
            VertexShaderModel,
            &pCode
            );

        if ( FAILED( hr ) )
        {
            if ( pLoggingGroup )
            {
                pLoggingGroup->WriteToLog
                    (
                    "CDeviceWrapper10::CreateTexRenderResources:  LoadShader on vertex shader failed with error 0x%x.",
                    hr
                    );
            };

            goto Cleanup;
        };


        //
        // Create the input layout.
        //

        const NumVertexElements = 2;
        D3D10_INPUT_ELEMENT_DESC Desc[NumVertexElements];
        ZeroMemory( &Desc, NumVertexElements * sizeof( D3D10_INPUT_ELEMENT_DESC ) );

        Desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        Desc[0].SemanticName = "Position";
        Desc[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;

        Desc[1].AlignedByteOffset = 12;
        Desc[1].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        Desc[1].SemanticName = "Color";
        Desc[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;

        hr = pD3DDevice->CreateInputLayout
            (
            Desc,
            NumVertexElements,
            pCode->GetBufferPointer(),
            pCode->GetBufferSize(),
            &m_pEL
            );

        if ( FAILED( hr ) )
        {
            if ( pLoggingGroup )
            {
                pLoggingGroup->WriteToLog
                    (
                    "CreateInputLayout failed with error 0x%x.",
                    hr
                    );
            };

            goto Cleanup;
        };


        pCode->Release();
        pCode = NULL;


        //
        // Populate the vertex buffer.
        //

        if ( NULL != m_pVB )
        {
            D3D10_BUFFER_DESC bufferDesc;
            m_pVB->GetDesc( &bufferDesc );
            bufferDesc.Usage = D3D10_USAGE_STAGING;
            bufferDesc.BindFlags = 0;
            bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;

            ID3D10Buffer * pStagingBuffer = NULL;
            hr = pD3DDevice->CreateBuffer
                (
                &bufferDesc,
                NULL,
                &pStagingBuffer
                );

            if ( FAILED( hr ) )
            {
                if ( pLoggingGroup )
                {
                    pLoggingGroup->WriteToLog
                        (
                        "CreateBuffer on vertex staging buffer failed with error 0x%x.",
                        hr
                        );
                };

                goto Cleanup;
            };

            BYTE *pVertixes;
            hr = pStagingBuffer->Map( D3D10_MAP_READ_WRITE, 0, (void**) &pVertixes );
            if( FAILED( hr ) )
                return false;

            // call the derived class to fill the buffer

            // arrange the grids in x-y matrix, fill the x axis first
            int nGridNoSquareRoot = 1;
            int nxGrids = 1;
            int nyGrids = 1;

            float yDim	= 2.0f / (float)nxGrids;
            float xDim	= 2.0f / (float)nyGrids;
            float yOffset = ( 0 / nGridNoSquareRoot ) * yDim - 1.0f;
            float xOffset = ( 0 % nGridNoSquareRoot ) * xDim - 1.0f;

            // fdx = width of one square divided by 2 (length and depth = 2)
            // UNUSED: float fdx = 2.0f * 1.0f / (float)m_nxDivisions;
            // UNUSED: float fdy = 2.0f * 1.0f / (float)m_nyDivisions;

            srand( 666 );

            for( UINT y = 0; y < m_nyDivisions; y++ )
                for( UINT x = 0; x < m_nxDivisions;  x++ )
                {
                    DWORD dwColor = D3DCOLOR_ARGB((BYTE)( rand() % 256 ),
                        (BYTE)( rand() % 256 ),
                        (BYTE)( rand() % 256 ),
                        (BYTE)( rand() % 256 ) );
                    DWORD dwColor2 = D3DCOLOR_ARGB((BYTE)( rand() % 256 ),
                        (BYTE)( rand() % 256 ),
                        (BYTE)( rand() % 256 ),
                        (BYTE)( rand() % 256 ) );

                    UINT vecX[] = { x, x + 1, x, x, x + 1, x + 1 };
                    UINT vecY[] = { y, y, y + 1, y + 1, y, y + 1 };

                    for( UINT uIndex = 0; uIndex < 6; uIndex++ )
                    {
                        D3DXVECTOR3 *ppositionVertixes = (D3DXVECTOR3*) ( pVertixes + 0 );
                        DWORD *pcolorVertixes = (DWORD*) ( pVertixes + 12 );

                        ppositionVertixes->x = xDim * ( vecX[uIndex] ) / (float)m_nxDivisions + xOffset;
                        ppositionVertixes->y = yDim * ( vecY[uIndex] ) / (float)m_nyDivisions + yOffset;
                        ppositionVertixes->z = 0.5f;

                        *pcolorVertixes = uIndex < 3? dwColor : dwColor2;

                        pVertixes += m_nStride;
                    }
                }

                pStagingBuffer->Unmap();

                pD3DDevice->CopyResource( m_pVB, pStagingBuffer );

                pStagingBuffer->Release();
                pStagingBuffer = NULL;
        };


        //
        // Populate the index buffer.
        //

        if ( m_pIB != NULL )
        {
            //
            // Create an index staging buffer.
            //

            D3D10_BUFFER_DESC bufferDesc;
            m_pIB->GetDesc( &bufferDesc );
            bufferDesc.Usage = D3D10_USAGE_STAGING;
            bufferDesc.BindFlags = 0;
            bufferDesc.CPUAccessFlags = D3D10_CPU_ACCESS_READ | D3D10_CPU_ACCESS_WRITE;

            ID3D10Buffer * pStagingBuffer = NULL;
            hr = pD3DDevice->CreateBuffer
                (
                &bufferDesc,
                NULL,
                &pStagingBuffer
                );

            if ( FAILED( hr ) )
            {
                if ( pLoggingGroup )
                {
                    pLoggingGroup->WriteToLog
                        (
                        "CreateBuffer on index staging buffer failed with error 0x%x.",
                        hr
                        );
                };

                goto Cleanup;
            };

            WORD * pIndices = NULL;
            hr = pStagingBuffer->Map
                (
                D3D10_MAP_READ_WRITE,
                0,
                (void**) &pIndices
                );

            if( FAILED( hr ) )
            {
                pLoggingGroup->WriteToLog
                    (
                    "Map on staging buffer failed with error 0x%x.",
                    hr
                    );

                goto Cleanup;
            };

            // call the derived class to fill the buffer
            for
                (
                UINT uIndex = 0;
            uIndex < 6 * m_nxDivisions * m_nyDivisions;
            ++uIndex
                )
            {
                *pIndices = (WORD) uIndex;
                ++pIndices;
            };

            pStagingBuffer->Unmap();

            pD3DDevice->CopyResource
                (
                m_pIB,
                pStagingBuffer
                );

            pStagingBuffer->Release();
            pStagingBuffer = NULL;
        };
    }
    else
    {
        //
        // Set up the arrays used to initialize the vertex and index buffers.
        //

        m_nNumVertices = 4;
        m_nNumIndixes = 4;

        // a quad
        // ------------------------------
        verts[0].Position.x = -1.0f;
        verts[0].Position.y = 1.0f;
        verts[0].Position.z = 1.0f;
        verts[0].Position.w = 1.0f;

        //verts[0].Texcoord.u = 2.5f;
        //verts[0].Texcoord.v = 2.5f;

        verts[0].Texcoord.u = 0.0f;
        verts[0].Texcoord.v = 0.0f;

        //verts[0].Texcoord2.u = 0.0f;
        //verts[0].Texcoord2.v = 0.0f;
        //verts[0].Texcoord2.w = 0.0f;

        // ------------------------------
        verts[1].Position.x = 1.0f;
        verts[1].Position.y = 1.0f;
        verts[1].Position.z = 1.0f;
        verts[1].Position.w = 1.0f;

        //verts[1].Texcoord.u = -1.5f;
        //verts[1].Texcoord.v = 2.5f;

        verts[1].Texcoord.u = 1.0f;
        verts[1].Texcoord.v = 0.0f;

        //verts[1].Texcoord2.u = 0.0f;
        //verts[1].Texcoord2.v = 0.0f;
        //verts[1].Texcoord2.w = 0.0f;

        // ------------------------------
        verts[2].Position.x = -1.0f;
        verts[2].Position.y = -1.0f;
        verts[2].Position.z = 1.0f;
        verts[2].Position.w = 1.0f;

        //verts[2].Texcoord.u = 2.5f;
        //verts[2].Texcoord.v = -1.5f;

        verts[2].Texcoord.u = 0.0f;
        verts[2].Texcoord.v = 1.0f;

        //verts[2].Texcoord2.u = 0.0f;
        //verts[2].Texcoord2.v = 0.0f;
        //verts[2].Texcoord2.w = 0.0f;

        // ------------------------------
        verts[3].Position.x = 1.0f;
        verts[3].Position.y = -1.0f;
        verts[3].Position.z = 1.0f;
        verts[3].Position.w = 1.0f;

        //verts[3].Texcoord.u = -1.5f;
        //verts[3].Texcoord.v = -1.5f;

        verts[3].Texcoord.u = 1.0f;
        verts[3].Texcoord.v = 1.0f;

        //verts[3].Texcoord2.u = 0.0f;
        //verts[3].Texcoord2.v = 0.0f;
        //verts[3].Texcoord2.w = 0.0f;

        // ------------------------------

        indices[ 0 ] = 0;
        indices[ 1 ] = 1;
        indices[ 2 ] = 2;
        indices[ 3 ] = 3;


        //
        // Create and initialize the vertex buffer.
        //

        m_nStride = sizeof( Vtx );

        {
            D3D10_SUBRESOURCE_DATA InitialData;
            D3D10_BUFFER_DESC DescBuffer;

            InitialData.pSysMem = verts;
            InitialData.SysMemPitch = 0;
            InitialData.SysMemSlicePitch = 0;

            DescBuffer.ByteWidth = m_nStride * m_nNumVertices;
            DescBuffer.Usage = D3D10_USAGE_IMMUTABLE;
            DescBuffer.BindFlags = D3D10_BIND_VERTEX_BUFFER;
            DescBuffer.CPUAccessFlags = 0;
            DescBuffer.MiscFlags = 0;

            hr = pD3DDevice->CreateBuffer
                (
                &DescBuffer,
                &InitialData,
                &m_pVB
                );

            if ( FAILED( hr ) )
            {
                if ( pLoggingGroup )
                {
                    pLoggingGroup->WriteToLog
                        (
                        "CreateBuffer on vertex buffer failed with error 0x%x.",
                        hr
                        );
                };

                goto Cleanup;
            };
        }


        //
        // Create and initialize the index buffer.
        //

        {
            D3D10_SUBRESOURCE_DATA InitialData;
            D3D10_BUFFER_DESC DescBuffer;

            InitialData.pSysMem = indices;
            InitialData.SysMemPitch = 0;
            InitialData.SysMemSlicePitch = 0;

            DescBuffer.ByteWidth = m_nNumIndixes * sizeof( WORD );
            DescBuffer.Usage = D3D10_USAGE_IMMUTABLE;
            DescBuffer.BindFlags = D3D10_BIND_INDEX_BUFFER;
            DescBuffer.CPUAccessFlags = 0;
            DescBuffer.MiscFlags = 0;

            hr = pD3DDevice->CreateBuffer
                (
                &DescBuffer,
                &InitialData,
                &m_pIB
                );

            if ( FAILED( hr ) )
            {
                if ( pLoggingGroup )
                {
                    pLoggingGroup->WriteToLog
                        (
                        "CreateBuffer on index buffer failed with error 0x%x.",
                        hr
                        );
                };

                goto Cleanup;
            };
        }


        //
        // Create the pixel shaders.
        //

        //
        // XR_BIAS range is [-0.750,1.250], more or less. These bits are encoded in the 10 bits in the RT
        // so that 0 means -0.750 and 0x1fe means 1.250. Because you are rendering to the 1010102_UNORM RTView,
        // the hardware will normalize the [0,1] range of the data the PS outputs into the [0,0x1fe] range of the RT View.
        // So the PS's output of 0 maps to 0 and 1.0 maps to 0x1fe (near enough).
        // But the scanout interprets [0,0x1fe] as [-0.750,1.250].
        // Thus, to transform your [0,1] texture data (which could be any integer-valued texture format, not just 8888
        // because, again, the values get normalized to [0,1] by the hw before getting into the shader)
        // you need to divide by 2 and then add .375: [0-1] becomes [0-0.5] when you /2, which then becomes [0.375,0.875] when you  +=0.375,
        // which then gets de-normalized and interpreted as XR_BIAS data, i.e. [0,1], i.e. unchanged.
        //

        char PixelShaderXR_BIAS[] =
            "struct PS_INPUT								\n"
            "{												\n"
            "	float4 Position : SV_Position;				\n"
            "	linear float2 Texcoord : Texcoord;			\n"
            "};												\n"
            "struct PS_OUTPUT								\n"
            "{												\n"
            "    float4 Color : SV_Target;					\n"
            "};												\n"
            "sampler defaultSampler : register(s0);			\n"
            "Texture2D myTex : register(t0);				\n"
            "PS_OUTPUT main( in PS_INPUT In )				\n"
            "{												\n"
            "	PS_OUTPUT Out = (PS_OUTPUT)0;				\n"
            "	Out.Color.r = (myTex.Sample(defaultSampler, In.Texcoord).r /2.0) +0.375; \n"
            "	Out.Color.g = (myTex.Sample(defaultSampler, In.Texcoord).g /2.0) +0.375; \n"
            "	Out.Color.b = (myTex.Sample(defaultSampler, In.Texcoord).b /2.0) +0.375; \n"
            "	Out.Color.a = myTex.Sample(defaultSampler, In.Texcoord).a; \n"
            "	return Out;									\n"
            "}												\n";

        hr = LoadShader
            (
            PixelShaderXR_BIAS,
            (void**) &m_pPixelShaderRemoveXR_BIAS,
            "main",
            PixelShaderModel,
            NULL
            );

        if ( FAILED( hr ) )
        {
            if ( pLoggingGroup )
            {
                pLoggingGroup->WriteToLog
                    (
                    "CDeviceWrapper10::CreateTexRenderResources:  LoadShader on pixel shader failed with error 0x%x.",
                    hr
                    );
            };

            goto Cleanup;
        };

        char PixelShader[] =
            "struct PS_INPUT								\n"
            "{												\n"
            "	float4 Position : SV_Position;				\n"
            "	linear float2 Texcoord : Texcoord;			\n"
            "};												\n"
            "struct PS_OUTPUT								\n"
            "{												\n"
            "    float4 Color : SV_Target;					\n"
            "};												\n"
            "sampler defaultSampler : register(s0);			\n"
            "Texture2D myTex : register(t0);				\n"
            "PS_OUTPUT main( in PS_INPUT In )				\n"
            "{												\n"
            "	PS_OUTPUT Out = (PS_OUTPUT)0;				\n"
            "	Out.Color = myTex.Sample(defaultSampler, In.Texcoord); \n"
            "	return Out;									\n"
            "}												\n";

        hr = LoadShader
            (
            PixelShader,
            (void**) &m_pPixelShader,
            "main",
            PixelShaderModel,
            NULL
            );

        if ( FAILED( hr ) )
        {
            if ( pLoggingGroup )
            {
                pLoggingGroup->WriteToLog
                    (
                    "CDeviceWrapper10::CreateTexRenderResources:  LoadShader on pixel shader failed with error 0x%x.",
                    hr
                    );
            };

            goto Cleanup;
        };


        //
        // Create the vertex shader.
        //

        char VertexShader[] =
            "cbuffer cb0 : register(b0)						\n"
            "{												\n"
            "	float4x4 modelViewProj : WORLDVIEWPROJ;		\n"
            "};												\n"
            "struct VS_INPUT								\n"
            "{												\n"
            "	float3 Position : Position;					\n"
            "	float2 Texcoord : Texcoord;					\n"
            //"	float3 Texcoord2 : Texcoord2;				\n"
            "};												\n"
            "struct VS_OUTPUT								\n"
            "{												\n"
            "	linear float4 Position : SV_Position;		\n"
            "	linear float2 Texcoord : Texcoord;			\n"
            //			"	linear float3 Texcoord2 : Texcoord2;	\n"
            "};                                             \n"
            "VS_OUTPUT main( in VS_INPUT In )				\n"
            "{												\n"
            "	VS_OUTPUT Out = (VS_OUTPUT)0;				\n"
            "	Out.Position = mul( modelViewProj, float4( In.Position, 1.0f ) );	\n"
            "	Out.Texcoord = In.Texcoord;					\n"
            "	return Out;									\n"
            "}												\n";

        hr = LoadShader
            (
            VertexShader,
            (void**) &m_pVertexShader,
            "main",
            VertexShaderModel,
            &pCode
            );

        if ( FAILED( hr ) )
        {
            if ( pLoggingGroup )
            {
                pLoggingGroup->WriteToLog
                    (
                    "CDeviceWrapper10::CreateTexRenderResources:  LoadShader on vertex shader failed with error 0x%x.",
                    hr
                    );
            };

            goto Cleanup;
        };


        //
        // Create the input layout.
        //

        const NumVertexElements = 2;
        D3D10_INPUT_ELEMENT_DESC Desc[NumVertexElements];
        Desc[0].InputSlot = 0;
        Desc[0].AlignedByteOffset = 0;
        Desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        Desc[0].SemanticName = "Position";
        Desc[0].SemanticIndex = 0;
        Desc[0].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
        Desc[0].InstanceDataStepRate = 0;

        Desc[1].InputSlot = 0;
        Desc[1].AlignedByteOffset = 16;
        Desc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        Desc[1].SemanticName = "Texcoord";
        Desc[1].SemanticIndex = 0;
        Desc[1].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
        Desc[1].InstanceDataStepRate = 0;

        /*
        Desc[2].InputSlot = 0;
        Desc[2].AlignedByteOffset = 24;
        Desc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
        Desc[2].SemanticName = "Texcoord2";
        Desc[2].SemanticIndex = 0;
        Desc[2].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
        Desc[2].InstanceDataStepRate = 0;
        */

        hr = pD3DDevice->CreateInputLayout
            (
            Desc,
            NumVertexElements,
            pCode->GetBufferPointer(),
            pCode->GetBufferSize(),
            &m_pEL
            );

        pCode->Release();
        pCode = NULL;

        if ( FAILED( hr ) )
        {
            if ( pLoggingGroup )
            {
                pLoggingGroup->WriteToLog
                    (
                    "CreateInputLayout failed with error 0x%x.",
                    hr
                    );
            };

            goto Cleanup;
        };
    };


    //
    // Set the vertex shader resources.
    //

    D3D10_BUFFER_DESC DescBuffer;
    ZeroMemory( &DescBuffer, sizeof( D3D10_BUFFER_DESC ) );
    DescBuffer.ByteWidth = 64;
    DescBuffer.Usage = D3D10_USAGE_DEFAULT;
    DescBuffer.BindFlags = D3D10_BIND_CONSTANT_BUFFER;

    switch( m_RotationMode )
    {
    case DXGI_MODE_ROTATION_UNSPECIFIED :
    case DXGI_MODE_ROTATION_IDENTITY :
        D3DXMatrixIdentity( &matrix );
        break;

    case DXGI_MODE_ROTATION_ROTATE90 :
        D3DXMatrixRotationZ( &matrix, (float)D3DX_PI / 2.0f );
        break;

    case DXGI_MODE_ROTATION_ROTATE180 :
        D3DXMatrixRotationZ( &matrix, (float)D3DX_PI );
        break;

    case DXGI_MODE_ROTATION_ROTATE270 :
        D3DXMatrixRotationZ( &matrix, -(float)D3DX_PI / 2.0f );
        break;

    default:
        {
            assert( FALSE );
        } break;
    };

    D3D10_SUBRESOURCE_DATA InitialData;
    InitialData.pSysMem = &matrix;
    InitialData.SysMemPitch = 0;
    InitialData.SysMemSlicePitch = 0;

    hr = pD3DDevice->CreateBuffer
        (
        &DescBuffer,
        &InitialData,
        &m_pVSBuffer
        );

    if( FAILED( hr ) )
    {
        if ( pLoggingGroup )
        {
            pLoggingGroup->WriteToLog
                (
                "CreateBuffer failed with error 0x%x.",
                hr
                );
        };

        goto Cleanup;
    };

    pD3DDevice->VSSetConstantBuffers
        (
        0,
        1,
        &m_pVSBuffer
        );


    //
    // Create our sampler state object.
    //

    D3D10_SAMPLER_DESC SamplerDesc;
    ZeroMemory( &SamplerDesc, sizeof( D3D10_SAMPLER_DESC ) );
    SamplerDesc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
    SamplerDesc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
    SamplerDesc.MipLODBias = 0;
    SamplerDesc.MaxAnisotropy = 1;
    SamplerDesc.ComparisonFunc = D3D10_COMPARISON_NEVER;
    SamplerDesc.BorderColor[0] = 1.0f;
    SamplerDesc.BorderColor[1] = 1.0f;
    SamplerDesc.BorderColor[2] = 1.0f;
    SamplerDesc.BorderColor[3] = 1.0f;

    SamplerDesc.MinLOD = -FLT_MAX;
    if
        (
        strcmp( VertexShaderModel, "vs_4_0_level_9_1" ) == 0 ||
        strcmp( VertexShaderModel, "vs_4_0_level_9_3" ) == 0
        )
    {
        SamplerDesc.MinLOD = 0.f;
    };

    SamplerDesc.MaxLOD = FLT_MAX;

    hr = pD3DDevice->CreateSamplerState
        (
        &SamplerDesc,
        &m_pSampler
        );

    if ( FAILED( hr ) )
    {
        if ( pLoggingGroup )
        {
            pLoggingGroup->WriteToLog
                (
                "CreateSamplerState failed with error 0x%x.",
                hr
                );
        };

        goto Cleanup;
    };


    //
    // Get prior states.
    //
    //pD3DDevice ->RSGetState(&m_pRastState);
    //pD3DDevice->OMGetBlendState(&m_pBlendState, NULL, NULL);
    //pD3DDevice->OMGetDepthStencilState(&m_pDepthState, NULL);


    //
    // Create our rasterization state object.
    //

    D3D10_RASTERIZER_DESC Rasterizer;
    ZeroMemory( &Rasterizer, sizeof( D3D10_RASTERIZER_DESC ) );
    Rasterizer.AntialiasedLineEnable = false;
    Rasterizer.CullMode = D3D10_CULL_NONE;
    Rasterizer.DepthBias = 0;
    Rasterizer.DepthBiasClamp = 0;
    Rasterizer.FillMode = D3D10_FILL_SOLID;
    Rasterizer.FrontCounterClockwise = false;
    Rasterizer.MultisampleEnable = true;
    Rasterizer.ScissorEnable = false;
    Rasterizer.SlopeScaledDepthBias = 0;
    Rasterizer.DepthClipEnable = true;

    hr = pD3DDevice->CreateRasterizerState
        (
        &Rasterizer,
        &m_pRastState
        );

    if ( FAILED( hr ) )
    {
        if ( pLoggingGroup )
        {
            pLoggingGroup->WriteToLog
                (
                "CreateRasterizerState failed with error 0x%x.",
                hr
                );
        };

        goto Cleanup;
    };


    //
    // Create our blend state object.
    //

    D3D10_BLEND_DESC Blend;
    ZeroMemory( &Blend, sizeof( D3D10_BLEND_DESC ) );
    Blend.AlphaToCoverageEnable = false;
    Blend.BlendOp = D3D10_BLEND_OP_ADD;
    Blend.BlendOpAlpha = D3D10_BLEND_OP_ADD;
    Blend.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;
    Blend.DestBlendAlpha = D3D10_BLEND_INV_SRC_ALPHA;
    Blend.SrcBlend = D3D10_BLEND_SRC_ALPHA;
    Blend.SrcBlendAlpha = D3D10_BLEND_SRC_ALPHA;
    Blend.RenderTargetWriteMask[0] = 0xf;
    Blend.RenderTargetWriteMask[1] = 0xf;
    Blend.RenderTargetWriteMask[2] = 0xf;
    Blend.RenderTargetWriteMask[3] = 0xf;
    Blend.RenderTargetWriteMask[4] = 0xf;
    Blend.RenderTargetWriteMask[5] = 0xf;
    Blend.RenderTargetWriteMask[6] = 0xf;
    Blend.RenderTargetWriteMask[7] = 0xf;

    hr = pD3DDevice->CreateBlendState
        (
        &Blend,
        &m_pBlendState
        );

    if ( FAILED( hr ) )
    {
        if ( pLoggingGroup )
        {
            pLoggingGroup->WriteToLog
                (
                "CreateBlendState failed with error 0x%x.",
                hr
                );
        };

        goto Cleanup;
    };


    //
    // Create our depth stencil states.
    //

    D3D10_DEPTH_STENCIL_DESC Depth;
    ZeroMemory( &Depth, sizeof( D3D10_DEPTH_STENCIL_DESC ) );
    Depth.DepthEnable = false;
    Depth.DepthFunc = D3D10_COMPARISON_LESS;
    Depth.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ALL;
    Depth.StencilEnable = false;
    Depth.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    Depth.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    Depth.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
    Depth.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
    Depth.BackFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
    Depth.BackFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
    Depth.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
    Depth.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
    Depth.StencilReadMask = 0;
    Depth.StencilWriteMask = 0;

    hr = pD3DDevice->CreateDepthStencilState
        (
        &Depth,
        &m_pDepthState
        );

    if ( FAILED( hr ) )
    {
        if ( pLoggingGroup )
        {
            pLoggingGroup->WriteToLog
                (
                "CreateDepthStencilState failed with error 0x%x.",
                hr
                );
        };

        goto Cleanup;
    };


    bReadyForTexRender = true;
    bOverallResult = true;


Cleanup:
    if ( pCode != NULL )
    {
        pCode->Release();
        pCode = NULL;
    };

	if( FAILED( hr ) )
		m_hrLastError = hr;

    return bOverallResult;
};


void CDeviceWrapper10::ReleaseTexRenderResources()
{
    bReadyForTexRender = false;

    if ( m_pVB != NULL )
    {
        if ( pD3DDevice != NULL )
        {
            // First unbind it...
            UINT Strides[] = { 0 };
            UINT Offsets[] = { 0 };
            ID3D10Buffer * pNullBuffer = NULL;
            pD3DDevice->IASetVertexBuffers
                (
                0,
                1,
                &pNullBuffer,
                Strides,
                Offsets
                );
        };

        // ...then release it.
        m_pVB->Release();
        m_pVB = NULL;
    };


    if ( m_pIB != NULL )
    {
        if ( pD3DDevice != NULL )
        {
            // First unbind it...
            pD3DDevice->IASetIndexBuffer
                (
                NULL,
                DXGI_FORMAT_UNKNOWN,
                0
                );
        };

        // ...then release it.
        m_pIB->Release();
        m_pIB = NULL;
    };


    if ( m_pVSBuffer != NULL )
    {
        if ( pD3DDevice != NULL )
        {
            // First unbind it...
            ID3D10Buffer * pNullBuffer = NULL;
            pD3DDevice->VSSetConstantBuffers
                (
                0,
                1,
                &pNullBuffer
                );
        };

        // ...then release it.
        m_pVSBuffer->Release();
        m_pVSBuffer = NULL;
    };


    if ( m_pEL != NULL )
    {
        if ( pD3DDevice != NULL )
        {
            // First unbind it...
            pD3DDevice->IASetInputLayout( NULL );
        };

        // ...then release it.
        m_pEL->Release();
        m_pEL = NULL;
    };


    if ( m_pPixelShaderRemoveXR_BIAS != NULL || m_pPixelShader != NULL)
    {
        if ( pD3DDevice != NULL )
        {
            // First unbind it...
            pD3DDevice->PSSetShader( NULL );
        };
    };

    if ( m_pPixelShader != NULL )
    {
        // ...then release it.
        m_pPixelShader->Release();
        m_pPixelShader = NULL;
    };

    if ( m_pPixelShaderRemoveXR_BIAS != NULL )
    {
        // ...then release it.
        m_pPixelShaderRemoveXR_BIAS->Release();
        m_pPixelShaderRemoveXR_BIAS = NULL;
    };

    if ( m_pVertexShader != NULL )
    {
        if ( pD3DDevice != NULL )
        {
            // First unbind it...
            pD3DDevice->VSSetShader( NULL );
        };

        // ...then release it.
        m_pVertexShader->Release();
        m_pVertexShader = NULL;
    };


    if ( m_pSampler != NULL )
    {
        if ( pD3DDevice != NULL )
        {
            // First unbind it...
            ID3D10SamplerState * pNullSampler = NULL;
            pD3DDevice->PSSetSamplers
                (
                0,
                1,
                &pNullSampler
                );
        };

        // ...then release it.
        m_pSampler->Release();
        m_pSampler = NULL;
    };


    if ( m_pRastState != NULL )
    {
        if ( pD3DDevice != NULL )
        {
            // First unbind it...
            pD3DDevice->RSSetState( NULL );
        };

        // ...then release it.
        m_pRastState->Release();
        m_pRastState = NULL;
    };


    if ( m_pBlendState != NULL )
    {
        m_pBlendState->Release();
        m_pBlendState = NULL;
    };


    if ( m_pDepthState != NULL )
    {
        if ( pD3DDevice != NULL )
        {
            // First unbind it...
            pD3DDevice->OMSetDepthStencilState
                (
                NULL,
                0
                );
        };

        // ...then release it.
        m_pDepthState->Release();
        m_pDepthState = NULL;
    };
};


bool CDeviceWrapper10::RenderTexture
    (
    ID3D10RenderTargetView *pRTView,
    ID3D10ShaderResourceView *pTexSRV,
    const D3D10_VIEWPORT *pD3D10ViewPort,
    BOOL renderXR_BIAS
    )
{
    // Verify that the necessary resources (VBs, shaders, etc.) have been initialized.
    if( !bReadyForTexRender )
    {
        if( pLoggingGroup )
            pLoggingGroup->WriteToLog( "Aborting CDeviceWrapper10::SetBBInitDeviceStates: Resources not initialized.\n" );
        return false;
    }

    // Set the device state.

    {
        UINT Offset = 0;

        pD3DDevice->IASetVertexBuffers( 0, 1, &m_pVB, &m_nStride, &Offset );
    }

    pD3DDevice->IASetIndexBuffer
        (
        m_pIB,
        DXGI_FORMAT_R16_UINT,
        0
        );

    if(renderXR_BIAS)
    {
        pD3DDevice->PSSetShader( m_pPixelShaderRemoveXR_BIAS );
    }
    else
    {
        pD3DDevice->PSSetShader( m_pPixelShader );
    }

    pD3DDevice->VSSetShader( m_pVertexShader );

    pD3DDevice->IASetInputLayout( m_pEL );
    pD3DDevice->IASetPrimitiveTopology
        (
        m_bAlternateRender
        ? D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        : D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP
        );

    if ( !m_bAlternateRender )
    {
        pD3DDevice->PSSetSamplers
            (
            0,
            1,
            &m_pSampler
            );

        // Assign the texture's shader resource view to the zero register.
        pD3DDevice->PSSetShaderResources( 0, 1, &pTexSRV );
    };

    pD3DDevice->RSSetState( m_pRastState );

    float aBlendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

    //#pragma prefast( suppress:6309, "NULL is valid for parameter 2 - SAL needs updating for OMSetBlendState() in d3d10.h" )
    pD3DDevice->OMSetBlendState
        (
        m_pBlendState,
        aBlendFactor,
        D3D10_DEFAULT_SAMPLE_MASK
        );

    pD3DDevice->OMSetDepthStencilState( m_pDepthState, 0 );

    // Set the render target and depth stencil.
    pD3DDevice->OMSetRenderTargets
        (
        1,
        &pRTView,
        NULL
        );

    // Set the view port.
    pD3DDevice->RSSetViewports
        (
        1,
        pD3D10ViewPort
        );

    // Render the texture.
    if( m_bAlternateRender )
    {
        pD3DDevice->DrawIndexed
            (
            m_nNumIndixes,
            0,
            0
            );
    }
    else
    {
        pD3DDevice->Draw
            (
            m_nNumVertices,
            0
            );
    };

    return true;
};


bool CDeviceWrapper10::GetFactory
    (
    IDXGIFactory **pFactoryPointer
    )
{
    HRESULT hresult;
    bool bReturn = false;

    if (!pD3DDevice)
    {
        if (pLoggingGroup)
            pLoggingGroup->WriteToLog("CDeviceWrapper10::GetFactory failed: Device not initialized.\n");
    }
    else
    {
        IDXGIDevice *pDXGIDevice = NULL;

        if (FAILED((hresult = pD3DDevice->QueryInterface(__uuidof(IDXGIDevice), (void **)(&pDXGIDevice)))))
        {
            if (pLoggingGroup)
                pLoggingGroup->WriteToLog("CDeviceWrapper10::GetFactory failed: Unable to retrieve IDXGIDevice interface from D3D10 device.\n");
        }
        else
        {
            IDXGIAdapter *pDeviceAdapter = NULL;

            if (FAILED((hresult = pDXGIDevice->GetAdapter(&pDeviceAdapter))))
            {
                if (pLoggingGroup)
                    pLoggingGroup->WriteToLog("CDeviceWrapper10::GetFactory failed: GetAdapter failed.\n");
            }
            else
            {
                if (SUCCEEDED((hresult = pDeviceAdapter->GetParent(__uuidof(IDXGIFactory), (void **)pFactoryPointer))))
                    bReturn = true;
                else
                    if (pLoggingGroup)
                        pLoggingGroup->WriteToLog("CDeviceWrapper10::GetFactory failed: GetParent failed.\n");

                pDeviceAdapter->Release();
            }

            pDXGIDevice->Release();
        }
    }

    return bReturn;
}


BOOL  CDeviceWrapper10::GetDevicePtr
    (
    ID3D10Device **pDevicePointer
    )
{
    if ( pDevicePointer == NULL )
    {
        return FALSE;
    }

    *pDevicePointer = NULL;

    if ( pD3DDevice == NULL )
    {
        return FALSE;
    }

    pD3DDevice->AddRef();
    *pDevicePointer = pD3DDevice;

    return TRUE;
}


void CDeviceWrapper10::ReleaseAllObjects()
{
    ReleaseTexRenderResources();

    if (pD3DDevice)
    {
        UINT uiRefCount;

        if (uiRefCount = pD3DDevice->Release())
            if (pLoggingGroup)
                pLoggingGroup->WriteToLog("Warning: Non-zero reference count after releasing device in CDeviceWrapper10::ReleaseAllObjects.\n");

        pD3DDevice = NULL;
    }
	m_hrLastError = S_OK;
}


////////////////////////////////////////////////////////////////////////////////
// CSwapChainWrapper10 Methods

CSwapChainWrapper10::~CSwapChainWrapper10()
{
}

BOOL CSwapChainWrapper10::InitializeSwapChain
    (
    DXGI_SWAP_CHAIN_DESC * pSwapChainDesc
    )
{
    //
    // Initialize locals.
    //

    HRESULT hresult = S_OK;
    BOOL bReturn = FALSE;
    char szMsg[ MSG_LEN ] = {0};
    ULONG ulRefCt = 0;

    IDXGIDevice * pDXGIDevice = NULL;
    IDXGIAdapter * pDXGIAdapter = NULL;
    IDXGIFactory * pDXGIFactory = NULL;
    IDXGIFactory2* pDXGIFactory2 = NULL;


    //
    // Validate members and state.
    //

    if ( pLoggingGroup == NULL )
    {
        StringCchPrintfA
            (
            szMsg,
            ARRAYSIZE( szMsg ),
            "%s at line %d.  FAILED: pLoggingGroup is NULL.\n",
            __FILE__,
            __LINE__
            );

        DebugOut.Write( szMsg );

        goto Cleanup;
    };


    if ( pParentD3DDevice == NULL )
    {
        pLoggingGroup->WriteToLog
            (
            "CSwapChainWrapper10::InitializeSwapChain:  Parent device object pointer is NULL."
            );

        goto Cleanup;
    };


    if ( pParentD3DDevice->pD3DDevice == NULL )
    {
        pLoggingGroup->WriteToLog
            (
            "CSwapChainWrapper10::InitializeSwapChain:  Parent device object is not initialized."
            );

        goto Cleanup;
    };


    //
    // Validate parameters.
    //

    if ( pSwapChainDesc == NULL )
    {
        if ( pLoggingGroup )
        {
            pLoggingGroup->WriteToLog
                (
                "CSwapChainWrapper10::InitializeSwapChain:  Swap chain descriptor pointer is NULL."
                );
        };

        goto Cleanup;
    };


    //
    // Release objects that we might be holding.
    //

	hrLastError = S_OK;
    ReleaseAllObjects();


    //
    // Query the device for its IDXGIDevice interface pointer.
    //

    hresult = pParentD3DDevice->pD3DDevice->QueryInterface
        (
        IID_IDXGIDevice,
        (void **) &pDXGIDevice
        );

    if ( FAILED( hresult ) )
    {
        pLoggingGroup->WriteToLog
            (
            "CSwapChainWrapper10::InitializeSwapChain:  QueryInterface on device for IDXGIDevice interface failed with error 0x%x.",
            hresult
            );

        goto Cleanup;
    };

    if ( pDXGIDevice == NULL )
    {
        pLoggingGroup->WriteToLog
            (
            "CSwapChainWrapper10::InitializeSwapChain:  QueryInterface on device for IDXGIDevice succeeded but returned NULL interface pointer."
            );

        goto Cleanup;
    };


    //
    // Get the device's adapter.
    //

    hresult = pDXGIDevice->GetAdapter
        (
        &pDXGIAdapter
        );

    if ( FAILED( hresult ) )
    {
        pLoggingGroup->WriteToLog
            (
            "CSwapChainWrapper10::InitializeSwapChain:  GetAdapter failed with error 0x%x.",
            hresult
            );

        goto Cleanup;
    };

    if ( pDXGIAdapter == NULL )
    {
        pLoggingGroup->WriteToLog
            (
            "CSwapChainWrapper10::InitializeSwapChain:  GetAdapter succeeded but returned a NULL adapter pointer."
            );

        goto Cleanup;
    };


    //
    // Get the adapter's factory.
    //

    hresult = pDXGIAdapter->GetParent
        (
        IID_IDXGIFactory,
        (void **) &pDXGIFactory
        );

    if ( FAILED( hresult ) )
    {
        pLoggingGroup->WriteToLog
            (
            "CSwapChainWrapper10::InitializeSwapChain:  GetParent failed with error 0x%x.",
            hresult
            );

        goto Cleanup;
    };

    if ( pDXGIFactory == NULL )
    {
        pLoggingGroup->WriteToLog
            (
            "CSwapChainWrapper10::InitializeSwapChain:  GetParent succeeded but returned a NULL factory pointer."
            );

        goto Cleanup;
    };


    //
    // Create the swap chain.
    //
    // Are we creating a Windowed or Windowless (DCOMP) swapchain?
    //
    if( pSwapChainDesc->OutputWindow == NULL)
    {
        // Create a Windowless (DCOMP Visual) swapchain
        DXGI_SWAP_CHAIN_DESC1 scd1;
        ZeroMemory(&scd1,sizeof(scd1));
        scd1.Width = pSwapChainDesc->BufferDesc.Width;
        scd1.Height = pSwapChainDesc->BufferDesc.Height;
        scd1.Format = pSwapChainDesc->BufferDesc.Format;
        scd1.Stereo = FALSE;
        scd1.BufferCount = pSwapChainDesc->BufferCount;
        scd1.BufferUsage = pSwapChainDesc->BufferUsage;
        scd1.SampleDesc.Count = pSwapChainDesc->SampleDesc.Count;
        scd1.SampleDesc.Quality = pSwapChainDesc->SampleDesc.Quality;
        scd1.Scaling = DXGI_SCALING_STRETCH;
        scd1.SwapEffect = pSwapChainDesc->SwapEffect;
        scd1.Flags = 0;

        hresult = pDXGIFactory->QueryInterface(__uuidof(IDXGIFactory2),(void**)&pDXGIFactory2);
        if ( FAILED( hresult ) )
        {
            pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitializeSwapChain: DXGIFactory->QueryInterface() for IDXGIFactory2 failed with error 0x%x.", hresult);
            goto Cleanup;
        };

        IDXGISwapChain1* pSC1 = NULL;
        hresult = pDXGIFactory2->CreateSwapChainForComposition(pDXGIDevice, &scd1, NULL, &pSC1);
        if ( FAILED( hresult ) )
        {
            pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitializeSwapChain: pDXGIFactory2->CreateSwapChainForComposition() failed with error 0x%x.", hresult);
            goto Cleanup;
        };

        pSwapChain = pSC1;
    }
    else // Create a regular windowed swapchain
    {

        hresult = pDXGIFactory->CreateSwapChain
            (
            pDXGIDevice,
            pSwapChainDesc,
            &pSwapChain
            );

        if ( FAILED( hresult ) )
        {
            pLoggingGroup->WriteToLog
                (
                "CSwapChainWrapper10::InitializeSwapChain:  CreateSwapChain failed with error 0x%x.",
                hresult
                );

            goto Cleanup;
        };
    } // end Windowed vs. Windowless swapchain creation check.

    if ( pSwapChain == NULL )
    {
        pLoggingGroup->WriteToLog
            (
            "CSwapChainWrapper10::InitializeSwapChain:  CreateSwapChain succeeded but returned a NULL swap chain pointer."
            );

        goto Cleanup;
    };


    bReturn = TRUE;


Cleanup:
    if ( pDXGIDevice != NULL )
    {
        pDXGIDevice->Release();
        pDXGIDevice = NULL;
    };


    if ( pDXGIAdapter != NULL )
    {
        pDXGIAdapter->Release();
        pDXGIAdapter = NULL;
    };


    if ( pDXGIFactory != NULL )
    {
        pDXGIFactory->Release();
        pDXGIFactory = NULL;
    };

    if ( pDXGIFactory2 != NULL )
    {
        pDXGIFactory2->Release();
        pDXGIFactory2 = NULL;
    };

	if( FAILED( hresult ) )
		hrLastError = hresult;
    return bReturn;
};


BOOL  CSwapChainWrapper10::InitializeFullscreenSwapChain
    (
    DXGI_SWAP_CHAIN_DESC* pSwapChainDesc
    )
{
    HRESULT     hresult = S_OK;
    BOOL        bReturn = TRUE;
    char        szMsg[MSG_LEN] = {0};
    ULONG       ulRefCt = 0;


    ReleaseAllObjects();

    if ( pParentD3DDevice == NULL )
    {
        bReturn = FALSE;
		hrLastError = hresult;
        if ( pLoggingGroup != NULL )
        {
            pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitializeFullscreenSwapChain failed: No parent device.\n");
        }
    }
    else
    {
        ID3D10Device   *pD3D10Device = NULL;
        IDXGIDevice    *pDXGIDevice = NULL;

        if ( (pD3D10Device = pParentD3DDevice->pD3DDevice) == NULL )
        {
            bReturn = FALSE;

            if ( pLoggingGroup != NULL )
            {
                pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitializeFullscreenSwapChain failed: Parent device not initialized.\n");
            }
        }
        else
            if ( FAILED ( hresult = pD3D10Device->QueryInterface( __uuidof(IDXGIDevice), (void **)&pDXGIDevice ) ) )
            {
                bReturn = FALSE;
				hrLastError = hresult;
                if ( pLoggingGroup != NULL )
                {
                    pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitializeFullscreenSwapChain failed: Unable to retrieve IDXGIDevice interface from D3D10 device.\n");
                }
            }

            if ( pDXGIDevice == NULL )
            {
                bReturn = FALSE;
            }
            else
            {
                IDXGIAdapter   *pDeviceAdapter = NULL;
                IDXGIFactory   *pParentFactory = NULL;

                if ( FAILED ( hresult = pDXGIDevice->GetAdapter( &pDeviceAdapter ) ) )
                {
                    bReturn = FALSE;
					hrLastError = hresult;
                    if ( pLoggingGroup != NULL )
                    {
                        pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitializeFullscreenSwapChain failed: GetAdapter failed.\n");
                    }
                }
                else
                {
                    if ( FAILED ( hresult = pDeviceAdapter->GetParent( __uuidof(IDXGIFactory), (void **)&pParentFactory ) ) )
                    {
                        bReturn = FALSE;
						hrLastError = hresult;
                        if ( pLoggingGroup != NULL )
                        {
                            pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitializeFullscreenSwapChain failed: GetParent failed.\n");
                        }
                    }

                    if ( ( ulRefCt = pDeviceAdapter->Release() ) != 1 )
                    {
                        bReturn = FALSE;
                        StringCchPrintf(
                            szMsg,
                            sizeof(szMsg),
                            "%s at line %d.  FAILED: IDXGIAdapter::Release() left RefCount = %d.\n",
                            __FILE__,
                            __LINE__,
                            ulRefCt
                            );
                        DebugOut.Write( szMsg );

                        if ( pLoggingGroup != NULL )
                        {
                            pLoggingGroup->WriteToLog( szMsg );
                        }
                    }
                }

                if ( pParentFactory == NULL )
                {
                    bReturn = FALSE;
                }
                else
                {
                    if ( FAILED ( hresult = pParentFactory->CreateSwapChain( pDXGIDevice, pSwapChainDesc, &pSwapChain ) ) )
                    {
                        bReturn = FALSE;
						hrLastError = hresult;
                        if ( pLoggingGroup != NULL )
                        {
                            pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitializeFullscreenSwapChain failed: CreateSwapChain failed.\n");
                        }
                    }

                    if ( ( ulRefCt = pParentFactory->Release() ) != 4 )
                    {
                        bReturn = FALSE;
                        StringCchPrintf(
                            szMsg,
                            sizeof(szMsg),
                            "%s at line %d.  FAILED: IDXGIFactory::Release() left RefCount = %d.\n",
                            __FILE__,
                            __LINE__,
                            ulRefCt
                            );
                        DebugOut.Write( szMsg );

                        if ( pLoggingGroup != NULL )
                        {
                            pLoggingGroup->WriteToLog( szMsg );
                        }
                    }
                }

                pDXGIDevice->Release();     // The RefCount here is too variable to check.  It depends on # display devices and other factors.
            }
    }

    return bReturn;
}


BOOL  CSwapChainWrapper10::GetSwapChainPtr
    (
    IDXGISwapChain **pSwapChainPointer
    )
{
    if ( pSwapChainPointer == NULL )
    {
        return FALSE;
    }

    *pSwapChainPointer = NULL;

    if ( pSwapChain == NULL )
    {
        return FALSE;
    }

    pSwapChain->AddRef();
    *pSwapChainPointer = pSwapChain;

    return TRUE;
}


void CSwapChainWrapper10::ReleaseAllObjects()
{
    if ( pSwapChain != NULL )
    {
        //
        // Restore the swap chain to windowed mode if necessary.
        // We can't arbitrarily set it to windowed mode as the
        // SetFullscreenState() call always fails when called from
        // a windowless (DCOMP bound) swapchain.

        BOOL bFullscreen = TRUE; // Assume we're fullscreen as a precaution.
        // worse that will happen is that the SetFullscreenState
        // call will fail for windowless swapchains.   That's
        // better than crashing if we try to release a swapchain
        // in fullscreen mode.
        //
        // Try to obtain the actual fullscreen state to determine whether or not to reset things
        // to windowed mode.
        //
        HRESULT hr = pSwapChain->GetFullscreenState(&bFullscreen, NULL);
        if (FAILED(hr))
        {
            pLoggingGroup->WriteToLog
                (
                "CSwapChainWrapper10::ReleaseAllObjects:  GettFullscreenState failed with error 0x%x.",
                hr
                );
			hrLastError = hr;
        }

        //
        // If we're in fullscreen mode, switch to windowed before releasing our swapchain.  If we don't we'll crash.
        //
        if (bFullscreen)
        {
            HRESULT hSetFullscreenStateResult = pSwapChain->SetFullscreenState
                (
                FALSE,
                NULL
                );

            if ( FAILED( hSetFullscreenStateResult ) )
            {
                if ( pLoggingGroup )
                {
                    pLoggingGroup->WriteToLog
                        (
                        "CSwapChainWrapper10::ReleaseAllObjects:  SetFullscreenState failed with error 0x%x.",
                        hSetFullscreenStateResult
                        );
                }
				hrLastError = hSetFullscreenStateResult;
            };

        }


        //
        // Release the swap chain.
        //

        UINT uiRefCount = pSwapChain->Release();
        if ( uiRefCount > 0 )
        {
            if ( pLoggingGroup )
            {
                pLoggingGroup->WriteToLog
                    (
                    "CSwapChainWrapper10::ReleaseAllObjects:  WARNING:  Non-zero reference count after releasing swap chain.\n"
                    );
            };
        };


        pSwapChain = NULL;
    };
};


bool CSwapChainWrapper10::InitBBFrom2DTexture
    (
    UINT uiBBIndex,
    __in ID3D10ShaderResourceView *pTexSRV,
    LPCSTR VertexShaderModel,
    LPCSTR PixelShaderModel,
    const HelperType::Vector2D<unsigned>* Dimensions
    )
{
    DXGI_RESULT hresult( pParentD3DDevice ) ;
    bool bReturn = false;

    if (!pSwapChain)
    {
        if (pLoggingGroup)
            pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitBBFrom2DTexture failed: Swap chain not initialized.\n");
    }
    else
        if (!pParentD3DDevice)
        {
            if (pLoggingGroup)
                pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitBBFrom2DTexture failed: No parent device.\n");
        }
        else
        {
            ID3D10Device *pD3D10DeviceFromSC = NULL;

            if (!(pD3D10DeviceFromSC = pParentD3DDevice->pD3DDevice))
            {
                if (pLoggingGroup)
                    pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitBBFrom2DTexture failed: Parent device not initialized.\n");
            }
            else
            {
                ID3D10Device *pD3D10DeviceFromTex = NULL;

                // Verify that the source texture belongs to the device that created the swap
                // chain's back buffers.

                {
                    ID3D10Resource *pTextureResource = NULL;

                    pTexSRV->GetResource(&pTextureResource);

                    if (!pTextureResource)
                    {
                        if (pLoggingGroup)
                            pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitBBFrom2DTexture failed: GetResource on shader resource view returned NULL.\n");
                    }
                    else
                    {
                        pTextureResource->GetDevice(&pD3D10DeviceFromTex);
                        pTextureResource->Release();
                    }
                }

                if (!pD3D10DeviceFromTex)
                {
                    if (pLoggingGroup)
                        pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitBBFrom2DTexture failed: GetDevice on texture returned NULL.\n");
                }
                else
                {
                    if (pD3D10DeviceFromTex != pD3D10DeviceFromSC)
                    {
                        if (pLoggingGroup)
                            pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitBBFrom2DTexture failed: Texture device does not match swap chain device.\n");
                    }
                    else
                    {
                        bool bParentDeviceReady = false;

                        // Verify that the resources necessary for rendering have been
                        // created.

                        if (!(bParentDeviceReady = pParentD3DDevice->IsReadyForTexRender()))
                        {
                            bParentDeviceReady = pParentD3DDevice->CreateTexRenderResources
                                (
                                VertexShaderModel,
                                PixelShaderModel
                                );
                        }

                        if (!bParentDeviceReady)
                        {
                            if (pLoggingGroup)
                                pLoggingGroup->WriteToLog("Aborting CSwapChainWrapper10::InitBBFrom2DTexture: Device resources unavailable.\n");
                        }
                        else
                        {
                            ID3D10Texture2D *pBBAsTexture = NULL;

                            // Obtain an ID3D10Texture2D interface on the back buffer.
                            if (FAILED((hresult = pSwapChain->GetBuffer(uiBBIndex, __uuidof(ID3D10Texture2D), (void **)(&pBBAsTexture)))))
                            {
                                if (pLoggingGroup)
                                    pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitBBFrom2DTexture failed: GetBuffer failed.\n");
								hrLastError = hresult;
                            }
                            else
                            {
                                // Create a render target view based on the back buffer.

                                D3D10_RENDER_TARGET_VIEW_DESC d3d10RTDesc;
                                ID3D10RenderTargetView *pRTView = NULL;

                                DXGI_SWAP_CHAIN_DESC scdescInit;
                                pSwapChain->GetDesc( &scdescInit );


                                d3d10RTDesc.Format = scdescInit.BufferDesc.Format;
                                //d3d10RTDesc.ResourceType = D3D10_RESOURCE_TEXTURE2D;

                                d3d10RTDesc.ViewDimension = scdescInit.SampleDesc.Count > 1? D3D10_RTV_DIMENSION_TEXTURE2DMS : D3D10_RTV_DIMENSION_TEXTURE2D;

                                //d3d10RTDesc.Texture2D.ArraySize = 1;
                                //d3d10RTDesc.Texture2D.FirstArraySlice = 0;
                                d3d10RTDesc.Texture2D.MipSlice = 0;

                                if(scdescInit.BufferDesc.Format == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM)
                                {
                                    d3d10RTDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
                                }

                                if (FAILED((hresult = pD3D10DeviceFromTex->CreateRenderTargetView(pBBAsTexture, &d3d10RTDesc, &pRTView))))
                                {
                                    if (pLoggingGroup)
                                        pLoggingGroup->WriteToLog("CSwapChainWrapper10::InitBBFrom2DTexture failed: CreateRenderTargetView failed.\n");
									hrLastError = hresult;
                                }
                                else
                                {
                                    // Set viewport to span entire back buffer.

                                    D3D10_VIEWPORT d3d10vp;

                                    d3d10vp.TopLeftX = 0;
                                    d3d10vp.TopLeftY = 0;
                                    d3d10vp.MinDepth = 0.0f;
                                    d3d10vp.MaxDepth = 1.0f;

                                    if (Dimensions)
                                    {
                                        d3d10vp.Width = Dimensions->x();
                                        d3d10vp.Height = Dimensions->y();
                                    }
                                    else
                                    {
                                        DXGI_SURFACE_DESC sd = {};

                                        {
                                            CHandle<ID3D10Resource> pBuffer;
                                            pSwapChain->GetBuffer( 0, __uuidof(pBuffer), reinterpret_cast<void**>( &pBuffer ) );
                                            if(pBuffer)
                                            {
                                                CHandle<IDXGISurface> pSurface;
                                                pBuffer->QueryInterface( __uuidof(pSurface), reinterpret_cast<void**>( &pSurface ) );
                                                if (pSurface)
                                                {
                                                    pSurface->GetDesc(&sd);
                                                }
                                            }
                                        }

                                        if( sd.Width && sd.Height )
                                        {
                                            d3d10vp.Width = sd.Width;
                                            d3d10vp.Height = sd.Height;
                                        }
                                        else
                                        {
                                            d3d10vp.Width = ((scdescInit.BufferDesc).Width);
                                            d3d10vp.Height = ((scdescInit.BufferDesc).Height);
                                            // workaround for rotation bug with NONPREROTATED, landscape mode
                                            // the viewport needs to be rotated
                                            if( 0 != ( scdescInit.Flags & DXGI_SWAP_CHAIN_FLAG_NONPREROTATED ) )
                                            {
                                                DXGI_OUTPUT_DESC outputDesc = {};

                                                CHandle<IDXGIOutput> pOutput;
                                                if( SUCCEEDED( pSwapChain->GetContainingOutput( &pOutput ) ) &&
                                                    SUCCEEDED( pOutput->GetDesc( &outputDesc ) ) &&
                                                    Helper::OddRotation(outputDesc.Rotation) )
                                                {
                                                    std::swap(d3d10vp.Width, d3d10vp.Height);
                                                }
                                            }
                                        }
                                    }

                                    DXGI_SWAP_CHAIN_DESC scdescInit;
                                    pSwapChain->GetDesc( &scdescInit );
                                    BOOL renderXR_BIAS = FALSE;
                                    if(scdescInit.BufferDesc.Format == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM)
                                    {
                                        renderXR_BIAS = TRUE;
                                    }

                                    bReturn = pParentD3DDevice->RenderTexture(
                                        pRTView,
                                        pTexSRV,
                                        &d3d10vp,
                                        renderXR_BIAS);

                                    // Unbind the render target view...
                                    ID3D10RenderTargetView * pNullRenderTargetView = NULL;
                                    pD3D10DeviceFromTex->OMSetRenderTargets
                                        (
                                        0,
                                        &pNullRenderTargetView,
                                        NULL
                                        );

                                    // ...and release it.
                                    pRTView->Release();
                                }

                                pBBAsTexture->Release();
                            }
                        }
                    }

                    pD3D10DeviceFromTex->Release();
                }
            }
        }

        return bReturn;
}


bool CSwapChainWrapper10::InitBBViaClear
    (
    UINT uiBBIndex
    )
{
    DXGI_RESULT hresult(pParentD3DDevice, S_OK);
    bool bReturn = false;

    if (!pSwapChain)
    {
        if (pLoggingGroup)
        {
            pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear failed: Swap chain not initialized.\n");
        }
        hresult = E_INVALIDARG;
        goto Cleanup;
    }

    if (!pParentD3DDevice)
    {
        if (pLoggingGroup)
        {
            pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear failed: No parent device.\n");
        }
        hresult = E_INVALIDARG;
        goto Cleanup;
    }

    ID3D10Device *pD3D10DeviceFromSC = NULL;
    if (!(pD3D10DeviceFromSC = pParentD3DDevice->pD3DDevice))
    {
        if (pLoggingGroup)
        {
            pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear failed: Parent device not initialized.\n");
        }
        hresult = E_INVALIDARG;
        goto Cleanup;
    }

    ID3D10Resource * pBuffer = NULL;
    hresult = pSwapChain->GetBuffer(uiBBIndex, __uuidof(pBuffer), reinterpret_cast<void**>(&pBuffer));
    if (FAILED(hresult))
    {
        if (pLoggingGroup)
        {
            pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear failed: GetBuffer failed. hresult=0x%x\n", hresult);
        }
        goto Cleanup;
    }

    IDXGISurface * pSurface = NULL;
    hresult = pBuffer->QueryInterface(__uuidof(pSurface), reinterpret_cast<void**>(&pSurface));
    if (FAILED(hresult))
    {
        if (pLoggingGroup)
        {
            pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear failed: QueryInterface (surface) failed.\n");
        }
        goto Cleanup;
    }

    DXGI_SURFACE_DESC sd;
    ZeroMemory( &sd, sizeof( DXGI_SURFACE_DESC ) );
    pSurface->GetDesc(&sd);

    pSurface->Release();
    pSurface = NULL;


    DXGI_SWAP_CHAIN_DESC scdescInit;
    pSwapChain->GetDesc( &scdescInit );

    D3D10_RENDER_TARGET_VIEW_DESC d3d10RTDesc;
    d3d10RTDesc.Format = scdescInit.BufferDesc.Format;

    if(scdescInit.BufferDesc.Format == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM)
    {
        d3d10RTDesc.Format = DXGI_FORMAT_R10G10B10A2_UNORM;
    }

    d3d10RTDesc.ViewDimension = scdescInit.SampleDesc.Count > 1? D3D10_RTV_DIMENSION_TEXTURE2DMS : D3D10_RTV_DIMENSION_TEXTURE2D;
    d3d10RTDesc.Texture2D.MipSlice = 0;

    ID3D10RenderTargetView * pRTView = NULL;
    hresult = pD3D10DeviceFromSC->CreateRenderTargetView( pBuffer, &d3d10RTDesc, reinterpret_cast<ID3D10RenderTargetView**> (&pRTView) );

    pBuffer->Release();
    pBuffer = NULL;

    if (FAILED(hresult))
    {
        if (pLoggingGroup)
        {
            pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear failed: CreateRenderTargetView failed. hresult=0x%x\n", hresult);
        }
        goto Cleanup;
    }

    pD3D10DeviceFromSC->OMSetRenderTargets(1, &pRTView, 0);

    D3D10_VIEWPORT Viewport;
    ZeroMemory( &Viewport, sizeof( Viewport ) );

    Viewport.TopLeftX = 0;
    Viewport.TopLeftY = 0;
    Viewport.Width = sd.Width;
    Viewport.Height = sd.Height;
    Viewport.MinDepth = 0.0f;
    Viewport.MaxDepth = 1.0f;
    pD3D10DeviceFromSC->RSSetViewports(1, &Viewport);

    float ClearColor[] = {0.7f, 0.2f, 0.4f, 0.9f};
    pD3D10DeviceFromSC->ClearRenderTargetView(pRTView, ClearColor);

    //
    // Clean up RTView by binding NULL and releasing
    //
    pD3D10DeviceFromSC->OMSetRenderTargets(0, 0, 0);
    pRTView->Release();
    pRTView = NULL;

    if(FAILED(hresult))
    {
        if (pLoggingGroup)
        {
            pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear failed: ClearRenderTargetView failed with hresult = 0x%x.\n", hresult);
        }
        goto Cleanup;
    }

    hresult = pSwapChain->Present( 0, 0 );
    if(FAILED(hresult))
    {
        if (pLoggingGroup)
        {
            pLoggingGroup->WriteToLog("clsDXGISwapChain::InitBBViaClear failed: Present failed with hresult = 0x%x.\n", hresult);
        }
        goto Cleanup;
    }


Cleanup:

    if( SUCCEEDED( hresult ))
    {
        bReturn = true;
    }
	else
		hrLastError = hresult;

    return bReturn;
}

void CSwapChainWrapper10::WrapSwapchain
    (
    __in IDXGISwapChain     *pD3DSwapChain
    )
{
    pSwapChain = pD3DSwapChain;

    if ( pSwapChain != NULL )
    {
        pSwapChain->AddRef();
    };
};

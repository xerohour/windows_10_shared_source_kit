//
// interaction.cpp
//
#include "WGFCompute.h"
#include "d3dx11.h"


//---------------------------------------------------------------------------------------------------------------------------------
tstring __stdcall ToString( CInteractionTest::e_WriteMethod value )
{
    switch( value )
    {
    case CInteractionTest::TypedUAVWrite  : return tstring( _T( "TypedUAVWrite" ) );
    case CInteractionTest::RawUAVWrite    : return tstring( _T( "RawUAVWrite" ) );
    case CInteractionTest::StructUAVWrite : return tstring( _T( "StructUAVWrite" ) );
    case CInteractionTest::StreamOutWrite : return tstring( _T( "StreamOutWrite" ) );
    case CInteractionTest::RTWrite        : return tstring( _T( "RTWrite" ) );
    default:
        return tstring( _T( "" ) );
        break;
    }
}
bool __stdcall FromString( CInteractionTest::e_WriteMethod *value, const tstring &str )
{
    if( tcistring( str ) == _T( "TypedUAVWrite" ) )  { *value = CInteractionTest::TypedUAVWrite; return true; }
    if( tcistring( str ) == _T( "RawUAVWrite" ) )    { *value = CInteractionTest::RawUAVWrite; return true; }
    if( tcistring( str ) == _T( "StructUAVWrite" ) ) { *value = CInteractionTest::StructUAVWrite; return true; }
    if( tcistring( str ) == _T( "StreamOutWrite" ) ) { *value = CInteractionTest::StreamOutWrite; return true; }
    if( tcistring( str ) == _T( "RTWrite" ) )        { *value = CInteractionTest::RTWrite; return true; }
    return false;
}

//---------------------------------------------------------------------------------------------------------------------------------
tstring __stdcall ToString( CInteractionTest::e_ReadMethod value )
{
    switch( value )
    {
    case CInteractionTest::TypedUAVRead  : return tstring( _T( "TypedUAVRead" ) );
    case CInteractionTest::RawUAVRead    : return tstring( _T( "RawUAVRead" ) );
    case CInteractionTest::StructUAVRead : return tstring( _T( "StructUAVRead" ) );
    case CInteractionTest::TypedSRVRead  : return tstring( _T( "TypedSRVRead" ) );
    case CInteractionTest::RawSRVRead    : return tstring( _T( "RawSRVRead" ) );
    case CInteractionTest::StructSRVRead : return tstring( _T( "StructSRVRead" ) );
    case CInteractionTest::VBRead        : return tstring( _T( "VBRead" ) );
    case CInteractionTest::IBRead        : return tstring( _T( "IBRead" ) );
    default:
        return tstring( _T( "" ) );
        break;
    }
}
bool __stdcall FromString( CInteractionTest::e_ReadMethod *value, const tstring &str )
{
    if( tcistring( str ) == _T( "TypedUAVRead" ) ) { *value = CInteractionTest::TypedUAVRead; return true; }
    if( tcistring( str ) == _T( "RawUAVRead" ) )   { *value = CInteractionTest::RawUAVRead; return true; }
    if( tcistring( str ) == _T( "StructUAVRead" ) ){ *value = CInteractionTest::StructUAVRead; return true; }
    if( tcistring( str ) == _T( "TypedSRVRead" ) ) { *value = CInteractionTest::TypedSRVRead; return true; }
    if( tcistring( str ) == _T( "RawSRVRead" ) )   { *value = CInteractionTest::RawSRVRead; return true; }
    if( tcistring( str ) == _T( "StructSRVRead" ) ){ *value = CInteractionTest::StructSRVRead; return true; }
    if( tcistring( str ) == _T( "VBRead" ) )       { *value = CInteractionTest::VBRead;  return true; }
    if( tcistring( str ) == _T( "IBRead" ) )       { *value = CInteractionTest::IBRead;  return true; }
    return false;
}

//---------------------------------------------------------------------------------------------------------------------------------
tstring __stdcall ToString( CInteractionTest::e_ResourceType value )
{
    switch( value )
    {
    case CInteractionTest::StructuredBuffer  : return tstring( _T( "StructuredBuffer" ) );
    case CInteractionTest::RawBuffer         : return tstring( _T( "RawBuffer" ) );
    case CInteractionTest::TypedBuffer       : return tstring( _T( "TypedBuffer" ) );
    case CInteractionTest::Texture1D         : return tstring( _T( "Texture1D" ) );
    case CInteractionTest::Texture1DArray    : return tstring( _T( "Texture1DArray" ) );
    case CInteractionTest::Texture2D         : return tstring( _T( "Texture2D" ) );
    case CInteractionTest::Texture2DArray    : return tstring( _T( "Texture2DArray" ) );
    case CInteractionTest::Texture3D         : return tstring( _T( "Texture3D" ) );
    default:
        return tstring( _T( "" ) );
        break;
    }
}
bool __stdcall FromString( CInteractionTest::e_ResourceType *value, const tstring &str )
{
    if( tcistring( str ) == _T( "StructuredBuffer"  ) ) { *value = CInteractionTest::StructuredBuffer; return true; }
    if( tcistring( str ) == _T( "RawBuffer"         ) ) { *value = CInteractionTest::RawBuffer; return true; }
    if( tcistring( str ) == _T( "TypedBuffer"       ) ){ *value = CInteractionTest::TypedBuffer; return true; }
    if( tcistring( str ) == _T( "Texture1D"         ) ){ *value = CInteractionTest::Texture1D; return true; }
    if( tcistring( str ) == _T( "Texture1DArray"    ) ){ *value = CInteractionTest::Texture1DArray; return true; }
    if( tcistring( str ) == _T( "Texture2D"         ) ){ *value = CInteractionTest::Texture2D; return true; }
    if( tcistring( str ) == _T( "Texture2DArray"    ) ){ *value = CInteractionTest::Texture2DArray; return true; }
    if( tcistring( str ) == _T( "Texture3D"         ) ){ *value = CInteractionTest::Texture3D; return true; }
    return false;
}

//---------------------------------------------------------------------------------------------------------------------------------
tstring __stdcall ToString( CInteractionTest::e_Stage value )
{
    switch( value )
    {
    case CInteractionTest::StreamOutStage   : return tstring( _T( "StreamOutStage" ) );
    case CInteractionTest::InputAssembler   : return tstring( _T( "InputAssembler" ) );
    case CInteractionTest::VertexShader     : return tstring( _T( "VertexShader" ) );
    case CInteractionTest::ComputeShader    : return tstring( _T( "ComputeShader" ) );
    case CInteractionTest::PixelShader      : return tstring( _T( "PixelShader" ) );
    default:
        return tstring( _T( "" ) );
        break;
    }
}
bool __stdcall FromString( CInteractionTest::e_Stage *value, const tstring &str )
{
    if( tcistring( str ) == _T( "StreamOutStage"    ) ) { *value = CInteractionTest::StreamOutStage; return true; }
    if( tcistring( str ) == _T( "InputAssembler"    ) ) { *value = CInteractionTest::InputAssembler; return true; }
    if( tcistring( str ) == _T( "VertexShader"      ) ) { *value = CInteractionTest::VertexShader; return true; }
    if( tcistring( str ) == _T( "ComputeShader"     ) ){ *value = CInteractionTest::ComputeShader; return true; }
    if( tcistring( str ) == _T( "PixelShader"       ) ){ *value = CInteractionTest::PixelShader; return true; }
    return false;
}

//---------------------------------------------------------------------------------------------------------------------------------
void ThrowTRFailure( HRESULT hr, const char * Message)
{
    if (FAILED(hr))
    {
        TRException exc(Message);
        char hrMsg[1000];
        StringCchPrintf(hrMsg, ARRAYSIZE(hrMsg), " hr = %08x, " );
        exc.Prepend(hrMsg);
        throw exc;
    }
}

//---------------------------------------------------------------------------------------------------------------------------------
void CompileShader(LPCSTR pResource, LPCSTR pFunctionName, LPCSTR pProfile, UINT uFlags, ID3D10Blob **ppShader) //throw TRException
{
    assert(ppShader);

    ID3D10Blob *pError;
    HRESULT hr = D3DX10CompileFromResource( NULL, pResource, NULL, NULL, NULL, pFunctionName, pProfile, uFlags | D3D10_SHADER_DEBUG, 0, NULL, ppShader, &pError, NULL );

    if( pError )
        CD3D11Test::WriteToLog( _T( "%s" ), pError->GetBufferPointer() );

    if( FAILED( hr ) )
    {
        CD3D11Test::WriteToLog( _T( "CompileShaderFromResource - result code '%s'" ), D3DHResultToString( hr ).c_str() );
        CD3D11Test::WriteToLog( _T( "Shader file '%s', entry point '%s'" ), pResource, pFunctionName );
        SAFE_RELEASE( (*ppShader) );
        SAFE_RELEASE( pError );

        ThrowTRFailure( hr, "[CInteractionTest::SetupShaders] CompileShaderFromResource Failed.");
    }
}

//---------------------------------------------------------------------------------------------------------------------------------
TEST_RESULT CInteractionTest::Setup()
{
    TEST_RESULT tRes = RESULT_PASS;

    // checking Compute support first
    if ( GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0)
    {
        D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwOptions;
        ZeroStructMem(hwOptions);

        HRESULT hr = GetFramework()->GetDevice()->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwOptions, sizeof(hwOptions) );
        if (FAILED(hr))
        {
            WriteToLog( _T("CheckFeatureSupport(D3D10_X_HARDWARE_OPTIONS) failed, hr=%s"), D3DHResultToString(hr).c_str() );
            return RESULT_FAIL;
        }
        else
        {
            if (!hwOptions.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x)
            {
                WriteToLog( _T("Skipping, as device does not support SM 4.x compute features") );
                return RESULT_SKIP;
            }
        }
    }

    try
    {
        //-------------------------------------------------------------------------------------------------------------------------
        // The shaders need to know how big the test resource is. Note, that to get arouind the cs_4_0 limitation that only one
        // UAV can be bound, the width of a a buffer may be doubled, so that the CS can write its results in one area and read
        // from another.
        D3D11_BUFFER_DESC DescBuffer;

        DescBuffer.ByteWidth            = (sizeof(SResourceDimensionConstants)+15)/16 * 16;
        DescBuffer.Usage                = D3D11_USAGE_DYNAMIC;
        DescBuffer.BindFlags            = D3D11_BIND_CONSTANT_BUFFER;
        DescBuffer.CPUAccessFlags       = D3D11_CPU_ACCESS_WRITE;
        DescBuffer.MiscFlags            = 0;
        DescBuffer.StructureByteStride  = 0;

        ThrowTRFailure( GetFramework()->CreateBufferTiledOptional(&DescBuffer, 0, &m_pResourceDimensionConstants),
            "Couldn't create resource dimensions constant buffer");

        //-------------------------------------------------------------------------------------------------------------------------
        //
        // Here we build a VB with two sets of coordinates for each element of any resource that we hope to read out via a 
        // UAV or SRV and then write into the pixel shader's target volume RT. 
        //
        // The first set of coordinates is the three-dimensional RT coordinates of each element of the output volume texture.
        // This coordinate has xy in the [-1,1]x[-1,1] RT space of the output resource with the third coordinate, 
        // being MRT index, as an integral value in [0,D]
        //
        // The second set of coordinates is the scaled resource coordinate in [0,1]x[0,1]x[0,1] for reading through a UAV or SRV.
        //
        // Both coordinates are made available to the PS and VS, allowing either stage to read a UAV or just pass-through
        // depending on the test.
        //

        SIZE_T SizeOfFloatsForPointSampleLocations =  sizeof(PreComputedElementCoordinates) * m_MaxWidth * m_MaxHeight * m_MaxDepth;
        WindowsTest::CAutoBytePtr Float4s(SizeOfFloatsForPointSampleLocations);
        PreComputedElementCoordinates * pPrecomputed = (PreComputedElementCoordinates*) Float4s.p();

        for( UINT w = 0;  w < m_MaxWidth; ++ w)
        {
            for( UINT h = 0;  h < m_MaxHeight; ++ h)
            {
                for( UINT d = 0;  d < m_MaxDepth; ++ d)
                {
                    UINT iPoint =  w + h*m_MaxWidth + d*m_MaxWidth*m_MaxHeight;

                    pPrecomputed[ iPoint ].RawPointCoordinates.x = (float) w;
                    pPrecomputed[ iPoint ].RawPointCoordinates.y = (float) h;
                    pPrecomputed[ iPoint ].RawPointCoordinates.z = (float) d;

                    pPrecomputed[ iPoint ].PointSamplePosition.x = ((float) w + 0.5f) / ((float)m_MaxWidth*(float)m_SizeMultiplier);
                    pPrecomputed[ iPoint ].PointSamplePosition.y = ((float) h + 0.5f) / (float)m_MaxHeight;
                    pPrecomputed[ iPoint ].PointSamplePosition.z = ((float) d + 0.5f) / (float)m_MaxDepth;

                    pPrecomputed[ iPoint ].TargetPointPosition.x = ((float) w / (float)m_MaxWidth )*2.f -1.f + 1/(float)m_MaxWidth;   //The 1/W offsets to the middle of the pixel
                    pPrecomputed[ iPoint ].TargetPointPosition.y = -( ((float) h / (float)m_MaxHeight)*2.f -1.f + 1/(float)m_MaxHeight);   //The 1/W offsets to the middle of the pixel
                    pPrecomputed[ iPoint ].TargetPointPosition.z = (float) d; //This is rendertarget index so it stays as an integral value
                }
            }
        }

        DescBuffer.ByteWidth            = (UINT) SizeOfFloatsForPointSampleLocations;
        DescBuffer.Usage                = D3D11_USAGE_IMMUTABLE;
        DescBuffer.BindFlags            = D3D11_BIND_VERTEX_BUFFER;
        DescBuffer.CPUAccessFlags       = 0;
        DescBuffer.MiscFlags            = 0;
        DescBuffer.StructureByteStride  = 0;

        D3D11_SUBRESOURCE_DATA srd = { pPrecomputed, 0, 0 };

        ThrowTRFailure( GetFramework()->CreateBufferTiledOptional(&DescBuffer, &srd, &m_pPointSampleLocations),
            "Couldn't create resource dimensions constant buffer");

        assert( m_BufferTypedFormat == DXGI_FORMAT_R32_UINT );
        BYTE DummyData[4] = { 0, 0, 0, 0 };
        srd.pSysMem = DummyData;
        DescBuffer.ByteWidth            = sizeof(DummyData);
        ThrowTRFailure( GetFramework()->CreateBufferTiledOptional(&DescBuffer, &srd, &m_pDummyDataBuffer),
            "Couldn't create resource dimensions constant buffer");

        //-------------------------------------------------------------------------------------------------------------------------
        // Create all shaders and vertex layouts
        WindowsTest::CHandle<ID3D10Blob> pCode;
        CompileShader( "interaction.sh", "VSMain", "vs_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreateVertexShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pVS ),
            "Couldn't create VSMain shader");

        assert( m_BufferTypedFormat == DXGI_FORMAT_R32_UINT ); //For DataFromComputeBuffer
        const D3D11_INPUT_ELEMENT_DESC SamplePoint[] = 
        {
            { "PointSamplePosition",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
            { "TargetPointPosition",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
            { "RawPointCoordinates",  0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D10_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },  
            { "DataFromComputeBuffer",0, DXGI_FORMAT_R8G8B8A8_UINT,      1, 0,                            D3D11_INPUT_PER_VERTEX_DATA, 0 },  
        };
        ThrowTRFailure(GetFramework()->GetDevice()->CreateInputLayout(
            SamplePoint, ARRAYSIZE(SamplePoint),
            (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(),
            & m_InputLayout),
            "Couldn't create input layout for point sample buffer");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "VSSRV1DSample", "vs_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreateVertexShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pVSSRV1DSample ),
            "Couldn't create VSSRV1DSample shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "VSSRV1DSampleArray", "vs_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreateVertexShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pVSSRV1DSampleArray ),
            "Couldn't create VSSRV1DSampleArray shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "VSSRV2DSample", "vs_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreateVertexShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pVSSRV2DSample ),
            "Couldn't create VSSRV2DSample shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "VSSRV2DSampleArray", "vs_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreateVertexShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pVSSRV2DSampleArray ),
            "Couldn't create VSSRV2DSampleArray shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "VSSRV3DSample", "vs_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreateVertexShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pVSSRV3DSample ),
            "Couldn't create VSSRV3DSample shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "VSSRVBufferSample", "vs_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreateVertexShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pVSSRVBufferSample ),
            "Couldn't create VSSRVBufferSample shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "VSSampleStructured", "vs_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreateVertexShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pVSSampleStructured ),
            "Couldn't create VSSampleStructured shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "VSSampleRawSRV", "vs_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreateVertexShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pVSSampleRawSRV ),
            "Couldn't create VSSampleRawSRV shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "GSMain", "gs_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreateGeometryShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pGS ),
            "Couldn't create geometry shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "PSSRVBufferSample", "ps_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSSRVBufferSample ),
            "Couldn't create PSSRVBufferSample shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "PSSRV1DSample", "ps_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSSRV1DSample ),
            "Couldn't create PSSRV1DSample shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "PSSRV2DSample", "ps_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSSRV2DSample ),
            "Couldn't create PSSRV2DSample shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "PSSRV1DArraySample", "ps_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSSRV1DArraySample ),
            "Couldn't create PSSRV1DArraySample shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "PSSRV2DArraySample", "ps_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSSRV2DArraySample ),
            "Couldn't create PSSRV2DArraySample shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "PSSRV3DSample", "ps_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSSRV3DSample ),
            "Couldn't create PSSRV3DSample shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "PSPassthrough", "ps_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSPassthrough ),
            "Couldn't create PSPassthrough shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "PSSampleStructured", "ps_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSSampleStructured ),
            "Couldn't create PSSampleStructured shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "PSSampleRawSRV", "ps_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSSampleRawSRV ),
            "Couldn't create PSSampleRawSRV shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "CSUavStructuredBuffer", "cs_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUAVStructuredBuffer ),
            "Couldn't create CSUavStructuredBuffer shader");

        CompileShader( "interaction.sh", "CSUavReadStructuredBuffer", "cs_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUavReadStructuredBuffer ),
            "Couldn't create CSUavReadStructuredBuffer shader");

        pCode = 0; //releases
        CompileShader( "interaction.sh", "CSUavReadRawBuffer", "cs_4_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUavReadRawBuffer ),
            "Couldn't create CSUavReadRawBuffer shader");

        if ( GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
        {
            pCode = 0; //releases
            CompileShader( "interaction.sh", "PSUAVStructuredBuffer", "ps_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSUAVStructuredBuffer ),
                "Couldn't create structured sampling pixel shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "PSUAVTypedBuffer", "ps_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSUAVTypedBuffer ),
                "Couldn't create PSUAVTypedBuffer shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "PSUAV1D", "ps_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSUAV1D ),
                "Couldn't create PSUAV1D shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "PSUAV1DArray", "ps_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSUAV1DArray ),
                "Couldn't create PSUAV1DArray shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "PSUAV2D", "ps_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSUAV2D ),
                "Couldn't create PSUAV2D shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "PSUAV2DArray", "ps_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSUAV2DArray ),
                "Couldn't create PSUAV2DArray shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "PSUAV3D", "ps_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSUAV3D ),
                "Couldn't create PSUAV3D shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "PSWriteRTArray", "ps_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSWriteRTArray ),
                "Couldn't create PSWriteRTArray shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "PSWriteRT", "ps_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreatePixelShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pPSWriteRT ),
                "Couldn't create PSWriteRT shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "CSUavRawBuffer", "cs_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUavRawBuffer ),
                "Couldn't create CSUavRawBuffer shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "CSUavTypedBuffer", "cs_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUAVTypedBuffer ),
                "Couldn't create CSUavTypedBuffer shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "CSUavTexture1D", "cs_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUavTexture1D ),
                "Couldn't create CSUavTexture1D shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "CSUavTexture1DArray", "cs_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUavTexture1DArray ),
                "Couldn't create CSUavTexture1DArray shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "CSUavTexture2D", "cs_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUavTexture2D ),
                "Couldn't create CSUavTexture2D shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "CSUavTexture2DArray", "cs_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUavTexture2DArray ),
                "Couldn't create CSUavTexture2DArray shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "CSUavTexture3D", "cs_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUavTexture3D ),
                "Couldn't create CSUavTexture3D shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "CSUavReadTypedBuffer", "cs_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUavReadTypedBuffer ),
                "Couldn't create CSUavReadTypedBuffer shader");

            pCode = 0; //releases
            pCode = 0; //releases
            CompileShader( "interaction.sh", "CSUavReadTexture1D", "cs_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUavReadTexture1D ),
                "Couldn't create CSUavReadTexture1D shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "CSUavReadTexture1DArray", "cs_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUavReadTexture1DArray ),
                "Couldn't create CSUavReadTexture1DArray shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "CSUavReadTexture2D", "cs_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUavReadTexture2D ),
                "Couldn't create CSUavReadTexture2D shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "CSUavReadTexture2DArray", "cs_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUavReadTexture2DArray ),
                "Couldn't create CSUavReadTexture2DArray shader");

            pCode = 0; //releases
            CompileShader( "interaction.sh", "CSUavReadTexture3D", "cs_5_0", 0, & pCode ); //throw
            ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pCSUavReadTexture3D ),
                "Couldn't create CSUavReadTexture3D shader");
        }

        //-------------------------------------------------------------------------------------------------------------------------
        // The test writes something, then reads it, then needs somewhere to write that last value so it can be validated.
        // On CS_4_0, this volume is never written by the CS, so we can turn off the UAV in that case (in fact we have to-
        // UAVs aren't supported in CS_4_0 on 3d textures).

        D3D11_TEXTURE3D_DESC Desc3D;

        Desc3D.Width = m_MaxWidth;
        Desc3D.Height = m_MaxHeight;
        Desc3D.Depth = m_MaxDepth;
        Desc3D.MipLevels = 1;
        Desc3D.Format = m_BufferTypelessFormat;
        Desc3D.Usage = D3D11_USAGE_DEFAULT;
        Desc3D.BindFlags = 
            D3D11_BIND_RENDER_TARGET | 
            ( ( GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 ) ? D3D11_BIND_UNORDERED_ACCESS : 0 );
        Desc3D.CPUAccessFlags = 0;
        Desc3D.MiscFlags = 0; //D3D11_RESOURCE_MISC_TEXTURECUBE;

        ThrowTRFailure( GetFramework()->CreateTexture3DMapDefaultOptional(&Desc3D, 0, &m_pDefaultOutputResource),
            "Couldn't create output tex3d resource");

        Desc3D.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
        Desc3D.Usage = D3D11_USAGE_STAGING;
        Desc3D.BindFlags = 0;

        ThrowTRFailure( GetFramework()->CreateTexture3DMapDefaultOptional(&Desc3D, 0, &m_pStagingOutputResource),
            "Couldn't create output tex3d staging resource");

        //Make a staging version of the output resource that we can fill with default values for quickly resetting the 
        //output volume with a known-bad pattern, so that Validate can tell if the PS failed to write or if the PS
        //did write, but wrote bad values.

        assert( m_BufferTypedFormat == DXGI_FORMAT_R32_UINT);
        SIZE_T SizeofInitialValues =  sizeof(BYTE) * 4 * m_MaxWidth * m_MaxHeight * m_MaxDepth;
        WindowsTest::CAutoBytePtr InitialValues(SizeofInitialValues);
        BYTE * pInitial = (BYTE*) InitialValues.p();

        for( UINT i = 0;  i <  SizeofInitialValues/sizeof(* pInitial); ++ i)
        {
            pInitial[i] = m_OutputBufferInitialValue;
        }

        srd.pSysMem = pInitial;
        srd.SysMemPitch = m_MaxWidth*sizeof(* pInitial)*4;
        srd.SysMemSlicePitch = srd.SysMemPitch*m_MaxHeight;

        ThrowTRFailure( GetFramework()->CreateTexture3DMapDefaultOptional(&Desc3D, &srd, &m_pStagingOutputResourceWithInitialValues),
            "Couldn't create output tex3d staging resource for initial values");

        //-------------------------------------------------------------------------------------------------------------------------
        // Create views for the validation buffer created above
        if ( GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
        {
            D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
        
            uavd.Format                 = m_BufferTypedFormat;
            uavd.ViewDimension          = D3D11_UAV_DIMENSION_TEXTURE3D;
            uavd.Texture3D.MipSlice     = 0;
            uavd.Texture3D.FirstWSlice  = 0;
            uavd.Texture3D.WSize        = m_MaxDepth;

            ThrowTRFailure( GetFramework()->GetDevice()->CreateUnorderedAccessView(m_pDefaultOutputResource, &uavd, & m_pOutputResourceUAV),
                "Couldn't create UAV for test buffer");
        }

        D3D11_RENDER_TARGET_VIEW_DESC rtdv;
        rtdv.Format                 = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtdv.ViewDimension          = D3D11_RTV_DIMENSION_TEXTURE3D;
        rtdv.Texture3D.FirstWSlice  = 0;
        rtdv.Texture3D.MipSlice     = 0;
        rtdv.Texture3D.WSize        = m_MaxDepth;

        ThrowTRFailure( GetFramework()->GetDevice()->CreateRenderTargetView(m_pDefaultOutputResource, &rtdv, &m_pOutputResourceRTV),
            "Couldn't create output tex3d RTV");

        GetFramework()->GetEffectiveContext()->OMSetRenderTargets( 1, &m_pOutputResourceRTV, 0 );
    }
    catch(TRException &exc)
    {
        tRes = exc.GetResult();
        exc.Prepend("[CInteractionBufferTest::Setup()] ");
        if (tRes != RESULT_PASS)
            WriteToLog( exc.GetError().c_str() );
    }

    //We do as much device init as possible here in Setup, thus allowing more state to go stale when the driver switches pipeline 
    //state back and forth between compute and graphics

    GetFramework()->GetEffectiveContext()->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
    UINT Stride = sizeof(PreComputedElementCoordinates);
    UINT Offset = 0;
    GetFramework()->GetEffectiveContext()->IASetVertexBuffers( 0, 1, & m_pPointSampleLocations, & Stride, & Offset );
    Stride = 0;
    GetFramework()->GetEffectiveContext()->IASetVertexBuffers( 1, 1, & m_pDummyDataBuffer, & Stride, & Offset );
    GetFramework()->GetEffectiveContext()->IASetInputLayout( m_InputLayout );
    GetFramework()->GetEffectiveContext()->GSSetShader( m_pGS, NULL, 0 );
    GetFramework()->GetEffectiveContext()->PSSetConstantBuffers( 0, 1, & m_pResourceDimensionConstants );
    GetFramework()->GetEffectiveContext()->VSSetConstantBuffers( 0, 1, & m_pResourceDimensionConstants );
    GetFramework()->GetEffectiveContext()->CSSetConstantBuffers( 0, 1, & m_pResourceDimensionConstants );
    GetFramework()->GetEffectiveContext()->OMSetRenderTargets( 1, &m_pOutputResourceRTV, 0 );
    float BlendFactor[] = { 0.f, 0.f, 0.f, 0.f };
    GetFramework()->GetEffectiveContext()->OMSetBlendState( 0, BlendFactor, D3D11_DEFAULT_SAMPLE_MASK );
    D3D11_VIEWPORT vp = { 0.f, 0.f, (float) m_MaxWidth, (float) m_MaxHeight, 0.f, 1.f };
    GetFramework()->GetEffectiveContext()->RSSetViewports( 1, &vp );

    ThrowTRFailure( ExecuteEffectiveContext(), "ExecuteEffectiveContext() failed." );

    return tRes;
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::Cleanup()
{
    m_pResourceDimensionConstants = 0;
    m_pVS = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::UpdateResourceDimensionCB()
{
    D3D11_MAPPED_SUBRESOURCE msr;
    ThrowTRFailure(  GetFramework()->GetEffectiveContext()->Map( m_pResourceDimensionConstants, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr ),
        "Couldn't map resource dimension CB");

    SResourceDimensionConstants * p = reinterpret_cast<SResourceDimensionConstants*> ( msr.pData );

    p->Divisor = 2;
    p->w = m_ResourceWidth == 1 ? 1 : m_ResourceWidth*m_SizeMultiplier;
    p->h = m_ResourceHeight == 1 ? 1 : m_ResourceHeight;
    p->d = m_ResourceDepth == 1 ? 1 : m_ResourceDepth;
    p->ArrayIndex = m_CurrentArraySlice;

    GetFramework()->GetEffectiveContext()->Unmap( m_pResourceDimensionConstants, 0);
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::CreateBuffer( UINT Bind, bool bStructured, bool bRaw  )
{
    m_bStructured = bStructured;
    m_bRaw = bRaw;

    D3D11_BUFFER_DESC DescBuffer;

    DescBuffer.ByteWidth            = (m_MaxWidth*m_SizeMultiplier + m_BufferUnusedLeadingElements) * m_BufferElementSize;
    DescBuffer.Usage                = D3D11_USAGE_DEFAULT;
    DescBuffer.BindFlags            = Bind;
    DescBuffer.CPUAccessFlags       = 0;
    DescBuffer.MiscFlags            = 0;
    DescBuffer.MiscFlags            |= m_bStructured ? D3D11_RESOURCE_MISC_BUFFER_STRUCTURED : 0;
    DescBuffer.MiscFlags            |= m_bRaw ? D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS : 0;
    DescBuffer.StructureByteStride  = m_bStructured ? m_BufferElementSize                   : 0;

    ThrowTRFailure( GetFramework()->CreateBufferTiledOptional(&DescBuffer, 0, &m_pTestBuffer),
        "Couldn't create default test buffer");

    DescBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ; //this is read also, because of the CS_4_0 case. See Validate or CSUAVRead for details.
    DescBuffer.Usage = D3D11_USAGE_STAGING;
    DescBuffer.BindFlags = 0;
    DescBuffer.MiscFlags = 0;

    ThrowTRFailure( GetFramework()->CreateBufferTiledOptional(&DescBuffer, 0, &m_pStagingBuffer),
        "Couldn't create staging test buffer");

    if ( Bind & D3D11_BIND_UNORDERED_ACCESS )
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
    

        uavd.ViewDimension          = D3D11_UAV_DIMENSION_BUFFER;
        uavd.Buffer.FirstElement    = m_BufferUnusedLeadingElements;
        uavd.Buffer.NumElements     = m_MaxWidth;

        bool bWriteNeeded = true;

        switch( m_WriteMethod )
        {
        case TypedUAVWrite:  uavd.Buffer.Flags = 0;                         uavd.Format = DXGI_FORMAT_R32_UINT;     break;
        case RawUAVWrite:    uavd.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW; uavd.Format = DXGI_FORMAT_R32_TYPELESS; break;
        case StructUAVWrite: uavd.Buffer.Flags = 0;                         uavd.Format = DXGI_FORMAT_UNKNOWN;      break;
        default : bWriteNeeded = false; break;
        }

        if ( bWriteNeeded )
        {
            ThrowTRFailure( GetFramework()->GetDevice()->CreateUnorderedAccessView(m_pTestBuffer, &uavd, & m_pTestWriteUAV),
                "Couldn't create UAV for write");
        }

        bool bReadNeeded = true;

        switch( m_ReadMethod )
        {
        case TypedUAVRead:  uavd.Buffer.Flags = 0;                         uavd.Format = m_BufferTypedFormat;      break;
        case RawUAVRead:    uavd.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW; uavd.Format = DXGI_FORMAT_R32_TYPELESS; break;
        case StructUAVRead: uavd.Buffer.Flags = 0;                         uavd.Format = DXGI_FORMAT_UNKNOWN;      break;
        default : bReadNeeded = false; break;
        }

        if ( bReadNeeded )
        {
            //In the read case, in CS_4_0, the UAV has to be the write buffer too, so this UAV has to grow so that 
            //the CS can write its data at the end of the buffer: (see Validate for more details).
            uavd.Buffer.NumElements     = m_MaxWidth * m_SizeMultiplier;
            ThrowTRFailure( GetFramework()->GetDevice()->CreateUnorderedAccessView(m_pTestBuffer, &uavd, & m_pTestReadUAV),
                "Couldn't create UAV for read");
        }
    }

    if ( Bind & D3D11_BIND_SHADER_RESOURCE )
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    
        srvd.ViewDimension          = D3D11_SRV_DIMENSION_BUFFEREX;
        srvd.BufferEx.FirstElement  = m_BufferUnusedLeadingElements;
        srvd.BufferEx.NumElements   = m_MaxWidth;

        switch( m_ReadMethod )
        {
        case TypedSRVRead:  srvd.BufferEx.Flags = 0;                           srvd.Format = m_BufferTypedFormat;      break;
        case RawSRVRead:    srvd.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW; srvd.Format = DXGI_FORMAT_R32_TYPELESS; break;
        case StructSRVRead: srvd.BufferEx.Flags = 0;                           srvd.Format = DXGI_FORMAT_UNKNOWN;      break;
        default : assert(false); break;
        }

        ThrowTRFailure( GetFramework()->GetDevice()->CreateShaderResourceView(m_pTestBuffer, &srvd, & m_pTestSRV),
            "Couldn't create test SRV");
    }

    if ( Bind & D3D11_BIND_RENDER_TARGET )
    {
        D3D11_RENDER_TARGET_VIEW_DESC rtvd;
    
        rtvd.ViewDimension          = D3D11_RTV_DIMENSION_BUFFER;
        rtvd.Buffer.FirstElement    = m_BufferUnusedLeadingElements;
        rtvd.Buffer.NumElements     = m_MaxWidth;
        rtvd.Format = m_BufferSRVFormat;

        ThrowTRFailure( GetFramework()->GetDevice()->CreateRenderTargetView(m_pTestBuffer, &rtvd, & m_pTestRTV),
            "Couldn't create test RTV");
    }

    m_ResourceWidth = m_MaxWidth;
    m_ResourceHeight = 1;
    m_ResourceDepth = 1;
    m_ResourceArraySize = 1;

    //This can only be called after resource sizes are set:
    CPUWrite( m_pTestBuffer, m_pStagingBuffer );
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::CreateTexture1D( UINT Bind, UINT ArraySize )
{
    m_bStructured = false;

    D3D11_TEXTURE1D_DESC Desc1D;

    Desc1D.Width = m_MaxWidth*m_SizeMultiplier;
    Desc1D.MipLevels = 1;
    Desc1D.ArraySize = ArraySize;
    Desc1D.Format = m_BufferTypelessFormat;
    Desc1D.Usage = D3D11_USAGE_DEFAULT;
    Desc1D.BindFlags = Bind;
    Desc1D.CPUAccessFlags = 0;
    Desc1D.MiscFlags = 0;

    ThrowTRFailure( GetFramework()->GetDevice()->CreateTexture1D(&Desc1D, 0, &m_pTest1D),
        "Couldn't create default test buffer");

    Desc1D.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc1D.Usage = D3D11_USAGE_STAGING;
    Desc1D.BindFlags = 0;

    ThrowTRFailure( GetFramework()->GetDevice()->CreateTexture1D(&Desc1D, 0, &m_pStaging1D),
        "Couldn't create staging test buffer");

    if ( Bind & D3D11_BIND_UNORDERED_ACCESS )
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
    
        uavd.Format                 = m_BufferTypedFormat;
        if ( ArraySize == 1 )
        {
            uavd.ViewDimension          = D3D11_UAV_DIMENSION_TEXTURE1D;
            uavd.Texture1D.MipSlice     = 0;
        }
        else
        {
            uavd.ViewDimension                  = D3D11_UAV_DIMENSION_TEXTURE1DARRAY;
            uavd.Texture1DArray.MipSlice        = 0;
            uavd.Texture1DArray.FirstArraySlice = 0;
            uavd.Texture1DArray.ArraySize       = ArraySize;
        }

        ThrowTRFailure( GetFramework()->GetDevice()->CreateUnorderedAccessView(m_pTest1D, &uavd, & m_pTestWriteUAV),
            "Couldn't create UAV for write");

        ThrowTRFailure( GetFramework()->GetDevice()->CreateUnorderedAccessView(m_pTest1D, &uavd, & m_pTestReadUAV),
            "Couldn't create UAV for read");
    }

    if ( Bind & D3D11_BIND_SHADER_RESOURCE )
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    
        srvd.Format                    = m_BufferSRVFormat;
        if ( ArraySize == 1 )
        {
            srvd.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE1D;
            srvd.Texture1D.MostDetailedMip = 0;
            srvd.Texture1D.MipLevels       = 1;
        }
        else
        {
            srvd.ViewDimension                  = D3D11_SRV_DIMENSION_TEXTURE1DARRAY;
            srvd.Texture1D.MostDetailedMip      = 0;
            srvd.Texture1D.MipLevels            = 1;
            srvd.Texture1DArray.FirstArraySlice = 0;
            srvd.Texture1DArray.ArraySize       = ArraySize;
        }

        ThrowTRFailure( GetFramework()->GetDevice()->CreateShaderResourceView(m_pTest1D, &srvd, & m_pTestSRV),
            "Couldn't create staging test buffer");
    }

    if ( Bind & D3D11_BIND_RENDER_TARGET )
    {
        D3D11_RENDER_TARGET_VIEW_DESC rtvd;
    
        rtvd.Format = m_BufferSRVFormat;
        if( ArraySize == 1 )
        {
            rtvd.ViewDimension          = D3D11_RTV_DIMENSION_TEXTURE1D;
            rtvd.Texture1D.MipSlice     = 0;
        }
        else
        {
            rtvd.ViewDimension                   = D3D11_RTV_DIMENSION_TEXTURE1DARRAY;
            rtvd.Texture1DArray.MipSlice         = 0;
            rtvd.Texture1DArray.FirstArraySlice  = 0;
            rtvd.Texture1DArray.ArraySize        = ArraySize;
        }

        ThrowTRFailure( GetFramework()->GetDevice()->CreateRenderTargetView(m_pTest1D, &rtvd, & m_pTestRTV),
            "Couldn't create test RTV");
    }

    m_ResourceWidth = m_MaxWidth;
    m_ResourceHeight = 1;
    m_ResourceDepth = 1;
    m_ResourceArraySize = ArraySize;

    //This can only be called after resource sizes are set:
    CPUWrite( m_pTest1D, m_pStaging1D );
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::CreateTexture2D( UINT Bind, UINT ArraySize )
{
    m_bStructured = false;

    D3D11_TEXTURE2D_DESC Desc2D;

    Desc2D.Width = m_MaxWidth*m_SizeMultiplier;
    Desc2D.Height = m_MaxHeight;
    Desc2D.MipLevels = 1;
    Desc2D.ArraySize = ArraySize;
    Desc2D.Format = m_BufferTypelessFormat;
    Desc2D.SampleDesc.Count = 1;
    Desc2D.SampleDesc.Quality = 0;
    Desc2D.Usage = D3D11_USAGE_DEFAULT;
    Desc2D.BindFlags = Bind;
    Desc2D.CPUAccessFlags = 0;
    Desc2D.MiscFlags = 0;

    ThrowTRFailure( GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional(&Desc2D, 0, &m_pTest2D),
        "Couldn't create default test buffer");

    Desc2D.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc2D.Usage = D3D11_USAGE_STAGING;
    Desc2D.BindFlags = 0;

    ThrowTRFailure( GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional(&Desc2D, 0, &m_pStaging2D),
        "Couldn't create staging test buffer");

    if ( Bind & D3D11_BIND_UNORDERED_ACCESS )
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
    
        uavd.Format                 = m_BufferTypedFormat;
        if ( ArraySize == 1 )
        {
            uavd.ViewDimension          = D3D11_UAV_DIMENSION_TEXTURE2D;
            uavd.Texture2D.MipSlice     = 0;
        }
        else
        {
            uavd.ViewDimension                  = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
            uavd.Texture2DArray.MipSlice        = 0;
            uavd.Texture2DArray.FirstArraySlice = 0;
            uavd.Texture2DArray.ArraySize       = ArraySize;
        }

        ThrowTRFailure( GetFramework()->GetDevice()->CreateUnorderedAccessView(m_pTest2D, &uavd, & m_pTestWriteUAV),
            "Couldn't create UAV for write");

        ThrowTRFailure( GetFramework()->GetDevice()->CreateUnorderedAccessView(m_pTest2D, &uavd, & m_pTestReadUAV),
            "Couldn't create UAV for read");
    }

    if ( Bind & D3D11_BIND_SHADER_RESOURCE )
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    
        srvd.Format                    = m_BufferSRVFormat;
        if ( ArraySize == 1 )
        {
            srvd.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvd.Texture2D.MostDetailedMip = 0;
            srvd.Texture2D.MipLevels       = 1;
        }
        else
        {
            srvd.ViewDimension                  = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
            srvd.Texture2D.MostDetailedMip      = 0;
            srvd.Texture2D.MipLevels            = 1;
            srvd.Texture2DArray.FirstArraySlice = 0;
            srvd.Texture2DArray.ArraySize       = ArraySize;
        }

        ThrowTRFailure( GetFramework()->GetDevice()->CreateShaderResourceView(m_pTest2D, &srvd, & m_pTestSRV),
            "Couldn't create staging test buffer");
    }

    if ( Bind & D3D11_BIND_RENDER_TARGET )
    {
        D3D11_RENDER_TARGET_VIEW_DESC rtvd;
    
        rtvd.Format = m_BufferSRVFormat;
        if( ArraySize == 1 )
        {
            rtvd.ViewDimension          = D3D11_RTV_DIMENSION_TEXTURE2D;
            rtvd.Texture2D.MipSlice     = 0;
        }
        else
        {
            rtvd.ViewDimension                   = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
            rtvd.Texture2DArray.MipSlice         = 0;
            rtvd.Texture2DArray.FirstArraySlice  = 0;
            rtvd.Texture2DArray.ArraySize        = ArraySize;
        }

        ThrowTRFailure( GetFramework()->GetDevice()->CreateRenderTargetView(m_pTest2D, &rtvd, & m_pTestRTV),
            "Couldn't create test RTV");
    }

    m_ResourceWidth = m_MaxWidth;
    m_ResourceHeight = m_MaxHeight;
    m_ResourceDepth = 1;
    m_ResourceArraySize = ArraySize;

    //This can only be called after resource sizes are set:
    CPUWrite( m_pTest2D, m_pStaging2D );
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::CreateTexture3D( UINT Bind )
{
    m_bStructured = false;

    D3D11_TEXTURE3D_DESC Desc2D;

    Desc2D.Width = m_MaxWidth*m_SizeMultiplier;
    Desc2D.Height = m_MaxHeight;
    Desc2D.Depth = m_MaxDepth;
    Desc2D.MipLevels = 1;
    Desc2D.Format = m_BufferTypelessFormat;
    Desc2D.Usage = D3D11_USAGE_DEFAULT;
    Desc2D.BindFlags = Bind;
    Desc2D.CPUAccessFlags = 0;
    Desc2D.MiscFlags = 0;

    ThrowTRFailure( GetFramework()->CreateTexture3DMapDefaultOptional(&Desc2D, 0, &m_pTest3D),
        "Couldn't create default test buffer");

    Desc2D.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    Desc2D.Usage = D3D11_USAGE_STAGING;
    Desc2D.BindFlags = 0;

    ThrowTRFailure( GetFramework()->CreateTexture3DMapDefaultOptional(&Desc2D, 0, &m_pStaging3D),
        "Couldn't create staging test buffer");

    if ( Bind & D3D11_BIND_UNORDERED_ACCESS )
    {
        D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
    
        uavd.Format                 = m_BufferTypedFormat;
        uavd.ViewDimension          = D3D11_UAV_DIMENSION_TEXTURE3D;
        uavd.Texture3D.MipSlice     = 0;
        uavd.Texture3D.FirstWSlice  = 0;
        uavd.Texture3D.WSize        = m_MaxDepth;

        ThrowTRFailure( GetFramework()->GetDevice()->CreateUnorderedAccessView(m_pTest3D, &uavd, & m_pTestWriteUAV),
            "Couldn't create UAV for write");

        ThrowTRFailure( GetFramework()->GetDevice()->CreateUnorderedAccessView(m_pTest3D, &uavd, & m_pTestReadUAV),
            "Couldn't create UAV for read");
    }

    if ( Bind & D3D11_BIND_SHADER_RESOURCE )
    {
        D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
    
        srvd.Format                    = m_BufferSRVFormat;
        srvd.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE3D;
        srvd.Texture3D.MostDetailedMip = 0;
        srvd.Texture3D.MipLevels       = 1;

        ThrowTRFailure( GetFramework()->GetDevice()->CreateShaderResourceView(m_pTest3D, &srvd, & m_pTestSRV),
            "Couldn't create staging test buffer");
    }

    m_ResourceWidth = m_MaxWidth;
    m_ResourceHeight = m_MaxHeight;
    m_ResourceDepth = m_MaxDepth;
    m_ResourceArraySize = 1;

    //This can only be called after resource sizes are set:
    CPUWrite( m_pTest3D, m_pStaging3D );
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::InitializeOutputVolume()
{
    GetFramework()->GetEffectiveContext()->CopyResource( m_pDefaultOutputResource, m_pStagingOutputResourceWithInitialValues );
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::CPUWrite( ID3D11Resource * pDefault, ID3D11Resource * pStaging )
{
    D3D11_MAPPED_SUBRESOURCE mr;
    for( UINT a = 0; a < m_ResourceArraySize; ++ a )
    {
        ThrowTRFailure( GetImmediateContext()->Map( pStaging, a, D3D11_MAP_WRITE, 0, & mr ),
            "Couldn't map staging buffer to write test pattern");

        assert( m_BufferTypedFormat == DXGI_FORMAT_R32_UINT );
        BYTE * p = (BYTE*) mr.pData;
        UINT RowPitch = mr.RowPitch / 4;
        UINT DepthPitch = mr.DepthPitch / 4;

        for( UINT w = 0; w < m_ResourceWidth; ++w )
        {
            for( UINT h = 0; h < m_ResourceHeight; ++h )
            {
                for( UINT d = 0; d < m_ResourceDepth; ++d )
                {
                    {
                        p[(d*DepthPitch + h*RowPitch + w)*4 + 0] = (BYTE)w;    
                        p[(d*DepthPitch + h*RowPitch + w)*4 + 1] = (BYTE)h;    
                        p[(d*DepthPitch + h*RowPitch + w)*4 + 2] = (BYTE)d;    
                        p[(d*DepthPitch + h*RowPitch + w)*4 + 3] = (BYTE)a;    
                    }
                }
            }
        }

        GetImmediateContext()->Unmap( pStaging, a );
    }
    GetImmediateContext()->CopyResource( pDefault, pStaging );
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::CSUAVWrite( ) //throw TEST_RESULT
{
    GetFramework()->GetEffectiveContext()->CSSetUnorderedAccessViews( 0 , 1, & m_pTestWriteUAV, NULL );
    switch ( m_InteractionResourceType )
    {
    case StructuredBuffer: GetFramework()->GetEffectiveContext()->CSSetShader( m_pCSUAVStructuredBuffer , NULL, 0 ); break;
    case RawBuffer:        GetFramework()->GetEffectiveContext()->CSSetShader( m_pCSUavRawBuffer        , NULL, 0 ); break;
    case TypedBuffer:      GetFramework()->GetEffectiveContext()->CSSetShader( m_pCSUAVTypedBuffer      , NULL, 0 ); break;
    case Texture1D:        GetFramework()->GetEffectiveContext()->CSSetShader( m_pCSUavTexture1D        , NULL, 0 ); break;
    case Texture1DArray:   GetFramework()->GetEffectiveContext()->CSSetShader( m_pCSUavTexture1DArray   , NULL, 0 ); break;
    case Texture2D:        GetFramework()->GetEffectiveContext()->CSSetShader( m_pCSUavTexture2D        , NULL, 0 ); break;
    case Texture2DArray:   GetFramework()->GetEffectiveContext()->CSSetShader( m_pCSUavTexture2DArray   , NULL, 0 ); break;
    case Texture3D:        GetFramework()->GetEffectiveContext()->CSSetShader( m_pCSUavTexture3D        , NULL, 0 ); break;
    default:
        assert(false);
        break;
    }
    GetFramework()->GetEffectiveContext()->Dispatch( m_ResourceWidth, m_ResourceHeight, std::max<UINT>(m_ResourceDepth, m_ResourceArraySize) );
    ID3D11UnorderedAccessView * pNull = 0;
    GetFramework()->GetEffectiveContext()->CSSetUnorderedAccessViews( 0 , 1, &pNull, NULL );
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::PSUAVWrite( ) //throw TEST_RESULT
{
    GetFramework()->GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 0 , NULL, NULL, 1, 1, & m_pTestWriteUAV, NULL );
    switch ( m_InteractionResourceType )
    {
    case StructuredBuffer: GetFramework()->GetEffectiveContext()->PSSetShader( m_pPSUAVStructuredBuffer , NULL, 0 ); break;
    case TypedBuffer:      GetFramework()->GetEffectiveContext()->PSSetShader( m_pPSUAVTypedBuffer      , NULL, 0 ); break;
    case RawBuffer:        GetFramework()->GetEffectiveContext()->PSSetShader( m_pPSUAVRawBuffer        , NULL, 0 ); break;
    case Texture1D:        GetFramework()->GetEffectiveContext()->PSSetShader( m_pPSUAV1D               , NULL, 0 ); break;
    case Texture1DArray:   GetFramework()->GetEffectiveContext()->PSSetShader( m_pPSUAV1DArray          , NULL, 0 ); break;
    case Texture2D:        GetFramework()->GetEffectiveContext()->PSSetShader( m_pPSUAV2D               , NULL, 0 ); break;
    case Texture2DArray:   GetFramework()->GetEffectiveContext()->PSSetShader( m_pPSUAV2DArray          , NULL, 0 ); break;
    case Texture3D:        GetFramework()->GetEffectiveContext()->PSSetShader( m_pPSUAV3D               , NULL, 0 ); break;
    default:
        assert(false);
        break;
    }
    GetFramework()->GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
    GetFramework()->GetEffectiveContext()->Draw( m_ResourceWidth * m_ResourceHeight * m_ResourceDepth, m_CurrentArraySlice * m_MaxWidth * m_MaxHeight );
    ID3D11UnorderedAccessView * pNull = 0;
    GetFramework()->GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 0 , NULL, NULL, 1, 1, &pNull, NULL );
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::CSUAVRead( ) //throw TEST_RESULT
{
    if ( m_InteractionResourceType == RawBuffer || m_InteractionResourceType == StructuredBuffer )
    {
        //This test is the only CS read case that is valid on CS_4_0. Hence, the read and verification-write must be packed
        //into one resource;
        GetFramework()->GetEffectiveContext()->CSSetUnorderedAccessViews( 0 , 1, & m_pTestReadUAV, NULL );
        m_ValidateInTestedBuffer = true;
    }
    else
    {
        GetFramework()->GetEffectiveContext()->CSSetUnorderedAccessViews( 0 , 1, & m_pTestReadUAV, NULL );
        GetFramework()->GetEffectiveContext()->CSSetUnorderedAccessViews( 1 , 1, & m_pOutputResourceUAV, NULL );
    }

    switch ( m_InteractionResourceType )
    {
    case StructuredBuffer:  GetFramework()->GetEffectiveContext()->CSSetShader(  m_pCSUavReadStructuredBuffer, NULL, 0 );break;
    case RawBuffer:         GetFramework()->GetEffectiveContext()->CSSetShader(  m_pCSUavReadRawBuffer       , NULL, 0 );break;
    case TypedBuffer:       GetFramework()->GetEffectiveContext()->CSSetShader(  m_pCSUavReadTypedBuffer     , NULL, 0 );break;
    case Texture1D:         GetFramework()->GetEffectiveContext()->CSSetShader(  m_pCSUavReadTexture1D       , NULL, 0 );break;
    case Texture1DArray:    GetFramework()->GetEffectiveContext()->CSSetShader(  m_pCSUavReadTexture1DArray  , NULL, 0 );break;
    case Texture2D:         GetFramework()->GetEffectiveContext()->CSSetShader(  m_pCSUavReadTexture2D       , NULL, 0 );break;
    case Texture2DArray:    GetFramework()->GetEffectiveContext()->CSSetShader(  m_pCSUavReadTexture2DArray  , NULL, 0 );break;
    case Texture3D:         GetFramework()->GetEffectiveContext()->CSSetShader(  m_pCSUavReadTexture3D       , NULL, 0 );break;
    default:
        assert(false);
        break;
    }

    GetFramework()->GetEffectiveContext()->Dispatch( m_ResourceWidth, m_ResourceHeight, std::max<UINT>(m_ResourceDepth, m_ResourceArraySize) );

    ID3D11UnorderedAccessView * pNull = 0;
    GetFramework()->GetEffectiveContext()->CSSetUnorderedAccessViews( 0 , 1, &pNull, NULL );
    if ( ! m_ValidateInTestedBuffer )
    {
        GetFramework()->GetEffectiveContext()->CSSetUnorderedAccessViews( 1 , 1, &pNull, NULL );
    }

    Validate( ); //throw TEST_RESULT
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::VSSRVRead( ) //throw TEST_RESULT
{
    InitializeOutputVolume();

    GetFramework()->GetEffectiveContext()->VSSetShaderResources( m_bStructured ? 1 : 0, 1, & m_pTestSRV );

    switch ( m_InteractionResourceType )
    {
    case StructuredBuffer:  GetFramework()->GetEffectiveContext()->VSSetShader(  m_pVSSampleStructured  , NULL, 0 );break;
    case RawBuffer:         GetFramework()->GetEffectiveContext()->VSSetShader(  m_pVSSampleRawSRV      , NULL, 0 );break;
    case TypedBuffer:       GetFramework()->GetEffectiveContext()->VSSetShader(  m_pVSSRVBufferSample   , NULL, 0 );break;
    case Texture1D:         GetFramework()->GetEffectiveContext()->VSSetShader(  m_pVSSRV1DSample       , NULL, 0 );break;
    case Texture1DArray:    GetFramework()->GetEffectiveContext()->VSSetShader(  m_pVSSRV1DSampleArray  , NULL, 0 );break;
    case Texture2D:         GetFramework()->GetEffectiveContext()->VSSetShader(  m_pVSSRV2DSample       , NULL, 0 );break;
    case Texture2DArray:    GetFramework()->GetEffectiveContext()->VSSetShader(  m_pVSSRV2DSampleArray  , NULL, 0 );break;
    case Texture3D:         GetFramework()->GetEffectiveContext()->VSSetShader(  m_pVSSRV3DSample       , NULL, 0 );break;
    default:
        assert(false);
        break;
    }
    GetFramework()->GetEffectiveContext()->PSSetShader( m_pPSPassthrough, NULL, 0 );

    GetFramework()->GetEffectiveContext()->Draw( m_ResourceWidth * m_ResourceHeight * m_ResourceDepth, m_CurrentArraySlice * m_MaxWidth * m_MaxHeight );

    ID3D11ShaderResourceView * pNull = 0;
    GetFramework()->GetEffectiveContext()->VSSetShaderResources( m_bStructured ? 1 : 0, 1, & pNull );

    Validate( ); //throw TEST_RESULT
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::PSSRVRead(  ) //throw TEST_RESULT
{
    InitializeOutputVolume();

    GetFramework()->GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
    GetFramework()->GetEffectiveContext()->PSSetShaderResources( m_bStructured ? 1 : 0, 1, & m_pTestSRV );
    switch ( m_InteractionResourceType )
    {
    case StructuredBuffer: 
        GetFramework()->GetEffectiveContext()->PSSetShader(  m_pPSSampleStructured, NULL, 0 );
        break;
    case RawBuffer: 
        GetFramework()->GetEffectiveContext()->PSSetShader(  m_pPSSampleRawSRV, NULL, 0 );
        break;
    case TypedBuffer: 
        GetFramework()->GetEffectiveContext()->PSSetShader(  m_pPSSRVBufferSample, NULL, 0 );
        break;
    case Texture1D:
        GetFramework()->GetEffectiveContext()->PSSetShader(  m_pPSSRV1DSample, NULL, 0 );
        break;
    case Texture2D:
        GetFramework()->GetEffectiveContext()->PSSetShader(  m_pPSSRV2DSample, NULL, 0 );
        break;
    case Texture1DArray:
        GetFramework()->GetEffectiveContext()->PSSetShader(  m_pPSSRV1DArraySample, NULL, 0 );
        break;
    case Texture2DArray:
        GetFramework()->GetEffectiveContext()->PSSetShader(  m_pPSSRV2DArraySample, NULL, 0 );
        break;
    case Texture3D:
        GetFramework()->GetEffectiveContext()->PSSetShader(  m_pPSSRV3DSample, NULL, 0 );
        break;
    default:
        assert(false);
        break;
    }

    GetFramework()->GetEffectiveContext()->Draw( m_ResourceWidth * m_ResourceHeight * m_ResourceDepth, m_CurrentArraySlice * m_MaxWidth * m_MaxHeight );

    ID3D11ShaderResourceView * pNull = 0;
    GetFramework()->GetEffectiveContext()->PSSetShaderResources( m_bStructured ? 1 : 0, 1, & pNull );

    Validate( ); //throw TEST_RESULT
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::PSRTWrite(  ) //throw TEST_RESULT
{
    InitializeOutputVolume();

    GetFramework()->GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 1 , &m_pTestRTV, NULL, 0, 0, NULL, NULL );

    GetFramework()->GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
    GetFramework()->GetEffectiveContext()->PSSetShader( m_ResourceArraySize == 1 ? m_pPSWriteRT : m_pPSWriteRTArray, NULL, 0 );
    GetFramework()->GetEffectiveContext()->Draw( m_ResourceWidth * m_ResourceHeight * m_ResourceDepth, m_CurrentArraySlice * m_MaxWidth * m_MaxHeight );

    ID3D11RenderTargetView * pNull = 0;
    GetFramework()->GetEffectiveContext()->OMSetRenderTargetsAndUnorderedAccessViews( 1 , &pNull, NULL, 0, 0, NULL, NULL );
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::IAVBRead(  ) //throw TEST_RESULT
{
    InitializeOutputVolume();

    UINT Stride = m_BufferElementSize;
    UINT Offset = m_BufferUnusedLeadingElements * m_BufferElementSize;
    GetFramework()->GetEffectiveContext()->IASetVertexBuffers( 1, 1, & m_pTestBuffer, & Stride, & Offset );
    GetFramework()->GetEffectiveContext()->VSSetShader( m_pVS, NULL, 0 );
    GetFramework()->GetEffectiveContext()->PSSetShader( m_pPSPassthrough, NULL, 0 );

    GetFramework()->GetEffectiveContext()->Draw( m_ResourceWidth * m_ResourceHeight * m_ResourceDepth, m_CurrentArraySlice * m_MaxWidth * m_MaxHeight );

    Stride = 0;
    GetFramework()->GetEffectiveContext()->IASetVertexBuffers( 1, 1, & m_pDummyDataBuffer, & Stride, & Offset );

    Validate( ); //throw TEST_RESULT
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::Validate( )
{
    D3D11_MAPPED_SUBRESOURCE mr;
    bool bValidateTestBuffer = false;
    struct BYTE4
    {
        BYTE x,y,z,w;
    };
    BYTE4 * p;
    UINT MessageCountRemaining = 6;

    ThrowTRFailure( ExecuteEffectiveContext(), "ExecuteEffectiveContext() failed." );
    
    if ( m_ValidateInTestedBuffer )
    {
        //This is the one CS read case that runs on CS_4_0, so can use only one UAV bind slot
        //The buffer is double-sized, and the results are placed at the end of the buffer.
        GetImmediateContext()->CopyResource( m_pStagingBuffer, m_pTestBuffer );
        ThrowTRFailure( GetImmediateContext()->Map( m_pStagingBuffer, 0, D3D11_MAP_READ, 0, & mr ),
            "Couldn't map staging buffer to validate pattern");
        p = (BYTE4*) mr.pData;
        p += m_ResourceWidth;
    }
    else
    {
        GetImmediateContext()->CopyResource( m_pStagingOutputResource, m_pDefaultOutputResource );
        ThrowTRFailure( GetImmediateContext()->Map( m_pStagingOutputResource, 0, D3D11_MAP_READ, 0, & mr ),
            "Couldn't map staging texture3d to validate pattern");
        p = (BYTE4*) mr.pData;
    }

    UINT cUnwritten = 0;
    UINT cIncorrect = 0;

    for( UINT w = 0;  w < m_ResourceWidth; ++ w)
    {
        for( UINT h = 0;  h < m_ResourceHeight; ++ h)
        {
            for( UINT d = 0;  d < m_ResourceDepth; ++ d)
            {
                BYTE4 * pThisPixel = p + w + h * mr.RowPitch/sizeof(BYTE4) + (d+m_CurrentArraySlice) * mr.DepthPitch / sizeof(BYTE4);
                if ( pThisPixel->x == m_OutputBufferInitialValue || pThisPixel->y == m_OutputBufferInitialValue || pThisPixel->z == m_OutputBufferInitialValue )
                {
                    if (MessageCountRemaining) {
                        MessageCountRemaining -= 1;
                        WriteToLog( "At least one component of output pixel %d,%d,%d wasn't written by the pixel shader that is sampling the computed values", w,h,d );
                    }

                    cUnwritten++;
                }
                if ( pThisPixel->x != (BYTE) w )
                {
                    cIncorrect++;
                    if (MessageCountRemaining) {
                        MessageCountRemaining -= 1;
                        WriteToLog( "The x component of pixel %d,%d,%d should contain its own coordinate. Instead it contained %d. "
                            "Either the intermediate value was not written correctly, or not read when sampling for verification purposes. "
                            "(Also possible that the output pixel shader wrote a bad value, if it wrote anything.)", w,h,d,pThisPixel->x );
                    }
                }
                if ( pThisPixel->y != (BYTE) h )
                {
                    cIncorrect++;
                    if (MessageCountRemaining) {
                        MessageCountRemaining -= 1;
                        WriteToLog( "The y component of pixel %d,%d,%d should contain its own coordinate. Instead it contained %d. "
                            "Either the intermediate value was not written correctly, or not read when sampling for verification purposes. "
                            "(Also possible that the output pixel shader wrote a bad value, if it wrote anything.)", w,h,d,pThisPixel->y );
                    }
                }
                if ( m_ResourceArraySize > 1 )
                {
                    if ( pThisPixel->w != (BYTE) m_CurrentArraySlice )
                    {
                        cIncorrect++;
                        if (MessageCountRemaining) {
                            MessageCountRemaining -= 1;
                            WriteToLog( "The w component of pixel %d,%d,%d should contain the array index (%d). Instead it contained %d. "
                                "Either the intermediate value was not written correctly, or not read when sampling for verification purposes. "
                                "(Also possible that the pixel shader wrote a bad value, if it wrote anything.)", w,h,d,m_CurrentArraySlice,pThisPixel->w );
                        }
                    }
                }
                else
                {
                    if ( pThisPixel->z != (BYTE) d )
                    {
                        cIncorrect++;
                        if (MessageCountRemaining) {
                            MessageCountRemaining -= 1;
                            WriteToLog( "The z component of pixel %d,%d,%d should contain its own coordinate. Instead it contained %d. "
                                "Either the intermediate value was not written correctly, or not read when sampling for verification purposes. "
                                "(Also possible that the pixel shader wrote a bad value, if it wrote anything.)", w,h,d,pThisPixel->z );
                        }
                    }
                }
            }
        }
    }

    if ( m_ValidateInTestedBuffer )
    {
        GetImmediateContext()->Unmap( m_pStagingBuffer, 0 );
    }
    else
    {
        GetImmediateContext()->Unmap( m_pStagingOutputResource, 0 );
    }

    if ( cUnwritten )
    {
        WriteToLog("%d of the %d pixels in the output volume were not written by the pixel shader. They all should have been.", 
            cUnwritten, m_ResourceWidth, m_ResourceHeight, m_ResourceDepth);
    }
    if ( cIncorrect )
    {
        WriteToLog("%d of the %d pixels in the output volume were incorrect.", 
            cIncorrect, m_ResourceWidth, m_ResourceHeight, m_ResourceDepth);
    }
    if ( cUnwritten || cIncorrect )
    {
        throw TRException( 
            "(Resource type:" + ToString(m_InteractionResourceType) + 
            ", Read method:" + ToString(m_ReadMethod) +
            ", Write method:" + ToString(m_WriteMethod) +
            ", WriteStage:" + ToString(m_WriteStage) +
            ", ReadStage:" + ToString(m_ReadStage) +
            ", : Some pixels had unexpected values.");
    }
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionTest::InitTestParameters()
{
    AddParameter<e_ResourceType>( _T("ResourceType"),&m_InteractionResourceType );
    AddParameter<e_ReadMethod>(   _T("ReadMethod"),  &m_ReadMethod );
    AddParameter<e_WriteMethod>(  _T("WriteMethod"), &m_WriteMethod );
    AddParameter<e_Stage>(        _T("WriteStage"),  &m_WriteStage );
    AddParameter<e_Stage>(        _T("ReadStage"),   &m_ReadStage );
    AddParameter<bool>(           _T("Requires_5_0"),&m_bRequires_5_0 );

    testfactor::RFactor rRequires5_0        = AddParameterValue( _T("Requires_5_0"), true );
    testfactor::RFactor rDoesNotRequire5_0  = AddParameterValue( _T("Requires_5_0"), false );
    
    testfactor::RFactor rStructuredBuffer   = AddParameterValue( _T("ResourceType"), CInteractionTest::StructuredBuffer );
    testfactor::RFactor rRawBuffer          = AddParameterValue( _T("ResourceType"), CInteractionTest::RawBuffer );
    testfactor::RFactor rTypedBuffer        = AddParameterValue( _T("ResourceType"), CInteractionTest::TypedBuffer );
    testfactor::RFactor rNonBufferResources = AddParameterValue( _T("ResourceType"), CInteractionTest::Texture1D ) +
                                              AddParameterValue( _T("ResourceType"), CInteractionTest::Texture1DArray ) +
                                              AddParameterValue( _T("ResourceType"), CInteractionTest::Texture2D ) +
                                              AddParameterValue( _T("ResourceType"), CInteractionTest::Texture2DArray ) +
                                              AddParameterValue( _T("ResourceType"), CInteractionTest::Texture3D );

    testfactor::RFactor rTypedUAVWrite    = AddParameterValue( _T("WriteMethod"),  CInteractionTest::TypedUAVWrite );
    testfactor::RFactor rRawUAVWrite      = AddParameterValue( _T("WriteMethod"),  CInteractionTest::RawUAVWrite );
    testfactor::RFactor rStructUAVWrite   = AddParameterValue( _T("WriteMethod"),  CInteractionTest::StructUAVWrite );
    testfactor::RFactor rStreamOutMethod  = AddParameterValue( _T("WriteMethod"),  CInteractionTest::StreamOutWrite );
    testfactor::RFactor rRTWrite          = AddParameterValue( _T("WriteMethod"),  CInteractionTest::RTWrite );

    testfactor::RFactor rTypedUAVRead     = AddParameterValue( _T("ReadMethod"),   CInteractionTest::TypedUAVRead );
    testfactor::RFactor rRawUAVRead       = AddParameterValue( _T("ReadMethod"),   CInteractionTest::RawUAVRead );
    testfactor::RFactor rStructUAVRead    = AddParameterValue( _T("ReadMethod"),   CInteractionTest::StructUAVRead );
    testfactor::RFactor rTypedSRVRead     = AddParameterValue( _T("ReadMethod"),   CInteractionTest::TypedSRVRead );
    testfactor::RFactor rRawSRVRead       = AddParameterValue( _T("ReadMethod"),   CInteractionTest::RawSRVRead );
    testfactor::RFactor rStructSRVRead    = AddParameterValue( _T("ReadMethod"),   CInteractionTest::StructSRVRead );
    testfactor::RFactor rIAIBRead         = AddParameterValue( _T("ReadMethod"),   CInteractionTest::IBRead );
    testfactor::RFactor rIAVBRead         = AddParameterValue( _T("ReadMethod"),   CInteractionTest::VBRead );

    testfactor::RFactor rVSRead           = AddParameterValue( _T("ReadStage"),    CInteractionTest::VertexShader );
    testfactor::RFactor rPSRead           = AddParameterValue( _T("ReadStage"),    CInteractionTest::PixelShader );
    testfactor::RFactor rInputAssembler   = AddParameterValue( _T("ReadStage"),    CInteractionTest::InputAssembler );
    testfactor::RFactor rCSRead           = AddParameterValue( _T("ReadStage"),    CInteractionTest::ComputeShader );

    testfactor::RFactor rCSWrite          = AddParameterValue( _T("WriteStage"),   CInteractionTest::ComputeShader );
    testfactor::RFactor rVSWrite          = AddParameterValue( _T("WriteStage"),   CInteractionTest::VertexShader );
    testfactor::RFactor rPSWrite          = AddParameterValue( _T("WriteStage"),   CInteractionTest::PixelShader );
    testfactor::RFactor rStreamOutStage   = AddParameterValue( _T("WriteStage"),   CInteractionTest::StreamOutStage );

                                        //Resource type                    Writer               Write Method        Reader                      Read method
    testfactor::RFactor rPri21aRaw    = (rRawBuffer) *                     rCSWrite *           rRawUAVWrite *      ( rVSRead + rPSRead ) *     rRawSRVRead   ; //Compute Pri 2 1.a raw
    testfactor::RFactor rPri21aStruct = (rStructuredBuffer) *              rCSWrite *           rStructUAVWrite *   ( rVSRead + rPSRead ) *     rStructSRVRead; //Compute Pri 2 1.a structured
    testfactor::RFactor rPri21b       = (rRawBuffer ) *                    rCSWrite *           rRawUAVWrite *      ( rInputAssembler )   *     rIAVBRead     ; //Compute Pri 2 1.b //TODO IB read
    testfactor::RFactor rPri21c       = (rNonBufferResources ) *           rCSWrite *           rStructUAVWrite *   ( rVSRead + rPSRead ) *     rTypedSRVRead ; //Compute Pri 2 1.c
    testfactor::RFactor rPri22a       = (rNonBufferResources ) *           rPSWrite *           rRTWrite *          ( rCSRead ) *               rTypedUAVRead ; //Compute Pri 2 2.a
    testfactor::RFactor rPri22b       = (rTypedBuffer ) *                  rPSWrite *           rRTWrite *          ( rCSRead ) *               rTypedUAVRead ; //Compute Pri 2 2.b
// Test spec bug: this is illegal    testfactor::RFactor rPri22c       = (rTypedBuffer ) *                  rStreamOutStage *    rStreamOutMethod *  ( rCSRead ) *               rTypedUAVRead ; //Compute Pri 2 2.c
    testfactor::RFactor rPri22dRaw    = (rRawBuffer) *                     rPSWrite *           rRawUAVWrite *      ( rCSRead ) *               rRawUAVRead   ; //Compute Pri 2 1.d
    testfactor::RFactor rPri22dStruct = (rStructuredBuffer) *              rPSWrite *           rStructUAVWrite *   ( rCSRead ) *               rStructUAVRead; //Compute Pri 2 1.d

    SetRootTestFactor( 
        ( rPri21aRaw + rPri21aStruct + rPri21b ) * rDoesNotRequire5_0 + 
        ( rPri21c ) * rRequires5_0 + 
        ( rPri22a ) * rRequires5_0 + 
        ( rPri22b ) * rRequires5_0 + 
        /* rPri22c + */ //SO UAV is illegal
        ( rPri22dRaw + rPri22dStruct ) * rDoesNotRequire5_0 );
}

//---------------------------------------------------------------------------------------------------------------------------------
TEST_RESULT CInteractionTest::ExecuteTestCase()
{
    bool bCS_4_0 = false;

    if ( GetFramework()->m_D3DOptions.FeatureLevel <= D3D_FEATURE_LEVEL_10_1 )
    {
        if ( m_bRequires_5_0 )
        {
            WriteToLog( _T("This case requires feature level 11.0") );
            return RESULT_SKIP;
        }

        D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS Features;
        GetFramework()->GetDevice()->CheckFeatureSupport( D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &Features, sizeof(Features) );
        if ( ! Features.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x )
        {
            WriteToLog( _T("This case requires compute shaders at feature level 10.0/10.1") );
            return RESULT_SKIP;
        }
        bCS_4_0 = true;
    }

    TEST_RESULT tRes = RESULT_PASS;

    try
    {
        UINT BindFlags = 0;
        switch( m_WriteMethod )
        {
        case StreamOutWrite: BindFlags |= D3D11_BIND_STREAM_OUTPUT;    break;
        case TypedUAVWrite:  BindFlags |= D3D11_BIND_UNORDERED_ACCESS; break;
        case RawUAVWrite:    BindFlags |= D3D11_BIND_UNORDERED_ACCESS; break;
        case StructUAVWrite: BindFlags |= D3D11_BIND_UNORDERED_ACCESS; break;
        case RTWrite:        BindFlags |= D3D11_BIND_RENDER_TARGET;    break;
        default:
            assert(false);
            break;
        }

        switch( m_ReadMethod )
        {
        case TypedUAVRead:  BindFlags |= D3D11_BIND_UNORDERED_ACCESS; break;
        case RawUAVRead:    BindFlags |= D3D11_BIND_UNORDERED_ACCESS; break;
        case StructUAVRead: BindFlags |= D3D11_BIND_UNORDERED_ACCESS; break;
        case TypedSRVRead:  BindFlags |= D3D11_BIND_SHADER_RESOURCE; break;
        case RawSRVRead:    BindFlags |= D3D11_BIND_SHADER_RESOURCE; break;
        case StructSRVRead: BindFlags |= D3D11_BIND_SHADER_RESOURCE; break;
        case IBRead:        BindFlags |= D3D11_BIND_INDEX_BUFFER;     break;
        case VBRead:        BindFlags |= D3D11_BIND_VERTEX_BUFFER;    break;
        default:
            assert(false);
            break;
        }

        switch( m_InteractionResourceType )
        {
        case StructuredBuffer:  CreateBuffer( BindFlags, true /* structured */ );                   break;
        case RawBuffer:         CreateBuffer( BindFlags, false/* structured */, true /* raw */ );   break;
        case TypedBuffer:       CreateBuffer( BindFlags );                                          break;
        case Texture1D:         CreateTexture1D( BindFlags, 1 );                                    break;
        case Texture2D:         CreateTexture2D( BindFlags, 1 );                                    break;
        case Texture1DArray:    CreateTexture1D( BindFlags, m_MaxArraySize );                       break;
        case Texture2DArray:    CreateTexture2D( BindFlags, m_MaxArraySize );                       break;
        case Texture3D:         CreateTexture3D( BindFlags );                                       break;
        default:
            assert(false);
            break;
        }

        m_ValidateInTestedBuffer = false; //this will be updated in CSUAVRead, if necessary

        for( m_CurrentArraySlice = 0; m_CurrentArraySlice < m_ResourceArraySize; ++ m_CurrentArraySlice )
        {
            UpdateResourceDimensionCB();

            switch( m_WriteStage )
            {
            case PixelShader:
                switch( m_WriteMethod )
                {
                case RTWrite:        PSRTWrite( );   break;
                case TypedUAVWrite:  PSUAVWrite( );  break;
                case RawUAVWrite:    PSUAVWrite( );  break;
                case StructUAVWrite: PSUAVWrite( );  break;
                default: assert(false);           break;
                }
                break;
            case VertexShader:
                switch( m_WriteMethod )
                {
                case TypedUAVWrite:  VSUAVWrite( );   break;
                case RawUAVWrite:    VSUAVWrite( );   break;
                case StructUAVWrite: VSUAVWrite( );   break;
                default: assert(false);           break;
                }
                break;
            case ComputeShader:
                switch( m_WriteMethod )
                {
                case TypedUAVWrite:  CSUAVWrite( );   break;
                case RawUAVWrite:    CSUAVWrite( );   break;
                case StructUAVWrite: CSUAVWrite( );   break;
                default: assert(false);           break;
                }
                break;
            case StreamOutStage:
                assert( m_WriteMethod == StreamOutWrite );
                SOWrite( );  
                break;
            default:
                assert(false);
                break;
            }

            switch( m_ReadStage )
            {
            case PixelShader:
                switch( m_ReadMethod )
                {
                case TypedSRVRead:  PSSRVRead( );  break;
                case RawSRVRead:    PSSRVRead( );  break;
                case StructSRVRead: PSSRVRead( );  break;
                case TypedUAVRead:  PSUAVRead( );  break;
                case RawUAVRead:    PSUAVRead( );  break;
                case StructUAVRead: PSUAVRead( );  break;
                default: assert(false);        break;
                }
                break;
            case VertexShader:
                switch( m_ReadMethod )
                {
                case TypedSRVRead:  VSSRVRead( );  break;
                case RawSRVRead:    VSSRVRead( );  break;
                case StructSRVRead: VSSRVRead( );  break;
                case TypedUAVRead:  VSUAVRead( );  break;
                case RawUAVRead:    VSUAVRead( );  break;
                case StructUAVRead: VSUAVRead( );  break;
                default: assert(false);        break;
                }
                break;
            case ComputeShader:
                switch( m_ReadMethod )
                {
                case TypedUAVRead:  CSUAVRead( );  break;
                case RawUAVRead:    CSUAVRead( );  break;
                case StructUAVRead: CSUAVRead( );  break;
                default: assert(false);        break;
                }
                break;
            case InputAssembler:
                switch( m_ReadMethod )
                {
                case VBRead:   IAVBRead( );   break;
                case IBRead:   assert(false && "not yet implemented");  break;
                default: assert(false);           break;
                }
                break;
            default:
                assert(false);
                break;
            }
        } //for array slice
    }
    catch(TRException &exc)
    {
        tRes = exc.GetResult();
        exc.Prepend("[CInteractionBufferTest::ExecuteTestCase()] ");
        if (tRes != RESULT_PASS)
            WriteToLog( exc.GetError().c_str() );
    }

    ThrowTRFailure( ExecuteEffectiveContext(), "ExecuteEffectiveContext() failed." );

    m_pTestSRV = 0;
    m_pTestRTV = 0;
    m_pTestWriteUAV = 0;
    m_pTestReadUAV = 0;
    m_pTestBuffer = 0;
    m_pStagingBuffer = 0;
    m_pTest1D = 0;
    m_pStaging1D = 0;
    m_pTest2D = 0;
    m_pStaging2D = 0;
    m_pTest3D = 0;
    m_pStaging3D = 0;

    return tRes;
}


//---------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------------------

const UINT cBackBufferSize = 256;

//---------------------------------------------------------------------------------------------------------------------------------
TEST_RESULT CInteractionPresentTest :: Setup()
{
    if ( GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
    {
        WriteToLog( _T("This case requires feature level 11.0") );
        return RESULT_SKIP;
    }

    TEST_RESULT tRes = RESULT_PASS;

    try
    {
        WindowsTest::CHandle<ID3D10Blob> pCode;
        CompileShader( "interaction.sh", "Life", "cs_5_0", 0, & pCode ); //throw
        ThrowTRFailure( GetFramework()->GetDevice()->CreateComputeShader( (DWORD*) pCode->GetBufferPointer(), pCode->GetBufferSize(), NULL, &m_pComputeLife ),
            "Couldn't create CSUavReadTexture3D shader");

        WindowsTest::CHandle<ID3D11Texture2D> pBB0;
        WindowsTest::CHandle<ID3D11Texture2D> pBB1;

        ThrowTRFailure( CreateBuffers(pBB0, pBB1), "Couldn't create buffers." );

        D3D11_UNORDERED_ACCESS_VIEW_DESC uavd;
    
        uavd.Format                 = DXGI_FORMAT_R32_UINT;
        uavd.ViewDimension          = D3D11_UAV_DIMENSION_TEXTURE2D;
        uavd.Texture2D.MipSlice     = 0;

        ThrowTRFailure( GetFramework()->GetDevice()->CreateUnorderedAccessView(pBB0, NULL, & m_pTestWriteUAV),
            "Couldn't create UAV for write");

        ThrowTRFailure( GetFramework()->GetDevice()->CreateShaderResourceView(pBB1, NULL, & m_pTestReadSRV),
            "Couldn't create UAV for write");

        GetFramework()->GetEffectiveContext()->CSSetShader( m_pComputeLife , NULL, 0 ); 

        D3D11_TEXTURE2D_DESC Desc2D;

        Desc2D.Width = cBackBufferSize;
        Desc2D.Height = cBackBufferSize;
        Desc2D.MipLevels = 1;
        Desc2D.ArraySize = 1;
        Desc2D.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        Desc2D.SampleDesc.Count = 1;
        Desc2D.SampleDesc.Quality = 0;
        Desc2D.Usage = D3D11_USAGE_STAGING;
        Desc2D.BindFlags = 0;
        Desc2D.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
        Desc2D.MiscFlags = 0;

        ThrowTRFailure( GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional(&Desc2D, 0, &m_pStaging2D0),
            "Couldn't create staging test 0 buffer");
        ThrowTRFailure( GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional(&Desc2D, 0, &m_pStaging2D1),
            "Couldn't create staging test 1 buffer");
        ThrowTRFailure( GetFramework()->CreateTexture2DTiledOptionalMapDefaultOptional(&Desc2D, 0, &m_pStagingCompare),
            "Couldn't create staging compare buffer");

        ThrowTRFailure( ExecuteEffectiveContext(), "ExecuteEffectiveContext() failed." );

        //copy starting pattern into 1st staging resource:
        D3D11_MAPPED_SUBRESOURCE mr;
        ThrowTRFailure( GetFramework()->GetImmediateContext()->Map( m_pStaging2D1, 0, D3D11_MAP_WRITE, 0, & mr ),
            "Couldn't map staging buffer to write test pattern");

        DWORD * p = (DWORD*) mr.pData;
        UINT RowPitch = mr.RowPitch / sizeof(*p);

        D3DX11_IMAGE_LOAD_INFO LoadInfo;
        LoadInfo.Usage = D3D11_USAGE_STAGING;
        LoadInfo.BindFlags = 0;
        LoadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_READ;
        WindowsTest::CHandle<ID3D11Resource> pTemp;
        ThrowTRFailure(
            D3DX11CreateTextureFromResourceA(
                GetFramework()->GetDevice(),
                GetModuleHandle(NULL),
                "breeder.bmp",
                &LoadInfo,
                nullptr,
                &pTemp,
                nullptr),
            "Couldn't load breeder.bmp");

        D3D11_MAPPED_SUBRESOURCE mr2;
        ThrowTRFailure( GetFramework()->GetImmediateContext()->Map( pTemp, 0, D3D11_MAP_READ, 0, &mr2 ),
            "Couldn't map breeder.bmp for read");

        DWORD * p2 = (DWORD*) mr2.pData;
        UINT RowPitch2 = mr2.RowPitch / sizeof(*p2);
        
        SIZE BitmapSize = { 134,139 };

        for( INT w = 0; w < cBackBufferSize; ++w )
        {
            for( INT h = 0; h < cBackBufferSize; ++h )
            {
                p[ h*RowPitch + w] = 0;
            }
        }

        {
            for( INT w = 0; w < BitmapSize.cx; ++w )
            {
                for( INT h = 0; h < BitmapSize.cy; ++h )
                {
                    p[ (h+cBackBufferSize-BitmapSize.cy)*RowPitch + w] = (
                        GetBValue( p2[ h*RowPitch2 + w ] ) > 0xf0 ) ? 0 : 0xffffffff;
                }
            }
        }

        GetFramework()->GetImmediateContext()->Unmap( pTemp, 0 );
        GetFramework()->GetImmediateContext()->Unmap( m_pStaging2D1, 0 );

        //Now populate GPU resources:
        GetFramework()->GetImmediateContext()->CopyResource( pBB0, m_pStaging2D1 );

        //Put start pattern in "front buffer":
        Present();
    }
    catch(TRException &exc)
    {
        tRes = exc.GetResult();
        exc.Prepend("[CInteractionPresentTest::ExecuteTestCase()] ");
        if (tRes != RESULT_PASS)
            WriteToLog( exc.GetError().c_str() );
    }
    
    return tRes;
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionPresentTest :: Cleanup()
{
    m_pTestWriteUAV = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------
void CInteractionPresentTest :: InitTestParameters()
{
    if ( GetFramework()->m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
        return;

    GetFramework()->m_RenderTargetOptions.NoDepthStencils = true;
    GetFramework()->m_RenderTargetOptions.DepthTextureDesc.MipLevels = 1;
    GetFramework()->m_RenderTargetOptions.NoSwapChains = false;
    GetFramework()->m_RenderTargetOptions.SwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    GetFramework()->m_RenderTargetOptions.SwapChainDesc.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
    GetFramework()->m_RenderTargetOptions.SwapChainDesc.Width = cBackBufferSize;
    GetFramework()->m_RenderTargetOptions.SwapChainDesc.Height = cBackBufferSize;

    GetFramework()->m_RenderTargetOptions.SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
    GetFramework()->m_RenderTargetOptions.SwapChainDesc.BufferCount = (UINT) 2;
    GetFramework()->m_RenderTargetOptions.SwapChainDesc.SampleDesc.Count  = 1;
    GetFramework()->m_RenderTargetOptions.SwapChainDesc.SampleDesc.Quality = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------
INT Wrap( INT c, INT a )
{
    return ( c + a ) & (cBackBufferSize-1);
}

INT WrappedOffset(INT x, INT y, INT xOff, INT yOff, UINT SourceRowPitch )
{
    return Wrap(x, xOff) + Wrap(y, yOff) * SourceRowPitch;
}
//---------------------------------------------------------------------------------------------------------------------------------
TEST_RESULT CInteractionPresentTest :: ExecuteTestCase()
{
    TEST_RESULT tRes = RESULT_PASS;

    try
    {
        for(UINT iPass = 0; iPass < 100; ++iPass )
        {
            WindowsTest::CHandle< ID3D11RenderTargetView > pRT;
            ID3D11RenderTargetView * pRTNull = 0;
            GetFramework()->GetEffectiveContext()->OMGetRenderTargets( 1, &pRT, 0 );
            GetFramework()->GetEffectiveContext()->OMSetRenderTargets( 1, &pRTNull, 0 );
            GetFramework()->GetEffectiveContext()->CSSetUnorderedAccessViews( 0 , 1, & m_pTestWriteUAV, NULL );
            GetFramework()->GetEffectiveContext()->CSSetShaderResources( 0 , 1, & m_pTestReadSRV);
            GetFramework()->GetEffectiveContext()->Dispatch( cBackBufferSize, cBackBufferSize, 1 );
            ID3D11UnorderedAccessView * pUAV=0;
            ID3D11ShaderResourceView * pSRV=0;
            GetFramework()->GetEffectiveContext()->CSSetUnorderedAccessViews( 0 , 1, & pUAV, NULL );
            GetFramework()->GetEffectiveContext()->CSSetShaderResources( 0 , 1, & pSRV);
            GetFramework()->GetEffectiveContext()->OMSetRenderTargets( 1, &pRT, 0 );

            //retrieve the computed frame and compare it against CPU-generated frame
            WindowsTest::CHandle< ID3D11Resource > pBB0;
            m_pTestWriteUAV->GetResource(&pBB0);
            GetFramework()->GetEffectiveContext()->CopyResource( m_pStagingCompare, pBB0 );

            ThrowTRFailure( ExecuteEffectiveContext(), "ExecuteEffectiveContext() failed." );

            D3D11_MAPPED_SUBRESOURCE mrSource;
            D3D11_MAPPED_SUBRESOURCE mrDest;
            D3D11_MAPPED_SUBRESOURCE mrGPU;
            ThrowTRFailure( GetFramework()->GetImmediateContext()->Map( m_pStaging2D1, 0, D3D11_MAP_READ_WRITE, 0, & mrSource ),
                "Couldn't map staging buffer to read test pattern");
            ThrowTRFailure( GetFramework()->GetImmediateContext()->Map( m_pStaging2D0, 0, D3D11_MAP_READ_WRITE, 0, & mrDest ),
                "Couldn't map staging buffer to write test pattern");
            ThrowTRFailure( GetFramework()->GetImmediateContext()->Map( m_pStagingCompare, 0, D3D11_MAP_READ_WRITE, 0, & mrGPU ),
                "Couldn't map staging buffer to read GPU result");

            DWORD * pSource = (DWORD*) mrSource.pData;
            UINT SourceRowPitch = mrSource.RowPitch / sizeof(*pSource);

            DWORD * pDest = (DWORD*) mrDest.pData;
            UINT DestRowPitch = mrDest.RowPitch / sizeof(*pDest);

            DWORD * pGPU = (DWORD*) mrGPU.pData;
            UINT GPURowPitch = mrGPU.RowPitch / sizeof(*pGPU);

            for( INT w = 0; w < cBackBufferSize; ++w )
            {
                for( INT h = 0; h < cBackBufferSize; ++h )
                {
                    UINT cAliveNeighbors = 0;
                    cAliveNeighbors += pSource[ WrappedOffset( w, h, -1,  0, SourceRowPitch ) ] == 0 ? 0 : 1;
                    cAliveNeighbors += pSource[ WrappedOffset( w, h, -1, -1, SourceRowPitch ) ] == 0 ? 0 : 1;
                    cAliveNeighbors += pSource[ WrappedOffset( w, h, -1,  1, SourceRowPitch ) ] == 0 ? 0 : 1;
                    cAliveNeighbors += pSource[ WrappedOffset( w, h,  1,  0, SourceRowPitch ) ] == 0 ? 0 : 1;
                    cAliveNeighbors += pSource[ WrappedOffset( w, h,  1, -1, SourceRowPitch ) ] == 0 ? 0 : 1;
                    cAliveNeighbors += pSource[ WrappedOffset( w, h,  1,  1, SourceRowPitch ) ] == 0 ? 0 : 1;

                    if ( pSource[ WrappedOffset( w, h,  0,  0, SourceRowPitch ) ] )
                    {
                        if ( cAliveNeighbors <= 1 || cAliveNeighbors >= 4 )
                            pDest[ WrappedOffset( w, h,  0,  0, DestRowPitch ) ] = 0;
                        else
                            pDest[ WrappedOffset( w, h,  0,  0, DestRowPitch ) ] = 0xffffffff;
                    }
                    else
                    {
                        if ( cAliveNeighbors != 3 )
                            pDest[ WrappedOffset( w, h,  0,  0, DestRowPitch ) ] = 0;
                        else
                            pDest[ WrappedOffset( w, h,  0,  0, DestRowPitch ) ] = 0xffffffff;
                    }

                    if ( pDest[ WrappedOffset( w, h,  0,  0, DestRowPitch ) ] !=
                         pGPU[ WrappedOffset( w, h,  0,  0, GPURowPitch ) ] )
                    {
                        WriteToLog( _T("After %d iterations, GPU frame did not match CPU simulation at (%d,%d) "), iPass, w, h);
                        ThrowTRFailure( E_FAIL, "Test fails");
                    }
                }
            }

            GetFramework()->GetImmediateContext()->Unmap( m_pStaging2D0, 0 );
            GetFramework()->GetImmediateContext()->Unmap( m_pStaging2D1, 0 );
            GetFramework()->GetImmediateContext()->Unmap( m_pStagingCompare, 0 );

            Present();
            std::swap( m_pStaging2D0, m_pStaging2D1 );
        }
    }
    catch(TRException &exc)
    {
        tRes = exc.GetResult();
        exc.Prepend("[CInteractionPresentTest::ExecuteTestCase()] ");
        if (tRes != RESULT_PASS)
            WriteToLog( exc.GetError().c_str() );
    }
    
    return tRes;
}


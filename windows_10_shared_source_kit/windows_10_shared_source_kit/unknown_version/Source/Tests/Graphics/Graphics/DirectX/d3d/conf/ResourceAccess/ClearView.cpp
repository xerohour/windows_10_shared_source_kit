
// Includes
#include "resourceaccess.h"

const WCHAR * ClearView_Inl = L"Clearview.inl";

//------------------------------------------------------------------------------------------
DXGI_FORMAT MSAACheckFormats[] = {
    DXGI_FORMAT_R8G8B8A8_UNORM,
    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
    DXGI_FORMAT_R8G8B8A8_SNORM,
    DXGI_FORMAT_R16G16_SNORM,
    DXGI_FORMAT_B5G6R5_UNORM,
    DXGI_FORMAT_B5G5R5A1_UNORM,
    DXGI_FORMAT_B4G4R4A4_UNORM,
    DXGI_FORMAT_R32G32B32A32_FLOAT,
    DXGI_FORMAT_R32G32B32A32_SINT,
    DXGI_FORMAT_R32G32B32A32_UINT,
    DXGI_FORMAT_R16G16B16A16_FLOAT,
    DXGI_FORMAT_R16G16B16A16_SNORM,
    DXGI_FORMAT_R16G16B16A16_UINT,
    DXGI_FORMAT_R16G16B16A16_SINT,
    DXGI_FORMAT_R10G10B10A2_UNORM,
    DXGI_FORMAT_R10G10B10A2_UINT,

    DXGI_FORMAT_B8G8R8A8_UNORM,
    DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
    DXGI_FORMAT_B8G8R8X8_UNORM,
    DXGI_FORMAT_B8G8R8X8_UNORM_SRGB,
    DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
    DXGI_FORMAT_UNKNOWN
};

// Video formats generate a lot of skips
// So lets separate those formats out
DXGI_FORMAT MSAACheckFormats_Video[] = {
    DXGI_FORMAT_YUY2,
    DXGI_FORMAT_NV12,
	DXGI_FORMAT_UNKNOWN
};

DXGI_FORMAT MSAACheckFormats_Video_Mips[] = {
    DXGI_FORMAT_AYUV,
	DXGI_FORMAT_UNKNOWN
};

BEGIN_NAMED_VALUES( eColorScenario )
NAMED_VALUE( _T( "RedFractional"         )  ,eRedFractional        )
NAMED_VALUE( _T( "GreenIntegral"         )  ,eGreenIntegral        )
NAMED_VALUE( _T( "GreenOutofRange"       )  ,eGreenOutofRange      )
NAMED_VALUE( _T( "RedInf"                )  ,eRedInf               )
NAMED_VALUE( _T( "GreenNaN"              )  ,eGreenNaN             )
END_NAMED_VALUES( eColorScenario );

BEGIN_NAMED_VALUES( eViewType )
NAMED_VALUE( _T( "RenderTarget"     )  ,eRTV  )
NAMED_VALUE( _T( "UnorderedAccess"  )  ,eUAV  )
NAMED_VALUE( _T( "VideoProcess"     )  ,eVPV  )
NAMED_VALUE( _T( "VideoDecode"      )  ,eVDV  )
NAMED_VALUE( _T( "VideoInput"       )  ,eVIV  )
END_NAMED_VALUES( eViewType );

bool HasIntComponent( DXGI_FORMAT format )
{
    const UINT NumComponents = CD3D10FormatHelper_Test::GetNumComponentsInFormat( format );
    for( UINT i=0; i < NumComponents; i++ )
    {
        D3D10_FORMAT_COMPONENT_INTERPRETATION type = CD3D10FormatHelper_Test::GetFormatComponentInterpretation( format, i );
        if( type == D3D10FCI_SINT || type == D3D10FCI_UINT )
        {
            return true;
        }
    }
    return false;
}
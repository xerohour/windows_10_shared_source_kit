#include "DXGIFMT.h"

DXGI_FORMAT_INFO DXGIFormats[] =
{
{    DXGI_FORMAT_UNKNOWN                                     , 0,                0},
{    DXGI_FORMAT_R32G32B32A32_TYPELESS                       , 1,                128},
{        DXGI_FORMAT_R32G32B32A32_FLOAT                      , 2,                128},
{        DXGI_FORMAT_R32G32B32A32_UINT                       , 3,                128},
{        DXGI_FORMAT_R32G32B32A32_SINT                       , 4,                128},
{    DXGI_FORMAT_R32G32B32_TYPELESS                          , 5,                96},
{        DXGI_FORMAT_R32G32B32_FLOAT                         , 6,                96},
{        DXGI_FORMAT_R32G32B32_UINT                          , 7,                96},
{        DXGI_FORMAT_R32G32B32_SINT                          , 8,                96},
{    DXGI_FORMAT_R16G16B16A16_TYPELESS                       , 9,                64},
{       DXGI_FORMAT_R16G16B16A16_FLOAT                       , 10,                64},
{        DXGI_FORMAT_R16G16B16A16_UNORM                      , 11,                64},
{        DXGI_FORMAT_R16G16B16A16_UINT                       , 12,                64},
{        DXGI_FORMAT_R16G16B16A16_SNORM                      , 13,                64},
{        DXGI_FORMAT_R16G16B16A16_SINT                       , 14,                64},
{    DXGI_FORMAT_R32G32_TYPELESS                             , 15,                64},
{        DXGI_FORMAT_R32G32_FLOAT                            , 16,                64},
{        DXGI_FORMAT_R32G32_UINT                             , 17,                64},
{        DXGI_FORMAT_R32G32_SINT                             , 18,                64},
{    DXGI_FORMAT_R32G8X24_TYPELESS                           , 19,                64},
{        DXGI_FORMAT_D32_FLOAT_S8X24_UINT                    , 20,                64},
{        DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS                , 21,                64},
{        DXGI_FORMAT_X32_TYPELESS_G8X24_UINT                 , 22,                64},
{    DXGI_FORMAT_R10G10B10A2_TYPELESS                        , 23,                32},
{        DXGI_FORMAT_R10G10B10A2_UNORM                       , 24,                32},
{        DXGI_FORMAT_R10G10B10A2_UINT                        , 25,                32},
{    DXGI_FORMAT_R11G11B10_FLOAT                             , 26,                32},
{    DXGI_FORMAT_R8G8B8A8_TYPELESS                           , 27,                32},
{        DXGI_FORMAT_R8G8B8A8_UNORM                          , 28,                32},
{        DXGI_FORMAT_R8G8B8A8_UNORM_SRGB                     , 29,                32},
{        DXGI_FORMAT_R8G8B8A8_UINT                           , 30,                32},
{        DXGI_FORMAT_R8G8B8A8_SNORM                          , 31,                32},
{        DXGI_FORMAT_R8G8B8A8_SINT                           , 32,                32},
{    DXGI_FORMAT_R16G16_TYPELESS                             , 33,                32},
{        DXGI_FORMAT_R16G16_FLOAT                            , 34,                32},
{        DXGI_FORMAT_R16G16_UNORM                            , 35,                32},
{        DXGI_FORMAT_R16G16_UINT                             , 36,                32},
{        DXGI_FORMAT_R16G16_SNORM                            , 37,                32},
{        DXGI_FORMAT_R16G16_SINT                             , 38,                32},
{    DXGI_FORMAT_R32_TYPELESS                                , 39,                32},
{        DXGI_FORMAT_D32_FLOAT                               , 40,                32},
{        DXGI_FORMAT_R32_FLOAT                               , 41,                32},
{        DXGI_FORMAT_R32_UINT                                , 42,                32},
{        DXGI_FORMAT_R32_SINT                                , 43,                32},
{    DXGI_FORMAT_R24G8_TYPELESS                              , 44,                32},
{        DXGI_FORMAT_D24_UNORM_S8_UINT                       , 45,                32},
{        DXGI_FORMAT_R24_UNORM_X8_TYPELESS                   , 46,                32},
{        DXGI_FORMAT_X24_TYPELESS_G8_UINT                    , 47,                32},
{    DXGI_FORMAT_R8G8_TYPELESS                               , 48,                16},
{        DXGI_FORMAT_R8G8_UNORM                              , 49,                16},
{        DXGI_FORMAT_R8G8_UINT                               , 50,                16},
{        DXGI_FORMAT_R8G8_SNORM                              , 51,                16},
{        DXGI_FORMAT_R8G8_SINT                               , 52,                16},
{    DXGI_FORMAT_R16_TYPELESS                                , 53,                16},
{        DXGI_FORMAT_R16_FLOAT                               , 54,                16},
{        DXGI_FORMAT_D16_UNORM                               , 55,                16},
{        DXGI_FORMAT_R16_UNORM                               , 56,                16},
{        DXGI_FORMAT_R16_UINT                                , 57,                16},
{        DXGI_FORMAT_R16_SNORM                               , 58,                16},
{        DXGI_FORMAT_R16_SINT                                , 59,                16},
{    DXGI_FORMAT_R8_TYPELESS                                 , 60,                8},
{        DXGI_FORMAT_R8_UNORM                                , 61,                8},
{        DXGI_FORMAT_R8_UINT                                 , 62,                8},
{        DXGI_FORMAT_R8_SNORM                                , 63,                8},
{        DXGI_FORMAT_R8_SINT                                 , 64,                8},
{    DXGI_FORMAT_A8_UNORM                                    , 65,                8},
{    DXGI_FORMAT_R1_UNORM                                    , 66,                1},
{    DXGI_FORMAT_R9G9B9E5_SHAREDEXP                          , 67,                16},
{    DXGI_FORMAT_R8G8_B8G8_UNORM                             , 68,                0},
{    DXGI_FORMAT_G8R8_G8B8_UNORM                             , 69,                0},
{    DXGI_FORMAT_BC1_UNORM                                   , 70,                0},
{    DXGI_FORMAT_BC1_UNORM_SRGB                              , 71,                0},
{    DXGI_FORMAT_BC2_UNORM                                   , 72,                0},
{    DXGI_FORMAT_BC2_UNORM_SRGB                              , 73,                0},
{    DXGI_FORMAT_BC3_UNORM                                   , 74,                0},
{    DXGI_FORMAT_BC3_UNORM_SRGB                              , 75,                0},
{    DXGI_FORMAT_BC4_UNORM                                   , 76,                0},
{    DXGI_FORMAT_BC4_SNORM                                   , 77,                0},
{    DXGI_FORMAT_BC5_UNORM                                   , 78,                0},
{    DXGI_FORMAT_BC5_SNORM                                   , 79,                0}
//{    DXGI_FORMAT_R5G6B5_TYPELESS                             , 80,                16},
//{    DXGI_FORMAT_R5G5B5A1_TYPELESS                           , 81,                16}
};

CONST UINT cuiNoOfFormats = sizeof(DXGIFormats) / sizeof(DXGI_FORMAT_INFO);

CONST DXGI_FORMAT_INFO* FindFormatRecord(DXGI_FORMAT dxgiformatSearch)
{
	if (cuiNoOfFormats)
	{
		UINT uiFormatIndex = 0;

		do
		{
			if ((DXGIFormats[uiFormatIndex]).dxgiformat == dxgiformatSearch)
				return &(DXGIFormats[uiFormatIndex]);
		}
		while ((++uiFormatIndex) < cuiNoOfFormats);
	}

	return NULL;
}



////////////////////////////////////////////////////////////////////////////////
// Copied directly from DXGI source file d3dformat_dxgiformat.cpp on 08/18/2005

DXGI_FORMAT DXGI_Formats_From_Source[] =
{
    {DXGI_FORMAT_UNKNOWN									    },
    {DXGI_FORMAT_R32G32B32A32_TYPELESS                          },
    {    DXGI_FORMAT_R32G32B32A32_FLOAT                               },
    {    DXGI_FORMAT_R32G32B32A32_UINT                          },
    {    DXGI_FORMAT_R32G32B32A32_SINT                          },
    {DXGI_FORMAT_R32G32B32_TYPELESS                             },
    {    DXGI_FORMAT_R32G32B32_FLOAT                            },
    {    DXGI_FORMAT_R32G32B32_UINT                             },
    {    DXGI_FORMAT_R32G32B32_SINT                             },

    {    DXGI_FORMAT_R16G16B16A16_UNORM                              },
    {DXGI_FORMAT_R16G16B16A16_TYPELESS                               },
    {	DXGI_FORMAT_R16G16B16A16_FLOAT                                },
    {    DXGI_FORMAT_R16G16B16A16_UINT                               },
    {    DXGI_FORMAT_R16G16B16A16_SNORM                              },
    {    DXGI_FORMAT_R16G16B16A16_SNORM                              },
    {    DXGI_FORMAT_R16G16B16A16_SINT                               },

    {DXGI_FORMAT_R32G32_TYPELESS                                },
    {    DXGI_FORMAT_R32G32_FLOAT                               },
    {    DXGI_FORMAT_R32G32_UINT                                },
    {    DXGI_FORMAT_R32G32_SINT                                },
    {DXGI_FORMAT_R32G8X24_TYPELESS                              },
    {    DXGI_FORMAT_D32_FLOAT_S8X24_UINT                       },
    {    DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS                   },
    {    DXGI_FORMAT_X32_TYPELESS_G8X24_UINT                    },

    {    DXGI_FORMAT_R10G10B10A2_UNORM                          },
    {DXGI_FORMAT_R10G10B10A2_TYPELESS                               },
    {    DXGI_FORMAT_R10G10B10A2_UINT                           },
    {DXGI_FORMAT_R11G11B10_FLOAT                                },

    {    DXGI_FORMAT_R8G8B8A8_UNORM                             },
    {DXGI_FORMAT_R8G8B8A8_TYPELESS                              },
    {    DXGI_FORMAT_R8G8B8A8_UNORM_SRGB                        },
    {    DXGI_FORMAT_R8G8B8A8_UINT                              },
    {    DXGI_FORMAT_R8G8B8A8_SNORM                             },
    {    DXGI_FORMAT_R8G8B8A8_SINT                              },

    {    DXGI_FORMAT_R16G16_UNORM                              },
    {DXGI_FORMAT_R16G16_TYPELESS                               },
    {    DXGI_FORMAT_R16G16_FLOAT                               },
    {    DXGI_FORMAT_R16G16_UINT                               },
    {    DXGI_FORMAT_R16G16_SNORM                              },
    {    DXGI_FORMAT_R16G16_SINT                               },

    {DXGI_FORMAT_R32_TYPELESS                                   },
    {    DXGI_FORMAT_D32_FLOAT                                        },
    {    DXGI_FORMAT_R32_FLOAT                               },
    {    DXGI_FORMAT_R32_UINT                                   },
    {    DXGI_FORMAT_R32_SINT                                   },
    {DXGI_FORMAT_R24G8_TYPELESS                                  },
    {    DXGI_FORMAT_D24_UNORM_S8_UINT                         },
    {    DXGI_FORMAT_R24_UNORM_X8_TYPELESS                       },
    {    DXGI_FORMAT_X24_TYPELESS_G8_UINT                        },

    {DXGI_FORMAT_R8G8_TYPELESS                                   },
    {    DXGI_FORMAT_R8G8_UNORM                               },
    {    DXGI_FORMAT_R8G8_UINT                                   },
    {    DXGI_FORMAT_R8G8_SNORM                                  },
    {    DXGI_FORMAT_R8G8_SINT                              ,   },    

    {    DXGI_FORMAT_R16_UNORM                               },
    {DXGI_FORMAT_R16_TYPELESS                                    },
    {    DXGI_FORMAT_R16_FLOAT                                },
    {    DXGI_FORMAT_D16_UNORM                                },
    {    DXGI_FORMAT_D16_UNORM                                 },
    {    DXGI_FORMAT_R16_UINT                                     },
    {    DXGI_FORMAT_R16_SNORM                                    },
    {    DXGI_FORMAT_R16_SINT                                     },

    {    DXGI_FORMAT_R8_UNORM                                },
    {DXGI_FORMAT_R8_TYPELESS                                 },
    {    DXGI_FORMAT_R8_UINT                                 },
    {    DXGI_FORMAT_R8_SNORM                                },
    {    DXGI_FORMAT_R8_SINT                                 },
    {DXGI_FORMAT_A8_UNORM                                    },

    {DXGI_FORMAT_R1_UNORM                                         },
    {DXGI_FORMAT_R9G9B9E5_SHAREDEXP                               },
    {DXGI_FORMAT_R8G8_B8G8_UNORM                                    },
    {DXGI_FORMAT_G8R8_G8B8_UNORM                                    },
    {DXGI_FORMAT_BC1_UNORM                                     },
    {DXGI_FORMAT_BC1_UNORM_SRGB                                },
    {DXGI_FORMAT_BC2_UNORM                                     },
    {DXGI_FORMAT_BC2_UNORM_SRGB                                },
    {DXGI_FORMAT_BC3_UNORM                                     },
    {DXGI_FORMAT_BC3_UNORM_SRGB                                },
    {DXGI_FORMAT_BC4_UNORM                                        },
    {DXGI_FORMAT_BC4_SNORM                                        },
    {DXGI_FORMAT_BC5_UNORM                                        },
    {DXGI_FORMAT_BC5_SNORM                                        },
    {DXGI_FORMAT_B5G6R5_UNORM                                    },
    {DXGI_FORMAT_B5G5R5A1_UNORM                                    },
    {DXGI_FORMAT_B8G8R8A8_UNORM                              }    
};

const UINT cuiNoOfFormatsFromSource = sizeof(DXGI_Formats_From_Source) / sizeof(DXGI_FORMAT);

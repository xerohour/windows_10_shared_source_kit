// Includes
#include "resourceaccess.h"
#include "RectScenarios.h"

// Rect assumes mip level 0 has size Edge0, and mip level 1 has size Edge1
// If a test needs different values it can change them on the fly
const LONG Edge0 = 64;
const LONG Edge1 = 32;

BEGIN_NAMED_VALUES( eRectScenario )
NAMED_VALUE( _T( "NULLRectList"                                              )  ,eNULLRectList                                              )
NAMED_VALUE( _T( "OneRectCoveringQuadrant1"                                  )  ,eOneRectCoveringQuadrant1                                  )
NAMED_VALUE( _T( "OneRectCoveringQuadrant2"                                  )  ,eOneRectCoveringQuadrant2                                  )
NAMED_VALUE( _T( "OneRectCoveringQuadrant3"                                  )  ,eOneRectCoveringQuadrant3                                  )
NAMED_VALUE( _T( "OneRectCoveringQuadrant4"                                  )  ,eOneRectCoveringQuadrant4                                  )
NAMED_VALUE( _T( "OneRectFullCoverage"                                       )  ,eOneRectFullCoverage                                       )
NAMED_VALUE( _T( "OneRectFullCoverageAndThenSomeRightBottom"                 )  ,eOneRectFullCoverageAndThenSomeRightBottom                 )
NAMED_VALUE( _T( "OneRectFullCoverageAndThenSomeTopLeft"                     )  ,eOneRectFullCoverageAndThenSomeTopLeft                     )
NAMED_VALUE( _T( "OneRectCompletelyOutside"                                  )  ,eOneRectCompletelyOutside                                  )
NAMED_VALUE( _T( "OneRectFourPixels"                                         )  ,eOneRectFourPixels                                         )
NAMED_VALUE( _T( "TwoIdenticalRects"                                         )  ,eTwoIdenticalRects                                         )
NAMED_VALUE( _T( "TwoPartialOverlap"                                         )  ,eTwoPartialOverlap                                         )
NAMED_VALUE( _T( "TwoRectsFullCoverage"                                      )  ,eTwoRectsFullCoverage                                      )
NAMED_VALUE( _T( "FourRectsTwoPixelGap"                                      )  ,eFourRectsTwoPixelGap                                      )
NAMED_VALUE( _T( "OneEmptyRectTopEqualsBottom"                               )  ,eOneEmptyRectTopEqualsBottom                               )
NAMED_VALUE( _T( "OneEmptyRectTopGreaterThanBottom"                          )  ,eOneEmptyRectTopGreaterThanBottom                          )
NAMED_VALUE( _T( "TwoRectCoveringQuadrant1WithEmptyRectLeftEqualsRight"      )  ,eTwoRectCoveringQuadrant1WithEmptyRectLeftEqualsRight      )
NAMED_VALUE( _T( "TwoRectCoveringQuadrant1WithEmptyRectLeftGreaterThanRight" )  ,eTwoRectCoveringQuadrant1WithEmptyRectLeftGreaterThanRight )
END_NAMED_VALUES( eRectScenario );


															// Left	  // Top      // Right       // Bottom
D3D11_RECT Mip0OneRectCoveringQuadrant1            [] = { { 0,        0,          Edge0/2,       Edge0/2     } };
D3D11_RECT Mip0OneRectCoveringQuadrant2            [] = { { Edge0/2,  0,          Edge0,         Edge0/2     } };
D3D11_RECT Mip0OneRectCoveringQuadrant3            [] = { { 0,        Edge0/2,    Edge0/2,       Edge0       } };
D3D11_RECT Mip0OneRectCoveringQuadrant4            [] = { { Edge0/2,  Edge0/2,    Edge0,         Edge0       } };
D3D11_RECT Mip0OneRectFullCoverage                 [] = { { 0,        0,          Edge0  ,       Edge0       } };
D3D11_RECT Mip0OneRectFullCoveragePlusRightBottom  [] = { { 0,        0,          Edge0+8,       Edge0+8     } };
D3D11_RECT Mip0OneRectFullCoveragePlusTopLeft      [] = { { -8,      -8,          Edge0,         Edge0       } };
D3D11_RECT Mip0OneRectCompletelyOutside            [] = { { -8,      -8,          -4,            -2          } };
D3D11_RECT Mip0OneRectFourPixels                   [] = { { 4,        4,           8,            8           } };
D3D11_RECT Mip0TwoIdenticalRects                   [] = { { Edge0/4,  Edge0/4,    (Edge0*3)/4,  (Edge0*3)/4  },
                                                          { Edge0/4,  Edge0/4,    (Edge0*3)/4,  (Edge0*3)/4  } };
D3D11_RECT Mip0TwoPartialOverlap                   [] = { { 0,        0,          (Edge0*3)/4,  (Edge0*3)/4  },
                                                          { Edge0/4,  Edge0/4,    Edge0,        Edge0        } };
D3D11_RECT Mip0TwoRectsFullCoverage                [] = { { 0,        0,          Edge0,        Edge0/2      },
                                                          { 0,        Edge0/2+2,  Edge0,        Edge0        } };
D3D11_RECT Mip0FourRectsTwoPixelGap                [] = { { 0,        0,          4,            2            },
                                                          { 8,        0,          Edge0,        Edge0/2      },
                                                          { 0,        4,          4,            Edge0        },
                                                          { 8,        Edge0/2+2,  Edge0,        Edge0        } };

// Win8: 625909: Empty rect cases (ClearView performs a no-op)
// An empty rectangle is one that has no area; that is, the coordinate of the right side is less than or equal to the coordinate of the left side, 
// or the coordinate of the bottom side is less than or equal to the coordinate of the top side.
															// Left	  // Top      // Right       // Bottom
D3D11_RECT Mip0OneEmptyRectTopEqualsBottom         [] = { { 0,        0,          Edge0,        0            } };	// Empty because top equals bottom
D3D11_RECT Mip0OneEmptyRectTopGreaterThanBottom    [] = { { 0,        Edge0,      Edge0,        0            } };	// Empty because top is greater than bottom
D3D11_RECT Mip0TwoRectCoveringQuadrant1WithEmptyRectLeftEqualsRight
	                                               [] = { { 0,        0,          Edge0/2,      Edge0/2      },
														  { Edge0/2,  0,          Edge0/2,      Edge0/2      } };	// Empty because left equals right
D3D11_RECT Mip0TwoRectCoveringQuadrant1WithEmptyRectLeftGreaterThanRight
	                                               [] = { { 0,        0,          Edge0/2,      Edge0/2      },
														  { Edge0/2+4,0,          Edge0/2,      Edge0/2      } };	// Empty because left is greater than right


SRectScenarios RectScenariosMip0[] = 
{
    NULL, 0 , eNULLRectList,
    Mip0OneRectCoveringQuadrant1                                 , ARRAYSIZE( Mip0OneRectCoveringQuadrant1                                  )  ,eOneRectCoveringQuadrant1                                 ,
    Mip0OneRectCoveringQuadrant2                                 , ARRAYSIZE( Mip0OneRectCoveringQuadrant2                                  )  ,eOneRectCoveringQuadrant2                                 ,
    Mip0OneRectCoveringQuadrant3                                 , ARRAYSIZE( Mip0OneRectCoveringQuadrant3                                  )  ,eOneRectCoveringQuadrant3                                 ,
    Mip0OneRectCoveringQuadrant4                                 , ARRAYSIZE( Mip0OneRectCoveringQuadrant4                                  )  ,eOneRectCoveringQuadrant4                                 ,
    Mip0OneRectFullCoverage                                      , ARRAYSIZE( Mip0OneRectFullCoverage                                       )  ,eOneRectFullCoverage                                      ,
    Mip0OneRectFullCoveragePlusRightBottom                       , ARRAYSIZE( Mip0OneRectFullCoveragePlusRightBottom                        )  ,eOneRectFullCoverageAndThenSomeRightBottom                ,
    Mip0OneRectFullCoveragePlusTopLeft                           , ARRAYSIZE( Mip0OneRectFullCoveragePlusTopLeft                            )  ,eOneRectFullCoverageAndThenSomeTopLeft                    ,
    Mip0OneRectCompletelyOutside                                 , ARRAYSIZE( Mip0OneRectCompletelyOutside                                  )  ,eOneRectCompletelyOutside                                 ,
    Mip0OneRectFourPixels                                        , ARRAYSIZE( Mip0OneRectFourPixels                                         )  ,eOneRectFourPixels                                        ,
    Mip0TwoIdenticalRects                                        , ARRAYSIZE( Mip0TwoIdenticalRects                                         )  ,eTwoIdenticalRects                                        ,
    Mip0TwoPartialOverlap                                        , ARRAYSIZE( Mip0TwoPartialOverlap                                         )  ,eTwoPartialOverlap                                        ,
    Mip0TwoRectsFullCoverage                                     , ARRAYSIZE( Mip0TwoRectsFullCoverage                                      )  ,eTwoRectsFullCoverage                                     ,
    Mip0FourRectsTwoPixelGap                                     , ARRAYSIZE( Mip0FourRectsTwoPixelGap                                      )  ,eFourRectsTwoPixelGap                                     ,
	Mip0OneEmptyRectTopEqualsBottom                              , ARRAYSIZE( Mip0OneEmptyRectTopEqualsBottom                               )  ,eOneEmptyRectTopEqualsBottom                              ,
	Mip0OneEmptyRectTopGreaterThanBottom                         , ARRAYSIZE( Mip0OneEmptyRectTopGreaterThanBottom                          )  ,eOneEmptyRectTopGreaterThanBottom                         ,
	Mip0TwoRectCoveringQuadrant1WithEmptyRectLeftEqualsRight     , ARRAYSIZE( Mip0TwoRectCoveringQuadrant1WithEmptyRectLeftEqualsRight      )  ,eTwoRectCoveringQuadrant1WithEmptyRectLeftEqualsRight     ,
	Mip0TwoRectCoveringQuadrant1WithEmptyRectLeftGreaterThanRight, ARRAYSIZE( Mip0TwoRectCoveringQuadrant1WithEmptyRectLeftGreaterThanRight )  ,eTwoRectCoveringQuadrant1WithEmptyRectLeftGreaterThanRight,
};
														// Left	  // Top      // Right       // Bottom
D3D11_RECT Mip1OneRectCoveringQuadrant1            [] = { { 0,        0,          Edge1/2,       Edge1/2     } };
D3D11_RECT Mip1OneRectCoveringQuadrant2            [] = { { Edge1/2,  0,          Edge1,         Edge1/2     } };
D3D11_RECT Mip1OneRectCoveringQuadrant3            [] = { { 0,        Edge1/2,    Edge1/2,       Edge1       } };
D3D11_RECT Mip1OneRectCoveringQuadrant4            [] = { { Edge1/2,  Edge1/2,    Edge1,         Edge1       } };
D3D11_RECT Mip1OneRectFullCoverage                 [] = { { 0,        0,          Edge1  ,       Edge1       } };
D3D11_RECT Mip1OneRectFullCoveragePlusRightBottom  [] = { { 0,        0,          Edge1+8,       Edge1+8     } };
D3D11_RECT Mip1OneRectFullCoveragePlusTopLeft      [] = { { -8,      -8,          Edge1,         Edge1       } };
D3D11_RECT Mip1OneRectCompletelyOutside            [] = { { -8,      -8,          -4,           -2           } };
D3D11_RECT Mip1OneRectFourPixels                   [] = { { 4,        4,           8,            8           } };
D3D11_RECT Mip1TwoIdenticalRects                   [] = { { Edge1/4,  Edge1/4,    (Edge1*3)/4,  (Edge1*3)/4  },
                                                          { Edge1/4,  Edge1/4,    (Edge1*3)/4,  (Edge1*3)/4  } };
D3D11_RECT Mip1TwoPartialOverlap                   [] = { { 0,        0,          (Edge1*3)/4,  (Edge1*3)/4  },
                                                          { Edge1/4,  Edge1/4,    Edge1,        Edge1        } };
D3D11_RECT Mip1TwoRectsFullCoverage                [] = { { 0,        0,          Edge1,        Edge1/2      },
                                                          { 0,        Edge1/2+2,  Edge1,        Edge1        } };
D3D11_RECT Mip1FourRectsTwoPixelGap                [] = { { 0,        0,          4,            2            },
                                                          { 8,        0,          Edge1,        Edge1/2      },
                                                          { 0,        4,          4,            Edge1        },
                                                          { 8,        Edge1/2+2,  Edge1,        Edge1        } };
D3D11_RECT Mip1OneEmptyRectTopEqualsBottom         [] = { { 0,        0,          Edge1,        0            } };	// Empty because top equals bottom
D3D11_RECT Mip1OneEmptyRectTopGreaterThanBottom    [] = { { 0,        Edge1,      Edge1,        0            } };	// Empty because top is greater than bottom
D3D11_RECT Mip1TwoRectCoveringQuadrant1WithEmptyRectLeftEqualsRight
	                                               [] = { { 0,        0,          Edge1/2,      Edge1/2      },
														  { Edge1/2,  0,          Edge1/2,      Edge1/2      } };	// Empty because left equals right
D3D11_RECT Mip1TwoRectCoveringQuadrant1WithEmptyRectLeftGreaterThanRight
	                                               [] = { { 0,        0,          Edge1/2,      Edge1/2      },
														  { Edge1/2+2,0,          Edge1/2,      Edge1/2      } };	// Empty because left is greater than right

SRectScenarios RectScenariosMip1[] = 
{
    NULL, 0 , eNULLRectList,
    Mip1OneRectCoveringQuadrant1                                 , ARRAYSIZE( Mip1OneRectCoveringQuadrant1                                  )  ,eOneRectCoveringQuadrant1                                 ,
    Mip1OneRectCoveringQuadrant2                                 , ARRAYSIZE( Mip1OneRectCoveringQuadrant2                                  )  ,eOneRectCoveringQuadrant2                                 ,
    Mip1OneRectCoveringQuadrant3                                 , ARRAYSIZE( Mip1OneRectCoveringQuadrant3                                  )  ,eOneRectCoveringQuadrant3                                 ,
    Mip1OneRectCoveringQuadrant4                                 , ARRAYSIZE( Mip1OneRectCoveringQuadrant4                                  )  ,eOneRectCoveringQuadrant4                                 ,
    Mip1OneRectFullCoverage                                      , ARRAYSIZE( Mip1OneRectFullCoverage                                       )  ,eOneRectFullCoverage                                      ,
    Mip1OneRectFullCoveragePlusRightBottom                       , ARRAYSIZE( Mip1OneRectFullCoveragePlusRightBottom                        )  ,eOneRectFullCoverageAndThenSomeRightBottom                ,
    Mip1OneRectFullCoveragePlusTopLeft                           , ARRAYSIZE( Mip1OneRectFullCoveragePlusTopLeft                            )  ,eOneRectFullCoverageAndThenSomeTopLeft                    ,
    Mip1OneRectCompletelyOutside                                 , ARRAYSIZE( Mip1OneRectCompletelyOutside                                  )  ,eOneRectCompletelyOutside                                 ,
    Mip1OneRectFourPixels                                        , ARRAYSIZE( Mip1OneRectFourPixels                                         )  ,eOneRectFourPixels                                        ,
    Mip1TwoIdenticalRects                                        , ARRAYSIZE( Mip1TwoIdenticalRects                                         )  ,eTwoIdenticalRects                                        ,
    Mip1TwoPartialOverlap                                        , ARRAYSIZE( Mip1TwoPartialOverlap                                         )  ,eTwoPartialOverlap                                        ,
    Mip1TwoRectsFullCoverage                                     , ARRAYSIZE( Mip1TwoRectsFullCoverage                                      )  ,eTwoRectsFullCoverage                                     ,
    Mip1FourRectsTwoPixelGap                                     , ARRAYSIZE( Mip1FourRectsTwoPixelGap                                      )  ,eFourRectsTwoPixelGap                                     ,
	Mip1OneEmptyRectTopEqualsBottom                              , ARRAYSIZE( Mip1OneEmptyRectTopEqualsBottom                               )  ,eOneEmptyRectTopEqualsBottom                              ,
	Mip1OneEmptyRectTopGreaterThanBottom                         , ARRAYSIZE( Mip1OneEmptyRectTopGreaterThanBottom                          )  ,eOneEmptyRectTopGreaterThanBottom                         ,
	Mip1TwoRectCoveringQuadrant1WithEmptyRectLeftEqualsRight     , ARRAYSIZE( Mip1TwoRectCoveringQuadrant1WithEmptyRectLeftEqualsRight      )  ,eTwoRectCoveringQuadrant1WithEmptyRectLeftEqualsRight     ,
	Mip1TwoRectCoveringQuadrant1WithEmptyRectLeftGreaterThanRight, ARRAYSIZE( Mip1TwoRectCoveringQuadrant1WithEmptyRectLeftGreaterThanRight )  ,eTwoRectCoveringQuadrant1WithEmptyRectLeftGreaterThanRight,
};

bool IsD3DRectEmpty( const D3D11_RECT &rect )
{
	// The IsRectEmpty function determines whether the specified rectangle is empty. 
	// An empty rectangle is one that has no area; that is, the coordinate of the right side is 
	// less than or equal to the coordinate of the left side, 
	// or the coordinate of the bottom side is less than or equal to the coordinate of the top side.

	if( rect.right <= rect.left ||
		rect.bottom <= rect.top )
	{
		return true;
	}

	return false;
}
#pragma once

typedef enum { 
    eNULLRectList,
    eOneRectCoveringQuadrant1,
    eOneRectCoveringQuadrant2,
    eOneRectCoveringQuadrant3,
    eOneRectCoveringQuadrant4,
    eOneRectFullCoverage,
    eOneRectFullCoverageAndThenSomeRightBottom,
    eOneRectFullCoverageAndThenSomeTopLeft,
    eOneRectCompletelyOutside,
    eOneRectFourPixels, 
    eTwoIdenticalRects, 
    eTwoPartialOverlap, 
    eTwoRectsFullCoverage, 
    eFourRectsTwoPixelGap,
	eOneEmptyRectTopEqualsBottom,
	eOneEmptyRectTopGreaterThanBottom,
	eTwoRectCoveringQuadrant1WithEmptyRectLeftEqualsRight,
	eTwoRectCoveringQuadrant1WithEmptyRectLeftGreaterThanRight
} eRectScenario;

struct SRectScenarios
{
    RECT * pRects;
    UINT   Count;
    eRectScenario SanityCheck;
};

DECLARE_NAMED_VALUES( eRectScenario );
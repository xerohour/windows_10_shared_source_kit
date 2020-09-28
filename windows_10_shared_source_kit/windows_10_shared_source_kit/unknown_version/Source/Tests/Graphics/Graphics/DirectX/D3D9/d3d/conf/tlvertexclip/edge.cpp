// FILE:        edge.cpp
// DESC:        TL vertex clip conformance tests (edge)

#include "TLVertexClip.h"

// NAME:        CTLVertexClipEdge()
// DESC:        TL vertex clip edge class constructor
// INPUT:       none
// OUTPUT:      none

CTLVertexClipEdge::CTLVertexClipEdge()
{
	m_nTests = EDGE_TESTS;
	m_szTestName = "TLVertexClip Edge";
	m_szCommandKey = "Edge";
}

// NAME:        ~CTLVertexClipEdge()
// DESC:        TL vertex clip edge class destructor
// INPUT:       none
// OUTPUT:      none

CTLVertexClipEdge::~CTLVertexClipEdge()
{
}

// NAME:        SetEndPoints()
// DESC:        set end points
// INPUT:       A........end point
//              B........end point
//              uTest....test number
// OUTPUT:      none
// COMMENTS:    end points define lines around viewport edges

void CTLVertexClipEdge::SetEndPoints(D3DVECTOR *A, D3DVECTOR *B, UINT uTest)
{
	int	nOffset = (((int) uTest) % TESTS_PER_EDGE) - 1;

	A->z = 0;
	B->z = 0;

    switch (uTest/TESTS_PER_EDGE)
    {
        case 0:
            A->x = CLIP_X;
            A->y = (float) m_Port.Top() + nOffset;
            B->x = CLIP_WIDTH-1.0f;
            B->y = (float) m_Port.Top() + nOffset;
        break; // draw a horizontal line at the top

        case 1:
            A->x = CLIP_X;
            A->y = (float) m_Port.Bottom() - nOffset;
            B->x = CLIP_WIDTH-1.0f;
            B->y = (float) m_Port.Bottom() - nOffset;
        break; // draw a horizontal line at the bottom

        case 2:
            A->x = (float) m_Port.Left() + nOffset;
            A->y = CLIP_Y;
            B->x = (float) m_Port.Left() + nOffset;
            B->y = CLIP_HEIGHT-1.0f;
        break; // draw a vertical line at the left

        case 3:
            A->x = (float) m_Port.Right() - nOffset;
            A->y = CLIP_Y;
            B->x = (float) m_Port.Right() - nOffset;
            B->y = CLIP_HEIGHT-1.0f;
        break; // draw a vertical line at the right

        default:
            A->x = 0;
            A->y = 0;
            B->x = 0;
            B->y = 0;
        break;
    }
}

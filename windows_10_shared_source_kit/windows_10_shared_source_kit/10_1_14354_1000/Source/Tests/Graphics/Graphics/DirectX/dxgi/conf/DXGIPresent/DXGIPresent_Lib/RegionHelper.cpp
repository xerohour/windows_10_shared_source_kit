#include <windows.h>
#include <RegionHelper.h>
#include <StrSafe.h>


//-----------------------------------------------------------------------------
// Constructor \ Destructor
//-----------------------------------------------------------------------------
RegionHelper::RegionHelper() :
    pDirtyRegion(NULL),
    m_NumberOfRects(0)
{
};

RegionHelper::~RegionHelper()
{
    if(pDirtyRegion)
    {
        delete[] pDirtyRegion;
    }
};


//-----------------------------------------------------------------------------
//
//  Copy Constructors
//
//-----------------------------------------------------------------------------
RegionHelper::RegionHelper(
    const RegionHelper& rhs) 
{
    Copy(&rhs);
}

//-----------------------------------------------------------------------------
//
//  Overloaded Assignment Operator
//
//-----------------------------------------------------------------------------
const RegionHelper& RegionHelper::operator=(
    const RegionHelper& rhs)
{
    Copy(&rhs);
    return *this;
}

//-----------------------------------------------------------------------------
// AddRects
//-----------------------------------------------------------------------------
VOID RegionHelper::AddRects(
    RECT *pRects,
    UINT numberOfRects)
{
    HRESULT hr = S_OK;
    
    if(pDirtyRegion != NULL)
    {
        delete[] pDirtyRegion; 
        pDirtyRegion = NULL;
    }
    
    pDirtyRegion = new char[sizeof RGNDATAHEADER + sizeof RECT * numberOfRects];
    ((RGNDATA*)pDirtyRegion)->rdh.dwSize    = sizeof RGNDATAHEADER;
    ((RGNDATA*)pDirtyRegion)->rdh.iType        = RDH_RECTANGLES;
    ((RGNDATA*)pDirtyRegion)->rdh.nCount    = numberOfRects;
    ((RGNDATA*)pDirtyRegion)->rdh.nRgnSize    = sizeof RECT * numberOfRects;

    for ( UINT i = 0; i < numberOfRects; i++)
    {
        memcpy( ((RGNDATA*)pDirtyRegion)->Buffer + sizeof RECT * i, &pRects[i], sizeof RECT);
    }

    m_NumberOfRects = numberOfRects;
}

//-----------------------------------------------------------------------------
// Copy
//-----------------------------------------------------------------------------
VOID RegionHelper::Copy(const RegionHelper* pCopy)
{
    HRESULT hr = S_OK;
    pDirtyRegion = NULL;

    if(pCopy->pDirtyRegion != NULL)
    {
         pDirtyRegion = new char[sizeof RGNDATAHEADER + sizeof RECT * pCopy->m_NumberOfRects];
        ((RGNDATA*)pDirtyRegion)->rdh.dwSize = sizeof RGNDATAHEADER;
        ((RGNDATA*)pDirtyRegion)->rdh.iType = RDH_RECTANGLES;
        ((RGNDATA*)pDirtyRegion)->rdh.nCount = pCopy->m_NumberOfRects;
        ((RGNDATA*)pDirtyRegion)->rdh.nRgnSize = sizeof RECT * pCopy->m_NumberOfRects;

        for ( UINT i = 0; i < pCopy->m_NumberOfRects; i++)
        {
            memcpy( ((RGNDATA*)pDirtyRegion)->Buffer, ((RGNDATA*)pCopy->pDirtyRegion)->Buffer, sizeof RECT * pCopy->m_NumberOfRects);
        }
    }

    Description = pCopy->Description;
    m_NumberOfRects = pCopy->m_NumberOfRects;
}

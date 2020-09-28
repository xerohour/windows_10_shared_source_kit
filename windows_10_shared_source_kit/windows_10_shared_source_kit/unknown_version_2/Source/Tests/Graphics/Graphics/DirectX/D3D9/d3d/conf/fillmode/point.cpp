#include <d3d8.h>
#include "FillMode.h"


CPointFill::CPointFill()
{
    m_szTestName = _T("LemniSnake Fill Mode Point");
    m_szCommandKey = _T("Point");
}

UINT CPointFill::TestInitialize()
{
	UINT uResult = CFillMode::TestInitialize();
	if ( D3DTESTINIT_RUN != uResult )
		return uResult;

	SetRenderState(D3DRS_FILLMODE, (DWORD)D3DFILL_POINT);
	return uResult;
}


bool CPointFill::ExecuteTest(UINT uiTest)
{
    if ( m_Flags.Check(POINTSPRITE) && !m_Flags.Check(GUARDBAND) )
    {
        //This removes the second TRIANGLEFAN snake
        //and hence allows some of the TRIANGLEFAN
        //test that have point sprites to be valid
        //ie not clipped.
        m_Flags.Set(KILLSNAKE2);
    }
    
    if (!CFillMode::ExecuteTest(uiTest))
		return false;

	if (IsCompareEnabled())
	{
        if (m_uCurrentPrimType != D3DPT_LINELIST && 
            m_uCurrentPrimType != D3DPT_LINESTRIP )
        {
            // In this case we have points or point sprites
            // Here we need to check for guard band (GB) if
            // we have GB then we can continue as normal if 
            // not then the runtime will clip the primitives 
            // and send them down to the driver.
            // In this case a new point (point sprite) is 
            // generated at clipping point (i.e intersection 
            // with the view port) This generated point is not
            // tagged in any way by the runtime and hence the 
            // driver has no idea that it was generated in the 
            // clipping operation. This is a runtime issue that 
            // should not be exposed by this test MANBUG 51149.
            
            if (!m_Flags.Check(GUARDBAND))
            {
                if (!AllSnakeInView())
                {
                    WriteStatus("$gPoint Size", "N/A");
			        SetCompareSrcRef(false);
			        Pass();
                }
            }
        }
	}
	return true;
}
#include "ViewPorts.h"

class SplitStrip : public ViewportsTest
{
protected:
	virtual TEST_RESULT Setup();
public:
	SplitStrip()
	{
		m_RTHeight = 4;
		m_RTWidth = 33;
		g_App.m_RenderTargetOptions.SwapChainDesc.Height = m_RTHeight;
		g_App.m_RenderTargetOptions.SwapChainDesc.Width = m_RTWidth;
	}
	void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
};

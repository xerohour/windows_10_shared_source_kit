#include "WGFInterfaces.h"

class CInterfaceFlowControl : public CInterfacesTest
{
	public:
		virtual void InitTestParameters();
		virtual TEST_RESULT Setup();
		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();
		virtual void CleanupTestCase();
		virtual void Cleanup();

		CInterfaceFlowControl(){}
		virtual ~CInterfaceFlowControl(){}

		static const std::string s_ClassTypes[4];

	protected:

		class ShaderClassType;
		static const ShaderClassType* m_pShaderGlassTypes[4];
};

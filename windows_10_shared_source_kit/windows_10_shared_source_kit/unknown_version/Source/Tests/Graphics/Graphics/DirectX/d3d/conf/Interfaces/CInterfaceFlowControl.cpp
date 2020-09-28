#include "CInterfaceFlowControl.h"

class CInterfaceFlowControl::ShaderClassType
{
		ShaderClassType( ShaderClassType const& ) :m_pTypeName(NULL) {}
		ShaderClassType& operator =( ShaderClassType const& ) {}
	protected:
		LPCSTR const m_pTypeName;
		ShaderClassType( LPCSTR name ) :m_pTypeName(name) {}
	public:
		virtual LPCSTR GetTypeName() const { return m_pTypeName; }
		virtual unsigned int GetNumBytes() const { return 4; }
		virtual unsigned int GetNumTextureSlots() const { return 0; }
		virtual unsigned int GetNumSamplerSlots() const { return 0; }
		virtual HRESULT InitResources( ID3D11Device* ) { return S_OK; }
		virtual ID3D11ShaderResourceView *const* GetResources() const { return NULL; }
		virtual ID3D11SamplerState *const* GetSamplers() const { return NULL; }
		virtual ~ShaderClassType(){}
};

void CInterfaceFlowControl::InitTestParameters()
{

}

TEST_RESULT CInterfaceFlowControl::Setup()
{
	{
		TEST_RESULT tr = CInterfacesTest::Setup();
		if( tr != RESULT_PASS )
		{
			return tr;
		}
	}
	return RESULT_PASS;
}

TEST_RESULT CInterfaceFlowControl::SetupTestCase()
{
	{
		TEST_RESULT tr = CInterfacesTest::SetupTestCase();
		if( tr != RESULT_PASS )
		{
			return tr;
		}
	}
	return RESULT_PASS;
}

TEST_RESULT CInterfaceFlowControl::ExecuteTestCase()
{
	return RESULT_PASS;
}

void CInterfaceFlowControl::CleanupTestCase()
{
	CInterfacesTest::CleanupTestCase();
}

void CInterfaceFlowControl::Cleanup()
{
	CInterfacesTest::Cleanup();
}

const std::string CInterfaceFlowControl::s_ClassTypes[] = 
{
	"Test0",
	"Test1",
	"Test2",
	"Test3"
};

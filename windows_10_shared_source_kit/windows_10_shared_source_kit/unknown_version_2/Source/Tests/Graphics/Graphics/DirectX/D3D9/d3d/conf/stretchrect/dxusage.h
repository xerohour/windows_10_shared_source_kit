
	UINT	m_uMask;
};

//-----------------------------------------------------------------------------

class CShader5xTest_bufinfo : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_bufinfo() : CShader5xTest_ResourceAccess(_T("bufinfo")){}
	void		InitTestParameters();
	TEST_RESULT	SetupTestCase();
	TEST_RESULT Setup( );

	tstring GetShaderCode(AssemblyHelper* pAsmHelper);
	bool CheckResult( void *pResult, UINT uRowPitch );

private:
	AssemblyHelper::MaskFlag	m_uMask;
};

//-----------------------------------------------------------------------------

class CShader5xTest_ld_base : public CShader5xTest_ResourceAccess
{
public:
	CShader5xTest_ld_base(const TCHAR* Instruction) : CShader5xTest_ResourceAccess( Instruction, true ) {}
	TEST_RESULT	InitResource();
};

//-----------------------------------------------------------------------------

class CShader5xTest_ld : public
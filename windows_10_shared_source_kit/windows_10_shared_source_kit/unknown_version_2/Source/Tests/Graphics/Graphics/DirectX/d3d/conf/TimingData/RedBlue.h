class CRedBlueTest : public CTimingDataTest
{
public:
    CRedBlueTest() : fSkipExecution(false) {}
    
    virtual TEST_RESULT Setup();
    virtual TEST_RESULT SetupTestCase();
    virtual TEST_RESULT ExecuteTestCase();
    virtual TEST_RESULT DoExecuteTestCase();
    virtual void CleanupTestCase()
    {
        CTimingDataTest::CleanupTestCase();
    }
    TEST_RESULT Initialize();
    TEST_RESULT Second();
    TEST_RESULT SaveAsPNG(ID3D11DeviceContext* pD3DContext, ID3D11Resource* pBackBuffer, const wchar_t* FileName);
    bool fSkipExecution;
protected:
};

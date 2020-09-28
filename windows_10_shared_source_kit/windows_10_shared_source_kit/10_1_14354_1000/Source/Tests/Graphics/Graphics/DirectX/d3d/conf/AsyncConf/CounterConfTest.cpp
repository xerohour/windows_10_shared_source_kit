#include "AsyncConf.h" 

// TODO: test well-known counters
// TODO: test custom counters

TEST_RESULT
CCounterConfTest::Setup()
{
	CAsyncConfTest::Setup();
	D3D11_COUNTER_INFO counterInfo;
	GetDevice()->CheckCounterInfo(&counterInfo);
	if( counterInfo.LastDeviceDependentCounter > 0 )
		SkipMatching( FilterEqual( _T( "CounterName" ), D3D11_COUNTER_DEVICE_DEPENDENT_0 ) && FilterGreaterEqual( _T( "Dev_Dep_Ctr" ), counterInfo.LastDeviceDependentCounter - D3D11_COUNTER_DEVICE_DEPENDENT_0 ) );
	else
		SkipMatching( FilterEqual( _T( "CounterName" ), D3D11_COUNTER_DEVICE_DEPENDENT_0 ) && FilterGreaterEqual( _T( "Dev_Dep_Ctr" ), 0 ) );
	return RESULT_PASS;
}

void
CCounterConfTest::Cleanup()
{
	CAsyncConfTest::Cleanup();
}

void CCounterConfTest::InitTestParameters()
{
	CUserValueSet<D3D11_COUNTER> *pCounterNameValues = new CUserValueSet< D3D11_COUNTER >();

	AddParameter<D3D11_COUNTER>( _T( "CounterName" ), &m_CounterName);
	testfactor::RFactor rCounterName = AddParameterValueSet( _T( "CounterName"), pCounterNameValues );
	testfactor::RFactor rCounterName_Dev_Dep = AddParameterValue( _T( "CounterName"), D3D11_COUNTER_DEVICE_DEPENDENT_0 );
	AddParameter<int>( _T( "Dev_Dep_Ctr" ), &m_nDeviceDependentNbr);
	SetParameterDefaultValue<int>( _T( "Dev_Dep_Ctr" ), -1);
	testfactor::RFactor rDevDepCtr = AddParameterValueSet( _T( "Dev_Dep_Ctr"), RangeValueSet<int>(0, 100, 1) );

	testfactor::RFactor rGpuJob = AddParameter<int>( _T( "Num_GpuJobs" ), &m_nGpuJobs, RangeValueSet(0, 5, 1) );

	SetRootTestFactor( (rCounterName + rCounterName_Dev_Dep * rDevDepCtr) * rGpuJob );
}

TEST_RESULT CCounterConfTest::ExecuteTestCase()
{
	HRESULT hr = S_OK, hr2 = S_OK;
	TEST_RESULT tRes = RESULT_PASS;
	ID3D11Counter *pCounter = NULL;

	D3D11_COUNTER_DESC counterDesc;
	if (m_CounterName == D3D11_COUNTER_DEVICE_DEPENDENT_0)
		counterDesc.Counter = (D3D11_COUNTER)(m_CounterName + m_nDeviceDependentNbr);
	else
		counterDesc.Counter = m_CounterName;
	counterDesc.MiscFlags = 0;
	hr = GetDevice()->CreateCounter(&counterDesc, &pCounter);
	if ( FAILED(hr) || pCounter == NULL)
	{
		if (hr == DXGI_ERROR_UNSUPPORTED || counterDesc.Counter >= D3D11_COUNTER_DEVICE_DEPENDENT_0)
		{
			tRes = RESULT_SKIP;
			WriteToLog( _T("Unsupported Counter. hr=%s\n"), D3DHResultToString(hr).c_str() );
		}
		else
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T("Failed to create Counter. hr=%s\n"), D3DHResultToString(hr).c_str() );
		}
		goto testDone;
	}

	D3D11_COUNTER_TYPE sCounterType;
	UINT sActiveCounters;
	hr2 = GetDevice()->CheckCounter(
		&counterDesc, 
		&sCounterType, 
		&sActiveCounters, 
		NULL, NULL, 
		NULL, NULL,
		NULL, NULL
		);

	if ( FAILED(hr2) )
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("Failed to perform CheckCounter call for this counter type. hr=%s\n"), D3DHResultToString(hr2).c_str() );
		goto testDone;
	}

	SetupGpuJob();

	GetEffectiveContext()->Begin(pCounter);

	// TODO: draw or stream-out something

	for (int n = 0; n < m_nGpuJobs; ++n)
	{
		GpuBoundJob(false);
	}

	GetEffectiveContext()->End(pCounter);

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context.") );
		tRes = RESULT_FAIL;
		goto testDone;
	}

	// TODO: test counter for reasonable value
	if (m_CounterName >= D3D11_COUNTER_DEVICE_DEPENDENT_0)
	{

	}
	else
		assert(!"Unexpected counter value");


testDone:
	// cleanup
	SAFE_RELEASE( pCounter );

	return tRes;
}


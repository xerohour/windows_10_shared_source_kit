#pragma once

#include "D3D11Test.h"
#include "FlagCombinations.h"

const UINT DO_NOT_FILTER_FLAG = 0xffffffff;

struct D3DResourceFlags
{
	D3DResourceFlags()
	{
		CPUAccessFlags = DO_NOT_FILTER_FLAG;
		MiscFlags = DO_NOT_FILTER_FLAG;
		BindFlags = DO_NOT_FILTER_FLAG;
		Usage = (D3D11_USAGE)DO_NOT_FILTER_FLAG;
		Map = (D3D11_MAP)DO_NOT_FILTER_FLAG;
	}
	D3D11_USAGE	Usage;
	UINT		BindFlags;
	UINT		CPUAccessFlags;
	UINT		MiscFlags;
	D3D11_MAP		Map;
};

enum RESOURCE_ACCESS_METHOD
{
	RAC_NONE			= 0x0000,
	RAC_MAP				= 0x0001,
	RAC_UPDATE_SRC		= 0x0002,
	RAC_UPDATE_DEST		= 0x0004,
	RAC_COPY_SRC		= 0x0010,
	RAC_COPY_DEST		= 0x0020,
	RAC_COPYREGION_SRC	= 0x0040,
	RAC_COPYREGION_DEST	= 0x0080,
	RAC_RESOLVE_SRC		= 0x0100,
	RAC_RESOLVE_DEST	= 0x0200,
};

extern void __stdcall Tokenize( const tstring &str, std::vector<tstring> *pTokens );
extern tstring __stdcall ToString( const D3DResourceFlags* pFlags );
extern bool __stdcall FromString( D3DResourceFlags **ppFlags, const tstring &str );
DECLARE_NAMED_VALUES( RESOURCE_ACCESS_METHOD )

//////////////////////////////////////////////////////////////////////////
// CD3DResourceFlagsValueSet
//
// Provides a value set of all valid or invalid flag combinations
//
// NOTE:	In current implementation, these objects can't be destroyed
//			until the tests are done running because TableValueSet doesn't 
//			copy the values.
//////////////////////////////////////////////////////////////////////////
class CD3DResourceFlagsValueSet// : public CValueSet< D3DResourceFlags* >
{
public:
	//For the Filters :
	//0xffffffff means do not use filter for this field
	//There can only be one Included or one Excluded Usage at a time
	//For the other flags, any include and exclude can be set
	//so if you have Included.BindFlags = INDEX_BUFFER | STREAM_OUTPUT, it will include
	//all bindCombinations that have Index Buffer and Stream Output set, NOT all IndexBuffer and all Stream Output

	//	ResourceDimension 	// Resource type to get the value set
	//	bValid 				// Return valid or invalid combinations
	//	bGenerateMapCombos 	// Set to true to get the possible map values for that combination
	//	pIncludedFlags		// Flags that must be in the combinations.  If you combine flags, it will add combinations 
	//						// that uses either. For example, BIND_VB | BIND_IB will return all combinations with 
	//						// either BIND_VB or BIND_IB or both. 
	//	pExcludedFlags		// Flags that won’t be in the combinations.  If you combine flags, it will exlude combinations 
	//						// that uses either. For example, BIND_VB | BIND_IB will return all combinations that excludes 
	//						// both BIND_VB and BIND_IB. 
	//	AccessMethod		// only return combinations that are bValid for that method
	CD3DResourceFlagsValueSet( CTest *pTest, D3D11_RESOURCE_DIMENSION ResourceDimension, bool bValid, bool bGenerateMapCombos = false, D3DResourceFlags *pIncludedFlags = NULL, D3DResourceFlags *pExcludedFlags = NULL, RESOURCE_ACCESS_METHOD AccessMethod = RAC_NONE );
	~CD3DResourceFlagsValueSet();
	//bool GetValue( size_t nIndex, D3DResourceFlags **pValue ) const;

	size_t GetSize() const { return (size_t)m_NumD3DResourceFlags; }

	testfactor::RFactor CreateFactor( CTestCaseParameter<D3D11_USAGE> *pUsageParam, CTestCaseParameter<D3D11_BIND_FLAG> *pBindFlagsParam, CTestCaseParameter<D3D11_CPU_ACCESS_FLAG> *pCPUAccessFlagsParam, CTestCaseParameter<D3D11_RESOURCE_MISC_FLAG> *pMiscFlagsParam, CTestCaseParameter<D3D11_MAP> *pMapParam );
	
	
private:
	void GenerateD3DResourceFlags();

	D3D11_RESOURCE_DIMENSION m_ResourceDimension;
	bool m_bValid;

	CTest *m_pTest;

	D3DResourceFlags m_ExcludedFlags;
	D3DResourceFlags m_IncludedFlags;
	RESOURCE_ACCESS_METHOD m_AccessMethod;
	bool m_bGenerateMapCombos;

	D3DResourceFlags	m_ArrayD3DResourceFlags[1024];
	UINT				m_NumD3DResourceFlags;

	CTableValueSet<D3D11_USAGE> *m_pUsageValueSet;
	CTableValueSet<D3D11_BIND_FLAG> *m_pBindFlagsValueSet;
	CTableValueSet<D3D11_CPU_ACCESS_FLAG> *m_pCPUAccessFlagsValueSet;
	CTableValueSet<D3D11_RESOURCE_MISC_FLAG> *m_pMiscFlagsValueSet;
	CTableValueSet<D3D11_MAP> *m_pMapValueSet;
};

extern CD3DResourceFlagsValueSet* __stdcall D3DResourceFlagsValueSet( D3D11_RESOURCE_DIMENSION ResourceDimension, bool bValid );

#pragma once
#include "WGFInterfaces.h"
#include <tref.h>

const unsigned int PrimeNumbers[] = 
{3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167
,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349
,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541,547
,557,563,569,571,577,587,593,599,601,607,613,617,619,631,641,643,647,653,659,661,673,677,683,691,701,709,719,727,733};

#define CallTreeMaxDepth 48
struct StructCallTree
{
	unsigned int CallTreeList[CallTreeMaxDepth];
	StructCallTree( ) { }
	StructCallTree(const unsigned int * pCallTreeList)
	{
		unsigned int i = 0;
		while (i < CallTreeMaxDepth  &&  pCallTreeList[i] != 0)
		{
			CallTreeList[i] = pCallTreeList[i];
			i++;
		}
		while (i < CallTreeMaxDepth)
		{
			CallTreeList[i] = 0;
			i++;
		}
	}
	bool operator==(const StructCallTree &other) const ;
};

tstring ToString( StructCallTree const& );
bool FromString( StructCallTree*, tstring const& );

class CFcallLimits : public CInterfacesTest 
{
	public:
		virtual void InitTestParameters();
		virtual TEST_RESULT Setup();
		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();
		virtual void CleanupTestCase();
		virtual void Cleanup();
		CFcallLimits(){}
		virtual ~CFcallLimits(){}
	
	protected:
		const char* GetEntryPoint();
		TEST_RESULT CompileTestShader( const std::string& shaderSource );
		TEST_RESULT CompilePassthroughShaders();
		TEST_RESULT CreateOutputBuffers();
		TEST_RESULT ReadOutputBuffers(unsigned int result[16]);
		void BindResources( ID3D11ClassInstance** pInstances );
		const char* GetAssemblyHeader();

		unsigned int m_ExpectedResults[16];
		unsigned int m_bodies;
		unsigned int m_depth;
		unsigned int m_whichClass[CallTreeMaxDepth];

		TCOMPtr<ID3D11Buffer*> m_rConstBuffer;

		TCOMPtr<ID3D11VertexShader*>    m_rPassthroughVS1; // outputs POSITION
		TCOMPtr<ID3D11VertexShader*>    m_rPassthroughVS2; // outputs SV_POSITION
		TCOMPtr<ID3D11HullShader*>      m_rPassthroughHS;
		TCOMPtr<ID3D11DomainShader*>    m_rPassthroughDS;
		TCOMPtr<ID3D11GeometryShader*>  m_rPassthroughGS;

		TCOMPtr<ID3D10Blob*> m_rPassthroughVSByteCode;

		TCOMPtr<ID3D11VertexShader*>    m_rVertexShader;
		TCOMPtr<ID3D11HullShader*>      m_rHullShader;
		TCOMPtr<ID3D11DomainShader*>    m_rDomainShader;
		TCOMPtr<ID3D11GeometryShader*>  m_rGeometryShader;
		TCOMPtr<ID3D11PixelShader*>     m_rPixelShader;
		TCOMPtr<ID3D11ComputeShader*>   m_rComputeShader;

		TCOMPtr<ID3D11Buffer*>              m_rOutputBuffer;
		TCOMPtr<ID3D11Buffer*>              m_rOutputBufferCopy;
		TCOMPtr<ID3D11UnorderedAccessView*> m_rUAV;

		StructCallTree m_CallTree;

		D3D_SHADER_STAGE m_shaderStage;

		unsigned int m_Permutations;

		struct Vertex
		{
			float m_x;
			float m_y;
			float m_z;
		};
		static const Vertex s_pVertexData[4];

		unsigned int m_CalculatedResult[16];

		TCOMPtr<ID3D11RasterizerState*> m_rRastState;
		TCOMPtr<ID3D11DepthStencilState*> m_rDepthStencilState;
		TCOMPtr<ID3D11InputLayout*> m_rInputLayout;
		TCOMPtr<ID3D11Buffer*> m_rVertexBuffer;
};


#include "WGFCompute.h"
#include <tref.h>

class CAppendConsumeDynamicData : public CComputeTest
{
		CAppendConsumeDynamicData( CAppendConsumeDynamicData const& ){}
		CAppendConsumeDynamicData& operator =( CAppendConsumeDynamicData const& ) { return *this; }
	protected:

		// predefined data
		static const std::pair<unsigned int, unsigned int> s_NumAppendsTable[5];
		static const unsigned int s_pAppendCountData[1024];
		static const unsigned int s_pConsumeCountData[1024];

		CAppendConsumeDynamicData(){}
	public:
		virtual ~CAppendConsumeDynamicData(){}
};


class CAppendConsume : public CAppendConsumeDynamicData 
{
	public:
		virtual void InitTestParameters();
		virtual TEST_RESULT Setup();
		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();
		virtual void CleanupTestCase();
		virtual void Cleanup();
		CAppendConsume(){}
		virtual ~CAppendConsume(){}
	
	protected:
		TCOMPtr<ID3D11Buffer*> m_rDynamicAppendCounts;
		TCOMPtr<ID3D11Buffer*> m_rDynamicConsumeCounts;
		TCOMPtr<ID3D11Buffer*> m_rAppendConsumeBuffer;
		TCOMPtr<ID3D11ShaderResourceView*> m_rDynamicAppendSRV;
		TCOMPtr<ID3D11ShaderResourceView*> m_rDynamicConsumeSRV;
		TCOMPtr<ID3D11UnorderedAccessView*> m_rAppConUAV;
		std::vector< TCOMPtr<ID3D11Buffer*> > m_rDispatchIndexCB;
		TCOMPtr<ID3D11Buffer*> m_rFilledSizeStaging;
		TCOMPtr<ID3D11Buffer*> m_rConsumeLimits;
		// the m_pCS is the AppendShader
		TCOMPtr<ID3D11ComputeShader*> m_rConsumeShader;

		TCOMPtr<ID3D11Buffer*> m_rDebugBuffer;

		//tstring m_StorageClass;
		unsigned int m_StructureSize;
		unsigned int m_TotalThreadDispatchCount;
		unsigned int m_TotalAppendCount; // total number of dynamic appends expected.
		unsigned int m_UAVSizeOffset; // increase view/buffer size
		unsigned int m_NumAppendGroups[3];
		unsigned int m_NumConsumeGroups[3];
		unsigned int m_AppendNumGroupThreads[3];
		unsigned int m_ConsumeNumGroupThreads[3];
		unsigned int m_AppendCountOffset;  // The append view be given an initail offset for data already written.
		unsigned int m_ConsumeCountOffset; // The consume offset can cause fewer, or more consumes to be possible, even consumes outside the view boundaries.
		unsigned int m_NumAppendDispatches; // The number of times draw will be called to cover all appends.
		bool m_UseHardwareAppendCounter; // pertains to setting the consume UAV only

	private:
		unsigned int m_RealConsumeGroupCountX;
};

class CAppendConsumePS : public CAppendConsumeDynamicData 
{
	public:
		virtual void InitTestParameters();
		virtual TEST_RESULT Setup();
		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();
		virtual void CleanupTestCase();
		virtual void Cleanup();
		CAppendConsumePS(){}
		virtual ~CAppendConsumePS(){}

	protected:
		TCOMPtr<ID3D11Buffer*> m_rDynamicAppendCounts;
		TCOMPtr<ID3D11Buffer*> m_rDynamicConsumeCounts;
		TCOMPtr<ID3D11Buffer*> m_rAppendConsumeBuffer;
		TCOMPtr<ID3D11ShaderResourceView*> m_rDynamicAppendSRV;
		TCOMPtr<ID3D11ShaderResourceView*> m_rDynamicConsumeSRV;
		TCOMPtr<ID3D11UnorderedAccessView*> m_rAppConUAV;
		TCOMPtr<ID3D11Buffer*> m_rFilledSizeStaging;
		TCOMPtr<ID3D11Buffer*> m_rConsumeLimits;
		TCOMPtr<ID3D11VertexShader*> m_rVertexShader;
		TCOMPtr<ID3D11PixelShader*> m_rConsumeShader;
		TCOMPtr<ID3D11PixelShader*> m_rAppendShader;
		TCOMPtr<ID3D11RasterizerState*> m_rRastState;
		TCOMPtr<ID3D11DepthStencilState*> m_rDepthStencilState;
		TCOMPtr<ID3D11InputLayout*> m_rInputLayout;
		TCOMPtr<ID3D11Buffer*> m_rVertexBuffer;
		TCOMPtr<ID3D11Buffer*> m_rIndexBuffer;
		TCOMPtr<ID3D10Blob*> m_rVertexByteCode;
		TCOMPtr<ID3D11Query*> m_rQuery;

		TCOMPtr<ID3D11Buffer*> m_rDebugBuffer;

		float m_AppendViewportWidth;
		float m_AppendViewportHeight;
		float m_ConsumeViewportWidth;
		float m_ConsumeViewportHeight;
		unsigned int m_StructureSize;
		unsigned int m_TotalPixelDispatchCount;
		unsigned int m_TotalAppendCount; // total number of dynamic appends expected.
		unsigned int m_UAVSizeOffset; // increase view/buffer size
		unsigned int m_AppendCountOffset;  // The append view be given an initail offset for data already written.
		unsigned int m_ConsumeCountOffset; // The consume offset can cause fewer, or more consumes to be possible, even consumes outside the view boundaries.
		bool m_UseHardwareAppendCounter; // pertains to setting the consume UAV only

		struct Vertex
		{
			union 
			{
				struct
				{
					float m_XYZ[3];
				};
				struct
				{
					float m_X;
					float m_Y;
					float m_Z;
				};
			};
		};

		static const Vertex s_pVertexData[4];
		static const unsigned int s_pIndexBufferData[4];
};

#pragma once

#include "StreamOut.h"
#include <vector>
//////////////////////////////////////////////////////////////////////////
// CStreamOutMultiStream
//////////////////////////////////////////////////////////////////////////
#define MAX_NUMBER_OF_PATCHES 3
#define NUMBER_OF_32_ELEMENT_LAYOUTS 128
class CStreamOutPatches : public CStreamOutTest
{
	protected:
		struct Vertex
		{
			unsigned int m_Data[D3D11_GS_INPUT_REGISTER_COMPONENTS * D3D11_GS_INPUT_REGISTER_COUNT];
		};
		static Vertex s_pPatchVertexData[D3D11_IA_PATCH_MAX_CONTROL_POINT_COUNT * MAX_NUMBER_OF_PATCHES];
		struct DataUnit 
		{
			unsigned int m_NumCompotents;	// valid range [1-4]
			bool m_IgnoredDataGap;			// Indicate if this data is ignored in stream out.
			DataUnit( unsigned int n, bool i ):m_NumCompotents(n),m_IgnoredDataGap(i){}
		};
		std::vector<DataUnit> m_DataLayout;
		struct LayoutWindow
		{
			unsigned int m_StartIndex;
			unsigned int m_WindowCount;
			LayoutWindow( unsigned int s, unsigned int c ):m_StartIndex(s),m_WindowCount(c){}
		};
		std::vector<LayoutWindow> m_LayoutWindows;
		std::vector<D3D11_SO_DECLARATION_ENTRY> m_SOBufferDecls;

		unsigned int m_ControlPointSize;
		unsigned int m_LayoutIndex;
		unsigned int m_StreamOutBufferCount;
		unsigned int m_NumberOfPatchesToDraw;
		unsigned int m_NumberOfDrawCalls;

		void SubmitLayout( unsigned int layout[4], bool ignore[4], unsigned int layoutCount );
		void BuildLayout( unsigned int layout[4], bool ignore[4], unsigned int layoutCount, unsigned int layoutCompCount );

		TCOMPtr<ID3D11Buffer*> m_rBufferCopies[4];
	public:
		CStreamOutPatches(){}
		~CStreamOutPatches(){}

		virtual void InitTestParameters();
		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();
		virtual void CleanupTestCase();
		virtual TEST_RESULT Setup();
		virtual void Cleanup();
};

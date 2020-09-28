#pragma once

#include "shader5x.h"
#include <d3d11.h>
#include <map>

// forward decls due to circular includes
class CShader5xTest;

class AssemblyHelper
{
public:

	typedef tstring TypeName;
	typedef tstring Semantic;
	typedef tstring Literal;

	enum MaskFlag
	{
		noMask = 0,
		x = 0x1,
		y = 0x2,
		z = 0x4,
		w = 0x8,
		xy = 0x3,
		xyz = 0x7,
		xyzw = 0xf,
		zw = 0xc,
	};
	// example: Swizzle s = {z,w,noMask,noMask}
	union Swizzle
	{
		struct
		{
			MaskFlag r;
			MaskFlag g;
			MaskFlag b;
			MaskFlag a;
		};
		MaskFlag flags[4];
		operator==(const Swizzle& rhs) const
		{
			return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a;
		}
		void ToIndices(int indices[4])
		{
			for (int n = 0; n < 4; ++n)
			{
				switch (flags[n])
				{
				default: throw AHException("bad mask");
				case x: indices[n] = 0; break;
				case y: indices[n] = 1; break;
				case z: indices[n] = 2; break;
				case w: indices[n] = 3; break;
				}
			}
		}
	};

	enum MemoryType
	{
		raw,
		structured,
		typed
	};

	struct MemoryIndex
	{
		UINT index;
		UINT offset;
		bool offsetAsStr;
		tstring offsetStr;
		MemoryType memoryType;
		tstring prefix;

		operator==(const MemoryIndex& rhs) const
		{
			return index == rhs.index && offset == rhs.offset && memoryType == rhs.memoryType && prefix == rhs.prefix && offsetAsStr == rhs.offsetAsStr && offsetStr == rhs.offsetStr;
		}
	};

	struct SwizzlePairExtraSwizzle
	{
		int dstAddressId;
		Swizzle dstAddressSwizzle;
		Swizzle srcSwizzle;

		operator==(const SwizzlePairExtraSwizzle& rhs) const
		{
			return dstAddressSwizzle == rhs.dstAddressSwizzle && dstAddressId == rhs.dstAddressId && srcSwizzle == rhs.srcSwizzle;
		}
	};

	struct OuputMaskInfo
	{
		MaskFlag	DestMask;
		MemoryType	MemoryType;
		tstring		TypeName;
		tstring		ResourceName;
		UINT		Stride;

		operator==(const OuputMaskInfo& rhs) const
		{
			return this->DestMask == rhs.DestMask &&
				   this->MemoryType == rhs.MemoryType &&
				   this->TypeName == rhs.TypeName &&
				   this->ResourceName == rhs.ResourceName &&
				   this->Stride == rhs.Stride;
		}
	};

	enum GlobalFlags
	{
		GF_NONE = 0x0,
		GF_REFACTOR_ALLOWED = 0x1,
		GF_DOUBLE_SUPPORT = 0x2,
		GF_RAW_AND_STRUCTURED_SUPPORT = 0x4,
		GF_DOUBLE_EXTENSIONS_SUPPORT = 0x8,
		GF_SHADER_EXTENSIONS_SUPPORT = 0x10,
		GF_USES_TILED_RESOURCES = 0x20
	};
	static const Swizzle swizX;
	static const Swizzle swizXY;
	static const Swizzle swizXYZ;
	static const Swizzle swizXYZW;
	static MaskFlag MakeMask( bool x, bool y, bool z, bool w );
	static Swizzle MakeSwizzle( MaskFlag mask );
	static void AssertSwizzle(Swizzle s);

	AssemblyHelper(UINT constBufferElemCount,
				   UINT tempRegCount,
		           DXGI_FORMAT RTFormat = DXGI_FORMAT_UNKNOWN,
				   D3D11_VIEWPORT viewPort = AssemblyHelper::sm_DefaultViewPort,
				    UINT numComponentsOutputRawUAV = 4);

	tstring MakeAssemblyShader(D3D_SHADER_STAGE shaderStage, D3D_FEATURE_LEVEL featureLevel,
								const D3D_OPTIONS &D3DOptions, tstring instructionSpecificCode,
								GlobalFlags gflags = GF_NONE );

	tstring MakePassThroughVSToFeedThis(D3D_FEATURE_LEVEL featureLevel, D3D_OPTIONS D3DOptions, D3D_SHADER_STAGE targetShaderStage) const;
	tstring MakePassThroughHSForDS(D3D_FEATURE_LEVEL featureLevel, D3D_OPTIONS D3DOptions) const;
	tstring MakePassThroughDSForHS(D3D_FEATURE_LEVEL featureLevel, D3D_OPTIONS D3DOptions) const;
	tstring MakePassThroughPSToOutputThis(D3D_FEATURE_LEVEL featureLevel, D3D_OPTIONS D3DOptions) const;

	HRESULT SetupPipelineWithShaderCode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CShader5xTest* pTest,
										D3D_SHADER_STAGE shaderStage,
										D3D_OPTIONS D3DOptions, tstring instructionSpecificCode,
										ID3D11RenderTargetView** pRTVs, ID3D11UnorderedAccessView** pUAVs, int numRenderTargets,
										ID3D10Blob **ppShaderBuf,
										ID3D11VertexShader** ppVS, ID3D11GeometryShader** ppGS,
										ID3D11PixelShader** ppPS, ID3D11HullShader** ppHS,
										ID3D11DomainShader** ppDS, ID3D11ComputeShader** ppCS,
										D3D11_INPUT_ELEMENT_DESC* inputDecls = NULL, int numInputDecls = 0,
										ID3D11InputLayout** ppInputLayout = NULL,
										bool bCreatePSPassThrough = true,
										bool bCreateVSPassThrough = true);

    const tstring & RetrieveMadeShader() const { return m_MadeShader; }

	void SetCycleCounterDecl(MaskFlag mask);
	void SetResourceDecl(int resourceIdx, tstring resourceDetails);
	void SetCustomUAVDecl(int uavIdx, tstring uavDetails);
	void SetCustomDecl(int iIdx, tstring details);
	void SetSamplerDecl(int samplerIdx, tstring samplerDetails);
	void SetPositionOutputRegIdx(int positionOutputRegIdx = -1);
	void SetPositionInputRegIdx(int positionInputRegIdx = -1);
	void SetSysvalInputRegMask(int inputRegIdx, Semantic sysValName, MaskFlag mask);
	void SetUserInputRegMask(int inputRegIdx, MaskFlag mask);
	void SetUserInputRegMaskTyped(int inputRegIdx, MaskFlag mask, tstring psInterpolation, TypeName typeName);
	void SetOutputRegMask(int outputRegIdx, MaskFlag mask);
	void SetOutputRegMaskTyped(int outputRegIdx, MaskFlag mask, TypeName typeName, tstring resourceName = "Texture2D");
	void SetOutputRegMaskRaw(int outputRegIdx, MaskFlag mask);
	void SetOutputRegMaskStructured(int outputRegIdx, MaskFlag mask, UINT stride);
	void SetupMovToTempFromInput(int tempRegIdx, MaskFlag tempMask, int inputRegIdx, Swizzle inputSwizzle);
	void SetupMovToTempFromCB(int tempRegIdx, MaskFlag tempMask, int cbOffset, Swizzle cbSwizzle);
	void SetupMovToTempFromLiteral(int tempRegIdx, MaskFlag tempMask, Literal literalVal);
	void SetupMovToOutputFromInput(int outputRegIdx, MaskFlag outputMask, int inputRegIdx, Swizzle inputSwizzle);
	void SetupMovToOutputFromTemp(int outputRegIdx, MaskFlag outputMask, int tempRegIdx, Swizzle tempSwizzle);
	void SetupMovToRawOutputFromTemp(int outputRegIdx, MaskFlag outputMask, int tempRegIdx, Swizzle tempSwizzle);
	void SetupMovToOutputFromLiteral(int outputRegIdx, MaskFlag outputMask, Literal literalVal);
	void SetupMovToStructuredTGSMFromTemp(int tgsmRegIdx, UINT srcAddress, UINT byteOffset,  int tempRegIdx, MaskFlag tempMask);
	void SetupMovToTempFromStructuredTGSM(int tempRegIdx, MaskFlag tempMask, int regIdx, UINT srcAddress, UINT byteOffset);
	void SetupMovToRawTGSMFromTemp(int tgsmRegIdx, UINT byteOffset, int tempRegIdx, MaskFlag tempMask);
	void SetupMovToTempFromRawTGSM(int tempRegIdx, MaskFlag tempMask, int regIdx, UINT byteOffset);
	void SetupMovToStructuredUAVFromTemp(int tgsmRegIdx, UINT srcAddress, UINT byteOffset,  int tempRegIdx, MaskFlag tempMask);
	void SetupMovToTempFromStructuredUAV(int tempRegIdx, MaskFlag tempMask, int regIdx, UINT srcAddress, UINT byteOffset);
	void SetupMovToRawUAVFromTemp(int tgsmRegIdx, UINT byteOffset, int tempRegIdx, MaskFlag tempMask);
	void SetupMovToTempFromRawUAV(int tempRegIdx, MaskFlag tempMask, int regIdx, UINT byteOffset);

	void SetupMovToUAVTypedFromTemp(
		int uavRegIdx, MaskFlag uavMask,
		int reg2Idx, Swizzle reg2Swizzle,
		int tempRegIdx, Swizzle tempSwizzle);

	void SetupMovToTempFromUAVTyped(
		int tempRegIdx, MaskFlag tempSwizzle,
		int reg2Idx, Swizzle reg2Swizzle,
		int uavRegIdx, Swizzle uacSwizzle);

	void ForceRawUAVOutputByteOffset(int forcedByteOffset = 0);
	void SetSuppressEmit(bool suppressEmit);
	void SetConstantBufferSize(int cbIdx, int cbSize);
	void SetIndexableTempSize(int tempIdx, int tempSize);
	void SetIndexableRange(bool inputNotOutput, int startIdx, int stopIdx);
	void EnableDoubleSupport( bool v = true );
	void EnableDoubleExtensionsSupport( bool v = true );
	void EnableShaderExtensionsSupport( bool v = true );
	void EnableRawAndStructuredSupport( bool v = true );
	void EnableRefactoring( bool v = true );
	void EnableTiledResources( bool v = true );

	tstring GetRawUAVOutputMovStr(D3D_SHADER_STAGE shaderStage, int outputRegIdx, MaskFlag outputMask, int offsetRegIndexWithXComponent);
	tstring GetRawUAVOutputOffsetStr(int offsetRegIndexWithXComponent);
	tstring GetRawOutputMovStr(D3D_SHADER_STAGE shaderStage, int outputRegIdx, MaskFlag outputMask);
	tstring GetRawOutputMovStrNoMask(D3D_SHADER_STAGE shaderStage, int outputRegIdx);
	static tstring GetOutputMovStr(D3D_SHADER_STAGE shaderStage, int outputRegIdx, MaskFlag outputMask);
	static tstring GetOutputMovStrNoMask(D3D_SHADER_STAGE shaderStage, int outputRegIdx);
	static tstring GetInputRegStr(D3D_SHADER_STAGE shaderStage, int inputRegIdx, tstring swizzleOrMask, bool decl);
	static tstring GetScreenPositionMovStr(D3D_SHADER_STAGE shaderStage, int tempRegIdx, int screenPositionInputRegIdx);

	int GetNumOutputs() const { return m_OutputRegMasks.size(); }
	int GetNumInputs() const { return (m_SysvalInputRegMasks.size() + m_UserInputRegMasks.size()); }

	struct AHException
	{
	public:
		AHException(tstring error)
			: m_Error( error )
		{}
		tstring GetError() { return m_Error; }
	private:
		tstring m_Error;
	};

protected:
	ID3D11VertexShader*	m_pPassThroughVS;
	ID3D11HullShader*	m_pPassThroughHS;
	ID3D11DomainShader*	m_pPassThroughDS;
	ID3D11PixelShader*	m_pPassThroughPS;
	DXGI_FORMAT			m_RTFormat;
	D3D11_VIEWPORT		m_viewPort;
	static D3D11_VIEWPORT sm_DefaultViewPort;

private:
	typedef std::pair<TypeName, tstring> TypenameInterpolationPair;
    tstring     m_MadeShader;
	typedef std::pair<TypenameInterpolationPair, MaskFlag> TypenameInterpolationMaskPair;
	typedef std::pair<UINT, MaskFlag> UintMaskPair;
	typedef std::pair<TypeName, MaskFlag> TypenameMaskPair;
	typedef std::pair<Semantic, MaskFlag> SemanticMaskPair;
	typedef std::pair<int, MaskFlag> RegMaskPair;
	typedef std::pair<int, Swizzle> RegSwizzlePair;
	typedef std::pair<int, MemoryIndex> RegMemoryIndexPair;
	typedef std::map<int, TypenameInterpolationMaskPair> RegTypenameInterpolationMaskMap;
	typedef std::map<int, OuputMaskInfo> RegOutputMaskMap;
	typedef std::map<int, SemanticMaskPair> RegSemanticMaskMap;
	typedef std::map<RegMaskPair, RegSwizzlePair> RegPairMap;
	typedef std::map<RegMaskPair, RegMemoryIndexPair> RegPairIndexMap;
	typedef std::map<RegMaskPair, Literal> RegLiteralMap;
	typedef std::map<int, tstring> IntStringMap;
	typedef std::map<int, int> IntIntMap;


	typedef std::pair<int, SwizzlePairExtraSwizzle> RegSwizzlePairExtraSwizzle;
	typedef std::map<RegMaskPair, RegSwizzlePairExtraSwizzle> RegSwizzlePairExtraSwizzleMap;


	UINT m_TempRegCount;
	int m_PositionOutputRegIdx;
	int m_PositionInputRegIdx;
	int m_ForcedRawUAVByteOffset;
	bool m_bSuppressEmit;
	bool m_bForceRawUAVByteOffset;
	int m_numComponentsOutputRawUAV;  // 1-4 components to be stored to the result raw UAV per thread/pixel.

	MaskFlag m_CycleCounterMask;
	bool m_bDeclCycleCounter;

	IntStringMap m_ResourceDecls;
	IntStringMap m_UAVDecls;
	IntStringMap m_CustomDecls;
	IntStringMap m_SamplerDecls;
	RegSemanticMaskMap m_SysvalInputRegMasks;
	RegTypenameInterpolationMaskMap m_UserInputRegMasks;
	RegOutputMaskMap m_OutputRegMasks;
	RegPairMap m_MovsToTempFromInput;
	RegPairMap m_MovsToTempFromCB;
	RegPairIndexMap m_MovsToTGSMFromTemp;
	RegPairIndexMap m_MovToTempFromTGSM;
	RegLiteralMap m_MovsToTempFromLit;
	RegPairMap m_MovsToOutputFromInput;
	RegPairMap m_MovsToOutputFromTemp;
	RegPairMap m_MovsToRawOutputFromTemp;
	RegPairMap m_MovsToStructuredOutputFromTemp;
	RegSwizzlePairExtraSwizzleMap m_MovsToUAVTypedFromTemp;
	RegSwizzlePairExtraSwizzleMap m_MovsToTempFromUAVTyped;
	RegLiteralMap m_MovsToOutputFromLit;
	IntIntMap m_ConstantBufferSizes;
	IntIntMap m_IndexableTemps;
	IntIntMap m_IndexRangeInputs;
	IntIntMap m_IndexRangeOutputs;
	GlobalFlags m_GlobalFlags;
};

extern tstring ToString(AssemblyHelper::MaskFlag);
extern tstring ToString(AssemblyHelper::Swizzle);
extern bool FromString(AssemblyHelper::MaskFlag*, tstring);
extern bool FromString(AssemblyHelper::Swizzle*, tstring);

extern AssemblyHelper::MaskFlag g_pMasks [ 15 ];
extern AssemblyHelper::Swizzle g_pSwizzles [ 17 ];
extern AssemblyHelper::Swizzle g_pAddressXYSwizzles [ 4 ];

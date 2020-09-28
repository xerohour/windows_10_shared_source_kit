#include "AsmHelper.h"
#include <string>

const AssemblyHelper::Swizzle AssemblyHelper::swizX = {x,noMask,noMask,noMask};
const AssemblyHelper::Swizzle AssemblyHelper::swizXY = {x,y,noMask,noMask};
const AssemblyHelper::Swizzle AssemblyHelper::swizXYZ = {x,y,z,noMask};
const AssemblyHelper::Swizzle AssemblyHelper::swizXYZW = {x,y,z,w};

D3D11_VIEWPORT AssemblyHelper::sm_DefaultViewPort;

AssemblyHelper::MaskFlag g_pMasks [ 15 ] =
{
	// 16 possible permutations of masks, 15 real ones
	AssemblyHelper::MakeMask(true , true , true , true ), // 1
	AssemblyHelper::MakeMask(true , true , true , false),
	AssemblyHelper::MakeMask(true , true , false, true ),
	AssemblyHelper::MakeMask(true , true , false, false),
	AssemblyHelper::MakeMask(true , false, true , true ), // 5
	AssemblyHelper::MakeMask(true , false, true , false),
	AssemblyHelper::MakeMask(true , false, false, true ),
	AssemblyHelper::MakeMask(true , false, false, false),
	AssemblyHelper::MakeMask(false, true , true , true ),
	AssemblyHelper::MakeMask(false, true , true , false), // 10
	AssemblyHelper::MakeMask(false, true , false, true ),
	AssemblyHelper::MakeMask(false, true , false, false),
	AssemblyHelper::MakeMask(false, false, true , true ),
	AssemblyHelper::MakeMask(false, false, true , false),
	AssemblyHelper::MakeMask(false, false, false, true ), // 15
	// no mask with all = false, must have at least one component in mask
};

AssemblyHelper::Swizzle g_pSwizzles [ 17 ] =
{
	// 4^4 permutations of swizzles, but let's use just a few interesting ones
	{AssemblyHelper::x, AssemblyHelper::y, AssemblyHelper::z, AssemblyHelper::w}, // xyzw 1
	{AssemblyHelper::x, AssemblyHelper::y, AssemblyHelper::x, AssemblyHelper::y}, // xyxy
	{AssemblyHelper::z, AssemblyHelper::w, AssemblyHelper::x, AssemblyHelper::y}, // zwxy
	{AssemblyHelper::z, AssemblyHelper::w, AssemblyHelper::z, AssemblyHelper::w}, // zwzw
	{AssemblyHelper::x, AssemblyHelper::x, AssemblyHelper::x, AssemblyHelper::x}, // xxxx 5
	{AssemblyHelper::y, AssemblyHelper::y, AssemblyHelper::y, AssemblyHelper::y}, // yyyy
	{AssemblyHelper::z, AssemblyHelper::z, AssemblyHelper::z, AssemblyHelper::z}, // zzzz
	{AssemblyHelper::w, AssemblyHelper::w, AssemblyHelper::w, AssemblyHelper::w}, // wwww
	{AssemblyHelper::y, AssemblyHelper::x, AssemblyHelper::w, AssemblyHelper::z}, // yxwz
	{AssemblyHelper::y, AssemblyHelper::y, AssemblyHelper::x, AssemblyHelper::x}, // yyxx 10
	{AssemblyHelper::w, AssemblyHelper::w, AssemblyHelper::z, AssemblyHelper::z}, // wwzz
	{AssemblyHelper::w, AssemblyHelper::z, AssemblyHelper::y, AssemblyHelper::x}, // wzyx
	{AssemblyHelper::y, AssemblyHelper::x, AssemblyHelper::w, AssemblyHelper::z}, // yxwz
	{AssemblyHelper::x, AssemblyHelper::y, AssemblyHelper::y, AssemblyHelper::y}, // xyyy
	{AssemblyHelper::y, AssemblyHelper::z, AssemblyHelper::z, AssemblyHelper::z}, // yzzz 15
	{AssemblyHelper::z, AssemblyHelper::w, AssemblyHelper::w, AssemblyHelper::w}, // zwww
	{AssemblyHelper::w, AssemblyHelper::x, AssemblyHelper::y, AssemblyHelper::z}, // wxyz 17
};

AssemblyHelper::Swizzle g_pAddressXYSwizzles [ 4 ] =
{
	{AssemblyHelper::x, AssemblyHelper::y, AssemblyHelper::z, AssemblyHelper::w},
	{AssemblyHelper::y, AssemblyHelper::x, AssemblyHelper::z, AssemblyHelper::w},
	{AssemblyHelper::x, AssemblyHelper::x, AssemblyHelper::z, AssemblyHelper::w},
	{AssemblyHelper::y, AssemblyHelper::y, AssemblyHelper::z, AssemblyHelper::w},
};

#define FOR_EACH_ITERATOR(itName, mapVar, mapType) for (mapType::const_iterator itName = mapVar.begin(); itName != mapVar.end(); ++itName)
const int RESERVED_POSITION_REGISTER = 15;
const int RESERVED_VERTEXID_REGISTER = 14;
const int RESERVED_RAWUAV_OFFSET_CALCULATION_REGISTER = 99;

// tests whether a map already has a value set for a given key
template<typename T, typename K>
static bool MapHasKey(const T& mapRef, const K& keyRef)
{
	T::const_iterator it = mapRef.find(keyRef);
	if (it != mapRef.end())
		return true;
	return false;
}

// adds a value to a map with a given key
// throws exception if key already exists
template<typename T, typename K, typename V>
static void AddValToMap_ThrowDuplicates(T& mapRef, const K& keyRef, const V& valRef)
{
	if (MapHasKey(mapRef, keyRef))
		throw AssemblyHelper::AHException( _T("Key value already assigned") );

	mapRef[keyRef] = valRef;
}

tstring ToString(AssemblyHelper::MaskFlag mask)
{
	tstring retVal = "";
	if (mask & AssemblyHelper::x)
		retVal += "x";
	if (mask & AssemblyHelper::y)
		retVal += "y";
	if (mask & AssemblyHelper::z)
		retVal += "z";
	if (mask & AssemblyHelper::w)
		retVal += "w";
	return retVal;
}

tstring ToString(AssemblyHelper::Swizzle s)
{
	AssemblyHelper::AssertSwizzle(s);
	tstring retVal = ToString(s.r);
	if (s.g != AssemblyHelper::noMask)
		retVal += ToString(s.g);
	if (s.b != AssemblyHelper::noMask)
		retVal += ToString(s.b);
	if (s.a != AssemblyHelper::noMask)
		retVal += ToString(s.a);
	return retVal;
}

bool FromString(AssemblyHelper::MaskFlag* pMask, tstring str)
{
	throw AssemblyHelper::AHException("not implemented");
	return false;
}

bool FromString(AssemblyHelper::Swizzle* pSwizzle, tstring str)
{
	throw AssemblyHelper::AHException("not implemented");
	return false;
}

// ============================================================================
AssemblyHelper::AssemblyHelper(UINT constBufferElemCount,
				               UINT tempRegCount,
		                       DXGI_FORMAT RTFormat,
	                           D3D11_VIEWPORT viewPort,
							   UINT numComponentsOutputRawUAV) :
	m_TempRegCount( tempRegCount ),
	m_PositionOutputRegIdx( -1 ),
	m_PositionInputRegIdx( -1 ),
	m_ForcedRawUAVByteOffset( -1 ),
	m_bSuppressEmit( false ),
	m_bForceRawUAVByteOffset( false ),
	m_bDeclCycleCounter( false ),
	m_GlobalFlags( GF_NONE ),
	m_RTFormat( RTFormat ),
	m_viewPort( viewPort ),
	m_numComponentsOutputRawUAV( numComponentsOutputRawUAV )
{
	if ( constBufferElemCount )
	{
		this->SetConstantBufferSize( 0, constBufferElemCount );
	}
	// m_numComponentsOutputRawUAV can only be 1 to 4
	if ( m_numComponentsOutputRawUAV > 4 || m_numComponentsOutputRawUAV == 0)
		m_numComponentsOutputRawUAV = 4;
}

AssemblyHelper::MaskFlag AssemblyHelper::MakeMask( bool _x, bool _y, bool _z, bool _w)
{
	UINT retVal = 0;
	if (_x)
		retVal |= x;
	if (_y)
		retVal |= y;
	if (_z)
		retVal |= z;
	if (_w)
		retVal |= w;
	return (MaskFlag)retVal;
}


AssemblyHelper::Swizzle AssemblyHelper::MakeSwizzle( MaskFlag mask )
{
	assert(mask != noMask);
	Swizzle retS = {noMask, noMask, noMask, noMask};
	int numSet = 0;
	if (mask & x)
	{
		retS.flags[numSet] = x;
		numSet += 1;
	}
	if (mask & y)
	{
		retS.flags[numSet] = y;
		numSet += 1;
	}
	if (mask & z)
	{
		retS.flags[numSet] = z;
		numSet += 1;
	}
	if (mask & w)
	{
		retS.flags[numSet] = w;
		numSet += 1;
	}
	assert(numSet > 0);
	AssertSwizzle(retS);
	return retS;
}


void AssemblyHelper::SetPositionOutputRegIdx(int positionOutputRegIdx)
{
	m_PositionOutputRegIdx = positionOutputRegIdx;
}


void AssemblyHelper::SetPositionInputRegIdx(int positionInputRegIdx)
{
	m_PositionInputRegIdx = positionInputRegIdx;
	SetSysvalInputRegMask(m_PositionInputRegIdx, "position", xyzw);
}

void AssemblyHelper::SetCycleCounterDecl(MaskFlag mask)
{
	m_bDeclCycleCounter = true;
	m_CycleCounterMask = mask;
}


void AssemblyHelper::SetResourceDecl(int resourceIdx, tstring resourceDetails)
{
	AddValToMap_ThrowDuplicates(m_ResourceDecls,
		resourceIdx, resourceDetails);
}


void AssemblyHelper::SetCustomDecl(int idx, tstring details)
{
	AddValToMap_ThrowDuplicates(m_CustomDecls,
		idx, details);
}

void AssemblyHelper::SetCustomUAVDecl(int uavIdx, tstring uavDetails)
{
	AddValToMap_ThrowDuplicates(m_UAVDecls,
		uavIdx, uavDetails);
}

void AssemblyHelper::SetSamplerDecl(int samplerIdx, tstring samplerDetails)
{
	AddValToMap_ThrowDuplicates(m_SamplerDecls,
		samplerIdx, samplerDetails);
}


void AssemblyHelper::SetSysvalInputRegMask(int inputRegIdx, Semantic sysValName, MaskFlag mask)
{
	AddValToMap_ThrowDuplicates(m_SysvalInputRegMasks,
		inputRegIdx,
		SemanticMaskPair(sysValName, mask) );
}


void AssemblyHelper::SetUserInputRegMask(int inputRegIdx, MaskFlag mask)
{
	SetUserInputRegMaskTyped(inputRegIdx, mask, "constant", "");
}


void AssemblyHelper::SetUserInputRegMaskTyped(int inputRegIdx, MaskFlag mask, tstring psInterpolation, TypeName typeName)
{
	AddValToMap_ThrowDuplicates(m_UserInputRegMasks,
		inputRegIdx, TypenameInterpolationMaskPair(TypenameInterpolationPair(typeName, psInterpolation), mask) );
}


void AssemblyHelper::SetOutputRegMask(int outputRegIdx, MaskFlag mask)
{
	SetOutputRegMaskTyped(outputRegIdx, mask, "");
}


void AssemblyHelper::SetOutputRegMaskTyped(int outputRegIdx, MaskFlag mask, TypeName typeName, tstring resourceName )
{
	OuputMaskInfo outputMaskInfo;
	outputMaskInfo.DestMask = mask;
	outputMaskInfo.MemoryType = typed;
	outputMaskInfo.TypeName = typeName;
	outputMaskInfo.ResourceName = resourceName;
	outputMaskInfo.Stride = 0;
	AddValToMap_ThrowDuplicates(m_OutputRegMasks, outputRegIdx, outputMaskInfo );
}

void AssemblyHelper::SetOutputRegMaskRaw(int outputRegIdx, MaskFlag mask)
{
	OuputMaskInfo outputMaskInfo;
	outputMaskInfo.DestMask = mask;
	outputMaskInfo.MemoryType = raw;
	outputMaskInfo.TypeName = "uint";
	outputMaskInfo.ResourceName = "Texture2D";
	outputMaskInfo.Stride = 0;
	AddValToMap_ThrowDuplicates(m_OutputRegMasks, outputRegIdx, outputMaskInfo);
}

void AssemblyHelper::SetOutputRegMaskStructured(int outputRegIdx, MaskFlag mask, UINT stride)
{
	OuputMaskInfo outputMaskInfo;
	outputMaskInfo.DestMask = mask;
	outputMaskInfo.MemoryType = structured;
	outputMaskInfo.TypeName = "uint";
	outputMaskInfo.ResourceName = "Texture2D";
	outputMaskInfo.Stride = stride;
	AddValToMap_ThrowDuplicates(m_OutputRegMasks, outputRegIdx, outputMaskInfo);
}

void AssemblyHelper::SetupMovToTempFromStructuredTGSM(int tempRegIdx,
											MaskFlag tempMask,
											int tgsmRegIdx,
											UINT srcAddress,
											UINT byteOffset)
{
	MemoryIndex mi = {srcAddress, byteOffset,structured, "g"};

	AddValToMap_ThrowDuplicates(m_MovToTempFromTGSM,
		RegMaskPair(tempRegIdx, tempMask),
		RegMemoryIndexPair(tgsmRegIdx, mi));
}

void AssemblyHelper::SetupMovToStructuredTGSMFromTemp(int tgsmRegIdx,
											UINT srcAddress,
											UINT byteOffset,
											int tempRegIdx,
											MaskFlag tempMask)
{
	MemoryIndex mi = {srcAddress, byteOffset,structured,"g"};

	AddValToMap_ThrowDuplicates(m_MovsToTGSMFromTemp,
		RegMaskPair(tempRegIdx, tempMask),
		RegMemoryIndexPair(tgsmRegIdx, mi));
}


void AssemblyHelper::SetupMovToTempFromRawTGSM(int tempRegIdx,
											MaskFlag tempMask,
											int tgsmRegIdx,
											UINT byteOffset)
{
	MemoryIndex mi = {0, byteOffset,raw,"g"};

	AddValToMap_ThrowDuplicates(m_MovToTempFromTGSM,
		RegMaskPair(tempRegIdx, tempMask),
		RegMemoryIndexPair(tgsmRegIdx, mi));
}


void AssemblyHelper::SetupMovToRawTGSMFromTemp(int tgsmRegIdx,
											UINT byteOffset,
											int tempRegIdx,
											MaskFlag tempMask)
{
	MemoryIndex mi = {0, byteOffset,raw,"g"};

	AddValToMap_ThrowDuplicates(m_MovsToTGSMFromTemp,
		RegMaskPair(tempRegIdx, tempMask),
		RegMemoryIndexPair(tgsmRegIdx, mi));
}

void AssemblyHelper::SetupMovToTempFromStructuredUAV(int tempRegIdx,
											MaskFlag tempMask,
											int tgsmRegIdx,
											UINT srcAddress,
											UINT byteOffset)
{
	MemoryIndex mi = {srcAddress, byteOffset,structured, "u"};

	AddValToMap_ThrowDuplicates(m_MovToTempFromTGSM,
		RegMaskPair(tempRegIdx, tempMask),
		RegMemoryIndexPair(tgsmRegIdx, mi));
}

void AssemblyHelper::SetupMovToStructuredUAVFromTemp(int tgsmRegIdx,
											UINT srcAddress,
											UINT byteOffset,
											int tempRegIdx,
											MaskFlag tempMask)
{
	MemoryIndex mi = {srcAddress, byteOffset,structured,"u"};

	AddValToMap_ThrowDuplicates(m_MovsToTGSMFromTemp,
		RegMaskPair(tempRegIdx, tempMask),
		RegMemoryIndexPair(tgsmRegIdx, mi));
}


void AssemblyHelper::SetupMovToTempFromRawUAV(int tempRegIdx,
											MaskFlag tempMask,
											int tgsmRegIdx,
											UINT byteOffset)
{
	MemoryIndex mi = {0, byteOffset,raw,"u"};

	AddValToMap_ThrowDuplicates(m_MovToTempFromTGSM,
		RegMaskPair(tempRegIdx, tempMask),
		RegMemoryIndexPair(tgsmRegIdx, mi));
}


void AssemblyHelper::SetupMovToRawUAVFromTemp(int tgsmRegIdx,
											UINT byteOffset,
											int tempRegIdx,
											MaskFlag tempMask)
{
	MemoryIndex mi = {0, byteOffset,raw,"u"};

	AddValToMap_ThrowDuplicates(m_MovsToTGSMFromTemp,
		RegMaskPair(tempRegIdx, tempMask),
		RegMemoryIndexPair(tgsmRegIdx, mi));
}

void AssemblyHelper::SetupMovToUAVTypedFromTemp(int uavRegIdx, MaskFlag uavMask,
											  int reg2Idx, Swizzle reg2Swizzle,
											  int tempRegIdx, Swizzle tempSwizzle)
{
	SwizzlePairExtraSwizzle pairs = {reg2Idx,reg2Swizzle,tempSwizzle};

	AddValToMap_ThrowDuplicates(m_MovsToUAVTypedFromTemp,
		RegMaskPair(uavRegIdx, uavMask),
		RegSwizzlePairExtraSwizzle(tempRegIdx, pairs) );
}

void AssemblyHelper::SetupMovToTempFromUAVTyped(int tempRegIdx, MaskFlag tempMask,
												int reg2Idx, Swizzle reg2Swizzle,
												int uavRegIdx, Swizzle uavSwizzle)
{
	SwizzlePairExtraSwizzle pairs = {reg2Idx,reg2Swizzle,uavSwizzle};

	AddValToMap_ThrowDuplicates(m_MovsToTempFromUAVTyped,
		RegMaskPair(tempRegIdx, tempMask),
		RegSwizzlePairExtraSwizzle(uavRegIdx, pairs) );
}

void AssemblyHelper::SetupMovToTempFromInput(int tempRegIdx, MaskFlag tempMask,
											 int inputRegIdx, Swizzle inputSwizzle)
{
	AddValToMap_ThrowDuplicates(m_MovsToTempFromInput,
		RegMaskPair(tempRegIdx, tempMask),
		RegSwizzlePair(inputRegIdx, inputSwizzle) );
}


void AssemblyHelper::SetupMovToTempFromCB(int tempRegIdx, MaskFlag tempMask,
										  int cbOffset, Swizzle cbSwizzle)
{
	AddValToMap_ThrowDuplicates(m_MovsToTempFromCB,
		RegMaskPair(tempRegIdx, tempMask),
		RegSwizzlePair(cbOffset, cbSwizzle) );
}

void AssemblyHelper::SetupMovToTempFromLiteral(int tempRegIdx, MaskFlag tempMask,
											   Literal literalVal)
{
	AddValToMap_ThrowDuplicates(m_MovsToTempFromLit,
		RegMaskPair(tempRegIdx, tempMask),
		literalVal);
}


void AssemblyHelper::SetupMovToOutputFromInput(int outputRegIdx, MaskFlag outputMask,
											   int inputRegIdx, Swizzle inputSwizzle)
{
	AddValToMap_ThrowDuplicates(m_MovsToOutputFromInput,
		RegMaskPair(outputRegIdx, outputMask),
		RegSwizzlePair(inputRegIdx, inputSwizzle) );
}


void AssemblyHelper::SetupMovToOutputFromTemp(int outputRegIdx, MaskFlag outputMask,
											  int tempRegIdx, Swizzle tempSwizzle)
{
	AddValToMap_ThrowDuplicates(m_MovsToOutputFromTemp,
		RegMaskPair(outputRegIdx, outputMask),
		RegSwizzlePair(tempRegIdx, tempSwizzle) );
}

void AssemblyHelper::SetupMovToRawOutputFromTemp(int outputRegIdx, MaskFlag outputMask,
											     int tempRegIdx, Swizzle tempSwizzle)
{
	AddValToMap_ThrowDuplicates(m_MovsToRawOutputFromTemp,
		RegMaskPair(outputRegIdx, outputMask),
		RegSwizzlePair(tempRegIdx, tempSwizzle) );
}

void AssemblyHelper::SetupMovToOutputFromLiteral(int outputRegIdx, MaskFlag outputMask,
												 Literal literalVal)
{
	AddValToMap_ThrowDuplicates(m_MovsToOutputFromLit,
		RegMaskPair(outputRegIdx, outputMask),
		literalVal);
}

void AssemblyHelper::ForceRawUAVOutputByteOffset(int forcedByteOffset)
{
	m_bForceRawUAVByteOffset = true;
	m_ForcedRawUAVByteOffset = forcedByteOffset;
}

void AssemblyHelper::SetSuppressEmit(bool suppressEmit)
{
	m_bSuppressEmit = suppressEmit;
}

void AssemblyHelper::SetConstantBufferSize(int cbIdx, int cbSize)
{
	AddValToMap_ThrowDuplicates(m_ConstantBufferSizes,
		cbIdx,
		cbSize);
}


void AssemblyHelper::SetIndexableTempSize(int tempIdx, int tempSize)
{
	AddValToMap_ThrowDuplicates(m_IndexableTemps,
		tempIdx,
		tempSize);
}


void AssemblyHelper::SetIndexableRange(bool inputNotOutput, int startIdx, int stopIdx)
{
	AddValToMap_ThrowDuplicates(inputNotOutput ? m_IndexRangeInputs : m_IndexRangeOutputs,
		startIdx,
		stopIdx);
}


void AssemblyHelper::AssertSwizzle(Swizzle s)
{
	assert (((s.r == x || s.r == y || s.r == z || s.r == w)				 ) || s.r == noMask);
	assert (((s.g == x || s.g == y || s.g == z || s.g == w) && s.r != noMask) || s.g == noMask);
	assert (((s.b == x || s.b == y || s.b == z || s.b == w) && s.g != noMask) || s.b == noMask);
	assert (((s.a == x || s.a == y || s.a == z || s.a == w) && s.b != noMask) || s.a == noMask);
}


void AssemblyHelper::EnableDoubleSupport( bool v )
{
	if( v )
	{
		m_GlobalFlags = static_cast<GlobalFlags>(m_GlobalFlags | GF_DOUBLE_SUPPORT);
	}
	else if( m_GlobalFlags & GF_DOUBLE_SUPPORT )
	{
		m_GlobalFlags = static_cast<GlobalFlags>(m_GlobalFlags ^ GF_DOUBLE_SUPPORT);
	}
}

void AssemblyHelper::EnableDoubleExtensionsSupport( bool v )
{
	if( v )
	{
		m_GlobalFlags = static_cast<GlobalFlags>(m_GlobalFlags | GF_DOUBLE_EXTENSIONS_SUPPORT);
	}
	else if( m_GlobalFlags & GF_DOUBLE_EXTENSIONS_SUPPORT )
	{
		m_GlobalFlags = static_cast<GlobalFlags>(m_GlobalFlags ^ GF_DOUBLE_EXTENSIONS_SUPPORT);
	}
}

void AssemblyHelper::EnableShaderExtensionsSupport( bool v )
{
	if( v )
	{
		m_GlobalFlags = static_cast<GlobalFlags>(m_GlobalFlags | GF_SHADER_EXTENSIONS_SUPPORT);
	}
	else if( m_GlobalFlags & GF_SHADER_EXTENSIONS_SUPPORT )
	{
		m_GlobalFlags = static_cast<GlobalFlags>(m_GlobalFlags ^ GF_SHADER_EXTENSIONS_SUPPORT);
	}
}

void AssemblyHelper::EnableRawAndStructuredSupport( bool v )
{
	if( v )
	{
		m_GlobalFlags = static_cast<GlobalFlags>(m_GlobalFlags | GF_RAW_AND_STRUCTURED_SUPPORT);
	}
	else if( m_GlobalFlags & GF_RAW_AND_STRUCTURED_SUPPORT )
	{
		m_GlobalFlags = static_cast<GlobalFlags>(m_GlobalFlags ^ GF_RAW_AND_STRUCTURED_SUPPORT);
	}
}

void AssemblyHelper::EnableRefactoring( bool v )
{
	if( v )
	{
		m_GlobalFlags = static_cast<GlobalFlags>(m_GlobalFlags | GF_REFACTOR_ALLOWED);
	}
	else if( m_GlobalFlags & GF_REFACTOR_ALLOWED )
	{
		m_GlobalFlags = static_cast<GlobalFlags>(m_GlobalFlags ^ GF_REFACTOR_ALLOWED);
	}
}

void AssemblyHelper::EnableTiledResources( bool v )
{
	if ( v )
	{
		m_GlobalFlags = static_cast<GlobalFlags>(m_GlobalFlags | GF_USES_TILED_RESOURCES);
	}
	else if ( m_GlobalFlags & GF_USES_TILED_RESOURCES )
	{
		m_GlobalFlags = static_cast<GlobalFlags>(m_GlobalFlags ^ GF_USES_TILED_RESOURCES);
	}
}

tstring AssemblyHelper::GetOutputMovStrNoMask(D3D_SHADER_STAGE shaderStage,
											  int outputRegIdx)
{
	switch (shaderStage)
	{
	default:
		throw AssemblyHelper::AHException("Invalid Shader Stage");
		break;
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_PIXEL:
	case D3D_SHADER_STAGE_HULL:
	case D3D_SHADER_STAGE_DOMAIN:
		return FormatString( "mov o[%i]", outputRegIdx );
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		return FormatString( "store_uav_typed u%i.xyzw, vThreadGroupID.xyzz", outputRegIdx );
		break;
	}
	return "";
}

tstring AssemblyHelper::GetOutputMovStr(D3D_SHADER_STAGE shaderStage,
										int outputRegIdx,
										MaskFlag outputMask)
{
	tstring retVal = AssemblyHelper::GetOutputMovStrNoMask(shaderStage, outputRegIdx);
	switch (shaderStage)
	{
	default:
		throw AssemblyHelper::AHException("Invalid Shader Stage");
		break;
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_PIXEL:
	case D3D_SHADER_STAGE_HULL:
	case D3D_SHADER_STAGE_DOMAIN:
		retVal += "." + ToString(outputMask);
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		// no mask allowed out store_uav_typed
		break;
	}
	return retVal;
}

tstring AssemblyHelper::GetRawOutputMovStrNoMask(D3D_SHADER_STAGE shaderStage,
											     int outputRegIdx)
{
	switch (shaderStage)
	{
	default:
		throw AssemblyHelper::AHException("Invalid Shader Stage");
		break;
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_PIXEL:
	case D3D_SHADER_STAGE_HULL:
	case D3D_SHADER_STAGE_DOMAIN:
		return GetOutputMovStrNoMask(shaderStage, outputRegIdx);
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		MaskFlag outputMask = AssemblyHelper::xyzw;
		if ( m_numComponentsOutputRawUAV == 1)
			outputMask = AssemblyHelper::x;
		return GetRawUAVOutputMovStr(D3D_SHADER_STAGE_COMPUTE, outputRegIdx, outputMask, RESERVED_RAWUAV_OFFSET_CALCULATION_REGISTER);

		break;
	}
	return "";
}

tstring AssemblyHelper::GetRawOutputMovStr(D3D_SHADER_STAGE shaderStage,
										   int outputRegIdx,
										   MaskFlag outputMask)
{
	tstring retVal = this->GetRawOutputMovStrNoMask(shaderStage, outputRegIdx);
	switch (shaderStage)
	{
	default:
		throw AssemblyHelper::AHException("Invalid Shader Stage");
		break;
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_PIXEL:
	case D3D_SHADER_STAGE_HULL:
	case D3D_SHADER_STAGE_DOMAIN:
		retVal += "." + ToString(outputMask);
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		// no mask allowed out store_uav_typed
		break;
	}
	return retVal;
}

tstring AssemblyHelper::GetRawUAVOutputMovStr(D3D_SHADER_STAGE shaderStage, int outputRegIdx, MaskFlag outputMask, int offsetRegIndexWithXComponent)
{
	tstring retVal = "";

	switch (shaderStage)
	{
	default:
		throw AssemblyHelper::AHException("Invalid Shader Stage");
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		const tstring offsetReg = FormatString("r%i.x", offsetRegIndexWithXComponent);

		if( m_bForceRawUAVByteOffset )
		{
			retVal = FormatString("store_raw u%i.%s, %d",
				outputRegIdx,
				ToString(outputMask).c_str(),
				m_ForcedRawUAVByteOffset);
		}
		else
		{
			retVal = FormatString("store_raw u%i.%s, %s",
			outputRegIdx,
			ToString(outputMask).c_str(),
			offsetReg.c_str());
		}
		break;
	}

	return retVal;
}

tstring AssemblyHelper::GetRawUAVOutputOffsetStr(int offsetRegIndexWithXComponent)
{
	// Local variables
	tstring computeRawBufferOffset = "";

	const UINT rtFormatElementSizeInBytes = GetBitsPerElement( m_RTFormat ) / 8 *  m_numComponentsOutputRawUAV;
	const UINT rowPitchInBytes = (UINT)(m_viewPort.Width) * rtFormatElementSizeInBytes;

	// Thus offset in bytes = (vThreadGroupID.y * rowPitchInBytes) + (vThreadGroupID.x * rtFormatElementSizeInBytes)
	// Put the offset in the temp reg index specified by the user
	computeRawBufferOffset += FormatString("itof r%i.xy, vThreadGroupID.xy \n", offsetRegIndexWithXComponent);																// Convert the ThreadGroupID to a float
	computeRawBufferOffset += FormatString("mul r%i.y, r%i.y, l(%i.000000) \n", offsetRegIndexWithXComponent, offsetRegIndexWithXComponent, rowPitchInBytes);				// (vThreadGroupID.y * rowPitchInBytes)
	computeRawBufferOffset += FormatString("mul r%i.x, r%i.x, l(%i.000000) \n", offsetRegIndexWithXComponent, offsetRegIndexWithXComponent, rtFormatElementSizeInBytes);	// (vThreadGroupID.x * rtFormatElementSizeInBytes)
	computeRawBufferOffset += FormatString("add r%i.x, r%i.x, r%i.y \n", offsetRegIndexWithXComponent, offsetRegIndexWithXComponent, offsetRegIndexWithXComponent);			// (vThreadGroupID.y * rowPitchInBytes) + (vThreadGroupID.x * rtFormatElementSizeInBytes)
	computeRawBufferOffset += FormatString("ftoi r%i.x, r%i.x \n", offsetRegIndexWithXComponent, offsetRegIndexWithXComponent);												// Convert the offset back to an int

	return computeRawBufferOffset;
}


tstring AssemblyHelper::GetInputRegStr(D3D_SHADER_STAGE shaderStage, int inputRegIdx, tstring swizzleOrMask, bool decl)
{
	tstring retStr;
	switch (shaderStage)
	{
	default:
		throw AHException("Shader stage does not have inputs");
		break;
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_PIXEL:
		retStr = FormatString( "v[%i].%s", inputRegIdx, (swizzleOrMask).c_str() );
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_HULL:
		retStr = FormatString( "v[%i][%i].%s", (decl ? 1 : 0), inputRegIdx, (swizzleOrMask).c_str() );
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		retStr = FormatString( "vcp[%i][%i].%s", (decl ? 1 : 0), inputRegIdx, (swizzleOrMask).c_str() );
		break;
	}
	return retStr;
}


tstring AssemblyHelper::GetScreenPositionMovStr(D3D_SHADER_STAGE shaderStage, int tempRegIdx, int screenPositionInputRegIdx)
{
	tstring retStr;
	switch (shaderStage)
	{
	default:
		throw AHException("Invalid Shader Stage");
		break;
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_GEOMETRY:
	case D3D_SHADER_STAGE_PIXEL:
	case D3D_SHADER_STAGE_HULL:
	case D3D_SHADER_STAGE_DOMAIN:
		{
			// most stages can access position through an input register
			Swizzle inputRegSwizzle = {x,y,noMask,noMask};
			tstring inputRegStr = GetInputRegStr(shaderStage, screenPositionInputRegIdx, ToString(inputRegSwizzle), false);
			tstring formatStr = "mov r%i.%s, " + inputRegStr + " \n";
			retStr += FormatString(formatStr.c_str(),
				tempRegIdx, ToString(xy).c_str() );
		}
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		{
			// compute shader position is implied by the Dispatch group ID on a 3D grid
			retStr += FormatString("itof r%i.xyz, vThreadGroupID.xyz \n", tempRegIdx);
			retStr += FormatString("add r%i.xyzw, r%i.xyzw, vec4(0.5,0.5,0.5,0) \n", tempRegIdx, tempRegIdx);
		}
		break;
	}
	retStr += FormatString("mov r%i.zw, vec4(0,0,0,1.0) \n", tempRegIdx);
	return retStr;
}


tstring AssemblyHelper::MakeAssemblyShader(D3D_SHADER_STAGE shaderStage,
										   D3D_FEATURE_LEVEL featureLevel,
										   const D3D_OPTIONS &D3DOptions,
										   tstring instructionSpecificCode,
										   GlobalFlags gflags )
{
	tstring & retStr = m_MadeShader;

    retStr = "";

	// shader profile, like vs_5_0
	retStr = CD3D11DeviceManager::GetShaderProfile(shaderStage, featureLevel, featureLevel, D3DOptions);
	retStr += "\n";

	const int numOutputs = max(1, m_OutputRegMasks.size());

	// shader-stage-specific declarations
	switch (shaderStage)
	{
	default:
		throw AHException("Invalid Shader Stage");
		break;
	case D3D_SHADER_STAGE_VERTEX:
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
		retStr += "dcl_inputPrimitive point \n";
		retStr += "dcl_outputTopology pointlist \n";
		retStr += "dcl_maxOutputVertexCount 1 \n";
		break;
	case D3D_SHADER_STAGE_PIXEL:
		break;
	case D3D_SHADER_STAGE_HULL:
		retStr += "hs_decls \n";
		retStr += "dcl_input_control_point_count 1 \n";
		retStr += "dcl_output_control_point_count 1 \n";
		retStr += "dcl_tessellator_partitioning partitioning_integer \n";
		retStr += "dcl_tessellator_domain domain_isoline \n";
		retStr += "dcl_tessellator_output_primitive output_point \n";
		retStr += "dcl_hs_max_tessfactor 1.f \n";
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		retStr += "dcl_input_control_point_count 1 \n";
		retStr += "dcl_tessellator_domain domain_isoline \n";
		retStr += "dcl_input_sv vpc[0].x, finalLineDensityTessFactor \n"; // tess factor inputs
		retStr += "dcl_input_sv vpc[1].x, finalLineDetailTessFactor \n";
		retStr += "dcl_input vDomain.xy \n";
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		retStr += "dcl_thread_group 1,1,1 \n";
		retStr += "dcl_input vThreadID.xyz \n";
		retStr += "dcl_input vThreadGroupID.xyz \n";
		retStr += "dcl_input vThreadIDInGroup.xyz \n";
		break;
	}

	// Global flag to enable doubles, D3D11.1 doubles extensions and shader extensions
	const GlobalFlags NeedShaderDcl = static_cast<GlobalFlags>(GF_REFACTOR_ALLOWED | GF_DOUBLE_SUPPORT | GF_RAW_AND_STRUCTURED_SUPPORT | GF_DOUBLE_EXTENSIONS_SUPPORT | GF_SHADER_EXTENSIONS_SUPPORT);
	if( gflags & NeedShaderDcl)
	{
		typedef struct { GlobalFlags const m_Flag; TCHAR const* m_pName; } GFlagNamePair;
		static const GFlagNamePair gFlagTable[] =
		{
			{ GF_REFACTOR_ALLOWED, _T("refactoringAllowed") },
			{ GF_DOUBLE_SUPPORT, _T("enableDoublePrecisionFloatOps") },
			{ GF_RAW_AND_STRUCTURED_SUPPORT, _T("enableRawAndStructuredBuffers") },
			{ GF_DOUBLE_EXTENSIONS_SUPPORT, _T("enable11_1DoubleExtensions") },
			{ GF_SHADER_EXTENSIONS_SUPPORT, _T("enable11_1ShaderExtensions") }
		};

		tstring gdcl;
		for( unsigned int i(0); i < sizeof(gFlagTable) / sizeof(GFlagNamePair); ++i )
		{
			if( gFlagTable[i].m_Flag & gflags )
			{
				if( gdcl.empty() )
				{
					gdcl += gFlagTable[i].m_pName;
				}
				else
				{
					gdcl += tstring(_T(" | ")) + gFlagTable[i].m_pName;
				}
			}
		}

		retStr += _T("dcl_globalFlags ") + gdcl + _T("\n");
	}

	// constant buffers are shared for all HS segments
	FOR_EACH_ITERATOR(it, m_ConstantBufferSizes, IntIntMap)
	{
		int cbIdx = it->first;
		int cbSize = it->second;
		retStr += FormatString("dcl_constantBuffer c%u[%u], dynamicIndexed \n", cbIdx, cbSize);
	}
	FOR_EACH_ITERATOR(it, m_ResourceDecls, IntStringMap)
	{
		const char* formatStr = "dcl_resource t%i, %s \n";
		retStr += FormatString(formatStr,
			(int)it->first,
			it->second.c_str() );
	}
	FOR_EACH_ITERATOR(it, m_UAVDecls, IntStringMap)
	{
		const char* formatStr = "%s \n";
		retStr += FormatString(formatStr,
			it->second.c_str() );
	}
	FOR_EACH_ITERATOR(it, m_CustomDecls, IntStringMap)
	{
		const char* formatStr = "%s \n";
		retStr += FormatString(formatStr,
			it->second.c_str() );
	}
	FOR_EACH_ITERATOR(it, m_SamplerDecls, IntStringMap)
	{
		const char* formatStr = "dcl_sampler s%i, %s \n";
		retStr += FormatString(formatStr,
			(int)it->first,
			it->second.c_str() );
	}

	// hull shader has to declare control-point, fork, and join phases independently
	if (shaderStage == D3D_SHADER_STAGE_HULL)
	{
		retStr += "\n" "hs_control_point_phase" "\n";
	}

	// declare temps
	if (m_TempRegCount > 0)
		retStr += FormatString("dcl_temps %u \n", m_TempRegCount);
	FOR_EACH_ITERATOR(it, m_IndexableTemps, IntIntMap)
	{
		retStr += FormatString("dcl_indexableTemp x%d[%d], 4 \n",
			it->first, it->second);
	}

	bool bHasNonTypedOuputs = false;

	if (D3D_SHADER_STAGE_COMPUTE == shaderStage)
	{
		FOR_EACH_ITERATOR(it, m_OutputRegMasks, RegOutputMaskMap)
		{
			const MemoryType memoryType = it->second.MemoryType;
			switch ( memoryType )
			{
			case typed:
			{
				TypeName typeName = it->second.TypeName;
				if (typeName == "")
					typeName = "uint";
				retStr += FormatString("dcl_uav_typed u%i, %s, %s \n",
					(int)it->first, it->second.ResourceName.c_str(), typeName.c_str() );
				break;
			}
			case raw:
				bHasNonTypedOuputs = true;
				retStr += FormatString("dcl_uav_raw u%i \n", (int)it->first );
				break;

			case structured:
				bHasNonTypedOuputs = true;
				retStr += FormatString("dcl_uav_structured u%i, %d \n", (int)it->first, it->second.Stride );
				break;
			}
		}
	}
	else
	{
		// declare inputs
		FOR_EACH_ITERATOR(it, m_SysvalInputRegMasks, RegSemanticMaskMap)
		{
			int inputRegIdx = it->first;

			MaskFlag inputRegMask = it->second.second;
			tstring inputRegStr = GetInputRegStr(shaderStage, inputRegIdx, ToString(inputRegMask), true);
			tstring formatStr;
			if (it->second.first == "position")
			{
				// all except pixel shader just treat position like a normal attribute
				if (shaderStage == D3D_SHADER_STAGE_PIXEL)
					formatStr = "dcl_input_ps_sv " + inputRegStr + ", linear_noperspective, %s \n";
				else
					formatStr = "dcl_input " + inputRegStr + " \n";
			}
			else if (it->second.first == "vertexID")
			{
				// all except vertex shader just treat vertexID like a normal attribute
				if (shaderStage == D3D_SHADER_STAGE_VERTEX)
					formatStr = "dcl_input_sv " + inputRegStr + ", %s \n";
				else
					formatStr = "dcl_input " + inputRegStr + " \n";
			}
			else
			{
				if (shaderStage == D3D_SHADER_STAGE_PIXEL)
					formatStr = "dcl_input_ps_sv " + inputRegStr + ", constant, %s \n";
				else
					formatStr = "dcl_input_sv " + inputRegStr + ", %s \n";

			}
			tstring semanticStr = it->second.first;
			retStr += FormatString( formatStr.c_str(), semanticStr.c_str() );
		}
		FOR_EACH_ITERATOR(it, m_UserInputRegMasks, RegTypenameInterpolationMaskMap)
		{
			int inputRegIdx = it->first;

			MaskFlag inputRegMask = it->second.second;
			tstring inputRegStr = GetInputRegStr(shaderStage, inputRegIdx, ToString(inputRegMask), true);
			tstring formatStr;
			// for pixel shader, we have an extra %s for interpolation mode, which is in it->second.second.second
			if (shaderStage == D3D_SHADER_STAGE_PIXEL)
				formatStr = "dcl_input_ps " + inputRegStr + ", %s \n";
			else
				formatStr = "dcl_input " + inputRegStr + " \n";
			tstring interpolationStr = it->second.first.second;
			retStr += FormatString( formatStr.c_str(), interpolationStr.c_str() );
		}

		FOR_EACH_ITERATOR(it, m_IndexRangeInputs, IntIntMap)
		{
			int rangeStart = it->first;
			int rangeStop = it->second;

			tstring formatStr = "dcl_indexRange v[%d], v[%d] \n";
			if (shaderStage == D3D_SHADER_STAGE_HULL || shaderStage == D3D_SHADER_STAGE_GEOMETRY)
				formatStr = "dcl_indexRange v[1][%d], v[1][%d] \n";
			else if (shaderStage == D3D_SHADER_STAGE_DOMAIN)
				formatStr = "dcl_indexRange vcp[1][%d], vcp[1][%d] \n";
			retStr += FormatString( formatStr.c_str(), rangeStart, rangeStop );
		}

		// declare outputs
		if (m_PositionOutputRegIdx >= 0)
		{
			const char* formatStr = "dcl_output_sv o[%i].xyzw, position \n";
			// compiler bug? sv_position in control point phase gets hoisted to fork phase?
			if (shaderStage == D3D_SHADER_STAGE_HULL)
				formatStr = "dcl_output o[%i].xyzw \n";
			if (shaderStage == D3D_SHADER_STAGE_PIXEL)
				formatStr = "dcl_output o[%i].xyzw \n";
			retStr += FormatString( formatStr, m_PositionOutputRegIdx );
		}
		FOR_EACH_ITERATOR(it, m_OutputRegMasks, RegOutputMaskMap)
		{
			// only pixel shader needs a typed output to avoid type mismatch with render-target
			TypeName typeName = it->second.TypeName;
			const int outputRegIdx = (int)it->first;

			if (typeName == "" || shaderStage != D3D_SHADER_STAGE_PIXEL)
			{
				const char* formatStr = "dcl_output o[%i].%s \n";
				retStr += FormatString(formatStr,
					outputRegIdx,
					ToString(it->second.DestMask).c_str() );
			}
			else
			{
				const char* formatStr = "dcl_output_typed o[%i].%s, %s \n";
				retStr += FormatString(formatStr,
					outputRegIdx,
					ToString(it->second.DestMask).c_str(),
					typeName.c_str() );
			}
		}
		FOR_EACH_ITERATOR(it, m_IndexRangeOutputs, IntIntMap)
		{
			int rangeStart = it->first;
			int rangeStop = it->second;

			tstring formatStr = "dcl_indexRange o[%d], o[%d] \n";
			retStr += FormatString( formatStr.c_str(), rangeStart, rangeStop );
		}
	}

	// Declare cycle counter
	if( m_bDeclCycleCounter )
	{
		retStr += _T("dcl_input vCycleCounter.") + ToString(m_CycleCounterMask) + _T("\n");
	}

	// Calculate the offset into the UAVs (Note: They all have the same offset)
	// We use vThreadGroupID as an implicit position, so each group invocation writes to a unique location
	// The computed offset will be stored in the .x component of the specified register.
	if( D3D_SHADER_STAGE_COMPUTE == shaderStage &&
		bHasNonTypedOuputs &&
		!m_bForceRawUAVByteOffset )
	{
		retStr += this->GetRawUAVOutputOffsetStr(RESERVED_RAWUAV_OFFSET_CALCULATION_REGISTER);
	}

	// initialize temps
	FOR_EACH_ITERATOR(it, m_MovsToTempFromLit, RegLiteralMap)
	{
		retStr += FormatString("mov r%i.%s, %s \n",
			(int)it->first.first, ToString((MaskFlag)it->first.second).c_str(),
			it->second.c_str() );
	}
	if (shaderStage != D3D_SHADER_STAGE_COMPUTE)
	{
		FOR_EACH_ITERATOR(it, m_MovsToTempFromInput, RegPairMap)
		{
			int inputRegIdx = it->second.first;
			Swizzle inputRegSwizzle = it->second.second;
			tstring inputRegStr = GetInputRegStr(shaderStage, inputRegIdx, ToString(inputRegSwizzle), false);
			tstring formatStr = "mov r%i.%s, " + inputRegStr + " \n";
			retStr += FormatString(formatStr.c_str(),
				(int)it->first.first, ToString((MaskFlag)it->first.second).c_str() );
		}
	}
	FOR_EACH_ITERATOR(it, m_MovsToTempFromCB, RegPairMap)
	{
		retStr += FormatString("mov r%i.%s, c0[%i].%s \n",
			(int)it->first.first, ToString((MaskFlag)it->first.second).c_str(),
			(int)it->second.first, ToString((Swizzle)it->second.second).c_str() );
	}

	FOR_EACH_ITERATOR(it, m_MovsToTGSMFromTemp, RegPairIndexMap)
	{
		switch(((MemoryIndex)it->second.second).memoryType)
		{
		case structured:
			retStr += FormatString("store_structured %s%i, l(%u), l(%u), r%i.%s \n",
				 ((MemoryIndex)it->second.second).prefix.c_str(),
				 (int)it->second.first,
				 ((MemoryIndex)it->second.second).index,
				 ((MemoryIndex)it->second.second).offset,
				(int)it->first.first,
				ToString((MaskFlag)it->first.second).c_str() );
			break;
		case raw:
			retStr += FormatString("store_raw %s%i, l(%u), r%i.%s \n",
				((MemoryIndex)it->second.second).prefix.c_str(),
				(int)it->second.first,
				((MemoryIndex)it->second.second).offset,
				(int)it->first.first,
				ToString((MaskFlag)it->first.second).c_str() );
			break;
		default:
			throw AHException("invaild memory type");
			break;
		}
	}

	FOR_EACH_ITERATOR(it, m_MovsToUAVTypedFromTemp, RegSwizzlePairExtraSwizzleMap)
	{
	  retStr += FormatString("store_uav_typed u%i.%s, r%i.%s, r%i.%s \n",
				(int)it->first.first,
				ToString((MaskFlag)it->first.second).c_str(),
				(int)((SwizzlePairExtraSwizzle)it->second.second).dstAddressId,
				ToString((Swizzle)((SwizzlePairExtraSwizzle)it->second.second).dstAddressSwizzle).c_str(),
				(int)it->second.first,
				ToString((Swizzle)it->second.second.srcSwizzle).c_str());
	}


	// move constant stuff to output
	FOR_EACH_ITERATOR(it, m_MovsToOutputFromLit, RegLiteralMap)
	{
		tstring formatStr
			= GetOutputMovStr(shaderStage, (int)it->first.first, (MaskFlag)it->first.second)
			+ ", %s \n";
		retStr += FormatString(formatStr.c_str(),
			it->second.c_str() );
	}
	if (shaderStage != D3D_SHADER_STAGE_COMPUTE)
	{
		FOR_EACH_ITERATOR(it, m_MovsToOutputFromInput, RegPairMap)
		{
			int inputRegIdx = it->second.first;
			Swizzle inputRegSwizzle = it->second.second;
			tstring inputRegStr = GetInputRegStr(shaderStage, inputRegIdx, ToString(inputRegSwizzle), false);
			tstring movStr = GetOutputMovStr(shaderStage, it->first.first, it->first.second);
			retStr += movStr + ", " + inputRegStr + " \n";
		}
	}

	tstring subroutinesStr;
	size_t firstLabel = instructionSpecificCode.find("label");
	if (firstLabel != tstring::npos)
	{
		subroutinesStr = instructionSpecificCode.substr(firstLabel);
		instructionSpecificCode = instructionSpecificCode.substr(0, firstLabel);
	}

	// add in the custom instruction code
	if (instructionSpecificCode.size() > 0)
	{
		retStr += "\n";
		retStr += instructionSpecificCode;
		retStr += "\n";
	}

	FOR_EACH_ITERATOR(it, m_MovToTempFromTGSM, RegPairIndexMap)
	{
		switch(((MemoryIndex)it->second.second).memoryType)
		{
		case structured:
			retStr += FormatString("ld_structured r%i.%s,  l(%u), l(%u), %s%i \n",
				(int)it->first.first,
				ToString((MaskFlag)it->first.second).c_str(),
				((MemoryIndex)it->second.second).index,
				((MemoryIndex)it->second.second).offset,
				((MemoryIndex)it->second.second).prefix.c_str(),
				(int)it->second.first );
			break;
		case raw:
			retStr += FormatString("ld_raw r%i.%s, l(%u), %s%i \n",
				(int)it->first.first,
				ToString((MaskFlag)it->first.second).c_str(),
				((MemoryIndex)it->second.second).offset,
				((MemoryIndex)it->second.second).prefix.c_str(),
				(int)it->second.first );
			break;
		default:
			throw AHException("invaild memory type for ld_ function");
			break;
		}
	}

	FOR_EACH_ITERATOR(it, m_MovsToTempFromUAVTyped, RegSwizzlePairExtraSwizzleMap)
	{
		retStr += FormatString("ld_uav_typed r%i.%s, r%i.%s, u%i.%s \n",
				(int)it->first.first,
				ToString((MaskFlag)it->first.second).c_str(),
				(int)((SwizzlePairExtraSwizzle)it->second.second).dstAddressId,
				ToString((Swizzle)((SwizzlePairExtraSwizzle)it->second.second).dstAddressSwizzle).c_str(),
				(int)it->second.first,
				ToString((Swizzle)it->second.second.srcSwizzle).c_str());
	}

	// move variable stuff to output
	FOR_EACH_ITERATOR(it, m_MovsToOutputFromTemp, RegPairMap)
	{
		tstring formatStr
			= AssemblyHelper::GetOutputMovStr(shaderStage, (int)it->first.first, (MaskFlag)it->first.second)
			+ ", r%i.%s \n";
		retStr += FormatString(formatStr.c_str(),
			(int)it->second.first, ToString((Swizzle)it->second.second).c_str() );
	}

	// move variable stuff to output
	FOR_EACH_ITERATOR(it, m_MovsToRawOutputFromTemp, RegPairMap)
	{
		tstring formatStr
			= this->GetRawOutputMovStr(shaderStage, (int)it->first.first, (MaskFlag)it->first.second)
			+ ", r%i.%s \n";
		retStr += FormatString(formatStr.c_str(),
			(int)it->second.first, ToString((Swizzle)it->second.second).c_str() );
	}

	// shader-stage-specific finish-up work
	switch (shaderStage)
	{
	default:
		throw AHException("Invalid Shader Stage");
		break;
	case D3D_SHADER_STAGE_VERTEX:
	case D3D_SHADER_STAGE_PIXEL:
	case D3D_SHADER_STAGE_DOMAIN:
	case D3D_SHADER_STAGE_COMPUTE:
		break;
	case D3D_SHADER_STAGE_HULL:
		retStr += "hs_fork_phase \n";
		retStr += "dcl_hs_fork_phase_instance_count 1 \n";
		retStr += "dcl_indexRange o[0], o[1] \n";
		retStr += "dcl_output_sv o0.x, finalLineDensityTessFactor \n";
		retStr += "dcl_output_sv o1.x, finalLineDetailTessFactor \n";
		retStr += "mov o[0].x, 1.0f \n";
		retStr += "mov o[1].x, 1.0f \n";
		retStr += "ret \n";
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
		if (!m_bSuppressEmit)
			retStr += "emit \n";
		break;
	}

	if (subroutinesStr.size() > 0)
	{
		retStr += subroutinesStr;
	}
	retStr += "\n";

	return retStr;
}


tstring AssemblyHelper::MakePassThroughVSToFeedThis(D3D_FEATURE_LEVEL featureLevel, D3D_OPTIONS D3DOptions, D3D_SHADER_STAGE targetShaderStage) const
{
	AssemblyHelper subHelper(0, 0);
	if (targetShaderStage == D3D_SHADER_STAGE_PIXEL)
	{
		if (m_PositionInputRegIdx >= 0)
		{
			const MaskFlag mask = xyzw;
			subHelper.SetPositionOutputRegIdx(m_PositionInputRegIdx);
			subHelper.SetUserInputRegMask(m_PositionInputRegIdx, mask);
			subHelper.SetupMovToOutputFromInput(m_PositionInputRegIdx, mask, m_PositionInputRegIdx, MakeSwizzle(mask) );
		}
		else
		{
			// if we're driving a pixel shader and didn't set position
			subHelper.SetPositionOutputRegIdx(RESERVED_POSITION_REGISTER);
			subHelper.SetupMovToOutputFromLiteral(RESERVED_POSITION_REGISTER, xyzw, "vec4( 0.0, 0.0, 0.0, 1.0 )"); // middle of render-target
		}
	}
	else if (m_PositionInputRegIdx >= 0)
	{
		// avoid linkage errors when the input stage can't handle SV_Position
		MaskFlag mask = xyzw;
		subHelper.SetOutputRegMask(m_PositionInputRegIdx, xyzw);
		subHelper.SetUserInputRegMask(m_PositionInputRegIdx, mask);
		subHelper.SetupMovToOutputFromInput(m_PositionInputRegIdx, mask, m_PositionInputRegIdx, MakeSwizzle(mask) );
	}

	FOR_EACH_ITERATOR(it, m_SysvalInputRegMasks, RegSemanticMaskMap)
	{
		int regIdx = it->first;

		// VS only has 16 input registers for FL10.0, 0 - 15
		if( D3D_FEATURE_LEVEL_10_0 == featureLevel &&
			regIdx > 15 )
			break;

		MaskFlag mask = it->second.second;
		Semantic semName = it->second.first;
		if (semName == "position")
			continue; // position is already taken care of for output
		if (semName == "vertexID")
			subHelper.SetSysvalInputRegMask(regIdx, semName, mask);
		else
			subHelper.SetUserInputRegMask(regIdx, mask);
		subHelper.SetupMovToOutputFromInput(regIdx, mask, regIdx, MakeSwizzle(mask) );
		subHelper.SetOutputRegMask(regIdx, mask);
	}

	FOR_EACH_ITERATOR(it, m_UserInputRegMasks, RegTypenameInterpolationMaskMap)
	{
		int regIdx = it->first;

		// VS only has 16 input registers for FL10.0, 0 - 15
		if( D3D_FEATURE_LEVEL_10_0 == featureLevel &&
			regIdx > 15 )
			break;

		TypeName typeName = it->second.first.first;
		tstring psInterpolation = it->second.first.second;
		MaskFlag mask = it->second.second;
		subHelper.SetUserInputRegMaskTyped(regIdx, mask, psInterpolation, typeName);
		subHelper.SetOutputRegMaskTyped(regIdx, mask, typeName);
		subHelper.SetupMovToOutputFromInput(regIdx, mask, regIdx, MakeSwizzle(mask) );
	}

	// Win7:719858
	// The spec says: "All vertex shaders must have a minimum of one input and one output, which can be as little as one scalar value.
	// Note that System Generated Values such as VertexID(7.17) and InstanceID(7.19) count as input."
	// Thus, we take a peek at the inputs specified, if there are none we add one.

	if( subHelper.GetNumOutputs() == 0 || subHelper.GetNumInputs() == 0 )
	{
		// We will just pass the vertex ID (Note: NOT subHelper)
		MaskFlag mask = AssemblyHelper::x;

		subHelper.SetSysvalInputRegMask(RESERVED_VERTEXID_REGISTER, "vertexID", mask);
		subHelper.SetupMovToOutputFromInput(RESERVED_VERTEXID_REGISTER, AssemblyHelper::x, RESERVED_VERTEXID_REGISTER, MakeSwizzle(mask) );
		subHelper.SetOutputRegMask(RESERVED_VERTEXID_REGISTER, mask);
	}

	return subHelper.MakeAssemblyShader(D3D_SHADER_STAGE_VERTEX, featureLevel, D3DOptions, "");
}


tstring AssemblyHelper::MakePassThroughPSToOutputThis(D3D_FEATURE_LEVEL featureLevel, D3D_OPTIONS D3DOptions) const
{
	AssemblyHelper subHelper(0, 0);
	if (m_PositionOutputRegIdx < 0)
		throw AHException("No position output");
	subHelper.SetPositionInputRegIdx(m_PositionOutputRegIdx);
	FOR_EACH_ITERATOR(it, m_OutputRegMasks, RegOutputMaskMap)
	{
		const int regIdx = it->first;
		const TypeName typeName = it->second.TypeName;
		const MaskFlag mask = it->second.DestMask;
		subHelper.SetUserInputRegMaskTyped(regIdx, mask, "constant", typeName);
		subHelper.SetOutputRegMaskTyped(regIdx, mask, typeName);
		subHelper.SetupMovToOutputFromInput(regIdx, mask, regIdx, MakeSwizzle(mask) );
	}

	return subHelper.MakeAssemblyShader(D3D_SHADER_STAGE_PIXEL, featureLevel, D3DOptions, "");
}


tstring AssemblyHelper::MakePassThroughHSForDS(D3D_FEATURE_LEVEL featureLevel, D3D_OPTIONS D3DOptions) const
{
	AssemblyHelper subHelper(0, 0);
	if (m_PositionInputRegIdx >= 0)
	{
		subHelper.SetPositionOutputRegIdx(m_PositionInputRegIdx);
	}
	FOR_EACH_ITERATOR(it, m_SysvalInputRegMasks, RegSemanticMaskMap)
	{
		int regIdx = it->first;
		MaskFlag mask = it->second.second;
		subHelper.SetUserInputRegMask(regIdx, mask);
		subHelper.SetupMovToOutputFromInput(regIdx, mask, regIdx, MakeSwizzle(mask) );
		if (it->second.first == "position")
			continue; // position is already taken care of for output
		subHelper.SetOutputRegMask(regIdx, mask);
	}
	FOR_EACH_ITERATOR(it, m_UserInputRegMasks, RegTypenameInterpolationMaskMap)
	{
		int regIdx = it->first;
		TypeName typeName = it->second.first.first;
		tstring psInterpolation = it->second.first.second;
		MaskFlag mask = it->second.second;
		subHelper.SetUserInputRegMaskTyped(regIdx, mask, psInterpolation, typeName);
		subHelper.SetOutputRegMaskTyped(regIdx, mask, typeName);
		subHelper.SetupMovToOutputFromInput(regIdx, mask, regIdx, MakeSwizzle(mask) );
	}

	return subHelper.MakeAssemblyShader(D3D_SHADER_STAGE_HULL, featureLevel, D3DOptions, "");
}


tstring AssemblyHelper::MakePassThroughDSForHS(D3D_FEATURE_LEVEL featureLevel, D3D_OPTIONS D3DOptions) const
{
	AssemblyHelper subHelper(0, 0);
	if (m_PositionOutputRegIdx >= 0)
	{
		subHelper.SetUserInputRegMask(m_PositionOutputRegIdx, xyzw);
		subHelper.SetPositionOutputRegIdx(m_PositionOutputRegIdx);
		subHelper.SetupMovToOutputFromInput(m_PositionOutputRegIdx, xyzw, m_PositionOutputRegIdx, MakeSwizzle(xyzw) );
	}
	FOR_EACH_ITERATOR(it, m_OutputRegMasks, RegOutputMaskMap)
	{
		int regIdx = it->first;
		const TypeName typeName = it->second.TypeName;
		const MaskFlag mask = it->second.DestMask;
		subHelper.SetUserInputRegMaskTyped(regIdx, mask, "constant", typeName);
		subHelper.SetOutputRegMaskTyped(regIdx, mask, typeName);
		subHelper.SetupMovToOutputFromInput(regIdx, mask, regIdx, MakeSwizzle(mask) );
	}

	return subHelper.MakeAssemblyShader(D3D_SHADER_STAGE_DOMAIN, featureLevel, D3DOptions, "");
}


static HRESULT MakeAndSetInputLayout(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, ID3D10Blob *pShaderBuf,
							   D3D11_INPUT_ELEMENT_DESC* inputDecls, int numInputDecls,
							   ID3D11InputLayout** ppInputLayout)
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	*ppInputLayout = NULL;

	if(	FAILED(	hr = pDevice->CreateInputLayout( inputDecls, numInputDecls,
		pShaderBuf->GetBufferPointer(),
		pShaderBuf->GetBufferSize(),
		ppInputLayout ) ) )
	{
		tRes = RESULT_FAIL;
	}
	else
	{
		pContext->IASetInputLayout( *ppInputLayout );
	}

	return hr;
}


HRESULT AssemblyHelper::SetupPipelineWithShaderCode(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CShader5xTest* pTest,
													D3D_SHADER_STAGE shaderStage,
													D3D_OPTIONS D3DOptions,
													tstring instructionSpecificCode,
													ID3D11RenderTargetView** pRTVs, ID3D11UnorderedAccessView** pUAVs, int numRenderTargets,
													ID3D10Blob **ppShaderBuf,
													ID3D11VertexShader** ppVS, ID3D11GeometryShader** ppGS,
													ID3D11PixelShader** ppPS, ID3D11HullShader** ppHS,
													ID3D11DomainShader** ppDS, ID3D11ComputeShader** ppCS,
													D3D11_INPUT_ELEMENT_DESC* inputDecls, int numInputDecls,
													ID3D11InputLayout** ppInputLayout,
													bool bCreatePSPassThrough,
													bool bCreateVSPassThrough)
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;
	const D3D_FEATURE_LEVEL featureLevel = D3DOptions.FeatureLevel;
	const int numOutputs = max(1, m_OutputRegMasks.size());
	// we should only ever have up to 8 outputs, unless we are not passing through via pixel shader
	const bool bIsComputeOrPixel = shaderStage == D3D_SHADER_STAGE_COMPUTE || shaderStage == D3D_SHADER_STAGE_PIXEL;
	assert( (!bCreatePSPassThrough && !bIsComputeOrPixel) || (numOutputs == numRenderTargets && numOutputs <= 8) );
	assert( numOutputs <= 32 );

	if (ppShaderBuf)
	{
		SAFE_RELEASE((*ppShaderBuf));
	}
	if (ppInputLayout)
	{
		SAFE_RELEASE((*ppInputLayout));
	}

	ID3D10Blob *pErrorBuf = NULL;
	ID3D10Blob *pShaderBuf = NULL;

	// make sure we have valid variables and that they don't already contain something
	SAFE_RELEASE((*ppVS));
	SAFE_RELEASE((*ppGS));
	SAFE_RELEASE((*ppPS));
	SAFE_RELEASE((*ppHS));
	SAFE_RELEASE((*ppDS));
	SAFE_RELEASE((*ppCS));

	// clear out pipeline state for shaders
	pContext->VSSetShader( NULL, NULL, 0 );
	pContext->GSSetShader( NULL, NULL, 0 );
	pContext->PSSetShader( NULL, NULL, 0 );
	pContext->HSSetShader( NULL, NULL, 0 );
	pContext->DSSetShader( NULL, NULL, 0 );
	pContext->CSSetShader( NULL, NULL, 0 );

	// Check the shader stage that is supposed to be created. Make sure it is valid for the current feature level
	switch( featureLevel )
	{
	default:
		WriteError( _T("Invalid feature level specified.") );
		tRes = RESULT_FAIL;
		goto setupDone;
		break;

	case D3D_FEATURE_LEVEL_10_0:
	case D3D_FEATURE_LEVEL_10_1:
		switch( shaderStage )
		{
		// Invalid
		default:
			WriteError( _T("Invalid shader stage specified for the current feature level.") );
			tRes = RESULT_FAIL;
			goto setupDone;
			break;

		// Valid
		case D3D_SHADER_STAGE_VERTEX:
		case D3D_SHADER_STAGE_GEOMETRY:
		case D3D_SHADER_STAGE_PIXEL:
		case D3D_SHADER_STAGE_COMPUTE:	// Compute is only valid on ref, or HW (if the HW supports it). It is the app's resposibility to run compute only if the HW supports it.
			break;
		};
		break;
	case D3D_FEATURE_LEVEL_11_0:
	case D3D_FEATURE_LEVEL_11_1:
		switch( shaderStage )
		{
		// Invalid
		default:
			WriteError( _T("Invalid shader stage specified for the current feature level.") );
			tRes = RESULT_FAIL;
			goto setupDone;
			break;

		// Valid
		case D3D_SHADER_STAGE_VERTEX:
		case D3D_SHADER_STAGE_HULL:
		case D3D_SHADER_STAGE_DOMAIN:
		case D3D_SHADER_STAGE_GEOMETRY:
		case D3D_SHADER_STAGE_PIXEL:
		case D3D_SHADER_STAGE_COMPUTE:
			break;
		};
		break;
	};

	// set our output texture
	FLOAT clearColor[4] = { 0,0,0,0 };
	UINT clearColorU[4] = {0, 0, 0, 0};

	if (shaderStage == D3D_SHADER_STAGE_COMPUTE)
	{
		for (int n = 0; n < numRenderTargets; ++n)
		{
			if ( pUAVs[n] )
				pContext->ClearUnorderedAccessViewUint(pUAVs[n], clearColorU);
		}
		// clear RTs
		pContext->OMSetRenderTargets( 0, NULL, NULL );
		// set UAVs
		UINT initialCounts[8] = {0};
		pContext->CSSetUnorderedAccessViews( 0, numRenderTargets, pUAVs, initialCounts );
	}
	else
	{
		for (int n = 0; n < numRenderTargets; ++n)
		{
			if ( pRTVs[n] )
				pContext->ClearRenderTargetView( pRTVs[n], clearColor );
		}
		// clear UAVs
		UINT initialCounts[8] = {0};
		ID3D11UnorderedAccessView* pNullUAVs[8] = {NULL};
		pContext->CSSetUnorderedAccessViews( 0, featureLevel >= D3D_FEATURE_LEVEL_11_0 ? 8 : 1, pNullUAVs, initialCounts );
		// set RTs
		pContext->OMSetRenderTargets( numRenderTargets, pRTVs, NULL );
	}

	// set the primitive topology to feed the VS with
	if (shaderStage == D3D_SHADER_STAGE_HULL || shaderStage == D3D_SHADER_STAGE_DOMAIN)
		pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_1_CONTROL_POINT_PATCHLIST );
	else
		pContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );

	// set up for stream output
	const char* outputNames[] =
	{
		"0_",  "1_",  "2_",  "3_",  "4_",  "5_",  "6_",  "7_",  "8_",  "9_",
		"10_", "11_", "12_", "13_", "14_", "15_", "16_", "17_", "18_", "19_",
		"20_", "21_", "22_", "23_", "24_", "25_", "26_", "27_", "28_", "29_",
		"30_", "31_"
	};
	const char* positionName = "SV_Position";
	D3D11_SO_DECLARATION_ENTRY pSODecls[32] =
	{
		{ 0, NULL, 0, 0, 0, 0 },
		// more, all initialized to 0x00
	};
	const UINT pBufferStrides[] =
	{
		numOutputs * 4 * sizeof( DWORD ),
	};

	// The stream outputs are not available on CS and PS
	int componentCountSum = 0;
	int numOutputsLessThanOrEqual64Components = 0;
	if ( shaderStage != D3D_SHADER_STAGE_PIXEL &&
		 shaderStage != D3D_SHADER_STAGE_COMPUTE )
	{
		for (int n = 0; n < numOutputs; ++n)
		{
			if (n == m_PositionOutputRegIdx)
			{
				pSODecls[n].SemanticName = positionName;
				pSODecls[n].ComponentCount = 4;
			}
			else
			{
				pSODecls[n].SemanticName = outputNames[n];
				switch (m_OutputRegMasks[n].DestMask)
				{
				default: assert(false); break;
				case x: pSODecls[n].ComponentCount = 1; break;
				case xy: pSODecls[n].ComponentCount = 2; break;
				case xyz: pSODecls[n].ComponentCount = 3; break;
				case xyzw: pSODecls[n].ComponentCount = 4; break;
				}
			}

			// Sum the number of components being output
			componentCountSum += pSODecls[n].ComponentCount;

			// Keep track of how many components we had before going over the 64 component limit (FL 10.0 and FL 10.1 only). Limit for FL11 is 128.
			if( componentCountSum <= 64 )
				numOutputsLessThanOrEqual64Components++;
		}
	}

	assert(numOutputsLessThanOrEqual64Components <= numOutputs);

	// set up some pass-through shaders
	if (shaderStage != D3D_SHADER_STAGE_COMPUTE)
	{
		if (shaderStage != D3D_SHADER_STAGE_PIXEL && bCreatePSPassThrough)
		{
			// pixel shader is needed to output result to render-target
			// pixel shader needs position input, so this shader needs to output position
			if (m_PositionOutputRegIdx < 0)
			{
				this->SetPositionOutputRegIdx( RESERVED_POSITION_REGISTER );
				this->SetupMovToOutputFromLiteral(RESERVED_POSITION_REGISTER, AssemblyHelper::xyzw, "vec4( 0.0, 0.0, 0.0, 1.0 )"); // middle of render-target
			}

			tstring szPassThroughPSCode = this->MakePassThroughPSToOutputThis( featureLevel, g_TestApp.m_D3DOptions );
			if ( !pTest->AssembleShader( szPassThroughPSCode.c_str(), &pShaderBuf, &pErrorBuf ) )
			{
				if (pErrorBuf != NULL)
				{
					WriteError( _T( "AssembleShader() failed assembling shader: %s" ), pErrorBuf->GetBufferPointer() );
				}
				tRes = RESULT_FAIL;
				goto setupDone;
			}
			if ( FAILED( hr = pDevice->CreatePixelShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, ppPS )) )
			{
				WriteError( _T("Failed to create pixel shader, hr=%s" ), D3DHResultToString(hr).c_str() );
				tRes = RESULT_FAIL;
				goto setupDone;
			}
			SAFE_RELEASE( pErrorBuf );
			SAFE_RELEASE( pShaderBuf );
		}

		if (bCreateVSPassThrough && shaderStage != D3D_SHADER_STAGE_VERTEX)
		{
			// vertex shader needed to drive other shaders via Draw()
			tstring szPassThroughVSCode = this->MakePassThroughVSToFeedThis( featureLevel, g_TestApp.m_D3DOptions, shaderStage );
			if ( !pTest->AssembleShader( szPassThroughVSCode.c_str(), &pShaderBuf, &pErrorBuf ) )
			{
				if (pErrorBuf != NULL)
				{
					WriteError( _T( "AssembleShader() failed assembling shader: %s" ), pErrorBuf->GetBufferPointer() );
				}
				tRes = RESULT_FAIL;
				goto setupDone;
			}
			if ( FAILED( hr = pDevice->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, ppVS )) )
			{
				WriteError( _T("Failed to create vertex shader, hr=%s" ), D3DHResultToString(hr).c_str() );
				tRes = RESULT_FAIL;
				goto setupDone;
			}
			if (ppInputLayout)
			{
				if (FAILED( hr = MakeAndSetInputLayout(pDevice, pContext, pShaderBuf, inputDecls, numInputDecls, ppInputLayout) ))
				{
					WriteError( _T("Failed to create input layout, hr=%s" ), D3DHResultToString(hr).c_str() );
					tRes = RESULT_FAIL;
					goto setupDone;
				}
			}
			SAFE_RELEASE( pErrorBuf );
			SAFE_RELEASE( pShaderBuf );
		}

		// Check the feature level, we only build pass through DS, HS, CS, if we are FL11 or greater
		if( featureLevel >= D3D_FEATURE_LEVEL_11_0 )
		{
			if (shaderStage == D3D_SHADER_STAGE_HULL)
			{
				// domain shader needed to output hull shader results
				tstring szPassThroughDSCode = this->MakePassThroughDSForHS(g_TestApp.m_D3DOptions.FeatureLevel, g_TestApp.m_D3DOptions);
				if ( !pTest->AssembleShader( szPassThroughDSCode.c_str(), &pShaderBuf, &pErrorBuf ) )
				{
					if (pErrorBuf != NULL)
					{
						WriteError( _T( "AssembleShader() failed assembling shader: %s" ), pErrorBuf->GetBufferPointer() );
					}
					tRes = RESULT_FAIL;
					goto setupDone;
				}
				if ( FAILED( hr = pDevice->CreateDomainShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, ppDS )) )
				{
					WriteError( _T("Failed to create domain shader, hr=%s" ), D3DHResultToString(hr).c_str() );
					tRes = RESULT_FAIL;
					goto setupDone;
				}

				// we make stream-out from the domain pass-through shader, not from the hull shader
				{
					hr = pDevice->CreateGeometryShaderWithStreamOutput( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(),
						pSODecls, numOutputs, pBufferStrides, 1, 0, NULL, ppGS );
					if ( FAILED( hr ) )
					{
						WriteError( _T("Failed to create stream-out geometry shader, hr=%s" ), D3DHResultToString(hr).c_str() );
						tRes = RESULT_FAIL;
						goto setupDone;
					}
				}
				SAFE_RELEASE( pErrorBuf );
				SAFE_RELEASE( pShaderBuf );
			}

			if (shaderStage == D3D_SHADER_STAGE_DOMAIN)
			{
				// hull shader needed to drive domain shader
				tstring szPassThroughHSCode = this->MakePassThroughHSForDS(D3D_FEATURE_LEVEL_11_0, g_TestApp.m_D3DOptions);
				if ( !pTest->AssembleShader( szPassThroughHSCode.c_str(), &pShaderBuf, &pErrorBuf ) )
				{
					if (pErrorBuf != NULL)
					{
						WriteError( _T( "AssembleShader() failed assembling shader: %s" ), pErrorBuf->GetBufferPointer() );
					}
					tRes = RESULT_FAIL;
					goto setupDone;
				}
				if ( FAILED( hr = pDevice->CreateHullShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, ppHS )) )
				{
					WriteError( _T("Failed to create hull shader, hr=%s" ), D3DHResultToString(hr).c_str() );
					tRes = RESULT_FAIL;
					goto setupDone;
				}
				SAFE_RELEASE( pErrorBuf );
				SAFE_RELEASE( pShaderBuf );
			}
		}
	}

	{
		// now we should build our own shader (might have changed a little state to make pass-through shaders happy)
		tstring shaderCode = this->MakeAssemblyShader(
			shaderStage, featureLevel, g_TestApp.m_D3DOptions,
			instructionSpecificCode, m_GlobalFlags );

		if ( !pTest->AssembleShader( shaderCode.c_str(), &pShaderBuf, &pErrorBuf, (UINT)m_GlobalFlags ) )
		{
			if (pErrorBuf != NULL)
			{
				WriteError( _T( "AssembleShader() failed assembling shader: %s" ), pErrorBuf->GetBufferPointer() );
			}
			hr = E_INVALIDARG;
			tRes = RESULT_FAIL;
			goto setupDone;
		}

		if (   shaderStage != D3D_SHADER_STAGE_COMPUTE // compute cannot stream out
			&& shaderStage != D3D_SHADER_STAGE_PIXEL   // pixel cannot stream out
			&& shaderStage != D3D_SHADER_STAGE_GEOMETRY // GS will create its own later
			&& shaderStage != D3D_SHADER_STAGE_HULL // HS needs to have DS do stream-out, done earlier
			)
		{
			const int numOutputForSOCompCountLimit = (featureLevel < D3D_FEATURE_LEVEL_11_0) ? numOutputsLessThanOrEqual64Components : numOutputs;
			hr = pDevice->CreateGeometryShaderWithStreamOutput( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(),
				pSODecls, numOutputForSOCompCountLimit, pBufferStrides, 1, 0, NULL, ppGS );
			if ( FAILED( hr ) )
			{
				WriteError( _T("Failed to create stream-out geometry shader, hr=%s" ), D3DHResultToString(hr).c_str() );
				tRes = RESULT_FAIL;
				goto setupDone;
			}
		}
	}

	// create our shader and set the pipeline with pass-throughs
	switch (shaderStage)
	{
	case D3D_SHADER_STAGE_VERTEX:
		if ( FAILED( hr = pDevice->CreateVertexShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, ppVS )) )
		{
			WriteError( _T("Failed to create vertex shader, hr=%s" ), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto setupDone;
		}
		if (ppInputLayout)
		{
			if (FAILED( hr = MakeAndSetInputLayout(pDevice, pContext, pShaderBuf, inputDecls, numInputDecls, ppInputLayout) ))
			{
				WriteError( _T("Failed to create input layout, hr=%s" ), D3DHResultToString(hr).c_str() );
				tRes = RESULT_FAIL;
				goto setupDone;
			}
		}
		pContext->VSSetShader( *ppVS, NULL, 0 );
		pContext->GSSetShader( *ppGS, NULL, 0 ); // stream-out
		pContext->PSSetShader( *ppPS, NULL, 0 ); // pass-through
		break;
	case D3D_SHADER_STAGE_GEOMETRY:
		{
			const int numOutputForSOCompCountLimit = (featureLevel < D3D_FEATURE_LEVEL_11_0) ? numOutputsLessThanOrEqual64Components : numOutputs;
			hr = pDevice->CreateGeometryShaderWithStreamOutput( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(),
				pSODecls, numOutputForSOCompCountLimit, pBufferStrides, 1, 0, NULL, ppGS );
			if ( FAILED( hr ) )
			{
				WriteError( _T("Failed to create geometry shader, hr=%s" ), D3DHResultToString(hr).c_str() );
				tRes = RESULT_FAIL;
				goto setupDone;
			}
			pContext->VSSetShader( *ppVS, NULL, 0 ); // pass-through
			pContext->GSSetShader( *ppGS, NULL, 0 );
			pContext->PSSetShader( *ppPS, NULL, 0 ); // pass-through
		}
		break;
	case D3D_SHADER_STAGE_PIXEL:
		if ( FAILED( hr = pDevice->CreatePixelShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, ppPS )) )
		{
			WriteError( _T("Failed to create pixel shader, hr=%s" ), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto setupDone;
		}
		pContext->VSSetShader( *ppVS, NULL, 0 ); // pass-through
		pContext->PSSetShader( *ppPS, NULL, 0 );
		break;
	case D3D_SHADER_STAGE_HULL:
		if ( FAILED( hr = pDevice->CreateHullShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, ppHS )) )
		{
			WriteError( _T("Failed to create hull shader, hr=%s" ), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto setupDone;
		}
		pContext->VSSetShader( *ppVS, NULL, 0 ); // pass-through
		pContext->HSSetShader( *ppHS, NULL, 0 );
		pContext->DSSetShader( *ppDS, NULL, 0 ); // pass-through
		pContext->GSSetShader( *ppGS, NULL, 0 ); // stream-out
		pContext->PSSetShader( *ppPS, NULL, 0 ); // pass-through
		break;
	case D3D_SHADER_STAGE_DOMAIN:
		if ( FAILED( hr = pDevice->CreateDomainShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, ppDS )) )
		{
			WriteError( _T("Failed to create domain shader, hr=%s" ), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto setupDone;
		}
		pContext->VSSetShader( *ppVS, NULL, 0 ); // pass-through
		pContext->HSSetShader( *ppHS, NULL, 0 ); // pass-through
		pContext->DSSetShader( *ppDS, NULL, 0 );
		pContext->GSSetShader( *ppGS, NULL, 0 ); // stream-out
		pContext->PSSetShader( *ppPS, NULL, 0 ); // pass-through
		break;
	case D3D_SHADER_STAGE_COMPUTE:
		if ( FAILED( hr = pDevice->CreateComputeShader( pShaderBuf->GetBufferPointer(), pShaderBuf->GetBufferSize(), NULL, ppCS )) )
		{
			WriteError( _T("Failed to create compute shader, hr=%s" ), D3DHResultToString(hr).c_str() );
			tRes = RESULT_FAIL;
			goto setupDone;
		}
		pContext->CSSetShader( *ppCS, NULL, 0 );
		break;
	}


setupDone:
	if (ppShaderBuf != NULL)
	{
		if (pShaderBuf != NULL)
			pShaderBuf->AddRef();
		*ppShaderBuf = pShaderBuf;
	}
	SAFE_RELEASE( pErrorBuf );
	SAFE_RELEASE( pShaderBuf );

	return hr;
}


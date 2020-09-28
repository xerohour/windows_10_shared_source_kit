#include "InputAssembler.h" 

CInputAssemblerApp  g_App;

const char szInitInputOutputStruct[] = 
	"struct InOut"	"\r\n"
	"{"				"\r\n";

const char szInitInputStruct[] = 
	"struct In"	"\r\n"
	"{"				"\r\n";

const char szInitOutputStruct[] = 
	"struct Out"	"\r\n"
	"{"				"\r\n";

const char szFinishInputOutputStruct[] = 
	"};"			"\r\n";

const char szInputVertexId[] = 
	"uint vertexID : SV_VertexID;"		"\r\n";

const char szInputInstanceId[] = 
	"uint instanceID : SV_InstanceID;"	"\r\n";

const char szOutputVertexId[] = 
	"uint vertexID : vertexID;"		"\r\n";

const char szOutputInstanceId[] = 
	"uint instanceID : instanceID;"	"\r\n";

const char szSemanticVertexId[] =
	"vertexID";

const char szSemanticInstanceId[] =
	"instanceID";

const char szInputOutputVSBody[] = 
	"InOut main( InOut input )\r\n"
	"{\r\n"
	"	return input; \r\n"
	"}\r\n"
	;

const char szVSBody[] = 
	"Out main( In input )\r\n"
	"{\r\n"
	"	return input; \r\n"
	"}\r\n"
	;

const char szInitVSBody[] = 
	"Out main( In input )\r\n"
	"{\r\n"
	"	Out output; \r\n"
	;

const char szFinishVSBody[] = 
	"	return output; \r\n"
	"}\r\n"
	;

BEGIN_NAMED_VALUES( BufferSizeCase )
	NAMED_VALUE( _T( "MaxBufferSizeCase" ), MaxBufferSizeCase )
	NAMED_VALUE( _T( "SmallSizeCase" ), SmallSizeCase )
	NAMED_VALUE( _T( "SizeRequiredByDrawCase" ), SizeRequiredByDrawCase )
END_NAMED_VALUES( BufferSizeCase )

BEGIN_NAMED_VALUES( VBOffsetsCase )
	NAMED_VALUE( _T( "ZeroOffsets" ), ZeroOffsets )
	NAMED_VALUE( _T( "SmallOffsets" ), SmallOffsets )
	NAMED_VALUE( _T( "BigOffsets" ), BigOffsets )
END_NAMED_VALUES( VBOffsetsCase )

BEGIN_NAMED_VALUES( InputClassCase )
	NAMED_VALUE( _T( "All_Vertex" ), ICC_AllVertex )
	NAMED_VALUE( _T( "All_Instance" ), ICC_AllInstance )
	NAMED_VALUE( _T( "AlternateBy_Vertex" ), ICC_AlternateByVertex )
	NAMED_VALUE( _T( "AlternateBy_Instance" ), ICC_AlternateByInstance )
	NAMED_VALUE( _T( "AlternateBy_2_Vertex" ), ICC_AlternateBy2Vertex )
	NAMED_VALUE( _T( "AlternateBy_2_Instance" ), ICC_AlternateBy2Instance )
	NAMED_VALUE( _T( "AlternateBy_Random_Vertex" ), ICC_AlternateByRandomVertex )
	NAMED_VALUE( _T( "AlternateBy_Random_Instance" ), ICC_AlternateByRandomInstance )
END_NAMED_VALUES( InputClassCase )

BEGIN_NAMED_VALUES( DrawMethod )
	NAMED_VALUE( _T( "Draw" ), Draw )
	NAMED_VALUE( _T( "DrawInstanced" ), DrawInstanced )
	NAMED_VALUE( _T( "DrawInstancedIndirect" ), DrawInstancedIndirect )
	NAMED_VALUE( _T( "DrawIndexed" ), DrawIndexed )
	NAMED_VALUE( _T( "DrawIndexedInstanced" ), DrawIndexedInstanced )
	NAMED_VALUE( _T( "DrawIndexedInstancedIndirect" ), DrawIndexedInstancedIndirect )
	NAMED_VALUE( _T( "DrawAuto" ), DrawAuto )
END_NAMED_VALUES( DrawMethod )

BEGIN_NAMED_VALUES( DrawIndirectType )
	NAMED_VALUE( _T( "UpdateSubresource" ), UpdateSubresource )
	NAMED_VALUE( _T( "CopyStructureCount" ), CopyStructureCount ) 
	NAMED_VALUE( _T( "StreamOutputBuffer" ), StreamOutputBuffer )
	NAMED_VALUE( _T( "RenderTargetBuffer" ), RenderTargetBuffer )
END_NAMED_VALUES( DrawIndirectType )

const D3D11_INPUT_CLASSIFICATION InputClassLayouts[ICC_NumCases][MaxInputElements] =
{
	{ //ICC_AllVertex
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA
	},
	{ //ICC_AllInstance
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA
	},
	{	//ICC_AlternateByVertex
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA
	},
	{	//ICC_AlternateByInstance
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA
	},
	{	//ICC_AlternateBy2Vertex
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA
	},
	{	//ICC_AlternateBy2Instance
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA
	},
	{	//ICC_AlternateByRandomVertex
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
	},
	{	//ICC_AlternateByRandomInstance
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_INSTANCE_DATA,
		D3D11_INPUT_PER_VERTEX_DATA,
	}
};

const UINT InputSlotElements[ISC_NumCases][MaxInputElements] =
{
	{
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31
	},
	{
		2, 1, 0, 1, 2, 0, 0, 1, 2, 2, 1, 1, 0, 2, 0, 2, 2, 2, 1, 1, 1, 1, 0, 0, 2, 0, 1, 0, 1, 2, 1, 0
	}
};

const UINT InstanceDataStepRateElements[IDSRC_NumCases][MaxInputElements] =
{
	{
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
	},
	{
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
	},
	{
		2, 3, 0, 1, 2, 4, 1, 2, 1, 3, 6, 1, 0, 1, 0, 2, 5, 10, 1, 2, 3, 4, 6, 17, 0, 9, 0, 2, 3, 1, 2, 7
	}
};

void LayoutInfo::InitializeInputElements(D3D11_INPUT_ELEMENT_DESC *pInputLayoutDesc, UINT numElements)
{
	pInputElements = pInputLayoutDesc;
	this->numElements = numElements;

	numBuffers = 0;
	numVertexElements = 0;
	numInstanceElements = 0;
	maxSlot = 0;
	IBBoundSize = 0;
	IBOffset = 0;
	IBStride = 0;
	pIBData = NULL;
	SOStride = 0;
	numSOElements = 0;
	numComponents = 0;
	numOutElements = 0;
	numSOElements = 0;
	bVertexId = false;
	bInstanceId = false;
	ZeroMemory(slots, sizeof(BufferSlot) * 32);

	//BUGBUG: stride not calculated correctly when AlignedByteOffset not already filed.

	for (UINT i=0; i<numElements; i++)
	{
		if (pInputLayoutDesc[i].InputSlot >= maxSlot)
			maxSlot = pInputLayoutDesc[i].InputSlot + 1;
		if (pInputLayoutDesc[i].InputSlotClass == D3D11_INPUT_PER_VERTEX_DATA)
		{
			numVertexElements++;
		}
		if (pInputLayoutDesc[i].InputSlotClass == D3D11_INPUT_PER_INSTANCE_DATA)
		{
			numInstanceElements++;
		}

		//Count the number of components used in the layout
		numComponents += GetNumComponentsForFormat(pInputLayoutDesc[i].Format);
	}

	for (UINT i=0; i<maxSlot; i++)
	{
		//Initialize to see if it's been set
		slots[i].firstElement = 0xff00ff00;

		for (UINT iElement=0; iElement<numElements; iElement++)
		{
			if (pInputLayoutDesc[iElement].InputSlot != i)
				continue;

			if (slots[i].firstElement == 0xff00ff00)
				slots[i].firstElement = iElement;

			slots[i].elementsIndex[slots[i].numElements] = iElement;
			
			slots[i].lastElement = iElement;
			slots[i].numElements++;
			numBuffers++;
			
			//get maxFormatSize for this slot
			UINT formatStride = GetFormatStrideInBytes(pInputLayoutDesc[iElement].Format);
			if (formatStride > slots[i].maxFormatStride)
				slots[i].maxFormatStride = formatStride;
			//break;
		}
	}
}

//Sets the elements stride
//Sets the SO stride
//Sets numOutElements
//Sets numSOElements
//Sets SORequiredSize
void LayoutInfo::SetLayoutData(bool bVertexId, bool bInstanceId, UINT numDrawElements)
{
	this->bVertexId = bVertexId;
	this->bInstanceId = bInstanceId;

	//Note: this assumes AlignedByteOffset is filed in the input layout desc, otherwise the stride will not be calculated correctly.
	for (UINT iSlot=0; iSlot<maxSlot; iSlot++)
	{
		//The last element can be different than the last sequential element in the layout
		//the last element is #1 in this example :
		// 0: offset = 0;  RGB32
		// 1: offset = 48; R32
		// 2: offset = 16; RG16
		UINT lastElement = 0;

		//Get the offset
		for (UINT iElement=slots[iSlot].firstElement; iElement <= slots[iSlot].lastElement; iElement++)
		{
			if (pInputElements[iElement].AlignedByteOffset > slots[iSlot].stride)
			{
				slots[iSlot].stride = pInputElements[iElement].AlignedByteOffset;
				lastElement = iElement;
			}
		}

		//Add the size of the last element's format to the stride.
		slots[iSlot].stride += GetFormatStrideInBytes(pInputElements[lastElement].Format);
	}

	UINT FormatStride = numElements > 0 ? GetFormatStrideInBytes(pInputElements[0].Format) : 4;
	SOSizeRequired = numDrawElements * (numElements + (bVertexId ? 1 : 0) + (bInstanceId ? 1 : 0)) * FormatStride;

	//if the amount of data to be written to the SO is too much OR if there's too many components for a SO buffer, limit the number of Output registers
	bool bLimitOutput = (SOSizeRequired > MaxSOBufferSize) || (numComponents > 64);
	numOutElements = bLimitOutput ? numElements / 2 : numElements;

	numSOElements = numOutElements + (bVertexId ? 1 : 0) + (bInstanceId ? 1 : 0);

	//Set the SO Stride
	SOStride = numSOElements * FormatStride;
}

//
std::string  LayoutInfo::CreateVSCode()
{
	assert(numElements + (bVertexId ? 1 : 0) + (bInstanceId ? 1 : 0) <= MaxInputElements);

	//Create the input structure
	std::string szVSInputStruct = szInitInputStruct;
	std::string szVSOutputStruct = szInitOutputStruct;

	for (UINT iElement=0; iElement<numElements; iElement++)
	{
		std::string semanticName = szSemantic + ToString(iElement);

		//Get correct type.
		szVSInputStruct += "\t" + GetShaderTypeFromFormat(pInputElements[iElement].Format) + " " + semanticName + " : " + semanticName + ";\r\n";
		if (iElement < numOutElements)
			szVSOutputStruct += "\t" + GetShaderTypeFromFormat(pInputElements[iElement].Format) + " " + semanticName + " : " + semanticName + ";\r\n";
	}

	if (bVertexId)
	{
		szVSInputStruct += szInputVertexId;
		szVSOutputStruct += szOutputVertexId;
	}

	if (bInstanceId)
	{
		szVSInputStruct += szInputInstanceId;
		szVSOutputStruct += szOutputInstanceId;
	}

	szVSInputStruct += szFinishInputOutputStruct;
	szVSOutputStruct += szFinishInputOutputStruct;


	std::string szVSNewBody;
	if (numOutElements != numElements)
	{
		szVSNewBody = szInitVSBody;
		for (UINT i=0; i < numOutElements; i++)
		{
			std::string semanticNameOut = szSemantic + ToString(i);
			std::string semanticNameA = szSemantic + ToString(i * 2);
			szVSNewBody += "\toutput." + semanticNameOut +" = input." + semanticNameA + ";\r\n";
		}
		if (bVertexId)
		{
			szVSNewBody += "\toutput.vertexID = input.vertexID;\r\n";
		}

		if (bInstanceId)
		{
			szVSNewBody += "\toutput.instanceID = input.instanceID;\r\n";
		}
		szVSNewBody += szFinishVSBody;
	}
	else
	{
		szVSNewBody = szVSBody;
	}


	return szVSInputStruct + szVSOutputStruct + szVSNewBody;
}

//
void LayoutInfo::CreateStreamOutputDecl(D3D11_SO_DECLARATION_ENTRY *pSODecl)
{
	//Set the SO decl
	for (UINT iElement=0; iElement<numOutElements; iElement++)
	{
		pSODecl[iElement].Stream = 0;
		pSODecl[iElement].SemanticName = szSemantic;
		pSODecl[iElement].SemanticIndex = iElement;

		pSODecl[iElement].StartComponent = 0;
		pSODecl[iElement].ComponentCount = (BYTE)GetNumComponentsForFormat(pInputElements[iElement].Format);
		pSODecl[iElement].OutputSlot = 0;
	}

	UINT currentEntry = numOutElements;
	if (bVertexId)
	{
		pSODecl[currentEntry].SemanticName = szSemanticVertexId;
		pSODecl[currentEntry].SemanticIndex = 0;
		pSODecl[currentEntry].StartComponent = 0;
		pSODecl[currentEntry].ComponentCount = 1;
		pSODecl[currentEntry].OutputSlot = 0;

		currentEntry++;
	}

	if (bInstanceId)
	{
		pSODecl[currentEntry].SemanticName = szSemanticInstanceId;
		pSODecl[currentEntry].SemanticIndex = 0;
		pSODecl[currentEntry].StartComponent = 0;
		pSODecl[currentEntry].ComponentCount = 1;
		pSODecl[currentEntry].OutputSlot = 0;
	}
}



CInputAssemblerTest::CInputAssemblerTest() : 
	m_pDevice(NULL), 
	m_pDeviceContext(NULL),
	m_pDebug(NULL),
	m_pInfoQueue(NULL), 
	m_pDSState(NULL),
	m_pSOBuffer(NULL), 
	m_pSOBufferZero(NULL),
	m_pSOBufferCopy(NULL), 
	m_pSOVBBuffer(NULL), 
	m_pSOArgBuffer(NULL), 
	m_pIndexBufferBig(NULL), 
	m_pIndexBufferSmall(NULL), 
	m_pDrawIndirectArgumentBuffer(NULL),
	m_pInputLayout(NULL), 
	m_pQuery(NULL), 
	m_pGS(NULL), 
	m_pVS(NULL),
	m_nBufferUAVFlag(0),
	m_bVertexID(false),
	m_bInstanceID(false),
	m_NumElements(0),
	m_MaxNumElements(0)
{

	for (UINT i=0; i<MaxInputElements; i++)
	{
		m_pVertexBuffers[i] = NULL;
	}

	memset( &m_BufferData[0][0], 0, sizeof(m_BufferData) );
	memset( &m_IndexBufferData[0], 0, sizeof(m_IndexBufferData) );

	//Unless overriden, create the buffers in Setup()
	m_bSetupBuffers = true;
}

TEST_RESULT CInputAssemblerTest::Setup()
{
	HRESULT hr;

	m_pDevice = GetDevice();
	m_pDeviceContext = GetEffectiveContext();

	if ( g_App.m_D3DOptions.Debug )
	{
		m_pDebug = g_App.GetDebug();
		m_pInfoQueue = g_App.GetInfoQueue();
	}
	else
	{
		m_pDebug = NULL;
		m_pInfoQueue = NULL;
	}

	GetShell()->SetNumDeviceWindows( 0 );

	if ( GetFramework()->m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1 )
	{
		m_MaxNumElements = 32;
	}
	else
	{
		m_MaxNumElements = 16;
	}

	//
	//Disable Rasterizer
	//
	SAFE_RELEASE( m_pDSState );

	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = FALSE;
	desc.StencilEnable = FALSE;

	if ( FAILED( m_pDevice->CreateDepthStencilState( &desc, &m_pDSState ) ) )
	{
		WriteToLog( "CreateDepthStencilState() failed" );
		return RESULT_FAIL;
	}

	m_pDeviceContext->OMSetDepthStencilState( m_pDSState, NULL );

	if (m_bSetupBuffers)
	{
		//
		//Create SO Buffer
		//
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.ByteWidth = MaxSOBufferSize;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, NULL, &m_pSOBuffer ) ) )
		{
			WriteToLog( "CreateBuffer(SO) failed" );
			return RESULT_FAIL;
		}

		bufferDesc.ByteWidth = MaxSOBufferSize;
		bufferDesc.Usage = D3D11_USAGE_STAGING;
		bufferDesc.BindFlags = 0;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;

		if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, NULL, &m_pSOBufferCopy ) ) )
		{
			WriteToLog( "CreateBuffer(SOCopy) failed" );
			return RESULT_FAIL;
		}

		ID3D11Buffer *pBuffers[] = { m_pSOBuffer };
		UINT Offsets[] = { 0 };
		m_pDeviceContext->SOSetTargets( 1, pBuffers, Offsets );

		//
		//Create input buffers
		//
		ZeroMemory( m_BufferData, sizeof(m_BufferData) );

		bufferDesc.ByteWidth = MaxBufferSize;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;
		for (UINT i=0; i<MaxInputElements; i++)
		{
			//TODO: probably need better data in there
			for (UINT iData=0; iData<MaxBufferSize; iData++)
			{
				m_BufferData[i][iData] = (i + iData) % 255;
			}

			D3D11_SUBRESOURCE_DATA subData;
			subData.pSysMem = (void *)m_BufferData[i];
			subData.SysMemPitch = 0;
			subData.SysMemSlicePitch = 0;

			if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, &subData, &m_pVertexBuffers[i] ) ) )
			{
				WriteToLog( "CreateBuffer() failed" );
				return RESULT_FAIL;
			}
		}

		//
		//Create SO-VB Buffer for DrawAuto input
		//
		bufferDesc.ByteWidth = MaxBufferSize;
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		////use data from vertex buffers
		//D3D11_SUBRESOURCE_DATA subData;
		//subData.pSysMem = (void *)m_BufferData[0];
		//subData.SysMemPitch = 0;
		//subData.SysMemSlicePitch = 0;

		if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, NULL/*&subData*/, &m_pSOVBBuffer ) ) )
		{
			WriteToLog( "CreateBuffer(SOVB) failed" );
			return RESULT_FAIL;
		}

		//
		//Create Index Buffers
		//
		bufferDesc.ByteWidth = MaxBufferSize * sizeof(WORD);
		bufferDesc.Usage = D3D11_USAGE_DEFAULT;
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = 0;
		bufferDesc.MiscFlags = 0;

		//TODO: probably need better data in there
		for (UINT iData=0; iData<MaxBufferSize; iData++)
		{
			m_IndexBufferData[iData] = (iData * iData + 643) % 65536;
		}

		D3D11_SUBRESOURCE_DATA subData;
		subData.pSysMem = (void *)m_IndexBufferData;
		subData.SysMemPitch = 0;
		subData.SysMemSlicePitch = 0;

		if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, &subData, &m_pIndexBufferBig ) ) )
		{
			WriteToLog( "CreateBuffer(m_pIndexBufferBig) failed" );
			return RESULT_FAIL;
		}

		bufferDesc.ByteWidth = 1 * sizeof(WORD);
		if ( FAILED( GetFramework()->CreateBufferTiledOptional( &bufferDesc, &subData, &m_pIndexBufferSmall ) ) )
		{
			WriteToLog( "CreateBuffer(m_pIndexBufferSmall) failed" );
			return RESULT_FAIL;
		}
	}

	//
	//Create Query
	//
	D3D11_QUERY_DESC    queryDesc;

	memset( &queryDesc, 0, sizeof(queryDesc) );
	queryDesc.Query = D3D11_QUERY_PIPELINE_STATISTICS;
	queryDesc.MiscFlags = 0;

	if ( FAILED(hr = m_pDevice->CreateQuery( &queryDesc, &m_pQuery ) ) )
	{
		WriteToLog( _T("Failed to create Query. hr=%s\n"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	//
	//Set topology
	//
	m_pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		return RESULT_FAIL;
	}

	return RESULT_PASS;
}

void CInputAssemblerTest::Cleanup()
{
	HRESULT hr;
	if (m_pDevice)
	{
		ID3D11Buffer *pBuffers[] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
		UINT Offsets[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		UINT Strides[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		m_pDeviceContext->SOSetTargets( 1, pBuffers, Offsets );
		m_pDeviceContext->OMSetDepthStencilState( NULL, NULL );

		m_pDeviceContext->IASetVertexBuffers(0, m_MaxNumElements, pBuffers, Strides, Offsets);
		m_pDeviceContext->IASetIndexBuffer(NULL, DXGI_FORMAT_R16_UINT, 0);

		m_pDeviceContext->VSSetShader( NULL, NULL, 0 );
		m_pDeviceContext->GSSetShader( NULL, NULL, 0 );
		m_pDeviceContext->IASetInputLayout( NULL );
		if( FAILED( hr = ExecuteEffectiveContext() ) )
		{
			WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		}
	}

	SAFE_RELEASE( m_pDSState );
	SAFE_RELEASE( m_pSOBuffer );
	SAFE_RELEASE( m_pSOBufferCopy );
	SAFE_RELEASE( m_pSOVBBuffer );
	SAFE_RELEASE( m_pIndexBufferBig );
	SAFE_RELEASE( m_pIndexBufferSmall );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pQuery );
	SAFE_RELEASE( m_pGS );
	SAFE_RELEASE( m_pVS );

	for (UINT i=0; i<MaxInputElements; i++)
		SAFE_RELEASE( m_pVertexBuffers[i] );
}

void CInputAssemblerTest::InitTestParameters()
{
}

//Make sure that LayoutInfo.SOSizeRequired is set before calling this
TEST_RESULT  CInputAssemblerTest::SetupPipeline(bool bNullInputLayout)
{
	TEST_RESULT     tRes = RESULT_PASS;
	HRESULT         hr;
	ID3D10Blob     *pVSShaderBuf = NULL;
	ID3D10Blob     *pVSErrorBuf = NULL;
	ID3D11Buffer   *pNullBuffer[] = { NULL };
	UINT            ZeroOffsets[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


	// Create a VS that matches.
	tstring szVS = m_LayoutInfo.CreateVSCode();

	LPCSTR  pszTarget = GetFramework()->GetShaderProfile( D3D_SHADER_STAGE_VERTEX, D3D_FEATURE_LEVEL_10_0, D3D_FEATURE_LEVEL_11_0);
	if ( pszTarget == NULL )
	{
		WriteToLog( _T("%s in %s at line %d.  FAILED: CD3D11TestFramework::GetShaderProfile(): No shader profile found that matches the current feature level and required feature level range."), __FUNCTION__, __FILE__, __LINE__ );
		goto TESTFAIL;
	}

	if ( FAILED( hr = D3DX11CompileFromMemory( szVS.c_str(), szVS.length(), "test.fx", NULL, NULL, "main", pszTarget, 0, 0, NULL, &pVSShaderBuf, &pVSErrorBuf, NULL ) ) )
	{
		if ( pVSErrorBuf != NULL && pVSErrorBuf->GetBufferPointer() != NULL )
		{
			WriteToLog( _T("%s in %s at line %d.  FAILED: D3DX11CompileFromMemory() failed - hr = %s, %s"), __FUNCTION__, __FILE__, __LINE__, D3DHResultToString(hr).c_str(), pVSErrorBuf->GetBufferPointer() );
		}
		else
		{
			WriteToLog( _T("%s in %s at line %d.  FAILED: D3DX11CompileFromMemory() failed - hr = %s"), __FUNCTION__, __FILE__, __LINE__, D3DHResultToString(hr).c_str() );
		}
  
		goto TESTFAIL;
	}

	hr = m_pDevice->CreateVertexShader( pVSShaderBuf->GetBufferPointer(), pVSShaderBuf->GetBufferSize(), NULL, &m_pVS );
	if ( FAILED(hr) )
	{
		WriteToLog( _T("%s in %s at line %d.  FAILED: CreateVertexShader() failed - hr = %s"), __FUNCTION__, __FILE__, __LINE__, D3DHResultToString(hr).c_str() );
		goto TESTFAIL;
	}

	// Create a SO decl that matches
	D3D11_SO_DECLARATION_ENTRY  soLayout[MaxInputElements] = {0};

	// Preallocate memory for semantic names
	char  szSOSemantic[MaxInputElements][32] = {0};

	for ( int i = 0; i < MaxInputElements; i++ )
	{
		soLayout[i].SemanticName = szSOSemantic[i];
	}

	m_LayoutInfo.CreateStreamOutputDecl( soLayout );

	hr = m_pDevice->CreateGeometryShaderWithStreamOutput( pVSShaderBuf->GetBufferPointer(), pVSShaderBuf->GetBufferSize(), soLayout, m_LayoutInfo.numSOElements, &m_LayoutInfo.SOStride, 1, 0, NULL, &m_pGS );
	if ( FAILED(hr) )
	{
		WriteToLog( "CreateGeometryShaderWithStreamOutput() failed" );
		goto TESTFAIL;
	}

	if (bNullInputLayout)
	{
		m_pDeviceContext->IASetInputLayout( NULL );
	}
	else
	{
		//Create the input layout
		hr = m_pDevice->CreateInputLayout(m_InputElements, m_NumElements, pVSShaderBuf->GetBufferPointer(), pVSShaderBuf->GetBufferSize(), &m_pInputLayout);
		if ( FAILED(hr) )
		{
			WriteToLog( "CreateInputLayout() failed" );
			goto TESTFAIL;
		}

		m_pDeviceContext->IASetInputLayout( m_pInputLayout );
	}

	m_pDeviceContext->VSSetShader( m_pVS, NULL, 0 );
	m_pDeviceContext->GSSetShader( m_pGS, NULL, 0 );
	m_pDeviceContext->PSSetShader( NULL, NULL, 0 );

	m_pDeviceContext->SOSetTargets(1, pNullBuffer, ZeroOffsets);
	m_pDeviceContext->SOSetTargets(1, &m_pSOBuffer, ZeroOffsets);

	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		goto TESTFAIL;
	}

	goto testDone;

TESTFAIL:
	tRes = RESULT_FAIL;

testDone:

	SAFE_RELEASE( pVSErrorBuf );
	SAFE_RELEASE( pVSShaderBuf );

	return tRes;
}

bool CInputAssemblerTest::VerifyDrawResults(UINT VertexCount, UINT StartVertexLocation)
{
	return VerifyDrawInstancedResults( VertexCount, 1, StartVertexLocation, 0 );
}

bool CInputAssemblerTest::VerifyDrawInstancedResults(UINT VertexCountPerInstance, UINT InstanceCount, UINT StartVertexLocation, UINT StartInstanceLocation)
{
	bool bRes = false;
	HRESULT hr;
	//Verify the content

	BYTE *pResult = NULL;
	BYTE *pCurOffset = NULL;
	UINT64 bufferOffsets[MaxInputElements];
	UINT uValidationBufferOffset[MaxInputElements][MaxInputElements]; // 32-bit offset into our validation buffer.   This needs to be 32-bit.  We want to explore 32-bit overflow.
	UINT zeroValues[4];
	ZeroMemory( uValidationBufferOffset, sizeof(uValidationBufferOffset) );
	ZeroMemory( bufferOffsets, sizeof(bufferOffsets) );
	ZeroMemory( zeroValues, sizeof(zeroValues) );

	//End the query
	GetEffectiveContext()->End(m_pQuery);
	
	//Execute the effective context after the query end
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	//Clear buffer
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if (FAILED (hr = GetImmediateContext()->Map( m_pSOBufferCopy, 0, D3D11_MAP_WRITE, 0, &mappedRes )))
	{
		WriteToLog( "Map (WRITE) failed on SO Buffer copy with hr=%s", D3DHResultToString(hr).c_str() );
		return false;
	}
	ZeroMemory(mappedRes.pData, MaxSOBufferSize);
	GetImmediateContext()->Unmap(m_pSOBufferCopy,0);

	m_pDeviceContext->CopyResource( m_pSOBufferCopy, m_pSOBuffer );
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	if (FAILED (hr = GetImmediateContext()->Map( m_pSOBufferCopy, 0, D3D11_MAP_READ, 0, &mappedRes )))
	{
		WriteToLog( "Map (READ) failed on SO Buffer copy with hr=%s", D3DHResultToString(hr).c_str() );
		return false;
	}


	
	pResult = (BYTE*)mappedRes.pData;
	pCurOffset = pResult;

	UINT InstanceDataStepRateCounter[MaxInputElements];

	for (UINT iSlot=0; iSlot<m_LayoutInfo.maxSlot; iSlot++)
	{
		if (m_LayoutInfo.slots[iSlot].numElements == 0)
			continue;

		UINT startLocation = 0;
		if (m_InputElements[ m_LayoutInfo.slots[iSlot].firstElement ].InputSlotClass == D3D11_INPUT_PER_VERTEX_DATA)
			startLocation = StartVertexLocation;
		else if (m_InputElements[ m_LayoutInfo.slots[iSlot].firstElement ].InputSlotClass == D3D11_INPUT_PER_INSTANCE_DATA)
			startLocation = StartInstanceLocation;

		bufferOffsets[iSlot] = (UINT64)m_LayoutInfo.slots[iSlot].VBOffset + (UINT64)m_LayoutInfo.slots[iSlot].stride * (UINT64)startLocation;

		for (UINT iElement=m_LayoutInfo.slots[iSlot].firstElement; iElement <= m_LayoutInfo.slots[iSlot].lastElement; iElement++)
		{
			if (m_InputElements[iElement].InputSlot != iSlot)
				continue;

			if (m_InputElements[iElement].InputSlotClass == D3D11_INPUT_PER_INSTANCE_DATA)
				InstanceDataStepRateCounter[iElement] = m_InputElements[iElement].InstanceDataStepRate;

			uValidationBufferOffset[iSlot][iElement] = m_LayoutInfo.slots[iSlot].VBOffset + (UINT64)m_LayoutInfo.slots[iSlot].stride * (UINT64)startLocation + m_InputElements[iElement].AlignedByteOffset;

		}
	}

	//Compare SO buffer with input buffers

	for (UINT iInstance=0; iInstance<InstanceCount; iInstance++)
	{
		for (UINT iVertex=0; iVertex<VertexCountPerInstance; iVertex++)
		{
			pCurOffset = pResult + (iInstance * VertexCountPerInstance * m_LayoutInfo.SOStride) + (iVertex * m_LayoutInfo.SOStride);

			for (UINT iElement=0; iElement<m_LayoutInfo.numOutElements; iElement++)
			{
				UINT ElementIndex = iElement;
				if (m_LayoutInfo.numOutElements != m_LayoutInfo.numElements)
				{
					ElementIndex = iElement * 2;
				}
				UINT iSlot = m_InputElements[ElementIndex].InputSlot;

				// The following pointer calculation can be bogus depending on the offset were using.  The test is smart enough not to read from this buffer except when it's valid.				
				void* pCurElement = (void*)(m_LayoutInfo.slots[iSlot].pVBData + uValidationBufferOffset[iSlot][ElementIndex]);

				UINT64 currentBufferOffset = (UINT64)bufferOffsets[iSlot] + (UINT64)m_InputElements[ElementIndex].AlignedByteOffset + (UINT64)m_LayoutInfo.slots[iSlot].maxFormatStride;
				int res = 0;
				bool b32bitOverflow = false;
				bool bOutOfBounds = false;

				//Not well defined for 32bit overflow.  Could wrap or return default of 0
				if (currentBufferOffset > UINT_MAX)
				{
					res = memcmp( zeroValues, pCurOffset, m_LayoutInfo.slots[iSlot].maxFormatStride );
					if (res != 0)
					{
						//Wrapping and out of bounds causes partial vertex read
						//Note: HW should return 0 like in regular out of bound case
						//
						//We are intentionally typecasting currentBufferOffset to a UINT to clamp it to 32-bits for this comparison.						
						if ((UINT)currentBufferOffset > m_LayoutInfo.slots[iSlot].VBBoundSize)
						{
							bOutOfBounds = true;
						}
						else
						{
							res = memcmp( pCurElement, pCurOffset, m_LayoutInfo.slots[iSlot].maxFormatStride );
						}
					}
					b32bitOverflow = true;
				}
				else
				{
					if (currentBufferOffset > m_LayoutInfo.slots[iSlot].VBBoundSize)
					{
						//Reading out of bound, so result should be 0.
						res = memcmp( zeroValues, pCurOffset, m_LayoutInfo.slots[iSlot].maxFormatStride );
						bOutOfBounds = true;
					}
					else
					{
						res = memcmp( pCurElement, pCurOffset, m_LayoutInfo.slots[iSlot].maxFormatStride );
					}
				}
				if (res != 0)
				{
					if (b32bitOverflow)
					{
						WriteToLog( "BufferOffset is > UINT_MAX, so 32bit overflow expects either 0 or to wrap" );
					}
					if (bOutOfBounds)
					{
						WriteToLog( "BufferOffset is out of bounds, so expects a value of 0" );
						if ((currentBufferOffset - m_LayoutInfo.slots[iSlot].maxFormatStride) <= m_LayoutInfo.slots[iSlot].VBBoundSize)
							WriteToLog( "In this case, the last element read doesn't fit entirely in the buffer, so all the elements read should be 0" );
					}
					tstring value = GetHexOfMemory(pCurOffset, m_LayoutInfo.slots[iSlot].maxFormatStride);
					tstring expected;
					if ( bOutOfBounds )
						expected = GetHexOfMemory((BYTE*)zeroValues, m_LayoutInfo.slots[iSlot].maxFormatStride);
					else if (b32bitOverflow)
						expected = GetHexOfMemory((BYTE*)pCurElement, m_LayoutInfo.slots[iSlot].maxFormatStride);
					else
						expected = GetHexOfMemory((BYTE*)pCurElement, m_LayoutInfo.slots[iSlot].maxFormatStride);
					WriteToLog( "Data from SO (0x%s) is not what is expected (0x%s) at Instance#%i, Vertex#%i, Element#%i", value.c_str(), expected.c_str(), iInstance, iVertex, iElement );
					goto TESTFAIL;
				}

				//TODO: the format stride might need to be adjusted
				pCurOffset += m_LayoutInfo.slots[iSlot].maxFormatStride;

				//Do not increment for instance
				if (m_InputElements[ElementIndex].InputSlotClass == D3D11_INPUT_PER_VERTEX_DATA)
				{
					uValidationBufferOffset[iSlot][ElementIndex] += m_LayoutInfo.slots[iSlot].stride; // This might intentionally overflow.
					bufferOffsets[iSlot] += m_LayoutInfo.slots[iSlot].maxFormatStride;
				}
			}

			if (m_bVertexID)
			{
				UINT SOVertexID = ((UINT*)pCurOffset)[0];
				if (SOVertexID != iVertex)
				{
					WriteToLog( "VertexID from SO expected : %i, result : %i", iVertex, SOVertexID );
					goto TESTFAIL;
				}
				pCurOffset += sizeof(UINT);
			}

			if (m_bInstanceID)
			{
				UINT SOInstanceID = ((UINT*)pCurOffset)[0];
				if (SOInstanceID != iInstance)
				{
					WriteToLog( "InstanceID from SO expected : %i, result : %i", iInstance, SOInstanceID );
					goto TESTFAIL;
				}
				pCurOffset += sizeof(UINT);
			}	
		}
		//increment uValidationBufferOffset for instance data and restart them for vertex data
		//take InstanceDataStepRate in account.
		for (UINT iSlot=0; iSlot<m_LayoutInfo.maxSlot; iSlot++)
		{
			if (m_LayoutInfo.slots[iSlot].numElements == 0)
				continue;

			for (UINT iElement=m_LayoutInfo.slots[iSlot].firstElement; iElement <= m_LayoutInfo.slots[iSlot].lastElement; iElement++)
			{
				if (m_InputElements[iElement].InputSlot != iSlot)
					continue;

				if (m_InputElements[iElement].InputSlotClass == D3D11_INPUT_PER_VERTEX_DATA)
				{
					uValidationBufferOffset[iSlot][iElement] = m_LayoutInfo.slots[iSlot].VBOffset + m_LayoutInfo.slots[iSlot].stride * StartVertexLocation + m_InputElements[iElement].AlignedByteOffset;
					bufferOffsets[iSlot] = (UINT64)m_LayoutInfo.slots[iSlot].VBOffset + (UINT64)StartVertexLocation * (UINT64)m_LayoutInfo.slots[iSlot].stride;

				}
				else if (m_InputElements[iElement].InputSlotClass == D3D11_INPUT_PER_INSTANCE_DATA)
				{
					if (1 == InstanceDataStepRateCounter[iElement])
					{
						uValidationBufferOffset[iSlot][iElement] += m_LayoutInfo.slots[iSlot].stride; // This might intentionally overflow.
						InstanceDataStepRateCounter[iElement] = m_InputElements[iElement].InstanceDataStepRate;
						bufferOffsets[iSlot] += m_LayoutInfo.slots[iSlot].maxFormatStride;
					}
					else if (1 < InstanceDataStepRateCounter[iElement])
					{
						InstanceDataStepRateCounter[iElement]--;
					}
				}

			}
		}
	}

	//Get the query results (done at the end to give it more time)
	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipeStatsEnd;
	hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pQuery, &pipeStatsEnd, sizeof(pipeStatsEnd), 0 );

	if( S_FALSE == hr )
	{
		// If GetDataLoop() returns S_FALSE, then the query hit the timeout
        WriteToLog( _T( "CInputAssemblerTest::VerifyDrawInstancedResultspQuery() - GetDataWithTimeout() timed out after %i seconds\n" ), GetFramework()->GetQueryTimeout() );

		// We should only fail if we needed to check the counters
		if( ShouldCheckCounters( InstanceCount ) )
			goto TESTFAIL;
	}

	if( FAILED(hr) )
	{
		WriteToLog( _T( "CInputAssemblerTest::VerifyDrawInstancedResultspQuery() - GetDataWithTimeout() failed with %s" ), D3DHResultToString(hr).c_str() );
		goto TESTFAIL;
	}

	if( ShouldCheckCounters( InstanceCount ) )
	{
		UINT64 expectedValue = InstanceCount * VertexCountPerInstance;
		const bool bVSInvocationsCanBeSmallerThanExpected = IsNoVBBound(); // Win8: 7341 - Some HW does optimizations on VS invocation when VBs are unbound, so the VSInvocation count can be smaller than expected value
		if ( !bVSInvocationsCanBeSmallerThanExpected && 
			 pipeStatsEnd.VSInvocations != expectedValue )
		{
			WriteToLog( _T( "Pipeline stats VSInvocations (%u) is not the expected value (%u)." ), (UINT)pipeStatsEnd.VSInvocations, (UINT)expectedValue );
			goto TESTFAIL;
		}
		if ( (bVSInvocationsCanBeSmallerThanExpected) &&
			 (pipeStatsEnd.VSInvocations > expectedValue || pipeStatsEnd.VSInvocations == 0) )
		{
			WriteToLog( _T( "Pipeline stats VSInvocations (%u) is zero or greater than the expected value (%u)." ), (UINT)pipeStatsEnd.VSInvocations, (UINT)expectedValue );
			goto TESTFAIL;
		}
		if (pipeStatsEnd.IAPrimitives != expectedValue)
		{
			WriteToLog( _T( "Pipeline stats IAPrimitives (%u) is not the expected value (%u)." ), (UINT)pipeStatsEnd.IAPrimitives, (UINT)expectedValue );
			goto TESTFAIL;
		}
		if (pipeStatsEnd.IAVertices != expectedValue)
		{
			WriteToLog( _T( "Pipeline stats IAVertices (%u) is not the expected value (%u)." ), (UINT)pipeStatsEnd.IAVertices, (UINT)expectedValue );
			goto TESTFAIL;
		}
	}

	bRes = true;

TESTFAIL:
	GetImmediateContext()->Unmap(m_pSOBufferCopy,0);

	return bRes;
}

bool CInputAssemblerTest::VerifyDrawIndexedResults(UINT IndexCount, UINT StartIndexLocation, INT BaseVertexLocation)
{
	return VerifyDrawIndexedInstancedResults(IndexCount, 1, StartIndexLocation, BaseVertexLocation, 0);
}

bool CInputAssemblerTest::VerifyDrawIndexedInstancedResults(UINT IndexCountPerInstance, UINT InstanceCount, UINT StartIndexLocation, INT BaseVertexLocation, UINT StartInstanceLocation)
{
	bool bRes = false;
	HRESULT hr;
	//Verify the content

	BYTE *pResult = NULL;
	BYTE *pCurOffset = NULL;
	WORD *pCurIndexBuffer = NULL;
	UINT64 bufferOffsets[MaxInputElements];
	UINT uValidationBufferOffset[MaxInputElements][MaxInputElements];
	UINT zeroValues[4];
	//TODO: need to use something better for 32bit index formats
	bool *bIndicesUsed = new bool[USHRT_MAX];
	ZeroMemory( uValidationBufferOffset, sizeof(uValidationBufferOffset) );
	ZeroMemory( bufferOffsets, sizeof(bufferOffsets) );
	ZeroMemory( zeroValues, sizeof(zeroValues) );

	UINT64 minVSInvocations = 0;
	UINT64 maxVSInvocations = 0;

	//End the query
	GetEffectiveContext()->End(m_pQuery);

	//Execute the effective context after the query end
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		return false;
	}

	//Clear buffer
	D3D11_MAPPED_SUBRESOURCE mappedRes;
	if (FAILED (hr = GetImmediateContext()->Map( m_pSOBufferCopy, 0, D3D11_MAP_WRITE, 0, &mappedRes )))
	{
		WriteToLog( "Map (WRITE) failed on SO Buffer copy with hr=%s", D3DHResultToString(hr).c_str() );
		bRes = false;
		goto TESTFAIL;
	}
	ZeroMemory(mappedRes.pData, MaxSOBufferSize);
	GetImmediateContext()->Unmap(m_pSOBufferCopy,0);

	m_pDeviceContext->CopyResource( m_pSOBufferCopy, m_pSOBuffer );
	if( FAILED( hr = ExecuteEffectiveContext() ) )
	{
		WriteToLog( _T("Failed to execute deferred context. hr=%s"), D3DHResultToString(hr).c_str() );
		bRes = false;
		goto TESTFAIL;
	}

	if (FAILED (hr = GetImmediateContext()->Map( m_pSOBufferCopy, 0, D3D11_MAP_READ, 0, &mappedRes )))
	{
		WriteToLog( "Map (READ) failed on SO Buffer copy with hr=%s", D3DHResultToString(hr).c_str() );
		bRes = false;
		goto TESTFAIL;
	}
	
	pResult = (BYTE*) mappedRes.pData;
	pCurOffset = pResult;

	UINT InstanceDataStepRateCounter[MaxInputElements];

	for (UINT iSlot=0; iSlot<m_LayoutInfo.maxSlot; iSlot++)
	{
		if (m_LayoutInfo.slots[iSlot].numElements == 0)
			continue;

		if (m_InputElements[m_LayoutInfo.slots[iSlot].firstElement].InputSlotClass != D3D11_INPUT_PER_INSTANCE_DATA)
			continue;

		bufferOffsets[iSlot] = (UINT64)m_LayoutInfo.slots[iSlot].VBOffset + (UINT64)m_LayoutInfo.slots[iSlot].stride * (UINT64)StartInstanceLocation;

		for (UINT iElement=m_LayoutInfo.slots[iSlot].firstElement; iElement <= m_LayoutInfo.slots[iSlot].lastElement; iElement++)
		{
			if (m_InputElements[iElement].InputSlot != iSlot)
				continue;

			InstanceDataStepRateCounter[iElement] = m_InputElements[iElement].InstanceDataStepRate;

			UINT64 currentBufferOffset = (UINT64)m_LayoutInfo.slots[iSlot].VBOffset + (UINT64)m_LayoutInfo.slots[iSlot].stride * (UINT64)StartInstanceLocation + (UINT64)m_InputElements[iElement].AlignedByteOffset;
			uValidationBufferOffset[iSlot][iElement] = (UINT)currentBufferOffset;

		}
	}

	//UINT numFormatComponents = GetNumComponentsForFormat(m_InputElements[0].Format);
	//DWORD FormatType = GetFormatType(m_InputElements[0].Format);

	//Compare SO buffer with input buffers

	for (UINT iInstance=0; iInstance<InstanceCount; iInstance++)
	{
		// reset index buffer for each instance
		pCurIndexBuffer = ((WORD*)m_LayoutInfo.pIBData) + StartIndexLocation + (m_LayoutInfo.IBOffset / m_LayoutInfo.IBStride);
		
		// reset if indices were used for each instance
		ZeroMemory( bIndicesUsed, sizeof( bool ) * USHRT_MAX );

		for (UINT iIndex=0; iIndex<IndexCountPerInstance; iIndex++)
		{
			pCurOffset = pResult + (iInstance * IndexCountPerInstance * m_LayoutInfo.SOStride) + (iIndex * m_LayoutInfo.SOStride);
			assert(pCurOffset);

			UINT IndexValue = 0;
			//make sure that IndexValue is 0 if out of bounds
			UINT64 uIndexOffset = (INT64)iIndex + (INT64)StartIndexLocation + (INT64)( m_LayoutInfo.IBOffset / m_LayoutInfo.IBStride); 
			if ( uIndexOffset <= UINT_MAX && m_LayoutInfo.IBBoundSize > (UINT)uIndexOffset )
			{
				IndexValue = pCurIndexBuffer[0];
			}
			if (! bIndicesUsed[IndexValue])
				minVSInvocations++;
			maxVSInvocations++;
			bIndicesUsed[IndexValue] = true;

			for (UINT iElement=0; iElement<m_LayoutInfo.numOutElements; iElement++)
			{
				UINT ElementIndex = iElement;
				if (m_LayoutInfo.numOutElements != m_LayoutInfo.numElements)
				{
					ElementIndex = iElement * 2;
				}
				UINT iSlot = m_InputElements[ElementIndex].InputSlot;

				UINT64 currentBufferOffset = 0;
				INT64 indexedOffset = 0; 
				if (m_InputElements[ElementIndex].InputSlotClass == D3D11_INPUT_PER_VERTEX_DATA)
				{
					indexedOffset = (INT64)m_LayoutInfo.slots[iSlot].stride * ((INT64)BaseVertexLocation + (INT64)IndexValue);
					UINT64 uIndexedOffset = (UINT64)indexedOffset; //This value is UINT since it can't be < 0
					currentBufferOffset = (UINT64)m_LayoutInfo.slots[iSlot].VBOffset + (UINT64)uIndexedOffset + (UINT64)m_InputElements[ElementIndex].AlignedByteOffset;
					uValidationBufferOffset[iSlot][ElementIndex] = (UINT)currentBufferOffset;
					currentBufferOffset += (UINT64)m_LayoutInfo.slots[iSlot].maxFormatStride;
				}
				else if (m_InputElements[iElement].InputSlotClass == D3D11_INPUT_PER_INSTANCE_DATA)
				{
					currentBufferOffset = (UINT64)bufferOffsets[iSlot] + (UINT64)m_InputElements[ElementIndex].AlignedByteOffset + (UINT64)m_LayoutInfo.slots[iSlot].maxFormatStride;
				}

				int res = 0;
				bool b32bitOverflow = false;
				bool bOutOfBounds = false;

				//Make sure SO Buffer is big enough
				assert( (UINT)(pCurOffset - pResult) <= MaxSOBufferSize );

				//Not well defined for 32bit overflow.  Could wrap or return default of 0
				if (currentBufferOffset > UINT_MAX)
				{
					res = memcmp( zeroValues, pCurOffset, m_LayoutInfo.slots[iSlot].maxFormatStride );
					if (res != 0)
					{
						//Wrapping and out of bounds causes partial vertex read
						//Note: HW should return 0 like in regular out of bound case
						//
						//We are intentionally typecasting currentBufferOffset to a UINT to clamp it to 32-bits for this comparison.						
						if (((UINT)currentBufferOffset > m_LayoutInfo.slots[iSlot].VBBoundSize) || (indexedOffset < 0) )
						{
							bOutOfBounds = true;
						}
						else
						{
							res = memcmp( (void*)(m_LayoutInfo.slots[iSlot].pVBData + uValidationBufferOffset[iSlot][ElementIndex]), pCurOffset, m_LayoutInfo.slots[iSlot].maxFormatStride );
						}
					}
					b32bitOverflow = true;
				}
				else
				{
					if ( (currentBufferOffset > m_LayoutInfo.slots[iSlot].VBBoundSize) || (indexedOffset < 0) )
					{
						//Reading out of bound, so result should be 0.
						res = memcmp( zeroValues, pCurOffset, m_LayoutInfo.slots[iSlot].maxFormatStride );
						bOutOfBounds = true;
					}
					else
					{
						res = memcmp( (void*)(m_LayoutInfo.slots[iSlot].pVBData + uValidationBufferOffset[iSlot][ElementIndex]), pCurOffset, m_LayoutInfo.slots[iSlot].maxFormatStride );
					}
				}
				if (res != 0)
				{
					if (b32bitOverflow)
					{
						WriteToLog( "BufferOffset is > UINT_MAX, so 32bit overflow expects either 0 or to wrap" );
					}
					if (bOutOfBounds)
					{
						WriteToLog( "BufferOffset is out of bounds, so expects a value of 0" );
						// In the following comparison, we are intentionally casting the 64-bit currentBufferOffset to a UINT to clamp it to 32-bits.
						if (((UINT)currentBufferOffset - m_LayoutInfo.slots[iSlot].maxFormatStride) <= m_LayoutInfo.slots[iSlot].VBBoundSize)
							WriteToLog( "In this case, the last element read doesn't fit entirely in the buffer, so all the elements read should be 0" );
						if (indexedOffset < 0)
							WriteToLog( "In this case, the indexed offset is smaller than 0" );
					}
					tstring value = GetHexOfMemory(pCurOffset, m_LayoutInfo.slots[iSlot].maxFormatStride);
					tstring expected;
					if ( bOutOfBounds )
						expected = GetHexOfMemory((BYTE*)zeroValues, m_LayoutInfo.slots[iSlot].maxFormatStride);
					else if (b32bitOverflow)
						expected = GetHexOfMemory((BYTE*)(m_LayoutInfo.slots[iSlot].pVBData) + uValidationBufferOffset[iSlot][ElementIndex], m_LayoutInfo.slots[iSlot].maxFormatStride);
					else
						expected = GetHexOfMemory((BYTE*)(m_LayoutInfo.slots[iSlot].pVBData) + uValidationBufferOffset[iSlot][ElementIndex], m_LayoutInfo.slots[iSlot].maxFormatStride);
					WriteToLog( "Data from SO (0x%s) is not what is expected (0x%s) at Instance#%i, Index#%i, Element#%i, IndexValue:%i", value.c_str(), expected.c_str(), iInstance, iIndex, iElement, IndexValue );
					goto TESTFAIL;
				}

				//TODO: the format stride might need to be adjusted
				pCurOffset += m_LayoutInfo.slots[iSlot].maxFormatStride;
			}

			if (m_bVertexID)
			{
				UINT SOVertexID = ((UINT*)pCurOffset)[0];
				if (SOVertexID != IndexValue)
				{
					WriteToLog( "VertexID from SO expected : %i, result : %i", IndexValue, SOVertexID );
					goto TESTFAIL;
				}
				pCurOffset += sizeof(UINT);
			}

			if (m_bInstanceID)
			{
				UINT SOInstanceID = ((UINT*)pCurOffset)[0];
				if (SOInstanceID != iInstance)
				{
					WriteToLog( "InstanceID from SO expected : %i, result : %i", iInstance, SOInstanceID );
					goto TESTFAIL;
				}
				pCurOffset += sizeof(UINT);
			}	

			pCurIndexBuffer++;
		}

		//increment uValidationBufferOffset for instance data
		//take InstanceDataStepRate in account.
		for (UINT iSlot=0; iSlot<m_LayoutInfo.maxSlot; iSlot++)
		{
			if (m_LayoutInfo.slots[iSlot].numElements == 0)
				continue;

			for (UINT iElement=m_LayoutInfo.slots[iSlot].firstElement; iElement <= m_LayoutInfo.slots[iSlot].lastElement; iElement++)
			{
				if (m_InputElements[iElement].InputSlot != iSlot)
					continue;

				//No need to restart uValidationBufferOffset for VertexData since it's calculated for each vertex (because of the index)
				if (m_InputElements[iElement].InputSlotClass != D3D11_INPUT_PER_INSTANCE_DATA)
					continue;

				if (1 == InstanceDataStepRateCounter[iElement])
				{
					uValidationBufferOffset[iSlot][iElement] += m_LayoutInfo.slots[iSlot].stride;
					bufferOffsets[iSlot] += m_LayoutInfo.slots[iSlot].maxFormatStride;
					InstanceDataStepRateCounter[iElement] = m_InputElements[iElement].InstanceDataStepRate;
				}
				else if (1 < InstanceDataStepRateCounter[iElement])
				{
					InstanceDataStepRateCounter[iElement]--;
				}
			}
		}
	}

	//Get the query results (done at the end to give it more time)
	D3D11_QUERY_DATA_PIPELINE_STATISTICS pipeStatsEnd;
	hr = GetFramework()->GetDataWithTimeout( GetImmediateContext(), m_pQuery, &pipeStatsEnd, sizeof(pipeStatsEnd), 0 );
	if (FAILED(hr))
	{
		WriteToLog( _T( "pQuery->GetData failed with %s" ), D3DHResultToString(hr).c_str() );
		goto TESTFAIL;
	}

	if( ShouldCheckCounters( InstanceCount ) )
	{
        if (hr == S_FALSE)
        {
		    WriteToLog( _T( "pQuery->GetData timed out" ) );
		    goto TESTFAIL;
        }
		UINT64 expectedValue = InstanceCount * IndexCountPerInstance;
		const bool bVSInvocationsCanBeSmallerThanExpected = IsNoVBBound(); // Win8: 7341 - Some HW does optimizations on VS invocation when VBs are unbound, so the VSInvocation count can be smaller than expected value
		if ( (!bVSInvocationsCanBeSmallerThanExpected) && 
			 (pipeStatsEnd.VSInvocations < minVSInvocations || pipeStatsEnd.VSInvocations > maxVSInvocations) )
		{
			WriteToLog( "Pipeline stats VSInvocations (%u) is not in the expected range (%u to %u).", (UINT)pipeStatsEnd.VSInvocations, (UINT)minVSInvocations, (UINT)maxVSInvocations );
			goto TESTFAIL;
		}
		if ( (bVSInvocationsCanBeSmallerThanExpected) && 
			 (pipeStatsEnd.VSInvocations < 1 || pipeStatsEnd.VSInvocations > maxVSInvocations) )
		{
			WriteToLog( "Pipeline stats VSInvocations (%u) is not in the expected range (%u to %u).", (UINT)pipeStatsEnd.VSInvocations, (UINT)1, (UINT)maxVSInvocations );
			goto TESTFAIL;
		}
		if (pipeStatsEnd.IAPrimitives != expectedValue)
		{
			WriteToLog( _T( "Pipeline stats IAPrimitives (%u) is not the expected value (%u)." ), (UINT)pipeStatsEnd.IAPrimitives, (UINT)expectedValue );
			goto TESTFAIL;
		}
		if (pipeStatsEnd.IAVertices != expectedValue)
		{
			WriteToLog( _T( "Pipeline stats IAVertices (%u) is not the expected value (%u)." ), (UINT)pipeStatsEnd.IAVertices, (UINT)expectedValue );
			goto TESTFAIL;
		}
	}

	bRes = true;

TESTFAIL:
	GetImmediateContext()->Unmap(m_pSOBufferCopy,0);
	delete[]( bIndicesUsed );

	return bRes;
}


bool CInputAssemblerTest::VerifyDrawAutoResults(UINT VertexCount)
{
	return VerifyDrawInstancedResults( VertexCount, 1, 0, 0 );
}

// If there are no elements in the input layout that are per-vertex (D3D11_INPUT_PER_VERTEX_DATA)
// then the implementation may choose to optimize this scenario and run the VS fewer times than InstanceCount * VertexCountPerInstance
// In this case the VS and IA counters would be lower than expected.  The counters are only checked in the case where there is per-vertex data
// OR
// If two instances have all per-instance attributes the same and the VS does not read InstanceID, 
// hardware can merge vertices across instances, which also results in lower VS counts (win7 bug: 709302)
bool CInputAssemblerTest::ShouldCheckCounters( UINT InstanceCount)
{
	// We don't check counters if priority 1
	if( GetFramework()->GetExecutionPriorityLevel() == 1 )
		return false;

	bool bCheckCounters = false;
	bool bIsAllPerInstance = true;
	bool bIsAllPerVertex = true;
	for( UINT i = 0; i < m_NumElements; i++ )
	{
		if( D3D11_INPUT_PER_VERTEX_DATA == m_InputElements[i].InputSlotClass )
		{   
			bIsAllPerInstance = false;
		}
		if( D3D11_INPUT_PER_INSTANCE_DATA == m_InputElements[i].InputSlotClass )
		{   
			bIsAllPerVertex = false;
		}
	}

	if (!bIsAllPerInstance)
	{
		if ( InstanceCount == 1 )
			bCheckCounters = true;
		// if all attributes are per-vertex, than instances are identical
		else if ( !bIsAllPerVertex )
		{
			for( UINT i = 0; i < m_NumElements; i++ )
			{
				// if the InstanceDataStepRate of any per-instance attribute is smaller than 2 but non-zero,
				// than no two instances would be identical
				if( 1 == m_InputElements[i].InstanceDataStepRate )
				{   
					bCheckCounters = true;
				}
			}
			
		}
	}

	return bCheckCounters;
}



// Valid
	//CTestCaseParameter< bool > *pMissingShaderElements = AddParameter< bool >( _T("MissingShaderElements"), &m_bMissingShaderElements);
	//testfactor::RFactor rMissingShaderElements = AddParameterValue< bool >( pMissingShaderElements, true);
	//SetParameterDefaultValue< bool >( pMissingShaderElements, false );
//CONF: Shader:float4, input:float3, hardware should set last float default value.
//CONF: Input element size bigger than shader element size. (Shader:float2, input:float4)
//CONF: Matrix : same semantic name, different index, different format

//////////////////////////////////////////////////////////////////////////////////////////////////

void CInputAssemblerApp::InitOptionVariables()
{
	CD3D11TestFramework::InitOptionVariables();

	// TODO: REF only is a temporary measure while no HAL capable hardware available
	UnRegisterOptionVariable( _T( "SrcOnly" ) );
	RegisterOptionVariable("DebugHelper", "DebugHelper", &m_bDebugHelper, false);
	m_D3DOptions.SrcOnly = true;
}


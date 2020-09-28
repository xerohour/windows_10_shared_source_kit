#include "InputAssembler.h" 

void ShuffleElements(UINT seed, UINT numElements, UINT *pArray)
{
	//Set the seed to the test case number
	srand(seed);

	for (UINT i = 0; i<numElements; i++)
	{
		pArray[i] = i;
	}

	UINT random;
	UINT temp;
	for (UINT last = numElements; last > 1; last--)
	{
		random = rand() % last;
		temp = pArray[random];
		pArray[random] = pArray[last - 1];
		pArray[last - 1] = temp;
	}
}


std::string GetShaderTypeFromFormat(DXGI_FORMAT format)
{
	const DXGIFormatInfo *pFormatInfo = CFormatInfoMap::Find(format);

	//Other formats not handled
	assert(pFormatInfo->dwFlags & (FF_UNORM | FF_SNORM | FF_FLOAT | FF_UINT | FF_SINT));

	std::string type;
	if (pFormatInfo->dwFlags & (FF_UNORM | FF_SNORM | FF_FLOAT))
		type = "float";
	else if (pFormatInfo->dwFlags & FF_UINT)
		type = "uint";
	else if (pFormatInfo->dwFlags & FF_SINT)
		type = "int";

	UINT numComponents = 0;
	for (; numComponents<4; numComponents++)
	{
		if (pFormatInfo->pBitsPerComponent[numComponents] == 0)
			break;
	}

	char vectorSize[2];
	vectorSize[0] = '0' + numComponents;
	vectorSize[1] = 0;
	type += vectorSize;

	return type;
}

DWORD GetFormatType(DXGI_FORMAT format)
{
	const DXGIFormatInfo *pFormatInfo = CFormatInfoMap::Find(format);

	//Other formats not handled
	assert(pFormatInfo->dwFlags & (FF_UNORM | FF_SNORM | FF_FLOAT | FF_UINT | FF_SINT));

	std::string type;
	if (pFormatInfo->dwFlags & (FF_UNORM | FF_SNORM | FF_FLOAT))
		return FF_FLOAT;
	else if (pFormatInfo->dwFlags & FF_UINT)
		return FF_UINT;
	else if (pFormatInfo->dwFlags & FF_SINT)
		return FF_SINT;

	return 0;
}

UINT GetNumComponentsForFormat(DXGI_FORMAT format)
{
	const DXGIFormatInfo *pFormatInfo = CFormatInfoMap::Find(format);

	//Other formats not handled
	assert(pFormatInfo->dwFlags & (FF_UNORM | FF_SNORM | FF_FLOAT | FF_UINT | FF_SINT));

	UINT numComponents = 0;
	for (; numComponents<4; numComponents++)
	{
		if (pFormatInfo->pBitsPerComponent[numComponents] == 0)
			break;
	}

	return numComponents;
}


UINT GetFormatStrideInBytes(DXGI_FORMAT format)
{
	const DXGIFormatInfo *pFormatInfo = CFormatInfoMap::Find(format);

	//Other formats not handled
	assert(pFormatInfo->dwFlags & (FF_UNORM | FF_SNORM | FF_FLOAT | FF_UINT | FF_SINT));

	UINT bytes = pFormatInfo->uBitsPerElement / 8;

	//Return 1 byte for formats with less than 8 bit.
	return bytes == 0 ? 1 : bytes;
}

tstring GetHexOfMemory(const BYTE* pMemory, UINT stride)
{
	tstring result = "";
	TCHAR szBuffer[10];
	for (UINT i=0; i<stride; i++)
	{
		sprintf(szBuffer, "%02x", pMemory[i]);
		result += szBuffer;
	}
	return result;
}

void AddFormatValues(DWORD formatType, UINT numComponents, void* pData1, void* pData2, void* pResult)
{
	if (formatType == FF_FLOAT)
	{
		for (UINT i=0; i<numComponents; i++)
		{
			((float*)pResult)[i] = ((float*)pData1)[i] + ((float*)pData2)[i];
		}
	}
	else if (formatType == FF_SINT)
	{
		for (UINT i=0; i<numComponents; i++)
		{
			((int*)pResult)[i] = ((int*)pData1)[i] + ((int*)pData2)[i];
		}
	}
	else if (formatType == FF_UINT)
	{
		for (UINT i=0; i<numComponents; i++)
		{
			((UINT*)pResult)[i] = ((UINT*)pData1)[i] + ((UINT*)pData2)[i];
		}
	}
}

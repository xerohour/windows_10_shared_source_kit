#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "ResMan.h"

CLRUTest::CLRUTest(PRIM_TYPES types)
{
	if (types == IS_TEXTURE)
	{
		m_Options.D3DOptions.fMinDXVersion = max(7.f, m_Options.D3DOptions.fMinDXVersion);
		m_szTestName   = "LRU Texture Test";
		m_szCommandKey = "lrutex";
		m_testType     = IS_TEXTURE;
		m_PrimCount    = 4;
	}
	else if (types == IS_CUBEMAP)
	{
		m_Options.D3DOptions.fMinDXVersion = max(7.f, m_Options.D3DOptions.fMinDXVersion);
		m_szTestName   = "LRU CubeMap Test";
		m_szCommandKey = "lrucube";
		m_testType     = IS_CUBEMAP;
		m_PrimCount    = 1;
	}
	else if (types == IS_VOLUME)
	{
		m_Options.D3DOptions.fMinDXVersion = max(8.f, m_Options.D3DOptions.fMinDXVersion);
		m_szTestName   = "LRU Volume Test";
		m_szCommandKey = "lruvol";
		m_testType     = IS_VOLUME;
		m_PrimCount    = 1;
	}
	else if (types == IS_NULL)
	{
		m_Options.D3DOptions.fMinDXVersion = max(8.f, m_Options.D3DOptions.fMinDXVersion);
		m_szTestName   = "LRU Mixed Test";
		m_szCommandKey = "lrumix";
		m_testType     = IS_NULL;
		m_PrimCount    = 1;
	}
	//m_Options.TestOptions.dwCaps |= TESTCAPS_SERIALEXECUTION;
}

//-----------------------------------------------------------------------------

UINT CLRUTest::TestInitialize(void)
{
	UINT   result;
	TCHAR  tcsBuffer[100];

	srand(10000);

	m_FixedEnd      = false;
	m_IsOutOfMemory = false;
	m_CountConstMemoryLeft     = 0;
	m_CountNegativeMemoryUsage = 0;
	m_EndIndex = NUMBER_OF_TESTS;
	m_ExtraCount = 0;

	SetTestRange(1, NUMBER_OF_TESTS);

	result = ResMan::TestInitialize();
	if ( result != D3DTESTINIT_RUN )
		return result;

	m_IsHWProcessing = m_pSrcDevice->GetBehaviors() & DEVICECREATE_HWVP;

	switch (m_testType)
	{
		case IS_TEXTURE:
		{
			m_MinExhaustNum = 100;
			if (!m_UseTextures)
			{
				WriteToLog("CLRUTest: Textures are not supported.\n");
				return D3DTESTINIT_SKIPALL;
			}
			m_UseCubeMaps  = false;
			m_UseVolMaps   = false;
			break;
		}
		case IS_CUBEMAP:
		{
			m_MinExhaustNum = 60;
			if (!m_UseCubeMaps)
			{
				WriteToLog("CLRUTest: Cube maps are not supported.\n");
				return D3DTESTINIT_SKIPALL;
			}
			m_UseTextures  = false;
			m_UseVolMaps   = false;
			break;
		}
		case IS_VOLUME:
		{
			m_MinExhaustNum = 60;
			if (!m_UseVolMaps)
			{
				WriteToLog("CLRUTest: Volumes are not supported.\n");
				return D3DTESTINIT_SKIPALL;
			}
			m_UseTextures  = false;
			m_UseCubeMaps  = false;
			break;
		}
		case IS_NULL:
		{
			m_MinExhaustNum = m_IsHWProcessing ? 100 : 60;
			break;
		}
		default:
			WriteToLog("CLRUTest: invalid m_testType %d.\n", m_testType);
			return D3DTESTINIT_ABORT;

	}
	//get initial video memory
	m_GetMemPrev = GetAvailableTextureMem();

#if !_WIN64
	{
		const UINT VIDMEM_CLAMP_2GB = ((UINT)((((ULONGLONG)2) * 1024 * 1024 * 1024) - (64 * 1024)));
		if (m_GetMemPrev > VIDMEM_CLAMP_2GB)
			m_GetMemPrev = VIDMEM_CLAMP_2GB;
	}
#endif
    
	if (KeySet("showmem"))
		WriteToLog("Initial memory=%u.\n", m_GetMemPrev);
	//see if we should run the fixed number of cases
	int temp;
	TCHAR tempStr[30];
	sprintf(tempStr, "%sExhaust", m_szCommandKey);
	m_FixedEnd = ReadInteger(tempStr, &temp);
	if (m_FixedEnd)
	{
		m_EndIndex = (DWORD)temp;
		WriteToLog("CLRUTest: Memory is exhausted on test %d\n", m_EndIndex);
	}
	//see if we should output the data into the file
	if (KeySet("output"))
	{
		sprintf(tempStr, "%s%d.txt", m_szCommandKey, (int)(m_pD3D->GetVersion()));
        m_output = fopen(tempStr, "w");
		if (NULL == m_output || fprintf(m_output, "%s\n", "      ") != 7)
			return D3DTESTINIT_ABORT;
		
	}
	//see if we should use the data from the file
	else if (KeySet("input"))
	{
		sprintf(tempStr, "%s%d.txt", m_szCommandKey, (int)(m_pD3D->GetVersion()));
        m_input = fopen(tempStr, "r");
		m_FixedEnd = true;
		if (m_input)
		{
			if ( fscanf(m_input, "%d", &m_EndIndex) == 1)
				WriteToLog("CLRUTest: Memory is exhausted at test %d\n", m_EndIndex);
			else
			{
				WriteToLog("CLRUTest: Error reading the file.\n");
				return D3DTESTINIT_ABORT;
			}
		}
		else
		{
			WriteToLog("CLRUTest: Error opening the file for reading.\n");
			return D3DTESTINIT_ABORT;
		}
	}

	if (KeySet("memfile"))
	{
		memfile = NULL;
		sprintf(tempStr, "%s%s%d.txt", "mem_", m_szCommandKey, (int)(m_pD3D->GetVersion()));
		memfile = fopen(tempStr, "w");
		GlobalMemoryStatus(&Buffer);
		fprintf(memfile, "%5s\t", "TestNum");
		fprintf(memfile, "%10s\t", "MemoryLoad");
		fprintf(memfile, "%10s\t", "TotalPhys");
		fprintf(memfile, "%10s\t", "AvailPhys");
		fprintf(memfile, "%10s\t", "TotalPageFile");
		fprintf(memfile, "%10s\t", "AvailPageFile");
		fprintf(memfile, "%10s\t", "TotalVirtual");
		fprintf(memfile, "%10s\n", "AvailVirtual");

		fprintf(memfile, "%5d\t", 0);
		fprintf(memfile, "%10d\t", Buffer.dwMemoryLoad);
		fprintf(memfile, "%10d\t", Buffer.dwTotalPhys);
		fprintf(memfile, "%10d\t", Buffer.dwAvailPhys);
		fprintf(memfile, "%10d\t", Buffer.dwTotalPageFile);
		fprintf(memfile, "%10d\t", Buffer.dwAvailPageFile);
		fprintf(memfile, "%10d\t", Buffer.dwTotalVirtual);
		fprintf(memfile, "%10d\n", Buffer.dwAvailVirtual);
	}

	return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------

bool CLRUTest::ExecuteTest(UINT iTest)
{
	HRESULT hr;
	msgString[0] = '\0';
	m_Message[0] = '\0';
	TCHAR szBuffer[30];

	UINT uStartIndex = iTest;
	if (iTest != 1 && (iTest == m_pRangeList->uStart || (m_pConstraints != NULL && iTest == m_pConstraints->uStart)))
		uStartIndex = 1;

	for (UINT index = uStartIndex; index <= iTest; index++)
	{
		if (memfile && (index-1)%25 == 0)
		{
			GlobalMemoryStatus(&Buffer);
			fprintf(memfile, "%5d\t", iTest);
			fprintf(memfile, "%10d\t", Buffer.dwMemoryLoad);
			fprintf(memfile, "%10d\t", Buffer.dwTotalPhys);
			fprintf(memfile, "%10d\t", Buffer.dwAvailPhys);
			fprintf(memfile, "%10d\t", Buffer.dwTotalPageFile);
			fprintf(memfile, "%10d\t", Buffer.dwAvailPageFile);
			fprintf(memfile, "%10d\t", Buffer.dwTotalVirtual);
			fprintf(memfile, "%10d\n", Buffer.dwAvailVirtual);
		}

		//analizing the video memory status
		if ( !m_FixedEnd && index > 1 && index <= m_EndIndex && m_CurrentOccupiedMemory > 300000 )
		{
			m_GetMem = GetAvailableTextureMem();
#if !_WIN64
			{
				const UINT VIDMEM_CLAMP_2GB = ((UINT)((((ULONGLONG)2) * 1024 * 1024 * 1024) - (64 * 1024)));
				if (m_GetMem > VIDMEM_CLAMP_2GB)
					m_GetMem = VIDMEM_CLAMP_2GB;
			}
#endif
			if (m_GetMem == m_GetMemPrev)
			{
				m_CountConstMemoryLeft++;
				if (KeySet("showmem"))
					WriteToLog("%d: GetMemPrev=%u, GetMem=%u, UsedMem=%u.\n", index, m_GetMemPrev, m_GetMem, m_TotalUsedMem);
			}
			else if (m_GetMem >= m_GetMemPrev)
			{
				m_CountNegativeMemoryUsage++;
				if (KeySet("showmem"))
					WriteToLog("%d: GetMemPrev=%u, GetMem=%u, UsedMem=%u.\n", index, m_GetMemPrev, m_GetMem, m_TotalUsedMem);
			}
			else
				m_CountConstMemoryLeft = 0;
			m_GetMemPrev = m_GetMem;
			if ( (m_CountConstMemoryLeft >= 3 || m_CountNegativeMemoryUsage >= 2) && 
				index < m_EndIndex && index >= m_MinExhaustNum )
			{
				m_EndIndex = index;
				if (KeySet("showmem"))
					WriteToLog("%d: AvailableMem=%u, UsedMem=%u.\n", index, m_GetMem, m_TotalUsedMem);
				WriteToLog("Memory is exhausted at test %d\n", index);
			}
		}
		if ( m_IsOutOfMemory && index < m_EndIndex)
		{
			m_EndIndex = index;
			m_ExtraCount = 0;
			if (KeySet("showmem"))
				WriteToLog("%d: AvailableMem=%u, UsedMem=%u.\n", index, GetAvailableTextureMem(), m_TotalUsedMem);
			WriteToLog("Memory is exhausted at test %d\n", index);
			SkipTestRange(2*(m_EndIndex+ m_ExtraCount) - 1, NUMBER_OF_TESTS);
		}
		if ( index == m_EndIndex && !m_IsOutOfMemory)
		{
			m_ExtraCount = (DWORD)ceil(m_ResInfosList.size()/(m_PrimCount*10.f));
			SkipTestRange(2*(m_EndIndex+ m_ExtraCount) - 1, NUMBER_OF_TESTS);
		}

		if ( index == 1 || index == m_EndIndex + m_ExtraCount )
		{
			startTexture = m_TexturesList.end();
			startCubeMap = m_CubesList.end();
			startVolTex  = m_VolumesList.end();
			startVBuffer = m_VBuffersList.end();
			startIBuffer = m_IBuffersList.end();
			startResInfo = m_ResInfosList.end();
		}
		else
		{
			//need to move pointers to get to currently tested resources; 
			//also if we don't start with first test then preload all resources
			//which would be used if the test run from the very beginging
			for (int i = 0; i < m_PrimCount; i++)
			{
				if (startResInfo == m_ResInfosList.end())
					startResInfo = m_ResInfosList.begin();
				else startResInfo++;
				switch ( (*startResInfo)->Type )
				{
					case IS_16INDEXBUFFER: 
					case IS_32INDEXBUFFER:
					{
						if (startVBuffer == m_VBuffersList.end())
							startVBuffer = m_VBuffersList.begin();
						else startVBuffer++;
						if (index != iTest)
							(*startVBuffer)->PreLoad();
						if (startIBuffer == m_IBuffersList.end())
							startIBuffer = m_IBuffersList.begin();
						else startIBuffer++;
						if (index != iTest)
							(*startIBuffer)->PreLoad();
						break;
					}
					case IS_TLVERTEXBUFFER:
					case IS_LVERTEXBUFFER:
					case IS_VERTEXBUFFER:
					{
						if (startVBuffer == m_VBuffersList.end())
							startVBuffer = m_VBuffersList.begin();
						else startVBuffer++;
						if (index != iTest)
							(*startVBuffer)->PreLoad();
						break;
					}
					case IS_TEXTURE: 
					{
						if (startTexture == m_TexturesList.end())
							startTexture = m_TexturesList.begin();
						else startTexture++;
						if (index != iTest)
							(*startTexture)->PreLoad();
						if (startVBuffer == m_VBuffersList.end())
							startVBuffer = m_VBuffersList.begin();
						else startVBuffer++;
						if (index != iTest)
							(*startVBuffer)->PreLoad();
						break;
					}
					case IS_CUBEMAP:
					{
						if (startCubeMap == m_CubesList.end())
							startCubeMap = m_CubesList.begin();
						else startCubeMap++;
						if (index != iTest)
							(*startCubeMap)->PreLoad();
						if (startVBuffer == m_VBuffersList.end())
							startVBuffer = m_VBuffersList.begin();
						else startVBuffer++;
						if (index != iTest)
							(*startVBuffer)->PreLoad();
						break;
					}
					case IS_VOLUME:
					{
						if (startVolTex == m_VolumesList.end())
							startVolTex = m_VolumesList.begin();
						else startVolTex++;
						if (index != iTest)
							(*startVolTex)->PreLoad();
						if (startVBuffer == m_VBuffersList.end())
							startVBuffer = m_VBuffersList.begin();
						else startVBuffer++;
						if (index != iTest)
							(*startVBuffer)->PreLoad();
						break;
					}
				}
			}
		}		

		//adding resources
		if ( index < m_EndIndex + m_ExtraCount )
		{
			int result = 0;
			int count_fail = 0;
			int count_skip = 0;

			LONG rand_number = 0;
			DWORD temp = 0;

			for (int i = 0; i < m_PrimCount; i++)
			{
				if (m_input)
				{
					if ( fscanf( m_input, "%d", &rand_number) != 1 || 
						rand_number > IS_VOLUME || rand_number < IS_NULL )
						return false;
				}
				else if (m_testType == IS_NULL)
				{
					bool go = false;
					while (!go)
					{
						if (m_IsHWProcessing)
							rand_number = rand()%8 + 1;
						else
							rand_number = rand()%3 + 6;
						switch (rand_number)
						{
							case IS_32INDEXBUFFER:
								if (m_Use32BitIB)  go = true; break;
							case IS_TEXTURE:
								if (m_UseTextures) go = true; break;
							case IS_CUBEMAP: 
								if (m_UseCubeMaps) go = true; break;
							case IS_VOLUME:
								if (m_UseVolMaps)  go = true; break;
							default:
								go = true; break;
						}
					}
				}
				else
					rand_number = m_testType;
				if (m_output)
					fprintf(m_output, "%d ", rand_number);
				
				switch (rand_number)
				{
					case IS_16INDEXBUFFER: case IS_32INDEXBUFFER:
						result = AddIndexBufferToList(rand_number);
						break;
					case IS_TLVERTEXBUFFER:
						result = AddTLVertexBufferToList();
						break;
					case IS_LVERTEXBUFFER:
						result = AddLVertexBufferToList();
						break;
					case IS_VERTEXBUFFER:
						result = AddVertexBufferToList();
						break;
					case IS_TEXTURE: 
						result = AddTextureToList((index-1)*m_PrimCount+i);
						break;
					case IS_CUBEMAP:
						result = AddCubeMapToList((index-1)*m_PrimCount+i);
						break;
					case IS_VOLUME:
						result = AddVolTextureToList((index-1)*m_PrimCount+i);
						break;
				}

				m_TotalUsedMem += m_CurrentOccupiedMemory;

				if (index == iTest)
				{
					if ( result == FAIL )
						count_fail++;
					else if ( result == SKIP )
						count_skip++;
					temp += m_TotalNumberOfSlices;
				}
				else
				{
					msgString[0] = '\0';
					m_Message[0] = '\0';
				}
			}

			if (KeySet("showmem") && (!((index-1)%5) ||  count_skip ||  count_fail))
				WriteToLog("%3d: AvailableMem=%12u, UsedMem=%12u.\n", index, GetAvailableTextureMem(), m_TotalUsedMem);

			if (index == iTest)
			{
                m_TotalNumberOfSlices = temp;
				BeginTestCase(msgString, iTest);
			
				if ( count_skip == m_PrimCount )
					SkipTestRange( m_uTestNumber, m_uTestNumber);
				else if ( count_fail )
					Fail();
			}
		}
		//if we repeat using resources then we want to get description of resources if it's current test
		else if (index == iTest)
		{
			RESINFOLIST::iterator ri;
			VBUFFERLIST::iterator vb;
			IBUFFERLIST::iterator ib;
			TEXTURELIST::iterator tex;
			CUBEMAPLIST::iterator cm;
			VOLTEXLIST::iterator vol;
			ri = startResInfo;
			vb = startVBuffer;
			ib = startIBuffer;
			tex = startTexture;
			cm = startCubeMap;
			vol = startVolTex;

			m_TotalNumberOfSlices = 0;
			for (int i = 0; i < m_PrimCount; i++)
			{
				if (ri == m_ResInfosList.end())
					ri = m_ResInfosList.begin();
				else ri++;
				m_TotalNumberOfSlices += (*ri)->NumSlices;
				FmtToString(FORMAT_TEXTURE, &((*ri)->Format), szBuffer);
				switch ( (*ri)->Type )
				{
					case IS_16INDEXBUFFER: case IS_32INDEXBUFFER:
					{
						if (vb == m_VBuffersList.end())
							vb = m_VBuffersList.begin();
						else vb++;
						if (ib == m_IBuffersList.end())
							ib = m_IBuffersList.begin();
						else ib++;
						int bits = ((*ri)->Format).d3dfFormat == FMT_INDEX16 ? 16 : 32;
						int size = 8 * (*ib ? (*ib)->GetSize() : 0);
						sprintf(msgString, "%s %d IB %d, ", msgString, bits, size/bits);
						sprintf(m_Message, "%s %d IB %d\n", m_Message, bits, size/bits);
						break;
					}
					case IS_TLVERTEXBUFFER:
					{
						if (vb == m_VBuffersList.end())
							vb = m_VBuffersList.begin();
						else vb++;
						int size = *vb ? (*vb)->GetSize() : 0;
						sprintf(msgString, "%sTLVB %d, ", msgString, size/sizeof(D3DTLVERTEX));
						sprintf(m_Message, "%sTLVB %d\n", m_Message, size/sizeof(D3DTLVERTEX));
						break;
					}
					case IS_LVERTEXBUFFER:
					{
						if (vb == m_VBuffersList.end())
							vb = m_VBuffersList.begin();
						else vb++;
						int size = *vb ? (*vb)->GetSize() : 0;
						sprintf(msgString, "%sLVB %d, ", msgString, size/sizeof(LVERTEX));
						sprintf(m_Message, "%sLVB %d\n", m_Message, size/sizeof(LVERTEX));
						break;
					}
					case IS_VERTEXBUFFER:
					{
						if (vb == m_VBuffersList.end())
							vb = m_VBuffersList.begin();
						else vb++;
						int size = *vb ? (*vb)->GetSize() : 0;
						sprintf(msgString, "%sVB %d, ", msgString, size/sizeof(VERTEX));
						sprintf(m_Message, "%sVB %d\n", m_Message, size/sizeof(VERTEX));
						break;
					}
					case IS_TEXTURE: 
					{
						sprintf(msgString, "%sT %s ", msgString, szBuffer);
						sprintf(m_Message, "%sT %s ", m_Message, szBuffer);
						if (tex == m_TexturesList.end())
							tex = m_TexturesList.begin();
						else tex++;
						if (vb == m_VBuffersList.end())
							vb = m_VBuffersList.begin();
						else vb++;
						if (*tex)
						{
							SURFACEDESC Desc;
							(*tex)->GetLevelDesc(0, &Desc);
							sprintf(msgString, "%s%dx%d %d, ", msgString, Desc.Width, Desc.Height, (*ri)->NumSlices);
							sprintf(m_Message, "%s%dx%d %d\n", m_Message, Desc.Width, Desc.Height, (*ri)->NumSlices);
						}
						else
						{
							sprintf(msgString, "%s%d, ", msgString, (*ri)->NumSlices);
							sprintf(m_Message, "%s%d\n", m_Message, (*ri)->NumSlices);
						}
						break;
					}
					case IS_CUBEMAP:
					{
						sprintf(msgString, "%sCM %s ", msgString, szBuffer);
						sprintf(m_Message, "%sCM %s ", m_Message, szBuffer);
						if (cm == m_CubesList.end())
							cm = m_CubesList.begin();
						else cm++;
						if (vb == m_VBuffersList.end())
							vb = m_VBuffersList.begin();
						else vb++;
						if (*cm)
						{
							SURFACEDESC Desc;
							(*cm)->GetLevelDesc(0, &Desc);
							sprintf(msgString, "%s%d %d, ", msgString, Desc.Width, (*ri)->NumSlices/6);
							sprintf(m_Message, "%s%d %d\n", m_Message, Desc.Width,  (*ri)->NumSlices/6);
						}
						else
						{
							sprintf(msgString, "%s%d, ", msgString, (*ri)->NumSlices);
							sprintf(m_Message, "%s%d\n", m_Message, (*ri)->NumSlices);
						}
						break;
					}
					case IS_VOLUME:
					{
						sprintf(msgString, "%sVOL %s ", msgString, szBuffer);
						sprintf(m_Message, "%sVOL %s ", m_Message, szBuffer);
						if (vol == m_VolumesList.end())
							vol = m_VolumesList.begin();
						else vol++;
						if (vb == m_VBuffersList.end())
							vb = m_VBuffersList.begin();
						else vb++;
						if (*vol)
						{
							VOLUME_DESC Desc;
							(*vol)->GetLevelDesc(0, &Desc);
							sprintf(msgString, "%s%dx%dx%d %d, ", msgString, Desc.Width, Desc.Height, Desc.Depth, (*vol)->GetLevelCount());
							sprintf(m_Message, "%s%dx%dx%d %d\n", m_Message, Desc.Width, Desc.Height, Desc.Depth, (*vol)->GetLevelCount());
						}
						else
						{
							sprintf(msgString, "%s%d, ", msgString, (*ri)->NumSlices);
							sprintf(m_Message, "%s%d\n", m_Message, (*ri)->NumSlices);
						}
						break;
					}
					case IS_NULL:
					{
						sprintf(msgString, "%sNULL, ", msgString);
						sprintf(m_Message, "%sNULL\n", m_Message);
					}
				}
			}

			BeginTestCase(msgString, iTest);
		}
	}	
	
	//BeginTestCase(msgString, iTest);
	
    hr = m_pSrcDevice->BeginScene();
	if (FAILED(hr))
	{
		WriteToLog("BeginScene() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	
    hr = DrawFunction(true);
    if (D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
	{
		m_pSrcDevice->EndScene();
        SkipTestRange(iTest, NUMBER_OF_TESTS);
        return false;
	}
		
	hr = m_pSrcDevice->EndScene();
	if (FAILED(hr))
	{
		WriteToLog("EndScene() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr) ,hr);
		Fail();
		return false;
	}
	
	return true;
}

//-----------------------------------------------------------------------------

bool CLRUTest::ProcessFrame(void)
{
	bool result = true;
	
	result = CheckListColors(true);

	if (result)
		Pass();
	else
		Fail();

	return result;
}

//-----------------------------------------------------------------------------

bool CLRUTest::TestTerminate(void)
{
	Cleanup();

	if (m_input)
		fclose( m_input);
	if (m_output)
	{
		int i = fseek( m_output, 0L, SEEK_SET );
		fprintf(m_output, "%d", m_EndIndex);
		fclose(m_output);
	}
	if (memfile)	fclose(memfile);

	DestroyAllResInfosFromList();
	DestroyAllVertexBuffersFromList();
	DestroyAllIndexBuffersFromList();
	DestroyAllTexturesFromList();
	DestroyAllCubeMapsFromList();
	DestroyAllVolTexturesFromList();

	return true;
}

//-----------------------------------------------------------------------------

bool CLRUTest::Setup()
{
	ResMan::Setup();

	if (!KeySet("nofillmem"))
		m_TotalUsedMem = FillVideoMemory();
	else
		m_TotalUsedMem = 0;

	return true;
}

//-----------------------------------------------------------------------------

void CLRUTest::Cleanup()
{
	ResMan::Cleanup();

	DestroyAllDefaultTexList();
}

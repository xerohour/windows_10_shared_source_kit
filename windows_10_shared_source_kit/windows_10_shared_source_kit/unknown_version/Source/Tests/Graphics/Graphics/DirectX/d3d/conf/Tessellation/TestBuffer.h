#pragma once

#pragma warning( disable : 4533 )
template<typename Ty>
class TestBuffer{
public:
	TestBuffer( ID3D11Device* device, ID3D11DeviceContext* context, 
		ID3D11Buffer* srcBuffer, size_t srcSize, tstring topology ) :
		m_Topology(topology), m_Error( "" )
	{
		ID3D11Buffer* pTempBuffer = NULL;
		HRESULT hr;
		
		// create buffer on device
		D3D11_BUFFER_DESC   bufDesc = {0};

		srcBuffer->GetDesc(&bufDesc);
		bufDesc.BindFlags = 0;
		bufDesc.Usage = D3D11_USAGE_STAGING;
		bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		if ( FAILED( hr = device->CreateBuffer( &bufDesc, NULL, &pTempBuffer ) ) )
		{
			m_Error = _T("CreateBuffer failed, hr = ") + D3DHResultToString(hr);
			goto Exit;
		}

		// copy resource into buffer
		context->CopyResource(pTempBuffer, srcBuffer);

		// map tha buffer into processor memory space
		D3D11_MAPPED_SUBRESOURCE  mappedRes = {0};

		if ( FAILED( hr = context->Map( pTempBuffer, 0, D3D11_MAP_READ, 0, &mappedRes ) ) )
		{
			m_Error = _T("Map failed, hr = ") + D3DHResultToString(hr);
			goto Exit;
		}

		Ty* pData = (Ty*)mappedRes.pData;

		// copy data 
		for ( size_t k = 0; k < srcSize; ++k )
		{
			m_Contents.push_back( pData[k] );
		}
		
		// unmap (unlock) device buffer
		context->Unmap(pTempBuffer, 0);

	Exit:
		// release device buffer
		SAFE_RELEASE(pTempBuffer);
	}    

	//---------------------------------------------------------------------------------------    
	~TestBuffer()
	{
	}

	//---------------------------------------------------------------------------------------    
	bool HasError()
	{
		return ( m_Error != "" );
	}   
	
	//---------------------------------------------------------------------------------------    
	const tstring ErrorString()
	{
		return m_Error;
	}
		 
	//---------------------------------------------------------------------------------------    
	void StandardizeVertexSequence()
	{
		bool IsTriangle = ( m_Topology == "triangle_cw" || m_Topology == "triangle_ccw" );
		bool IsLine = ( m_Topology == "line" );
		
		for ( size_t k = 0; k < m_Contents.size(); ++k )
		{
			if (IsTriangle)
			{
				TestBuffer::StandardizeVertexSequence( (PTRI)(&m_Contents[k]) );
			}
			else
			if (IsLine)
			{
				TestBuffer::StandardizeVertexSequence( (PLINE)(&m_Contents[k]) );
			}
		}
	}
	
	//---------------------------------------------------------------------------------------    
	void SortVertices()
	{
		std::sort(m_Contents.begin(), m_Contents.end());
	}
	
	//---------------------------------------------------------------------------------------    
	TEST_RESULT Compare(const TestBuffer* rhs)
	{
		size_t sizeSrc = m_Contents.size();
		size_t sizeREF = rhs->m_Contents.size();
		// compare two buffer sizes
		if ( sizeSrc == sizeREF )
		{
			// now compare the units
			for ( size_t i = 0; i < m_Contents.size(); ++i )
			{
				Ty  val0 = m_Contents[i];
				Ty  val1 = rhs->m_Contents[i];

				if ( memcmp( &val0, &val1, sizeof(val0) ) )
				{
					std::ostringstream oStr;
					oStr << "Buffer mismatch at index=" << i;
					m_Error = oStr.str();
					
					return RESULT_FAIL;
				}
			}
			
		}
		else
		{
			m_Error = FormatString(_T("Buffer sizes do not match (REF: %i, Source: %i)"), sizeREF, sizeSrc );
			return RESULT_FAIL;
		}
		
		return RESULT_PASS;
	}

	//---------------------------------------------------------------------------------------    
	const Ty* Contents()
	{
		return &(m_Contents[0]);
	}

private:
	// non copyable
	TestBuffer(const TestBuffer&){}
	TestBuffer& operator=(const TestBuffer&){}

	//---------------------------------------------------------------------------------------    
	static void  StandardizeVertexSequence( PTRI pTri )
	{
		VERT    vertTemp;
		int     nLowestRaw;

		// Find first vertex with lowest raw bit value, which is not the same as its magnitude in float.
		// The vertices are wound in clockwise sequence, i.e. ascending index value.
		// So proceed in reverse, starting with highest index value. 
		vertTemp = pTri->vert[2];
		nLowestRaw = 2;

		if ( memcmp( &vertTemp, &pTri->vert[1], sizeof(vertTemp) ) >= 0 )
		{
			vertTemp = pTri->vert[1];
			nLowestRaw = 1;
		}

		if ( memcmp( &vertTemp, &pTri->vert[0], sizeof(vertTemp) ) >= 0 )
		{
			vertTemp = pTri->vert[0];
			nLowestRaw = 0;
		}

		if ( memcmp( &vertTemp, &pTri->vert[2], sizeof(vertTemp) ) >= 0 
			&& memcmp( &pTri->vert[1], &pTri->vert[2], sizeof(vertTemp) ) > 0 )
		{
			vertTemp = pTri->vert[2];
			nLowestRaw = 2;
		}

		// While maintaining the winding sequence, move the vertex with the lowest raw bit value
		// to position 0 in the list.
		if ( nLowestRaw == 1 )
		{
			vertTemp = pTri->vert[0];
			pTri->vert[0] = pTri->vert[1];
			pTri->vert[1] = pTri->vert[2];
			pTri->vert[2] = vertTemp;
		}
		else
		if ( nLowestRaw == 2 )
		{
			vertTemp = pTri->vert[2];
			pTri->vert[2] = pTri->vert[1];
			pTri->vert[1] = pTri->vert[0];
			pTri->vert[0] = vertTemp;
		}
		
		return;
	}

	//---------------------------------------------------------------------------------------    
	static void  StandardizeVertexSequence( PLINE pLine )
	{
		VERT    vertTemp;
		int     nLowestRaw;

		// Find vertex with lowest raw bit value, which is not the same as its magnitude in float.
		nLowestRaw = 0;

		if ( memcmp( &pLine->vert[0], &pLine->vert[1], sizeof(pLine->vert[0]) ) > 0 )
		{
			nLowestRaw = 1;
		}

		// Move the vertex with the lowest raw bit value to position 0.
		if ( nLowestRaw == 1 )
		{
			vertTemp = pLine->vert[0];
			pLine->vert[0] = pLine->vert[1];
			pLine->vert[1] = vertTemp;
		}
		
		return;
	}
	
	// owns the Buffer
	tstring m_Topology;
	std::vector<Ty> m_Contents;
	tstring m_Error;
};

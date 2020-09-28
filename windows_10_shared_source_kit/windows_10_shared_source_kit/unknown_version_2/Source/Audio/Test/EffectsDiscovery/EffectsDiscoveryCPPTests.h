WindowsTest::gFCConflictingParameters );
			return false;
		}

		if ( !CFormatHelper::ConvertPixels(
			CONV_TO_FORMAT, GetRTViewFormat(m_Format), pScratch, height, width, depth, pYUVPlanarReformattedData ) )
		{
			LogError( __FILEW__, __LINE__, L"CFormatHelper::ConvertPixels() failed", false, ERRORCODE_TYPE_BOOL, WindowsTest::gFCConflictingParameters );
			return false;
		}

		// Convert the expected data to the view format

		// Now the data in pYUVPlanarReformattedData is tightly packed
		pResultData = pYUVPlanarReformattedData;
		uRowPitch = width * size;
		uDepthPitch = height * uRowPitch;
		delete [] pScratch;
	}

	bool bPass = true;
	bool bLogAll = GetFramework( )->LogVerbose( );
	for( UINT y = 0; y < height; ++y )
	{
		for( UINT x = 0; x < width; ++x )
		{	
			for( UINT iB = 0; iB < bytesToCheck; ++iB )
			{
				BYTE resultExpected = pExpected[ iB ];
	
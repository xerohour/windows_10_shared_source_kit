// no once-only bracket macros around this header -- this is special for testhelpers.h
// specifically, this is to enable one codebase for both unicode and ansi versions

#ifdef UNICODEPARSER
#define _ST( str )	L##str
#define ISALPHA		iswalpha
#define ISDIGIT		iswdigit
#define	ISSPACE		iswspace
#define STRTOL		wcstol
#define STRTOD		wcstod
#define SPRINTF		swprintf
#define STRLEN		wcslen
#define STRNCPY		wcsncpy
#define ODS			OutputDebugStringW
#define _Parser		_ParserW
#define chartype	wchar_t
#define unsigned_chartype wchar_t
#else
#define _ST( str )	str
#define ISALPHA		isalpha
#define ISDIGIT		isdigit
#define	ISSPACE		isspace
#define STRTOL		strtol
#define STRTOD		strtod
#define SPRINTF		sprintf
#define STRLEN		strlen
#define STRNCPY		strncpy
#define ODS			OutputDebugStringA
#define _Parser		_ParserA
#define chartype	char
#define unsigned_chartype unsigned char
#endif


class _Parser
{
	typedef std::basic_string < chartype > STRING;

protected:

	String			m_sName;
	chartype *		m_sScript;
	DWORD			m_dwScriptSize;
	bool			m_bFromFile;
	chartype *		m_psPtr;
	DWORD			m_dwLine;

	std::vector < std::basic_string < chartype > >
					m_v_sKeywords;
	std::vector < DWORD >
					m_v_dwTokens;

public:
	_Parser() :
		m_sScript( NULL ),
		m_psPtr( NULL )
		{}
	HRESULT Setup( chartype * sScript, DWORD dwSize )
	{
		m_sScript = sScript;
		m_dwScriptSize = dwSize/sizeof(m_sScript[0]) - 1;

		m_psPtr = m_sScript;

		m_dwLine = 1;

		m_v_sKeywords.clear();
		m_v_dwTokens.clear();

		return S_OK;
	}

	HRESULT SetupFromResource( LPCTSTR lpName )
	{
		m_sName = lpName;

		// find the resource pointer
		HRSRC h = FindResource( GetModuleHandle( NULL ), lpName, RT_RCDATA );
		if( h == NULL )
		{
			int iErr = GetLastError();
			return Error( _ST("Invalid resource name") );
		}

		DWORD dwSize = SizeofResource( NULL, h );

		HGLOBAL hg = LoadResource( NULL, h );
		if( hg == NULL )
			return Error( _ST("LoadResource() error") );

		BYTE * pData = (BYTE *) LockResource( hg );
		if( pData == NULL )
			return Error( _ST("Lockesource() error") );

		m_bFromFile = false;

		char * pDataBuf = new char[ dwSize + 2 ];
		if( pData == NULL )
			return Error( _ST("Out of memory") );

		memcpy( pDataBuf, pData, dwSize );
		pDataBuf[ dwSize ] = 0;
		pDataBuf[ dwSize + 1 ] = 0;

		return Setup( (chartype *) pDataBuf, dwSize );
	}

	HRESULT SetupFromFile( LPCTSTR lpFile )
	{
		m_sName = lpFile;
		// TODO: convert lpFile to char *
		std::ifstream ifs;
		ifs.open( lpFile, std::ios::in | std::ios::binary );
		if( ifs.fail() )
			return Error( _ST("Unable to open script") );

		ifs.seekg( 0, std::ios::end );
		DWORD dwSize = (DWORD)(ifs.tellg());

		char * pData = new char[ dwSize + 2 ];
		if( pData == NULL )
			return Error( _ST("Out of memory") );

		ifs.seekg( 0, std::ios::beg );
		ifs.read( pData, dwSize );
		pData[ dwSize ] = 0;
		pData[ dwSize + 1 ] = 0;

		m_bFromFile = true;

		return Setup( (chartype *) pData, dwSize );
	}

	void Cleanup()
	{
		SAFEDELETEA( m_sScript );
	}

	HRESULT AddKeyword( STRING & sKeyword, DWORD dwToken )
	{
		std::vector < STRING > :: iterator itCur, itEnd = m_v_sKeywords.end();
		for( itCur = m_v_sKeywords.begin(); itCur != itEnd; ++ itCur )
		{
			if( * itCur == sKeyword )
			{
				chartype sErr[ 128 ];
				SPRINTF( sErr, _ST("Duplicate keyword: %s"), sKeyword.c_str() );
				return Error( sErr );
			}
		}

		m_v_sKeywords.push_back( sKeyword );
		m_v_dwTokens.push_back( dwToken );

		return S_OK;
	}

	HRESULT GetToken( STRING & sKeyword, DWORD & dwToken )
	{
		std::vector < STRING > :: iterator itCur, itEnd = m_v_sKeywords.end();
		std::vector < DWORD > :: iterator itCurT, itEndT = m_v_dwTokens.end();
		for( itCur = m_v_sKeywords.begin(), itCurT = m_v_dwTokens.begin(); itCur != itEnd; ++ itCur, ++ itCurT )
		{
			if( * itCur == sKeyword )
			{
				dwToken = * itCurT;
				return S_OK;
			}
		}

		dwToken = 0xffffffff;
		chartype sErr[ 128 ];
		SPRINTF( sErr, _ST("Unknown token: %s"), sKeyword.c_str() );
		return Error( sErr );
	}

	void Reset()
	{
		m_psPtr = m_sScript;
	}

	DWORD GetLineNumber()
	{
		return m_dwLine;
	}

	HRESULT Error( chartype * sError )
	{
		chartype sErrorOut[ 256 ];
		SPRINTF( sErrorOut, _ST("Error: line %d: %s\n"), m_dwLine, sError );
		ODS( sErrorOut );
		return E_FAIL;
	}

	void KillWS()
	{
		while( ISSPACE( (unsigned_chartype) * m_psPtr ) || * m_psPtr == _ST('#') )
		{
			while( ISSPACE( (unsigned_chartype) * m_psPtr ) )
			{
				if( * m_psPtr == _ST('\n') )
					++ m_dwLine;

				++ m_psPtr;
			}

			if( * m_psPtr == _ST('#') )
			{
				while( * m_psPtr != _ST('\n') && * m_psPtr != _ST('\0') )
					++ m_psPtr;

				++ m_dwLine;

				if( * m_psPtr != _ST('\0') )
					++ m_psPtr;
			}
		}
	}

	void KillLeadingWS()
	{
		while( ISSPACE( (unsigned_chartype) * m_psPtr ) && * m_psPtr != _ST('\n') )
			++ m_psPtr;
	}

	HRESULT AdvancePtr( int iNum )
	{
		if( IsEof() )
			return S_OK;

		m_psPtr += iNum;
		if( m_psPtr < m_sScript )
		{
			m_psPtr = m_sScript;
			return Error( _ST("AdvancePtr: Out of range") );
		}

		if( m_psPtr > m_sScript + m_dwScriptSize - 1 )
		{
			m_psPtr = m_sScript + m_dwScriptSize - 1;
			return Error( _ST("AdvancePtr: Out of range") );
		}

		return S_OK;
	}

	HRESULT ReadChar( chartype & cChar )
	{
		KillWS();

		if( IsEof() )
			return S_OK;

		cChar = * m_psPtr ++;
		return S_OK;
	}

	HRESULT MatchChar( chartype cChar )
	{
		KillWS();

		if( IsEof() )
			return S_OK;

		if( * m_psPtr != cChar )
		{
			chartype sErr[ 128 ];
			SPRINTF( sErr, _ST("MatchChar: expected '%c' starting at "), cChar );
			DWORD dw = STRLEN( sErr );
			STRNCPY( sErr + dw, m_psPtr, 128 - dw - 1 );
			sErr[ 128 - dw - 1 ] = _ST( '\0' );
			return Error( sErr );
		}

		++ m_psPtr;
		return S_OK;
	}

	HRESULT UnreadChar()
	{
		-- m_psPtr;
		return S_OK;
	}

	HRESULT ReadName( STRING & sName )
	{
		KillWS();

		if( IsEof() )
			return S_OK;

		sName.erase();

		if( ISALPHA( (unsigned_chartype) * m_psPtr ) || * m_psPtr == _ST('_') || * m_psPtr == _ST('@') )
		{
			while( ISALPHA( (unsigned_chartype) * m_psPtr ) || ISDIGIT( (unsigned_chartype) * m_psPtr ) || * m_psPtr == _ST('_') || * m_psPtr == _ST('@') )
				sName.append( 1, * m_psPtr ++ );
		}
		else
		{
			chartype sErr[ 128 ];
			SPRINTF( sErr, _ST("ReadName: token starting at %.8s is not a name"), m_psPtr );
			return Error( sErr );
		}

		return S_OK;
	}

	HRESULT ReadFilename( STRING & sLine )
	{
		KillWS();

		if( IsEof() )
			return S_OK;

		sLine.erase();

		chartype * psPtr = m_psPtr;
		while( ISALPHA( (unsigned_chartype) * psPtr ) || ISDIGIT( (unsigned_chartype) * psPtr ) || * psPtr == _ST('_') || * psPtr == _ST('-') || * psPtr == _ST('\\') || * psPtr == _ST('.') )
			sLine.append( 1, * psPtr ++ );

		if( m_psPtr == psPtr )
		{
			chartype sErr[ 128 ];
			SPRINTF( sErr, _ST("ReadFilename: token starting at %.8s is not a filename"), m_psPtr );
			return Error( sErr );
		}

		m_psPtr = psPtr;
		return S_OK;
	}

	HRESULT ReadLine( STRING & sLine )
	{
		if( IsEof() )
			return S_OK;

		sLine.erase();

		while( ( * m_psPtr != _ST('#') ) &&
			   ( * m_psPtr != _ST('\r') ) &&
			   ( * m_psPtr != _ST('\n') ) &&
			   ( ! IsEof() ) )
			sLine.append( 1, * m_psPtr ++ );

		return S_OK;
	}

	HRESULT ReadFile( STRING & sFile )
	{
		if( IsEof() )
			return S_OK;

		sFile.erase();

		while( ! IsEof() )
			sFile.append( 1, * m_psPtr ++ );

		return S_OK;
	}

	HRESULT ReadInt( DWORD & dwInt )
	{
		KillWS();

		if( IsEof() )
			return S_OK;

		chartype * psPtr;
		dwInt = STRTOL( m_psPtr, & psPtr, 10 );
		if( psPtr == m_psPtr )
		{
			chartype sErr[ 128 ];
			SPRINTF( sErr, _ST("ReadInt: token starting at %.8s is not an int"), m_psPtr );
			return Error( sErr );
		}

		m_psPtr = psPtr;

		return S_OK;
	}

	HRESULT ReadHexInt( DWORD & dwInt )
	{
		KillWS();

		if( IsEof() )
			return S_OK;

		chartype * psPtr = m_psPtr;
		dwInt = 0;
		while( 1 )
		{
			if( ISDIGIT( (unsigned_chartype) * psPtr ) )
			{
				dwInt <<= 4;
				dwInt += * psPtr - _ST('0');
			}
			else if( * psPtr >= _ST('a') && * psPtr <= _ST('f') )
			{
				dwInt <<= 4;
				dwInt += * psPtr - _ST('a') + 10;
			}
			else if( * psPtr >= _ST('A') && * psPtr <= _ST('F') )
			{
				dwInt <<= 4;
				dwInt += * psPtr - _ST('A') + 10;
			}
			else break;

			++ psPtr;
		}
		if( psPtr == m_psPtr )
		{
			chartype sErr[ 128 ];
			SPRINTF( sErr, _ST("ReadHexInt: token starting at %.8s is not a hex int"), m_psPtr );
			return Error( sErr );
		}

		m_psPtr = psPtr;

		return S_OK;
	}

	HRESULT ReadFloat( float & fFloat )
	{
		KillWS();

		if( IsEof() )
			return S_OK;

		chartype * psPtr;
		fFloat = (float)STRTOD( m_psPtr, & psPtr );
		if( psPtr == m_psPtr )
		{
			chartype sErr[ 128 ];
			SPRINTF( sErr, _ST("ReadFloat: token starting at %.8s is not a float"), m_psPtr );
			return Error( sErr );
		}

		m_psPtr = psPtr;

		return S_OK;
	}

	// composite parsing functions -- just handy stuff
	HRESULT ReadVector2( D3DXVECTOR2 & vVect )
	{
		if( FAILED( MatchChar( _ST('(') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( vVect.x ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( vVect.y ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(')') ) ) ) return E_FAIL;
		return S_OK;
	}

	HRESULT ReadVector3( D3DXVECTOR3 & vVect )
	{
		if( FAILED( MatchChar( _ST('(') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( vVect.x ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( vVect.y ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( vVect.z ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(')') ) ) ) return E_FAIL;
		return S_OK;
	}

	HRESULT ReadVector4( D3DXVECTOR4 & vVect )
	{
		if( FAILED( MatchChar( _ST('(') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( vVect.x ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( vVect.y ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( vVect.z ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( vVect.w ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(')') ) ) ) return E_FAIL;
		return S_OK;
	}

	HRESULT ReadMatrix( D3DXMATRIX & mMatrix )
	{
		if( FAILED( MatchChar( _ST('(') ) ) ) return E_FAIL;

		if( FAILED( ReadFloat( mMatrix._11 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( mMatrix._12 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( mMatrix._13 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( mMatrix._14 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;

		if( FAILED( ReadFloat( mMatrix._21 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( mMatrix._22 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( mMatrix._23 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( mMatrix._24 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;

		if( FAILED( ReadFloat( mMatrix._31 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( mMatrix._32 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( mMatrix._33 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( mMatrix._34 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;

		if( FAILED( ReadFloat( mMatrix._41 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( mMatrix._42 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( mMatrix._43 ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadFloat( mMatrix._44 ) ) ) return E_FAIL;

		if( FAILED( MatchChar( _ST(')') ) ) ) return E_FAIL;
		return S_OK;
	}

	HRESULT ReadRect( RECT & rc )
	{
		if( FAILED( ReadInt( (DWORD &) rc.left ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadInt( (DWORD &) rc.top ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadInt( (DWORD &) rc.right ) ) ) return E_FAIL;
		if( FAILED( MatchChar( _ST(',') ) ) ) return E_FAIL;
		if( FAILED( ReadInt( (DWORD &) rc.bottom ) ) ) return E_FAIL;
		return S_OK;
	}

	HRESULT ReadAndChoose( DWORD dwTokTest0, DWORD dwTokTest1, DWORD & dwToken )
	{
		STRING sLine;
		if( FAILED( ReadName( sLine ) ) ) return E_FAIL;
		if( FAILED( GetToken( sLine, dwToken ) ) ) return E_FAIL;
		if( dwToken != dwTokTest0 && dwToken != dwTokTest1 )
		{
			dwToken = 0xffffffff;
			chartype sErr[ 128 ];
			SPRINTF( sErr, _ST("ReadAndChoose: unexpected token starting at %.8s"), m_psPtr );
			return Error( sErr );
			return E_FAIL;
		}
		return S_OK;
	}

	bool IsEof()
	{
		if( (DWORD)(m_psPtr - m_sScript) >= m_dwScriptSize )
			return true;
		else
			return false;
	}

	void SetEof()
	{
		// will cause IsEof() to return true
		m_psPtr = m_sScript + m_dwScriptSize;
	}
};

#undef _ST
#undef ISALPHA
#undef ISDIGIT
#undef ISSPACE
#undef STRTOL
#undef STRTOD
#undef SPRINTF
#undef STRLEN
#undef STRNCPY
#undef ODS
#undef _Parser
#undef chartype
#undef unsigned_chartype
//////////////////////////////////////////////////////////////////////////
//  TestGroupManager.cpp
//  created:	2004/09/20
//
//  purpose: Defines the CTestGroupManager class
//////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "Test.h"
#include "TestGroupManager.h"
#include "TestClassFactory.h"
#include "Framework.h"

//////////////////////////////////////////////////////////////////////////
// CTestGroupPath
//////////////////////////////////////////////////////////////////////////
size_t CTestGroupPath::nEnd = (size_t) 0 - 1;

CTestGroupPath::CTestGroupPath( const tstring &strPath )
{
	SetPath( strPath );
}

CTestGroupPath::CTestGroupPath( const TCHAR *szPath )
{
	SetPath( szPath );
}

CTestGroupPath::CTestGroupPath( const CTestGroupPath &Path, size_t nFirst /* = 0 */, size_t nLast /* = nEnd  */ )
{
	for( size_t nIndex = nFirst; nIndex < nLast && nIndex < Path.Depth(); ++nIndex )
	{
		m_PathComponents.push_back( Path.GetPathComponent( nIndex ) );
	}
}

bool CTestGroupPath::IsValidPathComponent( const tstring &strName ) const
{
	const tstring invalidChars( _T( " \\/:=," ) );
	if( strName.find_first_of( invalidChars ) != tstring::npos )
	{
		_DPF( "'%s' is not a valid test path component string\n", strName.c_str() );
		_DPF( "The following characters are not allowed in a path component string: \"%s\"\n", invalidChars.c_str() );
		return false;
	}
	
	return true;
}

void CTestGroupPath::SetPath( const tstring &strPath )
{
	size_t nStart = 0;
	
	if( strPath.empty() )
		return;
	
	for( ;; )
	{
		size_t nEnd = strPath.find_first_of( _T( "/\\" ), nStart );
		tstring strName = strPath.substr( nStart, nEnd == tstring::npos ? tstring::npos : nEnd - nStart );
		
		if( nStart > 0 || nEnd > 0 ) // Ignore leading '\'
		{
			assert( IsValidPathComponent( strName ) );
			m_PathComponents.push_back( strName );
			
			if( nEnd == tstring::npos )
				break;
		}
					
		nStart = nEnd + 1;
	}
}

void CTestGroupPath::SetPath( const TCHAR *szPath )
{
	SetPath( tstring( szPath ) );
}

tstring CTestGroupPath::ToString( size_t nFirst, size_t nDepth ) const
{
	tstring strResult;
	
	if( Depth() == 0 )
	{
		return _T( "" ); // Root path should return an empty string.
	}

	size_t nLast = nDepth < nEnd ? nDepth + nFirst : Depth();
	
	size_t nIndex = nFirst;
	
	for( ; ; )
	{
		strResult += m_PathComponents[nIndex];
		++nIndex;
		if( nIndex == nLast )
			break;
			
		strResult += _T( '\\' );
	}
		
	return strResult;
}

CTestGroupPath CTestGroupPath::operator+( const CTestGroupPath &Path ) const
{
	CTestGroupPath NewPath( *this );
	NewPath += Path;
	return NewPath;
}

CTestGroupPath &CTestGroupPath::operator+=( const CTestGroupPath &Path )
{
	m_PathComponents.insert( m_PathComponents.end(), Path.m_PathComponents.begin(), Path.m_PathComponents.end() );
	return *this;
}


//////////////////////////////////////////////////////////////////////////
// CTestGroupBranchNode
//////////////////////////////////////////////////////////////////////////

CTestGroupBranchNode::~CTestGroupBranchNode()
{
	// Cleanup child nodes
	for( GroupArray::iterator it = m_GroupArray.begin(); it != m_GroupArray.end(); ++it )
	{
		delete( *it );
	}
}

bool CTestGroupBranchNode::AddGroupNode( CTestGroupNode *pNode )
{
	GroupIndexMap::iterator it = m_GroupMap.find( pNode->GetName() );
	
	if( it != m_GroupMap.end() )
		return false;
		
	m_GroupMap.insert( GroupIndexMap::value_type( pNode->GetName(), m_GroupArray.size() ) );
	m_GroupArray.push_back( pNode );
	
	pNode->m_pParent = this;
	pNode->m_path = GetPath() + CTestGroupPath( pNode->GetName() );
	
	return true;
}

CTestGroupNode *CTestGroupBranchNode::AddTestNode( const CTestGroupPath &Path, const tstring &strClass, const tstring &strDesc )
{
	CTestGroupNode *pNode = NULL;
	tstring strRoot = Path.GetPathComponent( 0 );
	
	if( Path.Depth() == 1 )
	{
		pNode = new CTestGroupLeafNode( strRoot, strClass, strDesc );
		AddGroupNode( pNode );
	}
	else
	{
		pNode = GetSubGroupNode( strRoot );
		
		if( pNode && pNode->IsTestNode() )
		{
			WriteError( _T( "Cannot convert branch node to test node: %s" ), pNode->GetPath().ToString().c_str() );
			return NULL;
		}
			
		CTestGroupBranchNode *pBranch = reinterpret_cast< CTestGroupBranchNode *>( pNode );
		
		if( pBranch == NULL )
		{
			pBranch = new CTestGroupBranchNode( strRoot );
			AddGroupNode( pBranch );
		}
		
		CTestGroupPath SubPath( Path, 1 );
		pNode = pBranch->AddTestNode( SubPath, strClass, strDesc );
	}
	
	if( pNode )
		m_nNumTests++;
	
	return pNode;
}

CTestGroupNode *CTestGroupBranchNode::GetSubGroupNode( const CTestGroupPath &Path )
{
	if( Path.Empty() )
		return this; // An empty path implies this.
		
	if( Path[0].empty() )
		return this; // Empty path returns this
		
	GroupIndexMap::iterator it = m_GroupMap.find( Path[0] );
	if( it == m_GroupMap.end() )
		return false;
		
	CTestGroupNode *pNode = m_GroupArray[it->second];
	assert( pNode && _T( "There should be no NULL group node pointers in the tree." ) );
	if( !pNode )
		return NULL;
	
	if( Path.Depth() > 1 )
		return pNode->GetSubGroupNode( CTestGroupPath( Path, 1 ) );
	
	return pNode;		
}

bool CTestGroupBranchNode::GetTestClass( const CTestGroupPath &Path, tstring &strClass ) const
{
	GroupIndexMap::const_iterator it = m_GroupMap.find( Path[0] );
	if( it == m_GroupMap.end() )
		return false;
		
	CTestGroupNode *pNode = m_GroupArray[it->second];
	assert( pNode && _T( "There should be no NULL group node pointers in the tree." ) );
	if( !pNode )
		return false;
	
	return pNode->GetTestClass(  CTestGroupPath( Path, 1 ), strClass );	
};

CTest *CTestGroupBranchNode::GetTest( const CTestGroupPath &Path, size_t SubPath )
{
	GroupIndexMap::iterator it = m_GroupMap.find( Path[SubPath] );
	if( it == m_GroupMap.end() )
		return false;

	CTestGroupNode *pNode = m_GroupArray[it->second];
	assert( pNode && _T( "There should be no NULL group node pointers in the tree." ) );
	if( !pNode )
		return false;

	return pNode->GetTest( Path, SubPath + 1 );	
}

void CTestGroupBranchNode::ClearTest( const CTestGroupPath &Path, size_t SubPath )
{
	if( Path.Empty() )
		return; // Only leaf nodes contain a test.

	GroupIndexMap::iterator it = m_GroupMap.find( Path[SubPath] );
	if( it == m_GroupMap.end() )
		return;

	CTestGroupNode *pNode = m_GroupArray[it->second];
	assert( pNode && _T( "There should be no NULL group node pointers in the tree." ) );
	if( !pNode )
		return;
	
	pNode->ClearTest( Path, SubPath + 1 );	
}

bool CTestGroupBranchNode::GetChildGroupNodeIndex( const tstring &strName, size_t *pnIndex ) const
{
	GroupIndexMap::const_iterator it = m_GroupMap.find( strName );
	if( it == m_GroupMap.end() )
		return false; // Child node doesn't exist.
		
	*pnIndex = it->second;
	return true;
}

//////////////////////////////////////////////////////////////////////////
// CTestGroupLeafNode
//////////////////////////////////////////////////////////////////////////
CTestGroupLeafNode::~CTestGroupLeafNode()
{
	delete( m_pTest );
	m_pTest = NULL;
}

CTest *CTestGroupLeafNode::GetTest()
{
    return m_pTest;
}

CTest *CTestGroupLeafNode::NewTest()
{
    assert(m_pTest == nullptr);

	if( !m_pTest )
	{
		GetFramework()->PreAllocateTest();
		m_pTest = GetFramework()->AllocateTest( m_strTestClass );

		if( m_pTest )
		{
			m_pTest->m_pNode = this;
		}
	}

	return m_pTest;
}

CTest *CTestGroupLeafNode::GetTest( const CTestGroupPath &Path, size_t SubPath )
{
	assert( Path.Depth() == SubPath );

	return GetTest();	
}

void CTestGroupLeafNode::ClearTest( const CTestGroupPath &Path, size_t SubPath )
{
	assert( Path.Depth() == SubPath );
	
	if( m_pTest )
	{
		delete( m_pTest );
		m_pTest = NULL;
	}
}


//////////////////////////////////////////////////////////////////////////
// CTestGroupManager
//////////////////////////////////////////////////////////////////////////
CTestGroupManager::CTestGroupManager() : 
	m_Root( _T( "" ) ),
	m_pEffectiveRoot( &m_Root ),
	m_bRegistrationLocked( false )
{
}

CTestGroupManager::~CTestGroupManager()
{
}

CTest *CTestGroupManager::AllocateTest( const tstring &strTestClass )
{
	return m_TestClassFactory.AllocateTest( strTestClass );
}

bool CTestGroupManager::RegisterTest( const CTestGroupPath &Path, const tstring &strClass, const tstring &strDesc, TestAllocatorProc pAllocator )
{
	m_TestClassFactory.RegisterClassByName( strClass, pAllocator );

	if( m_bRegistrationLocked )
	{
		WriteError( _T( "Test classes can only be registered in the application objects's InitTestGroups() method." ) );
		assert( !_T( "Test classes can only be registered in the application objects's InitTestGroups() method." ) );
		return false;
	}
	
	size_t nStart = 0;
	if( m_Root.GetSubGroupNode( Path ) != NULL )
	{
		WriteError( _T( "Duplicate test path: %s" ), Path.ToString().c_str() );
		return false;
	}
		
	if( m_Root.AddTestNode( Path, strClass, strDesc ) == NULL )
	{
		WriteError( _T( "Failure adding test group at path: %s" ), Path.ToString().c_str() );
		return false;
	}
	
	return true;
}

bool CTestGroupManager::GetGroupIndex( const CTestGroupPath &Path, size_t &nIndexOut )
{
	const CTestGroupNode *pNode = &m_Root;
	nIndexOut = 0;
	
	size_t depth = 0;
	
	for( ;; )
	{
		tcistring strComponent( Path.GetPathComponent( depth ) );
		
		for( size_t n = 0; n < pNode->GetNumChildGroupNodes(); ++n )
		{
			const CTestGroupNode *pChild = pNode->GetChildGroupNode( n );
			
			if( strComponent == pChild->GetName() )
			{
				depth++;

				if( depth == Path.Depth() )
					return true;
					
				pNode = pChild;
				break;
			}
			else
			{
				nIndexOut += pChild->GetNumTests();
			}
		}
	}
	
	return false;
}

bool CTestGroupManager::FindFirstTestGroup( CTestGroupPath &PathOut, const CTestGroupPath &PathIn )
{
	const CTestGroupNode *pNode = m_pEffectiveRoot->GetSubGroupNode( PathIn );
	if( !pNode )
		return false; // Path doesn't exist.
		
	// Walk down the left-hand side of the tree.
	PathOut = PathIn;
	for( ;; )
	{
		const CTestGroupNode *pChild = pNode->GetChildGroupNode( 0 );
		if( !pChild )
			break; // Finished
		
		PathOut += pChild->GetName();
		pNode = pChild;
	}
	
	if( !pNode->IsTestNode() )
		return false; // Make sure this is a leaf node.
	
	return true;
}

bool CTestGroupManager::FindNextTestGroup( CTestGroupPath &PathOut, const CTestGroupPath &PathIn )
{
	if( PathIn.Empty() )
		return false; // The root node has no siblings.

	// Get the parent node
	CTestGroupPath ParentPath( CTestGroupPath( PathIn, 0, PathIn.Depth() - 1 ) );
	const CTestGroupNode *pParent = m_pEffectiveRoot->GetSubGroupNode( ParentPath );
	assert( pParent && _T( "How can this node not have a parent?" ) );
	if( !pParent )
		return false;
	
	assert( !pParent->IsTestNode() && _T( "How is a parent node also a leaf node?" ) );
	if( pParent->IsTestNode() )
		return false;
	
	// Find my sibling.
	size_t nIndex;
	const tstring &strName = PathIn[PathIn.Depth() - 1];
	if( !pParent->GetChildGroupNodeIndex( strName, &nIndex ) )
	{
		assert( !_T( "How can my parent have no children?  Am I adopted?" ) );
		return false;
	}
	
	// Is this the last node in the group?
	if( nIndex >= pParent->GetNumChildGroupNodes() - 1 )
	{
		// Search on the parent path.
		if( !FindNextTestGroup( PathOut, ParentPath) )
			return false;
			
		return true;
	}
			
	// Get the next sibling node in the group.
	const CTestGroupNode *pNode = pParent->GetChildGroupNode( nIndex + 1 );
	assert( pNode && _T( "There should be no NULL node pointers in the test group tree." ) );
	if( !pNode )
		return false;

	CTestGroupPath Path( ParentPath + CTestGroupPath( pNode->GetName() ) );
	
	// Return the first group under the new path.
	return FindFirstTestGroup( PathOut, Path );
}


bool CTestGroupManager::FindLastTestGroup( CTestGroupPath &PathOut, const CTestGroupPath &PathIn )
{
	const CTestGroupNode *pNode = m_pEffectiveRoot->GetSubGroupNode( PathIn );
	if( !pNode )
		return false; // Path doesn't exist.
		
	// Walk down the right-hand side of the tree.
	PathOut = PathIn;
	for( ;; )
	{
		const CTestGroupNode *pChild = pNode->GetChildGroupNode( pNode->GetNumChildGroupNodes() - 1 );
		if( !pChild )
			break; // Finished
		
		PathOut += pChild->GetName();
		pNode = pChild;
	}
	
	if( !pNode->IsTestNode() )
	{
		assert( !_T( "Empty test group branch node" ) );
		return false; // There really should be no empty branch nodes.
	}
	
	return true;
}

bool CTestGroupManager::FindPrevTestGroup( CTestGroupPath &PathOut, const CTestGroupPath &PathIn )
{
	if( PathIn.Empty() )
		return false; // The root node has no siblings.

	// Get the parent node
	CTestGroupPath ParentPath( CTestGroupPath( PathIn, 0, PathIn.Depth() - 1 ) );
	CTestGroupNode *pParent = m_pEffectiveRoot->GetSubGroupNode( ParentPath );
	assert( pParent && _T( "How can this node not have a parent?" ) );
	if( !pParent )
		return false;
	
	assert( !pParent->IsTestNode() && _T( "How is a parent node also a leaf node?" ) );
	if( pParent->IsTestNode() )
		return false;
	
	// Find my sibling.
	size_t nIndex;
	const tstring &strName = PathIn[PathIn.Depth() - 1];
	if( !pParent->GetChildGroupNodeIndex( strName, &nIndex ) )
	{
		assert( !_T( "How can my parent have no children?  Am I adopted?" ) );
		return false;
	}
	
	// Is this the first node in the group?
	if( nIndex == 0 )
	{
		// Search on the parent path.
		if( !FindPrevTestGroup( PathOut, ParentPath ) )
			return false;
			
		return true;
	}
			
	// Get the previous sibling node in the group.
	const CTestGroupNode *pNode = pParent->GetChildGroupNode( nIndex - 1 );
	assert( pNode && _T( "There should be no NULL node pointers in the test group tree." ) );
	if( !pNode )
		return false;

	CTestGroupPath Path( ParentPath + CTestGroupPath( pNode->GetName() ) );
	
	// Return the last group under the new path.
	return FindLastTestGroup( PathOut, Path );
}

bool CTestGroupManager::SetEffectiveRoot( const CTestGroupPath &Path )
{
	CTestGroupNode *pNode = m_Root.GetSubGroupNode( Path );
	if( pNode == NULL )
		return false;
		
	m_pEffectiveRoot = pNode;
	return true;
}




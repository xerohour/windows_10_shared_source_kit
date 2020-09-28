//////////////////////////////////////////////////////////////////////////
//  TestGroupManager.h
//  created:	2004/09/20
//
//  purpose: Defines the CTestGroupManger class
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "fwtypes.h"
#include "TestClassFactory.h"

#define ADD_TEST( szPath, ClassName ) \
	GetFramework()->RegisterTest( szPath, _T( #ClassName ), _T(""), AllocTest< ClassName > )
	
#define ADD_TEST_AND_DESC( szPath, ClassName, szDesc ) \
	GetFramework()->RegisterTest( szPath, _T( #ClassName ), szDesc, AllocTest< ClassName > )
//
//////////////////////////////////////////////////////////////////////////
// CTestGroupPath
//////////////////////////////////////////////////////////////////////////
class CTestGroupPath
{
public:
	CTestGroupPath() {}
		// Default constructor.
		
	CTestGroupPath( const tstring &strPath );
		// Constructs a path object from a path string.
		
	CTestGroupPath( const TCHAR *szPath );
		// Constructs a path object from a path string.
		
	CTestGroupPath( const CTestGroupPath &Path, size_t nFirst = 0, size_t nLast = nEnd );
		// Copy constructor.
		
	//
	// Operator overloads.
	//
	CTestGroupPath operator+( const CTestGroupPath &Path ) const;
		// Creates a new CTestGroupPath object by combining this path
		// with the given path.
		
	CTestGroupPath &operator+=( const CTestGroupPath &Path );
		// Appends the specified path to this path.
		
	const tstring &operator[]( size_t nIndex ) const { return m_PathComponents[nIndex]; }
		// Returns the path component string at the given index.
		
	bool operator==( const CTestGroupPath &Path ) const { return m_PathComponents == Path.m_PathComponents; }
		
	//
	// Methods
	//
	void SetPath( const tstring &strPath );
		// Reconstructs the current path from the given path string.
		
	void SetPath( const TCHAR *szPath );
		// Reconstructs the current path from the given path string.
	
	bool Empty() const { return m_PathComponents.empty(); }
		// Return true of the path is empty.
		
	size_t Depth() const { return m_PathComponents.size(); }
		// Returns the depth of the path.
	
	tstring ToString( size_t nFirst = 0, size_t nDepth = nEnd ) const;
		// Constructs a path string from the path.
		
	tstring GetPathComponent( size_t nIndex ) const { return nIndex < Depth() ? tstring( m_PathComponents.at( nIndex ).c_str() ) : tstring( _T( "" ) ); }
		// Returns the path component string at the given index.
		
	static size_t nEnd;
	
private:
	bool IsValidPathComponent( const tstring &strName ) const;
		
private:
	typedef std::vector< tcistring > PathComponentsType;
	PathComponentsType m_PathComponents;
};


//////////////////////////////////////////////////////////////////////////
// CTestGroupNode
//////////////////////////////////////////////////////////////////////////
class CTestGroupNode
{
public:
	CTestGroupNode( const tstring &strName ) : m_strName( strName ), m_pParent( NULL ), m_path( _T( "" ) ) {}
	virtual ~CTestGroupNode() {};
	
	//
	// Methods
	//
	const tstring &GetName() const { return m_strName; }
		// Returns the name of the node.
		
	virtual bool GetTestClass( const CTestGroupPath &Path, tstring &strClass ) const = 0;
		// Returns the name of the class registered to specified path.
		
	virtual bool IsTestNode() const = 0;
		// Returns true if the node is a CTestGroupLeafNode
		// class object.
		
	virtual CTest *GetTest( const CTestGroupPath &Path, size_t SubPath ) = 0;
		// Returns a pointer to the test object associated
		// with the specified path, allocating it if necessary.
		
	virtual void ClearTest( const CTestGroupPath &Path, size_t SubPath ) = 0;
		// Clears any cached test object at the specified node
		
	virtual CTestGroupNode *AddTestNode( const CTestGroupPath &Path, const tstring &strClass, const tstring &strDesc ) = 0;
		// Adds a test group path to the branch (if this is a branch node)
		
	virtual CTestGroupNode *GetSubGroupNode( const CTestGroupPath &Path ) = 0;
		// Returns a pointer to the subgroup with the given path.
		// If the path is invalid or this is a leaf node then
		// the return value is false.
		
	virtual const CTestGroupNode *GetChildGroupNode( size_t nIndex ) const = 0;
		// Returns a pointer to the child group node at the specified
		// index.
		// Leaf nodes will always return NULL.

	virtual bool GetChildGroupNodeIndex( const tstring &strName, size_t *pnIndex ) const = 0;
		// Retrieves the index of the child node with the specified name.
		// Returns false if the child node doesn't exist.
		// Leaf nodes will always return false;
		
	virtual size_t GetNumChildGroupNodes() const = 0;
		// Returns the number of child group nodes.
		// Leaf nodes will always return 0;
		
	virtual size_t GetNumTests() const = 0;
		// Return the number of test nodes in the branch.
		
	const CTestGroupPath &GetPath() const { return m_path; }

private:
	tstring m_strName;
	CTestGroupNode *m_pParent;
	CTestGroupPath m_path;
	friend class CTestGroupBranchNode;
};


//////////////////////////////////////////////////////////////////////////
// CTestGroupLeafNode
//////////////////////////////////////////////////////////////////////////
class CTestGroupLeafNode : public CTestGroupNode
{
public:
	CTestGroupLeafNode( const tstring &strName, const tstring &strClass, const tstring &strDesc ) : 
		CTestGroupNode( strName ),
		m_strTestClass( strClass ),
		m_strTestDesc( strDesc ),
		m_pTest( NULL )	{}
		
	~CTestGroupLeafNode();
	
    bool IsActive() { return nullptr != m_pTest; }
	CTest *GetTest();
	CTest *NewTest();
		
	virtual bool GetTestClass( const CTestGroupPath &Path, tstring &strClass ) const
		// For leaf nodes the path should be empty.
		{
			if( Path.Depth() > 0 )
				return false;
			strClass = m_strTestClass;
			return true;
		}

	virtual const tstring &GetTestDesc() const { return m_strTestDesc; }
	
	virtual CTest *GetTest( const CTestGroupPath &Path, size_t SubPath );
		// Returns a pointer to the associated test, allocating
		// the test object if necessary.
		// NOTE: The Path parameter must be empty
		// since this is a leaf node that has no children.
	
	virtual void ClearTest( const CTestGroupPath &Path, size_t SubPath );
	// Clears any cached test object at the specified node

	//
	// Stub out branch-node-only methods.
	//
	virtual CTestGroupNode *GetSubGroupNode( const CTestGroupPath &Path ) { return Path.Empty() ? this : NULL; }
	virtual const CTestGroupNode *GetChildGroupNode( size_t nIndex ) const { return NULL; }
	virtual size_t GetNumChildGroupNodes() const { return 0; }
	virtual bool GetChildGroupNodeIndex( const tstring &strName, size_t *pnIndex ) const { return false; }
	virtual size_t GetNumTests() const { return 1; }
	virtual CTestGroupNode *AddTestNode( const CTestGroupPath &Path, const tstring &strClass, const tstring &strDesc ) { return NULL; }
	
protected:	
	virtual bool IsTestNode() const { return true; }
	
private:
	tstring m_strTestClass; // Registered class name of this node.
	tstring m_strTestDesc;		// Description of test node
	CTest *m_pTest;			// Pointer to the test instance (NULL until first call to GetTest()).
};


//////////////////////////////////////////////////////////////////////////
// CTestGroupBranchNode
//////////////////////////////////////////////////////////////////////////
class CTestGroupBranchNode : public CTestGroupNode
{
public:
	CTestGroupBranchNode( const tstring &strName ) : CTestGroupNode( strName ), m_nNumTests( 0 ) {}
	virtual ~CTestGroupBranchNode();
	
	//
	// Methods
	//
	const tstring &GetChildGroupName( size_t nIndex ) const 
		{ static tstring strError( _T( "" ) ); return  nIndex < m_GroupArray.size() ? m_GroupArray[nIndex]->GetName() : strError; }
		// Returns the name of the group node at the given index.
		// Returns an empty string if the index is invalid.
		
	size_t GetChildGroupNodeCount() const { return m_GroupArray.size(); }
		// Returns the number of subgroup nodes in the branch.
	
	CTestGroupNode *GetSubGroupNode( const CTestGroupPath &Path );
		// Returns a pointer to the group with the given path.
		// Returns NULL if the group doesn't exist.
		
	virtual bool GetTestClass( const CTestGroupPath &Path, tstring &strClass ) const;
		// Returns the name of the class registered to specified path.
		
	virtual CTest *GetTest( const CTestGroupPath &Path, size_t SubPath = 0 );

	virtual void ClearTest( const CTestGroupPath &Path, size_t SubPath = 0 );

	virtual const CTestGroupNode *GetChildGroupNode( size_t nIndex ) const { return nIndex < GetNumChildGroupNodes() ? m_GroupArray[nIndex] : NULL; }
		// Returns a pointer to the child group node at the specified
		// index.
		
	virtual bool GetChildGroupNodeIndex( const tstring &strName, size_t *pnIndex ) const;
		// Retrieves the index of the child node with the specified name.
		// Returns false if the child node doesn't exist.
		
	virtual size_t GetNumChildGroupNodes() const { return m_GroupArray.size(); }
		// Returns the number of child group nodes.

	virtual CTestGroupNode *AddTestNode( const CTestGroupPath &Path, const tstring &strClass, const tstring &strDesc );

	virtual size_t GetNumTests() const { return m_nNumTests; }

private:
	bool AddGroupNode( CTestGroupNode *pNode );
		// Adds a node to the branch.  If the name of the node
		// clashes with a node already in the branch node then
		// The attempt fails and AddGroupNode() returns false.
		
protected:	
	bool IsTestNode() const { return false; }
	
private:
	typedef std::map< tcistring, size_t> GroupIndexMap;
	typedef std::vector< CTestGroupNode *> GroupArray;
	size_t m_nNumTests;		// Number of tests below this branch.
	GroupIndexMap m_GroupMap;
	GroupArray m_GroupArray;
};



//////////////////////////////////////////////////////////////////////////
// CTestGroupManager
//////////////////////////////////////////////////////////////////////////
class CTestGroupManager
{
public:
	bool RegisterTest( const CTestGroupPath &Path, const tstring &strClass, const tstring &strDesc, TestAllocatorProc pAllocator );

	size_t GetNumTests() const { return m_Root.GetNumTests(); }
	
	bool FindFirstTestGroup( CTestGroupPath &PathOut, const CTestGroupPath &PathIn = CTestGroupPath() );
		// Finds the first leaf node in the execution order
		// within the branch addressed by PathIn
		// If successful PathOut contains the path to the found node.
		//
		// PARAMETERS:
		//	PathIn
		//		Provides the path to the branch to be searched.
		//
		//	PathOut
		//		Reference to the CTestGroupPath object that will be set
		//		to the path relative to the active branch to the leaf node representing
		//		the first leaf node in the execution order within the
		//		input branch.
		//	
		// RETURN VALUE:
		//	Returns true if a path was found.
		//
	
	bool FindLastTestGroup( CTestGroupPath &PathOut, const CTestGroupPath &PathIn = CTestGroupPath() );
		// Finds the last leaf node in the execution order
		// within the branch addressed by PathIn.  
		// If successful PathOut contains the path to the found node.
		//
		// PARAMETERS:
		//	PathIn
		//		Provides the path to the branch to be searched.
		//
		//	PathOut
		//		Reference to the CTestGroupPath object that will be set
		//		to the path relative to the active branch to the leaf node representing
		//		the last leaf node in the execution order within the
		//		input branch.
		//	
		// RETURN VALUE:
		//	Returns true if a path was found.
		//
	
	bool FindNextTestGroup( CTestGroupPath &PathOut, const CTestGroupPath &PathIn = CTestGroupPath() );
		// Find the group that immediately follows the specified group
		// in execution order.	
		//
		// PARAMETERS:
		//	PathIn
		//		Provides the path to the branch to be searched.
		//
		//	PathOut
		//		Contains the path relative to the active branch to the test group containing the
		//		next group in the execution order.
		//
		// RETURN VALUE:
		//	Returns false if this is the last group node in the
		//	execution order.
		//
		
		
	bool FindPrevTestGroup( CTestGroupPath &PathOut, const CTestGroupPath &PathIn = CTestGroupPath() );
		// Find the group that immediately precedes the specified group
		// in execution order.	
		//
		// PARAMETERS:
		//	PathIn
		//		Provides the path to the branch to be searched.
		//
		//	PathOut
		//		Contains the path relative to the active branch to the test group containing the
		//		previous group in the execution order.
		//
		// RETURN VALUE:
		//	Returns false if this is the first group node in the
		//	execution order.
		//
		
	CTest * GetTest( const CTestGroupPath &Path ) { return m_pEffectiveRoot->GetTest( Path, 0 ); }
		// Returns a pointer to the test at the given path.
		
	void ClearTest( const CTestGroupPath &Path ) { m_pEffectiveRoot->ClearTest( Path, 0 ); }
		// Clears the cached test object at the given path.
		
	bool SetEffectiveRoot( const CTestGroupPath &Path );
		// Sets the active branch to the specified path.
		// This affects the behavior Find*TestGroup()
		// and GetTest() methods by treating all path values
		// relative to the active branch path.
		//
		// If the call fails then the active branch is left unchanged
		// and the function returns false.
		//
		// The default active branch is the root of the test group tree.
		//
		// To reset the active branch to the root call this function
		// with an empty path ( CTestGroupPath( "" ) ).
		//
		// NOTE: This function will *not* affect the static RegisterTest()
		// function.
		
	CTestGroupPath GetEffectiveRoot() const { return m_pEffectiveRoot ? m_pEffectiveRoot->GetPath() : CTestGroupPath(); }
		// Returns the path to the effective root.
		
	bool GetGroupIndex( const CTestGroupPath &Path, size_t &nIndexOut );
		// Retrieves the index of the specified group in the execution
		// order.
		
	CTestGroupManager();
	virtual ~CTestGroupManager();
protected:
	virtual void PreAllocateTest() {}
	class CTest *AllocateTest( const tstring &strTestClass );

private:	
	void SetLockTestRegistration( bool bLock ) { m_bRegistrationLocked = bLock; }
	
private:
	CTestGroupBranchNode m_Root;
	CTestGroupNode *m_pEffectiveRoot;
	CTestClassFactory m_TestClassFactory;
	
	bool m_bRegistrationLocked;
	
	friend class CWGFTestFramework;
	friend class CTestGroupLeafNode;
};



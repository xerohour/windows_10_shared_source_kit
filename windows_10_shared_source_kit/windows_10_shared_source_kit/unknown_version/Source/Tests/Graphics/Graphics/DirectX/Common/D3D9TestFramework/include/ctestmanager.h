#ifndef _CTESTMANAGER_H_
#define _CTESTMANAGER_H_

#include "Windows.h"
#include "FrameworkTypes.h"

#define TERMINATE_NUMBER    0xFFFFFFFF
#define DEFAULT_RANGE       0xFFFFFFFE

class CTest;
class CTestManager;

typedef struct _tagRANGE
{
    UINT                uStart;
    UINT                uStop;
    struct _tagRANGE  * pPrev;
    struct _tagRANGE  * pNext;
} TESTRANGE, *PTESTRANGE;

typedef struct _tagTestList
{
	CTest                 * pTest;
	PTESTRANGE              pTestRange;
	struct _tagTestList   * pPrev;
	struct _tagTestList   * pNext;
} TESTLIST, *PTESTLIST;

enum
{
    EXECUTE_RUN,
    EXECUTE_PAUSE,
    EXECUTE_STEP,
    EXECUTE_TERMINATE,
	EXECUTE_REPLAY
};

class CTest
{
    friend class CTestManager;

protected:

	bool					m_bSelected;
    bool            		m_bException;
    PTESTRANGE      		m_pRangeList;
    PTESTRANGE      		m_pConstraints;

    bool AddTestRange(PTESTRANGE * ppRange, UINT uStart, UINT uStop);
    bool SetTestRange(PTESTRANGE * ppRange, UINT uStart, UINT uStop);
    bool SkipTestRange(PTESTRANGE * ppRange, UINT uStart, UINT uStop);
    void ClearRangeList();

public:
	
	UINT        m_uTestNumber;		// Current test number
	UINT		m_uTotalTests;		// Total number of tests in the range
    LPCTSTR     m_szTestName;       // Short name used to describe the test
    LPCTSTR     m_szCommandKey;     // Command key used to manually set test ranges
	LPCTSTR		m_szLogName;		// Log name

    public:

    CTest();
    virtual ~CTest();

    inline	bool	IsSelected() { return m_bSelected; }

    virtual bool	Initialize();
    virtual void    Reset(void);
    virtual bool 	SetCurrentTest(UINT nTestNum);
    virtual bool    SetTestRange(UINT,UINT,bool fConstraint = false);
    virtual bool    AddTestRange(UINT,UINT,bool fConstraint = false);
    virtual bool    SkipTestRange(UINT,UINT,bool fConstraint = false);
    virtual bool    SkipTests(UINT uNumberCount = 1);
	virtual UINT	GetRemainingTests();
    virtual UINT 	FindNextTest(void);

	virtual void    NextGroup(void);
	virtual bool    PreviousGroup(void);
	virtual void	NextTest();
	virtual bool	IsTestFinished();
    inline  void    SetExceptionState(bool bOn) { m_bException = bOn; }
    inline  UINT    GetCurrentTestNum() { return m_uTestNumber; }
    inline  UINT    GetNumTests() { return m_uTotalTests; }

    virtual LRESULT	OnCommand(WPARAM wParam, LPARAM lParam);
    virtual LRESULT	OnMouse(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual LRESULT	OnKeydown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnKeyup(WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnSetCursor(WPARAM wParam, LPARAM lParam);
};


class CTestManager
{
	friend class CTest;

protected:

    PTESTLIST   m_pCurrentTest;
    PTESTLIST   m_pTestList;
	bool		m_bReverseGroup;
	UINT        m_uTestNumber;		// Current test object number
	UINT		m_uTotalTests;		// Total number of test objects in the range

public:

    DWORD       m_dwExecuteState;

    public:

    CTestManager();
    virtual ~CTestManager();

	virtual void	ReleaseAll();

    // Test management

    virtual bool    AddTest(CTest *);
    virtual bool    AddTest(CTest *,UINT,UINT);
    virtual bool    SetCurrentTest(CTest *);
    virtual CTest * GetTest(UINT uIndex);
    virtual CTest * GetActiveTest(UINT uIndex);
    virtual CTest * GetCurrentTest(void);
    virtual bool    AddTestRange(CTest *,UINT,UINT);
    virtual bool    QueryTestRange(CTest * pTest);

    inline  UINT    GetCurrentTestNum() { return m_uTestNumber; }
    inline  UINT    GetNumTests() { return m_uTotalTests; }

	virtual void	Run();
    virtual void	Step();
    virtual void	Pause();
    virtual void	Replay();
    virtual void    NextGroup();
	virtual void    PreviousGroup(void);
	virtual bool	IsTestFinished();
    virtual bool	IsTestPaused();
    virtual bool	IsTestReplaying();
};

#endif _CTESTMANAGER_H_


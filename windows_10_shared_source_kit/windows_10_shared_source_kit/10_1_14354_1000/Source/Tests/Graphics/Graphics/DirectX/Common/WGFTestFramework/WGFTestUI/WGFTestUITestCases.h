#pragma once

#include "WGFTestUIWindow.h"

#define CACHESIZE 512

typedef struct _ListItem
{
	int     iTestNumber;
	bool	enabled;
    tstring *ptcText;    // array of string keeping text for item and all subitems
} ListItem, *PListItem;

class CWGFTestUITestCasesWindow : public CWGFTestUIWindow
{
public:
	CWGFTestUITestCasesWindow();

	bool Create( UINT uID, CWGFTestUIWindow *pParent, int x, int y, int w, int h );
	
	void Invalidate();
	
	//update Test Cases List View when new test starts running or filters have been updated
	bool UpdateTestCasesListView();
	
	//create a playlist for selected set of test cases
	//returns false if playlist was not created
	bool ExecuteSelectedTestCases();

	//delete playlist created using Test Cases List View when execution has been paused
	bool DeleteTempPlayList();
	bool HasTempScript() const { return m_bTempScriptCreated; }
	
	//sets text color of an item corresponding to current test case to red
	bool SetCurrentTestCase();
	
	//handles list view drawing
	bool DrawItemCallback( CWGFTestUIWindow *pParent, LPDRAWITEMSTRUCT lpdis );

	//handle WM_NOTIFY messages
	bool NotifyCallback( int idCtrl, LPNMHDR pnmhdr );

	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	//clean up list and caches
	void DestroyList();

	//create caches
	void PrepCache( UINT iFrom, UINT iTo );

	//fill out cache elements
	void FillCache( PListItem list, size_t iStart, size_t iCount );

	void RetrieveItem( PListItem pliItem, size_t index );
	
private:
	size_t		m_nNumColumns;         //reflects (number of parameters + test case num + pri val + pri level)
	UINT		m_nNumRows;            //reflects number of test cases in the current test
	UINT		m_iCurrentTestCase;    //is used to determine which test case has been curent to switch red color to black when drawing an item in the list view
	bool		m_bTempScriptCreated;  //is used to determine if playlist has been created using Test Cases View List
	bool		m_bValidCasesOnly;
	HMENU		m_hTestCasesMenu;
	
	//memebers used for caching
	PListItem	m_pliCache;
	UINT		m_iCacheStart;
	UINT		m_iCacheCount;
	
	bool		m_bValid;
};

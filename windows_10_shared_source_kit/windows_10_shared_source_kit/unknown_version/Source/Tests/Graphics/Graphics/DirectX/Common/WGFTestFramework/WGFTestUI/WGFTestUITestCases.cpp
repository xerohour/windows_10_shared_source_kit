#include "pch.h"

#include "resource.h"
#include "WGFTestUITestCases.h"

extern HINSTANCE   m_hUserInterfaceInstance;

//------------------------------------------------------------------

CWGFTestUITestCasesWindow::CWGFTestUITestCasesWindow() :
	m_bTempScriptCreated( false ),
	m_iCurrentTestCase( -1 ), 
	m_hTestCasesMenu( NULL ),
	m_nNumColumns( 0 ),
	m_nNumRows( 0 ),
	m_pliCache( NULL ),
	m_iCacheStart( 0 ),
	m_iCacheCount( 0 ),
	m_bValidCasesOnly( false ),
	m_bValid( false )
{}

//------------------------------------------------------------------

bool CWGFTestUITestCasesWindow::Create( UINT uID, CWGFTestUIWindow *pParent, int x, int y, int w, int h )
{
	DWORD dwListViewExStyle = LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT;
	DWORD dwListViewStyle = LVS_NOLABELWRAP | LVS_REPORT | LVS_OWNERDRAWFIXED | LVS_OWNERDATA;
	
	if( !CreateEx( WS_EX_CLIENTEDGE, WC_LISTVIEW, NULL, WS_CHILD | dwListViewStyle, x, y, w, h, pParent, (HMENU) (INT_PTR) uID, 0 ) )
		return false;
	
	SendMessage( LVM_SETEXTENDEDLISTVIEWSTYLE, (WPARAM) dwListViewExStyle, (LPARAM) dwListViewExStyle );

	m_nNumColumns = 0;
	m_nNumRows    = 0;
	m_iCurrentTestCase   = -1;
	m_bTempScriptCreated = false;

	// Load menu for test cases.
	m_hTestCasesMenu = LoadMenu( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDR_MENU_TESTCASES ) );
		
	return true;
}

//------------------------------------------------------------------

void CWGFTestUITestCasesWindow::DestroyList()
{
	if ( m_bTempScriptCreated )
		DeleteTempPlayList();

	//remove all items and columns
	SendMessage( LVM_DELETEALLITEMS, 0, (LPARAM) 0 );		
	for (size_t i = 0; i < m_nNumColumns; i++)
		SendMessage( LVM_DELETECOLUMN, 0, (LPARAM) 0 );

	//clean up cache
	for (size_t i = 0; i < m_iCacheCount; i++)
		delete [] m_pliCache[ i ].ptcText;
	delete [] m_pliCache;
	m_pliCache = NULL;

	m_iCacheStart    = 0;
	m_iCacheCount    = 0;
	
	m_iCurrentTestCase = -1;

}

//------------------------------------------------------------------

void CWGFTestUITestCasesWindow::Invalidate()
{
	m_bValid = false;

	RedrawWindow( NULL, NULL, RDW_INVALIDATE );
}

bool CWGFTestUITestCasesWindow::UpdateTestCasesListView()
{
	DestroyList();
	
	size_t index = 0;

	// Add columns
	LVCOLUMN lvc;
	lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
	lvc.pszText = _T( "Test Number" );
	lvc.iSubItem = (int) index++;
	lvc.cx = 100;
	SendMessage( LVM_INSERTCOLUMN, lvc.iSubItem, (LPARAM) &lvc );
	
	CTest *pTest = GetFramework()->GetCurrentTest();
	
	if( pTest )
	{	
		//add a column for each test parameter
		for( int i = 0; i < pTest->GetNumParameters(); i++ )
		{
			const tstring &strName = pTest->GetTestCaseParameter( i )->GetName();

			lvc.pszText = _T( const_cast< LPTSTR >( strName.c_str() ) );
			lvc.iSubItem = (int) index++;
			lvc.cx = 100;
			SendMessage( LVM_INSERTCOLUMN, lvc.iSubItem, (LPARAM) &lvc );
		}
		
		//add columns for priority value
		lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		lvc.pszText = _T( "Priority Value" );
		lvc.iSubItem = (int) index++;
		lvc.cx = 100;
		SendMessage( LVM_INSERTCOLUMN, lvc.iSubItem, (LPARAM) &lvc );
	
		//add columns for priority level
		lvc.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
		lvc.pszText = _T( "Priority Level" );
		lvc.iSubItem = (int) index++;
		lvc.cx = 100;
		SendMessage( LVM_INSERTCOLUMN, lvc.iSubItem, (LPARAM) &lvc );
	
		m_nNumColumns = index;
		m_nNumRows    = pTest->GetNumVariations();

		LRESULT lr = SendMessage( LVM_SETITEMCOUNT, m_nNumRows, 0 ); 

		//set up cache at the begining of the list
		size_t nCaseIndex = GetFramework()->GetCurrentTestCase();
		
		PrepCache( nCaseIndex, nCaseIndex );
		SendMessage( LVM_ENSUREVISIBLE, GetFramework()->GetCurrentTestCase() - 1, (LPARAM) FALSE );
	}
	
	return true;
}

//------------------------------------------------------------------

//returns false if no test cases are selected
bool CWGFTestUITestCasesWindow::ExecuteSelectedTestCases()
{
	LRESULT lr = -1;
	lr = SendMessage( LVM_GETNEXTITEM, (WPARAM)(int) lr, (LPARAM) MAKELPARAM ((UINT) LVNI_SELECTED, 0) );

	//no test cases are selected -> we don't create playlist
	if (lr == -1)
		return false;

	//clean up existing playlist and setup a new one
	GetFramework()->PlayListClear();
    const CTestGroupPath &path = GetFramework()->GetCurrentTestPath();
		
	//insert selected test cases into the execute set
	while (lr != -1)
	{
		GetFramework()->PlayListAddCaseRange( path, lr + 1, lr + 1);
		
		lr = SendMessage( LVM_GETNEXTITEM, (WPARAM)(int) lr, (LPARAM) MAKELPARAM ((UINT) LVNI_SELECTED, 0) );
	}

	GetFramework()->GoToFirstTestCase();
	m_bTempScriptCreated = true;
	
	return true;
}

//------------------------------------------------------------------

bool CWGFTestUITestCasesWindow::DeleteTempPlayList()
{
	if ( m_bTempScriptCreated )
	{
		// Clean up existing playlist
		GetFramework()->PlayListClear();
		GetFramework()->PlayListAddBranch( CTestGroupPath() );
			
		m_bTempScriptCreated = false;

		//unselect all selected test cases
		LRESULT lr = -1;
		LVITEM lvi;
		lvi.state = 0;
		lvi.stateMask = LVIS_SELECTED;
			
		lr = SendMessage( LVM_SETITEMSTATE, (WPARAM)(int) lr, (LPARAM) &lvi );

		//remove focus from currently focused item
		lr = -1;
		lvi.state = 0;
		lvi.stateMask = LVIS_FOCUSED;
			
		lr = SendMessage( LVM_SETITEMSTATE, (WPARAM)(int) lr, (LPARAM) &lvi );
	}

	return true;
}

//------------------------------------------------------------------

bool CWGFTestUITestCasesWindow::SetCurrentTestCase()
{
	LRESULT lr;

	//need to draw previously set test case to change color to black
	lr = SendMessage( LVM_REDRAWITEMS, m_iCurrentTestCase, m_iCurrentTestCase );

	//need to draw newly set test case to change color to red
	m_iCurrentTestCase = GetFramework()->GetCurrentTestCase() - 1;
	lr = SendMessage( LVM_REDRAWITEMS, m_iCurrentTestCase, m_iCurrentTestCase );
	
	return true;
}


//------------------------------------------------------------------

//draw items in the TestCases table
bool CWGFTestUITestCasesWindow::DrawItemCallback( CWGFTestUIWindow *pParent, LPDRAWITEMSTRUCT lpdis )
{
	COLORREF clrPrevText   = GetTextColor( lpdis->hDC );
	COLORREF clrPrevBkText = GetBkColor  ( lpdis->hDC );
	int      iPrevBkMode   = GetBkMode   ( lpdis->hDC );
	RECT rc = lpdis->rcItem;
	PAINTSTRUCT ps;
	HDC hPaintDC = BeginPaint(&ps);
    int nLeft = lpdis->rcItem.left;
    RECT rcClient;
    
    GetClientRect( &rcClient );

	CTest *pTest = GetFramework()->GetCurrentTest();

	if( pTest )
	{
		ListItem li;
		RetrieveItem( &li, lpdis->itemID );
		
		//white is default color
		//gray out filtered out test cases
		//red is set for current test case
		if ( li.enabled )
		{
			if ( lpdis->itemID == GetFramework()->GetCurrentTestCase() - 1 )
				clrPrevText = SetTextColor( lpdis->hDC, RGB( 255, 0, 0 ) );
			else
				clrPrevText = SetTextColor( lpdis->hDC, RGB( 0, 0, 0 ) );
		}
		else
			clrPrevText = SetTextColor( lpdis->hDC, RGB( 192, 192, 192 ) );


		//indicate selected item
		if ( (lpdis->itemState & ODS_SELECTED) )
		{
			if ( li.enabled )
			{
				if ( lpdis->itemID == GetFramework()->GetCurrentTestCase() - 1 )
					clrPrevText = SetTextColor( lpdis->hDC, RGB( 255, 0, 0 ) );
				else
					clrPrevText = SetTextColor(lpdis->hDC, RGB( 255, 255, 255 ) ); 

				clrPrevBkText = SetBkColor(lpdis->hDC, RGB( 0, 0, 0 ) ); 
				iPrevBkMode = SetBkMode( lpdis->hDC, TRANSPARENT );
				FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH) GetStockObject(BLACK_BRUSH)); 
			}
			else
			{
				if ( lpdis->itemID == GetFramework()->GetCurrentTestCase() - 1 )
					clrPrevText = SetTextColor( lpdis->hDC, RGB( 255, 0, 0 ) );
				else
					clrPrevText = SetTextColor(lpdis->hDC, RGB( 255, 255, 255 ) ); 

				clrPrevBkText = SetBkColor(lpdis->hDC, RGB( 191, 191, 191 ) ); 
				iPrevBkMode = SetBkMode( lpdis->hDC, TRANSPARENT );
				HBRUSH hBrush = CreateSolidBrush( RGB( 191, 191, 191 ) );
				FillRect(lpdis->hDC, &lpdis->rcItem, hBrush); 
				DeleteObject( hBrush );
			}
		}
		else
		{
			FillRect(lpdis->hDC, &lpdis->rcItem, (HBRUSH) GetStockObject(WHITE_BRUSH));
			iPrevBkMode = SetBkMode( lpdis->hDC, TRANSPARENT );
		}

		//set focus
		if ( (lpdis->itemState & ODS_FOCUS) )
		{
			RECT rcFocus = lpdis->rcItem;
			rcFocus.bottom -= 1;
			COLORREF c = GetTextColor( lpdis->hDC );
			DrawFocusRect(lpdis->hDC, &rcFocus); 
		}
		
		for (size_t iColumn = 0; iColumn < m_nNumColumns; iColumn++)
		{
			//if all necessary columns have already been updated then exit the loop
			if ( nLeft >= rcClient.right )
				break;

			int w = (int) SendMessage( LVM_GETCOLUMNWIDTH, iColumn, 0 );
			
			int nRight = nLeft + w;

			//if current column does't need to be updated then proceed to next column
			if( nRight >= 0 )
			{
				LVITEM lvi;
				TCHAR tchBuffer[256];
				tchBuffer[ 0 ] = '\0';
				lvi.iSubItem = (int) iColumn;
				lvi.pszText = tchBuffer;
				lvi.cchTextMax = 256;
				SendMessage( LVM_GETITEMTEXT, lpdis->itemID, (LPARAM) &lvi );
				
				rc.left   = nLeft + 5;
				rc.right  = nLeft + w;
				DrawText( lpdis->hDC, lvi.pszText, -1, &rc, DT_LEFT );
			}
			
			nLeft = nRight;
		}
	}
	
	SetTextColor(lpdis->hDC, clrPrevText  ); 
    SetBkColor  (lpdis->hDC, clrPrevBkText);
	SetBkMode   (lpdis->hDC, iPrevBkMode  );

	EndPaint( &ps );
	    	
	return true;
}

//------------------------------------------------------------------

void CWGFTestUITestCasesWindow::PrepCache( UINT iFrom, UINT iTo )
{
    BOOL fOLFrom = FALSE;
    BOOL fOLTo = FALSE;

	//set of items is outside the list range
	if ( iFrom >= m_nNumRows )
		return;

	//trim the end selection
	if ( iTo >= m_nNumRows )
		iTo = m_nNumRows - 1;

	 // Check to see if iFrom is within cache
        if ((m_iCacheCount) &&
            (iFrom >= m_iCacheStart) &&
            (iFrom < m_iCacheStart + m_iCacheCount))
            fOLFrom = TRUE;

        // Check to see if iTo is within cache
        if ((m_iCacheCount) &&
            (iTo >= m_iCacheStart) &&
            (iTo <  m_iCacheStart + m_iCacheCount))
            fOLTo = TRUE;

        // If both iFrom and iTo are within current cache, no work is needed.
        if (fOLFrom && fOLTo)
            return;

        //define new cache limits
		//we want our range [iFrom, iTo] to be in the middle of new cache
		//use temp variables to keep new range values
		UINT nSize = iTo - iFrom + 1;
		if( nSize < CACHESIZE )
		{
			// Grow the range in both directions.
			UINT uGrow = CACHESIZE - nSize;
			UINT uFront = min( uGrow / 2, iFrom );
			UINT uBack = min( m_nNumRows - 1 - iTo, CACHESIZE - uFront - nSize );
			iFrom -= uFront;
			iTo += uBack;
		}

        // A new primary cache is needed; free the old one.
        if ( m_pliCache )
        {
			for (size_t n = 0; n < m_iCacheCount; n++)
				delete [] m_pliCache[ n ].ptcText;
            delete [] m_pliCache;
			m_pliCache = NULL;
		}

        // Set the index and count values for the new primary cache, and then retrieve the memory.
        m_iCacheStart = iFrom;
        m_iCacheCount = iTo - iFrom + 1;
        m_pliCache = new ListItem[ sizeof( ListItem ) * m_iCacheCount ];
        
        if( !m_pliCache )
			return;
			
		ZeroMemory( m_pliCache, sizeof( ListItem ) * m_iCacheCount );

        // Call function that accesses items information and fills cache elements here.
		FillCache( m_pliCache, m_iCacheStart, m_iCacheCount );
}

//------------------------------------------------------------------

void CWGFTestUITestCasesWindow::FillCache( PListItem list, size_t iStart, size_t iCount )
{
	size_t index = 0;

	CTest *pTest       = GetFramework()->GetCurrentTest();
	size_t nCaseNumber = GetFramework()->GetCurrentTestCase();

	while ( index < iCount )
	{
		list[ index ].ptcText = new tstring[ m_nNumColumns ];
		
		TCHAR str[256];
		_stprintf( str, _T( "%d" ), iStart + index + 1 );
		list[ index ].ptcText[ 0 ] = str;
		list[index].enabled = true;
		
		if( !pTest->GenerateVariation( iStart + index + 1 ) )
		{
			list[index].enabled = false;
		}
		
		CFilterHandle filter = pTest->GetRootFilter();
		
		if( filter.IsValid() && !filter->Match() )
		{
			list[index].enabled = false;
		}

		for (size_t iSubItem = 1; iSubItem < m_nNumColumns - 2; iSubItem++)
			list[ index ].ptcText[ iSubItem ] = pTest->GetTestCaseParameter( iSubItem - 1 )->GetValueAsString();

		// fill priority value and level
		list[index].ptcText[m_nNumColumns - 2] = ToString(pTest->GetVariationPriorityValue(iStart + index + 1));
		list[index].ptcText[m_nNumColumns - 1] = ToString(pTest->GetVariationPriorityLevel(iStart + index + 1));

		index++;
	}

	pTest->GenerateVariation( nCaseNumber );
}

//------------------------------------------------------------------

void CWGFTestUITestCasesWindow::RetrieveItem( PListItem pliItem, size_t index )
{
	// Check to see if the item is in the cache.
    if ((index >= m_iCacheStart) && (index < m_iCacheStart + m_iCacheCount))
        *pliItem = m_pliCache[ index - m_iCacheStart ];
	else 
	{
		PrepCache( index - CACHESIZE/2, index + CACHESIZE/2  - 1 );
		*pliItem = m_pliCache[ index - m_iCacheStart ];
	}
}

//------------------------------------------------------------------

bool CWGFTestUITestCasesWindow::NotifyCallback( int idCtrl, LPNMHDR pnmhdr )
{
	switch( pnmhdr->code )
	{
		case LVN_GETDISPINFO:
		{
			NMLVDISPINFO *plvdi = (LPNMLVDISPINFOA) pnmhdr;

			LRESULT lr = FALSE;
			ListItem liItem;

            if (-1 == plvdi->item.iItem)
			{
                OutputDebugString("LVOWNER: Request for -1 item?\n");
                DebugBreak();
            }
            // Retrieve information for item at index iItem.
            RetrieveItem( &liItem, plvdi->item.iItem );
			            
            if(plvdi->item.mask & LVIF_TEXT)
			{
                // Fill in the text information.
				plvdi->item.pszText = const_cast< LPTSTR >(liItem.ptcText[ plvdi->item.iSubItem ].c_str());
             }
            
			return FALSE;
			//break;
		}

		case LVN_ODCACHEHINT:
		{
			NMLVCACHEHINT *pCacheHint = (NMLVCACHEHINT *) pnmhdr;

			// Load the cache with the recommended range.
			PrepCache( pCacheHint->iFrom, pCacheHint->iTo );

			break;
		} 
	}

	return true; // Indicate that the notification was processed.
}

//------------------------------------------------------------------

LRESULT CWGFTestUITestCasesWindow::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam  )
{
	switch( uMsg )
	{
		case WM_DESTROY:
		{
			DestroyList();
			return 0;
		}

		//when right mouse button is down and CTRL key is pressed 
		//then pop up "Execute Test Cases" menu
		//if user decides to use it then a set of selected test cases is executed
		case WM_RBUTTONDOWN:
		{
			//get location at which action happened
			int x = LOWORD( lParam );
			int y = HIWORD( lParam );
			POINT pt;
			pt.x = x;
			pt.y = y;

			//pop up menu
			if( m_hTestCasesMenu )
			{
				ClientToScreen( &pt );
				HMENU hMenu = GetSubMenu( m_hTestCasesMenu, 0 );
								
				if( hMenu )
				{
					int id = (int) TrackPopupMenu( hMenu, TPM_RETURNCMD | TPM_NONOTIFY, pt.x, pt.y, 0, m_hWnd, NULL );
								
					switch( id )
					{
						case ID_TESTCASES_EXECUTECASES:
							ExecuteSelectedTestCases();
							GetParent()->GetParent()->SendMessage( WM_COMMAND, (WPARAM) ID_EXECUTION_PLAY, (LPARAM) GetHWnd() );
							break;

						/*case ID_TESTCASES_EXECUTECASES_STEP:
							m_TestCasesWindow.ExecuteSelectedTestCases();
							SetState( STATE_REPLAY );
							break;*/
					}
				}
			}

			break;
		}
		
		case WM_PAINT:
			if( !m_bValid )
			{
				UpdateTestCasesListView();
				m_bValid = true;
			}
			break;

		//selection of item by double-clicking left mouse button
		//this causes single test case execution
		//if this test case is invalid then action is canceled
		case WM_LBUTTONDBLCLK:
		{
			//get location at which action happened
			int x = LOWORD( lParam );
			int y = HIWORD( lParam );
			POINT pt;
			pt.x = x;
			pt.y = y;
			
			//find list item corresponding to this location
			LVHITTESTINFO hti;
			hti.pt = pt;
			hti.flags = LVHT_ONITEM;

			SendMessage( LVM_HITTEST, 0, (LPARAM) &hti ); 
			
			//bell out if no item found at specified location
			if (hti.iItem == -1)
				break;

			//execute test case at which action occured
			GetFramework()->GoToTestCase( (CTestGroupLeafNode *) GetFramework()->GetTestGroupNode( GetFramework()->GetCurrentTestPath() ), hti.iItem + 1 );
			GetParent()->GetParent()->SendMessage( WM_COMMAND, (WPARAM) ID_EXECUTION_REPLAY, (LPARAM) GetHWnd() );
			
			break;
		}
	}

	return CWGFTestUIWindow::WindowProc( uMsg, wParam, lParam );
}

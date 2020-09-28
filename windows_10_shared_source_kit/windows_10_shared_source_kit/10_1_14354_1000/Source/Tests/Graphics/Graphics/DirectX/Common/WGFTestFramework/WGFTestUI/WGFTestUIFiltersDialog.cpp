//////////////////////////////////////////////////////////////////////////
//  WGFTestUIFiltersDialog.cpp
//  created:	2005/01/05
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "resource.h"

#include "WGFTestUIFiltersDialog.h"

INT_PTR CWGFTestUIFiltersDialog::Execute( CWGFTestUIWindow *pParent )
{
	return DoModal( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDD_FILTERS ), pParent );
}

bool CWGFTestUIFiltersDialog::OnInitDialog()
{
	HWND hList = GetDlgItem( IDC_LIST_FILTERS );
	
	if( hList )
	{
		// Initialize columns
		LVCOLUMN lvc;
		lvc.mask = LVCF_TEXT;
		lvc.pszText = _T( "Name" );
		lvc.iSubItem = 0;
		lvc.iOrder = 0;
		ListView_InsertColumn( hList, 0, &lvc );
		
		lvc.pszText = _T( "Filter Expression" );
		lvc.iSubItem = 1;
		lvc.iOrder = 1;
		ListView_InsertColumn( hList, 1, &lvc );

		UpdateList();
		
		// Adjust column widths
		ListView_SetColumnWidth( hList, 0, LVSCW_AUTOSIZE_USEHEADER );
		ListView_SetColumnWidth( hList, 1, LVSCW_AUTOSIZE_USEHEADER );
		
		// Turn on gridlines.
		ListView_SetExtendedListViewStyle( hList, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );
	}
	
	// Set extended properties on the list control
	return true;
}

void CWGFTestUIFiltersDialog::UpdateList()
{
	HWND hList = GetDlgItem( IDC_LIST_FILTERS );
	
	if( hList )
	{
		ListView_DeleteAllItems( hList );
		
		// Add filter keys.
		CStringKeys::Iterator it;
		ListView_SetItemCount( hList, m_Keys.Size() );
		size_t nIndex = 0;
		for( it = m_Keys.Begin(); it != m_Keys.End(); ++it )
		{
			const tstring &name = CStringKeys::GetIteratorKeyName( it );
			const tstring &value = CStringKeys::GetIteratorKeyValue( it );
			
			LVITEM item;
			item.mask = LVIF_TEXT;
			item.pszText = const_cast< LPSTR >( name.c_str() );
			item.iItem = (int) nIndex;
			item.iSubItem = 0;
			ListView_InsertItem( hList, &item );
			
			item.iSubItem = 1;
			item.pszText = const_cast< LPSTR >( value.c_str() );
			ListView_SetItem( hList, &item );
		}
	}		
}

INT_PTR CWGFTestUIFiltersDialog::DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static TCHAR buffer[4096];
	bool bNew = false;
	
	switch( uMsg )
	{
		case WM_NOTIFY:
		{
			NMHDR *pnmh = (NMHDR *) lParam;
		
			switch( pnmh->code )
			{
				case LVN_ITEMCHANGED:
				{
					LPNMLISTVIEW pnmv = (LPNMLISTVIEW) lParam;
					
					int i = ListView_GetNextItem( GetDlgItem( IDC_LIST_FILTERS ), -1, LVNI_SELECTED );
					::EnableWindow( GetDlgItem( IDC_FILTER_EDIT ), i >= 0 );
					::EnableWindow( GetDlgItem( IDC_FILTER_DELETE ), i >= 0 );
					break;
				}
					
				case NM_DBLCLK:
					SendMessage( WM_COMMAND, (WPARAM) IDC_FILTER_EDIT, (LPARAM) GetHWnd() );
					break;
			}
				
			break;
		}
			
		case WM_COMMAND:
		{
			WORD wCode = HIWORD( wParam );
			WORD wID = LOWORD( wParam );
			
			switch( wID )
			{
				case IDOK:
					EndDialog( IDOK );
					break;
					
				case IDCANCEL:
					EndDialog( IDCANCEL );
					break;
					
				case IDC_FILTER_NEW:
				{
					HWND hList = GetDlgItem( IDC_LIST_FILTERS );					
					m_EditFilterDlg.m_strName = "";
					m_EditFilterDlg.m_strValue = "";
					if( m_EditFilterDlg.Execute( this, true ) == IDOK )
					{
						if( !m_EditFilterDlg.m_strName.empty() )
						{
							m_Keys.SetKey( m_EditFilterDlg.m_strName, m_EditFilterDlg.m_strValue );
							UpdateList();
						}
					}
					break;
				}
				
				case IDC_FILTER_EDIT:
				{
					HWND hList = GetDlgItem( IDC_LIST_FILTERS );					
					int i = ListView_GetNextItem( hList, -1, LVNI_SELECTED );
					
					if( i >= 0 )
					{
						ListView_GetItemText( hList, i, 0, buffer, sizeof( buffer ) );
						m_EditFilterDlg.m_strName = buffer;
						ListView_GetItemText( hList, i, 1, buffer, sizeof( buffer ) );
						m_EditFilterDlg.m_strValue = buffer;
						if( m_EditFilterDlg.Execute( this, false ) == IDOK )
						{
							m_Keys.SetKey( m_EditFilterDlg.m_strName, m_EditFilterDlg.m_strValue );
							UpdateList();
						}
					}
					
					break;
				}	
					
				case IDC_FILTER_DELETE:
				{
					HWND hList = GetDlgItem( IDC_LIST_FILTERS );					
					int i = ListView_GetNextItem( hList, -1, LVNI_SELECTED );
					
					if( i >= 0 )
					{
						ListView_GetItemText( hList, i, 0, buffer, sizeof( buffer ) );
						m_Keys.RemoveKey( buffer );
						UpdateList();
					}
					
					break;
				}
					
				case IDC_FILTER_DELETE_ALL:
					m_Keys.Clear();
					UpdateList();
					break;
			}
			
			break;
		}
			
		case WM_INITDIALOG:
			return (INT_PTR) OnInitDialog();
			break;
	}
	
	return 0;
}


INT_PTR CWGFTestUIEditFilterDialog::Execute( CWGFTestUIWindow *pParent, bool bNew )
{
	return DoModalParam( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDD_EDIT_FILTER ), pParent, (LPARAM) bNew );
}

bool CWGFTestUIEditFilterDialog::OnInitDialog( WPARAM wParam, LPARAM lParam )
{
	if( lParam )
	{
		SetWindowText( _T( "New Filter" ) );
		EnableWindow( GetDlgItem( IDC_EDIT_FILTER_NAME ), true );
	}
	else
	{
		SetWindowText( _T( "Edit Filter" ) );
		EnableWindow( GetDlgItem( IDC_EDIT_FILTER_NAME ), false );
	}
	
	SetDlgItemText( IDC_EDIT_FILTER_NAME, m_strName.c_str() );
	SetDlgItemText( IDC_EDIT_FILTER_VALUE, m_strValue.c_str() );
	
	return true;
}

INT_PTR CWGFTestUIEditFilterDialog::DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_INITDIALOG:
			return (INT_PTR) OnInitDialog( wParam, lParam );
			break;
			
		case WM_COMMAND:
		{
			WORD wCode = HIWORD( wParam );
			WORD wID = LOWORD( wParam );
			
			switch( wID )
			{
				case IDOK:
				{
					static TCHAR buffer[4096];
					GetDlgItemText( IDC_EDIT_FILTER_NAME, buffer, sizeof( buffer ) );
					m_strName = buffer;
					GetDlgItemText( IDC_EDIT_FILTER_VALUE, buffer, sizeof( buffer ) );
					m_strValue = buffer;
					EndDialog( IDOK );
					break;
				}
					
				case IDCANCEL:
					EndDialog( IDCANCEL );
					break;
			}
		}
	}
	
	return 0;
}
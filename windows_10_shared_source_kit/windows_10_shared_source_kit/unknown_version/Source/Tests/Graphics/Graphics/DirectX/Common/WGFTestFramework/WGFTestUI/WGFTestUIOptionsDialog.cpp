//////////////////////////////////////////////////////////////////////////
//  WGFTestUIOptionsDialog.cpp
//  created:	2005/01/05
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "pch.h"
#include "resource.h"
#include "WGFTestUIOptionsDialog.h"

INT_PTR CWGFTestUIOptionsDialog::Execute( CWGFTestUIWindow *pParent )
{
	return DoModal( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDD_OPTIONVARS ), pParent );
}

bool CWGFTestUIOptionsDialog::OnInitDialog()
{
	HWND hList = GetDlgItem( IDC_LIST_OPTIONS );
	
	m_List.Attach( hList );
	m_List.Init();

	if( hList )
	{
		ListView_DeleteAllItems( hList );
		
		// Add option keys.
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

		// Adjust column widths
		ListView_SetColumnWidth( hList, 0, LVSCW_AUTOSIZE_USEHEADER );
		ListView_SetColumnWidth( hList, 1, LVSCW_AUTOSIZE_USEHEADER );
		
		SetFocus( hList );
		
		if( ListView_GetItemCount( hList ) > 0 )
			ListView_SetItemState( hList, 0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED );
	}	
	
	return true;	
}

void CWGFTestUIOptionsDialog::UpdateKeys()
{
	static TCHAR buffer[4096];
	HWND hList = GetDlgItem( IDC_LIST_OPTIONS );
	
	if( hList )
	{
		for( int i = ListView_GetItemCount( hList ) - 1; i >= 0; --i )
		{
			ListView_GetItemText( hList, i, 0, buffer, sizeof( buffer ) );
			tstring strName = buffer;
			ListView_GetItemText( hList, i, 1, buffer, sizeof( buffer ) );
			tstring strValue = buffer;

			// Get the original value
			tstring strOrigValue;
			m_Keys.GetKey(strName, strOrigValue, tstring());
			if(strOrigValue == strValue)
			{
				// Value was unchanged so remove the key from the list to avoid updating it
				m_Keys.RemoveKey(strName);
			}
			else
			{
				// Value was modified so leave in the list to modify
				m_Keys.SetKey( strName, strValue );
			}
		}
	}		
}

INT_PTR CWGFTestUIOptionsDialog::DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	static TCHAR buffer[4096];
	bool bNew = false;
	
	switch( uMsg )
	{
		case WM_COMMAND:
		{
			WORD wCode = HIWORD( wParam );
			WORD wID = LOWORD( wParam );
			
			switch( wID )
			{
				case IDOK:
					UpdateKeys();
					EndDialog( IDOK );
					break;
					
				case IDCANCEL:
					EndDialog( IDCANCEL );
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



#include "pch.h"

#include "resource.h"
#include "WGFTestUIGoToDialog.h"

#include <strsafe.h>


CWGFTestUIGoToDialog::CWGFTestUIGoToDialog() :
	m_nCaseNumber( 0 )
{
}

INT_PTR CWGFTestUIGoToDialog::Execute( CWGFTestUIWindow *pParent )
{
	return DoModal( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDD_GOTOTESTCASE ), pParent );
}

INT_PTR CWGFTestUIGoToDialog::DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
        case WM_INITDIALOG:
        {
			tstring strCaseNum = ToString( m_nCaseNumber );
			SetDlgItemText( IDC_EDIT_CASENUMBER, strCaseNum.c_str() );
			SetDlgItemText( IDC_COMBO_TESTPATH, m_Path.ToString().c_str() );
			
			// Iterate through the available test groups.
			CTestGroupPath Path;
			if( GetFramework()->FindFirstTestGroup( Path ) )
			{
				do
				{
					tstring strPath = Path.ToString();
					SendDlgItemMessage( IDC_COMBO_TESTPATH, CB_ADDSTRING, 0, (LPARAM) strPath.c_str() );
				} while ( GetFramework()->FindNextTestGroup( Path, Path ) );
			}
			
            return TRUE;
		}
		                
		case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{
				case IDOK:
				{
					// Retrieve the data
					TCHAR szBuffer[1024];
					GetDlgItemText( IDC_EDIT_CASENUMBER, szBuffer, sizeof( szBuffer ) );
					if( !FromString( &m_nCaseNumber, tstring( szBuffer ) ) )
					{
						TCHAR szFormatBuffer[ 1024 ] = _T( "" );
						StringCchPrintf
						(
							szFormatBuffer,
							ARRAYSIZE( szFormatBuffer ),
							_T( "%s is not a valid number" ),
							szBuffer
						);

						MessageBox
						(
							m_hWnd,
							szFormatBuffer,
							_T( "Error" ),
							MB_ICONEXCLAMATION
						);

						return 0;
					}
					
					GetDlgItemText( IDC_COMBO_TESTPATH, szBuffer, sizeof( szBuffer ) );
					
					m_Path = CTestGroupPath( szBuffer );
					
					if( !GetFramework()->GetTestGroupNode( m_Path ) )
					{
						TCHAR szFormatBuffer[ 1024 ] = _T( "" );
						StringCchPrintf
						(
							szFormatBuffer,
							ARRAYSIZE( szFormatBuffer ),
							_T( "No such test group: %s" ),
							szBuffer
						);

						MessageBox
						(
							m_hWnd,
							szFormatBuffer,
							_T( "Error" ),
							MB_ICONEXCLAMATION
						);

						return 0;
					}
					
					EndDialog( IDOK );
					return 1;
					break;
				}
				
				case IDCANCEL:
					EndDialog( IDCANCEL );
					return 1;
					break;
							
			}

			break;
		}
	};
	
	return 0;
}
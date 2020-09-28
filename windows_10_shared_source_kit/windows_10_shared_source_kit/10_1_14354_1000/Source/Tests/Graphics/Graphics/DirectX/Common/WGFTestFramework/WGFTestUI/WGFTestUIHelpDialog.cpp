#include "pch.h"

#include "resource.h"
#include "WGFTestUIHelpDialog.h"
#include "shellapi.h"

#define RTF_HEADER "{\\rtf1\\ansi\\ansicpg1252\\deff0\\deflang1033 {\\colortbl ;\\red0\\green0\\blue255;}"

INT_PTR CWGFTestUIHelpDialog::Execute( CWGFTestUIWindow *pParent )
{
	return DoModal( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDD_HELP ), pParent );
}

INT_PTR CWGFTestUIHelpDialog::DialogProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
        case WM_INITDIALOG:
        {
			m_Groups.Create( GetDlgItem( IDC_HELPTREE ) );
			m_RichEdit.Create( GetDlgItem( IDC_HELPTEXT ) );
			
			m_RichEdit.EnableURLDetection( TRUE );
			m_RichEdit.SendMessage( EM_SETEVENTMASK, NULL, ENM_LINK );
			m_RichEdit.SendMessage( EM_SETTARGETDEVICE, NULL, NULL );

			GeneralHelp();

            return TRUE;
		}
		case WM_COMMAND:
		{
			switch( LOWORD( wParam ) )
			{		
				case IDCANCEL:
					EndDialog( IDCANCEL );
					return 1;
			}

			break;
		}
		case WM_NOTIFY:
		{
			LPNMHDR pnmh = (LPNMHDR) lParam;
			int idCtrl = (int) wParam;
			
			switch( pnmh->code )
			{
				case TVN_SELCHANGED:
				{
					LPNMTREEVIEW pnmtv = (LPNMTREEVIEW) lParam;
					SETTEXTEX pSetText;

					pSetText.codepage = 0x1200;
					pSetText.flags = ST_DEFAULT;
					
					if ( 1 == pnmtv->itemNew.lParam )
					{						
						GeneralHelp();
					}			
					else if ( 0 == pnmtv->itemNew.lParam || 0 < ((CTestGroupNode *)pnmtv->itemNew.lParam)->GetNumChildGroupNodes() )
					{					
						//No help for non-leaf nodes
						tstring szText = RTF_HEADER;
						szText += _T("\\b\\fs24 Please select a test node.\\par}");
						m_RichEdit.SetText( &pSetText, szText.c_str() );
					}
					else
					{
						TestParamHelp();
					}

					break;
				}
				case EN_LINK:
				{
					ENLINK *enlk = (ENLINK*) lParam;

					switch( enlk->msg )
					{
						case WM_LBUTTONDBLCLK:
						case WM_LBUTTONDOWN:
						{
							char *szBuf = new char[enlk->chrg.cpMax - enlk->chrg.cpMin];
							m_RichEdit.SetSelection( &enlk->chrg );
							m_RichEdit.GetSelText( szBuf );
							ShellExecute( GetHWnd(), "open", szBuf, NULL, NULL, SW_SHOW );
							delete[] szBuf;
							return 1;
						}
						default:
							return 0;
					}
				}
			}			
		}		
	};
	
	return 0;
}

void CWGFTestUIHelpDialog::GeneralHelp()
{
	static tstring szText;
	static bool bGenerated;

	SETTEXTEX pSetText;
	pSetText.codepage = 0x1200;
	pSetText.flags = ST_DEFAULT;
	m_RichEdit.SetText( &pSetText, _T("") );
	
	if ( !bGenerated )
	{
		int iLastCategory = 0;
		CWGFTestFramework &cFr = *GetFramework();

		szText = RTF_HEADER;

		const CPropertyManager *pOptionsManager = GetFramework()->GetOptionsPropertyManager();
		typedef std::multimap<tcistring,const CProperty *> OptionsByCategoryType;
		OptionsByCategoryType optionsByCategory;

		// Get the full list of properties from the options manager
		UINT optionsCount = pOptionsManager->GetPropertyCount();
		CProperty **pOptionsArray = new CProperty *[optionsCount];
		pOptionsManager->GetProperties(pOptionsArray, optionsCount);

		// For each option...
		for(UINT optionIndex = 0; optionIndex < optionsCount; ++optionIndex)
		{
			// ...get the category
			const CProperty *pOption = pOptionsArray[optionIndex];
			tstring category;
			if(!pOption->IsAttributeSet(_T("Category")))
			{
				//...if no category then set to "General"
				category = _T("GENERAL");
			}
			else
			{
				category = pOption->GetAttribute(_T("Category"));
			}

			optionsByCategory.insert(OptionsByCategoryType::value_type(ToUpper(category), pOption));
		}

		// For each category...
		OptionsByCategoryType::iterator itCategory = optionsByCategory.begin();
		tstring currentCategory = itCategory->first;
		szText += "\\b\\fs24 ";
		szText += currentCategory;
		szText += "\\par";

		for(;itCategory != optionsByCategory.end(); ++itCategory)
		{
			// ...adjust the current category name if needed
			if(currentCategory != tstring(itCategory->first))
			{
				currentCategory = itCategory->first;
				szText += "\\b\\fs24 ";
				szText += currentCategory;
				szText += "\\par";
			}

			const CProperty *pOption = itCategory->second;

			// ...determine of the property is hidden
			const tstring &isHiddenString = pOption->GetAttribute(_T("IsHidden"));
			bool isHidden = ToLower(isHiddenString) == _T("yes");

			if(!isHidden)
			{
				// ...and if not hidden then write out the option information
				const tstring &strName = pOption->GetName();
				const tstring &strDescription = pOption->GetAttribute(_T("Description"));
				const tstring &strCmdLineUsage = pOption->GetAttribute(_T("CmdLineUsage"));

				szText += "\\plain\\tab\\fs20\\b ";
				szText += strName;
				szText += " - \\b0";
				szText += strDescription;
				szText += "\\line\\tab\\tab\\cf1 Usage:  ";
				szText += strCmdLineUsage;
				szText += "\\cf0\\par";
			}
		}

		delete[](pOptionsArray);
		
		szText += "}";

		bGenerated = true;
	}

	m_RichEdit.SetText( &pSetText, szText.c_str() );
}
void CWGFTestUIHelpDialog::TestParamHelp()
{
	SETTEXTEX pSetText;
	pSetText.codepage = 0x1200;
	pSetText.flags = ST_DEFAULT;

	CTest &ct = *GetFramework()->GetTest( m_Groups.PathFromCurSel() );
	tstring szText = RTF_HEADER;

	szText += "\\b\\fs24";
	szText += ct.GetTestName();
	szText += "\\par\\b0\\fs20";
	szText += ct.GetTestDesc();
	szText += "\\par\\b\\fs24Parameters\\par\\b0";

	for( int i = 0; i < ct.GetNumParameters(); i++ )
	{
		szText += "\\b\\tab\\fs20";
		szText += ct.GetTestCaseParameter( i )->GetName();
		szText += "\\b0\\par\\tab\\tab\\fs20";
		szText += ct.GetTestCaseParameter( i )->GetDesc();
		szText += "\\par";
	}

	//General filter example - if we actually have at least one param
	//TODO: Could be done much more nicely, if I had a way of getting some kind of default or display value, even for inactive params
	if ( ct.GetNumParameters() )
	{
		CTestCaseParameterBase *tcp = ct.GetTestCaseParameter(0);

		szText += "\\par\\b\\fs24Example - \\b0\\fs20This would filter test cases to ";
		szText += "those where ";
		szText += tcp->GetName();
		szText += " matched <value>.";
		//szText += tcp->GetValueAsString();

		szText += "\\line\\tab\\cf1 ";
		szText += tcp->GetName();
		szText += "=<value>";
		//szText += tcp->GetValueAsString();
	}

	szText += "}";

	m_RichEdit.SetText( &pSetText, szText.c_str() );
}

void CWGFTestUIHelpTreeControl::Initialize( )
{
	TVINSERTSTRUCT tvis;

	tvis.hParent = TVI_ROOT;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	
	tvis.item.pszText = _T("Command Line Help");
	tvis.item.iImage = tvis.item.iSelectedImage = 2;
	tvis.item.lParam = 1;
	HTREEITEM hItem = (HTREEITEM) SendMessage( TVM_INSERTITEM, 0, (LPARAM) &tvis );	

	TreeView_Select( GetHWnd(), hItem, TVGN_CARET );

	tvis.item.pszText = _T("Test Parameter Help");
	tvis.item.iImage = tvis.item.iSelectedImage = 0;
	tvis.item.mask |= TVIF_STATE;
	tvis.item.stateMask = TVIS_BOLD;
	tvis.item.state = TVIS_BOLD;
	tvis.item.lParam = 0;

	hItem = (HTREEITEM) SendMessage( TVM_INSERTITEM, 0, (LPARAM) &tvis );

	CWGFTestUIGroupTreeControl::AddGroups( hItem );

	m_hImageList = ImageList_LoadBitmap( GetModuleHandle( NULL ), MAKEINTRESOURCE( IDB_GROUPS_ICONS ), 16, 0, CLR_NONE );	
	TreeView_SetImageList( m_hWnd, m_hImageList, TVSIL_NORMAL );
}

#pragma once

#include "WGFTestUIWindow.h"

class CWGFTestUIGroupsWindow : public CWGFTestUIGroupTreeControl
{
public:
	CWGFTestUIGroupsWindow();
	
	void Initialize();
	bool NotifyCallback( int idCtrl, LPNMHDR pnmhdr );
	bool HasTempScript() const { return m_bTempScript; }
	void DeleteTempPlayList()
		{
			if( m_bTempScript )
			{
				// Clean up existing playlist
				GetFramework()->PlayListClear();
				GetFramework()->PlayListAddBranch( CTestGroupPath() );
					
				m_bTempScript = false;
			}
		}

private:
	HMENU m_hGroupsMenu;
	bool m_bTempScript;
};
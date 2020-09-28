#pragma once

#include "WGFTestUIWindow.h"

typedef std::basic_string< TCHAR > tstring;

class CWGFTestUIStatusWindow : public CWGFTestUIWindow
{
public:
	CWGFTestUIStatusWindow();
	
	bool Create( UINT uID, CWGFTestUIWindow *pParent, int x, int y, int w, int h );
	
	LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	
	size_t SetItem( LPCTSTR szName, LPCTSTR szValue );
	void ClearItems( size_t nStart = 0, size_t nEnd = NPOS );
	void RemoveItem( LPCTSTR szName );
	void Refresh();
	
	static const size_t NPOS;
	
private:
	void InvalidateItemRect( size_t nIndex, bool bName, bool bValue );
	void UpdateScrollRange();

private:
	enum
	{
		DIRTY_NAME = 0x0001,
		DIRTY_VALUE = 0x0002,
		INVALID = 0x0004,
	};

	struct Item
	{
		Item() {}
		Item( const tstring &name, const tstring &value ) : strName( name ), strValue( value ), dwFlags( DIRTY_NAME | DIRTY_VALUE ) {}
		Item( const Item &item ) : strName( item.strName ), strValue( item.strValue ), dwFlags( item.dwFlags ) {}
		
		tstring strName;
		tstring strValue;
		DWORD dwFlags;
	};
	
	typedef std::map< tstring, size_t > ItemIndexMapType;
	typedef std::vector< Item > ItemsType;
	
	ItemIndexMapType m_ItemIndexMap;
	ItemsType m_Items;
	HFONT m_hFont;
	HBRUSH m_hBrush;
	int m_nLineHeight;
	int m_nMarginTop;
	int m_nMarginLeft;
	int m_nNameColWidth;
	HBITMAP m_hbmOffScreen;
};



class CWGFTestUIPropertyWindow : public CWGFTestUIWindow
{
public:
	bool Create( UINT uID, CWGFTestUIWindow *pParent, int x, int y, int w, int h );
};


#ifndef _CCommandLine_H_
#define _CCommandLine_H_



#define CMDLINEOPTIONBUFFER  1024

#include <windows.h>
#include "FrameworkTypes.h"

class CCommandLine
{
    private:

//  CLogWindow  cLogWin;
    
    TCHAR       szTempBuffer[MAX_PATH+CMDLINEOPTIONBUFFER];
    TCHAR      *szBuffer;
	TCHAR	   *szParam;
    DWORD       dwBufferSize;
    LPTSTR      FindKey(LPCTSTR,LPTSTR *);
    void        Remove(LPTSTR,LPTSTR);

    public:

    bool        ATOI(LPTSTR,int *);

	CCommandLine();
	~CCommandLine();

	// Retrive cmdline info
    virtual TCHAR*  GetParam() { return szParam; }

    // Read command line interfaces

    virtual void    ReadString(LPCTSTR pKey, LPCTSTR pDefault, __out_ecount(nBuffer) LPTSTR pBuffer, int nBuffer, bool bRemove = FALSE);
    virtual bool    ReadString(LPCTSTR pKey, __out_ecount(nBuffer) LPTSTR pBuffer, int nBuffer, bool bRemove = FALSE);

    virtual void    ReadInteger(LPCTSTR pKey, int iDefault, __out int * pInt, bool bRemove = FALSE);
    virtual bool    ReadInteger(LPCTSTR pKey, __out int * pInt, bool bRemove = FALSE);

    virtual void    ReadFloat(LPCTSTR pKey, float fDefault, __out float * pFloat, bool bRemove = FALSE);
    virtual bool    ReadFloat(LPCTSTR pKey, __out float * pFloat, bool bRemove = FALSE);

    virtual bool    KeySet(LPCTSTR pKey, bool bRemove=FALSE);

    // Append a string directly to the command line interface.

    virtual void    AddKeys(LPCTSTR pKey);

    // Refresh the command line

    void    Refresh();

    // Command line options window controls

//    virtual bool    CreateCommandWindow(HWND hParent, LPTSTR pTitle=NULL, bool bClose=TRUE);
//    virtual void    WriteCommandOption(LPTSTR pString, ...);
//    virtual void    ClearCommandWindow(void);
};

 
#endif


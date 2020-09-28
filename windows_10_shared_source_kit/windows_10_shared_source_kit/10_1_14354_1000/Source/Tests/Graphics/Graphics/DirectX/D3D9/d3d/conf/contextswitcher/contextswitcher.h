//
// FILE: D3dContextSwitch.h
// DESC: Direct3D context switch conformance test
// COMMENTS:
// OWNER: Youssef Barakat (YoussefB)
//

#include <d3d9.h>
#include <ddraw.h>
#include <string.h>
#include <string>
#include "CD3DTest.h"
#include "CFlags.h"
#include "CTextures.h"

//
// Number of tests to cycle through
//
#define TEST_CHANGE_PRIMARY_FRAME_INDEX 3
#define TEST_CHANGE_ALTERNATE_FRAME_INDEX 8

//
// Change focus every n'th frame
//
#define FOCUS_CHANGE_PRIMARY_FRAME_INDEX 5
#define FOCUS_CHANGE_ALTERNATE_FRAME_INDEX 15

//
// Change resolution every n'th frame
//
#define RES_CHANGE_PRIMARY_FRAME_INDEX 27
#define RES_CHANGE_ALTERNATE_FRAME_INDEX 17

#define DEFAULT_NUMBER_OF_TESTS 50

#define GLE(x) (m_pD3D->HResultToString(x))

#define CF_PRIMARY 0x00000001
#define CF_ALTERNATE 0x00000002
#define CF_PRIMARY_ONLY 0x00000004
#define CF_CHANGE_FOCUS 0x00000008
#define CF_CHANGE_RES 0x00000010
#define CF_SKIP 0x00000020
#define CF_VALIDATE 0x00000040
#define CF_APPEND 0x00000080

#define CONTEXT_SWITCHER_MUTEX_NAME "__CONTEXT_SWITCHER_MUTEX__"

#define PRI_LOGNAME	_T("ContextSwitcherPri")
#define ALT_LOGNAME	_T("ContextSwitcherAlt")
#define PRI_LOGNAME	_T("ContextSwitcherPri")
#define TMP_LOGNAME	_T("ContextSwitcherTmp")

#define CS_LOGNAME	_T("ContextSwitcher")
#define WTT_LOGGER_HEAD _T("<WTT-Logger>")
#define WTT_LOGGER_TAIL	_T("</WTT-Logger>")
#define EOL_LENGTH 3 

//
// This adjusts the number of tests for the primary process to
// gNumberOfDXVersions times the passed in number of tests
// plus an adjustment factor for every DX switch of gNumberOfDXVersions-1
// times 5 tests per switch.
//
#define ADJUSTED_TESTS(_x_) gNumberOfDXVersions*_x_+((gNumberOfDXVersions-1)*5)

typedef std::basic_string<TCHAR> tstring;
typedef std::basic_ifstream<tstring::value_type> ifstream;

typedef struct _MODELIST
{
   DEVMODE DeviceMode;
   struct _MODELIST *NextMode;
} MODELIST, *PMODELIST;

class CSwitcher: public CD3DTest
{
   public:
   CSwitcher();
   ~CSwitcher();
   virtual VOID CommandLineHelp(VOID);
   virtual bool CapsCheck(VOID);
   virtual bool Setup();
   virtual UINT TestInitialize(VOID);
   virtual bool ExecuteTest(UINT);
   virtual bool DisplayFrame(VOID);
   virtual bool TestTerminate(VOID);
   virtual VOID MyCleanup();
   virtual bool ProcessFrame(void);
   virtual void CSCreateMutex(void);
   virtual void ProcessError(HRESULT hr);


   public:
   HANDLE gMutex;

   private:
   CFlags gFlags;
   USHORT gNumberOfDXVersions;
   ULONG gTestChangeIndex;
   ULONG gFocusChangeIndex;
   ULONG gResChangeIndex;
   PROCESS_INFORMATION gProcessInfo;
   bool	bProcessFrameFlag;

   //
   // This is not required in the new merge routine
   //
   #ifndef USE_NEW_MERGE
   ULONG m_nTests;   
   #endif
   
   bool IsLost(HRESULT hResult);
   bool CSwitcher::Validate(VOID);
   VOID CSwitcher::Delete(TCHAR *FileName);
   bool CSwitcher::Merge(VOID);
   bool CSwitcher::MergeHead(TCHAR *FileMerge, TCHAR *FileName);
   bool CSwitcher::MergeMiddle(TCHAR *FileMerge, TCHAR *FileName);
   bool CSwitcher::MergeTail(TCHAR *FileMerge, TCHAR *FileName);
  bool CSwitcher::FindString(PCHAR pBuff, PCHAR tszString);
};

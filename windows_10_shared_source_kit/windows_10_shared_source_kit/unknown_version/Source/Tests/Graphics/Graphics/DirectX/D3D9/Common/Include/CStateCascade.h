//
// CStateCascade.h - state cascading mechanism
//

#ifndef _CSTATECASCADE_H_
#define _CSTATECASCADE_H_

#include <windows.h>
#include <string.h>

typedef struct StateEntry
{
    StateEntry();
    ~StateEntry();
    void Cascade();
    void AddToList(struct StateEntry *);

    char    *pszKey;
    char    *pszName;
    int     nCurrentState;
    int     nStates;
    bool    bEnabled;

    struct StateEntry *pNext;
} STATEENTRY, *PSTATEENTRY;

class CStateCascade
{
public:
    CStateCascade();
    ~CStateCascade();
    
    void Cascade(int); // Cascades the state machine the number of times specified.
    void CascadeToState(int); // Zero based.  If you key off the framework's uiTest in ExecuteTest, pass in uiTest - 1

    bool AddEntry(const char *, const char *, int, bool); // Params are cszKey, cszName, nStates, bEnabled
    bool SetEntryState(const char *, int); // Params are cszKey, nState (where nState goes from 0 to numStates)
    int  GetEntryState(const char *); // Returns a value from 0 to numStates - 1, where numStates is set in AddEntry.
    int  GetEntryStateCount(const char *); // Returns a value equal to the number of states this entry can be in.
    bool SetEntryEnable(const char *, bool); // Enables or disables the state referenced by the passed key.
    int  StateCount(); // Gets the total number of unique states the machine can be in.  Useful for SetTestRange()

    STATEENTRY *GetEntry(const char *); // Used to get the StateEntry struct.
private:
    STATEENTRY  *m_pEntries;
};

#endif


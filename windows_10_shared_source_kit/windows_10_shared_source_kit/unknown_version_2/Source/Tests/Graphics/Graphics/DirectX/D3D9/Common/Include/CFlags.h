// FILE:        CFlags.h
// DESC:        flags class header
// AUTHOR:      Todd M. Frost

#ifndef _CFLAGS
#define _CFLAGS

class CFlags
{
    private:
        DWORD m_dwCurrent; // current flags
        DWORD m_dwSaved;   // saved flags

    public:
        CFlags(VOID)
        {
            m_dwCurrent = (DWORD) 0;
            m_dwSaved = (DWORD) 0;
        };
        ~CFlags(VOID)
        {
        };
        inline bool Check(DWORD dwFlags)
        {
            if (m_dwCurrent & dwFlags)
                return true;
            else
                return false;
        };
        inline VOID Clear(DWORD dwFlags)
        {
            m_dwCurrent &= ~dwFlags;
        };
        inline VOID Set(DWORD dwFlags)
        {
            m_dwCurrent |= dwFlags;
        };
        inline DWORD Get(VOID)
        {
            return m_dwCurrent;
        };
        inline VOID Save(VOID)
        {
            m_dwSaved = m_dwCurrent;
        };
        inline VOID Restore(VOID)
        {
            m_dwCurrent = m_dwSaved;
        };
        inline VOID Replace(DWORD dwFlags)
        {
            m_dwCurrent = dwFlags;
        };
        inline DWORD Saved(VOID)
        {
            return m_dwSaved;
        };
};

#endif

#ifndef __UNMANAGED_PRIMARY_SURFACE_H_
#define __UNMANAGED_PRIMARY_SURFACE_H_

/*
 * Unmanaged Primary Surface Creation / Destruction Module - Spec
 * This module selects a random output head, and then attempts to create a full-screen device
 * on that head.
 *
 * As spec'd this module needs to:
 * - Create a device on a random output head
 * - Periodically attempt to gain exclusive mode access to the head it created the device on.
 * - Periodically create and destroy unmanaged priary surfaces (both shared and unshared)
 * - Log erros if primary surface destruction or device creation fail
 */

class CUnmanagedPrimarySurface : public CTestComponent
{
public:
    CUnmanagedPrimarySurface();
    virtual void PreWalk(CLogWriter* pLog);

private:
    bool        m_Initialized;
    DWORD       m_NumOfDisplays;
    I3DDisplayVector    m_Win32DisplayInfo;


    void InitializeDataStructures();
    DWORD GetNumOfDisplays();
    DWORD SelectRandomDisplay();
};

void FatalErrorCallBack(I3D_FATAL_ERROR Error, const wchar_t* szExtraData, void* pUserData);


#endif
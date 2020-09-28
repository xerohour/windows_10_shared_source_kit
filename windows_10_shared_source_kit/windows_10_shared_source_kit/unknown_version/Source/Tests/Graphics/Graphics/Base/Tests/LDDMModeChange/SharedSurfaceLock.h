#ifndef __SHARED_SURFACE_LOCK_H_
#define __SHARED_SURFACE_LOCK_H_

/*
 * Shared Primary Surface Locking Component - Spec
 */

class CSharedSurfaceLock : public CGenericI3DComponent
{
public:
    CSharedSurfaceLock();
//    virtual void PreWalk(CLogWriter* pLog);

protected:
    virtual void RunTest();
    virtual void EndTest();

private:
    void ReadWriteMemory(D3DLOCKED_RECT d3dLocked_Rect, UINT BytesPerPixel, UINT SurfaceWidth, UINT SurfaceHeight);
};

#endif
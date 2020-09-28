#ifndef __CQUERY_H__
#define __CQUERY_H__

#include "Framework.h"

class CQuery : public CObject
{
    LPDIRECT3DQUERY9 m_pQuery9;

	CDevice *m_pDevice;

    HRESULT (CQuery::*pGetDevice)(CDevice** ppDevice);
    virtual HRESULT GetDevice9(CDevice** ppDevice);
    virtual HRESULT GetDevice8(CDevice** ppDevice);
    virtual HRESULT GetDevice7(CDevice** ppDevice);
    virtual HRESULT GetDevice6(CDevice** ppDevice);

    QUERYTYPE (CQuery::*pGetType)();
    virtual QUERYTYPE GetType9();
    virtual QUERYTYPE GetType8();
    virtual QUERYTYPE GetType7();
    virtual QUERYTYPE GetType6();

    DWORD (CQuery::*pGetDataSize)();
    virtual DWORD GetDataSize9();
    virtual DWORD GetDataSize8();
    virtual DWORD GetDataSize7();
    virtual DWORD GetDataSize6();

    HRESULT (CQuery::*pIssue)(DWORD dwIssueFlags);
    virtual HRESULT Issue9(DWORD dwIssueFlags);
    virtual HRESULT Issue8(DWORD dwIssueFlags);
    virtual HRESULT Issue7(DWORD dwIssueFlags);
    virtual HRESULT Issue6(DWORD dwIssueFlags);

    HRESULT (CQuery::*pGetData)(void* pData,DWORD dwSize,DWORD dwGetDataFlags);
    virtual HRESULT GetData9(void* pData,DWORD dwSize,DWORD dwGetDataFlags);
    virtual HRESULT GetData8(void* pData,DWORD dwSize,DWORD dwGetDataFlags);
    virtual HRESULT GetData7(void* pData,DWORD dwSize,DWORD dwGetDataFlags);
    virtual HRESULT GetData6(void* pData,DWORD dwSize,DWORD dwGetDataFlags);

    void (CQuery::*pReleaseD3D)();
    void ReleaseD3D() { (this->*pReleaseD3D)();}
    
	void ReleaseD3D9();
	void ReleaseD3D8();
    void ReleaseD3D7();
    void ReleaseD3D6();

	void ReleaseAll();
    
protected:
	virtual void *GetInternalPointer(void);

    bool Init9(LPVOID pQuery);
    bool Init8(LPVOID pQuery);
    bool Init7(LPVOID pQuery);
    bool Init6(LPVOID pQuery);

public:

    CQuery();
    ~CQuery();

    virtual ULONG   WINAPI AddRef(void);
    virtual ULONG   WINAPI Release(void);

    virtual bool Init(LPVOID pQuery, CDevice *pDevice);

    virtual HRESULT GetDevice(CDevice** ppDevice)
                        { return (this->*pGetDevice)(ppDevice); }
    virtual QUERYTYPE GetType()
                        { return (this->*pGetType)(); }
    virtual DWORD GetDataSize()
                        { return (this->*pGetDataSize)(); }
    virtual HRESULT Issue(DWORD dwIssueFlags)
                        { return (this->*pIssue)(dwIssueFlags); }
    virtual HRESULT GetData(void* pData,DWORD dwSize,DWORD dwGetDataFlags)
                        { return (this->*pGetData)(pData, dwSize, dwGetDataFlags); }
};

// -------------------------------------------------------------------------------------


//
// CnQuery - used to tell framework to handle lists of swapchains into wrapper calls
//
class CnQuery : public CObject
{
private:
	CQuery** m_pQueries;
	UINT     m_uDevices;
public:
	~CnQuery();
    virtual bool Init(UINT uDevices);
	virtual bool GetQuery(CQuery **ppQuery, UINT n);
	virtual bool SetQuery(CQuery *pQuery, UINT n);

	//virtual bool GetDevice(CnDevice* ppDevice)
	virtual QUERYTYPE GetType();
    virtual DWORD GetDataSize();
    virtual bool Issue(DWORD dwIssueFlags);
	virtual HRESULT GetData(void* pData,DWORD dwSize,DWORD dwGetDataFlags);
};

#endif


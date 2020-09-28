#include "cfgmgr32.h"
#include "wmium.h"
#include <tchar.h>

template<class T> class CWmiInstance 
{

public:
    CWmiInstance() :
      m_hWmiBlock(INVALID_HANDLE_VALUE),
      m_WmiInstDataSize(0), m_pWmiInstData(NULL), m_Version(0)
    {
        m_DevInstId[0] = _T('\0');
    }

    virtual ~CWmiInstance()
    {
        Close();
    }

    BOOL IsOpened()
    {
        return INVALID_HANDLE_VALUE != m_hWmiBlock;
    }

    BOOL Get(T** ppT)
    {
        if(ppT == NULL)
		{
			return FALSE;
		}
		if (IsOpened()) {
            PWNODE_SINGLE_INSTANCE pSingle;
            ULONG size;
            //ULONG SingleSize;
            DWORD error;

            size = m_WmiInstDataSize;

            while (1) 
            {
                error = WmiQuerySingleInstance(
                    m_hWmiBlock, m_DevInstId, &size, m_pWmiInstData);

                switch (error) 
                {
                case ERROR_INSUFFICIENT_BUFFER:
                    delete[] m_pWmiInstData;

                    m_pWmiInstData = new BYTE[size];

                    if (m_pWmiInstData == NULL) 
                    {
                        return FALSE;
                    }

                    m_WmiInstDataSize = size;
                    continue;

                case ERROR_SUCCESS:
                    pSingle = (PWNODE_SINGLE_INSTANCE) m_pWmiInstData;

                    if (size == m_WmiInstDataSize)
					{
						//SingleSize = sizeof(T);
						//if(SingleSize = pSingle->SizeDataBlock)
						{
							m_Version = pSingle->WnodeHeader.Version;
							*ppT = ((T *)(m_pWmiInstData + pSingle->DataBlockOffset));
							return TRUE;
						}
                    }
                    return FALSE;

                default:
                    return FALSE;
                }
            }
        }

        return FALSE;
    }

    BOOL Set(const T& TObject)
    {
        if (IsOpened()) 
        {
            DWORD error;
            T newT = TObject;

            error = WmiSetSingleInstance(m_hWmiBlock,
                                         m_DevInstId,
                                         m_Version,
                                         sizeof(newT),
                                         &newT);

            // get the value back to see if the change is really succeeded.
            if (ERROR_SUCCESS == error) 
            {
                return TRUE;
            }
        }

        return FALSE;
    }

    BOOL SetItem(ULONG DataItemId, PVOID Data, ULONG DataLength)
    {
        if (IsOpened()) 
        {
            ULONG res;

            res = WmiSetSingleItem(m_hWmiBlock,
                                   m_DevInstId,
                                   DataItemId,
                                   0,
                                   DataLength,
                                   Data);

            if (res == ERROR_SUCCESS) 
            {
                return TRUE;
            }
        }

        return FALSE;
    }

    BOOL ExecuteMethod(ULONG MethodId,
                       PVOID InputBuffer,
                       ULONG InputLength,
                       PVOID OutputBuffer,
                       PULONG OutputLength)
    {
        if (IsOpened()) 
        {
            ULONG res;

            res = WmiExecuteMethod(m_hWmiBlock,
                                   m_DevInstId,
                                   MethodId,
                                   InputLength,
                                   InputBuffer,
                                   OutputLength,
                                   OutputBuffer);

            if (res == ERROR_SUCCESS) 
            {
                return TRUE;
            }
        }

        return FALSE;
    }

    BOOL Open(LPCTSTR DeviceId, const GUID& Guid, BOOL IdCooked = FALSE)
    {
        if (DeviceId == NULL) 
        {
            return FALSE;
        }

        // do nothing if already opened
        if (IsOpened()) 
        {
            return TRUE;
        }

        HRESULT hr = S_OK;

        //
        // WMI requires an instance number. We simply
        // append a '_0' here because we are only interested in
        // the device as a whole. A mouse can have one instance for
        // left button, one for the left button and one for
        // the movement although this arrangement is not pratical
        // at all. If the device plans to do so, the instance 0
        // should be reserved to control the device as a whole.
        //
        int len = lstrlen(DeviceId);

        if (len >= (sizeof(m_DevInstId)/sizeof(m_DevInstId[0])) - 2) 
        {
            OutputDebugString(TEXT("not enough size\n"));
            return FALSE;
        }

        if (IdCooked == FALSE) 
        {
            //
            // BUGBUG:  should really use WmiDevInstToInstanceName instead!
            //
            hr = StringCchCopyN(m_DevInstId, ARRAYSIZE(m_DevInstId), DeviceId, len + 1);
            if(FAILED(hr))
            {
                return FALSE;
            }

            m_DevInstId[len] = _T('_');
            m_DevInstId[len + 1] = _T('0');
            m_DevInstId[len + 2] = _T('\0');
        }
        else 
        {
            hr = StringCchCopy(m_DevInstId, ARRAYSIZE(m_DevInstId), DeviceId);
            if(FAILED(hr))
            {
                return FALSE;
            }
        }

        ULONG error;
        GUID guid = Guid;

        error = WmiOpenBlock(&guid, 0, &m_hWmiBlock);

        if (error == ERROR_SUCCESS) 
        {
            return TRUE;
        }
        else 
        {
            SetLastError(error);
            return FALSE;
        }
    }

    BOOL Close()
    {
        if (INVALID_HANDLE_VALUE != m_hWmiBlock) 
        {
            WmiCloseBlock(m_hWmiBlock);
        }

        m_hWmiBlock = INVALID_HANDLE_VALUE;
        m_DevInstId[0] = _T('\0');

        if (m_pWmiInstData) 
        {
            delete [] m_pWmiInstData;
            m_pWmiInstData = NULL;
        }

        m_WmiInstDataSize = 0;

        return TRUE;
    }

    WMIHANDLE GetWmiHandle()
    {
        return m_hWmiBlock;
    }

protected:
    WMIHANDLE   m_hWmiBlock;
    ULONG       m_WmiInstDataSize;
    ULONG       m_Version;
    BYTE*       m_pWmiInstData;
    TCHAR       m_DevInstId[MAX_DEVICE_ID_LEN + 2];
};

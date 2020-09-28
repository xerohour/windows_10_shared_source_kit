#pragma once

///////////////////////////////////////////////////////////////////////////////////////
//		CWMIHelper class
///////////////////////////////////////////////////////////////////////////////////////
class CWMIHelper
{
protected:
	IWbemLocator* m_pLocator;
	IWbemServices* m_pServices;
	BOOL m_bInitialized;

public:
	CWMIHelper();
	~CWMIHelper();
	HRESULT Init(LPCTSTR szNamespace, LPCTSTR);
	IWbemServices* GetWMIServices(){return m_pServices;};

	HRESULT GetWmiObject(IWbemClassObject** ppWMIObject, LPCTSTR szClassName, LPCTSTR szInstanceName);
};
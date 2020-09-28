#pragma once

// Includes
#include "D3DWDDM.h"

////////////////////////
// Encapsulate an Adapter with it's descriptors for easy access

class CAdapter
{
public:
	// Constructors/Destructors
	CAdapter(IDXGIAdapter1 *pAdapter);
	virtual  ~CAdapter();
	
	// Accessors
	bool IsValid() const;
	DevX::GRFX::AdapterDescriptorCore * GetDescriptor() const { return m_pAdapterDescriptor; }
	LARGE_INTEGER GetUserModeDriverVersion() const { return m_userModeDriverVersion; }
	tstring GetUserModeDriverVersionStr() const { return CAdapter::GetUserModeDriverVersionStr( m_userModeDriverVersion ); }
	static tstring GetUserModeDriverVersionStr( const LARGE_INTEGER &m_userModeDriverVersion );
	ATL::CComPtr<IDXGIAdapter1> GetAdapter() const { return m_pAdapter1; }

protected:
	void Setup(IDXGIAdapter1 *pAdapter);
	void Cleanup();

private:
	ATL::CComPtr<IDXGIAdapter1> m_pAdapter1;
	DevX::GRFX::AdapterDescriptorCore *m_pAdapterDescriptor;
	LARGE_INTEGER m_userModeDriverVersion;
};
#pragma once

#include "D3D11Test.h"
#include "new_off.h"
#include "d3dx10.h"
#include "new_on.h"

#include <WGFTestCore.h>

class CDepthStencilTestApp : public CD3D11TestFramework
{
private:
	TESTTYPE	GetTestType( ) const { return TESTTYPE_CONF; }
	void		InitOptionVariables( );
	bool		InitTestGroups( );

protected:
	bool		m_bLogShaders;
	bool		m_bPresent;

public:
	CDepthStencilTestApp() : m_bPresent(false) {}
	inline void		Present() { if ( m_bPresent ) CD3D11TestFramework::Present( 0, 0, 0, 0, 0, 0 );	}
	bool			IsDebugRuntime( ) const { return m_D3DOptions.Debug; }
	bool			IsLoggingShaders( ) const { return m_bLogShaders; }
	D3D_DRIVER_TYPE	GetSrcDriverType( ) const { return m_D3DOptions.DriverType; }
};

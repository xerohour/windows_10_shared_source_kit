er.
//
// This is what happens when people start throwing
// features into a test framework without clearly and
// rigidly thinking through and specifying the expected
// semantics, or the interactions with other features.
///////////////////////////////////////////////////////
bool CD3D::ResolveBehaviors9
(
	IN UINT Adapter,
	IN DWORD DeviceType,
	IN DWORD fBehaviors,
	__out DWORD * pfResolvedBehaviors
)
{
	//
	// Validate parameters
	//

	if ( pfResolvedBehaviors == NULL )
	{
		DPF(1, _T("CD3D::ResolveBehaviors() - invalid pointer.\n"));
		return false;
	};

	CAPS DevCaps;
	HRESULT hr = GetDeviceCaps9(Adapter, DeviceType, &DevCaps);
	if(FAILED(hr))
	{
		DPF(1, _T("CD3D::ResolveBehaviors() - GetDeviceCaps() failed. hr=%s(%x)\n"), HResultToString(hr), hr);
		return false;
	};


	DWORD ResolvedBehaviors = fBehaviors;


	//
	// Decode custom framework behavior flags.
	//
	// N.B. [SEdmison]:
	// Essentially, we're resolving *this-or-that* aliases to
	// the particular *this* or *that*, so that what we return
	// to the framework will be an unambiguous set of behavior
	// flags.
	//

	if ( (DEVICECREATE_SW_OR_HW_VP & ResolvedBehaviors) != 0 )
	{
		ResolvedBehaviors &= ~DEVICECREATE_SW_OR_HW_VP;

		// Assume HWVP.  If the device can't do HWTRANSFORMANDLIGHT,
		// then this will get overridden below.
		if((DevCaps.DeviceType == D3DDEVTYPE_HAL) != 0)
		{
			ResolvedBehaviors |= DEVICECREATE_HWVP;
		}
		else
		{
			ResolvedBehaviors |= DEVICECREATE_SWVP;
		};


		// Determine whether the device can do hardware vertex processing.
		if
		(
			(DevCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0
		)
		{
			ResolvedBehaviors |= DEVICECREATE_SWVP;
			ResolvedBehaviors &= ~DEVICECREATE_HWVP;
			ResolvedBehaviors &= ~DEVICECREATE_MIXED;
		};
	};


	if ( (DEVICECREATE_HWVP_OR_PURE & ResolvedBehaviors) != 0 )
	{
		ResolvedBehaviors &= ~DEVICECREATE_HWVP_OR_PURE;

		if((DevCaps.DevCaps & D3DDEVCAPS_PUREDEVICE) != 0)
		{
			ResolvedBehaviors &= ~DEVICECREATE_STANDARD;
			ResolvedBehaviors &= ~DEVICECREATE_MIXED;
			ResolvedBehaviors |= DEVICECREATE_PURE | DEVICECREATE_HWVP;
		}
		else
		{
			ResolvedBehaviors &= ~DEVICECREATE_PURE;
			ResolvedBehaviors |= DEVICECREATE_STANDARD | DEVICECREATE_HWVP;
		};
	};
	

	if ( (DEVICECREATE_BESTAVAILABLE & ResolvedBehaviors) != 0 )
	{
		ResolvedBehaviors &= ~DEVICECREATE_BESTAVAILABLE;

		// Assume HWVP.  If the device can't do HWTRANSFORMANDLIGHT,
		// then this will get overridden below.
		if ( (DevCaps.DevCaps & D3DDEVCAPS_PUREDEVICE) != 0 )
		{
			ResolvedBehaviors &= ~DEVICECREATE_STANDARD;
			ResolvedBehaviors &= ~DEVICECREATE_MIXED;
			ResolvedBehaviors |= DEVICECREATE_PURE | DEVICECREATE_HWVP;
		}
		else
		{
			ResolvedBehaviors &= ~DEVICECREATE_PURE;
			ResolvedBehaviors |= DEVICECREATE_STANDARD | DEVICECREATE_HWVP;
		};


		// Determine whether the device can do hardware vertex processing.
		if
		(
			(DevCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) == 0
		)
		{
			ResolvedBehaviors |= DEVICECREATE_SWVP;
			R
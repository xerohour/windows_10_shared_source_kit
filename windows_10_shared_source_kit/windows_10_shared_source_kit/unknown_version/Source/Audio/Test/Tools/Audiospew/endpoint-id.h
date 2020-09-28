// endpoint-id.h

// given any of:
// 1. "console", "communications", or "multimedia"
// 2. a WASAPI endpoint ID
// 3. a PKEY_AudioEndpoint_GUID
// 4. an endpoint friendly name
//
// return a WASAPI endpoint ID.
// the caller must CoTaskMemFree(*out) on success.
HRESULT FindWasapiEndpointId(EDataFlow flow, LPCWSTR in, _Outptr_ LPWSTR *out);
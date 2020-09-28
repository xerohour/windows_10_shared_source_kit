// Copyright(C) Microsoft. All rights reserved.

class CEndpointStreamingPropertyTests
{

public:

    CEndpointStreamingPropertyTests( IMMDevice* pMMDevice );

    DWORD RunHardwareAccelerationTest_OFF();
    DWORD RunHardwareAccelerationTest_ON();

    DWORD RunSystemEffectsTest( bool bEnabled );

private:

    HRESULT GetHardwareAccelerationSetting( bool& bAllowed );
    HRESULT SetHardwareAccelerationSetting( bool bAllowed );
    HRESULT ClearHardwareAccelerationSetting();

    HRESULT GetDisableSystemEffectsSetting( bool& bAllowed );
    HRESULT SetDisableSystemEffectsSetting( bool bAllowed );
    HRESULT ClearDisableSystemEffectsSetting();

    HRESULT InitializeInSharedMode( IAudioClient2* pAudioClient2, bool& isInitialized );
    HRESULT GetLfxEnable( IAudioClient2* pAudioClient2, UINT uKsAENodeId, bool& bLfxEnable );
    HRESULT GetGfxEnable( LPCWSTR pwszAdapter, UINT uKsAENodeId, bool& bGfxEnable );

    CComPtr<IMMDevice> m_spMMDevice;
    CComPtr<IAudioClient2> m_spAudioClient2;

};

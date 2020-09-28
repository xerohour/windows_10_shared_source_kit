res);
    static D3D10_REQUIREMENTS                   CPULockable(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   MultisampleResolveSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   DisplayScanOutSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   CastWithinBitLayout(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static BOOL                                 DX9VertexOrIndexFormat(DXGI_FORMAT Format);
    static BOOL                                 DX9TextureFormat(DXGI_FORMAT Format);
	static BOOL									FloatNormTextureFormat(DXGI_FORMAT Format);
	static BOOL									Planar(DXGI_FORMAT Format);
	
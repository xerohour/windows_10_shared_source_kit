if (D3D_OK != hr)
            break;

        //  create device
        PresentParams.BackBufferWidth = 640;
        PresentParams.BackBufferHeight = 480;
        PresentParams.BackBufferFormat = D3DFMT_UNKNOWN;
        PresentParams.BackBufferCount = 0;
        PresentParams.MultiSampleType = D3DMULTISAMPLE_NONE;
        PresentParams.SwapEffect = D3DSWAPEFFECT_DISCARD;
        PresentParams.hDeviceWindow = hDeviceWnd;
        PresentParams.Windowed = bWindowed;
        PresentParams.Flags = 0;
        PresentParams.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

        hr = pD3D9->CreateDevice(nIndex, D3DDEVTYPE_HAL, NULL, 
          
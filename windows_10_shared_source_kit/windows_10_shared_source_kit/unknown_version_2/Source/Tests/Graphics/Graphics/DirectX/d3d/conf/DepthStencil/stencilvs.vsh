 dwGradColors[0] = D3DRGBA( 0, 0, 0, 1);
    dwGradColors[1] = D3DRGBA(0.117, 1.0, 0.55, 1);
    dwGradColors[2] = D3DRGBA(0.08, 1.0, 0.38, 1);
    dwGradColors[3] = D3DRGBA(0, 1, 0, 1);
    if ( !m_TexImages[TEX_GRADIENT_FOR_BUMP].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_GRADIENT_FOR_BUMP gradient data.  Aborting.\n");
        return false;
    }

	//TEX_GRADIENT_FOR_BUMP2
    dwGradColors[0] = D3DRGBA( 0, 0, 0, 1);
    dwGradColors[1] = D3DRGBA(1, 0, 0, 1);
    dwGradCo
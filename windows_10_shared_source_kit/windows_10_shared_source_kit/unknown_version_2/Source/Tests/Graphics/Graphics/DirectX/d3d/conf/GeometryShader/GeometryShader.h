lors[2] = D3DRGBA(0, 1, 0, 1);
    dwGradColors[3] = D3DRGBA(0, 0, 1, 1);
    if ( !m_TexImages[TEX_GRADIENT_FOR_BUMP2].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_GRADIENT_FOR_BUMP2 gradient data.  Aborting.\n");
        return false;
    }

	////////////////////////CImage for Tex Mips<Cube,Vol///////////////

	//TEX_MIPS
    dwGradColors[0] = D3DRGBA( 1, 0, 0, 0.75f);
    dwGradColors[1] = D3DRGBA( 1, 0, 0, 0.75f);
    dwGradColors[2] = D3DRGBA( 1, 0, 0, 1);
    dwGradColors[3] = D3DRGBA( 1, 0, 0, 1);
    if ( !m_TexImages[TEX_MIPS].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_MIPS gradient data.  Aborting.\n");
        return false;
    }

	//TEX_VOLUME
    dwGradColors[0] = D3DRGBA( 1, 1, 1, 1);
    dwGradColors[1] = D3DRGBA( 1, 1, 1, 1);
    dwGradColors[2] = D3DRGBA( 1, 1, 1, 0.75f);
    dwGradColors[3] = D3DRGBA( 1, 1, 1, 0.75f);
    if ( !m_TexImages[TEX_VOLUME].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_VOLUME gradient data.  Aborting.\n");
        return false;
    }

	//TEX_CUBE
    dwGradColors[0] = D3DRGBA( 1, 1, 0, 0.75f);
    dwGradColors[1] = D3DRGBA( 1, 1, 0, 0.75f);
    dwGradColors[2] = D3DRGBA( 1, 1, 0, 1);
    dwGradColors[3] = D3DRGBA( 1, 1, 0, 1);
    if ( !m_TexImages[TEX_CUBE].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_CUBE gradient data.  Aborting.\n");
        return false;
    }

	//TEX_MIPSVOLUMECUBE_1
    dwGradColors[0] = D3DRGBA(1, 0, 1, 1);
    dwGradColors[1] = D3DRGBA(1, 0, 1, 1);
    dwGradColors[2] = D3DRGBA(1, 0, 1, 0.75f);
    dwGradColors[3] = D3DRGBA(1, 0, 1, 0.75f);
    if ( !m_TexImages[TEX_MIPSVOLUMECUBE_1].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_MIPSVOLUMECUBE_1 gradient data.  Aborting.\n");
        return false;
    }

	//TEX_MIPSVOLUMECUBE_2
    dwGradColors[0] = D3DRGBA(1, 0.1176f, 0.55f, 0.75f);
    dwGradColors[1] = D3DRGBA(1, 0.1176f, 0.55f, 0.75f);
    dwGradColors[2] = D3DRGBA(1, 0.1176f, 0.55f, 1);
    dwGradColors[3] = D3DRGBA(1, 0.1176f, 0.55f, 1);
    if ( !m_TexImages[TEX_MIPSVOLUMECUBE_2].LoadGradient(DEFAULT_TEXWIDTH, DEFAULT_TEXHEIGHT, dwGradColors) )
    {
        WriteToLog("(ERROR) Failed to create TEX_MIPSVOLUMECUBE_2 gradient data.  Aborting.\n");
        return false;
    }

	//TEX_MIPSVOLUMECUBE_3
    dwGradColors[0] = D3DRGBA( 0, 1, 1, 1);
    dwGradColo
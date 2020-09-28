         }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
       	    sprintf(msgString,"%sFilter: PPN, ",msgString);
            break;
        case 1:
        case 5:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))       
            {
                WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
       	    sprintf(msgString,"%sFilter: LPN, ",msgString);
            break;
        case 2:
        case 6:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
        	sprintf(msgString,"%sFilter: PLN, ",msgString);
            break;
        case 3:
        case 7:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))       
            {
                WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_NONE);
        	sprintf(msgString,"%sFilter: LLN, ",msgString);
            break;
        case 8:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MipFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
        	sprintf(msgString,"%sFilter: PPP, ",msgString);
            break;
        case 9:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MipFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_LINEAR);
            SetTextureStageState(0,D3DTSS_MIPFILTER,(DWORD)TEXF_POINT);
        	sprintf(msgString,"%sFilter: LLP, ",msgString);
            break;
        case 10:
            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MINFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MinFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MAGFPOINT))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MagFPoint.\n");
                return D3DTESTINIT_ABORT;
            }

            if (!(dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))       
            {
	            WriteToLog("\nDevice capability not supported: VolumeTextureFilter MipFLinear.\n");
                return D3DTESTINIT_SKIPALL;
            }

            SetTextureStageState(0,D3DTSS_MINFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0,D3DTSS_MAGFILTER,(DWORD)TEXF_POINT);
            SetTextureStageState(0
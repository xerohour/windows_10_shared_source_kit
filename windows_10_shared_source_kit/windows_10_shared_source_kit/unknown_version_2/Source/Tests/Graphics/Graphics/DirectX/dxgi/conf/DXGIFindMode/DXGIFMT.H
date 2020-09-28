                                  UCX_TAG);

        if (stringBuffer == NULL) {
            LogError(FlagRootHub, "Unable to allocate location strings");

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }
        
        RtlZeroMemory(stringBuffer, stringBufferLen);

        //
        // The location string for a USB de
// Copyright(C) Microsoft. All rights reserved.

#include <stdafx.h>
#include "TestParams.h"

namespace WAVEFORMATEX_PARAM_SPACE {

    UINT32 getInvalidCasesCount() 
    { 
        return sizeof(g_WaveCornerTestCases)/sizeof(g_WaveCornerTestCases[0]); 
    }

    WAVEFORMATEX getInvalidCase(UINT32 i)  
    {
        return g_WaveCornerTestCases[i]; 
    }

    UINT32 getValidCasesCount() 
    { 
        return sizeof(g_WaveTestCases)/sizeof(g_WaveTestCases[0]); 
    }

    WAVEFORMATEX getValidCase(UINT32 i)  
    { 
        return g_WaveTestCases[i]; 
    }

    UINT32 getValidSpecialCasesCount() 
    { 
        return sizeof(g_WaveTestCasesSpecial)/sizeof(g_WaveTestCasesSpecial[0]); 
    }

    WAVEFORMATEX getValidSpecialCase(UINT32 i)  
    { 
        return g_WaveTestCasesSpecial[i]; 
    }

    UINT32 getInvalidExtCasesCount() 
    { 
        return sizeof(g_WaveExtCornerTestCases)/sizeof(g_WaveExtCornerTestCases[0]); 
    }

    WAVEFORMATEXTENSIBLE getInvalidExtCase(UINT32 i)  
    { 
        return g_WaveExtCornerTestCases[i]; 
    }

    UINT32 getValidExtCasesCount() 
    { 
        return sizeof(g_WaveExtTestCases)/sizeof(g_WaveTestCases[0]); 
    }

    WAVEFORMATEXTENSIBLE getValidExtCase(UINT32 i)  
    { 
        return g_WaveExtTestCases[i]; 
    }

}

namespace ISFORMATSUPPORTED_PARAM_SPACE {

    UINT32 getInvalidParamCasesCount() 
    { 
        return sizeof( g_IsFormatSupportedInvalidParams )/ sizeof( g_IsFormatSupportedInvalidParams[0] ); 
    }

    ISFORMATSUPPORTED_PARAMS getInvalidParamCase(UINT32 i) 
    { 
        return g_IsFormatSupportedInvalidParams[i]; 
    }

    UINT32 getInvalidCasesCount() 
    { 
        return WAVEFORMATEX_PARAM_SPACE::getInvalidCasesCount(); 
    }

    ISFORMATSUPPORTED_PARAMS getInvalidCase(UINT32 i) 
    { 
        return ISFORMATSUPPORTED_PARAMS( WAVEFORMATEX_PARAM_SPACE::g_WaveCornerTestCases[i] ); 
    }

    UINT32 getInvalidCasesExtCount() 
    { 
        return WAVEFORMATEX_PARAM_SPACE::getInvalidExtCasesCount(); 
    }

    ISFORMATSUPPORTED_PARAMS getInvalidCaseExt(UINT32 i) 
    { 
        return ISFORMATSUPPORTED_PARAMS( WAVEFORMATEX_PARAM_SPACE::g_WaveExtCornerTestCases[i] ); 
    }
    
}

namespace INITIALIZE_PARAM_SPACE {

    UINT32 getInvalidParamCasesCount() 
    { 
        return sizeof( g_InitializeInvalidParams )/ sizeof( g_InitializeInvalidParams[0] ); 
    }

    INITIALIZE_PARAMS getInvalidParamCase(UINT32 i) 
    { 
        return g_InitializeInvalidParams[i]; 
    }

    UINT32 getInvalidCasesCount() 
    { 
        return WAVEFORMATEX_PARAM_SPACE::getInvalidCasesCount(); 
    }

    INITIALIZE_PARAMS getInvalidCase(UINT32 i, AudioClientType type, AUDCLNT_SHAREMODE mode, DWORD dwFlags) 
    { 
        return INITIALIZE_PARAMS( WAVEFORMATEX_PARAM_SPACE::g_WaveCornerTestCases[i],  mode, dwFlags, type); 
    }

    UINT32 getInvalidCasesExtCount() 
    { 
        return WAVEFORMATEX_PARAM_SPACE::getInvalidExtCasesCount(); 
    }

    INITIALIZE_PARAMS getInvalidCaseExt(UINT32 i, AudioClientType type, AUDCLNT_SHAREMODE mode, DWORD dwFlags) { 
        return INITIALIZE_PARAMS( WAVEFORMATEX_PARAM_SPACE::g_WaveExtCornerTestCases[i],  mode, dwFlags, type); 
    }

}

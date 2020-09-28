// ----------------------------------------------------------------------------------
// Test Cases
// ----------------------------------------------------------------------------------

// STANDARD FORMATS
DWORD Test_Latency          (void);
DWORD Test_Starve           (void);
DWORD Test_DurationLength   (void);
DWORD Test_Progression      (void);
DWORD Test_DriftAndJitter   (void);
DWORD Test_ReadPacket       (void);
DWORD Test_KeywordBurstRead (void);
DWORD Test_GetPosition      (void);
DWORD Test_SetGetPosition   (void);
#ifndef _WIN64
DWORD Test_MothraVsGodzilla (void);
#endif
DWORD Test_GetPositionMult  (void);
DWORD Test_WritePacket      (void);
DWORD Test_DriftAndJitter_ForAEC   (void);

DWORD Test_ERT_PCMStartLatency  (void);
DWORD Test_ERT_PauseLatency     (void);
DWORD Test_ERT_StopLatency      (void);
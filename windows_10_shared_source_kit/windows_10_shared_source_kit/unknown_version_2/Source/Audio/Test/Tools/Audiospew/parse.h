// parse.h

_Success_(return == true)
bool ParseWaveFormat(
    EDataFlow flow,
    LPCWSTR endpoint,
    _Inout_ int *pIndex,
    int argc,
    _In_reads_(argc) LPCWSTR argv[],
    _Out_ WAVEFORMATEX **ppWfx
);

//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3D10WrappedGlobals.cpp
//* 
//* Description: 
//*     Declare global methods so that we don't need to link with d3d10.lib
//*
//*****************************************************************************

// Ignore Prefast warnings 26060 (Range Precondition violation) and 28930 (Unused assignment of pointer variable)
#pragma warning( disable: 26060 )
#pragma warning( disable: 28930 )

#include "D3D10WrappedGlobals.h"
#include <WGFTestCore.h>

//////////////////////////////////
// Typedefs

typedef     HRESULT (WINAPI StateBlockMaskUnion)(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
typedef     HRESULT (WINAPI StateBlockMaskIntersect)(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
typedef     HRESULT (WINAPI StateBlockMaskDifference)(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
typedef     HRESULT (WINAPI StateBlockMaskEnableCapture)(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength);
typedef     HRESULT (WINAPI StateBlockMaskDisableCapture)(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength);
typedef     HRESULT (WINAPI StateBlockMaskEnableAll)(D3D10_STATE_BLOCK_MASK *pMask);
typedef     HRESULT (WINAPI StateBlockMaskDisableAll)(D3D10_STATE_BLOCK_MASK *pMask);
typedef     BOOL    (WINAPI StateBlockMaskGetSetting)(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT Entry);
typedef     HRESULT (WINAPI CreateStateBlock)(ID3D10Device *pDevice, D3D10_STATE_BLOCK_MASK *pStateBlockMask, ID3D10StateBlock **ppStateBlock);
typedef     HRESULT (WINAPI CompileEffectFromMemory)(void *pData, SIZE_T DataLength, LPCSTR pSrcFileName, CONST D3D10_SHADER_MACRO *pDefines, ID3D10Include *pInclude, UINT HLSLFlags, UINT FXFlags, ID3D10Blob **ppCompiledEffect, ID3D10Blob **ppErrors);
typedef     HRESULT (WINAPI CreateEffectFromMemory)(void *pData, SIZE_T DataLength, UINT FXFlags, ID3D10Device *pDevice, ID3D10EffectPool *pEffectPool, ID3D10Effect **ppEffect);
typedef     HRESULT (WINAPI CreateEffectPoolFromMemory)(void *pData, SIZE_T DataLength, UINT FXFlags, ID3D10Device *pDevice, ID3D10EffectPool **ppEffectPool);
typedef     HRESULT (WINAPI DisassembleEffect)(ID3D10Effect *pEffect, BOOL EnableColorCode, ID3D10Blob **ppDisassembly);
typedef     HRESULT (WINAPI CompileShader)(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, ID3D10Include* pInclude, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs);
typedef     HRESULT (WINAPI DisassembleShader)(CONST void *pShader, SIZE_T BytecodeLength, BOOL EnableColorCode, LPCSTR pComments, ID3D10Blob** ppDisassembly);
typedef     LPCSTR (WINAPI GetPixelShaderProfile)(ID3D10Device *pDevice);
typedef     LPCSTR (WINAPI GetVertexShaderProfile)(ID3D10Device *pDevice);
typedef     LPCSTR (WINAPI GetGeometryShaderProfile)(ID3D10Device *pDevice);
typedef     HRESULT (WINAPI ReflectShader)(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10ShaderReflection **ppReflector);
typedef     HRESULT (WINAPI PreprocessShader)(LPCSTR pSrcData, SIZE_T SrcDataSize, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, ID3D10Include* pInclude, ID3D10Blob** ppShaderText, ID3D10Blob** ppErrorMsgs);
typedef     HRESULT (WINAPI GetInputSignatureBlob)(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
typedef     HRESULT (WINAPI GetOutputSignatureBlob)(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
typedef     HRESULT (WINAPI GetInputAndOutputSignatureBlob)(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
typedef     HRESULT (WINAPI GetShaderDebugInfo)(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob** ppDebugInfo);
typedef     HRESULT (WINAPI CreateDevice)( IDXGIAdapter *pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, UINT SDKVersion, ID3D10Device **ppDevice);
typedef     HRESULT (WINAPI CreateDeviceAndSwapChain)( IDXGIAdapter *pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, UINT SDKVersion, DXGI_SWAP_CHAIN_DESC *pSwapChainDesc, IDXGISwapChain **ppSwapChain, ID3D10Device **ppDevice);
typedef     HRESULT (WINAPI CreateBlob)(SIZE_T NumBytes, ID3D10Blob* *ppBuffer);

//////////////////////////////////
// Wrapped methods

HRESULT WINAPI D3DWrappedStateBlockMaskUnion(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    StateBlockMaskUnion *pProc = (StateBlockMaskUnion *) GetProcAddress( hD3D10, "D3D10StateBlockMaskUnion" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10StateBlockMaskUnion - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pA, pB, pResult);
}
HRESULT WINAPI D3DWrappedStateBlockMaskIntersect(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    StateBlockMaskIntersect *pProc = (StateBlockMaskIntersect *) GetProcAddress( hD3D10, "D3D10StateBlockMaskIntersect" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10StateBlockMaskIntersect - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pA, pB, pResult);
}
HRESULT WINAPI D3DWrappedStateBlockMaskDifference(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    StateBlockMaskDifference *pProc = (StateBlockMaskDifference *) GetProcAddress( hD3D10, "D3D10StateBlockMaskDifference" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10StateBlockMaskDifference - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pA, pB, pResult);
}
HRESULT WINAPI D3DWrappedStateBlockMaskEnableCapture(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    StateBlockMaskEnableCapture *pProc = (StateBlockMaskEnableCapture *) GetProcAddress( hD3D10, "D3D10StateBlockMaskEnableCapture" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10StateBlockMaskEnableCapture - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pMask, StateType, RangeStart, RangeLength);
}
HRESULT WINAPI D3DWrappedStateBlockMaskDisableCapture(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    StateBlockMaskDisableCapture *pProc = (StateBlockMaskDisableCapture *) GetProcAddress( hD3D10, "D3D10StateBlockMaskDisableCapture" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10StateBlockMaskDisableCapture - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pMask, StateType, RangeStart, RangeLength);
}
HRESULT WINAPI D3DWrappedStateBlockMaskEnableAll(D3D10_STATE_BLOCK_MASK *pMask)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    StateBlockMaskEnableAll *pProc = (StateBlockMaskEnableAll *) GetProcAddress( hD3D10, "D3D10StateBlockMaskEnableAll" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10StateBlockMaskEnableAll - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pMask);
}
HRESULT WINAPI D3DWrappedStateBlockMaskDisableAll(D3D10_STATE_BLOCK_MASK *pMask)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    StateBlockMaskDisableAll *pProc = (StateBlockMaskDisableAll *) GetProcAddress( hD3D10, "D3D10StateBlockMaskDisableAll" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10StateBlockMaskDisableAll - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pMask);
}
BOOL    WINAPI D3DWrappedStateBlockMaskGetSetting(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT Entry)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return FALSE;
    }
    
    StateBlockMaskGetSetting *pProc = (StateBlockMaskGetSetting *) GetProcAddress( hD3D10, "D3D10StateBlockMaskGetSetting" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10StateBlockMaskGetSetting - Function not found in D3D10.dll" ) );
        return FALSE;
    }
    
    return pProc(pMask, StateType, Entry);
}
HRESULT WINAPI D3DWrappedCreateStateBlock(ID3D10Device *pDevice, D3D10_STATE_BLOCK_MASK *pStateBlockMask, ID3D10StateBlock **ppStateBlock)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    CreateStateBlock *pProc = (CreateStateBlock *) GetProcAddress( hD3D10, "D3D10CreateStateBlock" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10CreateStateBlock - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pDevice, pStateBlockMask, ppStateBlock);
}
HRESULT WINAPI D3DWrappedCompileEffectFromMemory(void *pData, SIZE_T DataLength, LPCSTR pSrcFileName, CONST D3D10_SHADER_MACRO *pDefines, ID3D10Include *pInclude, UINT HLSLFlags, UINT FXFlags, ID3D10Blob **ppCompiledEffect, ID3D10Blob **ppErrors)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    CompileEffectFromMemory *pProc = (CompileEffectFromMemory *) GetProcAddress( hD3D10, "D3D10CompileEffectFromMemory" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10CompileEffectFromMemory - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pData, DataLength, pSrcFileName, pDefines, pInclude, HLSLFlags, FXFlags, ppCompiledEffect, ppErrors);
}
HRESULT WINAPI D3DWrappedCreateEffectFromMemory(void *pData, SIZE_T DataLength, UINT FXFlags, ID3D10Device *pDevice, ID3D10EffectPool *pEffectPool, ID3D10Effect **ppEffect)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    CreateEffectFromMemory *pProc = (CreateEffectFromMemory *) GetProcAddress( hD3D10, "D3D10CreateEffectFromMemory" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10CreateEffectFromMemory - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pData, DataLength, FXFlags, pDevice, pEffectPool, ppEffect);
}
HRESULT WINAPI D3DWrappedCreateEffectPoolFromMemory(void *pData, SIZE_T DataLength, UINT FXFlags, ID3D10Device *pDevice, ID3D10EffectPool **ppEffectPool)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    CreateEffectPoolFromMemory *pProc = (CreateEffectPoolFromMemory *) GetProcAddress( hD3D10, "D3D10CreateEffectPoolFromMemory" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10CreateEffectPoolFromMemory - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pData, DataLength, FXFlags, pDevice, ppEffectPool);
}
HRESULT WINAPI D3DWrappedDisassembleEffect(ID3D10Effect *pEffect, BOOL EnableColorCode, ID3D10Blob **ppDisassembly)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    DisassembleEffect *pProc = (DisassembleEffect *) GetProcAddress( hD3D10, "D3D10DisassembleEffect" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10DisassembleEffect - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pEffect, EnableColorCode, ppDisassembly);
}
HRESULT WINAPI D3DWrappedCompileShader(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, ID3D10Include* pInclude, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    CompileShader *pProc = (CompileShader *) GetProcAddress( hD3D10, "D3D10CompileShader" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10CompileShader - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pSrcData, SrcDataLen, pFileName, pDefines, pInclude, pFunctionName, pProfile, Flags, ppShader, ppErrorMsgs);
}
HRESULT WINAPI D3DWrappedDisassembleShader(CONST void *pShader, SIZE_T BytecodeLength, BOOL EnableColorCode, LPCSTR pComments, ID3D10Blob** ppDisassembly)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    DisassembleShader *pProc = (DisassembleShader *) GetProcAddress( hD3D10, "D3D10DisassembleShader" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10DisassembleShader - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pShader, BytecodeLength, EnableColorCode, pComments, ppDisassembly);
}
LPCSTR WINAPI D3DWrappedGetPixelShaderProfile(ID3D10Device *pDevice)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return NULL;
    }
    
    GetPixelShaderProfile *pProc = (GetPixelShaderProfile *) GetProcAddress( hD3D10, "D3D10GetPixelShaderProfile" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10GetPixelShaderProfile - Function not found in D3D10.dll" ) );
        return NULL;
    }
    
    return pProc(pDevice);
}
LPCSTR WINAPI D3DWrappedGetVertexShaderProfile(ID3D10Device *pDevice)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return NULL;
    }
    
    GetVertexShaderProfile *pProc = (GetVertexShaderProfile *) GetProcAddress( hD3D10, "D3D10GetVertexShaderProfile" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10GetVertexShaderProfile - Function not found in D3D10.dll" ) );
        return NULL;
    }
    
    return pProc(pDevice);
}
LPCSTR WINAPI D3DWrappedGetGeometryShaderProfile(ID3D10Device *pDevice)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return NULL;
    }
    
    GetGeometryShaderProfile *pProc = (GetGeometryShaderProfile *) GetProcAddress( hD3D10, "D3D10GetGeometryShaderProfile" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10GetGeometryShaderProfile - Function not found in D3D10.dll" ) );
        return NULL;
    }
    
    return pProc(pDevice);
}
HRESULT WINAPI D3DWrappedReflectShader(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10ShaderReflection **ppReflector)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    ReflectShader *pProc = (ReflectShader *) GetProcAddress( hD3D10, "D3D10ReflectShader" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10ReflectShader - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pShaderBytecode, BytecodeLength, ppReflector);
}
HRESULT WINAPI D3DWrappedPreprocessShader(LPCSTR pSrcData, SIZE_T SrcDataSize, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, ID3D10Include* pInclude, ID3D10Blob** ppShaderText, ID3D10Blob** ppErrorMsgs)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    PreprocessShader *pProc = (PreprocessShader *) GetProcAddress( hD3D10, "D3D10PreprocessShader" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10PreprocessShader - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pSrcData, SrcDataSize, pFileName, pDefines, pInclude, ppShaderText, ppErrorMsgs);
}
HRESULT WINAPI D3DWrappedGetInputSignatureBlob(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    GetInputSignatureBlob *pProc = (GetInputSignatureBlob *) GetProcAddress( hD3D10, "D3D10GetInputSignatureBlob" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10GetInputSignatureBlob - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pShaderBytecode, BytecodeLength, ppSignatureBlob);
}
HRESULT WINAPI D3DWrappedGetOutputSignatureBlob(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    GetOutputSignatureBlob *pProc = (GetOutputSignatureBlob *) GetProcAddress( hD3D10, "D3D10GetOutputSignatureBlob" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10GetOutputSignatureBlob - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pShaderBytecode, BytecodeLength, ppSignatureBlob);
}
HRESULT WINAPI D3DWrappedGetInputAndOutputSignatureBlob(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    GetInputAndOutputSignatureBlob *pProc = (GetInputAndOutputSignatureBlob *) GetProcAddress( hD3D10, "D3D10GetInputAndOutputSignatureBlob" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10GetInputAndOutputSignatureBlob - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pShaderBytecode, BytecodeLength, ppSignatureBlob);
}
HRESULT WINAPI D3DWrappedGetShaderDebugInfo(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob** ppDebugInfo)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    GetShaderDebugInfo *pProc = (GetShaderDebugInfo *) GetProcAddress( hD3D10, "D3D10GetShaderDebugInfo" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10GetShaderDebugInfo - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pShaderBytecode, BytecodeLength, ppDebugInfo);
}
HRESULT WINAPI D3DWrappedCreateDevice( IDXGIAdapter *pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, UINT SDKVersion, ID3D10Device **ppDevice)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    CreateDevice *pProc = (CreateDevice *) GetProcAddress( hD3D10, "D3D10CreateDevice" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10CreateDevice - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pAdapter, DriverType, Software, Flags, SDKVersion, ppDevice);
}
HRESULT WINAPI D3DWrappedCreateDeviceAndSwapChain( IDXGIAdapter *pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, UINT SDKVersion, DXGI_SWAP_CHAIN_DESC *pSwapChainDesc, IDXGISwapChain **ppSwapChain, ID3D10Device **ppDevice)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    CreateDeviceAndSwapChain *pProc = (CreateDeviceAndSwapChain *) GetProcAddress( hD3D10, "D3D10CreateDeviceAndSwapChain" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10CreateDeviceAndSwapChain - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(pAdapter, DriverType, Software, Flags, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice);
}
HRESULT WINAPI D3DWrappedCreateBlob(SIZE_T NumBytes, ID3D10Blob* *ppBuffer)
{
    HMODULE hD3D10 = LoadLibrary("D3D10.dll");
    if (hD3D10 == NULL)
    {
        WriteError( _T( "D3D10.dll Not found" ) );
        return E_FAIL;
    }
    
    CreateBlob *pProc = (CreateBlob *) GetProcAddress( hD3D10, "D3D10CreateBlob" );
    if (pProc == NULL)
    {
        WriteError( _T( "D3D10CreateBlob - Function not found in D3D10.dll" ) );
        return E_FAIL;
    }
    
    return pProc(NumBytes, ppBuffer);
}

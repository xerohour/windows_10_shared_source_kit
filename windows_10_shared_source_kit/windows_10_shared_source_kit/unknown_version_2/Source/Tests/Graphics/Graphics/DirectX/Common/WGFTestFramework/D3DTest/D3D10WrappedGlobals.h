//*****************************************************************************
//***                        THIS FILE IS GENERATED                        ****
//***                       DO NOT MODIFY THIS FILE !!!                    ****
//*****************************************************************************
//*****************************************************************************
//* File: D3D10WrappedGlobals.h
//* 
//* Description: 
//*     Declare global methods so that we don't need to link with d3d10.lib
//*
//*****************************************************************************

// Ignore Prefast warnings 26060 (Range Precondition violation) and 28930 (Unused assignment of pointer variable)
#pragma warning( disable: 26060 )
#pragma warning( disable: 28930 )

#pragma once

#include "d3d10_1.h"
    HRESULT WINAPI D3DWrappedStateBlockMaskUnion(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
    HRESULT WINAPI D3DWrappedStateBlockMaskIntersect(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
    HRESULT WINAPI D3DWrappedStateBlockMaskDifference(D3D10_STATE_BLOCK_MASK *pA, D3D10_STATE_BLOCK_MASK *pB, D3D10_STATE_BLOCK_MASK *pResult);
    HRESULT WINAPI D3DWrappedStateBlockMaskEnableCapture(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength);
    HRESULT WINAPI D3DWrappedStateBlockMaskDisableCapture(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT RangeStart, UINT RangeLength);
    HRESULT WINAPI D3DWrappedStateBlockMaskEnableAll(D3D10_STATE_BLOCK_MASK *pMask);
    HRESULT WINAPI D3DWrappedStateBlockMaskDisableAll(D3D10_STATE_BLOCK_MASK *pMask);
    BOOL    WINAPI D3DWrappedStateBlockMaskGetSetting(D3D10_STATE_BLOCK_MASK *pMask, D3D10_DEVICE_STATE_TYPES StateType, UINT Entry);
    HRESULT WINAPI D3DWrappedCreateStateBlock(ID3D10Device *pDevice, D3D10_STATE_BLOCK_MASK *pStateBlockMask, ID3D10StateBlock **ppStateBlock);
    HRESULT WINAPI D3DWrappedCompileEffectFromMemory(void *pData, SIZE_T DataLength, LPCSTR pSrcFileName, CONST D3D10_SHADER_MACRO *pDefines, ID3D10Include *pInclude, UINT HLSLFlags, UINT FXFlags, ID3D10Blob **ppCompiledEffect, ID3D10Blob **ppErrors);
    HRESULT WINAPI D3DWrappedCreateEffectFromMemory(void *pData, SIZE_T DataLength, UINT FXFlags, ID3D10Device *pDevice, ID3D10EffectPool *pEffectPool, ID3D10Effect **ppEffect);
    HRESULT WINAPI D3DWrappedCreateEffectPoolFromMemory(void *pData, SIZE_T DataLength, UINT FXFlags, ID3D10Device *pDevice, ID3D10EffectPool **ppEffectPool);
    HRESULT WINAPI D3DWrappedDisassembleEffect(ID3D10Effect *pEffect, BOOL EnableColorCode, ID3D10Blob **ppDisassembly);
    HRESULT WINAPI D3DWrappedCompileShader(LPCSTR pSrcData, SIZE_T SrcDataLen, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, ID3D10Include* pInclude, LPCSTR pFunctionName, LPCSTR pProfile, UINT Flags, ID3D10Blob** ppShader, ID3D10Blob** ppErrorMsgs);
    HRESULT WINAPI D3DWrappedDisassembleShader(CONST void *pShader, SIZE_T BytecodeLength, BOOL EnableColorCode, LPCSTR pComments, ID3D10Blob** ppDisassembly);
    LPCSTR WINAPI D3DWrappedGetPixelShaderProfile(ID3D10Device *pDevice);
    LPCSTR WINAPI D3DWrappedGetVertexShaderProfile(ID3D10Device *pDevice);
    LPCSTR WINAPI D3DWrappedGetGeometryShaderProfile(ID3D10Device *pDevice);
    HRESULT WINAPI D3DWrappedReflectShader(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10ShaderReflection **ppReflector);
    HRESULT WINAPI D3DWrappedPreprocessShader(LPCSTR pSrcData, SIZE_T SrcDataSize, LPCSTR pFileName, CONST D3D10_SHADER_MACRO* pDefines, ID3D10Include* pInclude, ID3D10Blob** ppShaderText, ID3D10Blob** ppErrorMsgs);
    HRESULT WINAPI D3DWrappedGetInputSignatureBlob(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
    HRESULT WINAPI D3DWrappedGetOutputSignatureBlob(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
    HRESULT WINAPI D3DWrappedGetInputAndOutputSignatureBlob(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob **ppSignatureBlob);
    HRESULT WINAPI D3DWrappedGetShaderDebugInfo(CONST void *pShaderBytecode, SIZE_T BytecodeLength, ID3D10Blob** ppDebugInfo);
    HRESULT WINAPI D3DWrappedCreateDevice( IDXGIAdapter *pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, UINT SDKVersion, ID3D10Device **ppDevice);
    HRESULT WINAPI D3DWrappedCreateDeviceAndSwapChain( IDXGIAdapter *pAdapter, D3D10_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags, UINT SDKVersion, DXGI_SWAP_CHAIN_DESC *pSwapChainDesc, IDXGISwapChain **ppSwapChain, ID3D10Device **ppDevice);
    HRESULT WINAPI D3DWrappedCreateBlob(SIZE_T NumBytes, ID3D10Blob* *ppBuffer);

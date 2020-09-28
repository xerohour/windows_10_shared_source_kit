//////////////////////////////////////////////////////////////////////////
//  Direct3DFailureCategoryStrings.h
//  Created:	2010/11/10
//
//  Purpose: Contains declarations of failure categories.
//           These are used for Logging purposes (e.g LogError() in DXGLogger).
//           If new categories are needed the tester should add them 
//           here so they can be shared by all D3D Tests
//////////////////////////////////////////////////////////////////////////

#pragma once

// Includes
#include "WindowsTestFailureCategoryStrings.h"

// Failure Categories
namespace WindowsTest {
	namespace Graphics {
		namespace Direct3D {

			// Core
			const LPCWSTR gFCD3D11CreateDevice						= L"D3D: D3D11CreateDevice";

			// D3D11 Device
			const LPCWSTR gFCDeviceCheckFeatureSupport				= L"D3D: Device: Check Feature Support";
			const LPCWSTR gFCDeviceCheckFormatSupport				= L"D3D: Device: Check Format Support";
			const LPCWSTR gFCDeviceCheckMultisampleQualityLevels	= L"D3D: Device: Check Multisample Quality Levels";
			const LPCWSTR gFCDeviceCleanup							= L"D3D: Device: Cleanup";
			const LPCWSTR gFCDeviceCreateBlendState					= L"D3D: Device: Create Blend State";
			const LPCWSTR gFCDeviceCreateDepthStencilState			= L"D3D: Device: Create Depth Stencil State";
			const LPCWSTR gFCDeviceCreateInputLayout				= L"D3D: Device: Create Input Layout";
			const LPCWSTR gFCDeviceCreateQuery						= L"D3D: Device: Create Query";
			const LPCWSTR gFCDeviceCreateRasterizerState			= L"D3D: Device: Create Rasterizer State";
			const LPCWSTR gFCDeviceCreateResource					= L"D3D: Device: Create Resource";
			const LPCWSTR gFCDeviceCreateShader						= L"D3D: Device: Create Shader";
			const LPCWSTR gFCDeviceCreateState						= L"D3D: Device: Create State";
			const LPCWSTR gFCDeviceCreateView						= L"D3D: Device: Create View";

			// Video Device
			const LPCWSTR gFCVideoDeviceCreateVideoProcessorEnumerator	= L"D3D: Video Device: Create Video Processor Enumerator";
			const LPCWSTR gFCVideoDeviceCreateView						= L"D3D: Video Device: Create View";
			

			// Device context
			const LPCWSTR gFCDeviceContextExecute					= L"D3D: DeviceContext: Execute";
			const LPCWSTR gFCDeviceContextMap						= L"D3D: DeviceContext: Map";

			// Misc
			const LPCWSTR gFCProxyRef								= L"D3D: Proxy Ref";
			const LPCWSTR gFC10L9FilterHelper						= L"D3D: 10Level9 Filter Helper";

		}	// Direct3D

		namespace Direct3D9 {

			// Core
			const LPCWSTR gFCDirect3DCreate9						= L"D3D9: Direct3DCreate9";
			const LPCWSTR gFCCreateDevice							= L"D3D9: Create Device";
			const LPCWSTR gFCGetAdapterIdentifier					= L"D3D9: Get Adapter Identifier";

			const LPCWSTR gFCGetAdapterLUID							= L"D3D9Ex: Get Adapter LUID";

		}	// Direct3D9

		namespace Direct3DX {

			const LPCWSTR gFCCompileShader							= L"D3DX: Compile Shader";

		}	// Direct3DX

		namespace DXGI {

			// Core
			const LPCWSTR gFCCreateFactory							= L"DXGI: Create Factory";
			const LPCWSTR gFCDXGIEnumAdapters						= L"DXGI: Enum Adapters";

			// Object
			const LPCWSTR gFCObjectGetParent						= L"DXGI: Object: Get Parent";

			// Device
			const LPCWSTR gFCDeviceGetAdapter						= L"DXGI: Device: Get Adapter";

			// Adapter
			const LPCWSTR gFCAdapterCheckInterfaceSupport			= L"DXGI: Adapter: Check Interface Support";
			const LPCWSTR gFCAdapterEnumOutputs						= L"DXGI: Adapter: Enum Outputs";
			const LPCWSTR gFCAdapterGetDesc							= L"DXGI: Adapter: Get Desc";

			// Output
			const LPCWSTR gFCOutputGetDesc							= L"DXGI: Output: Get Desc";

			// Factory
			const LPCWSTR gFCFactoryCreateSwapChain					= L"DXGI: Factory: Create Swap Chain";

			// Swap chain
			const LPCWSTR gFCSwapChainPresent						= L"DXGI: Swap Chain: Present";
			const LPCWSTR gFCSwapChainGetBuffer						= L"DXGI: Swap Chain: Get Buffer";

		}	// DXGI

		namespace D3DKMT {

			// Core
			const LPCWSTR gFCD3DKMTEnumAdapters						= L"D3D KMT: Enum Adapters";
			const LPCWSTR gFCD3DKMTQueryAdapterInfo					= L"D3D KMT: Query Adapter Info";
			

		}	// DXGI

	}	// Graphics
}	// WindowsTest

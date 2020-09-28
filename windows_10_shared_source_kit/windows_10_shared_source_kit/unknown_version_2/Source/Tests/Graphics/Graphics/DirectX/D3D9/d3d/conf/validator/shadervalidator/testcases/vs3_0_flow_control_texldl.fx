

    None.

--*/

{

    PBRIDGE Child;
    PLIST_ENTRY Entry;
    PPCI_BRIDGE_REQUIREMENTS *Requirement;

    PAGED_CODE();

    //
    // If requirements had previously been saved for this bridge,
    // discard them now.
    //

    Requirement = &Segment->BridgeRequirementOverride[Bridge->Secondary];
    if (*Requirement != NULL) {
        PciFreePool(*Requirement);
    }

    //
    // Save the requirements for this bridge and clear the Bridge requirements
    // pointer.
    //

    *Requirement = Bridge->Requirements;
    Bridge->Requirements = NULL;

    //
    // Recurse and save requirements for any child bridges.
    //

    for (Entry = Bridge->Children.Flink;
         Entry != &Bridge->Children;
         Entry = Entry->Flink) {

        Child = CONTAINING_RECORD(Entry, BRIDGE, Link);
        PciSaveBridgeRequirements(Child, Segment);
    }

    return;
}

VOID
PciOverrideRequirement(
    _In_ PPCI_BRIDGE_WINDOW_REQUIREMENT Override,
    _Inout_ PPCI_BRIDGE_WINDOW_REQUIREMENT Requirement
    )

/*++

Routine Description:

    This routine overrides one bridge requirement.

Arguments:

    Override - Provides a pointer to the resource requirements found during hotplug enumeration.

    Preferred - Provides a pointer to the requirement to update.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    //
    // If the requirement is disabled, don't update it.
    //

    if (Requirement->Disabled != FALSE) {
        return;
    }

    //
    // If the requirement is less than the override, use the override.
    //

    if (Requirement->Size < Override->Size) {
        Requirement->Size = Override->Size;
    }

    //
    // If the alignment is less than the override, use the override.
    //

    if (Requirement->Alignment < Override->Alignment) {
        Requirement->Alignment = Override->Alignment;
    }

    return;
}

VOID
PciOverrideBridgeRequirements(
    _In_ PPCI_BRIDGE Bridge,
    _Inout_ PPCI_BRIDGE_REQUIREMENTS Preferred,
    _Inout_ PPCI_BRIDGE_REQUIREMENTS Fallback
    )

/*++

Routine Description:

    This routine looks for resource requirements which may have been determined
    during hotplug enumeration and, if found, updates the Preferred bridge
    requirements if more resources are needed than the default allocation.

Arguments:

    Bridge - Provides a pointer to bridge being configured.

    Preferred - Provides a pointer to the Preferred resource requirements of
        the bridge.

Return Value:

    None.

--*/

{

    PPCI_BRIDGE_REQUIREMENTS Override;
    UCHAR SecondaryBusNumber;
    PPCI_SEGMENT Segment;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Fallback);

    //
    // See if any resource requirements were stored for this bridge.
    //

    SecondaryBusNumber = LOCAL_BUS_NUMBER(Bridge->Bus->SecondaryBusNumber);
    Segment = Bridge->Bus->Root->Segment;
    Override = Segment->BridgeRequirementOverride[SecondaryBusNumber];

    if (Override == NULL) {
        return;
    }

    //
    // Override each resoource requirement.
    //

    PciOverrideRequirement(&Override->IoWindow, &Preferred->IoWindow);
    PciOverrideRequirement(&Override->MemoryWindow, &Preferred->MemoryWindow);
    PciOverrideRequirement(&Override->PrefetchWindow, &Preferred->PrefetchWindow);

    return;
}

VOID
PciAdjustBridgeRequirements(
    _Inout_ PBRIDGE Bridge
    )

/*++

Routine Description:

    This routine updates each of the bridge window requirements to round the
    size and alignment to alignment boundary appropriate to the bridge. This
    isn't strictly necessary, but might be useful if insufficient resources
    are preallocated to the bridge.

Arguments:

    Bridge - Provides a pointer to the bridge structure.

Return Value:

    None.

--*/

{

    ULONG IoWindowAlignment;
    ULONG MemoryWindowAlignment;
    PPCI_BRIDGE_REQUIREMENTS Requirements;

    PAGED_CODE();

    switch(Bridge->HeaderType) {
    case PCI_BRIDGE_TYPE:
        IoWindowAlignment = PCI_TYPE1_IO_WINDOW_ALIGNMENT;
        MemoryWindowAlignment = PCI_TYPE1_MEMORY_WINDOW_ALIGNMENT;
        break;

    case PCI_CARDBUS_BRIDGE_TYPE:
        IoWindowAlignment = PCI_TYPE2_IO_WINDOW_ALIGNMENT;
        MemoryWindowAlignment = PCI_TYPE2_MEMORY_WINDOW_ALIGNMENT;
        break;

    default:
        return;
    }

    Requirements = Bridge->Requirements;
    if (Requirements->IoWindow.Size > 0) {
        Requirements->IoWindow.Size =
            ALIGN_RANGE_UP(Requirements->IoWindow.Size,
                           IoWindowAlignment);

        if (Requirements->IoWindow.Alignment < IoWindowAlignment) {
            Requirements->IoWindow.Alignment = IoWindowAlignment;
        }
    }

    if (Requirements->MemoryWindow.Size > 0) {
        Requirements->MemoryWindow.Size =
            ALIGN_RANGE_UP(Requirements->MemoryWindow.Size,
                           MemoryWindowAlignment);

        if (Requirements->MemoryWindow.Alignment < MemoryWindowAlignment) {
            Requirements->MemoryWindow.Alignment = MemoryWindowAlignment;
        }
    }

    if (Requirements->PrefetchWindow.Size > 0) {
        Requirements->PrefetchWindow.Size =
            ALIGN_RANGE_UP(Requirements->PrefetchWindow.Size,
                           MemoryWindowAlignment);

        if (Requirements->PrefetchWindow.Alignment < MemoryWindowAlignment) {
            Requirements->PrefetchWindow.Alignment = MemoryWindowAlignment;
        }
    }
}

ULONG PciValidateBridgeConfig(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ PBRIDGE Bridge,
    _In_ ULONG Indent
    )

{

    PBRIDGE Child;
    PLIST_ENTRY Entry;
    ULONG NodeCount;
    UCHAR Primary;
    UCHAR Secondary;
    UCHAR Subordinate;

    PAGED_CODE();

    NodeCount = 1;

    if (Bridge->RootBus == FALSE) {
        PciReadBusNumsRaw(PciIF,
                          Bridge->Primary,
                          Bridge->Slot,
                          &Primary,
                          &Secondary,
                          &Subordinate);

        TRACE(BUS_NUMBER_TRACE_LEVEL | HOTPLUG_TRACE_LEVEL | RESOURCE_TRACE_LEVEL,
              "%*sBus:%d Dev:%d Fun:%d Sec:%d Sub:%d\n"
              "%*sIoWindow:       Size %#8I64x Align %#8I64x\n"
              "%*sMemoryWindow:   Size %#8I64x Align %#8I64x\n"
              "%*sPrefetchWindow: Size %#8I64x Align %#8I64x\n\n",
              Indent," ",
              Bridge->Primary,
              Bridge->Slot.u.bits.DeviceNumber,
              Bridge->Slot.u.bits.FunctionNumber,
              Bridge->Secondary,
              Bridge->Subordinate,
              Indent," ",
              Bridge->Requirements->IoWindow.Size,
              Bridge->Requirements->IoWindow.Alignment,
              Indent," ",
              Bridge->Requirements->MemoryWindow.Size,
              Bridge->Requirements->MemoryWindow.Alignment,
              Indent," ",
              Bridge->Requirements->PrefetchWindow.Size,
              Bridge->Requirements->PrefetchWindow.Alignment);

        //
        // If this bridge was configured successfully, validate
        // the primary/secondary/subordinate values.
        //

        if ((Bridge->Primary !=0) &&
            (Bridge->Secondary !=0) &&
            (Bridge->Subordinate != 0) &&
            ((Bridge->Secondary <= Bridge->Primary) ||
             (Bridge->Subordinate < Bridge->Secondary))) {

            PCI_ASSERT(FALSE);

            return NodeCount;
        }
    }

    for (Entry = Bridge->Children.Flink;
         Entry != &Bridge->Children;
         Entry = Entry->Flink) {

        Child = CONTAINING_RECORD(Entry, BRIDGE, Link);
        NodeCount += PciValidateBridgeConfig(PciIF, Child, Indent+4);
    }

    return NodeCount;
}


VOID
PciValidateBridgeConfiguration(
    _In_ PPCI_BUS_INTERFACE_STANDARD PciIF,
    _In_ PBRIDGE Bridge
    )

/*++

Routine Description:

    This routine validates the bus numbers in each node of the hierarchy.
    To be used in debugging.

Arguments:

    PciIF - Provides a pointer to the root bus interface.

    Bridge - Provides a pointer to the topmost bridge.

Return Value:

    Number of bridge nodes in the hierarchy.

--*/

{

    ULONG Count;

    PAGED_CODE();

    Count = PciValidateBridgeConfig(PciIF, Bridge, 0);
    return;
}
////////////////////////////////////////////////////////////////////////////////
// UpdateTiles.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TiledResources.h"

// Base Update Tiles Test class, from which we derive specializations for Texture2D and Texture3D
template <TILED_RESOURCE_BASE resourceBase>
class CUpdateTilesTestBase : public CResourceBaseTestHelper<resourceBase>
{
    // Helpers to clean up typenames
    CRESOURCE_BASE_TEST_HELPER__TYPENAME_TYPEDEFS

        // Parameters
    struct
    {
        CVariationParameter<RESIDENCY_PATTERN_2D_3D> *pResidencyPattern;
        CVariationParameter<COPY_TILES_PATTERN> *pCopyTilesPattern;
        CVariationParameter<UINT> *pTextureDimensions;
    } m_parameters;

protected:
    wchar_t *windowNames;
    CUpdateTilesTestBase() : windowNames(nullptr) {}
    virtual ~CUpdateTilesTestBase() { if (windowNames) delete[] windowNames; }

public:
    void InitTestParameters();
    TEST_RESULT ExecuteTestCase();
};


// Actual test classes
class CUpdateTilesTestTex2D : public CUpdateTilesTestBase < TRB_TEXTURE_2D > { };
class CUpdateTilesTestTex3D : public CUpdateTilesTestBase < TRB_TEXTURE_3D > { };




// Test Implementation
template <TILED_RESOURCE_BASE resourceBase>
void CUpdateTilesTestBase<resourceBase>::InitTestParameters()
{
    CreateParameter(_T("TiledResourcesTier"), &CTiledResourcesTest::m_parameters.pTiledResourcesTier);
    CreateParameter(_T("ResidencyPattern"), &m_parameters.pResidencyPattern);
    CreateParameter(_T("CopyTilesPattern"), &m_parameters.pCopyTilesPattern);
    CreateParameter(_T("TextureDimensions"), &m_parameters.pTextureDimensions);

    testfactor::RFactor tiledResourcesTier1 = AddParameterValue<UINT>(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 1U);
    testfactor::RFactor tiledResourcesTier2 = AddParameterValue<UINT>(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 2U);
    testfactor::RFactor tiledResourcesTier3 = AddParameterValue<UINT>(CTiledResourcesTest::m_parameters.pTiledResourcesTier, 3U);

    testfactor::RFactor residencyPatternMip0 = AddParameterValueSet<RESIDENCY_PATTERN_2D_3D>(
        m_parameters.pResidencyPattern,
        new CUserValueSet<RESIDENCY_PATTERN_2D_3D>(
        RP2D_3D_FULL,
        RP2D_3D_FRAGMENTED_MULTI_TILE,
        RP2D_3D_UNIFORM_BOX
        )
        );
    testfactor::RFactor residencyPatternMip1 = AddParameterValue(m_parameters.pResidencyPattern, RP2D_3D_FRAGMENTED_MIP_1);

    testfactor::RFactor copyTilesPatternMip0 = AddParameterValueSet<COPY_TILES_PATTERN>(
        m_parameters.pCopyTilesPattern,
        new CUserValueSet<COPY_TILES_PATTERN>(
        COPY_TILES_FULL,
        COPY_TILES_CENTER_BOX,
        COPY_TILES_PARTIAL
        )
        );
    testfactor::RFactor copyTilesPatternMip1 = AddParameterValue(m_parameters.pCopyTilesPattern, COPY_TILES_MIP1);

    testfactor::RFactor textureDimensionsTier1 = AddParameterValue<UINT>(
        m_parameters.pTextureDimensions,
        (UINT)(4 * CResourceBaseTestHelper<resourceBase>::widthPerTile)
        );
    testfactor::RFactor textureDimensionsTier2 = AddParameterValue<UINT>(
        m_parameters.pTextureDimensions,
        (UINT)(3.5 * CResourceBaseTestHelper<resourceBase>::widthPerTile)
        );

    // When using Texture2D, skip building factors for Tier 3 (Tier 2 tests correct functionality)
    // When using Texture3D, skip building factors for Tiers 1 and 2
    testfactor::RFactor textureDimensions;
    if (TRB_TEXTURE_2D == resourceBase)
    {
        textureDimensions = (tiledResourcesTier1 * textureDimensionsTier1) +
            ((textureDimensionsTier1 + textureDimensionsTier2) * tiledResourcesTier2);
    }
    else
    {
        textureDimensions = ((textureDimensionsTier1 + textureDimensionsTier2) * tiledResourcesTier3);
    }

    testfactor::RFactor patterns = (residencyPatternMip0 * copyTilesPatternMip0) + (residencyPatternMip1 * copyTilesPatternMip1);
    testfactor::RFactor root = patterns * textureDimensions;
    SetRootTestFactor(root);
}

template <TILED_RESOURCE_BASE resourceBase>
TEST_RESULT CUpdateTilesTestBase<resourceBase>::ExecuteTestCase()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    CHandle<ID3D11TextureType> pTiledResource = NULL;
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    TileMappingType tileMappingMip0;
    TileMappingType tileMappingMip1;
    TileMappingType tileMapping;
    TileMappingType bufferMapping;
    BufferMappingType bufferOutputMapping;
    TileMappingType postTileMappings;
    CTiledResourceHelper::CTilePoolManager *pTilePoolManager = GetFramework()->GetTilePoolManager();
    UINT startTiles;
    CHandle<ID3D11RenderTargetView> pRTV;
    COPY_TILES_PATTERN copyTilesPattern = m_parameters.pCopyTilesPattern->GetValue();
    std::vector<BYTE> bufferData(TILE_SIZE_IN_BYTES * 255, 0);
    UINT mipLevel = 0;

    CResourceBaseTestHelper<resourceBase>::GenerateTileMapping(tileMappingMip1, 2, 2, 2, nullptr);

    // Width, Height and Depth (if applicable) are the same
    const UINT width = m_parameters.pTextureDimensions->GetValue();
    const UINT height = width;
    const UINT depth = width;

    const UINT pixelsPerTile = TILE_SIZE_IN_BYTES / sizeof(R8G8B8A8_PIXEL);
    const UINT pixelDataSize = pixelsPerTile * sizeof(R8G8B8A8_PIXEL) * CResourceBaseTestHelper<resourceBase>::totalNumTiles;
    assert(pixelDataSize < bufferData.size());

    R8G8B8A8_PIXEL *pixelData = reinterpret_cast<R8G8B8A8_PIXEL *>(bufferData.data());
    CResourceBaseTestHelper<resourceBase>::FillBufferWithTilePattern(pixelData, 4, 4, 4);

    // Create the tiled texture 
    tr = CTiledResourcesTest::CreateTiledTexture(width, height, depth, &pTiledResource, D3D11_BIND_RENDER_TARGET);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Allocate the tiles from the framework tile pool
    startTiles = pTilePoolManager->FetchRange(pTiledResource, CResourceBaseTestHelper<resourceBase>::totalNumTiles);

    // Initialize all tier-1 tile mappings to blank tile
    if (1 == CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue())
    {
        // Set the entire resource to the shared NULL tile
        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResource, 1, pTilePoolManager->GetTilePool(), RP2D_3D_CLEAR_ALL_MIPS, tileMapping);
        if (RESULT_PASS != tr)
        {
            return tr;
        }
    }

    // Update the tiled resource
    tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResource, startTiles, pTilePoolManager->GetTilePool(), m_parameters.pResidencyPattern->GetValue(), tileMapping);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    switch (m_parameters.pResidencyPattern->GetValue())
    {
    case RP2D_3D_FULL:
    case RP2D_3D_FRAGMENTED_MULTI_TILE:
    case RP2D_3D_FRAGMENTED_MULTI_TILE_SKIPS:
    case RP2D_3D_FRAGMENTED_REUSE_SINGLE_TILE:
    case RP2D_3D_UNIFORM_BOX:
    case RP2D_3D_CLEAR_MIP_0:
        // Clear the mip level not under test
        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResource, startTiles, pTilePoolManager->GetTilePool(), RP2D_3D_CLEAR_MIP_1, tileMappingMip1);
        if (RESULT_PASS != tr)
        {
            return tr;
        }
        mipLevel = 0;
        tileMappingMip0.Overlay(tileMapping);
        tileMapping = tileMappingMip0;
        break;

    case RP2D_3D_FULL_MIP_1:
    case RP2D_3D_FRAGMENTED_MIP_1:
    case RP2D_3D_CLEAR_MIP_1:
        // Clear the mip level not under test
        tr = CResourceBaseTestHelper<resourceBase>::UpdateTileMappings(pTiledResource, startTiles, pTilePoolManager->GetTilePool(), RP2D_3D_CLEAR_MIP_0, tileMappingMip0);
        if (RESULT_PASS != tr)
        {
            return tr;
        }
        mipLevel = 1;
        tileMappingMip1.Overlay(tileMapping);
        tileMapping = tileMappingMip1;
        break;
    }

    // Clear the tiled resource
    float clearColor[4] = { 0.5f, 0.5f, 0.5f, 0.5f };
    CD3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    CResourceBaseTestHelper<resourceBase>::GenerateRenderTargetViewDesc(
        rtvDesc,
        pTiledResource,
        D3D11_RTV_DIMENSION_TEXTURE2D,  // ignored for Texture3D
        DXGI_FORMAT_R8G8B8A8_UNORM,
        0
        );

    hr = GetDevice()->CreateRenderTargetView(pTiledResource, &rtvDesc, &pRTV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
        return RESULT_FAIL;
    }
    pContext->TiledResourceBarrier(nullptr, pRTV);
    pContext->ClearRenderTargetView(pRTV, clearColor);
    pRTV->Release();

    if (TRB_TEXTURE_2D == resourceBase)
    {
        rtvDesc.Texture2D.MipSlice = 1;
    }
    else
    {
        rtvDesc.Texture3D.MipSlice = 1;
        rtvDesc.Texture3D.FirstWSlice = 0;
        rtvDesc.Texture3D.WSize = -1;
    }
    hr = GetDevice()->CreateRenderTargetView(pTiledResource, &rtvDesc, &pRTV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
        return RESULT_FAIL;
    }
    pContext->TiledResourceBarrier(nullptr, pRTV);
    pContext->ClearRenderTargetView(pRTV, clearColor);

    // Copy the tiles from the source buffer
    pContext->TiledResourceBarrier(pTiledResource, pTiledResource);
    tr = CTiledResourcesTest::UpdateTiles(pTiledResource, bufferData.data(), copyTilesPattern, 0, bufferMapping);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Figure out which parts of the resource to validate
    tr = CResourceBaseTestHelper<resourceBase>::IntersectMappings(tileMapping, copyTilesPattern, postTileMappings, bufferOutputMapping);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    {
        const UINT numSlices = (TRB_TEXTURE_2D == resourceBase ? 1 : CResourceBaseTestHelper<resourceBase>::depthPerTile * (mipLevel == 0 ? 4 : 2));
        UINT nameIndex = 0;
        if (CUpdateTilesTestBase<resourceBase>::windowNames == nullptr)
        {
            CUpdateTilesTestBase<resourceBase>::windowNames = new wchar_t[2 * numSlices * 64];
        }
        for (UINT slice = 0; slice < numSlices; ++slice)
        {
            wchar_t sliceName[32];
            auto GenerateName = [&](const UINT mipLevel, const UINT inSlice) -> const wchar_t*
            {
                swprintf_s(sliceName, ARRAYSIZE(sliceName), L", Slice %d", inSlice);
                swprintf_s(windowNames + (64 * nameIndex), 64, L"Tiled Resource (Mip %d%s)", 0, (numSlices == 1 ? L"" : sliceName));
                return(windowNames + 64 * nameIndex++);
            };
            ShowResource(GenerateName(0, slice), pTiledResource, 0, 0, slice);
            ShowResource(GenerateName(1, slice), pTiledResource, 1, 0, slice, 2.0f);
        }
    }

    // Validate that the tiled resource matches expected, accounting for NULLs and unchanged tiles
    tr = CTiledResourcesTest::ValidateTileAgainstBuffer(pTiledResource, pixelData, (COPY_TILES_MIP1 == copyTilesPattern ? 1 : 0), 0, postTileMappings, clearColor);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    if (RESULT_PASS != CheckNullSentinel())
    {
        tr = RESULT_FAIL;
    }

    return tr;
}
﻿<?xml version='1.0' encoding='utf-8' standalone='yes'?>
<Content xmlns="http://microsoft.com/schemas/Windows/Kits/Manifest">
  <ContentGroup
      BuildArchitecture="each"
      Destination="$(_BuildArch)"
      >
    <FeatureReference Name="Device.Graphics"/>
    <BinaryFile
        Destination="nttest\windowstest\graphics\d3d\conf"
        Name="DXGIHybridPresent.exe"
        Source="$(OBJ_PATH)\$(O)"
        />
  </ContentGroup>
</Content>
// StaticDynamicVB.cpp: implementation of the CStaticDynamicVBTest class.
//
//////////////////////////////////////////////////////////////////////

#include "StaticDynamicVB.h"


typedef struct
{
	float x, y, z;
	float nx, ny, nz;
	D3DCOLOR color;
} D3DVERTEXCOLOR;

typedef struct
{
	float tu, tv;
} D3DVERTEXTEXTURE;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStaticDynamicVBTest::CStaticDynamicVBTest() :
	m_pStaticVertexBuffers( NULL )
{
	m_szTestName	= _T("StaticDynamicVB");
	m_szCommandKey	= _T("StaticDynamicVB");
}

CStaticDynamicVBTest::~CStaticDynamicVBTest()
{
}


//////////////////////////////////////////////////////////////////////
// Name:	BuildTestVectors()
// Desc:	virtual method for each test to initialize its vector list
//////////////////////////////////////////////////////////////////////
void CStaticDynamicVBTest::BuildTestVectors()
{
#if FAST_TEST
    DWORD arraydwLockNo[]			= {1, 2};
    DWORD arraydwFlagsNo[]			= { D3DLOCK_DISCARD, D3DLOCK_NOOVERWRITE, 0 };
    DWORD arraydwOffsetNo[]			= {0, 192};
    DWORD arraydwVerticesNo[]		= {24};
#else//FAST_TEST
	DWORD arraydwLockNo[]			= {1, 2, 3, 4};
	DWORD arraydwFlagsNo[]			= { D3DLOCK_DISCARD, D3DLOCK_NOOVERWRITE, 0 };
	DWORD arraydwOffsetNo[]			= {0, 48, 144, 192};
	DWORD arraydwVerticesNo[]		= {24};
#endif

	m_arraydwLockNo		= vector<DWORD>(arraydwLockNo, arraydwLockNo + sizeof(arraydwLockNo) / sizeof DWORD);
	m_arraydwFlagsNo	= vector<DWORD>(arraydwFlagsNo, arraydwFlagsNo + sizeof(arraydwFlagsNo) / sizeof DWORD);
	m_arraydwOffsetNo	= vector<DWORD>(arraydwOffsetNo, arraydwOffsetNo + sizeof(arraydwOffsetNo) / sizeof DWORD);
	m_arraydwVerticesNo = vector<DWORD>(arraydwVerticesNo, arraydwVerticesNo + sizeof(arraydwVerticesNo) / sizeof DWORD);
}


//////////////////////////////////////////////////////////////////////
// Name:	CopyDynamicVertex( void*, D3DVERTEX*, D3DCOLOR )
// Desc:	copy the shape vertex into the dynamic Vertex
//////////////////////////////////////////////////////////////////////
void CStaticDynamicVBTest::CopyDynamicVertex( void* pDynamicVertex, D3DVERTEX* pShapeVertex, D3DCOLOR color )
{
	D3DVERTEXCOLOR *pMyDynamicVertex = (D3DVERTEXCOLOR *)pDynamicVertex;
	pMyDynamicVertex->x = pShapeVertex->x;
	pMyDynamicVertex->y = pShapeVertex->y;
	pMyDynamicVertex->z = pShapeVertex->z;
	pMyDynamicVertex->nx = pShapeVertex->nx;
	pMyDynamicVertex->ny = pShapeVertex->ny;
	pMyDynamicVertex->nz = pShapeVertex->nz;
	pMyDynamicVertex->color = color;
}


//////////////////////////////////////////////////////////////////////
// Name:	ConfirmDevice( CAPS*, DWORD, FORMAT )
// Desc:	Called during device initialization, this code checks the device for some minimum set of capabilities
//////////////////////////////////////////////////////////////////////
HRESULT CStaticDynamicVBTest::ConfirmDevice( CAPS* pCaps, DWORD dwBehavior, FORMAT Format )
{
	HRESULT hResult = D3D_OK;
	if( SUCCEEDED( hResult = CDynamicVBTest::ConfirmDevice( pCaps, dwBehavior, Format ) ) )
	{
		if( pCaps->MaxStreams < 2 )
		{
			WriteToLog( _T( "Device doesn't support 2 vertex streams, skipping test." ) );
			hResult = D3DERR_NOTAVAILABLE;
		}
	}
	return hResult;
}


//////////////////////////////////////////////////////////////////////
// Name:	OneTimeSceneInit()
// Desc:	Called during initial app startup, this function performs all the permanent initialization
//////////////////////////////////////////////////////////////////////
HRESULT CStaticDynamicVBTest::OneTimeSceneInit()
{
	if( SUCCEEDED( CDynamicVBTest::OneTimeSceneInit() ) )
	{
		m_DynamicStride = sizeof D3DVERTEXCOLOR;
		m_StaticStride = sizeof D3DVERTEXTEXTURE;
		VERTEXELEMENT D3DVERTEX_Elements[] =		// dynamic vertex elements
		{
			// Stream	Offset	Type					Method					Usage					UsageIndex
			{	0,		0,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
			{	0,		12,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0},
			{	0,		24,		D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		0},
			{	1,		0,		D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0},
			D3DDECL_END()
		};
		m_VertexElements = vector<VERTEXELEMENT>( D3DVERTEX_Elements, D3DVERTEX_Elements + sizeof(D3DVERTEX_Elements) / sizeof VERTEXELEMENT );

		DWORD dwDecl[] =
		{
			D3DVSD_STREAM(0),
			D3DVSD_REG( D3DVSDE_POSITION,	D3DVSDT_FLOAT3 ),
			D3DVSD_REG( D3DVSDE_NORMAL,		D3DVSDT_FLOAT3 ),
			D3DVSD_REG( D3DVSDE_DIFFUSE,	D3DVSDT_D3DCOLOR ),
			D3DVSD_STREAM(1),
			D3DVSD_REG( D3DVSDE_TEXCOORD0,	D3DVSDT_FLOAT2 ),
			D3DVSD_END()
		};
		m_p81VertexDeclaration = vector<DWORD>( dwDecl, dwDecl + sizeof( dwDecl ) / sizeof DWORD );
	}

	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	RestoreDeviceObjects()
// Desc:	Restore device-memory objects and state after a device is created or resized.
//////////////////////////////////////////////////////////////////////
HRESULT CStaticDynamicVBTest::RestoreDeviceObjects()
{
	HRESULT hResult = D3D_OK;
	if( SUCCEEDED( hResult = CDynamicVBTest::RestoreDeviceObjects() ) )
	{
		try
		{
			// create and fill the static vb
			if( !CreateVertexBuffer(	m_AllShapes.m_nVertices * m_StaticStride,
										USAGE_WRITEONLY,
										0,
										POOL_DEFAULT,
										&m_pStaticVertexBuffers ) )
				throw E_FAIL;

			D3DVERTEXTEXTURE *pVertixes;
			if( !m_pStaticVertexBuffers->Lock( 0, 0, (void**)&pVertixes, 0 ) )
				throw E_FAIL;
			for( int iVertex = 0; iVertex < m_AllShapes.m_nVertices; iVertex++ )
			{
				pVertixes->tu = (m_AllShapes.m_pVertices + iVertex)->tu;
				pVertixes->tv = (m_AllShapes.m_pVertices + iVertex)->tv;
				pVertixes++;
			}
			if( !m_pStaticVertexBuffers->Unlock() )
				throw E_FAIL;

			// set the static vb
			if( !SetStreamSource( 1, m_pStaticVertexBuffers, m_StaticStride, 0 ) )
				throw E_FAIL;
		}
		catch( HRESULT hError )
		{
			return hError;
		}
	}

	return hResult;
}


//////////////////////////////////////////////////////////////////////
// Name:	InvalidateDeviceObjects()
// Desc:	Called when the device-dependent objects are about to be lost.
//////////////////////////////////////////////////////////////////////
HRESULT CStaticDynamicVBTest::InvalidateDeviceObjects()
{
	CDynamicVBTest::InvalidateDeviceObjects();
	RELEASE( m_pStaticVertexBuffers );

	return D3D_OK;
}
//////////////////////////////////////////////////////////////////////////
//  Centroid.h
//  created:	2005/07/18
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "multisample.h"
	
class CMultisampleTest_Centroid : public CMultisampleTest
{
public:
	CMultisampleTest_Centroid() 
		: CMultisampleTest(),
		  m_bCentroid( false ),
		  m_uSampleIndex( 0 ),
		  m_pTexView( NULL ),
		  m_pPSCentroid( NULL ),
		  m_pSampler( NULL ),
          m_pTexture( NULL )
	{}

	void InitTestParameters();

protected:
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	BOOL Validate( BOOL bResult );
	void CleanupTestCase();

	bool m_bCentroid;
	UINT m_uSampleIndex;
	ID3D11Texture2D			*m_pTexture;
	ID3D11ShaderResourceView	*m_pTexView;
	ID3D11PixelShader		*m_pPSCentroid;
	ID3D11SamplerState		*m_pSampler;
};

// Test for Centroid when the hardware supports FeatureLevel11
// (even if a lesser feature level is used against the hardware)
enum CentroidInterpolationMode
{
	CentroidInterpolationMode_Linear = 0,
	CentroidInterpolationMode_Linear_NoPerspective = 1,
	CentroidInterpolationMode_Max, // Always at end
};
DECLARE_NAMED_VALUES(CentroidInterpolationMode);
inline CentroidInterpolationMode& operator++( CentroidInterpolationMode& e )
{ e = CentroidInterpolationMode( UINT( e ) + 1 ); return e; }
inline CentroidInterpolationMode operator++( CentroidInterpolationMode& e, int )
{ const CentroidInterpolationMode eRet = e; ++e; return eRet; }

enum TriangleOffsetAxis
{
	TriangleOffsetAxis_X = 0,
	TriangleOffsetAxis_Y = 1,
};
DECLARE_NAMED_VALUES(TriangleOffsetAxis);
inline TriangleOffsetAxis& operator++( TriangleOffsetAxis& e )
{ e = TriangleOffsetAxis( UINT( e ) + 1 ); return e; }
inline TriangleOffsetAxis operator++( TriangleOffsetAxis& e, int )
{ const TriangleOffsetAxis eRet = e; ++e; return eRet; }

class CMultisampleTest_Centroid11 : public CMultisampleTest
{
public:
	CMultisampleTest_Centroid11() 
		: CMultisampleTest(),
		  m_pSRView( NULL ),
		  m_pRTTexture2DSS( NULL ),
		  m_pSSRTView( NULL ),
		  m_pSTexture2DSS( NULL ),
		  m_pPSConvertMSToSS( NULL )
	{
		ZeroMemory( m_pPSCentroid, sizeof( m_pPSCentroid ) );
	}

	void InitTestParameters();

protected:
	TEST_RESULT SetupPipeline();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();

	ID3D11PixelShader *m_pPSCentroid[ CentroidInterpolationMode_Max ];
	ID3D11ShaderResourceView *m_pSRView;
	ID3D11Texture2D *m_pRTTexture2DSS;
	ID3D11RenderTargetView *m_pSSRTView;
	ID3D11Texture2D *m_pSTexture2DSS;
	ID3D11PixelShader *m_pPSConvertMSToSS;
	UINT m_FullyCoveredSampleMask;
	const float *m_pSamplePixelOffsets;
	const UINT16 *m_pSamplesCoveredByTriangleXOffset;
	const UINT16 *m_pSamplesCoveredByTriangleYOffset;

	// External Test Paramters:
	INT m_MinCoveredSampleFromMask;
	CentroidInterpolationMode m_CentroidInterpolationMode;
	TriangleOffsetAxis m_TriangleOffsetAxis;
	INT m_TriangleOffset;

	UINT m_SamplesCoveredByTriangleOffset;
	UINT m_PixelX;
	UINT m_PixelY;
	VERTEX m_TriangleVertex[ 3 ];
};

// Test for Centroid when the hardware supports FeatureLevel11
// (even if a lesser feature level is used against the hardware)
// This test focuses on the centroid value of a pixel that is not covered
class CMultisampleTest_Centroid11Uncovered : public CMultisampleTest
{
public:
	CMultisampleTest_Centroid11Uncovered() 
		: CMultisampleTest(),
		  m_pSRView( NULL ),
		  m_pRTTexture2DSS( NULL ),
		  m_pSSRTView( NULL ),
		  m_pPSConvertMSToSS( NULL )
	{
		ZeroMemory( m_pPSCentroid, sizeof( m_pPSCentroid ) );
		ZeroMemory( m_pPSCentroidUncovered, sizeof( m_pPSCentroidUncovered ) );
		ZeroMemory( m_apSTexture2DSS, sizeof( m_apSTexture2DSS ) );
	}

	void InitTestParameters();

protected:
	TEST_RESULT SetupPipeline();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();

	ID3D11PixelShader *m_pPSCentroid[ CentroidInterpolationMode_Max ];
	ID3D11PixelShader *m_pPSCentroidUncovered[ CentroidInterpolationMode_Max ];
	ID3D11ShaderResourceView *m_pSRView;
	ID3D11Texture2D *m_pRTTexture2DSS;
	ID3D11RenderTargetView *m_pSSRTView;
	ID3D11Texture2D *m_apSTexture2DSS[ 2 ];
	ID3D11PixelShader *m_pPSConvertMSToSS;
	UINT m_FullyCoveredSampleMask;
	const float *m_pSamplePixelOffsets;

	// External Test Paramters:
	INT m_MinCoveredSampleFromMask;
	CentroidInterpolationMode m_CentroidInterpolationMode;
};


!include ..\..\conf.mk


TARGETNAME=TexFormats

SOURCES= \
        ..\Compare.cpp \
        ..\DrawGrid.cpp \
	..\PackedRGB.cpp \
        ..\FloatTex.cpp \
        ..\FVF3.cpp \
        ..\FVF4.cpp \
        ..\Print.cpp \
        ..\SysMem.cpp \
        ..\TexFormats.cpp \
	..\FloatPrecision.cpp \
        ..\TexFormats.rc \
        ..\Verify.cpp





SOURCES=$(SOURCES) ..\kitContent.kml

////////////////////////////////////////////////////////////////////////////////
// CustomEvent.h
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "TimingData.h"

////////////////////////////////////////////////////////////////////////////////
// CCustomEventTest
////////////////////////////////////////////////////////////////////////////////

class CCustomEventTest : public CTimingDataTest
{
public:
    CCustomEventTest();
    virtual TEST_RESULT Setup();
    virtual TEST_RESULT ExecuteTestCase();

protected:
};

#include "Test_Include.fx"

int Test_Count = 140;

string TestInfo
<
    string TestType = "VS";
>
= "vs3_0_flow_control_texldl";

//cases with texldl r2, r#, s0

VS_CRITERIA VS_001_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_001_Desc = "vs_3_0 : texldl r# is allowed";
string VS_001 =
    "vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "texldl r2, r0, s0 "
    "mov o0, r2 ";

VS_CRITERIA VS_002_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_002_Desc = "vs_3_0 : texldl r# inside if_gt block is allowed";
string VS_002 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_gt c0.x, r0.x "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_003_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_003_Desc = "vs_3_0 : texldl r# inside if_lt block is allowed";
string VS_003 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_lt c0.y, r0.y "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_004_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_004_Desc = "vs_3_0 : texldl r# inside if_ge block is allowed";
string VS_004 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ge c0.z, r0.z "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_005_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_005_Desc = "vs_3_0 : texldl r# inside if_le block is allowed";
string VS_005 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_le c0.w, r0.w "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_006_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_006_Desc = "vs_3_0 : texldl r# inside if_eq block is allowed";
string VS_006 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_eq c0.x, r0.y "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_007_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_007_Desc = "vs_3_0 : texldl r# inside if_ne block is allowed";
string VS_007 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ne c0.z, r0.w "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_008_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_008_Desc = "vs_3_0 : texldl r# inside if p.x block is allowed";
string VS_008 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.x "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_009_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_009_Desc = "vs_3_0 : texldl r# inside if !p.y block is allowed";
string VS_009 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.y "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_010_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_010_Desc = "vs_3_0 : texldl r# inside if p.z block is allowed";
string VS_010 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.z "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_011_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_011_Desc = "vs_3_0 : texldl r# inside if !p.w block is allowed";
string VS_011 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.w "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_012_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_012_Desc = "vs_3_0 : texldl r# inside if (true) block is allowed";
string VS_012 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";
    
VS_CRITERIA VS_013_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_013_Desc = "vs_3_0 : texldl r# inside if !(true) block is allowed";
string VS_013 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_014_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_014_Desc = "vs_3_0 : texldl r# inside if (false) block is allowed";
string VS_014 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";
    
VS_CRITERIA VS_015_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_015_Desc = "vs_3_0 : texldl r# inside if !(false) block is allowed";
string VS_015 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "
    "	texldl r2, r0, s0 "
    "else "
    "	texldl r2, r1, s0 "
    "endif "
    "mov o0, r2 ";
    

VS_CRITERIA VS_016_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_016_Desc = "vs_3_0 : texldl r# inside loop block is allowed";
string VS_016 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_017_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_017_Desc = "vs_3_0 : texldl r# inside rep block is allowed";
string VS_017 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_018_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_018_Desc = "vs_3_0 : texldl r# inside call block is allowed";
string VS_018 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "call l0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_019_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_019_Desc = "vs_3_0 : texldl r# inside callnz (true) block is allowed";
string VS_019 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, b0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_020_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_020_Desc = "vs_3_0 : texldl r# inside callnz !(true) block is allowed";
string VS_020 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, !b0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_021_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_021_Desc = "vs_3_0 : texldl r# inside callnz (false) block is allowed";
string VS_021 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, b0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_022_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_022_Desc = "vs_3_0 : texldl r# inside callnz !(false) block is allowed";
string VS_022 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "callnz l0, !b0 "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_023_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_023_Desc = "vs_3_0 : texldl r# inside callnz !p.x block is allowed";
string VS_023 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, !p0.x "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";

VS_CRITERIA VS_024_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_024_Desc = "vs_3_0 : texldl r# inside callnz p.y block is allowed";
string VS_024 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, p0.y "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_025_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_025_Desc = "vs_3_0 : texldl r# inside callnz !p.z block is allowed";
string VS_025 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, !p0.z "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_026_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_026_Desc = "vs_3_0 : texldl r# inside callnz p.w block is allowed";
string VS_026 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defb b0, false "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "callnz l0, p0.w "
    "mov o0, r2 "
	"ret "
    "label l0 "
    "	texldl r2, r0, s0 "
    "ret ";
    
VS_CRITERIA VS_027_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_027_Desc = "vs_3_0 : texldl r# inside loop block with break is allowed";
string VS_027 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	break "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_028_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_028_Desc = "vs_3_0 : texldl r# inside loop block with break is allowed";
string VS_028 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_029_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_029_Desc = "vs_3_0 : texldl r# inside rep block with break is allowed";
string VS_029 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	break "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_030_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_030_Desc = "vs_3_0 : texldl r# inside rep block with break is allowed";
string VS_030 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    

VS_CRITERIA VS_031_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_031_Desc = "vs_3_0 : texldl r# inside loop block with break_gt is allowed";
string VS_031 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	break_gt r0.x, r1.x "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_032_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_032_Desc = "vs_3_0 : texldl r# inside loop block with break_gt is allowed";
string VS_032 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_gt r0.x, r1.x "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_033_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_033_Desc = "vs_3_0 : texldl r# inside loop block with break_lt is allowed";
string VS_033 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	break_lt r0.y, r1.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_034_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_034_Desc = "vs_3_0 : texldl r# inside loop block with break_lt is allowed";
string VS_034 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_lt r0.y, r1.y "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_035_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_035_Desc = "vs_3_0 : texldl r# inside loop block with break_ge is allowed";
string VS_035 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	break_ge r0.z, r1.z "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_036_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_036_Desc = "vs_3_0 : texldl r# inside loop block with break_ge is allowed";
string VS_036 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_ge r0.z, r1.z "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_037_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_037_Desc = "vs_3_0 : texldl r# inside loop block with break_le is allowed";
string VS_037 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	break_le r0.w, r1.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_038_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_038_Desc = "vs_3_0 : texldl r# inside loop block with break_le is allowed";
string VS_038 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_le r0.w, r1.w "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_039_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_039_Desc = "vs_3_0 : texldl r# inside loop block with break_eq is allowed";
string VS_039 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	break_eq r0.x, r1.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_040_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_040_Desc = "vs_3_0 : texldl r# inside loop block with break_eq is allowed";
string VS_040 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_eq r0.x, r1.y "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_041_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_041_Desc = "vs_3_0 : texldl r# inside loop block with break_ne is allowed";
string VS_041 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	break_ne r0.z, r1.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_042_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_042_Desc = "vs_3_0 : texldl r# inside loop block with break_ne is allowed";
string VS_042 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	break_ne r0.z, r1.w "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 ";    
    
VS_CRITERIA VS_043_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_043_Desc = "vs_3_0 : texldl r# inside rep block with break_gt is allowed";
string VS_043 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	break_gt r0.x, r1.x "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_044_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_044_Desc = "vs_3_0 : texldl r# inside rep block with break_gt is allowed";
string VS_044 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_gt r0.x, r1.x "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_045_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_045_Desc = "vs_3_0 : texldl r# inside rep block with break_lt is allowed";
string VS_045 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	break_lt r0.y, r1.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_046_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_046_Desc = "vs_3_0 : texldl r# inside rep block with break_lt is allowed";
string VS_046 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_lt r0.y, r1.y "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_047_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_047_Desc = "vs_3_0 : texldl r# inside rep block with break_ge is allowed";
string VS_047 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	break_ge r0.z, r1.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_048_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_048_Desc = "vs_3_0 : texldl r# inside rep block with break_ge is allowed";
string VS_048 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_ge r0.z, r1.z "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_049_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_049_Desc = "vs_3_0 : texldl r# inside rep block with break_le is allowed";
string VS_049 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	break_le r0.w, r1.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_050_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_050_Desc = "vs_3_0 : texldl r# inside rep block with break_le is allowed";
string VS_050 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_le r0.w, r1.w "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_051_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_051_Desc = "vs_3_0 : texldl r# inside rep block with break_eq is allowed";
string VS_051 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	break_eq r0.x, r1.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_052_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_052_Desc = "vs_3_0 : texldl r# inside rep block with break_eq is allowed";
string VS_052 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_eq r0.x, r1.y "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_053_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_053_Desc = "vs_3_0 : texldl r# inside rep block with break_ne is allowed";
string VS_053 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	break_ne r0.z, r1.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_054_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_054_Desc = "vs_3_0 : texldl r# inside rep block with break_ne is allowed";
string VS_054 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "rep i0 "
    "	add r1, r1, c0.x "
    "	break_ne r0.z, r1.w "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 ";
    

VS_CRITERIA VS_055_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_055_Desc = "vs_3_0 : texldl r# inside loop block with breakp !p.x is allowed";
string VS_055 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.x "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 "; 


VS_CRITERIA VS_056_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_056_Desc = "vs_3_0 : texldl r# inside loop block with breakp p.x is allowed";
string VS_056 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	breakp p0.x "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_057_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_057_Desc = "vs_3_0 : texldl r# inside loop block with breakp p.y is allowed";
string VS_057 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.y "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 "; 


VS_CRITERIA VS_058_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_058_Desc = "vs_3_0 : texldl r# inside loop block with breakp !p.y is allowed";
string VS_058 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	breakp !p0.y "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_059_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_059_Desc = "vs_3_0 : texldl r# inside loop block with breakp !p.z is allowed";
string VS_059 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.z "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 "; 


VS_CRITERIA VS_060_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_060_Desc = "vs_3_0 : texldl r# inside loop block with breakp p.z is allowed";
string VS_060 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	breakp p0.z "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    

VS_CRITERIA VS_061_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_061_Desc = "vs_3_0 : texldl r# inside loop block with breakp p.w is allowed";
string VS_061 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.w "
    "	texldl r2, r0, s0 "
    "endloop "
    "mov o0, r2 "; 


VS_CRITERIA VS_062_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_062_Desc = "vs_3_0 : texldl r# inside loop block with breakp !p.w is allowed";
string VS_062 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "loop aL, i0 "
    "	texldl r2, r0, s0 "
    "	breakp !p0.w "
    "	add r1, r1, c0.x "
    "endloop "
    "mov o0, r2 ";
    
VS_CRITERIA VS_063_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_063_Desc = "vs_3_0 : texldl r# inside rep block with breakp p.x is allowed";
string VS_063 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.x "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 "; 

VS_CRITERIA VS_064_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_064_Desc = "vs_3_0 : texldl r# inside rep block with breakp !p.x is allowed";
string VS_064 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	breakp !p0.x "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_065_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_065_Desc = "vs_3_0 : texldl r# inside rep block with breakp !p.y is allowed";
string VS_065 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.y "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 "; 

VS_CRITERIA VS_066_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_066_Desc = "vs_3_0 : texldl r# inside rep block with breakp p.y is allowed";
string VS_066 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	breakp p0.y "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_067_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_067_Desc = "vs_3_0 : texldl r# inside rep block with breakp p.z is allowed";
string VS_067 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp p0.z "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 "; 

VS_CRITERIA VS_068_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_068_Desc = "vs_3_0 : texldl r# inside rep block with breakp !p.z is allowed";
string VS_068 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	breakp !p0.z "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
VS_CRITERIA VS_069_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_069_Desc = "vs_3_0 : texldl r# inside rep block with breakp !p.w is allowed";
string VS_069 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	add r1, r1, c0.x "
    "	breakp !p0.w "
    "	texldl r2, r0, s0 "
    "endrep "
    "mov o0, r2 "; 

VS_CRITERIA VS_070_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_070_Desc = "vs_3_0 : texldl r# inside rep block with breakp p.w is allowed";
string VS_070 =
	"vs_3_0 "
    "dcl_2d s0 "
    "dcl_position v2 "
	"dcl_position o0 "
    "defi i0, 2, 0, 0, 0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_gt p0, r1, c1 " 
    "rep i0 "
    "	texldl r2, r0, s0 "
    "	breakp p0.w "
    "	add r1, r1, c0.x "
    "endrep "
    "mov o0, r2 ";
    
//cases with texldl r2, t#, s0

VS_CRITERIA VS_071_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_071_Desc = "vs_3_0 : texldl v# is allowed";
string VS_071 =
    "vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "texldl r2, v0, s0 "
    "mov o0, r2 ";

VS_CRITERIA VS_072_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_072_Desc = "vs_3_0 : texldl v# inside if_gt block is allowed";
string VS_072 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_gt c0.x, r0.x "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_073_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_073_Desc = "vs_3_0 : texldl v# inside if_lt block is allowed";
string VS_073 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_lt c0.y, r0.y "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_074_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_074_Desc = "vs_3_0 : texldl v# inside if_ge block is allowed";
string VS_074 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ge c0.z, r0.z "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_075_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_075_Desc = "vs_3_0 : texldl v# inside if_le block is allowed";
string VS_075 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_le c0.w, r0.w "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_076_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_076_Desc = "vs_3_0 : texldl v# inside if_eq block is allowed";
string VS_076 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_eq c0.x, r0.y "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_077_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_077_Desc = "vs_3_0 : texldl v# inside if_ne block is allowed";
string VS_077 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if_ne c0.z, r0.w "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_078_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_078_Desc = "vs_3_0 : texldl v# inside if p.x block is allowed";
string VS_078 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.x "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_079_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_079_Desc = "vs_3_0 : texldl v# inside if !p.y block is allowed";
string VS_079 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.y "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_080_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_080_Desc = "vs_3_0 : texldl v# inside if p.z block is allowed";
string VS_080 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if p0.z "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_081_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_081_Desc = "vs_3_0 : texldl v# inside if !p.w block is allowed";
string VS_081 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "setp_eq p0, r0, r1 "
    "if !p0.w "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";

VS_CRITERIA VS_082_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_082_Desc = "vs_3_0 : texldl v# inside if (true) block is allowed";
string VS_082 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if b0 "
    "	texldl r2, v0, s0 "
    "else "
    "	texldl r2, v1, s0 "
    "endif "
    "mov o0, r2 ";
    
VS_CRITERIA VS_083_Criteria = { true, false, 0x0300, -1, -1, -1, -1, 0, -1, 0.f };
string VS_083_Desc = "vs_3_0 : texldl v# inside if !(true) block is allowed";
string VS_083 =
	"vs_3_0 "
    "dcl_texcoord0 v0 "
    "dcl_texcoord1 v1 "
    "dcl_position v2 "
	"dcl_position o0 "
    "dcl_2d s0 "
    "defb b0, true "
    "def c0, 0, 1, -1, 0 "
    "def c1, 0, 1, -1, 0 "
    "mov r0, c0 "
    "mov r1, c1 "
    "if !b0 "

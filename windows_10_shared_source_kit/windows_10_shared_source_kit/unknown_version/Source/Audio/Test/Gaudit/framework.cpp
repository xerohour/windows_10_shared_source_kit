//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
// Copyright(C) Microsoft.All rights reserved.
//
//  File:       framework.cpp
//
//
//--------------------------------------------------------------------------

#include "common.h"



// Helper class for updating params
class CParamsBuilder
{
    PGAUDITPARAMS m_pParams;
public:
    CParamsBuilder(PGAUDITPARAMS pParams) : m_pParams(pParams) {}
    
    __forceinline void OnHandle(HANDLE hFile) 
    {
        m_pParams->hFile = hFile; 
        m_pParams->Prop.Flags = 0; 
        m_pParams->cbSize = sizeof(KSPROPERTY); 
    }

    __forceinline void OnPin(HANDLE hFile, ULONG PinId) 
    { 
        m_pParams->hFile = hFile;
        m_pParams->PinProp.PinId = PinId; 
        m_pParams->Prop.Flags = 0;
        m_pParams->cbSize = sizeof(KSP_PIN); 
    }
    __forceinline void OnNode(HANDLE hFile, ULONG  NodeId) 
    {
        m_pParams->hFile = hFile; 
        m_pParams->NodeProp.NodeId = NodeId; 
        m_pParams->Prop.Flags = KSPROPERTY_TYPE_TOPOLOGY;
        m_pParams->cbSize = sizeof(KSNODEPROPERTY); 
    }


    BOOL OnSet() 
    {
        m_pParams->Prop.Flags |= KSPROPERTY_TYPE_SET; 

        BOOL fRes = IsPropertySupported(m_pParams->hFile, &m_pParams->Prop, m_pParams->cbSize, KSPROPERTY_TYPE_SET);
        SLOG(eInfo3, "Property SET is %s", (fRes) ? "supported" : "not supported");

        return fRes;
    }

    BOOL OnGet() 
    {
        m_pParams->Prop.Flags |= KSPROPERTY_TYPE_GET; 

        BOOL fRes = IsPropertySupported(m_pParams->hFile, &m_pParams->Prop, m_pParams->cbSize, KSPROPERTY_TYPE_GET);
        SLOG(eInfo3, "Property GET is %s", (fRes) ? "supported" : "not supported");

        return fRes;
    } 

    __forceinline void OnCreate() { memset(m_pParams, 0, sizeof(*m_pParams)); }

    operator PGAUDITPARAMS() { return m_pParams; }
    PGAUDITPARAMS operator ->() { return m_pParams; }
};

// Helper function: handles GET/SET properties, IOCTL's, ABORT requests and test flags
__forceinline DWORD DoTest(PGAUDITTESTFCT pFct, CParamsBuilder& Params, ULONG ulTestFlags)
{
    BOOL fSupported;
    DWORD dwResult = FNS_PASS;
    DWORD dwOverallResult = FNS_PASS;

    
#ifndef BUILD_TAEF
    // check user intervention
    if (g_IShell->RequestAbortWithPM())
        return FNS_ABORTED;
#endif

    INCREMENT_INDENT()

    if (GOF_PROP == (ulTestFlags & MASK_GOF))
    {
        // run set test cases
        fSupported = Params.OnSet();
        //    if utTestFlags don't explicitly mention RUN_IF_SUPPORTED  OR if this Property is supported AND ulTestFlags explicitly mentions RUN_IF_SUPPORTED
        if (!(ulTestFlags & RUN_IF_SUPPORTED) || fSupported)
        {
            dwResult = pFct(Params);
            if (dwResult != FNS_PASS)
                dwOverallResult = dwResult;
        }

        // make sure we don't forget about old flags!
        Params->Prop.Flags &= ~KSPROPERTY_TYPE_SET;

        // run get test cases
        fSupported = Params.OnGet();
        if (!(ulTestFlags & RUN_IF_SUPPORTED) || fSupported )
        {
            dwResult = pFct(Params);
            if (dwResult != FNS_PASS)
                dwOverallResult = dwResult;
        }
    }

    if (GOF_IOCTL == (ulTestFlags & MASK_GOF))
    {
        dwResult = pFct(Params);
        if (dwResult != FNS_PASS)
            dwOverallResult = dwResult;
    }
    
    if (RUN_PIN_CREATE == (ulTestFlags))
   {
        dwResult = pFct(Params);
        if (dwResult != FNS_PASS)
            dwOverallResult = dwResult;
   }

    DECREMENT_INDENT()

    return dwOverallResult;
}


// Main test handler
DWORD TestAllDevicesInTheList(TTList<CKsFilter>& list, PGAUDITTESTFCT pFct, PGAUDITPARAMS pParams);


// Main test algorithm
DWORD TestAlgorithm(PGAUDITTESTFCT pFct, PGAUDITPARAMS pParams)
{
    CKsFilter* pfilter = g_pGauditModule->GetDevice()->m_pFilter;
    TTList<CKsFilter> list;
    DWORD dwResult = FNS_PASS;
    CParamsBuilder Params(pParams);

    _ASSERT(!g_nIndentLevel);


    list.AddTail(pfilter);

    // new filter test
    FRAMEWORK()->NewFilter(pfilter);

// sysaudio stuff is only for XP
#if (NTDDI_VERSION < NTDDI_VISTA)
    DWORD dwFlags = g_pGauditModule->GetDevice()->m_dwFlags; 
    ULONG ulTestFlags = g_pGauditModule->GetTestDef()->ulGauditFlags;

    if (dwFlags & SYSAUDIO_DEVICE)
    {
        // close filter handle
        // this makes sure we can run create test scenarios
        // and also that we get consistent behavior
        SAFE_CLOSE_HANDLE(pfilter->m_handle)

        // run filter create test case: failure point
        if (ulTestFlags & RUN_FILTER_CREATE)
        {
            Params.OnCreate();
            if (FNS_PASS != (dwResult = DoTest(pFct, Params, ulTestFlags)))
                goto EXIT;
        }

        // instantiate filter
        if (!pfilter->Instantiate())
        {
            SLOG(eError, "ERROR: Failed to instantiate filter\n");
            dwResult = FNS_FAIL;
            goto EXIT;
        }

        // run test case for filter: failure point
        if (ulTestFlags & RUN_PROP)
        {
            Params.OnHandle(pfilter->m_handle);
            if (FNS_PASS != (dwResult = DoTest(pFct, Params, ulTestFlags)))
                goto EXIT;
        }

        // acquire sysaudio devices
        ULONG nDevCount = GetSysaudioDevices(pfilter->m_handle);
        for(ULONG nDev = 0; nDev < nDevCount && (dwResult == FNS_PASS); nDev++)
        {
            // new internal device test
            INCREMENT_INDENT()

            for(UINT i=0; i < 2 && (dwResult == FNS_PASS); i++)
            {
                BOOL fTest = (!i && SetSysaudioDeviceInfo(pfilter->m_handle, nDev)) ||
                             (i && SetSysaudioDeviceUncombined(pfilter->m_handle, nDev));

                if (fTest)
                {
                    FRAMEWORK()->NewDevice(nDev);

                    BOOL fResult;

                    // enumerate nodes&pins for inner device
                    fResult = pfilter->EnumerateNodes();
                    fResult = fResult && pfilter->EnumeratePins();

                    if (fResult)
                    {
                        dwResult = TestAllDevicesInTheList(list, pFct, pParams);
                    }

                    // free kslib structures
                    TTNode<CKsPin>* pPinNode = pfilter->m_listPins.GetHead();
                    while(pPinNode)
                    {
                        delete pPinNode->pData;
                        pPinNode = pfilter->m_listPins.GetNext(pPinNode);
                    }
                    pfilter->m_listPins.Empty();
                    TTNode<CKsNode>* pNodeNode = pfilter->m_listNodes.GetHead();
                    while(pNodeNode)
                    {
                        delete pNodeNode->pData;
                        pNodeNode = pfilter->m_listNodes.GetNext(pNodeNode);
                    }
                    pfilter->m_listNodes.Empty();
                    TTNode<CKsConnection>* pConnNode = pfilter->m_listConnections.GetHead();
                    while(pConnNode)
                    {
                        delete pConnNode->pData;
                        pConnNode = pfilter->m_listConnections.GetNext(pConnNode);
                    }
                    pfilter->m_listConnections.Empty();
                    pfilter->m_listRenderSinkPins.Empty();
                    pfilter->m_listCaptureSinkPins.Empty();
                    pfilter->m_listRenderSourcePins.Empty();
                    pfilter->m_listCaptureSourcePins.Empty();
                    pfilter->m_listNoCommPins.Empty();                    
                }
            }

            DECREMENT_INDENT()
        }
    } else if (dwFlags & WDMAUD_DEVICE)
    {
        // close filter handle
        // this makes sure we can run create test scenarios
        // and also that we get consistent behavior
        SAFE_CLOSE_HANDLE(pfilter->m_handle)

        // run filter create test case: failure point
        if (ulTestFlags & RUN_FILTER_CREATE)
        {
            Params.OnCreate();
            if (FNS_PASS != (dwResult = DoTest(pFct, Params, ulTestFlags)))
                goto EXIT;
        }

        // instantiate filter
        if (!pfilter->Instantiate())
        {
            SLOG(eError, "ERROR: Failed to instantiate filter\n");
            dwResult = FNS_FAIL;
            goto EXIT;
        }

        // run test case for filter: failure point
        if (ulTestFlags & RUN_PROP)
        {
            Params.OnHandle(pfilter->m_handle);
            if (FNS_PASS != (dwResult = DoTest(pFct, Params, ulTestFlags)))
                goto EXIT;
        }
    }    
    else
#endif // (NTDDI_VERSION < NTDDI_VISTA)
        return TestAllDevicesInTheList(list, pFct, pParams);

#if (NTDDI_VERSION < NTDDI_VISTA)
EXIT:

    return dwResult;
#endif // (NTDDI_VERSION < NTDDI_VISTA)
}



// Iterates and tests all devices, pins and nodes and for each calls
// all the existent properties
DWORD TestAllDevicesInTheList(TTList<CKsFilter>& list, PGAUDITTESTFCT pFct, PGAUDITPARAMS pParams)
{
    DWORD dwResult = FNS_PASS;
    CParamsBuilder Params(pParams);
    PKSMULTIPLE_ITEM pranges = NULL;
    ULONG ulTestFlags = g_pGauditModule->GetTestDef()->ulGauditFlags;
    CKsPin* ppin = NULL;

    _ASSERT(!g_nIndentLevel);

    // for each filter
    for (
        TTNode<CKsFilter>* pfnode = list.GetHead();
        NULL != pfnode;
        pfnode = list.GetNext(pfnode)
    ) {
        CKsFilter* pfilter = pfnode->pData;

        // run test case for filter: failure point - it will test for Set / Get property with Filter handle.
        if (ulTestFlags & RUN_PROP)     // Run_Prop menas run this property test on the filter
        {
            Params.OnHandle(pfilter->m_handle);
            if (FNS_PASS != (dwResult = DoTest(pFct, Params, ulTestFlags)))
                goto EXIT;
        }

        // for all the nodes test on filter - it will test for Get /Set for Property supported on each Node of Filter
        for (
            TTNode<CKsNode>* pNodeNode = pfilter->m_listNodes.GetHead();
            NULL != pNodeNode;
            pNodeNode = pfilter->m_listNodes.GetNext(pNodeNode)
        ) {
            CKsNode* pksNode = pNodeNode->pData;
            // new node test
            FRAMEWORK()->NewNode(pksNode);

            // run test case for filter&node
            if (ulTestFlags & RUN_NODE_PROP)
            {
                Params.OnNode(pfilter->m_handle, pksNode->m_nId);
                if (FNS_PASS != (dwResult = DoTest(pFct, Params, ulTestFlags)))
                    goto EXIT;
            }
        } // for each node on the filter

        // make a list of pointers to the five interesting pin lists
        TTList<CKsPin>* PinLists[] = {
            &pfilter->m_listRenderSinkPins,
            &pfilter->m_listCaptureSinkPins,
            &pfilter->m_listRenderSourcePins,
            &pfilter->m_listCaptureSourcePins,
            &pfilter->m_listNoCommPins
        };

        // for each of the lists
        for (
            DWORD dwType = 0;
            dwType < ARRAYSIZE(PinLists);
            dwType++
        ) {
            TTList<CKsPin>* pPinList = PinLists[dwType];

            // for each pin on the list
            for (
                TTNode<CKsPin>* pPinNode = pPinList->GetHead();
                NULL != pPinNode;
                pPinNode = pPinList->GetNext(pPinNode)
            ) {
        
                ppin = pPinNode->pData;
                // new pin test
                FRAMEWORK()->NewPin(ppin);

                // run create test cases: failure point
                if ((ulTestFlags & RUN_PIN_CREATE) )//&& dwType <2)    // as we want to create only sink pin and not source pins as they will require handle of source pin to create it
                {
                    Params.OnCreate();
                    if (FNS_PASS != (dwResult = DoTest(pFct, Params, ulTestFlags)))
                            goto EXIT;
                }

     
                // run test case for filter&pin: failure point
                if (ulTestFlags & RUN_PIN_PROP)
                {
                    Params.OnPin(pfilter->m_handle, ppin->m_nId);
                    if (FNS_PASS != (dwResult = DoTest(pFct, Params, ulTestFlags)))
                        goto EXIT;
                }

                pranges = NULL;
            
                if (!pfilter->GetPinPropertyMulti(ppin->m_nId, KSPROPSETID_Pin, KSPROPERTY_PIN_DATARANGES, &pranges))
                {
                    SLOG(eError, "ERROR: Failed to get pin %ld dataranges\n", ppin->m_nId);
                    dwResult = FNS_FAIL;
                    goto EXIT;
                }
                PKSDATARANGE pKsDataRange = (PKSDATARANGE) (pranges + 1);
                pKsDataRange = (PKSDATARANGE)( ( ULONG_PTR(pKsDataRange) + (ULONG_PTR)7) & ~(ULONG_PTR)7);

                bool thisIsAttributeList = false;
                bool nextIsAttributeList = false;

                // for each of the data ranges
                BOOL fCreatedPin = FALSE;
                for(UINT i=0; i < pranges->Count; i++)
                {
                    thisIsAttributeList = nextIsAttributeList;
                    nextIsAttributeList = false;
                    
                    if(thisIsAttributeList)  {
                        goto NextRange;
                    }

                    nextIsAttributeList = ((pKsDataRange->Flags & KSDATARANGE_ATTRIBUTES) == KSDATARANGE_ATTRIBUTES);
                    
                    fCreatedPin = FALSE;
                    ZeroMemory(&AudioPinFormat,sizeof(KSDATAFORMAT_WAVEFORMATEX));
                    ZeroMemory(&DsoundPinFormat,sizeof(KSDATAFORMAT_DSOUND));
                    ZeroMemory(&MidiPinFormat,sizeof(KSDATAFORMAT));
                    PinType = 0;
                    if (pKsDataRange->MajorFormat == KSDATAFORMAT_TYPE_AUDIO)   {       // if the format is realted to AUDIO then only create pin
                        // following 2 lines are commented as it is not necessary that majorFormat is equal to KSDATAFORMAT_TYPE_AUDIO menas that range is of KSDATARANGE_AUDIO
                        //PKSDATARANGE_AUDIO  pformat  =  (PKSDATARANGE_AUDIO) pKsDataRange;  
                        //pformat = (PKSDATARANGE_AUDIO)( ( ULONG_PTR(pformat) + (ULONG_PTR)7) & ~(ULONG_PTR)7);
                        fCreatedPin = CreatePinInstance(pfilter->m_handle, pKsDataRange, ppin->m_nId, 0, &ppin->m_handle);      // chaged for pformat to pKsDataRange
                    }
                    if (!fCreatedPin)//(!CreatePinInstance(pfilter->m_handle, pformat, ppin->m_nId, 0, &ppin->m_handle))
                        goto NextRange;                

                    INCREMENT_INDENT()
    
                    // run test case for pin: failure point
                    if (ulTestFlags & RUN_PROP)
                    {
                        Params.OnHandle(ppin->m_handle);
                        if (FNS_PASS != (dwResult = DoTest(pFct, Params, ulTestFlags))) {
                            SAFE_CLOSE_HANDLE(ppin->m_handle);
                            goto EXIT;
                        }
                    }

                    // for all the nodes test on pin
                    for (
                        TTNode<CKsNode>* pNodeNode = pfilter->m_listNodes.GetHead();
                        NULL != pNodeNode;
                        pNodeNode = pfilter->m_listNodes.GetNext(pNodeNode)
                    ) {
                        CKsNode* pksNode = pNodeNode->pData;
                        
                        // new node on pin test
                        FRAMEWORK()->NewNodePin(pksNode);

                        // run test case for node&pin: failure point
                        if (ulTestFlags & RUN_NODE_PROP)
                        {
                            Params.OnNode(ppin->m_handle, pksNode->m_nId);
                                if (FNS_PASS != (dwResult = DoTest(pFct, Params, ulTestFlags))) {
                                    SAFE_CLOSE_HANDLE(ppin->m_handle);
                                    goto EXIT;
                                }
                        }
                    } // for each node test on pin
                    ppin->ClosePin();
                    DECREMENT_INDENT()    
NextRange:                    
                    ULONG size = (thisIsAttributeList ? ((PKSMULTIPLE_ITEM)pKsDataRange)->Size : pKsDataRange->FormatSize);
                    pKsDataRange = PKSDATARANGE(((LPBYTE)pKsDataRange) + size ) ;  //since formatsize may be different then size of KSDATARANGE or KSDATARNAGE_AUDIO
                    pKsDataRange = PKSDATARANGE( (ULONG_PTR(pKsDataRange)  + (ULONG_PTR)7) & ~(ULONG_PTR)7);
                } // for each of the data ranges
                
                // free dataranges
                LocalFree(pranges);
                pranges = NULL;

                // invalidate pin
                ppin = NULL;

            } // for each pin on the list
        } // for each list
    } // for each filter

EXIT:

    if (pranges)
    {
        LocalFree(pranges);
    }

    if (ppin)
         ppin =  NULL;
    

    // restore indentation in case we've exit prematurely
    while (g_nIndentLevel > 0)
    {
        DECREMENT_INDENT()
    }

    return dwResult;
}

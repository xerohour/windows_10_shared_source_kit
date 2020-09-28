#include <Adapter.h>
#include <DXGLogger.h>
#include <CDebugOut.h>
#include <DXGIScanoutFormats.h>


UINT OutputModeContainer::GetTotalModeCount()
{
    UINT modeCount = 0;
    for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
    {
        modeCount+= RawModes[rotation].size();
    }

    return modeCount;
}

void PrintModes(std::vector < ModeInformation >* pFormats)
{
    for(UINT i = 0; i < (*pFormats).size(); i++)
    {
        DebugOut.Write("ModeDesc.Width %d ModeDesc.Height %d format %d ModeDesc.RefreshRate.Numerator %d ModeDesc.RefreshRate.Denominator %d ModeDesc.ScanlineOrdering %d ModeDesc.Scaling %d uiMultisampleCount %d uiMultisampleQuality %d \n" ,
            (*pFormats)[i].ModeDesc.Width, 
            (*pFormats)[i].ModeDesc.Height,
            (*pFormats)[i].ModeDesc.Format,
            (*pFormats)[i].ModeDesc.RefreshRate.Numerator,
            (*pFormats)[i].ModeDesc.RefreshRate.Denominator,
            (*pFormats)[i].ModeDesc.ScanlineOrdering,
            (*pFormats)[i].ModeDesc.Scaling,
            (*pFormats)[i].uiMultisampleCount,
            (*pFormats)[i].uiMultisampleQuality);
    }
}

void PrintModes(std::vector < Adapter >* pAdapters)
{
    for(UINT k = 0; k < (*pAdapters).size();k++)
    {
       if( (*pAdapters)[k].OutputModeContainer.size() == 0)
       {
            DebugOut.Write("adapter[%d] has no outputs\n", k);
       }
       else
       {
           //
           // Iterate all outputs
           //
           for(UINT j = 0; j < (*pAdapters)[k].OutputModeContainer.size();j++)
           {
               for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
               {
                   DebugOut.Write("adapter[%d].Output[%d].RawModes[rotation %d].size()=%d \n", k, j, rotation,(*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size());
                   if((*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size())
                   {                
                       PrintModes(&(*pAdapters)[k].OutputModeContainer[j].RawModes[rotation]);
                   }
               }
           }
       }
    }
}

 void CopyDefaultOutputToNoOutputAdapters(std::vector < Adapter >* pAdapters)
 {
     OutputModeContainer defaultOutput;
     bool foundDefault = false;
     //
     // Iterate all outputs find the default monitor
     //
     
    DebugOut.Write( "CopyDefaultOutputToNoOutputAdapters adapterModes.size() %d.\n",(*pAdapters).size());

     for(UINT k = 0; k < (*pAdapters).size();k++)
     {
         for(UINT j = 0; j < (*pAdapters)[k].OutputModeContainer.size();j++)
         {
             MONITORINFOEXW monitorinfo;
             ZeroMemory( (LPVOID)&monitorinfo, sizeof(MONITORINFOEXW) );
             monitorinfo.cbSize = sizeof( MONITORINFOEXW );
             
             //
             // all the modes for this output should have the same hmonitor.  Just check the first one.
             //
             GetMonitorInfoW( (*pAdapters)[k].OutputModeContainer[j].RawModes[0][0].hmonitor, &monitorinfo );
             
             if(monitorinfo.dwFlags & MONITORINFOF_PRIMARY)
             {
                 DebugOut.Write( "found  defaultOutput on adapter %d output .\n",k,j);
                 defaultOutput = (*pAdapters)[k].OutputModeContainer[j];
                 foundDefault = true;
                 break;
             }
                

         }

         if(foundDefault)
         {
             break;
         }
     } 

     if(foundDefault)
     {
         for(UINT k = 0; k < (*pAdapters).size();k++)
         {
             if( (*pAdapters)[k].OutputModeContainer.size() == 0)
             {
                 DebugOut.Write("adapter[%d] has no outputs. Copying primary monitor mode info \n", k);
                 
                 
                 //
                 // Set the AdapterOrdinal in the ModeInformation Data. Probably redundant but
                 // exists for convenience
                 //
                 for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
                 {
                     for(UINT i = 0; i < defaultOutput.RawModes[rotation].size(); i++)
                     {               
                         defaultOutput.RawModes[rotation][i].uiDXGIAdapterOrdinal = k;
                     }
                 }

                 (*pAdapters)[k].OutputModeContainer.push_back(defaultOutput);
             }
         }
     }
 }

void FirstModePerFormat(std::vector < Adapter >* pAdapters)
{
    for(UINT k = 0; k < (*pAdapters).size();k++)
    {
        for(UINT j = 0; j < (*pAdapters)[k].OutputModeContainer.size();j++)
        {
            for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
            {
                for(UINT uiFormatIndex = 0; uiFormatIndex < clsScanOutFormats::cuiNoOfScanOutFormats;uiFormatIndex++)
                {
                    bool foundFirstModeFormat = false;
                    for(UINT i = 0; i < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size(); i++)
                    {
                        DXGI_MODE_DESC dxgiMode = (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][i].ModeDesc;
                        if(clsScanOutFormats::DXGIScanOutFormats[ uiFormatIndex ] == dxgiMode.Format)
                        {
                            if(foundFirstModeFormat)
                            {
                                (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].erase
                                (
                                    (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].begin() + i--
                                );                      
                            }

                            foundFirstModeFormat = true;
                        }

                    }
                }
            }
        }
    }
}

void FilterLowResolutionModes(std::vector < Adapter >* pAdapters)
{
    
    UINT width = 800;
    UINT height = 600;

    for(UINT k = 0; k < (*pAdapters).size();k++)
    {
        //
        // Iterate all outputs
        //
        for(UINT j = 0; j < (*pAdapters)[k].OutputModeContainer.size();j++)
        {
            for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
            {
                UINT swapableWidth = width;
                UINT swapableHeight = height;

                if(rotation == 1 || rotation == 3)
                {
                    std::swap( swapableWidth, swapableHeight );
                }

                for(UINT i = 0; i < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size(); i++)
                {
                    if( (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Width < swapableWidth ||
							    (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Height < swapableHeight)
                    {
                        (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].erase
					    (
						    (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].begin() + i--
					    );
                    
                    }
                }
            }
        }
    }
}

void GetHighResolutionModes(std::vector < Adapter >* pAdapters)
{
    std::vector < Adapter > highModes;

    for(UINT k = 0; k < (*pAdapters).size();k++)
    {
        Adapter adapter;
        highModes.push_back(adapter);

        //
        // Iterate all outputs
        //
        for(UINT j = 0; j < (*pAdapters)[k].OutputModeContainer.size();j++)
        {
            OutputModeContainer outputModeContainer;
            highModes[k].OutputModeContainer.push_back(outputModeContainer);

            for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
            {      
                if((*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size() > 0)
                {
                    highModes[k].OutputModeContainer[j].RawModes[rotation].push_back((*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ 0 ]);
 
                    for(UINT i = 0; i < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size(); i++)
                    {

                        if( highModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Width < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Width || 
                            highModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Height < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Height)
                        {
                            highModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Width = (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Width;
                            highModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Height = (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Height;                   
                        }

                    }
                }
            }

            //
            // Remove all but the formats for the high and low resolutions
            //
            for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
            {      
                if((*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size() > 0)
                {
                    for(UINT i = 0; i < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size(); i++)
                    {
                        if( (highModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Width != (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Width || 
                            highModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Height != (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Height))

                        {
                            (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].erase
                            (
                                (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].begin() + i--
                            );                        
                        }                        
                    }
                }
            }
        }
    }
}

void GetHighLowMidResolutionModes(std::vector < Adapter >* pAdapters)
{
    std::vector < Adapter > highModes;
    std::vector < Adapter > lowModes;


    for(UINT k = 0; k < (*pAdapters).size();k++)
    {
        Adapter adapter;
        highModes.push_back(adapter);
        lowModes.push_back(adapter);

        //
        // Iterate all outputs
        //
        for(UINT j = 0; j < (*pAdapters)[k].OutputModeContainer.size();j++)
        {
            OutputModeContainer outputModeContainer;
            lowModes[k].OutputModeContainer.push_back(outputModeContainer);
            highModes[k].OutputModeContainer.push_back(outputModeContainer);

            for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
            {      
                if((*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size() > 0)
                {
                    highModes[k].OutputModeContainer[j].RawModes[rotation].push_back((*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ 0 ]);
                    lowModes[k].OutputModeContainer[j].RawModes[rotation].push_back((*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ 0 ]);

                    for(UINT i = 0; i < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size(); i++)
                    {

                        if( highModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Width < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Width || 
                            highModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Height < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Height)
                        {
                            highModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Width = (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Width;
                            highModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Height = (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Height;                   
                        }

                        if( lowModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Width > (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Width || 
                            lowModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Height > (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Height)
                        {
                            lowModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Width = (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Width;
                            lowModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Height = (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Height;
                        }
                    }
                }
            }

            //
            // Remove all but the formats for the high and low resolutions
            //
            for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
            {      
                if((*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size() > 0)
                {
                    for(UINT i = 0; i < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size(); i++)
                    {
                        if( (highModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Width != (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Width || 
                            highModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Height != (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Height) &&
                            (lowModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Width != (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Width || 
                            lowModes[k].OutputModeContainer[j].RawModes[rotation][0].ModeDesc.Height != (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][ i ].ModeDesc.Height))

                        {
                            (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].erase
                            (
                                (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].begin() + i--
                            );                        
                        }                        
                    }
                }
            }
        }
    }
}

void GetHighLowMidMultisampleQuality(std::vector < Adapter >* pAdapters)
{
    std::vector < Adapter > highModes;
    std::vector < Adapter > lowModes;


    for(UINT k = 0; k < (*pAdapters).size();k++)
    {
        Adapter adapter;
        highModes.push_back(adapter);
        lowModes.push_back(adapter);

        //
        // Iterate all outputs
        //
        for(UINT j = 0; j < (*pAdapters)[k].OutputModeContainer.size();j++)
        {
            OutputModeContainer outputModeContainer;
            lowModes[k].OutputModeContainer.push_back(outputModeContainer);
            highModes[k].OutputModeContainer.push_back(outputModeContainer);

            for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
            {      
                if((*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size() > 0)
                {
                    GetHighLowMidMultisampleQuality(&(*pAdapters)[k].OutputModeContainer[j].RawModes[rotation]);
                }
            }

        }
    }
}

void GetHighLowMidMultisampleCount(std::vector < Adapter >* pAdapters)
{
    std::vector < Adapter > highModes;
    std::vector < Adapter > lowModes;


    for(UINT k = 0; k < (*pAdapters).size();k++)
    {
        Adapter adapter;
        highModes.push_back(adapter);
        lowModes.push_back(adapter);

        //
        // Iterate all outputs
        //
        for(UINT j = 0; j < (*pAdapters)[k].OutputModeContainer.size();j++)
        {
            OutputModeContainer outputModeContainer;
            lowModes[k].OutputModeContainer.push_back(outputModeContainer);
            highModes[k].OutputModeContainer.push_back(outputModeContainer);

            for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
            {      
                if((*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size() > 0)
                {
                    GetHighLowMidMultisampleCount(&(*pAdapters)[k].OutputModeContainer[j].RawModes[rotation]);
                }
            }
        }
    }
}


void RemoveModesThatVaryOnlyByRefresh(std::vector < Adapter >* pAdapters)
{
    for(UINT k = 0; k < (*pAdapters).size();k++)
    {
        for(UINT j = 0; j < (*pAdapters)[k].OutputModeContainer.size();j++)
        {
            for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
            {
                for(UINT i = 0; i < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size(); i++)
                {
                    DXGI_MODE_DESC dxgiMode = (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][i].ModeDesc;

                    for(UINT rotationInner = 0; rotationInner < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotationInner++)
                    {
                        for(UINT h = 0; h < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotationInner].size(); h++)
                        {
                            if ( i == h )
					        {
						        continue;
					        };

                            DXGI_MODE_DESC dxgiExistingMode = (*pAdapters)[k].OutputModeContainer[j].RawModes[rotationInner][h].ModeDesc;
                            
                            if
					        (
						        dxgiMode.Width == dxgiExistingMode.Width &&
						        dxgiMode.Height == dxgiExistingMode.Height &&
						        dxgiMode.Format == dxgiExistingMode.Format &&
						        dxgiMode.ScanlineOrdering == dxgiExistingMode.ScanlineOrdering &&
						        dxgiMode.Scaling == dxgiExistingMode.Scaling
					        )
					        {
                                (*pAdapters)[k].OutputModeContainer[j].RawModes[rotationInner].erase
			                    (
				                    (*pAdapters)[k].OutputModeContainer[j].RawModes[rotationInner].begin() + h--
			                    );
                            }
                        }
                    }
                }
            }
        }
    }
}

//----------------------------------------------------------------------------
// UseMinimalFormatList
//
// Filters out all display modes that aren't R8G8B8A8 or B8G8R8A8
//----------------------------------------------------------------------------
void UseMinimalFormatList(std::vector < Adapter >* pAdapters)
{
    for(UINT k = 0; k < (*pAdapters).size();k++)
    {
        for(UINT j = 0; j < (*pAdapters)[k].OutputModeContainer.size();j++)
        {
            for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
            {
                for(UINT i = 0; i < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size(); i++)
                {
                    DXGI_MODE_DESC dxgiMode = (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][i].ModeDesc;
					// Erase any mode that isn't R8G8B8A8 or B8G8R8A8
                    if
					(
						(dxgiMode.Format != DXGI_FORMAT_R8G8B8A8_UNORM &&
						 dxgiMode.Format != DXGI_FORMAT_B8G8R8A8_UNORM)
					)
					{
                        (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].erase
			            (
				            (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].begin()+i--
			            );
                    }
                }
            }
        }
    }
}

void RemoveModesThatVaryOnlyByFormat(std::vector < Adapter >* pAdapters)
{
    for(UINT k = 0; k < (*pAdapters).size();k++)
    {
        for(UINT j = 0; j < (*pAdapters)[k].OutputModeContainer.size();j++)
        {
            for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
            {
                for(UINT i = 0; i < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size(); i++)
                {
                    DXGI_MODE_DESC dxgiMode = (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][i].ModeDesc;

                    for(UINT rotationInner = 0; rotationInner < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotationInner++)
                    {
                        for(UINT h = 0; h < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotationInner].size(); h++)
                        {
                            if ( i == h )
					        {
						        continue;
					        };

                            DXGI_MODE_DESC dxgiExistingMode = (*pAdapters)[k].OutputModeContainer[j].RawModes[rotationInner][h].ModeDesc;
                        
                            if
					        (
						        dxgiMode.Width == dxgiExistingMode.Width &&
						        dxgiMode.Height == dxgiExistingMode.Height &&
						        dxgiMode.RefreshRate.Numerator == dxgiExistingMode.RefreshRate.Numerator &&
						        dxgiMode.RefreshRate.Denominator == dxgiExistingMode.RefreshRate.Denominator &&
						        dxgiMode.ScanlineOrdering == dxgiExistingMode.ScanlineOrdering &&
						        dxgiMode.Scaling == dxgiExistingMode.Scaling
					        )
					        {
                                (*pAdapters)[k].OutputModeContainer[j].RawModes[rotationInner].erase
			                    (
				                    (*pAdapters)[k].OutputModeContainer[j].RawModes[rotationInner].begin() + h--
			                    );
                            }
                        }
                    }
                }
            }
        }
    }
}

void ToFlatList(std::vector < Adapter >* pAdapters, std::vector< ModeInformation >* pList)
{
    for(UINT k = 0; k < (*pAdapters).size();k++)
    {
        for(UINT j = 0; j < (*pAdapters)[k].OutputModeContainer.size();j++)
        {
            for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
            {
                for(UINT mode = 0; mode < (*pAdapters)[k].OutputModeContainer[j].RawModes[rotation].size(); mode++ )
                {
                    (*pList).push_back((*pAdapters)[k].OutputModeContainer[j].RawModes[rotation][mode]);
                }
            }
        }
    }
}

void FromFlatList(std::vector< ModeInformation >* pList, std::vector < Adapter >* pAdapters, UINT rotation )
{
    
    DebugOut.Write("Total modes: %d\n", pList->size());
    for(UINT k = 0; k < pList->size();k++)
    {
        UINT numAdapters = 0;
        if(pAdapters->size() < ( (*pList)[k].uiDXGIAdapterOrdinal + 1 ))
        {
            numAdapters = ( (*pList)[k].uiDXGIAdapterOrdinal + 1 ) - pAdapters->size();
        }
               
        for(UINT i = 0; i < numAdapters ; i++)
        {
            Adapter adapter;
            adapter.outputAttached = false;
            (*pAdapters).push_back(adapter);
            DebugOut.Write("add adpater\n");
        }        
        
        UINT numOuputs = 0;
        if((*pAdapters)[( (*pList)[k].uiDXGIAdapterOrdinal )].OutputModeContainer.size() < ((*pList)[k].uiDXGIOutputOrdinal + 1 ) )
        {
            numOuputs = ((*pList)[k].uiDXGIOutputOrdinal + 1 ) - (*pAdapters)[( (*pList)[k].uiDXGIAdapterOrdinal )].OutputModeContainer.size();
        }
        
        for(UINT i = 0; i < numOuputs; i++)
        {
            OutputModeContainer outputModeContainer;
            (*pAdapters)[(*pList)[k].uiDXGIAdapterOrdinal].OutputModeContainer.push_back(outputModeContainer);
            (*pAdapters)[(*pList)[k].uiDXGIAdapterOrdinal].outputAttached = true;
            DebugOut.Write("add output\n");
        }

        (*pAdapters)[(*pList)[k].uiDXGIAdapterOrdinal].OutputModeContainer[(*pList)[k].uiDXGIOutputOrdinal].RawModes[rotation].push_back((*pList)[k]);
    }
}

void SeperateDwmOnAndOffModes(
    __in std::vector < Adapter >* pModes,
    __out std::vector < Adapter >* pDwmOnModes,
    __out std::vector < Adapter >* pDwmOffModes)
{
    (*pDwmOnModes) = (*pModes);
    (*pDwmOffModes) = (*pModes);
    bool noDwmOffModes = true;
    bool noDwmOnModes = true;

    
    for(UINT k = 0; k < pModes->size();k++)
    {
        for(UINT j = 0; j < (*pModes)[k].OutputModeContainer.size();j++)
        {
            for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
            {
                (*pDwmOffModes)[k].OutputModeContainer[j].RawModes[rotation].clear();
                (*pDwmOnModes)[k].OutputModeContainer[j].RawModes[rotation].clear();
                
                for(UINT i = 0; i < (*pModes)[k].OutputModeContainer[j].RawModes[rotation].size();i++)
                {
                    if((*pModes)[k].OutputModeContainer[j].RawModes[rotation][i].devmode.dmBitsPerPel < 32)
                    {
                        (*pDwmOffModes)[k].OutputModeContainer[j].RawModes[rotation].push_back((*pModes)[k].OutputModeContainer[j].RawModes[rotation][i]);
                        noDwmOffModes = false;

                        DebugOut.Write("pDwmOffModes dmBitsPerPel=%d\n",(*pModes)[k].OutputModeContainer[j].RawModes[rotation][i].devmode.dmBitsPerPel);
                        
                    }
                    else
                    {
                        (*pDwmOnModes)[k].OutputModeContainer[j].RawModes[rotation].push_back((*pModes)[k].OutputModeContainer[j].RawModes[rotation][i]); 
                        noDwmOnModes = false;
                        
                        DebugOut.Write("pDwmOnModes dmBitsPerPel=%d\n",(*pModes)[k].OutputModeContainer[j].RawModes[rotation][i].devmode.dmBitsPerPel);
                    }
                }
            }
        }
    }

    if(noDwmOffModes)
    {
        (*pDwmOffModes).clear();
    }

    if(noDwmOnModes)
    {
        (*pDwmOnModes).clear();
    }
}

void GetHighLowMidMultisampleQuality(std::vector < ModeInformation >* pFormats)
{
    std::vector < ModeInformation > highModes;
    std::vector < ModeInformation > lowModes;

    if( (*pFormats).size() )
    {
        highModes.push_back((*pFormats)[ 0 ]);
        lowModes.push_back((*pFormats)[ 0 ]);
    }

    for(UINT i = 0; i < (*pFormats).size(); i++)
    {

        if( highModes[0].uiMultisampleQuality < (*pFormats)[ i ].uiMultisampleQuality )
        {
            highModes[0].uiMultisampleQuality = (*pFormats)[ i ].uiMultisampleQuality;
        }

        if( lowModes[0].uiMultisampleQuality > (*pFormats)[ i ].uiMultisampleQuality )
        {
            lowModes[0].uiMultisampleQuality = (*pFormats)[ i ].uiMultisampleQuality;
        }
    }

    //
    // Remove all but the formats for the high and low resolutions
    //
    for(UINT i = 0; i < (*pFormats).size(); i++)
    {
        if( (highModes[0].uiMultisampleQuality != (*pFormats)[ i ].uiMultisampleQuality ) &&
            (lowModes[0].uiMultisampleQuality != (*pFormats)[ i ].uiMultisampleQuality ))

        {
            (*pFormats).erase
                (
                (*pFormats).begin() + i--
                );                        
        }                        
    }       
}
                
void GetHighLowMidMultisampleCount(std::vector < ModeInformation >* pFormats)
{
    std::vector < ModeInformation > highModes;
    std::vector < ModeInformation > lowModes;

    if( (*pFormats).size() )
    {
        highModes.push_back((*pFormats)[ 0 ]);
        lowModes.push_back((*pFormats)[ 0 ]);
    }

    for(UINT i = 0; i < (*pFormats).size(); i++)
    {

        if( highModes[0].uiMultisampleCount < (*pFormats)[ i ].uiMultisampleCount )
        {
            highModes[0].uiMultisampleCount = (*pFormats)[ i ].uiMultisampleCount;
        }

        if( lowModes[0].uiMultisampleCount > (*pFormats)[ i ].uiMultisampleCount )
        {
            lowModes[0].uiMultisampleCount = (*pFormats)[ i ].uiMultisampleCount;
        }
    }

    //
    // Remove all but the formats for the high and low resolutions
    //
    for(UINT i = 0; i < (*pFormats).size(); i++)
    {
        if( (highModes[0].uiMultisampleCount != (*pFormats)[ i ].uiMultisampleCount ) &&
            (lowModes[0].uiMultisampleCount != (*pFormats)[ i ].uiMultisampleCount ))

        {
            (*pFormats).erase
                (
                (*pFormats).begin() + i--
                );                        
        }
    }       
}

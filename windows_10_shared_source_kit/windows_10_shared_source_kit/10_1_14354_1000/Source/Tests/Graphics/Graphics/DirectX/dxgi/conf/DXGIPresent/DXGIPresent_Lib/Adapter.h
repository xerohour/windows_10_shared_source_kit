#pragma once

#include <modeinformation.h>
#include <vector>   
#include <dxgi.h>


class OutputModeContainer
{

public:
    OutputModeContainer(){};
    virtual ~OutputModeContainer(){};
    UINT GetTotalModeCount();



public:
    //
    //  a vector for each rotataion DXGI_MODE_ROTATION_IDENTITY - DXGI_MODE_ROTATION_ROTATE270
    //
    std::vector < ModeInformation > RawModes[4];
};


struct Adapter
{
    std::vector < OutputModeContainer > OutputModeContainer;
    bool outputAttached;

};

void GetHighResolutionModes(std::vector < Adapter >* pAdapters);
void GetHighLowMidResolutionModes(std::vector < Adapter >* pAdapters);
void GetHighLowMidMultisampleCount(std::vector < Adapter >* pAdapters);
void GetHighLowMidMultisampleCount(std::vector < ModeInformation >* pFormats);

void GetHighLowMidMultisampleQuality(std::vector < Adapter >* pAdapters);
void GetHighLowMidMultisampleQuality(std::vector < ModeInformation >* pFormats);

void CopyDefaultOutputToNoOutputAdapters(std::vector < Adapter >* pAdapters);

void UseMinimalFormatList(std::vector < Adapter >* pAdapters);
void RemoveModesThatVaryOnlyByFormat(std::vector < Adapter >* pAdapters);
void FirstModePerFormat(std::vector < Adapter >* pAdapters);
void RemoveModesThatVaryOnlyByRefresh(std::vector < Adapter >* pAdapters);
void PrintModes(std::vector < ModeInformation >* pFormats);
void PrintModes(std::vector < Adapter >* pAdapters);
void ToFlatList(std::vector < Adapter >* pAdapters, std::vector< ModeInformation >* pList);
void FromFlatList(std::vector< ModeInformation >* pList, std::vector < Adapter >* pAdapters, UINT rotation );

void SeperateDwmOnAndOffModes(
    __in std::vector < Adapter >* pModes,
    __out std::vector < Adapter >* pDwmOnModes,
    __out std::vector < Adapter >* pDwmOffModes);

void FilterLowResolutionModes(std::vector < Adapter >* pAdapters);



#include "testcomponentspch.h"
#include "DisplayModeStructures.h"

/*
 * These structures are used internally to store and sort different display modes by 
 * the CRandomModeChange and CRandomMultiMon test components
 */


bool DisplayMode::operator==(const DisplayMode &rhs) const
{
    if((DisplayNumber == rhs.DisplayNumber) &&
        (ModeIndex == rhs.ModeIndex) &&
        (Width == rhs.Width) &&
        (Height == rhs.Height) &&
        (ColorDepth == rhs.ColorDepth) &&
        (Frequency == rhs.Frequency))
        return true;
    else
        return false;
}


bool FullDisplayMode::operator<(const FullDisplayMode &rhs) const
{
    for(DWORD i = 0; i < DisplayModes.size(); i++)
    {
        if(i >= rhs.DisplayModes.size())
            return false;

        if(DisplayModes[i].DisplayNumber < rhs.DisplayModes[i].DisplayNumber)
            return true;
        else if(DisplayModes[i].DisplayNumber > rhs.DisplayModes[i].DisplayNumber)
            return false;

        if(DisplayModes[i].Width < rhs.DisplayModes[i].Width)
            return true;
        else if(DisplayModes[i].Width > rhs.DisplayModes[i].Width)
            return false;

        if(DisplayModes[i].Height < rhs.DisplayModes[i].Height)
            return true;
        else if(DisplayModes[i].Height > rhs.DisplayModes[i].Height)
            return false;

        if(DisplayModes[i].ColorDepth < rhs.DisplayModes[i].ColorDepth)
            return true;
        else if(DisplayModes[i].ColorDepth > rhs.DisplayModes[i].ColorDepth)
            return false;

        if(DisplayModes[i].Frequency < rhs.DisplayModes[i].Frequency)
            return true;
        else if(DisplayModes[i].Frequency > rhs.DisplayModes[i].Frequency)
            return false;
    }

    return false;
}

bool FullDisplayMode::operator==(const FullDisplayMode &rhs) const
{
    return DisplayModes == rhs.DisplayModes;
}



#ifndef __DISPLAY_MODE_STRUCTURES_H_
#define __DISPLAY_MODE_STRUCTURES_H_

/*
 * These structures are used internally to store and sort different display modes by 
 * the CRandomModeChange and CRandomMultiMon test components
 */

struct DisplayMode
{
    DWORD DisplayNumber;
    DWORD ModeIndex;
	DWORD Width;
	DWORD Height;
	DWORD ColorDepth;
	DWORD Frequency;

    bool operator==(const DisplayMode &rhs) const;
};

struct FullDisplayMode
{
    std::vector<DisplayMode> DisplayModes;
    bool isSupported;

    bool operator<(const FullDisplayMode &rhs) const;
    bool operator==(const FullDisplayMode &rhs) const;
};

#endif
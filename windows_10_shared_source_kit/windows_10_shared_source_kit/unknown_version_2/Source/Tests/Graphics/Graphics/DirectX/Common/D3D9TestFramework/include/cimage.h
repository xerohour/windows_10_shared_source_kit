//
// CImage.h - Declaration of the CImage class and dependencies
//

#ifndef _CIMAGE_H_
#define _CIMAGE_H_

#include "Framework.h"

#define CIL_MAKEPCT(x) ((float)x/255.0f)
#define CIL_RED(x) ( (x >> 16) & 0xFF)
#define CIL_GREEN(x) ( (x >> 8) & 0xFF)
#define CIL_BLUE(x) ( x & 0xFF)
#define CIL_ALPHA(x) ( (x >> 24) & 0xFF)

typedef FLOAT (*_fChannelFunction)(FLOAT u, FLOAT v);

typedef struct CHANNELFUNCS
{   
    _fChannelFunction fAlpha;
    _fChannelFunction fRed;
    _fChannelFunction fGreen;
    _fChannelFunction fBlue;
} CHANNELFUNCS, *PCHANNELFUNCS;

class CImage
{
	friend class CSurface;
public:

	CImage();
	~CImage();

		
	bool Copy(CImage *pImage);
    bool Load(LPCTSTR Filename, float fAlphaOverride = 1.0f);    // The value to put into the alpha channel if there is no data in the file format (eg BMPs)	
    bool LoadStripes(DWORD dwW,DWORD dwH,DWORD dwStripes,DWORD dwColorArray[],bool  bHorizontal,bool  bDiagonal = false);
	bool LoadStripes(DWORD dwW,DWORD dwH,DWORD dwStripes,FColor* pColors,bool  bHorizontal,bool  bDiagonal = false);
	bool LoadGradient(DWORD dwW,DWORD dwH,DWORD dwColors[4]);
	bool LoadGradient(DWORD dwW,DWORD dwH,FColor pColors[4]);
    bool LoadAlgorithms(DWORD dwWidth, DWORD dwHeight, PCHANNELFUNCS pFunctions);
	bool ColorFill(FColor* FillColor);
	bool ColorFill(DWORD dwWidth, DWORD dwHeight, D3DCOLOR FillColor);
	bool HasData();

	inline DWORD GetWidth() const { return m_dwWidth; }
	inline DWORD GetHeight() const { return m_dwHeight; }
	inline FColor* GetData() { return m_pImageData; }
	virtual DWORD* GetPackedData();
    virtual bool GetData(DWORD &dwWidth, DWORD &dwHeight, FColor* pImageData);
    virtual bool SetData(DWORD dwWidth, DWORD dwHeight, FColor*);
    virtual bool ScaleData(DWORD,DWORD);

private:

	// Image load data
    DWORD      m_dwWidth;
    DWORD      m_dwHeight;
    FColor*	   m_pImageData;
	DWORD*	   m_pPackedImageData;

	// Private cleanup functions
	void ReleaseImageData();

	// Individual loading functions
	bool LoadBMP(LPCTSTR Filename, float fAlphaOverride = 0);

};

#endif
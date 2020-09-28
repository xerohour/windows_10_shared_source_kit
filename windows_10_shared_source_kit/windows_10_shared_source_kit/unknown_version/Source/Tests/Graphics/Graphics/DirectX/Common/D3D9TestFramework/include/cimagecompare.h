#ifndef __CIMAGECOMPARE_H__
#define __CIMAGECOMPARE_H__

#include "FrameworkTypes.h"
#include "FSurface.h"

enum CMPIMGMODE {
	CMPIMGMODE_RGB=0,
	CMPIMGMODE_R,
	CMPIMGMODE_G,
	CMPIMGMODE_B,
	CMPIMGMODE_A,
	CMPIMGMODE_END
};

//Defines for surface comparison
#define S_DIMENSIONMATCH						_HRESULT_TYPEDEF_(0x00000001L)

#define COMPIMG_OPT_FORCEFSURF					0x00000001L

class CSurface;
class CImageCompare
{
	UINT            m_nTotalPixels;
	UINT            m_nPerChannelFail;
	UINT            m_nPerPixelFail;
	FLOAT			m_fSumDeltaR, m_fSumDeltaG, m_fSumDeltaB, m_fSumDeltaA;

	float			m_fPassRequired;
	float			m_fPerPixelChannelTol;
	float			m_fPerPixelTol;
	float			m_fImgDeltaWght;
	UINT			m_uMinPixels;
	UINT			m_uMaxPixels;
	CMPIMGMODE		m_CmpImgMode;
	
	FColor			m_fClearColor;
	FSurface		*m_pSrc, *m_pRef, *m_pCmp;
	bool			m_bCompareInPlace;
	float           m_fGain;					// If == -1, do adaptive Gain.
	bool			m_bFailOnly;
	bool			m_bGenerateCompare;
	bool			m_bIncludeClearColor;		// Use the clear color when calc'ing total pixels
	bool			m_bCompareAlpha;

	void			ResetState();
    virtual int 	CompareSurfaceFormats(SURFACEDESC *pSurf1, SURFACEDESC *pSurf2);

	virtual HRESULT CompareInPlace(CSurface *pSurf1, CSurface *pSurf2, CSurface *pResultSurface);
	virtual HRESULT CompareConverted(CSurface *pSurf1, CSurface *pSurf2, CSurface *pResultSurface);

public:

    CImageCompare();
    ~CImageCompare();

    virtual HRESULT CompareImages(CSurface *pSurf1, CSurface *pSurf2, CSurface *pResultSurface=NULL, DWORD dwFlags = 0);

    virtual FLOAT   GetCompareResult();

	inline FSurface*	GetFloatCompareSurface() { return m_pCmp; }
    inline float    	GetGain() { return m_fGain; }
    inline void     	SetGain(float fVal) { m_fGain = fVal; }
    inline FColor*  	GetClearColor() { return &m_fClearColor; }
    inline void     	SetClearColor(DWORD dwVal) { m_fClearColor = dwVal; }
	inline void     	SetClearColor(FColor *pfColor) { m_fClearColor = *pfColor; }
	inline float    	GetPassRequired() { return m_fPassRequired; }
    inline void     	SetPassRequired(float fVal) { m_fPassRequired = fVal; }
	inline bool     	GetFailOnly() { return m_bFailOnly; }
    inline void     	SetFailOnly(bool bVal) { m_bFailOnly = bVal; }
	inline bool     	GetGenerateCompare() { return m_bGenerateCompare; }
    inline void     	SetGenerateCompare(bool bVal) { m_bGenerateCompare = bVal; }
	inline bool     	GetIncClearColor() { return m_bIncludeClearColor; }
    inline void     	SetIncClearColor(bool bVal) { m_bIncludeClearColor = bVal; }
	inline CMPIMGMODE	GetCmpImgMode() { return m_CmpImgMode; }
    inline void			SetCmpImgMode(CMPIMGMODE bVal) { m_CmpImgMode = bVal; }
	void				NextCmpImgMode();
	TCHAR*				GetCmpImgModeStr();
	inline void			SetCompareAlpha(bool bCompareAlpha) { m_bCompareAlpha = bCompareAlpha; }
	inline bool			GetCompareAlpha() { return m_bCompareAlpha; }

	inline float    GetTolPerPixelChannel() { return m_fPerPixelChannelTol; }
    inline void     SetTolPerPixelChannel(float fVal) { m_fPerPixelChannelTol = fVal; }
	inline float    GetTolPerPixel() { return m_fPerPixelTol; }
    inline void     SetTolPerPixel(float fVal) { m_fPerPixelTol = fVal; }
	inline float    GetImgDeltaWght() { return m_fImgDeltaWght; }
    inline void     SetImgDeltaWght(float fVal) { m_fImgDeltaWght = fVal; }
	inline UINT     GetMinPixels() { return m_uMinPixels; }
    inline void     SetMinPixels(UINT uVal) { m_uMinPixels = uVal; }
	inline UINT     GetMaxPixels() { return m_uMaxPixels; }
    inline void     SetMaxPixels(UINT uVal) { m_uMaxPixels = uVal; }
	inline void		GetResultDetails(UINT &uTotalPixels, UINT &uPerChannelFail, UINT &uPerPixelFail,
									 FLOAT &fSumDeltaR, FLOAT &fSumDeltaG, FLOAT &fSumDeltaB, FLOAT &fSumDeltaA)
	{
		uTotalPixels = m_nTotalPixels;
		uPerChannelFail = m_nPerChannelFail;
		uPerPixelFail = m_nPerPixelFail;
		fSumDeltaR = m_fSumDeltaR;
		fSumDeltaG = m_fSumDeltaG;
		fSumDeltaB = m_fSumDeltaB;
		fSumDeltaA = m_fSumDeltaA;
	}
};

 
#endif


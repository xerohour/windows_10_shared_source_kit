// FILE:        FillMode.h
// DESC:        Header for the CFillMode test class
// AUTHOR:      Anthony Clarke

#ifndef _FILLMODE
#define _FILLMODE

#include "CD3DTest.h"
#include "CTextures.h"
#include "types.h"
#include "Path.h"
#include "CFlags.h"

#define SNAKE_SIZE 30				// Must be an even number.
#define NUM_PRIMTYPES 6				// This is the number of D3D enum primitive types eg D3DPT_TRIANGLESTRIP
#if FAST_TEST
#define DEFAULT_PRIMTYPE_VARS 12
#else//FAST_TEST
#define DEFAULT_PRIMTYPE_VARS 100	// Default number of variations to try per primitive type.
									// Way primitives are rendered is changed by the framework
									// every three call to RenderPrimitive.
#endif//FAST_TEST
#define MIN_MAXPOINTSIZE 16			// Good value for point size
#define NUM_COLORS 3
#define LEMNISNAKE_STEPS 100		// Number of steps in Lemnisnakes path.

//Define bit mask flags
#define INVALID				0x00000001
#define POINTSPRITE			0x00000002
#define KILLSNAKE2          0x00000004
#define GUARDBAND           0x00000008

class CFillMode : public CD3DTest
{
	friend class CPointFill;

public:
    CFillMode();
    ~CFillMode();

    virtual void CommandLineHelp(void);
    virtual UINT TestInitialize(VOID);
    virtual bool ExecuteTest(UINT uiTest);
    virtual bool TestTerminate(VOID);
    virtual bool ProcessFrame(VOID);
	
	void GenerateSnake (UINT uiTest);	
	void StepSnake (UINT uiTest);
	void SetPointSize();
	void ClearPointSize();
    bool AllSnakeInView();
    bool NeedDx6TLVertWorkAround(void);

private:
    inline bool InsideDisplayWindow(float fsx, float fsy);    
	CFlags m_Flags;
	float m_fTenthWidth, m_fTenthHeight, m_fPointSize;
	UINT m_uCurrentPrimType;	
	TCHAR m_sztCurrentPrimType[MAX_PATH];	
    int m_nTotalVariations, m_nVarsPerType;
    bool DoCapsCheck(void);
	D3DTLVERTEX m_rgSnakeVertices[SNAKE_SIZE];
	D3DTLVERTEX m_rgSnakeVertices2[SNAKE_SIZE];
	DWORD m_dwVertexCount;
	float m_fTriFanTheta;
	CPath m_Path;	
};


class CSolidFill : public CFillMode
{
public:
	CSolidFill();
		
	UINT TestInitialize();
};


class CLineFill : public CFillMode
{
public:
	CLineFill();
		
	UINT TestInitialize();
};


class CPointFill : public CFillMode
{
public:
	CPointFill();

	UINT TestInitialize();
	bool ExecuteTest(UINT uiTest);
};

#endif

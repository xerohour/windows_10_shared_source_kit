#ifndef __SURFBOARD_COMPONENT_H_
#define __SURFBOARD_COMPONENT_H_


#define MAX_TEXTURE_COUNT 1000
#define BVT_TEXTURE_COUNT 250
#define THE_TEXTURE_COUNT 10

struct Vertex
{
    float Pos[4];
    float Tex[2];
};

const DWORD FVF = (D3DFVF_XYZRHW | D3DFVF_TEX1);

class CSurfboardManager : public CGenericI3DComponent
{
public:
    /*
      In the constructor, we describe this component, and each variable it contains
    */
    CSurfboardManager();
    ~CSurfboardManager(){ FreeResources(); }

protected:
    /*
      Overrides from CGenericI3DComponent

      This is where we actually execute the test.
    */
    virtual void RunTest();
    virtual void EndTest();

private:
    CI3DTexturePtr m_pTexture[MAX_TEXTURE_COUNT];
    CI3DVertexBufferPtr m_pVertexBuffer;

    void CreateTheVB();
    void FreeResources();
};

class CSurfboardComponent : public CGenericI3DComponent
{
public:
    /*
      In the constructor, we describe this component, and each variable it contains
    */
    CSurfboardComponent();

protected:
    /*
      Overrides from CGenericI3DComponent

      This is where we actually execute the test.
    */
    virtual void RunTest();
    virtual void EndTest();

private:
    void CreateTheTexture(CI3DTexturePtr &pTexture, UINT uSize);
    void Render(UINT nFrame, UINT uSize);
};


typedef enum _TriangleType {
    EquilateralTriangle,
    RightTriangle,
    IsoscelesTriangle,
    ScaleneTriangle,
    ObtuseTriangle
} TriangleType;

class CTriangle
{
private:
    TriangleType m_Type;
    double m_Base;
    int m_iBase;
    int m_iHeight;
public:
    CTriangle(TriangleType _Type,
              UINT _iPixelsPerTriangle);
    ~CTriangle();

    int Base();
    int Height();
    int Pixels();
};



struct ResultsData
{
    ResultsData(UINT  PrimitivesPerFrame,
                UINT  TrianglesPerPrimitive,
                UINT  PixelsPerTriangle,
                UINT  ShaderInstructions,
                UINT  PremptionRequestCount,
                float AverageP,
                ULONGLONG  LongestPreemptionWait,
                UINT  DMAPacketCount,
                float AverageDMAP,
                UINT  CountOfPreemptedPackets,
                ULONGLONG  LongestConsecutiveTime,
                ULONGLONG  LongestDMAPacket):
        uiPrimitivesPerFrame(PrimitivesPerFrame),
        uiTrianglesPerPrimitive(TrianglesPerPrimitive),
        uiPixelsPerTriangle(PixelsPerTriangle),
        uiShaderInstructions(ShaderInstructions),
        // Preemption Stats
        uiPreemptionRequestCount(PremptionRequestCount),
        fAveragePremptionTime(AverageP),
        ullLongestPreemptionWait(LongestPreemptionWait),
        // DMA stats
        uiDMAPacketCount(DMAPacketCount),
        fAverageDMAProcessingTime(AverageDMAP),
        uiCountOfPreemptedPackets(CountOfPreemptedPackets),
        ullLongestConsecutiveTime(LongestConsecutiveTime),
        ullLongestDMAPacket(LongestDMAPacket)
    {
    };
    UINT  uiPrimitivesPerFrame;
    UINT  uiTrianglesPerPrimitive;
    UINT  uiPixelsPerTriangle;
    UINT  uiShaderInstructions;
    UINT  uiPreemptionRequestCount;
    float fAveragePremptionTime;
    ULONGLONG  ullLongestPreemptionWait;
    UINT  uiDMAPacketCount;
    float fAverageDMAProcessingTime;
    UINT  uiCountOfPreemptedPackets;
    ULONGLONG  ullLongestConsecutiveTime;
    ULONGLONG  ullLongestDMAPacket;
};

#define RENDER_OK   0
#define RENDER_PRESENT_ERROR 1
#define RENDER_EXCEEDED_TIME 2

class CBasePreemption : public CGenericI3DComponent
{
public:
    /*
      In the constructor, we describe this component, and each variable it contains
    */
    CBasePreemption();
    ~CBasePreemption(){};

    void SetupCommandLine(UINT _TotalTime,
                          UINT _nTriangles,
                          UINT _nppPrimitive,
                          UINT _nppFrame,
                          UINT _nShaderInstructions);

    void CreateTheVB(CTriangle* _pTriangle,int _iTriangles);
    void FreeResources();

    bool GetPreemptionLevelSupported();
    D3DKMDT_GRAPHICS_PREEMPTION_GRANULARITY m_Level;


    void CreateTheTexture(CI3DTexturePtr &pTexture, UINT uSize);
    DWORD Render(UINT nFrame, UINT uSize);
    bool Work(bool _GrowTriangleList,
              bool _GrowTriangleSize,
              bool _GrowPrimitiveCount);

    void StandardTest(bool _GrowTriangleList,
                      bool _GrowTriangleSize,
                      bool _GrowPrimitiveCount,
                      bool _Loop);

    void WriteResults(bool _GrowTriangleList,
                      bool _GrowTriangleSize,
                      bool _GrowPrimitiveCount);

    CI3DTexturePtr m_pTexture[MAX_TEXTURE_COUNT];
    CI3DVertexBufferPtr m_pVertexBuffer;
    Vertex* m_pVertices;

    CI3DShaderPtr m_pShader;

    // Values read off the command line
    UINT m_nTotalTime;
    UINT m_nTriangles;
    UINT m_nppPrimitive;
    UINT m_nppFrame;
    UINT m_nShaderInstructions;
    bool m_bWantCSVFile;
    bool m_bDWM;

    // result data from each loop
    std::vector<ResultsData*> m_Results;

    UINT UnicodeToAnsi(wchar_t *pwszWide, LPSTR *ppszAnsi);

    ULONGLONG m_ullLongestPreemptionRequestTime;
};


class CPreemptionShader : public CBasePreemption
{
public:
    CPreemptionShader();
    ~CPreemptionShader(){};

protected:
    //
    //  Overrides from CGenericI3DComponent
    //
    //  This is where we actually execute the test.
    //
    virtual void RunTest();
    virtual void EndTest();
};


// The CGrowTriangleList class grows the triangle list from 1 triangle to 2^14th 
// by mulitplying by two every time throught the loop.

class CGrowTriangleList : public CBasePreemption
{
public:
    //
    //  In the constructor, we describe this component, and each variable it contains
    //
    CGrowTriangleList();
    ~CGrowTriangleList(){};

protected:
    //
    //  Overrides from CGenericI3DComponent
    //
    //  This is where we actually execute the test.
    //
    virtual void RunTest();
    virtual void EndTest();
};

// The CGrowTriangleSize class grows the size of the triangles used from the base size
// to x by mulitplying by two every time throught the loop. Should start with small
// triangle size

class CGrowTriangleSize : public CBasePreemption
{
public:
    //
    //  In the constructor, we describe this component, and each variable it contains
    //
    CGrowTriangleSize();
    ~CGrowTriangleSize(){};

protected:
    //
    //  Overrides from CGenericI3DComponent
    //
    //  This is where we actually execute the test.
    //
    virtual void RunTest();
    virtual void EndTest();
};

// The CGrowTriangleSize class grows the size of the triangles used from the base size
// to x by mulitplying by two every time throught the loop. Should start with small
// triangle size

class CGrowPrimitiveCount : public CBasePreemption
{
public:
    //
    //  In the constructor, we describe this component, and each variable it contains
    //
    CGrowPrimitiveCount();
    ~CGrowPrimitiveCount(){};

protected:
    //
    //  Overrides from CGenericI3DComponent
    //
    //  This is where we actually execute the test.
    //
    virtual void RunTest();
    virtual void EndTest();
};


#endif

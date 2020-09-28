struct VS_CRITERIA
{
    bool	bPassAssembler;
    bool	bRequireSWVP;
    int		nVertexShaderVersion;
    int		nMaxVertexShaderConst;
    int		nDynamicFlowControlDepth;
    int		nNumTemps;
    int		nStaticFlowControlDepth;
    int		nCaps;
    int		nMaxVertexShader30InstructionSlots;
    float	fMinDXVersion;
};

struct PS_CRITERIA
{
    bool	bPassAssembler;
    bool	bRequireREF;
    int		nPixelShaderVersion;
    int		nDynamicFlowControlDepth;
    int		nNumTemps;
    int		nStaticFlowControlDepth;
    int		nNumInstructionSlots;
    int		nCaps;
    int		nMaxPixelShader30InstructionSlots;
    float	fMinDXVersion;
};

#define CAPS_ARBITRARYSWIZZLE       1
#define CAPS_GRADIENTINSTRUCTIONS   2
#define CAPS_PREDICATION            4
#define CAPS_NODEPENDENTREADLIMIT   8
#define CAPS_NOTEXINSTRUCTIONLIMIT 16

technique T0 { pass P0 { } }
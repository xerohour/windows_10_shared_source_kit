typedef enum _D3DVSD_TOKENTYPE
{
    D3DVSD_TOKEN_NOP        = 0,    // NOP or extension
    D3DVSD_TOKEN_STREAM,            // stream selector
    D3DVSD_TOKEN_STREAMDATA,        // stream data definition (map to vertex input memory)
    D3DVSD_TOKEN_TESSELLATOR,       // vertex input memory from tessellator
    D3DVSD_TOKEN_CONSTMEM,          // constant memory from shader
    D3DVSD_TOKEN_EXT,               // extension
    D3DVSD_TOKEN_END = 7,           // end-of-array (requires all DWORD bits to be 1)
    D3DVSD_FORCE_DWORD = 0x7fffffff,// force 32-bit size enum
} D3DVSD_TOKENTYPE;

#define D3DVSD_TOKENTYPESHIFT   29
#define D3DVSD_TOKENTYPEMASK    (7 << D3DVSD_TOKENTYPESHIFT)

#define D3DVSD_STREAMNUMBERSHIFT 0
#define D3DVSD_STREAMNUMBERMASK (0xF << D3DVSD_STREAMNUMBERSHIFT)

#define D3DVSD_DATALOADTYPESHIFT 28
#define D3DVSD_DATALOADTYPEMASK (0x1 << D3DVSD_DATALOADTYPESHIFT)

#define D3DVSD_DATATYPESHIFT 16
#define D3DVSD_DATATYPEMASK (0xF << D3DVSD_DATATYPESHIFT)

#define D3DVSD_SKIPCOUNTSHIFT 16
#define D3DVSD_SKIPCOUNTMASK (0xF << D3DVSD_SKIPCOUNTSHIFT)

#define D3DVSD_VERTEXREGSHIFT 0
#define D3DVSD_VERTEXREGMASK (0x1F << D3DVSD_VERTEXREGSHIFT)

#define D3DVSD_VERTEXREGINSHIFT 20
#define D3DVSD_VERTEXREGINMASK (0xF << D3DVSD_VERTEXREGINSHIFT)

#define D3DVSD_CONSTCOUNTSHIFT 25
#define D3DVSD_CONSTCOUNTMASK (0xF << D3DVSD_CONSTCOUNTSHIFT)

#define D3DVSD_CONSTADDRESSSHIFT 0
#define D3DVSD_CONSTADDRESSMASK (0x7F << D3DVSD_CONSTADDRESSSHIFT)

#define D3DVSD_CONSTRSSHIFT 16
#define D3DVSD_CONSTRSMASK (0x1FFF << D3DVSD_CONSTRSSHIFT)

#define D3DVSD_EXTCOUNTSHIFT 24
#define D3DVSD_EXTCOUNTMASK (0x1F << D3DVSD_EXTCOUNTSHIFT)

#define D3DVSD_EXTINFOSHIFT 0
#define D3DVSD_EXTINFOMASK (0xFFFFFF << D3DVSD_EXTINFOSHIFT)

#define D3DVSD_MAKETOKENTYPE(tokenType) ((tokenType << D3DVSD_TOKENTYPESHIFT) & D3DVSD_TOKENTYPEMASK)

// macros for generation of CreateVertexShader Declaration token array

// Set current stream
// _StreamNumber [0..(MaxStreams-1)] stream to get data from
//
#define D3DVSD_STREAM( _StreamNumber ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAM) | (_StreamNumber))

// Set tessellator stream
//
#define D3DVSD_STREAMTESSSHIFT 28
#define D3DVSD_STREAMTESSMASK (1 << D3DVSD_STREAMTESSSHIFT)
#define D3DVSD_STREAM_TESS( ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAM) | (D3DVSD_STREAMTESSMASK))

// bind single vertex register to vertex element from vertex stream
//
// _VertexRegister [0..15] address of the vertex register
// _Type [D3DVSDT_*] dimensionality and arithmetic data type

#define D3DVSD_REG( _VertexRegister, _Type ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAMDATA) |            \
     ((_Type) << D3DVSD_DATATYPESHIFT) | (_VertexRegister))

// Skip _DWORDCount DWORDs in vertex
//
#define D3DVSD_SKIP( _DWORDCount ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_STREAMDATA) | 0x10000000 | \
     ((_DWORDCount) << D3DVSD_SKIPCOUNTSHIFT))

// load data into vertex shader constant memory
//
// _ConstantAddress [0..95] - address of constant array to begin filling data
// _Count [0..15] - number of constant vectors to load (4 DWORDs each)
// followed by 4*_Count DWORDS of data
//
#define D3DVSD_CONST( _ConstantAddress, _Count ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_CONSTMEM) | \
     ((_Count) << D3DVSD_CONSTCOUNTSHIFT) | (_ConstantAddress))

// enable tessellator generated normals
//
// _VertexRegisterIn  [0..15] address of vertex register whose input stream
//                            will be used in normal computation
// _VertexRegisterOut [0..15] address of vertex register to output the normal to
//
#define D3DVSD_TESSNORMAL( _VertexRegisterIn, _VertexRegisterOut ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_TESSELLATOR) | \
     ((_VertexRegisterIn) << D3DVSD_VERTEXREGINSHIFT) | \
     ((0x02) << D3DVSD_DATATYPESHIFT) | (_VertexRegisterOut))

// enable tessellator generated surface parameters
//
// _VertexRegister [0..15] address of vertex register to output parameters
//
#define D3DVSD_TESSUV( _VertexRegister ) \
    (D3DVSD_MAKETOKENTYPE(D3DVSD_TOKEN_TESSELLATOR) | 0x10000000 | \
     ((0x01) << D3DVSD_DATATYPESHIFT) | (_VertexRegister))

// Generates END token
//
#define D3DVSD_END() 0xFFFFFFFF

// Generates NOP token
#define D3DVSD_NOP() 0x00000000

// bit declarations for _Type fields
#define D3DVSDT_FLOAT1      0x00    // 1D float expanded to (value, 0., 0., 1.)
#define D3DVSDT_FLOAT2      0x01    // 2D float expanded to (value, value, 0., 1.)
#define D3DVSDT_FLOAT3      0x02    // 3D float expanded to (value, value, value, 1.)
#define D3DVSDT_FLOAT4      0x03    // 4D float
#define D3DVSDT_D3DCOLOR    0x04    // 4D packed unsigned bytes mapped to 0. to 1. range
                                    // Input is in D3DCOLOR format (ARGB) expanded to (R, G, B, A)
#define D3DVSDT_UBYTE4      0x05    // 4D unsigned byte
#define D3DVSDT_SHORT2      0x06    // 2D signed short expanded to (value, value, 0., 1.)
#define D3DVSDT_SHORT4      0x07    // 4D signed short

// assignments of vertex input registers for fixed function vertex shader
//
#define D3DVSDE_POSITION        0
#define D3DVSDE_BLENDWEIGHT     1
#define D3DVSDE_BLENDINDICES    2
#define D3DVSDE_NORMAL          3
#define D3DVSDE_PSIZE           4
#define D3DVSDE_DIFFUSE         5
#define D3DVSDE_SPECULAR        6
#define D3DVSDE_TEXCOORD0       7
#define D3DVSDE_TEXCOORD1       8
#define D3DVSDE_TEXCOORD2       9
#define D3DVSDE_TEXCOORD3       10
#define D3DVSDE_TEXCOORD4       11
#define D3DVSDE_TEXCOORD5       12
#define D3DVSDE_TEXCOORD6       13
#define D3DVSDE_TEXCOORD7       14
#define D3DVSDE_POSITION2       15
#define D3DVSDE_NORMAL2         16

// Maximum supported number of texture coordinate sets
#define D3DDP_MAXTEXCOORD   8


//
// Instruction Token Bit Definitions
//
#define D3DSI_OPCODE_MASK       0x0000FFFF

//
// Co-Issue Instruction Modifier - if set then this instruction is to be
// issued in parallel with the previous instruction(s) for which this bit
// is not set.
//
#define D3DSI_COISSUE           0x40000000

// destination parameter write mask
#define D3DSP_WRITEMASK_0       0x00010000  // Component 0 (X;Red)
#define D3DSP_WRITEMASK_1       0x00020000  // Component 1 (Y;Green)
#define D3DSP_WRITEMASK_2       0x00040000  // Component 2 (Z;Blue)
#define D3DSP_WRITEMASK_3       0x00080000  // Component 3 (W;Alpha)
#define D3DSP_WRITEMASK_ALL     0x000F0000  // All Components

// destination parameter modifiers
#define D3DSP_DSTMOD_SHIFT      20
#define D3DSP_DSTMOD_MASK       0x00F00000

// destination parameter 
#define D3DSP_DSTSHIFT_SHIFT    24
#define D3DSP_DSTSHIFT_MASK     0x0F000000

// destination/source parameter register type
#define D3DSP_REGTYPE_SHIFT     28
#define D3DSP_REGTYPE_MASK      0x70000000


// Source operand addressing modes

#define D3DVS_ADDRESSMODE_SHIFT 13
#define D3DVS_ADDRESSMODE_MASK  (1 << D3DVS_ADDRESSMODE_SHIFT)

// Source operand swizzle definitions
//
#define D3DVS_SWIZZLE_SHIFT     16
#define D3DVS_SWIZZLE_MASK      0x00FF0000

// The following bits define where to take component X from:

#define D3DVS_X_X       (0 << D3DVS_SWIZZLE_SHIFT)
#define D3DVS_X_Y       (1 << D3DVS_SWIZZLE_SHIFT)
#define D3DVS_X_Z       (2 << D3DVS_SWIZZLE_SHIFT)
#define D3DVS_X_W       (3 << D3DVS_SWIZZLE_SHIFT)

// The following bits define where to take component Y from:

#define D3DVS_Y_X       (0 << (D3DVS_SWIZZLE_SHIFT + 2))
#define D3DVS_Y_Y       (1 << (D3DVS_SWIZZLE_SHIFT + 2))
#define D3DVS_Y_Z       (2 << (D3DVS_SWIZZLE_SHIFT + 2))
#define D3DVS_Y_W       (3 << (D3DVS_SWIZZLE_SHIFT + 2))

// The following bits define where to take component Z from:

#define D3DVS_Z_X       (0 << (D3DVS_SWIZZLE_SHIFT + 4))
#define D3DVS_Z_Y       (1 << (D3DVS_SWIZZLE_SHIFT + 4))
#define D3DVS_Z_Z       (2 << (D3DVS_SWIZZLE_SHIFT + 4))
#define D3DVS_Z_W       (3 << (D3DVS_SWIZZLE_SHIFT + 4))

// The following bits define where to take component W from:

#define D3DVS_W_X       (0 << (D3DVS_SWIZZLE_SHIFT + 6))
#define D3DVS_W_Y       (1 << (D3DVS_SWIZZLE_SHIFT + 6))
#define D3DVS_W_Z       (2 << (D3DVS_SWIZZLE_SHIFT + 6))
#define D3DVS_W_W       (3 << (D3DVS_SWIZZLE_SHIFT + 6))

// Value when there is no swizzle (X is taken from X, Y is taken from Y,
// Z is taken from Z, W is taken from W
//
#define D3DVS_NOSWIZZLE (D3DVS_X_X | D3DVS_Y_Y | D3DVS_Z_Z | D3DVS_W_W)

// source parameter swizzle
#define D3DSP_SWIZZLE_SHIFT     16
#define D3DSP_SWIZZLE_MASK      0x00FF0000

#define D3DSP_NOSWIZZLE \
    ( (0 << (D3DSP_SWIZZLE_SHIFT + 0)) | \
      (1 << (D3DSP_SWIZZLE_SHIFT + 2)) | \
      (2 << (D3DSP_SWIZZLE_SHIFT + 4)) | \
      (3 << (D3DSP_SWIZZLE_SHIFT + 6)) )

// pixel-shader swizzle ops
#define D3DSP_REPLICATERED \
    ( (0 << (D3DSP_SWIZZLE_SHIFT + 0)) | \
      (0 << (D3DSP_SWIZZLE_SHIFT + 2)) | \
      (0 << (D3DSP_SWIZZLE_SHIFT + 4)) | \
      (0 << (D3DSP_SWIZZLE_SHIFT + 6)) )

#define D3DSP_REPLICATEGREEN \
    ( (1 << (D3DSP_SWIZZLE_SHIFT + 0)) | \
      (1 << (D3DSP_SWIZZLE_SHIFT + 2)) | \
      (1 << (D3DSP_SWIZZLE_SHIFT + 4)) | \
      (1 << (D3DSP_SWIZZLE_SHIFT + 6)) )

#define D3DSP_REPLICATEBLUE \
    ( (2 << (D3DSP_SWIZZLE_SHIFT + 0)) | \
      (2 << (D3DSP_SWIZZLE_SHIFT + 2)) | \
      (2 << (D3DSP_SWIZZLE_SHIFT + 4)) | \
      (2 << (D3DSP_SWIZZLE_SHIFT + 6)) )

#define D3DSP_REPLICATEALPHA \
    ( (3 << (D3DSP_SWIZZLE_SHIFT + 0)) | \
      (3 << (D3DSP_SWIZZLE_SHIFT + 2)) | \
      (3 << (D3DSP_SWIZZLE_SHIFT + 4)) | \
      (3 << (D3DSP_SWIZZLE_SHIFT + 6)) )

// source parameter modifiers
#define D3DSP_SRCMOD_SHIFT      24
#define D3DSP_SRCMOD_MASK       0x0F000000

// pixel shader version token
#define D3DPS_VERSION(_Major,_Minor) (0xFFFF0000|((_Major)<<8)|(_Minor))

// vertex shader version token
#define D3DVS_VERSION(_Major,_Minor) (0xFFFE0000|((_Major)<<8)|(_Minor))

// extract major/minor from version cap
#define D3DSHADER_VERSION_MAJOR(_Version) (((_Version)>>8)&0xFF)
#define D3DSHADER_VERSION_MINOR(_Version) (((_Version)>>0)&0xFF)

// destination/source parameter register type
#define D3DSI_COMMENTSIZE_SHIFT     16
#define D3DSI_COMMENTSIZE_MASK      0x7FFF0000
#define D3DSHADER_COMMENT(_DWordSize) \
    ((((_DWordSize)<<D3DSI_COMMENTSIZE_SHIFT)&D3DSI_COMMENTSIZE_MASK)|D3DSIO_COMMENT)

// pixel/vertex shader end token
#define D3DPS_END()  0x0000FFFF
#define D3DVS_END()  0x0000FFFF

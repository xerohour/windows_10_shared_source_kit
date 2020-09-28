#ifndef __TEST_IMAGE_H
#define __TEST_IMAGE_H

#include "BaseVid.h"
#include "d3dkmthk.h"


#define TEX_DIMENTIONS	2048


/*
    Defining a custom vertex type.
*/
struct Vertex
{
    float Pos[4];
    DWORD Color;
    FLOAT tu1, tv1;   // The texture coordinates
    FLOAT tu2, tv2;   // The texture coordinates

};

const DWORD FVF = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2);

extern const char* PSCONST_SHADER;

/*
   This class creates an image with multiple triangles. Each triangle is created by combining two textures
   using the pixel shader defined.
*/
   
class CImage
{

    public:
    /*
      In the constructor, we describe this component, and each variable it contains.
    */
    CImage(UINT numTriangles, CLogWriter* pLog);
    
    CImage(CLogWriter* pLog);

    ~CImage();
    /*
      Creates an Image.
     */
    HRESULT CImage::CreateImage(CI3DDevice* pDevice,  bool bFlush=false);

    /*
      Render an Image.
    */
    CI3DSurface2Ptr RenderImage(CI3DDevice* pDevice, bool bFlush=false);


private:
    /*
       Used for logging.
    */
    CLogWriter* m_pLog;
    /*
       Number of triangles to render per RenderImage call
    */
    UINT m_nTriangles;
    /*
       The actual number of textures.
    */
    UINT m_nTexture;
    /*
       The requested number of textures.
    */
    UINT m_nRequiredTextures;
    /*
       Amount of available video memory
    */
    LONGLONG m_AvailableVidMem;
    /*
       The vertices of the triangles.
    */
    Vertex* mVertices;	// 3 vertices per triangle
   /*
       The textures to be applied.
    */
    CI3DTexturePtr* m_pTexture;

    /*
       Number of bytes per pixel for the two textures.
    */
    int* m_nBytesPerPixel;

    /*
       Creates the 2 textures which are applied to the triangle.
    */
    HRESULT CreateTriangleTexture(CI3DDevice*, int, bool);
    /*
       Create the texture to be applied to the triangle.
    */
    HRESULT CreateTheTexture(CI3DDevice*, int );
    /*
       Creates the 2 textures which are applied to the triangle.
    */
    void DrawTriangle(CI3DDevice* ,int );

    void InitializeImage();
    
    UINT CalculateNumRequiredTexture();

    LONGLONG GetVidMemSize();
};

#endif
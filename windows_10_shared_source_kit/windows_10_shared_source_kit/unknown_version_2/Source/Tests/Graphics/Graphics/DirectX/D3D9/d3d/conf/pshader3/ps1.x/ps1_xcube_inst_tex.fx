#include "include.fx"

Technique 
< 
	String	Name = "ps_1_1 tex : Cube Level = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
        Texture[0] = <TexCube_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex t0
                mov r0,t0
        };
         
		VertexShader = <VolTLVS>;
   }
}


Technique 
< 
	String	Name = "ps_1_1 tex : Cube Level = 1";
	String	Shape = "CubeTL1";
>
{
    Pass P0
    {        
        Texture[0] = <TexCube_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex t0
                mov r0,t0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_1_1 tex : Cube Level = 2";
	String	Shape = "CubeTL2";
>
{
    Pass P0
    {        
        Texture[0] = <TexCube_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
		ps_1_1

	        tex  t0
                mov r0,t0
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_1_1 tex : Cube Level = 3";
	String	Shape = "CubeTL3";
>
{
    Pass P0
    {        
        Texture[0] = <TexCube_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex  t0
                mov r0,t0
        };
         
	VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_1_1 tex : Cube Level = 4";
	String	Shape = "CubeTL4";
>
{
    Pass P0
    {        
	Texture[0] = <TexCube_0>;
	MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
 	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex  t0
                mov r0,t0
        };
         
		VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Cube MaxMipLevel[0] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
	Texture[0] = <TexCube_0>;
	MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex  t0
                mov r0,t0
        };
         
	VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Cube MaxMipLevel[0] = 1";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
	Texture[0] = <TexCube_0>;
	MipFilter[0] = Point;
        MaxMipLevel[0] = 1;
        MipMapLODBias[0] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex  t0
                mov r0,t0
        };
         
	VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Cube MaxMipLevel[0] = 2";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
        Texture[0] = <TexCube_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 2;
        MipMapLODBias[0] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex  t0
                mov r0,t0
        };
         
	VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Cube MaxMipLevel[0] = 3";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
        Texture[0] = <TexCube_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 3;
        MipMapLODBias[0] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex  t0
                mov r0,t0
        };
         
	VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Cube MaxMipLevel[0] = 4";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
        Texture[0] = <TexCube_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 4;
        MipMapLODBias[0] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex  t0
                mov r0,t0
        };
         
	VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Cube MaxMipLevel[0] = 5";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
	Texture[0] = <TexCube_0>;
	MipFilter[0] = Point;
        MaxMipLevel[0] = 5;
        MipMapLODBias[0] = 0;
		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex  t0
                mov r0,t0
        };
        
	VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Cube Level = 0, MipMapLODBias[0] = 0";
	String	Shape = "CubeTL";
>
{
    Pass P0
    {        
        Texture[0] = <TexCube_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 0;
		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex  t0
                mov r0,t0
        };
         
	VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Cube Level = 0, MipMapLODBias[0] = 1";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <TexCube_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 1;
		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex  t0
                mov r0,t0
        };
         
	VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Cube Level = 0, MipMapLODBias[0] = 2";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <TexCube_0>;
		MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 2;
		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex  t0
                mov r0,t0
        };
        
	VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Cube Level = 0, MipMapLODBias[0] = 3";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <TexCube_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 3;
        CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex  t0
                mov r0,t0
        };
        
	VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Cube Level = 0, MipMapLODBias[0] = 4";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <TexCube_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 4;
	CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex  t0
                mov r0,t0
        };
         
	VertexShader = <VolTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Cube Level = 0, MipMapLODBias[0] = 5";
	String	Shape = "CubeTL";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <TexCube_0>;
        MipFilter[0] = Point;
        MaxMipLevel[0] = 0;
        MipMapLODBias[0] = 5;
        CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_1
                
                tex  t0
                mov r0,t0
        };
         
	VertexShader = <VolTLVS>;
   }
}



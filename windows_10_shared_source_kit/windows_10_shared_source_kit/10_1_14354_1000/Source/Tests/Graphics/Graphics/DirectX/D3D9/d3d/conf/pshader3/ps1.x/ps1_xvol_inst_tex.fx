#include "include.fx"

Technique 
< 
	String	Name = "ps_1_1 tex : Volume Level = 0, Z Slice";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 1, Z Slice";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 2, Z Slice";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 3, Z Slice";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 4, Z Slice";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
	Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 0, Z Slice";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
	Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 1, Z Slice";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
	Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 2, Z Slice";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 3, Z Slice";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 4, Z Slice";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 5, Z Slice";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 0, Z Slice";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 1, Z Slice";
	String	Shape = "VolTLZ0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 2, Z Slice";
	String	Shape = "VolTLZ0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 3, Z Slice";
	String	Shape = "VolTLZ0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 4, Z Slice";
	String	Shape = "VolTLZ0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 5, Z Slice";
	String	Shape = "VolTLZ0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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

// Begin the X slice

Technique 
< 
	String	Name = "ps_1_1 tex : Volume Level = 0, X Slice";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 1, X Slice";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 2, X Slice";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 3, X Slice";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 4, X Slice";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 0, X Slice";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 1, X Slice";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 2, X Slice";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 3, X Slice";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 4, X Slice";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 5, X Slice";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 0, X Slice";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 1, X Slice";
	String	Shape = "VolTLX0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 2, X Slice";
	String	Shape = "VolTLX0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 3, X Slice";
	String	Shape = "VolTLX0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 4, X Slice";
	String	Shape = "VolTLX0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 5, X Slice";
	String	Shape = "VolTLX0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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

// Begin the Y slice

Technique 
< 
	String	Name = "ps_1_1 tex : Volume Level = 0, Y Slice";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 1, Y Slice";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 2, Y Slice";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 3, Y Slice";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 4, Y Slice";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
	Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 0, Y Slice";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
	Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 1, Y Slice";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
	Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 2, Y Slice";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 3, Y Slice";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 4, Y Slice";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume MaxMipLevel[0] = 5, Y Slice";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
	Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 0, Y Slice";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 1, Y Slice";
	String	Shape = "VolTLY0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
	Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 2, Y Slice";
	String	Shape = "VolTLY0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 3, Y Slice";
	String	Shape = "VolTLY0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 4, Y Slice";
	String	Shape = "VolTLY0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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
	String	Name = "ps_1_1 tex : Volume Level = 0, MipMapLODBias[0] = 5, Y Slice";
	String	Shape = "VolTLY0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <TexVol_0>;
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

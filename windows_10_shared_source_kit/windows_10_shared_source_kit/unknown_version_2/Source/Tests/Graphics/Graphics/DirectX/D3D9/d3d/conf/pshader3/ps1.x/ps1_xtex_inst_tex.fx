#include "include.fx"

Technique 
< 
	String	Name = "ps_1_1 tex : Level = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
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
         
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "ps_1_1 tex : Level = 1";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
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
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_1_1 tex : Level = 2";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
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
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_1_1 tex : Level = 3";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
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
         
	VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_1_1 tex : Level = 4";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
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
         
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : MaxMipLevel[0] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
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
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : MaxMipLevel[0] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
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
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : MaxMipLevel[0] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
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
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : MaxMipLevel[0] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
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
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : MaxMipLevel[0] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
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
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : MaxMipLevel[0] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
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
        
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Level = 0, MipMapLODBias[0] = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
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
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Level = 0, MipMapLODBias[0] = 1";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
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
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Level = 0, MipMapLODBias[0] = 2";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
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
        
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Level = 0, MipMapLODBias[0] = 3";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
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
        
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Level = 0, MipMapLODBias[0] = 4";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
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
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 tex : Level = 0, MipMapLODBias[0] = 5";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
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
         
	VertexShader = <TexTLVS>;
   }
}



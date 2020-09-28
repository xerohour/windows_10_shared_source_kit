#include "include.fx"

//*****************************************************************************
// texreg2gb

Technique 
< 
	String	Name = "ps_1_2 texreg2gb : Level = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex t0
                
                texreg2gb       t1, t0
                mov r0,t1
        };
         
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "ps_1_2 texreg2gb : Level = 1";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex t0
                texreg2gb       t1, t0
                mov r0,t1
        };
         
		VertexShader = <TexTLVS>;
   }
}
/*
Technique 
< 
	String	Name = "ps_1_2 texreg2gb : Level = 2";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
		ps_1_2

	        tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_1_2 texreg2gb : Level = 3";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_1_2 texreg2gb : Level = 4";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
	MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
 	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
         
		VertexShader = <TexTLVS>;
   }
}
*/
Technique 
< 
	String	Name = "ps_1_2 texreg2gb : MaxMipLevel[0] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
	MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2gb : MaxMipLevel[0] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
	MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 1;
        MaxMipLevel[1] = 1;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}
/*
Technique 
< 
	String	Name = "ps_1_2 texreg2gb : MaxMipLevel[0] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 2;
        MaxMipLevel[1] = 2;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2gb : MaxMipLevel[0] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 3;
        MaxMipLevel[1] = 3;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2gb : MaxMipLevel[0] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 4;
        MaxMipLevel[1] = 4;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2gb : MaxMipLevel[0] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
	MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 5;
        MaxMipLevel[1] = 5;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
        
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2gb : Level = 0, MipMapLODBias[0] = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
	CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2gb : Level = 0, MipMapLODBias[0] = 1";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
	MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 1;
        MipMapLODBias[1] = 1;
		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2gb : Level = 0, MipMapLODBias[0] = 2";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
	MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 2;
        MipMapLODBias[1] = 2;
		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
        
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2gb : Level = 0, MipMapLODBias[0] = 3";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 3;
        MipMapLODBias[1] = 3;
        CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
        
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2gb : Level = 0, MipMapLODBias[0] = 4";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 4;
        MipMapLODBias[1] = 4;
		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2gb : Level = 0, MipMapLODBias[0] = 5";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 5;
        MipMapLODBias[1] = 5;
        CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2gb       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}
*/
//*****************************************************************************
// texreg2ar

Technique 
< 
	String	Name = "ps_1_2 texreg2ar : Level = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex t0
                
                texreg2ar       t1, t0
                mov r0,t1
        };
         
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "ps_1_2 texreg2ar : Level = 1";
	String	Shape = "TexTL1";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex t0
                texreg2ar       t1, t0
                mov r0,t1
        };
         
		VertexShader = <TexTLVS>;
   }
}

/*
Technique 
< 
	String	Name = "ps_1_2 texreg2ar : Level = 2";
	String	Shape = "TexTL2";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
		ps_1_2

	        tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_1_2 texreg2ar : Level = 3";
	String	Shape = "TexTL3";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_1_2 texreg2ar : Level = 4";
	String	Shape = "TexTL4";
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
	MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
 	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
         
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2ar : MaxMipLevel[0] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
	MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2ar : MaxMipLevel[0] = 1";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
	MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 1;
        MaxMipLevel[1] = 1;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2ar : MaxMipLevel[0] = 2";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 2;
        MaxMipLevel[1] = 2;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2ar : MaxMipLevel[0] = 3";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 3;
        MaxMipLevel[1] = 3;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2ar : MaxMipLevel[0] = 4";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 4;
        MaxMipLevel[1] = 4;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
        CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2ar : MaxMipLevel[0] = 5";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
	Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
	MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 5;
        MaxMipLevel[1] = 5;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
	CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
        
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2ar : Level = 0, MipMapLODBias[0] = 0";
	String	Shape = "TexTL0";
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 0;
        MipMapLODBias[1] = 0;
		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2ar : Level = 0, MipMapLODBias[0] = 1";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
		MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 1;
        MipMapLODBias[1] = 1;
		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2ar : Level = 0, MipMapLODBias[0] = 2";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
		Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
		MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 2;
        MipMapLODBias[1] = 2;
		CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
        
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2ar : Level = 0, MipMapLODBias[0] = 3";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 3;
        MipMapLODBias[1] = 3;
        CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
        
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2ar : Level = 0, MipMapLODBias[0] = 4";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 4;
        MipMapLODBias[1] = 4;
	CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_2 texreg2ar : Level = 0, MipMapLODBias[0] = 5";
	String	Shape = "TexTL0";
	bool    UseLODBias = true;
>
{
    Pass P0
    {        
        Texture[0] = <Tex2D_0>;
        Texture[1] = <Tex2D_1>;
        MipFilter[0] = POINT;
        MipFilter[1] = POINT;
        MaxMipLevel[0] = 0;
        MaxMipLevel[1] = 0;
        MipMapLODBias[0] = 5;
        MipMapLODBias[1] = 5;
        CullMode = None; 
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
                ps_1_2
                
                tex  t0
                texreg2ar       t1, t0
                mov r0,t1
        };
         
	VertexShader = <TexTLVS>;
   }
}
*/

#include "include.fx"

Technique 
< 
	String	Name = "ps_1_1 texcoord : Level = 0";
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
                
                texcoord t0
                mov r0,t0
        };
         
		VertexShader = <TexTLVS>;
   }
}


Technique 
< 
	String	Name = "ps_1_1 texcoord : Level = 1";
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
                
                texcoord t0
                mov r0,t0
        };
         
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 texcoord : Level = 2";
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

	        texcoord t0
                mov r0,t0
        };
         
		VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 texcoord : Level = 3";
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
                
                texcoord t0
                mov r0,t0
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 texcoord : Level = 4";
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
                
                texcoord t0
                mov r0,t0
        };
         
		VertexShader = <TexTLVS>;
   }
}

//***************************************************************************************

Technique 
< 
	String	Name = "ps_1_1 texcrd : Level = 0";
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
                ps_1_4
                
                def 	c0,	0.0,	0.0,	0.0,	1.0
                
                texcrd r1.rgb, t0.xyz
                
                mov     r1.a, c0.a
                mov     r0, r1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 texcrd : Level = 1";
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
                ps_1_4
                
                def 	c0,	0.0,	0.0,	0.0,	1.0
                
                texcrd r1.rgb, t0.xyz
                
                mov     r1.a, c0.a
                mov     r0, r1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 texcrd : Level = 2";
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
                ps_1_4
                
                def 	c0,	0.0,	0.0,	0.0,	1.0
                
                texcrd r1.rgb, t0.xyz
                
                mov     r1.a, c0.a
                mov     r0, r1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 texcrd : Level = 3";
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
                ps_1_4
                
                def 	c0,	0.0,	0.0,	0.0,	1.0
                
                texcrd r1.rgb, t0.xyz
                
                mov     r1.a, c0.a
                mov     r0, r1
        };
         
	VertexShader = <TexTLVS>;
   }
}

Technique 
< 
	String	Name = "ps_1_1 texcrd : Level = 4";
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
                ps_1_4
                
                def 	c0,	0.0,	0.0,	0.0,	1.0
                
                texcrd r1.rgb, t0.xyz
                
                mov     r1.a, c0.a
                mov     r0, r1
        };
         
	VertexShader = <TexTLVS>;
   }
}

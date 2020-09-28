eTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// 1-t2
    AddShaderDef("1-t2",
                 "tex t2\n"
                 "mov r0, 1-t2\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// 1-t3
    AddShaderDef("1-t3",
                 "tex t3\n"
                 "mov r0, 1-t3\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// ****************************** Color Invert ******************************
	// 1-v0
    AddShaderDef("1-v0",
                 "mov r0, 1-v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// 1-v1
    AddShaderDef("1-v1",
                 "mov r0, 1-v1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// ****************************** Texture Alpha Replicate ******************************
	// t0.a
    AddShaderDef("Alpha Replicate t0",
                 "tex t0\n"
                 "mul r0, v0, t0.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// t1.a
	AddShaderDef("Alpha Replicate t1",
                 "tex t1\n"
                 "mul r0, v0, t1.a\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// t2.a
	AddShaderDef("Alpha Replicate t2",
                 "tex t2\n"
                 "mul r0, v0, t2.a\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, true);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// t3.a
	AddShaderDef("Alpha Replicate t3",
                 "tex t3\n"
                 "mul r0, v0, t3.a\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, true);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// ****************************** Color Alpha Replicate ******************************
	
	// v0.a
    AddShaderDef("v0.a",
				 "tex t0\n"
                 "mul r0, v0.a, t0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	// v1.a
    AddShaderDef("v1.a",
				 "tex t0\n"
                 "mul r0, v1.a, t0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	
	// ****************************** Constant Alpha Replicate ******************************
	// c0.a
    AddShaderDef("c0.a",
                 "mul r0, v0, c0.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);

	// c1.a
    AddShaderDef("c1.a",
                 "mul r0, v0, c1.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);

	// c2.a
    AddShaderDef("c2.a",
                 "mul r0, v0, c2.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);

	// c3.a
    AddShaderDef("c3.a",
                 "mul r0, v0, c3.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);
	
	// c4.a
    AddShaderDef("c4.a",
                 "mul r0, v0, c4.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);

	// c5.a
    AddShaderDef("c5.a",
                 "mul r0, v0, c5.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);

	// c6.a
    AddShaderDef("c6.a",
                 "mul r0, v0, c6.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);

	// c7.a
    AddShaderDef("c7.a",
                 "mul r0, v0, c7.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddConstantsToLastShaderDef(fConstants, 32);

	// ****************************** Bias Texture ******************************
	// t0_bias
	AddShaderDef("t0_bias",
                 "tex t0\n"
                 "add r0, v0, t0_bias\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddConstantsToLastShaderDef(fConstants, 24);

	// t1_bias
	AddShaderDef("t1_bias",
                 "tex t1\n"
                 "add r0, v0, t1_bias\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
	AddConstantsToLastShaderDef(fConstants, 24);

	// t2_bias
	AddShaderDef("t2_bias",
                 "tex t2\n"
                 "add r0, v0, t2_bias\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	AddConstantsToLastShaderDef(fConstants, 24);

	// t3_bias
	AddShaderDef("t3_bias",
                 "tex t3\n"
                 "add r0, v0, t3_bias\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	AddConstantsToLastShaderDef(fConstants, 24);

	// ****************************** Bias Color ******************************
	// v0_bias
    AddShaderDef("v0_bias",
                 "mov r0, v0_bias\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);

	// v1_bias
    AddShaderDef("v1_bias",
                 "mov r0, v1_bias\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);

	// ****************************** Sign Scaling Texture ******************************
	// t0_bx2 
	AddShaderDef("t0_bx2",
                 "tex t0\n"
                 "mov r0, t0_bx2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// t1_bx2 
	AddShaderDef("t1_bx2",
                 "tex t1\n"
                 "mov r0, t1_bx2\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// t2_bx2 
	AddShaderDef("t2_bx2",
                 "tex t2\n"
                 "mov r0, t2_bx2\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// t3_bx2 
	AddShaderDef("t3_bx2",
                 "tex t3\n"
                 "mov r0, t3_bx2\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// ****************************** Sign Scaling Color ******************************
	// v0_bx2 
	AddShaderDef("v0_bx2",
                 "mov r0, v0_bx2\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices);

	// v1_bx2 
	AddShaderDef("v1_bx2",
                 "mov r0, v1_bx2\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices);


	// ****************************** Testing known hardware issues. ******************************

	//1) Nvidia Gef3 can't do bias on negative values.
    AddShaderDef("negative value bias",
				 "def c0, -0.5f, -0.5f, -0.5f, -0.5f\n"
                 "mov_sat r1, c0\n"
                 "mov r0, r1_bias\n"
				 "mov r0, -r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices);

// *************************** Version 1.1 ***************************
	// Can use all the 1.0 shaders as 1.1
	// ****************************** Texture Blue to Alpha ******************************
	// t0.b
    AddShaderDef("Blue to Alpha t0",
                 "tex t0\n"
				 "tex t1\n"
				 "mov r0, t1\n"
                 "mov r0.a, t0.b\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, true, SHADE_PLAIN, PSVERSION(1, 1));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
	
	// t3.b
	AddShaderDef("Blue to Alpha t3",
                 "tex t0\n"
				 "tex t3\n"
				 "mov r0.rgb, t0\n"
                 "+ mov r0.a, t3.b\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, true, SHADE_PLAIN, PSVERSION(1, 1));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex,3);

	// ****************************** Color Blue to Alpha ******************************
	// v0.b
    AddShaderDef("Blue to Alpha v0",
				 "tex t0\n"
				 "mov r0.rgb, t0\n"
                 "+ mov r0.a, v0.b\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, true, SHADE_PLAIN, PSVERSION(1, 1));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// v1.b
    AddShaderDef("Blue to Alpha v1",
				 "tex t0\n"
				 "mov r0.rgb, t0\n"
                 "+ mov r0.a, v1.b\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, true, SHADE_PLAIN, PSVERSION(1, 1));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	
	// ****************************** Constant Blue to Alpha ******************************
	// c2.b
    AddShaderDef("Blue to Alpha c2",		
				 "mov r0.rgb, v0\n"
                 "+mov r0.a, c2.b\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 1));
    AddConstantsToLastShaderDef(fConstants, 32);

	// c7.b
    AddShaderDef("Blue to Alpha c7",
                 "mov r0.rgb, v0\n"
                 "+mov r0.a, c7.b\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 1));
    AddConstantsToLastShaderDef(fConstants, 32);

	// ****************************** Texcoord Blue to Alpha ******************************
	// t0.b
	AddShaderDef("Blue to Alpha texcoord t0",
				 "texcoord t0 \n"
                 "mov r0.a, t0.b \n"
				 "mov r0.rgb, r0.a \n",
                 MYVOLTLVERTEX_FVF, g_VolTexVertices, PSVERSION(1, 1));

	// ****************************** Register Blue to Alpha ******************************
	// r1.b
	AddShaderDef("Blue to Alpha r1",
				 "texcoord t0 \n"
				 "mov r1, t0 \n"
                 "mov r0.a, r1.b \n"
				 "mov r0.rgb, r0.a \n",
                 MYVOLTLVERTEX_FVF, g_VolTexVertices, PSVERSION(1, 1));

// *************************** Version 1.2 ***************************
	// Can use all the 1.0 and 1.1 shaders as 1.2

// *************************** Version 1.3 ***************************
	// Can use all the 1.0, 1.1 and 1.2 shaders as 1.3


// *************************** Version 1.4 ***************************
	// ****************************** Texture Negate ******************************
	// -r0
    AddShaderDef(" -r0",
                 "texld r0, t0\n"
				 "texld r1, t1\n"
                 "add r0, r1, -r0\n",
                 MYTLVERTEXTEX2_FVF, g_FourColorVerticesTex2, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);

	// -r5
    AddShaderDef("-r5",
                 "texld r5, t5\n"
                 "add r0, v0, -r5\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
	AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// ****************************** Color Negate ******************************
	// -v0
    AddShaderDef("-v0",
                 "texld r0, t0\n"
				 "add r0, r0, -v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// -v1
    AddShaderDef("-v1",
				 "texld r0, t0\n" 
				 "add r0, r0, -v1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// ****************************** Texture Invert ******************************
    // 1-r0
    AddShaderDef("1-r0",
                 "texld r0, t0\n"
                 "mov r0, 1-r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);

	// 1-r5
    AddShaderDef("1-r5",
                 "texld r5, t5\n"
                 "mov r0, 1-r5\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
	AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);    
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// ****************************** Color Invert ******************************
	// 1-v0
    AddShaderDef("1-v0",
                 "mov r0, 1-v0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// 1-v1
    AddShaderDef("1-v1",
                 "mov r0, 1-v1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);

	// ****************************** Texture Alpha Replicate ******************************
	// t0.a
    AddShaderDef("Alpha Replicate t0",
                 "texld r0, t0\n"
                 "mul r0, v0, r0.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);


	// t5.a
	AddShaderDef("Alpha Replicate t5",
                 "texld r5, t5\n"
                 "mul r0, v0, r5.a\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// ****************************** Color Alpha Replicate ******************************
	
	// v0.a
    AddShaderDef("v0.a",
				 "texld r0, t0\n"
                 "mul r0, v0.a, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	// v1.a
    AddShaderDef("v1.a",
				 "texld r0, t0\n"
                 "mul r0, v1.a, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	
	// ****************************** Constant Alpha Replicate ******************************
	// c0.a
    AddShaderDef("c0.a",
                 "mul r0, v0, c0.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);

	// c7.a
    AddShaderDef("c7.a",
                 "mul r0, v0, c7.a\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);

	// ****************************** Bias Texture ******************************
	// r0_bias
	AddShaderDef("r0_bias",
                 "texld r0, t0\n"
                 "add r0, v0, r0_bias\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pCheckerTex);
	AddConstantsToLastShaderDef(fConstants, 24);

	// r5_bias
	AddShaderDef("r5_bias",
                 "texld r5, t5\n"
                 "add r0, v0, r5_bias\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
	AddConstantsToLastShaderDef(fConstants, 24);

	// ****************************** Bias Color ******************************
	// v0_bias
    AddShaderDef("v0_bias",
                 "mov r0, v0_bias\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));

	// v1_bias
    AddShaderDef("v1_bias",
                 "mov r0, v1_bias\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));

	// ****************************** Sign Scaling Texture ******************************
	// r0_bx2 
	AddShaderDef("r0_bx2",
                 "texld r0, t0\n"
                 "mov r0, r0_bx2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pColorCheckerTex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// r5_bx2 
	AddShaderDef("r5_bx2",
                 "texld r5, t5\n"
                 "mov r0, r5_bx2\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// ****************************** Sign Scaling Color ******************************
	// v0_bx2 
	AddShaderDef("v0_bx2",
                 "mov r0, v0_bx2\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));

	// v1_bx2 
	AddShaderDef("v1_bx2",
                 "mov r0, v1_bx2\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));

	// ****************************** Texture x2 ******************************
	// r0_x2 
	AddShaderDef("r0_x2",
                 "texld r0, t0\n"
                 "mov r0, r0_x2\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// r5_x2 
	AddShaderDef("r5_x2",
                 "texld r5, t5\n"
                 "mov r0, r5_x2\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDP3Tex);
    AddConstantsToLastShaderDef(fConstants, 24);

	// ****************************** Color x2 ******************************
	// v0_x2 
	AddShaderDef("v0_x2",
                 "mov r0, v0_x2\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));

	// v1_x2 
	AddShaderDef("v1_x2",
                 "mov r0, v1_x2\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));

	// ****************************** Texture Red Replicate ******************************
	// r0.r
    AddShaderDef("Red Replicate r0",
                 "texld r0, t0\n"
                 "mul r0, v0, r0.r\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);


	// r5.r
	AddShaderDef("Red Replicate r5",
                 "texld r5, t5\n"
                 "mul r0, v0, r5.r\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// ****************************** Color Red Replicate ******************************	
	// v0.r
    AddShaderDef("v0.r",
				 "texld r0, t0\n"
                 "mul r0, v0.r, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	// v1.r
    AddShaderDef("v1.r",
				 "texld r0, t0\n"
                 "mul r0, v1.r, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	
	// ****************************** Constant Red Replicate ******************************
	// c0.r
    AddShaderDef("c0.r",
                 "mul r0, v0, c0.r\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);

	// c7.r
    AddShaderDef("c7.r",
                 "mul r0, v0, c7.r\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);

	// ****************************** Texture Green Replicate ******************************
	// r0.g
    AddShaderDef("Green Replicate r0",
                 "texld r0, t0\n"
                 "mul r0, v0, r0.g\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);


	// r5.g
	AddShaderDef("Green Replicate r5",
                 "texld r5, t5\n"
                 "mul r0, v0, r5.g\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// ****************************** Color Green Replicate ******************************	
	// v0.g
    AddShaderDef("v0.g",
				 "texld r0, t0\n"
                 "mul r0, v0.g, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	// v1.g
    AddShaderDef("v1.g",
				 "texld r0, t0\n"
                 "mul r0, v1.g, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	
	// ****************************** Constant Green Replicate ******************************
	// c0.g
    AddShaderDef("c0.g",
                 "mul r0, v0, c0.g\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);

	// c7.g
    AddShaderDef("c7.g",
                 "mul r0, v0, c7.g\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);


	// ****************************** Texture Blue Replicate ******************************
	// r0.b
    AddShaderDef("Blue Replicate r0",
                 "texld r0, t0\n"
                 "mul r0, v0, r0.b\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, true, SHADE_PLAIN, PSVERSION(1, 4));
	AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// r5.b
	AddShaderDef("Blue Replicate r5",
                 "texld r5, t5\n"
                 "mul r0, v0, r5.b\n",
                 MYTLVERTEXTEX6_FVF, g_FourColorVerticesTex6, true, SHADE_PLAIN, PSVERSION(1, 4));
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
	AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef(NULL);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);

	// ****************************** Color Blue Replicate ******************************	
	// v0.b
    AddShaderDef("v0.b",
				 "texld r0, t0\n"
                 "mul r0, v0.b, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	// v1.b
    AddShaderDef("v1.b",
				 "texld r0, t0\n"
                 "mul r0, v1.b, r0\n",
                 MYTLVERTEX_FVF, g_FourColorAlphaVertices, PSVERSION(1, 4));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);

	
	// ****************************** Constant Blue Replicate ******************************
	// c0.b
    AddShaderDef("c0.b",
                 "mul r0, v0, c0.b\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);

	// c7.b
    AddShaderDef("c7.b",
                 "mul r0, v0, c7.b\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(1, 4));
    AddConstantsToLastShaderDef(fConstants2, 32);

	// ****************************** Texcoord Blue to Alpha ******************************
	// t0.b
	AddShaderDef("Blue to Alpha texcrd t0",
				 "texcrd r1.rgb, t0.xyz \n"
                 "mov r0.a, r1.b \n"
				 "mov r0.rgb, r0.a \n",
                MYVOLTLVERTEX_FVF, g_VolTexVertices, PSVERSION(1, 4));

	// t5.b
	AddShaderDef("Blue to Alpha texcrd t5",
				 "texcrd r1.rgb, t5.xyz \n"
                 "mov r0.a, r1.b \n"
				 "mov r0.rgb, r0.a \n",
                MYTLVERTEX3_FVF, g_Texm3Vertices, PSVERSION(1, 4));

// *************************** Version 2.0 ***************************
// *******************************************************************

	//negate
    AddShaderDef("negate",
                 "def c0, 0.0, 0.0, 0.0, 1.0\n"
                 "def c1, 1.0, 1.0, 1.0, 1.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "texld r0.rgba,t0,s0\n"
				 "add r1, -r0, -c0\n"
				 "mul r1, r1, -c1\n"
				 "mov oC0, r1\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);


	//replicate  .r .rrrr etc
    AddShaderDef("replicate .r .rrrr etc",
                 "def c0, 0.33, 0.33, 0.33, 1.0\n"
				 "dcl t0.xy\n"
				 "dcl_2d s0\n"
				 "texld r0.rgba,t0,s0\n"
				 "mov r1, r0.r\n"
				 "mov r2, r0.gggg\n"
				 "mov r3, r0.b\n"
				 "mul r1, r1, c0\n"
				 "mul r2, r2, c0\n"
				 "mul r3, r3, c0\n"
				 "mov r0, r1\n"
				 "add r0, r0, r2\n"
				 "add r0, r0, r3\n"
				 "mov r0.a, c0.aaaa\n"
				 "mov oC0, r0\n",
                 MYTLVERTEX_FVF, g_FourColorVertices, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);


	//swizzle .rgba .gbra .brga .abgr 
    AddShaderDef("swizzle .rgba .gbra .brga .abgr",
                 "def c0, 0.25, 0.25, 0.25, 0.25\n"
				 "dcl t0.xy\n"
				 "dcl v0.rgba\n"
				 "dcl_2d s0\n"
				 "dcl_2d s1\n"
				 "dcl_2d s2\n"
				 "texld r0,t0,s0\n"
				 "texld r1,t0,s1\n"
				 "texld r2,t0,s2\n"

				 "mov r3, r0.rgba\n"
				 "mov r4, r1.gbra\n"
				 "mov r5, r2.brga\n"
				 "mov r6, v0.abgr\n"
				 "mul r3, r3, c0\n"
				 "mul r4, r4, c0\n"
				 "mul r5, r5, c0\n"
				 "mul r6, r6, c0\n"

				 "mov r0, r3\n"
				 "add r0, r0, r4\n"
				 "add r0, r0, r5\n"
				 "add r0, r0, r6\n"
				 "mov oC0, r0\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);


	//swizzle .rgba .gbra .brga .abgr with write mask
    AddShaderDef("swizzle .rgba .gbra .brga .abgr with write mask",
                 "def c0, 0.25, 0.25, 0.25, 0.25\n"
                 "def c1, 0.0, 0.0, 0.0, 0.0\n"
				 "dcl t0.xy\n"
				 "dcl v0.rgba\n"
				 "dcl_2d s0\n"
				 "dcl_2d s1\n"
				 "dcl_2d s2\n"
				 "texld r0,t0,s0\n"
				 "texld r1,t0,s1\n"
				 "texld r2,t0,s2\n"

				 //initialize r3 r4 r5 r6
				 "mov r3, c1\n"
				 "mov r4, c1\n"
				 "mov r5, c1\n"
				 "mov r6, c1\n"

				 "mov r3.rg, r0.rgba\n"
				 "mov r4.ba, r1.gbra\n"
				 "mov r5.ra, r2.brga\n"
				 "mov r6.a, v0.abgr\n"
				 "mul r3, r3, c0\n"
				 "mul r4, r4, c0\n"
				 "mul r5, r5, c0\n"
				 "mul r6, r6, c0\n"

				 "mov r0, r3\n"
				 "add r0, r0, r4\n"
				 "add r0, r0, r5\n"
				 "add r0, r0, r6\n"
				 "mov oC0, r0\n",
                 MYTLVERTEXTEX4_FVF, g_FourColorVerticesTex4, PSVERSION(2,0));
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pDXLogoTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pGradientTex);
    AddTextureToLastShaderDef((CnBaseTexture**)&m_pBrickTex);

	// ****************************** Texcoord Blue to Alpha ******************************
	// t0.b
	AddShaderDef("Blue to Alpha dcl t0",
				 "dcl t0.xyz \n"
                 "mov r0.a, t0.b \n"
				 "mov r0.rgb, r0.a \n"
				 "mov oC0, r0 \n",
                 MYVOLTLVERTEX_FVF, g_VolTexVertices, PSVERSION(2, 0));

	// t5.b
	AddShaderDef("Blue to Alpha dcl t5",
				 "dcl t5.xyz \n"
                 "mov r0.a, t5.b \n"
				 "mov r0.rgb, r0.a \n"
				 "mov oC0, r0 \n",
                 MYTLVERTEX3_FVF, g_Texm3Vertices, PSVERSION(2, 0));


    return S_OK;
}

bool CPSSrcMod::GenerateTextureData()
{
	// DP3
    if ( !m_DP3Image.Load("DP3.bmp", 1.0f) )
    {
        WriteToLog(_T("(ERROR) Failed to load DP3.bmp.  Aborting.\n"));
        return false;
    }

	// Gradient
	DWORD dwColors[4] =
    {
        D3DRGBA(0, 1, 0, 1),
        D3DRGBA(0, 1, 0, 1),
        D3DRGBA(1, 1, 1, 0.5),
        D3DRGBA(1, 1, 1, 0.5)
    };
    if (NULL == (m_GradientImage.LoadGradient(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwColors)))
    {
        WriteToLog(_T("(ERROR) Failed to create gradient data.  Aborting.\n"));
        return false;
    }
	return CPixelShader::GenerateTextureData();
}

bool CPSSrcMod::LoadTextures()
{       
	if (!SetTextureFormat(TEXFORMAT))
		return false;	

    DWORD dwLevels = (DWORD) 1;
    DWORD dwPool = POOL_DEFAULT;
    DWORD dwUsage = (DWORD) 0;
    FORMAT *pFormat = &m_pTexFormats[m_pCurrentMode->nTextureFormat];

	// DP3
    if (!CreateTexture(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pDP3Tex))
    {
        WriteToLog(_T("(ERROR) Failed to create DP3 texture.  Aborting.\n"));
        return false;
    }
    if (!m_pDP3Tex->LoadImage(dwLevels, &m_DP3Image))
    {
        WriteToLog(_T("Unable to load DP3 texture.\n"));
        return false;
    }
    
	//Gradient
    if (!CreateTexture(DEFAULT_TEXSIZE, DEFAULT_TEXSIZE, dwLevels, dwUsage, *pFormat, dwPool, &m_pGradientTex))
    {
        WriteToLog(_T("(ERROR) Failed to create gradient texture.  Aborting.\n"));
        return false;
    }
    if (!m_pGradientTex->LoadImage(dwLevels, &m_GradientImage))
    {
        WriteToLog(_T("Unable to load gradient texture.\n"));
        return false;
    }

	return CPixelShader::LoadTextures();
}
/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    devapi.c

Abstract:

    This module contains functions associated with PCI devices











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "devapi.tmh"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciRestoreDeviceRegisters(
    __inout PPCI_DEVICE Device,
    __inout PPCI_SAVE_RESTORE_DATA SavedRegisters
    );

VOID
PciResetBridge(
    __in PPCI_DEVICE Device
    );

BOOLEAN
PciOnVGAPath(
    __in PPCI_DEVICE Device
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciSaveSlotInformation)
    #pragma alloc_text(PAGE, PciGetSavedSlotInformation)
    #pragma alloc_text(PAGE, PciUpdateSavedDeviceConfiguration)
    #pragma alloc_text(PAGE, PciApplyHPP)
    #pragma alloc_text(PAGE, PciOnPostPath)
    #pragma alloc_text(PAGE, PciOnVGAPath)
    #pragma alloc_text(PAGE, PciSlotPresentInParentMethod)
    #pragma alloc_text(PAGE, PciDeviceOnDebugPath)
#endif

// --------------------------------------------------------------------- Globals

extern PPCI_DEBUG_PORT PciDebugPorts;
extern ULONG           PciDebugPortsCount;

volatile LONG PciDebugSaveDone;
volatile LONG PciDebugSaveNotDone;

// ------------------------------------------------------------------- Functions

_IRQL_requires_max_(HIGH_LEVEL)
NTSTATUS
PciUpdateHardware(
    __inout PPCI_DEVICE Device,
    __in_opt PVOID ReconnectDevice
    )
/*++

Routine Description:

    This routine is used to update the device hardware with the settings
    stored in the device extension.  These settings include the device
    resources and any saved registers that need to be restored.  To do this,
    the device must be disconnected from the bus.  Because disconnecting
    the device from the bus may cause the machine to crash (if the device is
    a "critical" device like an I/O APIC) this routine may be called in the
    context of KeIpiGenericCall

    In addition, the given ConfigHeader should be updated with the changes
    made to the hardware.

Arguments:

    Device - The PCI_DEVICE in question.

    ReconnectDevice - Supplies a flag used to connect or reconnect the device.


Return Value:

    VOID

--*/
{

    NTSTATUS status;

    NON_PAGED_CODE();

    PciDisconnectDevice(Device);

    status = Device->Operations->SetResources(Device, &Device->Resources);
    if (!NT_SUCCESS(status)) {
        TraceEvents(Device->Parent->BlackBoxHandle,
                    TRACE_LEVEL_ERROR,
                    DBG_DEVPOWER,
                    "(%x:%x) failed to set device resources %!STATUS!",
                    Device->Slot.u.bits.DeviceNumber,
                    Device->Slot.u.bits.FunctionNumber,
                    status);
        goto exit;
    }

    //
    // This must come after the SetResources, since for a cardbus bridge, the set resources
    // alters the bridge control register's settings for memory prefetchability.
    //
    status = PciRestoreDeviceRegisters(Device, &Device->SavedRegisters);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (((Device->HeaderType == PCI_BRIDGE_TYPE) ||
         (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) &&
        (Device->HackFlags & PCI_HACK_RESET_BRIDGE_ON_POWERUP)) {

        PciResetBridge(Device);
    }

exit:

    //
    // During a link quiescent transaction we perform a Reconnect vs a Connect
    // in order to properly preserve the prior state of the device decodes.
    // This is particularly important for VGA devices which have decodes initially
    // disabled by the BIOS because we honor this config and do not restore them
    // during a standard Connect.
    //

    if (((BOOLEAN)(ULONG_PTR)ReconnectDevice) != FALSE) {
        PciReconnectDevice(Device);

    } else {
        PciConnectDevice(Device);
    }

    //
    // After we've connected the device, restore TPH. This has to be done after
    // connect because it may touch MMIO space on the device, which doesn't work
    // until after the connect.
    //

    if ((Device->ExpressPort != NULL) &&
        (Device->ExpressPort->TphRequesterCapability != 0)) {
        ExpressRestoreTphRequesterCapability(Device->ExpressPort);
    }


    return status;
}

NTSTATUS
PciProbeForRequirements(
    __inout PPCI_DEVICE Device,
    __in_opt PVOID UnusedParameter
    )
/*++

Routine Description:

    This routine probes the given device to determine its resource
    requirements.  To do this, the device must be disconnected from
    the bus.  Because disconnecting the device from the bus may cause
    the machine to crash (if the device is a "critical" device like
    an I/O APIC) this routine may be called in the context of
    KeIpiGenericCall

Parameters:

    Device - The device to probe for resource requirements.

    UnusedParameter - Exists so that the function matches the calling
        convention imposed by PciExecuteCriticalSystemRoutine.  Unused.

Return Value:

    NT Status code.

--*/
{
    NTSTATUS status;

    NON_PAGED_CODE();

    UNREFERENCED_PARAMETER(UnusedParameter);

    PciDisconnectDevice(Device);

    status = Device->Operations->GetRequirements(Device,
                                                 &Device->Requirements
                                                 );
    PciReconnectDevice(Device);

    DebugSetDebugFlag(Device, PCI_GOT_REQUIREMENTS);

    return status;
}

NTSTATUS
PciRestoreDeviceRegisters(
    __inout PPCI_DEVICE Device,
    __inout PPCI_SAVE_RESTORE_DATA SavedRegisters
    )
/*++

Routine Description:

    This routine updates a device's configuration registers with
    the versions saved in the device extension.  It does this
    for all registers that the driver modifies except for resource-related
    registers, like BARs.  In addition, it updates the given config
    header with the changes, so that the up-to-date config header
    can be stored in the registry.

Parameters:

    Device - The device to update.

    SavedRegisters - The data to use to update the device.

Return Value:

    NT Status code.

--*/
{
    USHORT bridgeControl, bits;
    USHORT readCacheLineSize;
    USHORT command;

    NON_PAGED_CODE();

    PCI_ASSERT(!Device->Connected);

    PciWriteConfigRegister(Device,
                           common.InterruptLine,
                           &SavedRegisters->InterruptLine
                           );

    if ((Device->HeaderType == PCI_BRIDGE_TYPE) ||
        (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) {

        //
        // On a cardbus controller, defer control of the a few cardbus
        // specific bits to the pcmcia driver.
        //
        if (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE) {

            bits = PCI_ASSERT_BRIDGE_RESET |
                   PCI_ENABLE_CARDBUS_IRQ_ROUTING |
                   PCI_ENABLE_CARDBUS_WRITE_POSTING;

            PciReadConfigRegister(Device,
                                  bridgeCommon.BridgeControl,
                                  &bridgeControl
                                  );

            bridgeControl &= bits;
            SavedRegisters->bridge.BridgeControl &= ~bits;
            SavedRegisters->bridge.BridgeControl |= bridgeControl;
        }

        //
        // Strip the ISA bit from the value written to hardware if this bridge
        // does not function correctly with the ISA bit set.  For these
        // bridges, there are cases where the ISA bit is "logically" set for
        // the purposes of resource arbitration even though it is never
        // reflected back to hardware.
        //
        bridgeControl = SavedRegisters->bridge.BridgeControl;
        if (Device->HackFlags & PCI_HACK_AVOID_HARDWARE_ISA_BIT) {
            bridgeControl &= ~PCI_ENABLE_BRIDGE_ISA;
        }

        PciWriteConfigRegister(Device,
                               bridgeCommon.BridgeControl,
                               &bridgeControl
                               );

        //
        // Validate that the ISA bit stuck with this write unless this bridge
        // either doesn't implement the ISA bit or malfunctions when it is
        // set.  In general resource arbitration depends on the bit being set.
        //
        PciReadConfigRegister(Device,
                              bridgeCommon.BridgeControl,
                              &bridgeControl);

        if ((SavedRegisters->bridge.BridgeControl & PCI_ENABLE_BRIDGE_ISA) &&
            ((bridgeControl & PCI_ENABLE_BRIDGE_ISA) == 0)) {

            if (((Device->HackFlags & PCI_HACK_IGNORE_NON_STICKY_ISA) == 0) &&
                ((Device->HackFlags & PCI_HACK_AVOID_HARDWARE_ISA_BIT) == 0)) {

                TraceEvents(Device->Parent->BlackBoxHandle,
                            TRACE_LEVEL_ERROR,
                            DBG_DEVPOWER,
                            "(%x:%x) ISA bit didn't stick",
                            Device->Slot.u.bits.DeviceNumber,
                            Device->Slot.u.bits.FunctionNumber);

                return STATUS_DEVICE_DATA_ERROR;
            }
        }

        PciWriteConfigRegister(Device, bridgeCommon.PrimaryBus, &Device->BusNumber);
        PciWriteConfigRegister(Device,
                               bridgeCommon.SecondaryBus,
                               &SavedRegisters->bridge.SecondaryBusNumber
                               );
        PciWriteConfigRegister(Device,
                               bridgeCommon.SubordinateBus,
                               &SavedRegisters->bridge.SubordinateBusNumber
                               );

    }

    PciWriteConfigRegister(Device, CacheLineSize, &SavedRegisters->CacheLineSize);

    if (Device->ConfigureMwi) {

        //
        // Check if the cache line size stuck which means the hardware liked it
        //
        PciReadConfigRegister(Device, CacheLineSize, &readCacheLineSize);

        if ((readCacheLineSize == SavedRegisters->CacheLineSize) &&
            (readCacheLineSize != 0)) {

            SavedRegisters->Command |= PCI_ENABLE_WRITE_AND_INVALIDATE;

        } else {
            TraceEvents(Device->Parent->BlackBoxHandle,
                        TRACE_LEVEL_VERBOSE,
                        DBG_DEVPOWER,
                        "(%x:%x) cache line size non-sticky, not enabling MWI",
                        Device->Slot.u.bits.DeviceNumber,
                        Device->Slot.u.bits.FunctionNumber);
        }

        PCI_DEVICE_CLEAR_FLAG(Device, ConfigureMwi);
    }

    PciWriteConfigRegister(Device, LatencyTimer, &SavedRegisters->LatencyTimer);

    //
    // If the device has a HyperTransport MSI Mapping capability, enable
    // that now.
    //
    if (Device->HtMsiMapCapability != 0) {
        PciProgramHtMsiMapCapability(Device);
    }

    //
    // Restore the command register, but *not* the decodes.  They will
    // be restored separately.
    //
    command = SavedRegisters->Command & ~PCI_DECODE_ENABLES;
    command |= PCI_DISABLE_LEVEL_INTERRUPT;
    PciWriteConfigRegister(Device, Command, &command);

    //
    // In addition, for Express devices restore the Express specific registers.
    //

    if (Device->ExpressPort != NULL) {
        ExpressRestorePortRegisters(Device->ExpressPort);

    } else if (Device->ExpressCompatibilityPort != NULL) {
        ExpressCompatibilityRestorePortRegisters(
            Device->ExpressCompatibilityPort);

    }

    return STATUS_SUCCESS;
}

BOOLEAN
PciDeviceConnected(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine is used to tell if a device is connected to the PCI bus and
    decoding PCI transactions.  This is determined by checking to see if
    the command register is clear or not.

Arguments:

    Device - The PCI_DEVICE in question.

Return Value:

    TRUE if the device is connected.
    FALSE otherwise.

--*/
{
    USHORT command;

    NON_PAGED_CODE();

    PciReadConfigRegister(Device, Command, &command);

    if (command & (PCI_ENABLE_MEMORY_SPACE |
                   PCI_ENABLE_IO_SPACE |
                   PCI_ENABLE_BUS_MASTER)) {

        return TRUE;
    } else {
        return FALSE;
    }
}

_IRQL_requires_max_(HIGH_LEVEL)
VOID
PciConnectDevice(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine is used to connect a device to the PCI bus.  This is
    done by setting the I/O, Memory and Bus Master enables in the
    device's command register.

    In addition, update the ConfigHeader with the changes.

Arguments:

    Device - The PCI_DEVICE in question.

Return Value:

    VOID

--*/
{
    USHORT status, command, decodes;

    NON_PAGED_CODE();

    PCI_ASSERT(!Device->Connected);

    if (Device->HackFlags & PCI_HACK_PRESERVE_COMMAND) {
        return;
    }

    TraceEvents(Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_VERBOSE,
                DBG_DEVPOWER,
                "(%x:%x) PciConnectDevice",
                Device->Slot.u.bits.DeviceNumber,
                Device->Slot.u.bits.FunctionNumber);

    PciReadConfigRegister(Device, Command, &command);

    //
    // We would like to turn on the I/O, memory and bus master enable bits.
    // Mask these against the decodes that have been explicitly disabled
    // on this device, through hacks and other things.
    // Leave interrupts disabled.  They'll be enabled in PciEnableInterrupts.
    //
    decodes = PCI_DECODE_ENABLES & Device->PossibleDecodes & Device->ResourceDecodes;

    command &= ~PCI_DECODE_ENABLES;
    command |= PCI_DISABLE_LEVEL_INTERRUPT;
    command |= decodes;

    PciWriteConfigRegister(Device, Command, &command);

    Device->PreviousCommand = 0;

    //
    // When we first connect the device we want to clear off the status bits
    // indicating errors that may have occured before we got here so we can
    // detect errors that occur from now on.
    //
    status = PCI_STATUS_DATA_PARITY_DETECTED
            | PCI_STATUS_SIGNALED_TARGET_ABORT
            | PCI_STATUS_RECEIVED_TARGET_ABORT
            | PCI_STATUS_RECEIVED_MASTER_ABORT
            | PCI_STATUS_SIGNALED_SYSTEM_ERROR
            | PCI_STATUS_DETECTED_PARITY_ERROR;

    PciWriteConfigRegister(Device, Status, &status);

    //
    // Special case for some devices that require a write to the command
    // register for second time after status register update.
    //
    // N. B. This is the only place where this workaround is required.
    //

    if (Device->HackFlags & PCI_HACK_COMMAND_REWRITE) {
        PciWriteConfigRegister(Device, Command, &command);
    }

    //
    // If this is a bridge then do the same to the Secondary Status.
    // This is conveniently laid out using the same bit definitions as
    // the normal status register.  Unfortunaely they are in different
    // places in the header.
    //
    if (Device->HeaderType == PCI_BRIDGE_TYPE) {
        PciWriteConfigRegister(Device, type1.SecondaryStatus, &status);
    } else if (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE) {
        PciWriteConfigRegister(Device, type2.SecondaryStatus, &status);
    }

    if (Device->ExpressPort != NULL) {

        //
        // If this is an SR-IOV device and the driver supports SR-IOV then
        // enable the VF BARs.
        //

        if (Device->ExpressPort->EnableSriovDecodes != FALSE) {
            PciEnableSriovDecodes(Device);
        }

        ExpressPortObffEnable(Device->ExpressPort);
    }

    PCI_DEVICE_SET_FLAG(Device, Connected);

    return;
}

_IRQL_requires_max_(HIGH_LEVEL)
VOID
PciDisconnectDevice(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine is used to disconnect a device from the PCI bus.  This is
    done by clearing the I/O, Memory and Bus Master enables in the
    device's command register.

Arguments:

    Device - The PCI_DEVICE in question.

Return Value:

    VOID

--*/
{
    USHORT command = 0;

    NON_PAGED_CODE();

    if (!Device->Connected) {
        return;
    }

    PCI_ASSERT(Device->PreviousCommand == 0);

    TraceEvents(Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_VERBOSE,
                DBG_DEVPOWER,
                "(%x:%x) PciDisconnectDevice",
                Device->Slot.u.bits.DeviceNumber,
                Device->Slot.u.bits.FunctionNumber);

    PciReadConfigRegister(Device, Command, &command);

    Device->PreviousCommand = command;

    command &= ~(PCI_ENABLE_IO_SPACE | PCI_ENABLE_MEMORY_SPACE);
    command |= PCI_DISABLE_LEVEL_INTERRUPT;

    //
    //  If our debugger is bus mastering then dont clear this bit as it
    //  will blow away the DMA engine on the card.
    //

    if (!(Device->OnDebugPath)) {
        command &= ~PCI_ENABLE_BUS_MASTER;
    }

    if (Device->ExpressPort != NULL) {

        //
        // If this is an SR-IOV device and the driver supports SR-IOV,
        // disable the VF BARs now.
        //

        if (Device->ExpressPort->EnableSriovDecodes != FALSE) {
            PciDisableSriovDecodes(Device);
        }

        ExpressPortObffDisable(Device->ExpressPort);
    }

    PciWriteConfigRegister(Device, Command, &command);

    PCI_DEVICE_CLEAR_FLAG(Device, Connected);

    return;
}

_IRQL_requires_max_(HIGH_LEVEL)
VOID
PciReconnectDevice(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine is used to reconnect a device from the PCI bus.  This is
    done by setting the I/O, Memory and Bus Master enables in the
    device's command register to the values they were before a previous
    call to PciDisconnectDevice.

Arguments:

    Device - The PCI_DEVICE in question.

Return Value:

    VOID

--*/
{
    NON_PAGED_CODE();

    TraceEvents(Device->Parent->BlackBoxHandle,
                TRACE_LEVEL_VERBOSE,
                DBG_DEVPOWER,
                "(%x:%x) PciReconnectDevice",
                Device->Slot.u.bits.DeviceNumber,
                Device->Slot.u.bits.FunctionNumber);

    PciWriteConfigRegister(Device, Command, &Device->PreviousCommand);
    Device->PreviousCommand = 0;

    if (Device->ExpressPort != NULL) {

        if (Device->ExpressPort->EnableSriovDecodes != FALSE) {
            PciEnableSriovDecodes(Device);
        }

        ExpressPortObffEnable(Device->ExpressPort);
    }

    PCI_DEVICE_SET_FLAG(Device, Connected);
}

PCI_DISCONNECTION_POLICY
PciCanDisableDevice(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine is used to check whether a device can be disabled,
    meaning that it can be disconnected from the bus for an indefinite
    period of time, for example because a IRP_MN_STOP_DEVICE IRP has
    been sent.  This is not used to see whether a device can be
    disconnected from the bus for a transient operation, like probing
    the BARs.  Disconnecting for transient operations is not considered
    disabling.  Note that since disabling a device can mean an implicit
    transition to D3, the ability of the device to enter D3 is conveyed by this
    routine as well.

Arguments:

    Device - The PCI_DEVICE in question.

Return Value:

    PowerDownDevice if the device can be disconnected and powered down.

    DisconnectDevice if the device must be kept in a high power state but can
        be disconnected from decoding its resources.

    KeepDeviceActive if the device must be kept fully active.

--*/
{

    BOOLEAN active;

    NON_PAGED_CODE();

    active = FALSE;
    if ((Device->HackFlags & PCI_HACK_NEVER_DISCONNECT) ||
        (Device->HackFlags & PCI_HACK_NO_DISABLE_FOR_STOP_REMOVE) ||
        (Device->HackFlags & PCI_HACK_DONT_DISABLE)) {

        active = TRUE;
    }

    if (Device->HackFlags & PCI_HACK_CB_SHARE_CMD_BITS) {
        //
        // This is a multifunction cardbus controller with a shared
        // command register.  Never turn off any of the functions because it has
        // the unfortunate side effect of turning off all of them!
        //









        active = TRUE;
    }

    //
    // Devices on the debugging path should always stay on.  Debugging devices
    // always fail QueryRemove and QueryStop, but Removes can come through
    // without queries (after a failed start, for example).
    //
    if (Device->OnDebugPath) {

        active = TRUE;
    }

    if (active != FALSE) {
        return KeepDeviceActive;
    }

    if (Device->HackFlags & PCI_HACK_DISABLE_POWER_DOWN) {
        return DisconnectDevice;
    }

    //
    // If a bridge cannot enter D3 because its bus cannot enter B2, keep the
    // bridge in a high power state but disconnect its decodes.
    //

    if ((Device->HeaderType == PCI_BRIDGE_TYPE) &&
        (Device->BridgeFlags.D3Illegal == 1)) {

        return DisconnectDevice;
    }

    return PowerDownDevice;
}

PCI_DISCONNECTION_POLICY
PciCanDisableDeviceForPowerDown(
    __in PPCI_DEVICE Device,
    __in DEVICE_POWER_STATE PowerState,
    __in POWER_ACTION PowerAction
    )
/*++

Routine Description:

    This routine is used to check whether a device can be disabled in
    order to power it down.  This is similar to PciCanDisableDevice,
    but takes into account some power-specific behaviors.

Arguments:

    Device - The PCI_DEVICE in question.

    PowerState - The power state the device will be in if it is
        powered down.

    PowerAction - The cause of the request to power down the device.

Return Value:

    PowerDownDevice if the device can be powered down normally.

    DisconnectDevice if the device must be kept in a high power state but can
        be disconnected from decoding its resources.

    KeepDeviceActive if the device must be kept fully active.

--*/
{

    BOOLEAN active;

    NON_PAGED_CODE();

    active = FALSE;

    //
    // Devices that shouldn't be disabled ever also shouldn't be
    // disabled for power down.
    //

    if ((Device->HackFlags & PCI_HACK_NEVER_DISCONNECT) ||
        (Device->HackFlags & PCI_HACK_CB_SHARE_CMD_BITS) ||
        (Device->HackFlags & PCI_HACK_DONT_DISABLE)) {

        active = TRUE;
    }

    if (Device->HackFlags & PCI_HACK_DISABLE_POWER_DOWN) {
        active = TRUE;
    }

    //
    // If this device is on the debug path then don't power it down so we
    // can report if this crashes the machine...
    //

    if (Device->OnDebugPath) {
        active = TRUE;
    }

    //
    // Device driver should likely not be powering down any device
    // that's on the hibernate path or the crashdump path
    //

    if ((PowerAction == PowerActionHibernate) &&
        (Device->DeviceUsage.Hibernate || Device->DeviceUsage.CrashDump)) {

        active = TRUE;
    }

    if ((Device->BaseClass == PCI_CLASS_BRIDGE_DEV) &&
        (Device->SubClass == PCI_SUBCLASS_BR_PCI_TO_PCI)) {

        //
        // Don't power down bridges for warm reboots, because if
        // the video or boot device is behind the bridge and the BIOS
        // doesn't power things up, we won't be able to get to the
        // boot or video path.
        //

        if ((PowerState == PowerDeviceD3) &&
            (PowerAction == PowerActionShutdownReset)) {

            active = TRUE;
        }
    }

    if (Device->OnPostPath != FALSE) {

        //
        // If we are a device on the VGA path then don't turn off for shutdown
        // so we can display the "Safe to turn off your machine" screen.  For
        // hibernate we also don't want to turn off so we can display the
        // "Dumping to disk" progress bar.  The video device is not put on the
        // hibernation path so that its driver can be updated.
        //

        if ((PowerState == PowerDeviceD3) &&
            ((PowerAction == PowerActionShutdownReset) ||
             (PowerAction == PowerActionShutdownOff) ||
             (PowerAction == PowerActionShutdown) ||
             (PowerAction == PowerActionHibernate))) {

            active = TRUE;

        }
    }

    if (active != FALSE) {
        return KeepDeviceActive;
    }

    //
    // If a bridge cannot enter D3 because its bus cannot enter B2, keep the
    // bridge in a high power state but disconnect its decodes.
    //

    if ((PowerState == PowerDeviceD3) &&
        (Device->HeaderType == PCI_BRIDGE_TYPE) &&
        (Device->BridgeFlags.D3Illegal == 1)) {

        return DisconnectDevice;
    }

    return PowerDownDevice;
}

NTSTATUS
PciReadSlotIdData(
    __in PPCI_BUS Bus,
    __in PCI_SLOT_NUMBER Slot,
    __out PPCI_DEVICE_ID_DATA IdData
    )
/*++

Routine Description:

    This routine reads the configuration data necessary to
    identify a device in the given bus/device/function,
    and returns it in the IdData structure.

Arguments:

    Bus - The PCI_BUS structure representing the bus the device is on.

    Slot - The slot the device is in.

    IdData - On success, this structure will contain the identification data
        of the device.

Return Value:

    STATUS_SUCCESS - If the data was retrieved successfully
    STATUS_DEVICE_PROTOCOL_ERROR - If there was an error communicating
        with the device while retrieving the identification data.

--*/
{
    PCI_CAPABILITIES_HEADER header;
    UCHAR headerType;
    USHORT status;
    UCHAR capPtr;

    NON_PAGED_CODE();

    RtlZeroMemory(IdData, sizeof(PCI_DEVICE_ID_DATA));

    PciReadSlotConfigRegister(Bus, Slot, VendorID, &IdData->VendorID);
    if ((IdData->VendorID == 0xFFFF) || (IdData->VendorID == 0)) {
        return STATUS_DEVICE_DOES_NOT_EXIST;
    }

    PciReadSlotConfigRegister(Bus, Slot, DeviceID, &IdData->DeviceID);
    PciReadSlotConfigRegister(Bus, Slot, RevisionID, &IdData->RevisionID);
    PciReadSlotConfigRegister(Bus, Slot, ProgIf, &IdData->ProgIf);
    PciReadSlotConfigRegister(Bus, Slot, SubClass, &IdData->SubClass);
    PciReadSlotConfigRegister(Bus, Slot, BaseClass, &IdData->BaseClass);
    PciReadSlotConfigRegister(Bus, Slot, HeaderType, &IdData->HeaderType);

    //
    // Now get the subsystem IDs.  These are in a different place per header type.
    //
    headerType = IdData->HeaderType & ~PCI_MULTIFUNCTION;
    switch (headerType) {

    case PCI_DEVICE_TYPE:

        PciReadSlotConfigRegister(Bus, Slot, type0.SubVendorID, &IdData->SubVendorID);
        PciReadSlotConfigRegister(Bus, Slot, type0.SubSystemID, &IdData->SubSystemID);
        break;

    case PCI_BRIDGE_TYPE:

        PciReadSlotConfigRegister(Bus, Slot, Status, &status);
        if (!(status & PCI_STATUS_CAPABILITIES_LIST)) {
            break;
        }
        PciReadSlotConfigRegister(Bus, Slot, type1.CapabilitiesPtr, &capPtr);

        while (capPtr) {

            if ((capPtr < PCI_TYPE1_HEADER_LENGTH) ||
                ((capPtr & 0x3) != 0) ||
                (capPtr > (sizeof(PCI_COMMON_CONFIG)
                           - sizeof(PCI_CAPABILITIES_HEADER)))) {

                return STATUS_DEVICE_PROTOCOL_ERROR;
            }

            PciReadSlotConfig(Bus,
                              Slot,
                              &header,
                              capPtr,
                              sizeof(PCI_CAPABILITIES_HEADER)
                              );
            if (header.CapabilityID == PCI_CAPABILITY_ID_P2P_SSID) {

                PciReadSlotConfig(Bus,
                                  Slot,
                                  &IdData->SubVendorID,
                                  capPtr
                                  + FIELD_OFFSET(PCI_SUBSYSTEM_IDS_CAPABILITY,SubVendorID),
                                  sizeof(USHORT)
                                  );

                PciReadSlotConfig(Bus,
                                  Slot,
                                  &IdData->SubSystemID,
                                  capPtr
                                  + FIELD_OFFSET(PCI_SUBSYSTEM_IDS_CAPABILITY,SubSystemID),
                                  sizeof(USHORT)
                                  );
                break;
            }

            capPtr = header.Next;
        }
        break;

    case PCI_CARDBUS_BRIDGE_TYPE:

        PciReadSlotConfigRegister(Bus,
                                  Slot,
                                  type2.SubVendorID,
                                  &IdData->SubVendorID
                                  );
        PciReadSlotConfigRegister(Bus,
                                  Slot,
                                  type2.SubSystemID,
                                  &IdData->SubSystemID
                                  );
        break;

    default:

        //
        // Illegal header type
        //
        return STATUS_DEVICE_PROTOCOL_ERROR;
    }

    return STATUS_SUCCESS;
}

_IRQL_requires_(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
PciGetSavedSlotInformation(
    _In_ PPCI_BUS Bus,
    _In_ PCI_SLOT_NUMBER Slot,
    _In_ PWSTR InfoKeyName,
    _In_ ULONG SlotInfoSize,
    _Out_ PVOID SlotInfo
    )
/*++

Description:

    This retrieves the saved configuration of a device from the registry.
    This is for information that needs to be maintained across
    PDO deletions.

Arguments:

    Bus - the parent bus of the device in question.

    Slot - The slot number of the device.

    InfoKeyName - The registry key name to read

    SlotInfoSize - The size in bytes of the SlotInfo buffer.

    SlotInfo - Pointer to the buffer receiving the slot information.

Return Value:

    NT Status code.

--*/
{
    WCHAR valueName[sizeof("DEV_xx&FUN_xx")];
    PCHAR valueBuffer;
    ULONG valueLength;
    HANDLE deviceHandle;
    NTSTATUS status;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(SlotInfoSize);

    deviceHandle = NULL;

    //
    // The saved config information is stored in a subkey of the *parent's*
    // PnP device registry key.
    //
    status = IoOpenDeviceRegistryKey(Bus->PhysicalDeviceObject,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     KEY_READ | KEY_WRITE,
                                     &deviceHandle
                                     );
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Under the InfoKeyName subkey are a number of values, each with the
    // format DEV_xx&FUN_xx.  These are either the initial configs of the
    // devices underneath this parent, or other persisted data for the
    // specified slot.  The value data is a REG_BINARY containing the
    // config header for the device as saved.
    //

    status = RtlStringCbPrintfW(valueName,
                                sizeof(valueName),
                                L"DEV_%02x&FUN_%02x",
                                Slot.u.bits.DeviceNumber,
                                Slot.u.bits.FunctionNumber
                                );

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    valueBuffer = NULL;
    valueLength = 0;
    status = PciGetRegistryValue(valueName,
                                 InfoKeyName,
                                 deviceHandle,
                                 REG_BINARY,
                                 &valueBuffer,
                                 &valueLength
                                 );
    if (NT_SUCCESS(status)) {

        PCI_ASSERT(valueLength == SlotInfoSize);

        RtlCopyMemory(SlotInfo, valueBuffer, valueLength);
        PciFreePool(valueBuffer);
    }

exit:

    if (deviceHandle) {
        ZwClose(deviceHandle);
    }

    return status;
}

_IRQL_requires_(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
PciSaveSlotInformation(
    _In_ PPCI_DEVICE Device,
    _In_ PWSTR InfoKeyName,
    _In_ USHORT InfoKeySize,
    _In_ BOOLEAN IsVolatile,
    _In_ ULONG SlotInfoSize,
    _In_ PVOID SlotInfo
    )
/*++

Description:

    This saves the configuration of a device in the registry.
    This should be done when the device is first enumerated
    so that the configuration will be maintained across PDO
    deletions.  It should also be done when changes are made
    to the device that should be maintained across PDO deletions,
    like the interrupt line register being updated.  This
    configuration data is saved in a volatile registry key.

    For hot-swap/replace scenarios, where a device is replaced
    with exactly the same device (differing only by serial number),
    it is important to not report the new serial number in order
    to maintain device specific settings (iSCSI boot etc). This
    information is therefore saved in a non-volatile key that
    will persist across reboots.

Arguments:

    Device - The device to save the config header of.

    InfoKeyName - The name of the registry key to be created/updated under
                  the DeviceParameters key for this device.

    InfoKeySize - Size in bytes of the InfoKeyName string.

    IsVolatile - If true then key will be volatile and not persist
                 across reboots.

    SlotInfoSize - Size in bytes of the SlotInfo to be saved.

    SlotInfo - Pointer to the slot information to be saved.

Return Value:

    Status

--*/
{
    NTSTATUS status;
    OBJECT_ATTRIBUTES attributes;
    UNICODE_STRING unicodeString;
    HANDLE deviceHandle, configHandle;
    WCHAR buffer[sizeof("DEV_xx&FUN_xx")];

    PAGED_CODE();

    deviceHandle = NULL;
    configHandle = NULL;

    status = IoOpenDeviceRegistryKey(Device->Parent->PhysicalDeviceObject,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     KEY_WRITE,
                                     &deviceHandle
                                     );
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    unicodeString.Length = (unicodeString.MaximumLength = InfoKeySize) - sizeof(WCHAR);
    unicodeString.Buffer = InfoKeyName;

    InitializeObjectAttributes(&attributes,
                               &unicodeString,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               deviceHandle,
                               NULL
                               );

    status = ZwCreateKey(&configHandle,
                         KEY_WRITE,
                         &attributes,
                         0,
                         NULL,
                         IsVolatile ? REG_OPTION_VOLATILE : REG_OPTION_NON_VOLATILE,
                         NULL
                         );
    if (!NT_SUCCESS(status) || (configHandle == 0)) {
        goto exit;
    }

    status = RtlStringCbPrintfW(buffer,
                                sizeof(buffer),
                                L"DEV_%02x&FUN_%02x",
                                Device->Slot.u.bits.DeviceNumber,
                                Device->Slot.u.bits.FunctionNumber
                                );

    if (!NT_SUCCESS(status)) {
        ERROR("PciSaveSlotInformation: Error constructing device id (0x%08X\n).\n",
              status);

        goto exit;
    }

    RtlInitUnicodeString(&unicodeString, buffer);

    status = ZwSetValueKey(configHandle,
                           &unicodeString,
                           0,
                           REG_BINARY,
                           SlotInfo,
                           SlotInfoSize
                           );

exit:

    if (deviceHandle) {
        ZwClose(deviceHandle);
    }
    if (configHandle) {
        ZwClose(configHandle);
    }
    return status;
}

NTSTATUS
PciUpdateSavedDeviceConfiguration(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine saves away the device state that needs to be preserved in the
    registry.  The registry saved config is used to save settings across
    PDO deletions.  It is a combination of the current settings and the BIOS
    settings.

    The resource settings of a device that are computed by the OS should be
    overwritten by the settings computed by the BIOS, since the registry config
    is used to determine the preferred requirements for a device.  Also, the bus
    number assignment should not be changed from the assignment given by the
    BIOS since the registry config is used to determine whether a device is
    on the debug path, which is determined by BIOS-assigned bus/device/function.

Arguments:

    Device - The device to update the saved config of.

Return Value:

    NT Status code.

--*/
{
    PCI_SAVE_RESTORE_DATA saveRestore;
    PCI_SAVED_CONFIGURATION config;
    NTSTATUS status;

    PAGED_CODE();

    status = PciGetSavedDeviceConfiguration(Device, &config);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // For the most part, the currently saved config should be maintained.  This
    // is so that BIOS-configurations are preserved.  However, get the new values
    // of the registers that are saved/restored, to comprehend updates based
    // on _HPP and other such things.
    //
    RtlCopyMemory(&saveRestore,
                  &Device->SavedRegisters,
                  sizeof(PCI_SAVE_RESTORE_DATA)
                  );

    if ((Device->HeaderType == PCI_BRIDGE_TYPE) ||

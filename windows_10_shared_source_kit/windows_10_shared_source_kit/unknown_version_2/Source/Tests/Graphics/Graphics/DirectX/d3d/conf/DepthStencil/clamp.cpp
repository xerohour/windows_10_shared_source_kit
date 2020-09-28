etup(void);
		
	protected:
		bool DoCapsCheck();

		//Texture generation for shaders.
		virtual bool GenerateTextureData();
		virtual bool LoadTextures();
		virtual void ReleaseTextures();
		virtual void ReleaseTextureData();
		
		virtual HRESULT PopulateShaderInfo() = 0;

		void AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices);
		void AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices, DWORD dwRequired);
		void AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices, bool bAlpha);
		void AddShaderDef(char *pName, char *pCode, DWORD dwFVF, void *pVertices, ShaderType Type, DWORD dwRequired);
		void AddShaderDef(TCHAR *pName, char *pCode, DWORD dwFVF, void *pVertices, bool bAlpha, ShaderType Type, DWORD dwRequired);		

		void AddTextureToLastShaderDef(CnBaseTexture **ppTexture);
		void AddTextureToLastShaderDef(CnBaseTexture **ppTexture, UINT uiStage);
		void SetLastShaderWrapAndMip(void);
		void SetCoordsForLastPSStages(DWORD dwStage, DWORD dwCoordNum);
		void SetLastShaderTexFormats(BYTE byFormats);
		void SetLastShaderFlags(DWORD dwFlags);
		void SetConstantsStart(DWORD dwStart);
		void AddConstantsToLastShaderDef(float a, float b, float c, float d);
		void AddConstantsToLastShaderDef(float f[4]);
		void AddConstantsToLastShaderDef(float *f, DWORD dwCount);
		void AddBumpStageForLastShaderDef(UINT uStageNum);

		bool GenerateBrickTexture(CImage *pImage);
		bool GenerateBumpMap(CImage* pImage);

		DWORD m_dwMinPSVersion, m_dwRequestedVersion;
		int m_nTexFormat, m_nBumpFormat, m_nCubeFormat, m_nVolFormat;
		UINT m_uPSCommonBumpFormats, m_uPSCommonTexFormats, m_uPSCommonCubeFormats;
		UINT m_uPSCommonVolFormats, m_uTotalFormats, m_uNumShaderVersions;
		bool SetTextureFormat(BYTE byFormat = 0);		
		void CleanUpReg(void);
		void ReCreateShaderAndBuffers(MYSHADER * pShader);

	private:
		bool SetupBasicScene();
		bool CreateLoadedVertexBuffers(MYSHADER * pShader);
		bool m_bSupports255, m_bSpecificVerRequest;
		DWORD m_dwTested255;
		UINT m_uiLastTest, m_uiShaderNotRendered;
		bool m_bFormatChanged;    //Has any format changed since last time textures were loaded
		bool m_bShaderAdded;	//This is a flag to say if the last shader was added or not 
								//It prevents the need for if statements around all the AddShaderDef calls.
		bool CreateShaderVersionArray();
		void ResetAllShaderTexFormats();
		void ResetShaderTexFormats(MYSHADER * pShader);
		bool NeedPalette(UINT uNumFormats, FORMAT *pFormats);
		static FLOAT fBumpDu(FLOAT u, FLOAT v);
		static FLOAT fBumpDv(FLOAT u, FLOAT v);
		static FLOAT fBumpLuminance(FLOAT u, FLOAT v);		
		static FLOAT fSin(FLOAT u, FLOAT v);
		void DeleteShaderMemory(MYSHADER * pShader);

	protected:
		bool            m_bLastForward;
		SHADERVECTOR	m_vShaders;
		DWORD			m_dwSupported;
		DWORD			m_dwCurrentShader;
		DWORD			m_dwCurrentVersion;
		BYTE			m_byFormats;			
		std::vector<DWORD> m_vShaderVersions;
		CFlags			m_Flags;
		int m_nDisplaySizePow2;

		FORMAT *m_pTexFormats, *m_pBumpFormats;

		//Textures for shaders.
		CImage      m_DXLogoImage;
		CImage      m_CheckerImage;
		CImage      m_ColorCheckerImage;
		CImage      m_BrickImage;

		CnTexture   *m_pDXLogoTex;
		CnTexture   *m_pCheckerTex;
		CnTexture   *m_pColorCheckerTex;
		CnTexture   *m_pBrickTex;
};

class CPSOps : public CPixelShader
{
	public:
		CPSOps();
		UINT TestInitialize(void);

	protected:

		bool GenerateTextureData();
		bool LoadTextures();
		void ReleaseTextures();
	
		HRESULT PopulateShaderInfo();

		bool		m_bBump;

		CImage      m_Checker2Image;
		CImage      m_DP3Image;
		CImage      m_GradientImage;
		CImage      m_StripesImage;
		CImage      m_SineBumpImage;

		CnTexture   *m_pChecker2Tex;        
		CnTexture   *m_pDP3Tex;        
		CnTexture   *m_pGradientTex;		
		CnTexture   *m_pStripesTex;		
		CnTexture   *m_pSineBumpTex;
};

class CPSOps_Bump : public CPSOps
{
	public:
		CPSOps_Bump();

	protected:
		HRESULT PopulateShaderInfo();
};

class CPSRegister : public CPixelShader
{
	public:
		CPSRegister();
		UINT TestInitialize(void);

	protected:
		HRESULT PopulateShaderInfo();
};

class CPSSrcMod : public CPixelShader
{
	public:
		CPSSrcMod();
		UINT TestInitialize(void);

	protected:

   		bool GenerateTextureData();
		bool LoadTextures();
		void ReleaseTextures();
	
		HRESULT PopulateShaderInfo();


		CImage m_DP3Image;
		CImage m_GradientImage;

		CnTexture *m_pDP3Tex;
		CnTexture *m_pGradientTex;
};

class CPSDestMod : public CPixelShader
{
	public:
		CPSDestMod();
		UINT TestInitialize(void);

	protected:
		HRESULT PopulateShaderInfo();
};

class CPSAddress : public CPixelShader
{
	public:

		CPSAddress();
		UINT TestInitialize(void);

	protected:
               
		bool GenerateTextureData();
		bool LoadTextures();
		void ReleaseTextures();
		void ReleaseTextureData();
	
		HRESULT PopulateShaderInfo();
		HRESULT PopulateShaderInfo_Bump();
		HRESULT PopulateShaderInfo_Cube();
		HRESULT PopulateShaderInfo_Vol();
		HRESULT PopulateShaderInfo_BumpCube();

		bool		m_bBump;
		bool		m_bCube;
		bool		m_bVol;

		CImage      m_StripesVecBx2Image;
		CImage		m_VecSomeBx2Image;
		CImage		m_StripesImage;
		CImage		m_SineBumpImage;
		CImage		m_AddStripesImage;
		CImage      m_GradientImage;

		CImage      *m_pCubeImage[6];

		CImage      *m_pMipImage[MIP_LEVELS];
		
		CImage      *m_pVolImage[VOLUME_DEPTH];

		CnTexture	    *m_pStripesVecBx2;
		CnTexture	    *m_pVecSomeBx2;        
		CnTexture		*m_pStripesTex;
		CnTexture		*m_pSineBumpTex;		
		CnTexture		*m_pAddStripesTex;		
		CnTexture       *m_pGradientTex;		
		CnTexture       *m_pMipsTex;
		CnCubeTexture	*m_pCubeTex;
		CnVolTexture    *m_pVolTex;
};

class CPSAddress_Bump : public CPSAddress
{
	public:
		CPSAddress_Bump();
};

class CPSAddress_Cube : public CPSAddress
{
	public:
		CPSAddress_Cube();
};

class CPSAddress_Vol : public CPSAddress
{
	public:
		CPSAddress_Vol();
};

class CPSAddress_BumpCube : public CPSAddress
{
	public:
		CPSAddress_BumpCube();
};

class CReadShader : public CPixelShader
{
	public:
		CReadShader();
		~CReadShader();

		UINT TestInitialize(void);
		bool ExecuteTest(UINT);
		bool TestTerminate(void);
		void CommandLineHelp(void);		
		void ProcessArgs(void);
	
	protected:
		bool GenerateTextureData();
		bool LoadTextures();
		void ReleaseTextures();
	
		HRESULT PopulateShaderInfo();

	private:		

		HRESULT AddShader(TCHAR *sztType, TCHAR *sztName, TCHAR *sztCodes, DWORD dwVersion);
		TCHAR m_sztFilename[BUFFER_LENGTH];

		CImage	m_StripesImage;
		CImage	m_SineBumpImage;
		CImage	m_GradientImage;
        
		CnTexture	*m_pStripesTex;
		CnTexture	*m_pSineBumpTex;
		CnTexture	*m_pGradientTex;
};

class CPSVertShader : public CPSAddress
{
	public:
		CPSVertShader();
		UINT TestInitialize(void);
		void ProcessArgs(void);
		void CommandLineHelp(void);
	private:
		void ChangeVertexShaderType();
		bool GetVertexShaderCode(MYSHADER * pShader, TCHAR *sztCode, DWORD dwSize, DWORD *rgdwDeclarFromFVF);
		HRESULT DeclaratorFromFVF(DWORD dwFVF, DWORD Declarator[MAX_FVF_DECL_SIZE]);
};

class CPSVertShader_Bump : public CPSVertShader
{
	public:
		CPSVertShader_Bump();
};

class CPSVertShader_Cube : public CPSVertShader
{
	public:
		CPSVertShader_Cube();
};

class CPSVertShader_Vol : public CPSVertShader
{
	public:
		CPSVertShader_Vol();
};

class CPSVertShader_BumpCube : public CPSVertShader
{
	public:
		CPSVertShader_BumpCube();
};

class CPSLimits : public CPixelShader
{
	public:
		CPSLimits();
		UINT TestInitialize(void);

	protected:

		bool GenerateTextureData();
		bool LoadTextures();
		void ReleaseTextures();
	
		HRES
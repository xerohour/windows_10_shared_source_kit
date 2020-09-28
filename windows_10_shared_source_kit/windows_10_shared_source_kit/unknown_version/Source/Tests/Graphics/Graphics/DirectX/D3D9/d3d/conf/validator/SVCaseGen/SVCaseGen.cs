using System;
using System.Collections;
using System.IO;

namespace SVCaseGen
{
	public struct GeneratorOptions
	{
		public GeneratorOptions( string	shaderVersion, string instruction, int numSourceRegs,
			bool staticFlow, bool dynamicFlow, bool isAllowedInDFC, 
			string [ ] destRegsAllowed, string [ ] destModsAllowed,
			string [ ] writeMasksAllowed, SrcRegOptions [ ] srcRegInfo )
		{
			ShaderVersion = shaderVersion;
			Instruction = instruction;
			NumSourceRegs = numSourceRegs;
			StaticFlow = staticFlow;
			DynamicFlow = dynamicFlow;
			IsAllowedInDFC = isAllowedInDFC;
			DestRegsAllowed = destRegsAllowed;
			DestModsAllowed = destModsAllowed;
			WriteMasksAllowed = writeMasksAllowed;
			SrcRegInfo = srcRegInfo;
		}
									
		public string				ShaderVersion;
		public string				Instruction;
		public int					NumSourceRegs;
		public bool					StaticFlow;
		public bool					DynamicFlow;
		public bool					IsAllowedInDFC;
		public string [ ]			DestRegsAllowed;
		public string [ ]			DestModsAllowed;
		public string [ ]			WriteMasksAllowed;
		public SrcRegOptions [ ] 	SrcRegInfo; 
	}

	//-------------------------------------------------------------------------------------------------------------------------

	public struct FlowInfo
	{
		public FlowInfo(int DFNest, int SFNest, int loopNest, int callNest, int SFCount, int count, bool pass)
		{
			DynamicNesting = DFNest;
			StaticNesting  = SFNest;
			LoopNesting    = loopNest;
			CallNesting    = callNest;
			StaticCount    = SFCount;
			BlockCount     = count;
			Pass           = pass;
		}
		public void ClearFlowInfo()
		{
			DynamicNesting = 0;
			StaticNesting  = 0;
			LoopNesting    = 0;
			CallNesting    = 0;
			StaticCount    = 0;
			BlockCount     = 0;
			Pass           = true;
		}

		public int  DynamicNesting;
		public int  StaticNesting;
		public int  LoopNesting;
		public int  CallNesting;
		public int  StaticCount;
		public int  BlockCount;
		public bool Pass;
	}

	//-------------------------------------------------------------------------------------------------------------------------
	 
	public struct SrcRegOptions
	{
		public SrcRegOptions( string [ ] srcRegsAllowed, bool sourceMod, bool replicateSwizzles,
			bool arbitrarySwizzles, bool generalSwizzles, bool mustInitialize )
		{
			SrcRegsAllowed = srcRegsAllowed;
			SourceMod = sourceMod;
			ReplicateSwizzles = replicateSwizzles;
			ArbitrarySwizzles = arbitrarySwizzles;
			GeneralSwizzles = generalSwizzles;
			MustInitialize = mustInitialize;
		}
		
		public string [ ]		SrcRegsAllowed;

		//					allow = true, not allow = false
		public bool			SourceMod;
		public bool			ReplicateSwizzles;
		public bool			ArbitrarySwizzles;
		public bool			GeneralSwizzles;
		public bool			MustInitialize;
	}

	//-------------------------------------------------------------------------------------------------------------------------

	public class SVCaseGen
	{
		private string [ ] writeMasks = 
		{
			"x", "y", "z", "w",
			"xy", "yz", "zw", "xz", "xw", "yw",
			"xyz", "yzw", "xzw",
			"xyzw"
		};

		private string [ ] invalidMasks =
		{
			"yx", "zx", "zy", "wx", "wz", "wy",
			"zyx", "wzy", "wzx", "wyx", "yxw",
			"wzyx", "zxwy"
		};

		private string [ ] repSwizzles = { "x", "y", "z", "w" };
		private string [ ] genSwizzles = { "yzxw", "zxyw", "wzyx" };
		private string [ ] arbSwizzles = 
		{ 
			"wyxz", "xzyw", "xywz",
			"zyx", "xzy", "ywx",
			"yx", "wz", "zy"
		};

		private string [ ] dclUsageV = { "position", "texcoord", "color", "normal", "psize" };
		private string [ ] dclUsageO = { "color", "texcoord", "fog", "tangent", "depth" };
		private string [ ] dclUsageS = { "2d", "cube", "volume" };

		// flow control block instructions
		private string [ ] staticFlowControl  =	{ "if b0", "if b0 nop else", "loop aL, i0", "rep i0" };
		private string [ ] dynamicFlowControl = {	"if_gt c0.x, c0.y", "if_gt c0.x, c0.y nop else", "if p0.x", "if p0.x nop else",
													"loop aL, i0 break", "rep i0 break", "loop aL, i0 breakp p0.x", "rep i0 breakp p0.x",
													"loop aL, i0 break_gt c0.x, c0.y", "rep i0 break_gt c0.x, c0.y" };

		private string [ ] compMods	= { "gt", "lt", "ge", "le", "ne", "eq" };

		// instructions that require ONLY replicate swizzles
		private string [ ] repInstructions = { "exp", "expp", "log", "logp", "pow", "rcp", "rsq", "dp2add" };

		private string [ ] specialTexInst = { "texbem", "texbeml", "texm3x2tex", "texm3x3spec", "texm3x3tex", "texm3x3vspec", "texreg2ar",
												"texreg2gb", "texdp3", "texdp3tex", "texm3x3", "texreg2rgb" };

		private string [ ] shaderVersions =
		{
			"vs_3_0", "vs_3_sw",
			"vs_2_0", "vs_2_x", "vs_2_sw",
			"vs_1_1",
			"ps_3_0", "ps_3_sw",
			"ps_2_0", "ps_2_x", "ps_2_sw",
			"ps_1_4", "ps_1_3", "ps_1_2", "ps_1_1"
		};

		struct VersionInfo
		{
			public VersionInfo( string version,
				string versionCode,
				bool softwareRequired,
				string [ ] registers,
				int [ ] readLimits,
				int [ ] readPorts,
				string [ ] sourceMods,
				string [ ] destMods )
			{
				Version = version;
				VersionCode = versionCode;
				SoftwareRequired = softwareRequired;
				Registers = registers;
				ReadLimits = readLimits;
				ReadPorts = readPorts;
				SourceMods = sourceMods;
				DestMods = destMods;
			}

			public string		Version;
			public string		VersionCode;
			public bool			SoftwareRequired;
			public string [ ]	Registers;
			public int [ ]		ReadLimits;
			public int [ ]		ReadPorts;
			public string [ ]	SourceMods;
			public string [ ]	DestMods;			
		}

		enum ShaderType
		{
			Vertex,
			Pixel
		}

		private delegate bool SpecialCase( string result, ref int caseNum, ShaderType type, ref GeneratorOptions options );

		private Hashtable versionToInfo;
		private Hashtable instToCase;

		public SVCaseGen()
		{
			versionToInfo = new Hashtable( );
			instToCase = new Hashtable( );

			VersionInfo VS3 = new VersionInfo( 
				"vs_3_0",
				"0x0300",
				false,
				new string[ ]	{ "v#",	"r#", "c#",	"a0", "b#", "i#", "aL", "p0", "s#", "o#" },
				new int[ ]		{    0,	   0,    0,    0,    1,    1,    0,    1,    1,   -1 },
				new int[ ]		{    1,	   3,    1,    1,    1,    1,    1,    1,    1,   -1 },
				new string[ ]	{ "-", "_abs", "-_abs", "" },
				new string[ ]	{ "_sat" }			
				);

			versionToInfo[ "vs_3_0" ] = VS3;
			VS3.Version = "vs_3_sw";
			VS3.SoftwareRequired = true;
			versionToInfo[ "vs_3_sw" ] = VS3;

			VersionInfo VS2 = new VersionInfo( 
				"vs_2_0",
				"0x0200",
				false,
				new string[ ]	{ "v#", "r#", "c#", "a0", "b#", "i#", "aL", "oPos", "oFog", "oPts", "oD#", "oT#" },
				new int[ ]		{    0,	   0,    2,    2,    1,    1,    2,     -1,     -1,     -1,    -1,    -1 },
				new int[ ]		{    1,	   3,    1,    1,    1,    1,    1,     -1,     -1,     -1,    -1,    -1 },
				new string[ ]	{ "-", "" },
				new string[ ]	{ }			
				);

			versionToInfo[ "vs_2_0" ] = VS2;
			VS2.Version = "vs_2_x";
			VS2.Registers = new string [ ]	{ "v#", "r#", "c#", "a0", "b#", "i#", "aL", "p0", "oPos", "oFog", "oPts", "oD#", "oT#" };
			VS2.ReadLimits = new int[ ]		{    0,	   0,    2,    2,    1,    1,    2,    1,     -1,     -1,     -1,    -1,    -1 };
			VS2.ReadPorts = new int[ ]		{    1,	   3,    1,    1,    1,    1,    1,    1,     -1,     -1,     -1,    -1,    -1 };

			versionToInfo[ "vs_2_x" ] = VS2;
			VS2.Version = "vs_2_sw";
			VS2.SoftwareRequired = true;
			versionToInfo[ "vs_2_sw" ] = VS2;

			VersionInfo VS1 = new VersionInfo( 
				"vs_1_1",
				"0x0101",
				false,
				new string[ ]	{ "v#", "r#", "c#", "a0", "oPos", "oFog", "oPts", "oD#", "oT#" },
				new int[ ]		{    0,    0,    0,    0,     -1,     -1,     -1,    -1,    -1 },
				new int[ ]		{    1,    3,    1,    1,     -1,     -1,     -1,    -1,    -1 },
				new string[ ]	{ "-", "" },
				new string[ ]	{ }
				);

			versionToInfo[ "vs_1_1" ] = VS1;

			VersionInfo PS3 = new VersionInfo( 
				"ps_3_0",
				"0x0300",
				false,
				new string[ ]	{ "v#", "r#", "c#", "b#", "i#", "aL", "p0", "s#", "vFace", "vPos", "oC#", "oDepth" },
				new int[ ]		{    0,    0,    0,    1,    1,    0,    1,    1,       0,      0,    -1,       -1 },
				new int[ ]		{    1,    3,    1,    1,    1,    1,    1,    1,       1,      1,    -1,       -1 },
				new string[ ]	{ "-", "_abs", "-_abs", "" },
				new string[ ]	{ "_pp", "_centroid", "_sat" }
				);

			versionToInfo[ "ps_3_0" ] = PS3;
			PS3.Version = "ps_3_sw";
			PS3.SoftwareRequired = true;
			versionToInfo[ "ps_3_sw" ] = PS3;
			
			VersionInfo PS2 = new VersionInfo( 
				"ps_2_0",
				"0x0200",
				false,
				new string[ ]	{ "v#", "r#", "c#", "b#", "i#", "p0", "s#", "t#", "oC#", "oDepth" },
				new int[ ]		{    0,    0,    2,    1,    1,    1,    1,    1,    -1,       -1 },
				new int[ ]		{    1,    3,    1,    1,    1,    1,    1,    1,    -1,       -1 },
				new string[ ]	{ "-", "" },
				new string[ ]	{ "_pp", "_sat" }
				);

			versionToInfo[ "ps_2_0" ] = PS2;
			PS2.Version = "ps_2_x";
			versionToInfo[ "ps_2_x" ] = PS2;
			PS2.Version = "ps_2_sw";
			PS2.SoftwareRequired = true;
			versionToInfo[ "ps_2_sw" ] = PS2;
			
			VersionInfo PS14 = new VersionInfo( 
				"ps_1_4",
				"0x0104",
				false,
				new string[ ]	{ "v#", "r#", "c#", "t#" },
				new int[ ]		{    0,    0,    0,    0 },
				new int[ ]		{    2,    3,    2,    1 },
				new string[ ]	{ "_bias", "1-", "-", "_x2", "_bx2", "_dz", "_db", "_dw", "_da", "-_x2", "-_bx2", "" },
				new string[ ]	{ "_x2", "_x4", "_x8", "_d2", "_d4", "_d8", "_sat" }
				);
			versionToInfo[ "ps_1_4" ] = PS14;

			VersionInfo PS1X = new VersionInfo( 
				"ps_1_3",
				"0x0103",
				false,
				new string[ ]	{ "v#", "r#", "c#", "t#" },
				new int[ ]		{    0,    0,    0,    0 },
				new int[ ]		{    2,    2,    2,    3 },
				new string[ ]	{ "_bias", "1-", "-", "_bx2", "_dz", "_db", "_dw", "_da", "-_bx2", "" },
				new string[ ]	{ "_x2", "_x4", "_d2", "_sat" }
				);
			versionToInfo[ "ps_1_3" ] = PS1X;
			PS1X.Version = "ps_1_2";
			PS1X.VersionCode = "0x0102";
			versionToInfo[ "ps_1_2" ] = PS1X;
			PS1X.Version = "ps_1_1";
			PS1X.ReadPorts = new int[ ] { 2, 2, 2, 2 };
			PS1X.VersionCode = "0x0101";
			versionToInfo[ "ps_1_1" ] = PS1X;
		}

		public string[ ] GetWriteMasks( string version )
		{
			if(  String.IsNullOrEmpty( version ) == true )
			{
				return null;
			}

			string [ ] wm = new string[ writeMasks.Length ];
			Array.Copy( writeMasks, wm, wm.Length );
			
			if( version.StartsWith( "ps", StringComparison.Ordinal ) )
				for( int i = 0; i < wm.Length; i++ )
					wm[ i ] = PosToColor( (string) wm[ i ] );

			return wm;
		}

		private string PosToColor( string mask )
		{
			return mask.Replace( "x", "r" ).Replace( "y", "g" ).Replace( "z", "b" ).Replace( "w", "a" );
		}

        // Comment out this utility as it is not currently used
        //
        //private string ColorToPos( string mask )
        //{
        //    return mask.Replace( "r", "x" ).Replace( "g", "y" ).Replace( "b", "z" ).Replace( "a", "w" );
        //}

		public string[ ] GetShaderVersions( )
		{
			return shaderVersions;
		}

		public string [ ] GetRegisters( string version )
		{
			VersionInfo info = (VersionInfo) versionToInfo[ version ];
			return info.Registers;
		}

		public string [ ] GetDestMods( string version )
		{
			VersionInfo info = (VersionInfo) versionToInfo[ version ];
			return info.DestMods;
		}

		//-------------------------------------------------------------------------------------------------------------------------

		public string GenerateCases( GeneratorOptions options )
		{
			int numCases = 1;
			ShaderType type = options.ShaderVersion.StartsWith( "vs", StringComparison.Ordinal ) ? ShaderType.Vertex : ShaderType.Pixel;
			string cases = "";
			if( !versionToInfo.Contains( options.ShaderVersion ) )
				return cases;

			if( options.Instruction == "texldd" )
				ChangeOptionsTexldd( ref options );

			cases += GetSourceRegCases( ref numCases, type, ref options );
			cases += GetDestRegCases( ref numCases, type, ref options );
			cases += GetWriteMaskCases( ref numCases, type, ref options );
			cases += GetFlowControlCases( ref numCases, type, ref options );

			string header = "#include \"Test_Include.fx\"\r\n\r\n" +
				"int Test_Count = " + (numCases - 1) + ";\r\n\r\n" +
				"string TestInfo\r\n" +
				"<\r\n" +
				"	string TestType = " + ( type == ShaderType.Vertex ? "\"VS\";" : "\"PS\";" ) + "\r\n" +
				">\r\n" +
				"= \"" + options.ShaderVersion + "_inst_" + options.Instruction + "\";\r\n\r\n";
							
			return header + cases;			
		}
		
		//-------------------------------------------------------------------------------------------------------------------------

		private string GetSourceRegCases( ref int caseNum, ShaderType type, ref GeneratorOptions options )
		{
			string cases = "";
			VersionInfo info = (VersionInfo) versionToInfo[ options.ShaderVersion ];
			string prefix = type == ShaderType.Pixel ? "PS" : "VS";
			string destReg = options.DestRegsAllowed != null ? options.DestRegsAllowed[ 0 ] : null;   			
			if( destReg == null || options.NumSourceRegs == 0 )
				return cases;
			if( options.Instruction == "bem" )
				destReg += ".rg";

			ArrayList comboList = CreateRegCombos( options.SrcRegInfo );
			ArrayList passCases = new ArrayList( );
			ArrayList failCases = new ArrayList( );
			foreach( ArrayList combo in comboList )
				GetComboScenarios( ref options, combo, passCases, failCases );

			SrcRegOptions [ ] failInfo = new SrcRegOptions[ options.NumSourceRegs ];
			for( int i = 0; i < failInfo.Length; i++ )
				failInfo[ i ].SrcRegsAllowed = GetMissingElements( info.Registers, options.SrcRegInfo[ i ].SrcRegsAllowed );
			comboList = CreateRegCombos( failInfo );
			for( int i = 0; i < comboList.Count; i++ )
			{
				ArrayList combo = (ArrayList) comboList[ i ];
				for( int j = 0; j < combo.Count; j++ )
					combo[ j ] = ( (string) combo[ j ] ).Replace( "#", "0" );
				failCases.Add( combo );
			}
			
			GetSourceModScenarios( ref options, type, passCases, failCases );
			
			foreach( ArrayList pass in passCases )
				cases += GenerateSourceCase( true, pass, prefix, ref options, type, caseNum++ );

			foreach( ArrayList fail in failCases )
				cases += GenerateSourceCase( false, fail, prefix, ref options, type, caseNum++ );

			return cases;
		}

		//-------------------------------------------------------------------------------------------------------------------------

		private string GetDestRegCases( ref int caseNum, ShaderType type, ref GeneratorOptions options )
		{
			if( Array.IndexOf( options.WriteMasksAllowed, "xyzw" ) < 0 && Array.IndexOf( options.WriteMasksAllowed, "rgba" ) < 0 )
				return "";

			string cases = "";
			VersionInfo info = (VersionInfo) versionToInfo[ options.ShaderVersion ];
			string prefix = type == ShaderType.Vertex ? "VS" : "PS";
			string [ ] invalidRegs = GetMissingElements( info.Registers, options.DestRegsAllowed );
			string [ ] invalidMods = GetMissingElements( info.DestMods, options.DestModsAllowed );
			string minDX = "0.f";
			bool   pred;
			
			string sourceRegs = "";
			ArrayList sourceList = new ArrayList( );
			if( options.NumSourceRegs > 0 )
			{
				ArrayList combos = CreateRegCombos( options.SrcRegInfo );
				if( combos.Count == 0 )
					return "";
				ArrayList pass = new ArrayList( );
				ArrayList dummy = new ArrayList( );
				foreach( ArrayList combo in combos )
					GetComboScenarios( ref options, combo, pass, dummy );
				sourceList = (ArrayList) pass[ 0 ];
			}
			
			// special cases //////
			string tex = "";
            if (options.Instruction.StartsWith("texm3x2", StringComparison.Ordinal))
			{
				tex += FormatShaderLine( "texm3x2pad t1, t0", false );
			}
            else if (options.Instruction.StartsWith("texm3x3", StringComparison.Ordinal))
			{
				tex +=	
					FormatShaderLine( "texm3x3pad t1, t0", false ) +
					FormatShaderLine( "texm3x3pad t2, t0", false );
			}
			else if( options.Instruction == "texld" && options.ShaderVersion == "ps_1_4" )
			{
				sourceList[ 0 ] += ".xyz";
			}
			else if( Array.IndexOf( repInstructions, options.Instruction ) >= 0 )
			{
				for( int i = 0; i < sourceList.Count; i++ )
                    if (((string)sourceList[i]).IndexOf("vPos", StringComparison.Ordinal) < 0)
						sourceList[ i ] += ".x";
			}
            else if (options.Instruction == "cnd" && options.ShaderVersion.StartsWith("ps_1", StringComparison.Ordinal) && options.ShaderVersion != "ps_1_4")
			{
				sourceList[ 0 ] = "r0.a";
			}
			
			ArrayList sourceListOrig = (ArrayList) sourceList.Clone( );
			//////////////////////
			
			string defs;

			string caseStr;
			foreach( string reg in options.DestRegsAllowed )
			{
				string realReg = reg.Replace( "#", ( reg[ 0 ] == 'c' ? "1" : "0" ) );
				// special cases
				if( Array.IndexOf( specialTexInst, options.Instruction ) >= 0 /* && options.Instruction != "texld" */ )
					realReg = "t3";
                if (options.Instruction.StartsWith("texm3x2", StringComparison.Ordinal))
					realReg = "t2";
				else if( ( options.Instruction == "dp4" || options.Instruction == "cmp" ) && ( options.ShaderVersion == "ps_1_2" || options.ShaderVersion == "ps_1_3" ) )
					realReg = "r1";
				else if( options.Instruction == "texdepth" )
				{
					realReg = "r5";
					tex = 
						FormatShaderLine( "mov r5, c0", false ) +
						FormatShaderLine( "phase", false );
				}
				else if( options.Instruction == "texld" || options.Instruction == "sincos" )
					realReg = "r4";
				
				sourceList = (ArrayList) sourceListOrig.Clone( );
				string dcl = "";
				if( reg == "o#" )
					dcl = FormatShaderLine( "dcl_" + dclUsageO[ new Random( ).Next( dclUsageO.Length ) ] + " " + realReg, false );
				else if( ( reg == "oDepth" || reg == "oFog" || reg == "oPts" ) && Array.IndexOf( repInstructions, options.Instruction ) < 0 )
				{
					for( int i = 0; i < sourceList.Count; i++ )
                        if (((string)sourceList[i]).IndexOf("vPos", StringComparison.Ordinal) < 0)
							sourceList[ i ] += ".x";
						else
							sourceList[ i ] = "vPos.x";

				}

				sourceRegs = "";
				for( int i = 0; i < sourceList.Count; i++ )
					sourceRegs += ", " + sourceList[ i ]; 
				defs = GetConstantDefs( ref options, sourceList, realReg );

                pred = sourceRegs.IndexOf("p0", StringComparison.Ordinal) >= 0 || realReg.IndexOf("p0", StringComparison.Ordinal) >= 0;

				caseStr = GetCaseString( type, caseNum++ );
				cases += 
					GetCaseHeader( true, prefix, caseStr, ref options, "dest " + reg + " is allowed", minDX, -1, -1, false, pred ) +
					FormatShaderLine( options.ShaderVersion, false ) +
					dcl +
					defs +
					tex +
					FormatShaderLine( options.Instruction + " " + realReg + sourceRegs, reg == "oC#" ) +
					( reg != "oC#" ? GetPSOutput( ref options ) : "" );

				foreach( string mod in options.DestModsAllowed )
				{
                    if (mod == "_sat" && (realReg.IndexOf("oDepth", StringComparison.Ordinal) >= 0 || realReg.IndexOf("oC", StringComparison.Ordinal) >= 0))
						continue;

                    pred = sourceRegs.IndexOf("p0", StringComparison.Ordinal) >= 0 || realReg.IndexOf("p0", StringComparison.Ordinal) >= 0;
					
					caseStr = GetCaseString( type, caseNum++ );
					cases += 
						GetCaseHeader( true, prefix, caseStr, ref options, mod + " dest " + reg + " is allowed", minDX, -1, -1, false, pred ) +
						FormatShaderLine( options.ShaderVersion, false ) +
						dcl +
						defs +
						tex +
						FormatShaderLine( options.Instruction + mod + " " + realReg + sourceRegs, reg == "oC#" ) +
						( reg != "oC#" ? GetPSOutput( ref options ) : "" );

				}
				foreach( string mod in invalidMods )
				{
                    pred = sourceRegs.IndexOf("p0", StringComparison.Ordinal) >= 0 || realReg.IndexOf("p0", StringComparison.Ordinal) >= 0;
					
					caseStr = GetCaseString( type, caseNum++ );
					cases += 
						GetCaseHeader( false, prefix, caseStr, ref options, mod + " dest " + reg + " is NOT allowed", minDX, -1, -1, false, pred ) +
						FormatShaderLine( options.ShaderVersion, false ) +
						dcl +
						defs +
						tex +
						FormatShaderLine( options.Instruction + mod + " " + realReg + sourceRegs, reg == "oC#" ) +
						( reg != "oC#" ? GetPSOutput( ref options ) : "" );
				}
			}

			foreach( string reg in invalidRegs )
			{
				// Instructions of the form "killreg oC0" are not valid; however, there was a bug in the test that
				// was treating them as valid (Windows 8 177123). Once this test bug is fixed, it will expose a product
				// bug in the shader validator (Windows 8 348173). Because the product bug is not going to be fixed,
				// supress the generation of these cases.
				if (options.Instruction.StartsWith("texkill", StringComparison.Ordinal) && reg == "oC#")
					continue;

				string realReg = reg.Replace( "#", ( reg[ 0 ] == 'c' ? "1" : "0" ) );
				string dcl = "";
				// special cases
				sourceList = (ArrayList) sourceListOrig.Clone( );
				if( reg == "o#" )
					dcl = FormatShaderLine( "dcl_" + dclUsageO[ new Random( ).Next( dclUsageO.Length ) ] + " " + realReg, false );
				else if( options.NumSourceRegs > 0 && ( reg == "oFog" || reg == "oPts" ) )
					continue;
				else if( reg == "oDepth" && Array.IndexOf( repInstructions, options.Instruction ) < 0 )
				{
					for( int i = 0; i < sourceList.Count; i++ )
                        if (((string)sourceList[i]).IndexOf("vPos", StringComparison.Ordinal) < 0)
							sourceList[ i ] += ".x";
						else
							sourceList[ i ] = "vPos.x";
				}

				sourceRegs = "";
				for( int i = 0; i < sourceList.Count; i++ )
					sourceRegs += ", " + sourceList[ i ]; 
				defs = GetConstantDefs( ref options, sourceList, realReg );

				minDX = "0.f";
				// bugs #84085 and #84086
                if ((options.Instruction.StartsWith("setp", StringComparison.Ordinal) && realReg.StartsWith("r", StringComparison.Ordinal) && options.ShaderVersion.StartsWith("ps_2", StringComparison.Ordinal)) ||
                    (realReg == "p0" && options.ShaderVersion.StartsWith("ps_2", StringComparison.Ordinal)))
					minDX = "9.109";
					//if Windows OS bugs 1134730 and 1134731 for 9L get fixed then replace minDX with 9c
					//minDX = "9.039f"; 

                pred = sourceRegs.IndexOf("p0", StringComparison.Ordinal) >= 0 || realReg.IndexOf("p0", StringComparison.Ordinal) >= 0;
				
				caseStr = GetCaseString( type, caseNum++ );
				cases += 
					GetCaseHeader( false, prefix, caseStr, ref options, "dest " + reg + " is NOT allowed", minDX, -1, -1, false, pred ) +
					FormatShaderLine( options.ShaderVersion, false ) +
					dcl +
					defs +
					tex +
					FormatShaderLine( options.Instruction + " " + realReg + sourceRegs, reg == "oC#" ) +
					( reg != "oC#" ? GetPSOutput( ref options ) : "" );

				foreach( string mod in info.DestMods )
				{
                    pred = sourceRegs.IndexOf("p0", StringComparison.Ordinal) >= 0 || realReg.IndexOf("p0", StringComparison.Ordinal) >= 0;

					caseStr = GetCaseString( type, caseNum++ );
					cases += 
						GetCaseHeader( false, prefix, caseStr, ref options, mod + " dest " + reg + " is NOT allowed", minDX, -1, -1, false, pred ) +
						FormatShaderLine( options.ShaderVersion, false ) +
						dcl +
						defs +
						tex +
						FormatShaderLine( options.Instruction + mod + " " + realReg + sourceRegs, reg == "oC#" ) +
						( reg != "oC#" ? GetPSOutput( ref options ) : "" );
				}
			}
			
			return cases;
		}

		//-------------------------------------------------------------------------------------------------------------------------

		private string GetWriteMaskCases( ref int caseNum, ShaderType type, ref GeneratorOptions options )
		{
			VersionInfo info = (VersionInfo) versionToInfo[ options.ShaderVersion ];
			string [ ] allMasks = GetWriteMasks( options.ShaderVersion );
			string [ ] notAllowed = GetMissingElements( allMasks, options.WriteMasksAllowed );
			string [ ] invalid = new string[ notAllowed.Length + invalidMasks.Length ];
			Array.Copy( notAllowed, invalid, notAllowed.Length );
			Array.Copy( invalidMasks, 0, invalid, notAllowed.Length, invalidMasks.Length );
			string prefix = type == ShaderType.Vertex ? "VS" : "PS";
			string minDX = "0.f";
			string cases = "";
			bool   pred;
			
			string sourceRegs = "";
			ArrayList sourceList = new ArrayList( );
			if( options.NumSourceRegs > 0 )
			{
				ArrayList combos = CreateRegCombos( options.SrcRegInfo );
				if( combos.Count == 0 )
					return "";
				ArrayList pass = new ArrayList( );
				ArrayList dummy = new ArrayList( );
				foreach( ArrayList combo in combos )
					GetComboScenarios( ref options, combo, pass, dummy );
				sourceList = (ArrayList) pass[ 0 ];
			}


			// special cases //////
			string tex = "";
            if (options.Instruction.StartsWith("texm3x2", StringComparison.Ordinal))
			{
				tex += FormatShaderLine( "texm3x2pad t1, t0", false );
			}
            else if (options.Instruction.StartsWith("texm3x3", StringComparison.Ordinal))
			{
				tex +=	
					FormatShaderLine( "texm3x3pad t1, t0", false ) +
					FormatShaderLine( "texm3x3pad t2, t0", false );
			}
			else if( options.Instruction == "texld" && options.ShaderVersion == "ps_1_4" )
			{
				sourceList[ 0 ] += ".xyz";
			}
			else if( Array.IndexOf( repInstructions, options.Instruction ) >= 0 )
			{
				for( int i = 0; i < sourceList.Count; i++ )
                    if (((string)sourceList[0]).IndexOf("vPos", StringComparison.Ordinal) < 0)
						sourceList[ i ] += ".x";
			}
            else if (options.Instruction == "cnd" && options.ShaderVersion.StartsWith("ps_1", StringComparison.Ordinal) && options.ShaderVersion != "ps_1_4")
			{
				sourceList[ 0 ] = "r0.a";
			}
			else if( options.Instruction == "sincos" )
			{
				if( ( (string) sourceList[ 0 ] ).IndexOf( "vPos", StringComparison.Ordinal ) < 0 )
					sourceList[ 0 ] += ".x";
			}
			//////////////////////
			
			for( int i = 0; i < sourceList.Count; i++ )
				sourceRegs += ", " + sourceList[ i ]; 
			string defs = GetConstantDefs( ref options, sourceList, options.DestRegsAllowed[ 0 ].Replace( "#", "0" ) );

			string caseStr;
			foreach( string mask in options.WriteMasksAllowed )
			{
				caseStr = GetCaseString( type, caseNum++ );
				string realReg = options.DestRegsAllowed[ 0 ].Replace( "#", "0" );
				// special cases
				if( Array.IndexOf( specialTexInst, options.Instruction ) >= 0 )
					realReg = "t3";
				if( options.Instruction.StartsWith( "texm3x2", StringComparison.Ordinal ) )
					realReg = "t2";
				string dcl = "";
				if( realReg == "o0" )
					dcl = FormatShaderLine( "dcl_" + dclUsageO[ new Random( ).Next( dclUsageO.Length ) ] + " " + realReg, false );
				else if( options.Instruction == "texdepth" )
				{
					realReg = "r5";
					tex = 
						FormatShaderLine( "mov r5, c0", false ) +
						FormatShaderLine( "phase", false );
				}
				else if( ( options.Instruction == "dp4" || options.Instruction == "cmp" ) && ( options.ShaderVersion == "ps_1_2" || options.ShaderVersion == "ps_1_3" ) )
					realReg = "r1";
				else if( options.Instruction == "texld" || options.Instruction == "sincos" )
					realReg = "r4";

				pred = sourceRegs.IndexOf( "p0", StringComparison.Ordinal ) >= 0 || realReg.IndexOf( "p0", StringComparison.Ordinal ) >= 0;
				
				cases += 
					GetCaseHeader( true, prefix, caseStr, ref options, "write mask ." + mask + " is allowed", minDX, -1, -1, false, pred ) +
					FormatShaderLine( options.ShaderVersion, false ) +
					dcl +
					defs +
					tex +
					FormatShaderLine(	options.Instruction + " " + realReg + "." + 
					mask + sourceRegs, realReg == "oC0" ) +
					( realReg != "oC0" ? GetPSOutput( ref options ) : "" );
			}
			
			foreach( string mask in invalid )
			{
				caseStr = GetCaseString( type, caseNum++ );
				string realReg = options.DestRegsAllowed[ 0 ].Replace( "#", "0" );
				// special cases
				if( Array.IndexOf( specialTexInst, options.Instruction ) >= 0 /* && options.Instruction != "texld" */ )
					realReg = "t3";
				if( options.Instruction.StartsWith( "texm3x2", StringComparison.Ordinal ) )
					realReg = "t2";
				string dcl = "";
				if( realReg == "o0" )
					dcl = FormatShaderLine( "dcl_" + dclUsageO[ new Random( ).Next( dclUsageO.Length ) ] + " " + realReg, false );
				else if( options.Instruction == "texdepth" )
				{
					realReg = "r5";
					tex = 
						FormatShaderLine( "mov r5, c0", false ) +
						FormatShaderLine( "phase", false );
				}
				else if( ( options.Instruction == "dp4" || options.Instruction == "cmp" ) && ( options.ShaderVersion == "ps_1_2" || options.ShaderVersion == "ps_1_3" ) )
					realReg = "r1";
				else if( options.Instruction == "texld" || options.Instruction == "sincos" )
					realReg = "r4";

				pred = sourceRegs.IndexOf( "p0", StringComparison.Ordinal ) >= 0 || realReg.IndexOf( "p0", StringComparison.Ordinal ) >= 0;
				
				cases += 
					GetCaseHeader( false, prefix, caseStr, ref options, "write mask ." + mask + " is NOT allowed", minDX, -1, -1, false, pred ) +
					FormatShaderLine( options.ShaderVersion, false ) +
					dcl +
					defs +
					tex +
					FormatShaderLine(	options.Instruction + " " + realReg + "." + 
					mask + sourceRegs, realReg == "oC0" ) +
					( realReg != "oC0" ? GetPSOutput( ref options ) : "" );
			}

			return cases;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private string GetFlowControlCases( ref int caseNum, ShaderType type, ref GeneratorOptions options )
		{
			VersionInfo info = (VersionInfo) versionToInfo[ options.ShaderVersion ];
			string cases = "";
			string caseStr = "";
			string destReg = options.DestRegsAllowed[ 0 ].Replace( "#", "0" );
			if( Array.IndexOf( options.WriteMasksAllowed, "xyzw" ) < 0 && Array.IndexOf( options.WriteMasksAllowed, "rgba" ) < 0 )
				destReg += "." + options.WriteMasksAllowed[ 0 ];
			
			ArrayList sourceList = new ArrayList( );
			string sourceRegs = "";
			if( options.NumSourceRegs > 0 )
			{
				ArrayList combos = CreateRegCombos( options.SrcRegInfo );
				if( combos.Count == 0 )
					return "";
				ArrayList pass = new ArrayList( );
				ArrayList dummy = new ArrayList( );
				foreach( ArrayList combo in combos )
					GetComboScenarios( ref options, combo, pass, dummy );
				sourceList = (ArrayList) pass[ 0 ];
			}

			string tex = "";
            if (options.Instruction.StartsWith("texm3x2", StringComparison.Ordinal))
			{
				tex += FormatShaderLine( "texm3x2pad t1, t0", false );
			}
            else if (options.Instruction.StartsWith("texm3x3", StringComparison.Ordinal))
			{
				tex +=	
					FormatShaderLine( "texm3x3pad t1, t0", false ) +
					FormatShaderLine( "texm3x3pad t2, t0", false );
			}
			else if( options.Instruction == "texld" && options.ShaderVersion == "ps_1_4" )
			{
				sourceList[ 0 ] += ".xyz";
				tex = FormatShaderLine( "texcrd r0.rgb, t0.xyz", false );
			}
			else if( Array.IndexOf( repInstructions, options.Instruction ) >= 0 )
			{
				for( int i = 0; i < sourceList.Count; i++ )
                    if (((string)sourceList[i]).IndexOf("vPos", StringComparison.Ordinal) < 0)
						sourceList[ i ] += ".x";
			}
            else if (options.Instruction == "cnd" && options.ShaderVersion.StartsWith("ps_1", StringComparison.Ordinal) && options.ShaderVersion != "ps_1_4")
			{
				sourceList[ 0 ] = "r0.a";
			}
			else if( options.Instruction == "sincos" || destReg == "oDepth" )
			{
                if (((string)sourceList[0]).IndexOf("vPos", StringComparison.Ordinal) < 0)
					sourceList[ 0 ] += ".x";
			}
			
			for( int i = 0; i < sourceList.Count; i++ )
				sourceRegs += ", " + sourceList[ i ];

            bool IsAllowedInDFC = options.IsAllowedInDFC || sourceRegs.IndexOf("r0", StringComparison.Ordinal) < 0;

			string instruction = FormatShaderLine( "	" + options.Instruction + " " + destReg + 
				sourceRegs, false );
			string prefix = type == ShaderType.Pixel ? "PS" : "VS";
			string minDX = "0.f";
			string dcl = "";
			string defs;
			bool   pred;
			int    DFCDepth;
			int    SFCDepth;

			ArrayList regs;
			if( options.DestRegsAllowed[ 0 ] == "o#" )
				dcl = FormatShaderLine( "dcl_" + dclUsageO[ new Random( ).Next( dclUsageO.Length ) ] + " " + destReg, false );
			
			ArrayList combined = new ArrayList( );
			if( options.StaticFlow )
			{
				foreach( string i in staticFlowControl )
				{
					int [] BlockCount = {1, 8, 16, 24, 25 };
					FlowInfo flowInfo = new FlowInfo(0, 0, 0, 0, 0, 1, true);
					string block = String.Copy( i );
					
					for( int BlockIndex = 0; BlockIndex < 5; BlockIndex++ )
					{
						flowInfo.ClearFlowInfo();
						regs = new ArrayList( );
						regs.AddRange( sourceList );
						flowInfo.BlockCount = BlockCount[BlockIndex];
                        if (block.IndexOf("loop", StringComparison.Ordinal) >= 0 || block.IndexOf("rep", StringComparison.Ordinal) >= 0)
							flowInfo.BlockCount = BlockIndex + 1;
						string close = GetFCInstInfo( ref options, ref block, regs, ref flowInfo );

						defs = GetConstantDefs( ref options, regs, destReg );

						string nestOpen = "";
						string nestClose = "";
						string tab = "";

						for( int j = 0; j < flowInfo.BlockCount; j++ )
						{
							nestOpen += FormatShaderLine( tab + block, false );
							nestClose = FormatShaderLine( tab + close, j == 0 ) + nestClose;
							tab += "	";
						}
						
						//check if dest or src registers are predicates or block is with predicates (ifp, breakp)
                        pred = sourceRegs.IndexOf("p0", StringComparison.Ordinal) >= 0 || destReg.IndexOf("p0", StringComparison.Ordinal) >= 0;
                        pred |= (block.IndexOf("if p0", StringComparison.Ordinal) >= 0 || block.IndexOf("if !p0", StringComparison.Ordinal) >= 0 || block.IndexOf("breakp", StringComparison.Ordinal) >= 0);

						//check what values to send as caps DynamicFlowControlDepth and StaticFlowControlDepth
						DFCDepth = flowInfo.DynamicNesting;
						SFCDepth = Math.Max(flowInfo.LoopNesting, flowInfo.CallNesting);

						//Dynamic flow nesting limits for vs_2x: D3DCAPS9.VS20Caps.DynamicFlowControlDepth (0 OR 24)
                        if (options.ShaderVersion.IndexOf("vs_2_x", StringComparison.Ordinal) >= 0 && flowInfo.DynamicNesting > 0)
							DFCDepth = 24;

						//Static flow nesting limits for ps_2x: if(D3DCAPS9.PS20Caps.StaticFlowControlDepth > 0) 24 else 0
                        if (options.ShaderVersion.IndexOf("ps_2_x", StringComparison.Ordinal) >= 0 && flowInfo.StaticNesting > 0)
							SFCDepth = Math.Max(1, SFCDepth);
	
						caseStr = GetCaseString( type, caseNum++ );
						cases +=	
							GetCaseHeader( flowInfo.Pass, prefix, caseStr, ref options, "is" + ( flowInfo.Pass ? "" : " NOT" ) + 
							" allowed in a " + flowInfo.BlockCount + " level " + block + " block", minDX, DFCDepth, SFCDepth, false, pred ) +
							FormatShaderLine( options.ShaderVersion, false ) +
							defs +
							nestOpen +
							FormatShaderLine( tab + options.Instruction + " " + destReg + sourceRegs, false ) +
							nestClose;
					}

					combined.Add( block );
				}
			}

			if( options.DynamicFlow )
			{
				foreach( string i in dynamicFlowControl )
				{
					int [] BlockCount = {1, 8, 16, 24, 25 };
					FlowInfo flowInfo = new FlowInfo(0, 0, 0, 0, 0, 1, true);
					flowInfo.BlockCount = 1;
					string block = String.Copy( i );
					
					for( int BlockIndex = 0; BlockIndex < 5; BlockIndex++ )
					{
						flowInfo.ClearFlowInfo();
						regs = new ArrayList( );
						regs.AddRange( sourceList );
						flowInfo.BlockCount = BlockCount[BlockIndex];
						string close = GetFCInstInfo( ref options, ref block, regs, ref flowInfo );

						defs = GetConstantDefs( ref options, regs, destReg );

						string nestOpen = "";
						string nestClose = "";
						string tab = "";

						for( int j = 0; j < flowInfo.BlockCount; j++ )
						{
							nestOpen += FormatShaderLine( tab + block, false );
							nestClose = FormatShaderLine( tab + close, j == 0 ) + nestClose;
							tab += "	";
						}
						
						//check if dest or src registers are predicates or block is with predicates (ifp, breakp)
                        pred = sourceRegs.IndexOf("p0", StringComparison.Ordinal) >= 0 || destReg.IndexOf("p0", StringComparison.Ordinal) >= 0;
                        pred |= (block.IndexOf("if p0", StringComparison.Ordinal) >= 0 || block.IndexOf("if !p0", StringComparison.Ordinal) >= 0 || block.IndexOf("breakp", StringComparison.Ordinal) >= 0);

						//check what values to send as caps DynamicFlowControlDepth and StaticFlowControlDepth
						DFCDepth = flowInfo.DynamicNesting;
						SFCDepth = Math.Max(flowInfo.LoopNesting, flowInfo.CallNesting);

						//Dynamic flow nesting limits: D3DCAPS9.VS20Caps.DynamicFlowControlDepth (0 OR 24)
                        if (options.ShaderVersion.IndexOf("vs_2_x", StringComparison.Ordinal) >= 0 && flowInfo.DynamicNesting > 0)
							DFCDepth = 24;

						//Static flow nesting limits: if(D3DCAPS9.PS20Caps.StaticFlowControlDepth > 0) 24 else 0
                        if (options.ShaderVersion.IndexOf("ps_2_x", StringComparison.Ordinal) >= 0 && flowInfo.StaticNesting > 0)
							SFCDepth = Math.Max(1, SFCDepth);
							
						caseStr = GetCaseString( type, caseNum++ );
						cases +=	
							GetCaseHeader( flowInfo.Pass && IsAllowedInDFC, prefix, caseStr, ref options, "is" + ( flowInfo.Pass && IsAllowedInDFC ? "" : " NOT" ) + 
							" allowed in a " + flowInfo.BlockCount + " level " + block + " block", minDX, DFCDepth, SFCDepth, false, pred ) +
							FormatShaderLine( options.ShaderVersion, false ) +
							defs +
							nestOpen +
							FormatShaderLine( tab + options.Instruction + " " + destReg + sourceRegs, false ) +
							nestClose;
					}
					combined.Add( block );
				}
			}
			
			if (options.DynamicFlow || options.StaticFlow)
			{
				instruction = FormatShaderLine( "		" + options.Instruction + " " + destReg + 
					sourceRegs, false );
				foreach( string i in combined )
				{
					foreach( string j in combined )
					{
						FlowInfo flowInfo = new FlowInfo(0, 0, 0, 0, 0, 1, true);
						flowInfo.BlockCount = 1;
						string outer = String.Copy( i );
						string inner = String.Copy( j );
						regs = new ArrayList( );
						regs.AddRange( sourceList );
						string outerClose = GetFCInstInfo( ref options, ref outer, regs, ref flowInfo );
						string innerClose = GetFCInstInfo( ref options, ref inner, regs, ref flowInfo );

						if( innerClose == "" || outerClose == "" )
							continue;

						defs = GetConstantDefs( ref options, regs, destReg );
						
						// bug #84084
						minDX = "0.f";
						bool staticNest = ( Array.IndexOf( staticFlowControl, inner ) >= 0 && ( Array.IndexOf( staticFlowControl, outer ) >= 0 ) );
                        if (!staticNest && options.ShaderVersion.StartsWith("ps", StringComparison.Ordinal) && options.Instruction.StartsWith("texld", StringComparison.Ordinal) &&
                            options.Instruction != "texldd" && outer.IndexOf("break", StringComparison.Ordinal) >= 0)
							minDX = "9.109";
							//if Windows OS bug 1134727 for 9L gets fixed then replace minDX with 9c
							//minDX = "9.039f";

						//check if dest or src registers are predicates or block is with predicates (ifp, breakp)
                        pred = sourceRegs.IndexOf("p0", StringComparison.Ordinal) >= 0 || destReg.IndexOf("p0", StringComparison.Ordinal) >= 0;
                        pred |= (inner.IndexOf("if p0", StringComparison.Ordinal) >= 0 || inner.IndexOf("if !p0", StringComparison.Ordinal) >= 0 || inner.IndexOf("breakp", StringComparison.Ordinal) >= 0);
                        pred |= (outer.IndexOf("if p0", StringComparison.Ordinal) >= 0 || outer.IndexOf("if !p0", StringComparison.Ordinal) >= 0 || outer.IndexOf("breakp", StringComparison.Ordinal) >= 0);

						//check what values to send as caps DynamicFlowControlDepth and StaticFlowControlDepth
						DFCDepth = flowInfo.DynamicNesting;
						SFCDepth = Math.Max(flowInfo.LoopNesting, flowInfo.CallNesting);

						//Dynamic flow nesting limits: D3DCAPS9.VS20Caps.DynamicFlowControlDepth (0 OR 24)
                        if (options.ShaderVersion.IndexOf("vs_2_x", StringComparison.Ordinal) >= 0 && flowInfo.DynamicNesting > 0)
							DFCDepth = 24;

						//Static flow nesting limits: if(D3DCAPS9.PS20Caps.StaticFlowControlDepth > 0) 24 else 0
                        if (options.ShaderVersion.IndexOf("ps_2_x", StringComparison.Ordinal) >= 0 && flowInfo.StaticNesting > 0)
							SFCDepth = Math.Max(1, SFCDepth);
												
						caseStr = GetCaseString( type, caseNum++ );
						cases +=	
							GetCaseHeader( (IsAllowedInDFC || staticNest) &&  flowInfo.Pass, prefix, caseStr, ref options, 
							"is" + ( (IsAllowedInDFC || staticNest) &&  flowInfo.Pass ? "" : " NOT" ) + 
							" allowed in a " + outer + " and " + inner + " block", minDX, DFCDepth, SFCDepth, false, pred ) +
							FormatShaderLine( options.ShaderVersion, false ) +
							defs +
							FormatShaderLine( outer, false ) +
							FormatShaderLine( "	" + inner, false ) +
							instruction +
							FormatShaderLine( "	" + innerClose, false ) +
							FormatShaderLine( outerClose, true );
					}
				}
			}

			return cases;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		// determines what regs need to be declared and what the closing instruction should be
		//param flowInfo returns the info how this instruction influence flow counts, such as DFNesting, SFNesting, SFCount, LoopNesting and CallNesting
		private string GetFCInstInfo( ref GeneratorOptions options, ref string block, ArrayList regs, ref FlowInfo flowInfo )
		{
			string close = "";
			Random rnd = new Random( );

            if (block.StartsWith("if", StringComparison.Ordinal))
			{
                if (block.StartsWith("if_gt", StringComparison.Ordinal))
				{
					block = block.Replace( "gt", compMods[ rnd.Next( compMods.Length ) ] );
					flowInfo.DynamicNesting += flowInfo.BlockCount;
				}
                else if (block.IndexOf("b0", StringComparison.Ordinal) >= 0)
				{
                    regs.Add("b0");
                    if (block.IndexOf("else", StringComparison.Ordinal) >= 0)
					{
						//flowInfo.BlockCount = (flowInfo.BlockCount + (flowInfo.BlockCount&0x1))/2;
						flowInfo.StaticCount += flowInfo.BlockCount;
					}
					flowInfo.StaticNesting += flowInfo.BlockCount;
					flowInfo.StaticCount += flowInfo.BlockCount;
				}
				else
				{
					regs.Add( "p0" );
					flowInfo.DynamicNesting += flowInfo.BlockCount;
				}
				close = "endif";
			}
            else if (block.StartsWith("loop", StringComparison.Ordinal))
			{
				flowInfo.StaticCount += flowInfo.BlockCount;
				flowInfo.LoopNesting += flowInfo.BlockCount;

                if (block.IndexOf("break", StringComparison.Ordinal) >= 0)
				{
                    if (block.IndexOf("breakp", StringComparison.Ordinal) >= 0)
						regs.Add( "p0" );
                    else if (block.IndexOf("break_gt", StringComparison.Ordinal) >= 0)
					{
						block = block.Replace( "gt", compMods[ rnd.Next( compMods.Length ) ] ); 
						flowInfo.DynamicNesting += flowInfo.BlockCount;
					}
					else
						flowInfo.DynamicNesting += flowInfo.BlockCount;
				}
				
				close = "endloop";
				regs.Add( "i0" );

                if (options.ShaderVersion.StartsWith("ps_2", StringComparison.Ordinal))
					flowInfo.Pass = false;
			}
			else
			{
				flowInfo.StaticCount += flowInfo.BlockCount;
				flowInfo.LoopNesting += flowInfo.BlockCount;

                if (block.IndexOf("break", StringComparison.Ordinal) >= 0)
				{
                    if (block.IndexOf("breakp", StringComparison.Ordinal) >= 0)
						regs.Add( "p0" );
                    else if (block.IndexOf("break_gt", StringComparison.Ordinal) >= 0)
					{
						block = block.Replace( "gt", compMods[ rnd.Next( compMods.Length ) ] ); 
						flowInfo.DynamicNesting += flowInfo.BlockCount;
					}
					else
						flowInfo.DynamicNesting += flowInfo.BlockCount;
				}
				close = "endrep";
				regs.Add( "i0" );
			}

            if (options.ShaderVersion.IndexOf("vs_2_0", StringComparison.Ordinal) >= 0)
			{
				if ( flowInfo.LoopNesting > 1 || flowInfo.CallNesting > 1 ||
					flowInfo.StaticNesting > 16 || flowInfo.StaticCount > 16 ||
					flowInfo.DynamicNesting > 0 )
					flowInfo.Pass = false;
			}
            if (options.ShaderVersion.IndexOf("ps_2_0", StringComparison.Ordinal) >= 0)
			{
				if ( flowInfo.LoopNesting > 0 || flowInfo.CallNesting > 0 ||
					flowInfo.StaticNesting > 0 || flowInfo.StaticCount > 0 ||
					flowInfo.DynamicNesting > 0 )
					flowInfo.Pass = false;
			}
            else if (options.ShaderVersion.IndexOf("vs_2_x", StringComparison.Ordinal) >= 0)
			{
				if ( flowInfo.LoopNesting > 4 || flowInfo.CallNesting > 4 ||
					flowInfo.StaticNesting > 16 || flowInfo.StaticCount > 16 ||
					flowInfo.DynamicNesting > 24 )
					flowInfo.Pass = false;
			}
			else
			{
				if ( flowInfo.LoopNesting > 4 || flowInfo.CallNesting > 4 ||
					flowInfo.StaticNesting > 24 || flowInfo.DynamicNesting > 24 )
					flowInfo.Pass = false;
			}

			return close;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private string GetCaseHeader( bool pass, string prefix, string caseStr, ref GeneratorOptions options, string description, 
			string minDX, int DFCDepth, int SFCDepth, bool arbSwiz, bool pred )
		{
			VersionInfo info = (VersionInfo) versionToInfo[ options.ShaderVersion ];
			string caps = "";
			
			ArrayList capsList = new ArrayList( );

            if (options.ShaderVersion.IndexOf("2_x", StringComparison.Ordinal) >= 0)
			{
				//if shader uses predicates then we want to check for CAPS_PREDICATION cap
				if( pred )
					capsList.Add( "CAPS_PREDICATION" );
			
				if( prefix == "PS" )
				{
					//if shader uses arbitrary swizzles then we want to check for CAPS_ARBITRARYSWIZZLE cap
					if( arbSwiz )
						capsList.Add( "CAPS_ARBITRARYSWIZZLE" );
				
					//if shader uses gradient instructions then we want to check for CAPS_GRADIENTINSTRUCTIONS cap
					if( options.Instruction == "dsx" || options.Instruction == "dsy" || options.Instruction == "texldd" )
						capsList.Add( "CAPS_GRADIENTINSTRUCTIONS" );
				}
			}
			else
			{
				DFCDepth = -1;
				SFCDepth  = -1;
			}

			foreach( string cap in capsList )
				caps += cap + " + ";
			if( caps != "" )
				caps = caps.Substring( 0, caps.Length - 3 );

			if( info.SoftwareRequired )
				caps = "";

			return	prefix + "_CRITERIA " + caseStr + "_Criteria = " + 
				GetCriteriaStruct( pass, info.SoftwareRequired, info.VersionCode, DFCDepth, SFCDepth, caps, minDX ) + ";\r\n" +
				"string " + caseStr + "_Desc = \"" + options.ShaderVersion + " : " + options.Instruction +
				" " + description + "\";\r\n" +
				"string " + caseStr + " = \r\n";
		}

		private string FormatShaderLine( string line, bool last )
		{
			return "	\"" + line + ( last ? " \";\r\n\r\n" : " \"\r\n" );
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private string GenerateSourceCase( bool pass, ArrayList regs, string prefix, ref GeneratorOptions options, ShaderType type, int caseNum )
		{
			string oneCase = "";
			string sourceRegs = "";
			foreach( string reg in regs )
				sourceRegs += ", " + reg;
			bool arbSwiz = false;
			foreach( string swiz in arbSwizzles )
			{
				foreach( string reg in regs )
				{
                    if (reg.EndsWith("." + PosToColor(swiz), StringComparison.Ordinal))
					{
						arbSwiz = true;
						break;
					}
				}
			}
			
			string caseStr = GetCaseString( type, caseNum );
			string destReg = options.DestRegsAllowed[ 0 ].Replace( "#", "0" );
			if( options.Instruction == "sincos" )
				destReg = "r4";

			if( Array.IndexOf( options.WriteMasksAllowed, "xyzw" ) < 0 && Array.IndexOf( options.WriteMasksAllowed, "rgba" ) < 0 )
				destReg += "." + options.WriteMasksAllowed[ 0 ];

            if (destReg == "t0" || options.Instruction.StartsWith("texm3x3", StringComparison.Ordinal))
				destReg = "t3";
            if (options.Instruction.StartsWith("texm3x2", StringComparison.Ordinal))
				destReg = "t2";

			string minDX = "0.f";
			// bug #84807 - t# read limits broken
            if (options.ShaderVersion.StartsWith("ps_2", StringComparison.Ordinal) && regs.IndexOf("t0") != regs.LastIndexOf("t0"))
				minDX = "9.109";
				//if Windows OS bug 1134733 for 9L gets fixed then replace minDX with 9c
				//minDX = "9.039f";

			string defs = GetConstantDefs( ref options, regs, destReg );
			string output = GetPSOutput( ref options );
            if (options.ShaderVersion.StartsWith("ps_1", StringComparison.Ordinal) && defs.IndexOf("mov r0, c0", StringComparison.Ordinal) >= 0) 
				output = "";
			string tex = "";
            if (options.Instruction.StartsWith("texm3x2", StringComparison.Ordinal))
			{
				tex += FormatShaderLine( "texm3x2pad t1, t0", false );
			}
            else if (options.Instruction.StartsWith("texm3x3", StringComparison.Ordinal))
			{
				tex +=	
					FormatShaderLine( "texm3x3pad t1, t0", false ) +
					FormatShaderLine( "texm3x3pad t2, t0", false );
			}
			else if( ( options.Instruction == "dp4" || options.Instruction == "cmp" ) && ( options.ShaderVersion == "ps_1_2" || options.ShaderVersion == "ps_1_3" ) )
			{
				foreach( string reg in regs )
				{
                    if (reg.IndexOf(destReg, StringComparison.Ordinal) >= 0)
					{
						pass = false;
						break;
					}
				}
			}
			else if( options.Instruction == "texld" && options.ShaderVersion == "ps_1_4" )
			{
				tex = FormatShaderLine( "texcrd r4.rgb, t0", false );
				defs = "";
			}

			oneCase +=	GetCaseHeader( pass, prefix, caseStr, ref options, "source reg combination " + sourceRegs.Substring( 2 ) +
                " is " + (pass ? "" : "NOT ") + "allowed", minDX, -1, -1, arbSwiz, sourceRegs.IndexOf("p0", StringComparison.Ordinal) >= 0 || destReg.IndexOf("p0", StringComparison.Ordinal) >= 0) +
				FormatShaderLine( options.ShaderVersion, false ) +
				defs +
				tex +
				FormatShaderLine( options.Instruction + " " + destReg + sourceRegs, output == "" ) +
				output;
	
			return oneCase;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private string GetPSOutput( ref GeneratorOptions options )
		{
			VersionInfo info = (VersionInfo) versionToInfo[ options.ShaderVersion ];
			string output = "";
            if (options.ShaderVersion.StartsWith("ps_1", StringComparison.Ordinal))
				output = FormatShaderLine( "mov r0, c0", true );
            else if (options.ShaderVersion.StartsWith("ps", StringComparison.Ordinal))
				output = FormatShaderLine( "mov oC0, c0", true );
            else if (options.ShaderVersion == "vs_1_1" || options.ShaderVersion.StartsWith("vs_2", StringComparison.Ordinal))
				output = FormatShaderLine( "mov oPos, c0", true );
			else
				output = FormatShaderLine( "", true );

			return output;			
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private string GetConstantDefs( ref GeneratorOptions options, ArrayList regList, string destReg )
		{
			VersionInfo info = (VersionInfo) versionToInfo[ options.ShaderVersion ];
			string defs = "";
			string movs = "";
			ArrayList regs = (ArrayList) regList.Clone( );
			
			foreach( string source in regs )
			{
				if( source == "oDepth" || source == "vFace" )
					continue;

                if (source.IndexOf("vPos", StringComparison.Ordinal) >= 0 && defs.IndexOf("vPos", StringComparison.Ordinal) < 0)
				{
					defs += FormatShaderLine( "dcl vPos.xy", false );
					continue;
				}

				Random rnd = new Random( );

                int mod = source.IndexOf("_", StringComparison.Ordinal);
				if( mod < 0 )
                    mod = source.IndexOf(".", StringComparison.Ordinal);
				string reg = mod < 0 ? source.Substring( source.Length - 2, 2 ) : source.Substring( mod - 2, 2 );

				switch( reg[ 0 ] )
				{
					case 'c':
                        if (defs.IndexOf(reg, StringComparison.Ordinal) < 0)
							defs = FormatShaderLine( "def " + reg + ", 1, 1, 1, 1", false ) + defs;
						break;
					case 'b':
                        if (defs.IndexOf(reg, StringComparison.Ordinal) < 0)
							defs = FormatShaderLine( "defb " + reg + ", true", false ) + defs;
						break;
					case 'i':
                        if (defs.IndexOf(reg, StringComparison.Ordinal) < 0)
							defs = FormatShaderLine( "defi " + reg + ", 0, 0, 0, 0", false ) + defs;
						break;
					case 'r':
                        if (movs.IndexOf(reg, StringComparison.Ordinal) < 0)
							movs += FormatShaderLine( "mov " + reg + ", c0", false );
						break;
					case 't':
                        if (defs.IndexOf(reg, StringComparison.Ordinal) < 0)
						{
                            if (options.ShaderVersion == "ps_1_4" && !destReg.StartsWith(reg, StringComparison.Ordinal))
							{
								if( options.Instruction == "texld" )
									defs += FormatShaderLine( "texcrd r0.rgb, " + reg, false );
								else
									defs += FormatShaderLine( "texcrd r0, " + reg, false );
							}
                            else if (options.ShaderVersion.IndexOf("1", StringComparison.Ordinal) >= 0)
								defs += FormatShaderLine( "texcoord " + reg, false );
                            else if (!options.ShaderVersion.StartsWith("ps_1", StringComparison.Ordinal))
								defs += FormatShaderLine( "dcl " + reg, false );
						}
						break;
					case 'v':
                        if (defs.IndexOf(reg, StringComparison.Ordinal) < 0 && !options.ShaderVersion.StartsWith("ps_1", StringComparison.Ordinal))
						{
                            if (options.ShaderVersion.StartsWith("ps_2", StringComparison.Ordinal))
								defs += FormatShaderLine( "dcl " + reg, false );
                            else if (options.ShaderVersion.StartsWith("vs", StringComparison.Ordinal))
							{
								string usage = dclUsageO[ rnd.Next( dclUsageV.Length ) ];
                                while (defs.IndexOf(usage, StringComparison.Ordinal) >= 0)
									usage = dclUsageO[ rnd.Next( dclUsageV.Length ) ];
								defs += FormatShaderLine( "dcl_" + usage + " " + reg, false );
							}
							else
							{	
								string usage = dclUsageO[ rnd.Next( dclUsageO.Length ) ];
                                while (defs.IndexOf(usage, StringComparison.Ordinal) >= 0)
									usage = dclUsageO[ rnd.Next( dclUsageO.Length ) ];
								defs += FormatShaderLine( "dcl_" + usage + " " + reg, false );
							}
						}
						break;
					case 's':
                        if (defs.IndexOf(reg, StringComparison.Ordinal) < 0)
							defs += FormatShaderLine( "dcl_" + dclUsageS[ rnd.Next( dclUsageS.Length ) ] + " " + reg, false );
						break;
					case 'a':
                        if (movs.IndexOf(reg, StringComparison.Ordinal) < 0)
						{
							if( options.ShaderVersion == "vs_1_1" )
								movs += FormatShaderLine( "mov " + reg + ".x, c0.x", false );
                            else if (options.ShaderVersion.StartsWith("vs", StringComparison.Ordinal))
								movs += FormatShaderLine( "mova " + reg + ".x, c0.x", false );
						}
						break;
				}
			}

			if( options.Instruction == "texkill" )
			{
                if (!options.ShaderVersion.StartsWith("ps_1", StringComparison.Ordinal))
					movs += FormatShaderLine( "mov r0, c0", false );
                if (options.ShaderVersion.StartsWith("ps_2", StringComparison.Ordinal))
					defs += FormatShaderLine( "dcl t0", false );
			}

			return defs + movs;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private string [ ] GetMissingElements( string [ ] original, string [ ] subset )
		{
			int size = 0;
			string [ ] missingTemp = new string[ original.Length ];
			foreach( string str in original )
				if( Array.IndexOf( subset, str ) < 0 )
					missingTemp[ size++ ] = str;
			string [ ] missing = new string[ size ];	
			Array.Copy( missingTemp, 0, missing, 0, size );
			return missing;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private string GetCriteriaStruct( bool pass, bool software, string version, int DFCDepth, int SFCDepth, string caps, string minDX )
		{
			return	"{ " + (pass ? "true" : "false") + ", " + (software ? "true" : "false") +
				", " + version + ", " + DFCDepth + ", -1, " + SFCDepth + ", -1, " + (caps == "" ? "0" : caps) + ", -1, " + minDX + " }";
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private string GetCaseString( ShaderType type, int caseNum )
		{
			string ret;
			if( type == ShaderType.Vertex )
				ret = "VS_";
			else
				ret = "PS_";
			if( caseNum < 10 )
				ret += "00" + caseNum;
			else if( caseNum < 100 )
				ret += "0" + caseNum;
			else
				ret += caseNum;
			return ret;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private ArrayList CreateRegCombos( SrcRegOptions [ ] regOptions )
		{
			if( regOptions == null || regOptions.Length < 1 )
				return null;
			ArrayList final = new ArrayList( );
			
			foreach( string reg in regOptions[ 0 ].SrcRegsAllowed )
			{
				ArrayList current = new ArrayList( );
				current.Add( reg );
				CreateRegCombosRec( regOptions, 1, current, final ); 
			}
			return final;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private void CreateRegCombosRec( SrcRegOptions [ ] regOptions, int regNum, ArrayList current, ArrayList final )
		{
			if( regNum >= regOptions.Length )
			{
				final.Add( current );
				return;
			}

			foreach( string reg in regOptions[ regNum ].SrcRegsAllowed )
			{
				ArrayList next = (ArrayList) current.Clone( );
				next.Add( reg );
				CreateRegCombosRec( regOptions, regNum + 1, next, final );
			}
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private void GetComboScenarios( ref GeneratorOptions options, ArrayList combo, ArrayList pass, ArrayList fail )
		{
			VersionInfo info = (VersionInfo) versionToInfo[ options.ShaderVersion ];
			Hashtable indices = new Hashtable( );
			Hashtable count = new Hashtable( );
			ArrayList temp = null;

            if (options.ShaderVersion.StartsWith("ps_2", StringComparison.Ordinal) && combo.Contains("v#") && combo.Contains("t#") && !info.SoftwareRequired)
			{
				temp = (ArrayList) combo.Clone( );
				for( int i = 0; i < temp.Count; i++ )
					temp[ i ] = ( (string) temp[ i ] ).Replace( "#", "0" );
				fail.Add( temp );
				return;
			}

			for( int i = 0; i < combo.Count; i++ )
			{
				if( !indices.ContainsKey( combo[ i ] ) )
					indices[ combo[ i ] ] = Array.IndexOf( info.Registers, combo[ i ] );
				int regNum = 1;
				if( count[ combo[ i ] ] == null )
				{
					count[ combo[ i ] ] = 1;
				}
				else
				{
					regNum = (int) count[ combo[ i ] ];
					regNum++;
					count[ combo[ i ] ] = regNum;
				}
			}
			
			bool singleReads = true;
			foreach( string reg in indices.Keys )
			{
				int regCount = (int) count[ reg ];
				if( regCount > 1 )
				{
					singleReads = false;
					int readLimit = info.ReadLimits[ (int) indices[ reg ] ];
                    if (reg == "t#" && options.ShaderVersion.IndexOf("1", StringComparison.Ordinal) >= 0 && options.Instruction.StartsWith("tex", StringComparison.Ordinal))
						readLimit = 1;
					if( readLimit != 0 && regCount > readLimit )
					{
						temp = (ArrayList) combo.Clone( );
						for( int i = 0; i < temp.Count; i++ )
							temp[ i ] = ( (string) temp[ i ] ).Replace( "#", "0" );
						if( info.SoftwareRequired && !( options.Instruction == "sincos" && reg == "c#" ) )
							pass.Add( temp );
						else
							fail.Add( temp );
					}

					int index = 0;
					temp = (ArrayList) combo.Clone( );
					for( int i = 0; i < temp.Count; i++ )
					{
						string tempReg = (string) temp[ i ];
						if( tempReg == reg )
							temp[ i ] = tempReg.Replace( "#", "" + index++ );
						else
							temp[ i ] = tempReg.Replace( "#", "0" );
					}

                    if (options.ShaderVersion.StartsWith("ps_2", StringComparison.Ordinal) && reg == "v#" && regCount > 2)
					{
						fail.Add( temp );
						continue;
					}

					int readPorts = info.ReadPorts[ (int) indices[ reg ] ];
					if( ( regCount <= readPorts || info.SoftwareRequired ||
						( options.Instruction == "sincos" && reg == "c#" ) ) )
						pass.Add( temp );
					else
						fail.Add( temp );
					
				}	
			}

			if( singleReads )
			{
				temp = (ArrayList) combo.Clone( );
				for( int i = 0; i < temp.Count; i++ )
					temp[ i ] = ( (string) temp[ i ] ).Replace( "#", "0" );
				pass.Add( temp );
			}

			for( int i = 0; i < pass.Count; i++ )
			{	
				ArrayList onePass = (ArrayList) pass[ i ];
				for( int j = 0; j < onePass.Count; j++ )
					if( (string) onePass[ j ] == "vPos" )
					{
						if( Array.IndexOf( repInstructions, options.Instruction ) >= 0 )
							onePass[ j ] = "vPos.x";
						else
							onePass[ j ] = "vPos.xy";
					}
			}
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private void GetSourceModScenarios( ref GeneratorOptions options, ShaderType type, ArrayList pass, ArrayList fail )
		{
			if( pass.Count == 0 || options.NumSourceRegs == 0 )
				return;

			VersionInfo info = (VersionInfo) versionToInfo[ options.ShaderVersion ];
			string [ ] modsAllowed = info.SourceMods;
			bool useColor = type == ShaderType.Pixel;
			
			ArrayList sourceMods = new ArrayList( );
			ArrayList temp = null;

			ArrayList regList = (ArrayList) ( (ArrayList) pass[ new Random( ).Next( pass.Count ) ] ).Clone( );

			// remove unmodified source regs for instructions that require replicate swizzle
			if( Array.IndexOf( repInstructions, options.Instruction ) >= 0 || options.Instruction == "sincos" )
				pass.Clear( );

			Random rnd = new Random( );
			ArrayList regLists = new ArrayList( );
			int attempts = 200;
			for( int i = 0; i < 5; i++ )
			{
                while (pass.Count > 0 && options.ShaderVersion.StartsWith("ps_3", StringComparison.Ordinal) && (regList.Contains("vPos.x") || regList.Contains("vPos.xy")) && attempts-- > 0)
					regList = (ArrayList) ( (ArrayList) pass[ rnd.Next( pass.Count ) ] ).Clone( );
				if( ( pass.Count == 0 && ( regList.Contains( "vPos.x" ) || regList.Contains( "vPos.xy" ) ) || attempts <= 0 ) )
					return;
				regLists.Add( regList );
			}

			bool cnd = false;
			// lrp only allows 1-reg source mod in PS_1_X
            if (options.Instruction == "lrp" && options.ShaderVersion.IndexOf("1", StringComparison.Ordinal) >= 0)
				modsAllowed = new string [ ] { "1-" };
				// tex* instructions allow _bx2 in PS_1_X
            else if (options.Instruction.StartsWith("tex", StringComparison.Ordinal) && options.ShaderVersion.StartsWith("ps_1", StringComparison.Ordinal))
				modsAllowed = new string [ ] { "_bx2" };
				// cnd requires r0.a as src0 in ps_1_1, ps_1_2, ps1_3
            else if (options.Instruction == "cnd" && options.ShaderVersion.StartsWith("ps_1", StringComparison.Ordinal) && options.ShaderVersion != "ps_1_4")
			{
				for( int i = 0; i < pass.Count; i++ )
					( (ArrayList) pass[ i ] )[ 0 ] = "r0.a";
				for( int i = 0; i < 5; i++ )
					( (ArrayList) regLists[ i ] )[ 0 ] = "r0.a";
				cnd = true;
			}
			
			if( Array.IndexOf( repInstructions, options.Instruction ) < 0 && options.Instruction != "sincos" )
			{
				if( options.SrcRegInfo[ 0 ].SourceMod )
				{
					foreach( string mod in modsAllowed )
					{
                        if ((mod.StartsWith("_d", StringComparison.Ordinal) && (options.Instruction != "texld" || options.Instruction != "texcrd")))
							continue;
						temp = new ArrayList( );
						temp.Add( mod );
						GetSourceModScenariosRec( ref options, modsAllowed, 1, temp, sourceMods, (ArrayList) regLists[ rnd.Next( 5 ) ] );
					}
				}
				else 
				{
					temp = new ArrayList( );
					temp.Add( "" );
					GetSourceModScenariosRec( ref options, modsAllowed, 1, temp, sourceMods, (ArrayList) regLists[ rnd.Next( 5 ) ] );
				}
			}

			ArrayList notAllowed = new ArrayList( );
			for( int i = 0; i < options.SrcRegInfo.Length; i++ )
			{
				temp = new ArrayList( );
				for( int j = 0; j < options.NumSourceRegs; j++ )
					temp.Add( "" );
	
				if( !options.SrcRegInfo[ i ].SourceMod )
				{
					foreach( string mod in info.SourceMods )
					{
                        if (mod.StartsWith("_d", StringComparison.Ordinal) && (options.Instruction != "texld" || options.Instruction != "texcrd") || mod == "")
							continue;
						temp[ i ] = mod;
						notAllowed.Add( temp.Clone( ) );
					}
				}

				if( Array.IndexOf( repInstructions, options.Instruction ) >= 0 && options.NumSourceRegs > 1 )
					continue;

				// get swizzle cases...
				foreach( string swizzle in repSwizzles )
				{
					if( cnd )
						break;
					temp[ i ] = "." + ( useColor ? PosToColor( swizzle ) : swizzle );
					if( ( options.SrcRegInfo[ i ].ReplicateSwizzles ||
                        (swizzle == "w" && type == ShaderType.Pixel && options.ShaderVersion.StartsWith("ps_1", StringComparison.Ordinal) && !options.Instruction.StartsWith("tex", StringComparison.Ordinal))))
						sourceMods.Add( temp.Clone( ) );
					else
						notAllowed.Add( temp.Clone( ) );
				}

				foreach( string swizzle in genSwizzles )
				{
					temp[ i ] = "." + ( useColor ? PosToColor( swizzle ) : swizzle );
					if( options.SrcRegInfo[ i ].GeneralSwizzles )
						sourceMods.Add( temp.Clone( ) );
					else
						notAllowed.Add( temp.Clone( ) );
				}

				foreach( string swizzle in arbSwizzles )
				{
					temp[ i ] = "." + ( useColor ? PosToColor( swizzle ) : swizzle );
					if( options.SrcRegInfo[ i ].ArbitrarySwizzles )
						sourceMods.Add( temp.Clone( ) );
					else
						notAllowed.Add( temp.Clone( ) );
				}
			}
			/*
						int idx = regList.IndexOf( "c0" );
						if( idx >= 0 && type == ShaderType.Vertex && Array.IndexOf( repInstructions, options.Instruction ) < 0 && options.Instruction != "sincos" )
						{
							temp = new ArrayList( );
							for( int i = 0; i < options.NumSourceRegs; i++ )
								temp.Add( "" );
							if( options.ShaderVersion.IndexOf( "1" ) >= 0 )
							{
								temp[ idx ] = "[ a0.x ]";
								sourceMods.Add( temp.Clone( ) );
								temp[ idx ] = "[ a0.y ]";
								notAllowed.Add( temp.Clone( ) );
							}
							else
							{
								temp[ idx ] = "[ a0.x ]";
								sourceMods.Add( temp.Clone( ) );
							}
						}
			*/			
			CombineMods( pass, (ArrayList) regLists[ rnd.Next( 5 ) ], sourceMods, options.NumSourceRegs, ref options );
			CombineMods( fail, (ArrayList) regLists[ rnd.Next( 5 ) ], notAllowed, options.NumSourceRegs, ref options );
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private void GetSourceModScenariosRec( ref GeneratorOptions options, string [ ] modsAllowed, int regNum, ArrayList current, ArrayList final, ArrayList regList )
		{
			if( regNum >= options.SrcRegInfo.Length )
			{
				final.Add( current );
				return;
			}

			ArrayList temp = null;
            if (options.SrcRegInfo[regNum].SourceMod && !(((string)regList[regNum]).StartsWith("c", StringComparison.Ordinal) && options.ShaderVersion.StartsWith("ps_1", StringComparison.Ordinal)))
			{
				foreach( string mod in modsAllowed )
				{
                    if (mod.StartsWith("_d", StringComparison.Ordinal) && (options.Instruction != "texld" || options.Instruction != "texcrd"))
						continue;
					temp = (ArrayList) current.Clone( );
					temp.Add( mod );
					GetSourceModScenariosRec( ref options, modsAllowed, regNum + 1, temp, final, regList );
				}
			}
			else
			{
				temp = (ArrayList) current.Clone( );
				temp.Add( "" );
				GetSourceModScenariosRec( ref options, modsAllowed, regNum + 1, temp, final, regList );
			}

		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private void CombineMods( ArrayList final, ArrayList regList, ArrayList mods, int numRegs, ref GeneratorOptions options )
		{
            if (((string)regList[0]).StartsWith("c", StringComparison.Ordinal) && options.ShaderVersion.StartsWith("ps_1", StringComparison.Ordinal))
				return;

			ArrayList temp;
			foreach( ArrayList modList in mods )
			{
				temp = (ArrayList) regList.Clone( );
				for( int i = 0; i < numRegs; i++ )
				{
					string reg = (string) temp[ i ];
					string mod = (string) modList[ i ];
					
					if( mod == "-" )
						temp[ i ] = mod + reg;
					else if( mod == "1-" && reg == "r0.a" )
						temp[ i ] = "1 - r0.a";
					else if( mod == "1-" )
						temp[ i ] = "1 - " + reg;
					else if( mod.Length > 1 && mod[ 0 ] == '-' )
						temp[ i ] = mod[ 0 ].ToString( ) + reg + mod.Substring( 1 );
					else
						temp[ i ] = reg + mod;
				}
				final.Add( temp );
			}
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		public void SaveOptions( GeneratorOptions options, string filename )
		{
			if( options.Instruction == "texldd" )
				RevertOptionsTexldd( ref options );
			
			StreamWriter file = null;
			try
			{
				file = File.AppendText( filename );
			}
			catch( Exception e )
			{
				Console.Error.WriteLine( e.StackTrace );
			}
			
			string genData =	
				options.ShaderVersion + "," +
				options.Instruction + "," +
				options.StaticFlow + "," +
				options.DynamicFlow + "," +
				options.IsAllowedInDFC + "," +
				options.NumSourceRegs + "," +
				options.DestRegsAllowed.Length + "," + 
				GetArrayStr( options.DestRegsAllowed ) +
				options.DestModsAllowed.Length + "," +
				GetArrayStr( options.DestModsAllowed ) +
				options.WriteMasksAllowed.Length + "," +
				GetArrayStr( options.WriteMasksAllowed ) +
				options.SrcRegInfo.Length + ",";

			for( int i = 0; i < options.SrcRegInfo.Length; i++ )
			{
				genData +=
					options.SrcRegInfo[ i ].SrcRegsAllowed.Length + "," +
					GetArrayStr( options.SrcRegInfo[ i ].SrcRegsAllowed ) +
					options.SrcRegInfo[ i ].SourceMod + "," +
					options.SrcRegInfo[ i ].ReplicateSwizzles + "," +
					options.SrcRegInfo[ i ].ArbitrarySwizzles + "," +
					options.SrcRegInfo[ i ].GeneralSwizzles + "," +
					options.SrcRegInfo[ i ].MustInitialize + ",";
			}

			genData = genData.Substring( 0, genData.Length - 1 );

			try
			{
				file.WriteLine( genData );
			}
			catch( Exception e ) 
			{ 
				Console.Error.WriteLine( e.StackTrace );
			}
			finally
			{
				file.Close( );
			}
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private string GetArrayStr( string [ ] arr )
		{
			string res = "";
			if( arr.Length == 0 )
				return res;
			foreach( string str in arr )
				res += str + ",";
			return res;
		}	

		//-------------------------------------------------------------------------------------------------------------------------
		
		public GeneratorOptions [ ] ParseOptions( string filename )
		{
			ArrayList optionsList = new ArrayList( );
			StreamReader file = null;
			try
			{
				file = File.OpenText( filename );
			}
			catch( Exception e )
			{
				Console.Error.WriteLine( e.StackTrace );
				throw new IOException( "Failed to open " + filename );
			}

			
			string genData = file.ReadLine( );
			while( genData != null && genData != "" )
			{
				GeneratorOptions options;
				string [ ] genTokens = genData.Split( new char[ ] { ',' } );
				int idx = 0;
				try
				{
					options.ShaderVersion		= genTokens[ idx++ ];
					options.Instruction			= genTokens[ idx++ ];
					options.StaticFlow			= Boolean.Parse( genTokens[ idx++ ] );
					options.DynamicFlow			= Boolean.Parse( genTokens[ idx++ ] );
					options.IsAllowedInDFC		= Boolean.Parse( genTokens[ idx++ ] );
					options.NumSourceRegs		= Int32.Parse( genTokens[ idx++ ] );
					options.DestRegsAllowed		= GetStrArrFromTokens( genTokens, ref idx );
					options.DestModsAllowed		= GetStrArrFromTokens( genTokens, ref idx );
					options.WriteMasksAllowed	= GetStrArrFromTokens( genTokens, ref idx );

					int size = Int32.Parse( genTokens[ idx++ ] );
					SrcRegOptions [ ] srcInfo = new SrcRegOptions[ size ];
					for( int i = 0; i < size; i++ )
					{
						srcInfo[ i ].SrcRegsAllowed		= GetStrArrFromTokens( genTokens, ref idx );
						srcInfo[ i ].SourceMod			= Boolean.Parse( genTokens[ idx++ ] );
						srcInfo[ i ].ReplicateSwizzles	= Boolean.Parse( genTokens[ idx++ ] );
						srcInfo[ i ].ArbitrarySwizzles	= Boolean.Parse( genTokens[ idx++ ] );
						srcInfo[ i ].GeneralSwizzles	= Boolean.Parse( genTokens[ idx++ ] );
						srcInfo[ i ].MustInitialize		= Boolean.Parse( genTokens[ idx++ ] );
					}
					options.SrcRegInfo = srcInfo;
				}
				catch( Exception e )
				{
					Console.Error.WriteLine( e.StackTrace );
					throw new IOException( "File format is invalid\n" );
				}
				optionsList.Add( options );
				genData = file.ReadLine( );
			}
			
			file.Close( );
			GeneratorOptions [ ] ret = new GeneratorOptions[ optionsList.Count ];
			for( int i = 0; i < optionsList.Count; i++ )
				ret[ i ] = (GeneratorOptions) optionsList[ i ];
			
			return ret;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private string [ ] GetStrArrFromTokens( string [ ] tokens, ref int index )
		{
			int size = Int32.Parse( tokens[ index++ ] );
			string [ ] ret;
			if( size == 0 )
				ret = new string [ ] { };
			else
				ret = new string[ size ];

			for( int i = 0; i < size; i++, index++ )
				ret[ i ] = tokens[ index ];
			return ret;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private void ChangeOptionsTexldd( ref GeneratorOptions options )
		{
			SrcRegOptions [ ] texlddRegs = new SrcRegOptions[ 4 ];
			Array.Copy( options.SrcRegInfo, texlddRegs, 3 );
			Array.Copy( options.SrcRegInfo, 2, texlddRegs, 3, 1 );
			options.NumSourceRegs = 4;
			options.SrcRegInfo = texlddRegs;
		}

		//-------------------------------------------------------------------------------------------------------------------------
		
		private void RevertOptionsTexldd( ref GeneratorOptions options )
		{
			SrcRegOptions [ ] oldRegs = new SrcRegOptions[ 3 ];
			Array.Copy( options.SrcRegInfo, oldRegs, 3 );
			options.NumSourceRegs = 3;
			options.SrcRegInfo = oldRegs;
		}
	}


}
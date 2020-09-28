////////////////////////////////////////////////////////////////////////////
//  DoubleArith.h
//  created:	2009/03/02
//
//  purpose: Test correctness of calculations performed at double precision. 
//
//  4/24/11:        D3D11.1 Shader Model Improvements
//  Calculations:   ddiv, drcp, dfma
//  Conversions:    dtoi, dtou, itod, utod
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"

class CShader5xTest_DoubleArith : public CShader5xTest
{
	public:

		//typedef struct { FLOAT_INPUT_CLASS m_ClassX, m_ClassY; } FloatClassPair;
		//typedef struct { DOUBLE_CLASSIFICATION m_ClassX, m_ClassY; } DoubleClassPair;

		struct Binary64
		{
			union 
			{
				unsigned long long m_AsULongLong;
				unsigned long m_AsULong[2];
				double m_AsDouble;
			};
			bool operator ==( Binary64 const& b ) const;
			bool operator !=( Binary64 const& b ) const;
			void Negate();
			void Abs();
			void Saturate();
			//DOUBLE_CLASSIFICATION GetClassification() const;
		};

		struct Binary64Vec2
		{
			union
			{
				struct { Binary64 m_Vec[2]; };
				struct { Binary64 m_X, m_Y; };
			};
			bool operator ==( Binary64Vec2 const& v ) const;
			Binary64& operator []( int );
			Binary64 const& operator []( int i ) const;
			void Negate();
			void Abs();
			void Saturate();
			//DoubleClassPair GetClassification() const;
		};
		
		struct Binary32Vec2
		{
			union
			{
				struct { unsigned int m_VecAsUInt[2]; };
				struct { float m_Vec[2]; };
				struct { float m_X, m_Y; };
			};
			bool operator ==( Binary32Vec2 const& ) const;
			float& operator []( int );
			float const& operator []( int ) const;
			void Negate();
			void Abs();
			//FloatClassPair GetClassification() const;
		};		

		struct SignedBinary32Vec2
		{
			union
			{
				struct { int m_VecAsInt[2]; };
				struct { float m_Vec[2]; };
				struct { float m_X, m_Y; };
			};
			bool operator ==( SignedBinary32Vec2 const& ) const;
			float& operator []( int );
			float const& operator []( int ) const;
			void Negate();
			void Abs();
		};		

		// Binary64 tests methods
		typedef bool (*IsA)( Binary64 const& );
		static bool IsINF( Binary64 const& );
		static bool IsNegINF( Binary64 const& );
		static bool IsPosINF( Binary64 const& );
		static bool IsNaN( Binary64 const& );
		static bool IsZero( Binary64 const& );
		static bool IsNegZero( Binary64 const& );
		static bool IsPosZero( Binary64 const& );
		static bool IsPosOrNegZero( Binary64 const& );
		static bool IsDNorm( Binary64 const& );
		static bool IsDNorm( float const& );
		static bool IsNegDNorm( Binary64 const& );
		static bool IsPosDNorm( Binary64 const& );
		static bool IsNorm( Binary64 const& );
		static bool IsReal( Binary64 const& );
		static bool IsNegReal( Binary64 const& );
		static bool IsPosReal( Binary64 const& );
		static bool IsPosOne( Binary64 const& );
		static bool IsNegOne( Binary64 const& );
		static bool IsSaturatedReal( Binary64 const& );

		// Useful masks and values.
		static const Binary64 s_Sign64;
		static const Binary64 s_Exponent64;
		static const Binary64 s_Fraction64;
		static const Binary64 s_OneULP64;
		static const Binary64 s_NegINF64;
		static const Binary64 s_PosINF64;
		static const Binary64 s_One64;
		static const Binary64 s_NegOne64;
		static const Binary64 s_Zero64;
		static const Binary64 s_NegZero64;
		static const Binary64 s_BadFood64;
		static const Binary64 s_NegNextToSmallestNorm;
		
		virtual ~CShader5xTest_DoubleArith();
		virtual TEST_RESULT Setup();
		virtual TEST_RESULT SetupTestCase();

	protected:
		// Check if a test result matches a gold value
		static bool CompareBinary64Vec2(Binary64Vec2 const& gold,
										Binary64Vec2 const& res,
										tstring const& reg,
										tstring const& mask,
										tstring const& swizzle,
										bool bAllowDeNormFlush = false, 
										bool bSuppressErrorMsg = false,
                                        bool bAllowOneULPDifference = false );
		// Check if a test result matches a gold value
		static bool CompareBinary64( Binary64 const& gold, Binary64 const& res, bool bAllowDeNormFlush = false, bool bAllowOneULPDifference = false );
		static bool CheckBinary64Vec2Result(	Binary64Vec2 const& res,
												tstring const& reg,
												tstring const& mask,
												IsA const pIsAFunc,
												tstring const& message,
												bool bAllowDeNormFlush = false,
												bool bSuppressErrorMsg = false );
		static DOUBLE_CLASSIFICATION GetModifiedClass( DOUBLE_CLASSIFICATION const&, SRC_MOD_TYPE const& );
		static DOUBLE_CLASSIFICATION GetModifiedClass( DOUBLE_CLASSIFICATION const&, INST_MOD_TYPE const& );
		template<typename _Type>
			static _Type GetModifiedValue( _Type const&, SRC_MOD_TYPE const& );
		static Binary64 ConvertToDouble( float const& );
		static float ConvertToFloat( Binary64 const& );

		
		CShader5xTest_DoubleArith();
};

class CShader5xTest_dadd : public CShader5xTest_DoubleArith
{
	#define GEN_ADD_RESULT_COUNT 2136
	private:
		static const Binary64Vec2 s_DAddSrc0[GEN_ADD_RESULT_COUNT];
		static const Binary64Vec2 s_DAddSrc1[GEN_ADD_RESULT_COUNT];
		static const Binary64Vec2 s_DAddRes[GEN_ADD_RESULT_COUNT];
	protected:
		Binary64Vec2 m_Src0;
		Binary64Vec2 m_Src1;
		Binary64Vec2 m_Result;
		DOUBLE_CLASSIFICATION m_Src0Class;
		DOUBLE_CLASSIFICATION m_Src1Class;
		tstring m_Mask;
		tstring m_Swizzle;
		SRC_MOD_TYPE m_Src0Modifier;
		SRC_MOD_TYPE m_Src1Modifier;
		INST_MOD_TYPE m_InstModifier;
		bool m_Generated;
		OPERAND_TYPE m_OperandType[2];
	public:
		virtual void InitTestParameters();

		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();

		virtual ~CShader5xTest_dadd();
};

class CShader5xTest_dmul : public CShader5xTest_DoubleArith
{
	#define GEN_MUL_RESULT_COUNT 1099
	private:
		static const Binary64Vec2 s_DMulSrc0[GEN_MUL_RESULT_COUNT];
		static const Binary64Vec2 s_DMulSrc1[GEN_MUL_RESULT_COUNT];
		static const Binary64Vec2 s_DMulRes[GEN_MUL_RESULT_COUNT];
	protected:
		Binary64Vec2 m_Src0;
		Binary64Vec2 m_Src1;
		Binary64Vec2 m_Result;
		DOUBLE_CLASSIFICATION m_Src0Class;
		DOUBLE_CLASSIFICATION m_Src1Class;
		tstring m_Mask;
		tstring m_Swizzle;
		SRC_MOD_TYPE m_Src0Modifier;
		SRC_MOD_TYPE m_Src1Modifier;
		INST_MOD_TYPE m_InstModifier;
		bool m_Generated;
		OPERAND_TYPE m_OperandType[2];
	public:
		virtual void InitTestParameters();

		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();

		virtual ~CShader5xTest_dmul();
};

class CShader5xTest_ftod : public CShader5xTest_DoubleArith
{
	protected:
		Binary32Vec2 m_Src;
		//DOUBLE_CLASSIFICATION m_ResultClass;
		tstring m_Mask;
		tstring m_Swizzle;
		SRC_MOD_TYPE m_SrcModifier;
	public:
		virtual void InitTestParameters();

		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();

		virtual ~CShader5xTest_ftod();
};

class CShader5xTest_dtof : public CShader5xTest_DoubleArith
{
	protected:
		Binary64Vec2 m_Src;
		tstring m_Mask;
		tstring m_Swizzle;
		SRC_MOD_TYPE m_SrcModifier;
		OPERAND_TYPE m_OperandType;

		Binary64Vec2 ReCalcSrcSwizzle(Binary64Vec2 const&);

	public:
		virtual void InitTestParameters();

		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();

		virtual ~CShader5xTest_dtof();
};

// D3D11.1 Shader Model Improvements
//  4/24/11
// Calculations:    ddiv, drcp, dfma
// Conversions:     dtoi, dtou, itod, utod

class CShader5xTest_ddiv : public CShader5xTest_DoubleArith
{
	#define GEN_DIV_RESULT_COUNT 1099
	private:
		static const Binary64Vec2 s_DDivSrc0[GEN_DIV_RESULT_COUNT];
		static const Binary64Vec2 s_DDivSrc1[GEN_DIV_RESULT_COUNT];
		static const Binary64Vec2 s_DDivRes[GEN_DIV_RESULT_COUNT];
	protected:
		Binary64Vec2 m_Src0;
		Binary64Vec2 m_Src1;
		Binary64Vec2 m_Result;
		DOUBLE_CLASSIFICATION m_Src0Class;
		DOUBLE_CLASSIFICATION m_Src1Class;
		tstring m_Mask;
		tstring m_Swizzle;
		SRC_MOD_TYPE m_Src0Modifier;
		SRC_MOD_TYPE m_Src1Modifier;
		INST_MOD_TYPE m_InstModifier;
		tstring m_InstructionCode;
		bool m_Generated;
		OPERAND_TYPE m_OperandType[2];
	public:
		virtual void InitTestParameters();

		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();

		virtual ~CShader5xTest_ddiv();
};

class CShader5xTest_drcp : public CShader5xTest_DoubleArith
{
	#define GEN_RCP_RESULT_COUNT 1099
	private:
		static const Binary64Vec2 s_DRCPSrc0[GEN_RCP_RESULT_COUNT];
		static const Binary64Vec2 s_DRCPRes[GEN_RCP_RESULT_COUNT];
	protected:
		Binary64Vec2 m_Src0;
		Binary64Vec2 m_Result;
		DOUBLE_CLASSIFICATION m_Src0Class;
		tstring m_Mask;
		tstring m_Swizzle;
		SRC_MOD_TYPE m_Src0Modifier;
		INST_MOD_TYPE m_InstModifier;
		bool m_Generated;
		OPERAND_TYPE m_OperandType[2];
	public:
		virtual void InitTestParameters();

		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();

		virtual ~CShader5xTest_drcp();
};

class CShader5xTest_dfma : public CShader5xTest_DoubleArith
{
	#define GEN_FMA_RESULT_COUNT 1105

	private:
		static const Binary64Vec2 s_DFMASrc0[GEN_FMA_RESULT_COUNT];
		static const Binary64Vec2 s_DFMASrc1[GEN_FMA_RESULT_COUNT];
		static const Binary64Vec2 s_DFMASrc2[GEN_FMA_RESULT_COUNT];
		static const Binary64Vec2 s_DFMARes[GEN_FMA_RESULT_COUNT];
	protected:
		Binary64Vec2 m_Src0;
		Binary64Vec2 m_Src1;
		Binary64Vec2 m_Src2;
		Binary64Vec2 m_Result;
		DOUBLE_CLASSIFICATION m_Src0Class;
		DOUBLE_CLASSIFICATION m_Src1Class;
		DOUBLE_CLASSIFICATION m_Src2Class;
		tstring m_Mask;
		tstring m_Swizzle;
		SRC_MOD_TYPE m_Src0Modifier;
		SRC_MOD_TYPE m_Src1Modifier;
		SRC_MOD_TYPE m_Src2Modifier;
		INST_MOD_TYPE m_InstModifier;
		tstring m_InstructionCode;
		bool m_Generated;
		OPERAND_TYPE m_OperandType[3];
	public:
		virtual void InitTestParameters();

		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();

		virtual ~CShader5xTest_dfma();
};

class CShader5xTest_dtoi : public CShader5xTest_DoubleArith
{
	protected:
		Binary64Vec2 m_Src;
		tstring m_Mask;
		tstring m_Swizzle;
		SRC_MOD_TYPE m_SrcModifier;
		OPERAND_TYPE m_OperandType;

		Binary64Vec2 ReCalcSrcSwizzle(Binary64Vec2 const&);

	public:
		virtual void InitTestParameters();

		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();

		virtual ~CShader5xTest_dtoi();
};

class CShader5xTest_dtou : public CShader5xTest_DoubleArith
{
	protected:
		Binary64Vec2 m_Src;
		tstring m_Mask;
		tstring m_Swizzle;
		SRC_MOD_TYPE m_SrcModifier;
		OPERAND_TYPE m_OperandType;

		Binary64Vec2 ReCalcSrcSwizzle(Binary64Vec2 const&);

	public:
		virtual void InitTestParameters();

		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();

		virtual ~CShader5xTest_dtou();
};

class CShader5xTest_itod : public CShader5xTest_DoubleArith
{
	protected:
		Binary32Vec2 m_Src;
		tstring m_Mask;
		tstring m_Swizzle;
		SRC_MOD_TYPE m_SrcModifier;
	public:
		virtual void InitTestParameters();

		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();

		virtual ~CShader5xTest_itod();
};

class CShader5xTest_utod : public CShader5xTest_DoubleArith
{
	protected:
		Binary32Vec2 m_Src;
		tstring m_Mask;
		tstring m_Swizzle;
		SRC_MOD_TYPE m_SrcModifier;
		OPERAND_TYPE m_OperandType;
	public:
		virtual void InitTestParameters();

		virtual TEST_RESULT SetupTestCase();
		virtual TEST_RESULT ExecuteTestCase();

		virtual ~CShader5xTest_utod();
};

tstring ToString( CShader5xTest_DoubleArith::Binary64 const& );
tstring ToString( CShader5xTest_DoubleArith::Binary64Vec2 const& );
tstring ToString( CShader5xTest_DoubleArith::Binary32Vec2 const& );
tstring ToString( DOUBLE_CLASSIFICATION const& );
bool FromString( CShader5xTest_DoubleArith::Binary64*, tstring const& );
bool FromString( CShader5xTest_DoubleArith::Binary64Vec2*, tstring const& );
bool FromString( CShader5xTest_DoubleArith::Binary32Vec2*, tstring const& );
bool FromString( DOUBLE_CLASSIFICATION*, tstring const& );

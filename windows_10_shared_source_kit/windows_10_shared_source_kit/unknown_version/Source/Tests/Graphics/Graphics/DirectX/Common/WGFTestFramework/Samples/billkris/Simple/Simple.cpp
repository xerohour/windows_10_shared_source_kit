#include <initguid.h>
#include "WGFTestCore.h"
#include "WGFTestUI.h"
#include <stdio.h>

using namespace testfactor;

//////////////////////////////////////////////////////////////////////////
// CMyTest - Base class for test used in this sample.
//////////////////////////////////////////////////////////////////////////
class CMyTest : public CTest
{
public:
	CMyTest() {}
	TEST_RESULT ExecuteTestCase()
		{
			// Pretend to skip every fifth test case
			if( ( GetFramework()->GetCurrentTestCase() % 5 ) == 0 )
				return RESULT_SKIP;
			
			// Pretend to fail every third test case
			if( ( GetFramework()->GetCurrentTestCase() % 3 ) == 0 )
				return RESULT_FAIL;
				
			return RESULT_PASS;
		}
};


//////////////////////////////////////////////////////////////////////////
// CFooTest
//////////////////////////////////////////////////////////////////////////
class CFooTest : public CMyTest
{
public:

	CFooTest() :
		m_fSlider( 0 ),
		//m_TexFormat( DXGI_FORMAT_UNKNOWN ),
		m_nCounter( 0 ) {}

	void InitTestParameters()
		{
			// Simple full-permutation test
			//AddParameter( _T( "TexFormat" ), &m_TexFormat, FormatValueSet( _T( "!( {DEPTH} || {TYPELESS} )" ) ) );
			AddParameter( _T( "Slider" ), &m_fSlider, GradientValueSet( -1.3f, 5.7f, 64  ) );
			AddParameter( _T( "Counter" ), &m_nCounter, RangeValueSet( 0, 12, 1 ) );
		}
		
	TEST_RESULT Setup()
		{
			return RESULT_PASS;
		}
		
protected:
	//DXGI_FORMAT m_TexFormat;
	float m_fSlider;
	int m_nCounter;
};

struct CarInfo
{
	tstring strMake;
	tstring strModel;
	tstring strColor;
};

CarInfo CarData[] =
{
	{ _T( "Ford" ), _T( "Explorer" ), _T( "White" ) },
	{ _T( "Honda" ), _T( "Accord" ), _T( "Black" ) },
	{ _T( "Toyota" ), _T( "Corolla" ), _T( "Green" ) },
	{ _T( "Mazda" ), _T( "Miata" ), _T( "Orange" ) },
	{ _T( "Chevrolet" ), _T( "Corvette" ), _T( "Red" ) },
	{ _T( "Volkswagen" ), _T( "Beetle" ), _T( "Pink (with purple flowers)" ) },
	{ _T( "Dodge" ), _T( "Viper" ), _T( "Yellow" ) },
	{ _T( "Hyundai" ), _T( "Accent" ), _T( "Brown" ) },
	{ _T( "Chrysler" ), _T( "Sebring" ), _T( "Maroon" ) },
	{ _T( "Ford" ), _T( "Mustang" ), _T( "Blue" ) },
	{ _T( "Pontiac" ), _T( "Trans Am" ), _T( "Black" ) },
};

//////////////////////////////////////////////////////////////////////////
// CBarTest
//////////////////////////////////////////////////////////////////////////
class CBarTest : public CMyTest
{
public:
	
	CBarTest() {}
		
	void InitTestParameters()
		{
			// Simple full-permutation test
			RFactor make = AddParameter( 
				_T( "Make" ), 
				&m_CarInfo.strMake, 
				new CTableValueSet< tstring >( 
					&CarData[0].strMake,
					sizeof( CarData[0] ),
					sizeof( CarData ) / sizeof( CarData[0] ) ) );
			
			RFactor model = AddParameter( 
				_T( "Model" ), 
				&m_CarInfo.strModel, 
				new CTableValueSet< tstring >( 
					&CarData[0].strModel,
					sizeof( CarData[0] ),
					sizeof( CarData ) / sizeof( CarData[0] ) ) );
			
			RFactor color = AddParameter( 
				_T( "Color" ), 
				&m_CarInfo.strColor, 
				new CTableValueSet< tstring >( 
					&CarData[0].strColor,
					sizeof( CarData[0] ),
					sizeof( CarData ) / sizeof( CarData[0] ) ) );
					
			SetRootTestFactor( make % model % color );
		}
		
	TEST_RESULT Setup()
		{
			return RESULT_PASS;
		}
	
protected:
	CarInfo m_CarInfo;
};


struct MealInfo
{
	tstring strMeal;
	tstring strFood;
	tstring strDrink;
	tstring strAppetizer;
};


const tstring BreakfastFoodNames[] =
{
	tstring( _T( "Eggs" ) ),
	tstring( _T( "Cereal" ) ),
	tstring( _T( "Pancakes" ) ),
};

const tstring LunchFoodNames[] =
{
	tstring( _T( "Soup" ) ),
	tstring( _T( "Sandwich" ) ),
};

const tstring DinnerFoodNames[] =
{
	tstring( _T( "Steak" ) ),
	tstring( _T( "Lasagna" ) ),
};

const tstring BreakfastDrinkNames[] =
{
	tstring( _T( "Orange Juice" ) ),
	tstring( _T( "Water" ) ),
	tstring( _T( "Milk" ) ),
};

const tstring LunchDrinkNames[] =
{
	tstring( _T( "Milk" ) ),
	tstring( _T( "Water" ) ),
	tstring( _T( "Mountain Dew" ) ),
};

const tstring DinnerDrinkNames[] =
{
	tstring( _T( "Water" ) ),
	tstring( _T( "Wine" ) ),
	tstring( _T( "Beer" ) ),
};

const tstring AppetizerNames[] =
{
	tstring( _T( "Salad" ) ),
	tstring( _T( "Soup" ) ),
	tstring( _T( "Chips 'n' Salsa" ) ),
};

//////////////////////////////////////////////////////////////////////////
// CBazTest
//////////////////////////////////////////////////////////////////////////
class CBazTest : public CMyTest
{
public:
	
	CBazTest() {};
	void InitTestParameters()
		{
			static bool bHappy;
			
			// Add array of structures as a parameter set for this test.
			AddParameter( _T( "Meal" ), &m_MealInfo.strMeal );
			AddParameter( _T( "Food" ), &m_MealInfo.strFood );
			AddParameter( _T( "Drink" ), &m_MealInfo.strDrink );
			AddParameter( _T( "Appetizer" ), &m_MealInfo.strAppetizer );
			CTestCaseParameter< bool > *pHappy = AddParameter( _T( "Happy" ), &bHappy );
			
			RFactor happy = AddParameterValueSet( pHappy, BoolValueSet() ); 
			
			// The following usage of AddParameterValueSet require RTTI enabled.
			RFactor meal_breakfast = AddParameterValue<tstring>( _T( "Meal" ), tstring( "Breakfast" ) );
			RFactor meal_lunch = AddParameterValue<tstring>( _T( "Meal" ), tstring( "Lunch" ) );
			RFactor meal_dinner = AddParameterValue<tstring>( _T( "Meal" ), tstring( "Dinner" ) );
			
			RFactor breakfast_food = AddParameterValueSet<tstring>( _T( "Food" ), new CTableValueSet<tstring>( BreakfastFoodNames, sizeof( tstring ), ARRAY_SIZE( BreakfastFoodNames ) ) );
			RFactor lunch_food = AddParameterValueSet<tstring>( _T( "Food" ), new CTableValueSet<tstring>( LunchFoodNames, sizeof( tstring ), ARRAY_SIZE( LunchFoodNames ) ) );
			RFactor dinner_food = AddParameterValueSet<tstring>( _T( "Food" ), new CTableValueSet<tstring>( DinnerFoodNames, sizeof( tstring ), ARRAY_SIZE( DinnerFoodNames ) ) );
			
			RFactor breakfast_drink = AddParameterValueSet<tstring>( 
				_T( "Drink" ), 
				new CTableValueSet<tstring>( 
					BreakfastDrinkNames, 
					sizeof( tstring ), 
					ARRAY_SIZE( BreakfastDrinkNames ) ) );
					
			RFactor lunch_drink = AddParameterValueSet<tstring>( 
				_T( "Drink" ), 
				new CTableValueSet<tstring>( 
					LunchDrinkNames, 
					sizeof( tstring ), 
					ARRAY_SIZE( LunchDrinkNames ) ) );
					
			RFactor dinner_drink = AddParameterValueSet<tstring>( 
				_T( "Drink" ), 
				new CTableValueSet<tstring>( 
					DinnerDrinkNames, 
					sizeof( tstring ), 
					ARRAY_SIZE( DinnerDrinkNames ) ) );
			
			RFactor dinner_appetizer = AddParameterValueSet<tstring>( 
				_T( "Appetizer" ), 
				new CTableValueSet<tstring>( 
					AppetizerNames, 
					sizeof( tstring ), 
					ARRAY_SIZE( AppetizerNames ) ) );

			SetRootTestFactor( 
				happy * meal_breakfast * breakfast_food * breakfast_drink + 
				meal_lunch * lunch_food * lunch_drink + 
				meal_dinner * dinner_food * dinner_drink * dinner_appetizer
				);

			// Load priority patterns from XML
			CVariationCoreXmlLoader loader;
			tstring errorString;
			HRESULT hr;
			if(FAILED(hr = loader.CreateXmlFileForRead(_T("VarCore.xml"), errorString)))
				WriteError(errorString.c_str());
			if(FAILED(hr = loader.LoadVariationGroupXml( this, _T("Baz"), errorString)))
				WriteError(errorString.c_str());

			// Write a new XML
			CVariationCoreXmlBuilder builder;
			for(size_t i = 0; i < GetNumPriorityProfiles(); ++i)
			{
				CPriorityProfile *priorityProfile = GetPriorityProfile(i);
				builder.AddPriorityProfile(this, _T("Baz"), priorityProfile);
			}
			builder.SaveXml(_T("Simple.xml"), errorString);
		}

	TEST_RESULT Setup()
		{
			return RESULT_PASS;
		}

protected:		
	MealInfo m_MealInfo;
};

//////////////////////////////////////////////////////////////////////////
// CMyTestApp - An instance of a single object derived from CWGFTestFramework
// is required by the framework.
//////////////////////////////////////////////////////////////////////////
class CMyTestApp : public CWGFTestFramework
{
public:
	CMyTestApp() {}
	
	virtual TESTTYPE GetTestType() const { return TESTTYPE_API; }
	
	bool InitTestGroups()
		{
			// Register test class with the group manager.
			if( !ADD_TEST( _T( "foobar\\foo" ), CFooTest ) ) return false;
			if( !ADD_TEST( _T( "foobar\\bar" ), CBarTest ) ) return false;
			if( !ADD_TEST( _T( "baz" ), CBazTest ) ) return false;

			return true;
		}
		
	bool Setup()
		{
			return true;
		}
};

CMyTestApp  g_Application;

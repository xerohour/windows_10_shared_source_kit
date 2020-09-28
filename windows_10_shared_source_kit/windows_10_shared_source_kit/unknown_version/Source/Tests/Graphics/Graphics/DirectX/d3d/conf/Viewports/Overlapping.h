#include "ViewPorts.h"

enum LOCATION
{
	TOP_LEFT_FRONT = 'A',
	TOP_RIGHT_FRONT = 'B',
	BOTTOM_RIGHT_FRONT = 'C',
	BOTTOM_LEFT_FRONT = 'D',
	TOP_FRONT = 'E',
	RIGHT_FRONT = 'F',
	BOTTOM_FRONT = 'G',
	LEFT_FRONT = 'H',
	MIDDLE_FRONT = 'I',
	MIDDLE_WIDE_FRONT = 'J',
	MIDDLE_TALL_FRONT = 'K',
	TOP_LEFT_BACK = 'a',
	TOP_RIGHT_BACK = 'b',
	BOTTOM_RIGHT_BACK = 'c',
	BOTTOM_LEFT_BACK = 'd',
	TOP_BACK = 'e',
	RIGHT_BACK = 'f',
	BOTTOM_BACK = 'g',
	LEFT_BACK = 'h',
	MIDDLE_BACK = 'i',
	MIDDLE_WIDE_BACK = 'j',
	MIDDLE_TALL_BACK = 'k',
};

class OverlappingView : public ViewportsTest
{
public:
	OverlappingView() : m_pViewportPattern(_T("aa")) {}
	void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();

private:
	tstring m_pViewportPattern;
};
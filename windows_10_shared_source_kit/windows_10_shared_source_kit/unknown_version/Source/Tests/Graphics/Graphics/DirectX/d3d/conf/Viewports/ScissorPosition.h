#include "Viewports.h"

enum ScissorPositions
{
	LARGER_THAN_TARGET,
	OFF_TARGET
};

DECLARE_NAMED_VALUES( ScissorPositions )

class ScissorPosition : public ViewportsTest
{
private:
	ScissorPositions m_position;
	UINT m_scissorSize;
public:
	void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
};
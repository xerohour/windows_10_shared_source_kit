#include "MultiScissor.h"

struct color4
{
	color4(FLOAT R, FLOAT G, FLOAT B, FLOAT A) {r=R;g=G;b=B;a=A;}
	color4() {r=0.0f;g=0.0f;b=0.0f;a=0.0f;}
	FLOAT r;
	FLOAT g;
	FLOAT b;
	FLOAT a;
};

class BadIndex : public MultiScissor
{
private:
	UINT m_ViewportArrayIndex;

public:
	void InitTestParameters();
	TEST_RESULT ExecuteTestCase();
};

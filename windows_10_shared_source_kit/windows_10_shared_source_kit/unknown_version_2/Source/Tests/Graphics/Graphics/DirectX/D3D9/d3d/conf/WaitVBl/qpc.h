#ifndef MY_QPC_H_INCLUDED
#define MY_QPC_H_INCLUDED

#include <windows.h>

typedef enum _QPCStateVals{QPC_Uninitialized, QPC_OK, QPC_Unavailable} QPCStateVals;

class clsQPC
{
protected:
	static QPCStateVals CurrentState;
	static LARGE_INTEGER Frequency;
public:
	static QPCStateVals Initialize();
	static LARGE_INTEGER GetFrequency();
};

#endif

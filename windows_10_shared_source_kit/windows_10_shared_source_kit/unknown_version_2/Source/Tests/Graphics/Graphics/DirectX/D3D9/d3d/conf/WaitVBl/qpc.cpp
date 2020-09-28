#include "qpc.h"

QPCStateVals clsQPC::CurrentState = QPC_Uninitialized;
LARGE_INTEGER clsQPC::Frequency;

QPCStateVals clsQPC::Initialize()
{
	if (CurrentState == QPC_Uninitialized)
	{
		CurrentState = QPC_Unavailable;
		if (QueryPerformanceFrequency(&Frequency))
			if (Frequency.QuadPart > 0)
				CurrentState = QPC_OK;
	}

	return CurrentState;
}

LARGE_INTEGER clsQPC::GetFrequency()
{
	return Frequency;
}

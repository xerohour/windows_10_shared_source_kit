// rnumber.h
//
// random number generator
//
// Created: 07 August 1996  :  Eron J. Hennessey

#ifndef _RNUMBER_H_
#define _RNUMBER_H_

#include <math.h>
#include <stdlib.h>
#include <time.h>

int Randomize(int nSeed) // set the random number seed according to the clock
{
	const unsigned int t = nSeed ? nSeed : (unsigned)time(NULL);
	srand(t);
	return t;
}

int RoundFloat(float nNum) // for rounding floats to the cooresponding int.
{
	double nFract; // fractional part of nNum
	double nTemp; // because modf needs it.

	nFract = fabs(modf(nNum, &nTemp));

	if(nNum > 0.0)  // number is positive
	{
		if(nFract <= 0.50)
			return (int)floor(nNum);
		else
			return (int)ceil(nNum);
	}
	else // number is negative
	{
		if(nFract >= 0.50)
			return (int)floor(nNum);
		else
			return (int)ceil(nNum);
	}
}

int GetRandomNumber(int nLow, int nHigh)
{
	// generates a random number with a lower limit of nLow and and upper
	// limit of nHigh.  nLow and nHigh are included in the possible values.

	// my random number algorithm is as follows:
	//
	// 1. Generate a random number between zero and one
	//           a = (rand() / RAND_MAX)
	// 2. Multiply this number by the difference between the High and Low number
	//           b = a * (nHigh - nLow)
	// 3. Add this amount to the Low number
	//           c = nLow + b

	// This gives a random number guaranteed to be between nLow and nHigh.

	int nReturn;
	float a, b;

	// swap low and high if they're wrong.
	if(nLow > nHigh)
	{
		int nTemp = nLow;
		nLow = nHigh;
		nHigh = nTemp;
	}

	a = (float)rand() / RAND_MAX;
	b = a * (float)(nHigh - nLow);
	
	nReturn = RoundFloat((float)nLow + b); 
	
	// nLow + (((float)(rand() / RAND_MAX)) * (nHigh - nLow));
	
	return nReturn; 
}

#endif

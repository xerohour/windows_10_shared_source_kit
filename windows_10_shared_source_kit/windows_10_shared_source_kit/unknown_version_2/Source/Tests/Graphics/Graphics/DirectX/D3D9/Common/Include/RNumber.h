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

int Randomize(int nSeed = 0);

int RoundFloat(float nNum);

int GetRandomNumber(int nLow, int nHigh);

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

#endif

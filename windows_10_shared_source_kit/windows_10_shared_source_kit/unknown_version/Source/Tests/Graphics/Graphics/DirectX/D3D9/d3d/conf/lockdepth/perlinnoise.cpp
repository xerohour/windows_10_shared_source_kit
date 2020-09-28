//
//  PerlinNoise.cpp
//
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2000 Microsoft Corporation.
//
//

//-----------------------------------------------------------------------------

#include <math.h>

//-----------------------------------------------------------------------------

double Noise1( int x, int y )
{
	int n = x + y * 57;
	n = (n<<13) ^ n;
	return (1.0f - ( ( n * ( n * n * 15731 + 789221 ) + 1376312589) & 0x7fffffff) / 1073741824.0f);
}

//-----------------------------------------------------------------------------

double SmoothNoise1(int x, int y)
{
	double corners = (Noise1(x-1,y-1) + Noise1(x+1,y-1) + Noise1(x-1,y+1) + Noise1(x+1,y+1))/16.f;
	double sides = (Noise1(x-1,y) + Noise1(x+1,y) + Noise1(x,y-1) + Noise1(x,y+1))/8.f;
	double center = (Noise1(x, y))/4.f;
	return corners + sides + center;
}

//-----------------------------------------------------------------------------

double Cubic_Interpolate(double v0, double v1, double v2, double v3, double x)
{
	double P = (v3-v2) - (v0-v1);
	double Q = (v0-v1) - P;
	double R = v2-v0;
	double S = v1;

	return P*x*x*x + Q*x*x + R*x + S;
}

//-----------------------------------------------------------------------------

double Cosine_Interpolate(double a, double b, double x)
{
	const static double pi = atan(1.f)*4.f;
	double ft = x * pi;
	double f = (1.f - cos(ft) ) * 0.5f;
	return a*(1.f - f) + b * f;
}

//-----------------------------------------------------------------------------

double InterpolatedNoise_1(double x, double y)
{
	int integer_X = (int)floor(x);
	double fractional_X = x - (double)integer_X;

	int integer_Y = (int)floor(y);
	double fractional_Y = y - (double)integer_Y;

	double v1 = SmoothNoise1(integer_X, integer_Y );
	double v2 = SmoothNoise1(integer_X+1, integer_Y);
	double v3 = SmoothNoise1(integer_X, integer_Y+1);
	double v4 = SmoothNoise1(integer_X+1, integer_Y+1);

	double i1 = Cosine_Interpolate( v1, v2, fractional_X );
	double i2 = Cosine_Interpolate( v3, v4, fractional_X );

	return Cosine_Interpolate( i1, i2, fractional_Y );
}

//-----------------------------------------------------------------------------

double PerlinNoise_2D(double x, double y, int nOctaves, double persistance)
{

	double total = 0.f;

	for( int i=0; i<nOctaves; i++)
	{
		double frequency = pow(2.0, (double)i);
		double amplitude = pow(persistance,(double)i);
		
		total += InterpolatedNoise_1( x * frequency, y * frequency ) * amplitude;
	}

	return total;
}

//-----------------------------------------------------------------------------


#ifndef __PerlinNoise_h__
#define __PerlinNoise_h__

// given 2 coordinates compute a noise value consistine of n Octaves
double PerlinNoise_2D(double x, double y, int nOctaves, double persistance);

#endif
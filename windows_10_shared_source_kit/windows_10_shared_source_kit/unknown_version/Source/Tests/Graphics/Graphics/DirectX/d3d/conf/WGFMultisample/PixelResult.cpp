#include "PixelResult.h"
#include <assert.h> 

PixelResult::PixelResult(unsigned int x, unsigned int y) {
	width = x;
	height = y;
	unsigned int size = width * height;
	pix = new float[size]; 
	// zero out the memory
	float *init = pix;
	for (unsigned int i = 0; i < size; i++)
		*init++ = 0.f;
}

PixelResult::PixelResult(const PixelResult &cp) {
	height = cp.height;
	width = cp.width;
	pix = new float[height * width];
	// copy the image over
	for (unsigned int i = 0; i < height * width; i++) {
		pix[i] = cp.pix[i];
	}
}

float PixelResult::getPixelValue(unsigned int i) const {
	// invalid i, if they are greater than the image space
	assert(i < width * height);
	return pix[i];
}

// Set the pixel value
// The value interpretation is customized by the tester
bool PixelResult::setPixelValue(unsigned int x, unsigned int y, float val) {
	// invalid x or y, if they are greater than the image space
	assert(x < width && y < height);
	// pixels are indexed row by row. (x + y * width) gets the index of the pixel.
	pix[x + y * width] = val;

	return true;
}

#pragma once
/*
 * Class PixelResult is a utility class for pixel by pixel comparison in fill rule tests.
 * This class provides the data structure to store one value for each pixel position used for validation in tests.
 * Client specifies the height and width of the render target, assuming the upper left corner is at 0, 0
 */
class PixelResult {
public:
	// Initialize a "w" by "h" pixel dummy render target for comparison
	// default value is 128 for width and height
	PixelResult(unsigned int w = 128, unsigned int h = 128);
	PixelResult(const PixelResult &cp);
	~PixelResult() { if (pix) delete [] pix; }
	// return pix[i] value
	float getPixelValue(unsigned int i) const;
	// Set the value associated with the pixel
	// The value interpretation is customized by the tester
	// returns false if x or y is invalid
	bool setPixelValue(unsigned int x, unsigned int y, float val);
private:
	// pix stores an array of the pixel information
	float *pix;
	unsigned int height;
	unsigned int width;
};
#ifndef _COLORCONVERTER_h
#define _COLORCONVERTER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class ColorConverter 
{

public:
	static void RgbToHsv(uint8_t r, uint8_t g, uint8_t b, double &hue, double &saturation, double &value);


	void  colorConverterExample() {
		uint8_t red = 50;
		uint8_t green = 100;
		uint8_t blue = 150;
		double hue, saturation, value;
		ColorConverter::RgbToHsv(red, green, blue, hue, saturation, value);
	}

private:
	static double threeway_max(double a, double b, double c);
	static double threeway_min(double a, double b, double c);
	static double hue2rgb(double p, double q, double t);
};
#endif // _COLORCONVERTER_h




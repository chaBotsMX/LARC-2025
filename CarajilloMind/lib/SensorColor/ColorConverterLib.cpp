
#include "ColorConverterLib.h"
#include <Arduino.h>

void ColorConverter::RgbToHsv(uint8_t red, uint8_t green, uint8_t blue, double& hue, double& saturation, double& value)
{
	auto rd = static_cast<double>(red) / 255;
	auto gd = static_cast<double>(green) / 255;
	auto bd = static_cast<double>(blue) / 255;
	auto max = threeway_max(rd, gd, bd), min = threeway_min(rd, gd, bd);
	 
	value = max;

	auto d = max - min;
	saturation = max == 0 ? 0 : d / max;

	hue = 0;
	if (max != min)
	{
		if (max == rd)
		{
			hue = (gd - bd) / d + (gd < bd ? 6 : 0);
		}
		else if (max == gd)
		{
			hue = (bd - rd) / d + 2;
		}
		else if (max == bd)
		{
			hue = (rd - gd) / d + 4;
		}
		hue /= 6;
	}
}

double inline ColorConverter::threeway_max(double a, double b, double c)
{
	return max(a, max(b, c));
}

double inline ColorConverter::threeway_min(double a, double b, double c)
{
	return min(a, min(b, c));
}


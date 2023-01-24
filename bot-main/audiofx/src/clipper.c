#include "clipper.h"

#include "math_extra.h"

float mono_clipper(float in) {
	float sign = 1.0;
	if(in < 0.0) {
		sign = -1.0;
		in = -in;
	}
	
	float x = (1 - in);
	x = x * x * x; // x ^ 9
	x = x * x * x;
	
	float out = 1 - x;
	if(out > 1.0) out = 1.0;
	
	return sign * out * 0.5; // minus 6 decibels. Should hopefully prevent too loud of peaks
}
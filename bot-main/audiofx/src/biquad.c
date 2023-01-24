#include "biquad.h"

#include "math_extra.h"

// Use the RBJ EQ cookbook to compute biquad coefficients.

void biquad_make_lowshelf(Biquad *bq, double sample_freq, double center_freq, double gain_db, double q) {
	double A = pow(10.0, gain_db / 40.0);
	double w0 = TAU * center_freq / sample_freq;
	
	double cosw0 = cos(w0);
	double sinw0 = sin(w0);
	
	double alpha = sinw0 / (2.0 * q);
	
	double asA2 = 2.0 * sqrt(A) * alpha;
	
	// The numbers stored in the biquad will all be normalized by a0.
	double a0, a1, a2, b0, b1, b2;
	
	// RBJ Cookbook formula. Substitute pre-calculated values for speed, hopefully,
	// although the compiler would probably do it for us.
	b0 =    A*( (A+1) - (A-1)*cosw0 + asA2 );
	b1 =  2*A*( (A-1) - (A+1)*cosw0        );
	b2 =    A*( (A+1) - (A-1)*cosw0 - asA2 );
	a0 =        (A+1) + (A-1)*cosw0 + asA2  ;
	a1 =   -2*( (A-1) + (A+1)*cosw0        );
	a2 =        (A+1) + (A-1)*cosw0 - asA2  ;
	
	bq->b0 = (float)(b0 / a0);
	bq->b1 = (float)(b1 / a0);
	bq->b2 = (float)(b2 / a0);
	bq->a1 = (float)(a1 / a0);
	bq->a2 = (float)(a2 / a0);
}

void biquad_make_peak(Biquad *bq, double sample_freq, double center_freq, double gain_db, double q) {
	double A = pow(10.0, gain_db / 40.0);
	double w0 = TAU * center_freq / sample_freq;
	
	double cosw0 = cos(w0);
	double sinw0 = sin(w0);
	
	double alpha = sinw0 / (2.0 * q);
	
	//double asA2 = 2.0 * sqrt(A) * alpha;
	
	// The numbers stored in the biquad will all be normalized by a0.
	double a0, a1, a2, b0, b1, b2;
	
	// RBJ Cookbook formula. Substitute pre-calculated values for speed, hopefully,
	// although the compiler would probably do it for us.
	b0 =   1 + alpha*A;
	b1 =  -2*cosw0;
	b2 =   1 - alpha*A;
	a0 =   1 + alpha/A;
	a1 =  -2*cosw0;
	a2 =   1 - alpha/A;
	
	bq->b0 = (float)(b0 / a0);
	bq->b1 = (float)(b1 / a0);
	bq->b2 = (float)(b2 / a0);
	bq->a1 = (float)(a1 / a0);
	bq->a2 = (float)(a2 / a0);
}

float mono_biquad_update(Biquad *bq, float in) {
	// Direct Form II. Supposedly the fastest form for digital biquads.
	// See these links for more info:
	//     https://www.earlevel.com/main/2012/11/26/biquad-c-source-code/
	//     https://www.earlevel.com/main/2003/02/28/biquads/
	
	// A note on conventions...
	// The RBJ cookbook uses b0..b2 to multiply x[0], x[1], x[2]
	// The earlevel code uses a0..a2 to multiply x[0], x[1], x[2] (effecitvely)
	// But we are using the RBJ cookbook.
	// So the names here are swapped from the earlevel code.
	float out = in * bq->b0 + bq->z1;
	bq->z1 = in * bq->b1 + bq->z2 - bq->a1 * out;
	bq->z2 = in * bq->b2 - bq->a2 * out;
	
	return out;
}

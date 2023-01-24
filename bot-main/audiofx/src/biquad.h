#ifndef BIQUAD_H
#define BIQUAD_H

typedef struct {
	float b0;
	float b1;
	float b2;
	float a1;
	float a2;
	
	// Delayed samples
	float z1;
	float z2;
} Biquad;

#define biquad_new() ((Biquad){0})

void biquad_make_lowshelf(Biquad *biquad, double sample_freq, double center_freq, double gain_db, double q);
void biquad_make_highshelf(Biquad *biquad, double sample_freq, double center_freq, double gain_db, double q);
void biquad_make_peak(Biquad *biquad, double sample_freq, double center_freq, double gain_db, double q);

float mono_biquad_update(Biquad *biquad, float in);

#endif
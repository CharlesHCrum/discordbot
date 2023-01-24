#ifndef AUDIO_DATA_H
#define AUDIO_DATA_H

#include <stdint.h>

// This is a simple way to package the audio data loading and saving into
// a handful of functions so that the main code is easier to read.
typedef struct {
	uint64_t frames;
	uint32_t channels;
	uint32_t sample_rate;
	float *sample_data;
} AudioData;

#define audio_data_new() ((AudioData){0})

typedef enum {
// Define constants for possible errors in the audio_data functions.
	AUDIO_DATA_OK = 0,
	AUDIO_DATA_COULDNT_OPEN_INPUT,  // Specifically couldn't open error.
	AUDIO_DATA_COULDNT_READ,        // Generic IO error.
	AUDIO_DATA_COULDNT_OPEN_OUTPUT, // Specifically couldn't open error.
	AUDIO_DATA_COULDNT_WRITE        // Generic IO error.
} AudioDataError;

AudioDataError audio_data_load(AudioData *out, const char *filepath);
AudioDataError audio_data_save(AudioData *in, const char *filepath);

/**
 * Cleans up internal memory allocations of the given AudioData object.
 * (that is, cleans up the sample_data).
 *
 * This does NOT free the 'data' pointer itself.
 */
void audio_data_cleanup(AudioData *data);

#endif
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "biquad.h"
#include "clipper.h"
#include "audio_data.h"
#include "error_handler.h"

void process_audio(AudioData *data) {
	Biquad low_shelf_left, low_shelf_right;
	Biquad mid_dip_left, mid_dip_right;
	
	low_shelf_left = biquad_new();
	mid_dip_left = biquad_new();
	
	biquad_make_lowshelf(&low_shelf_left, data->sample_rate, 320, 9.0, 1.14);
	biquad_make_peak(&mid_dip_left, data->sample_rate, 2240, -3.5, 0.16);
	
	// Copy the biquads for the right channel
	low_shelf_right = low_shelf_left;
	mid_dip_right = mid_dip_left;

	// Process stream as stereo. frames * 2 = sample count for stereo stream;
	// left channel and right channel are interleaved.
	for(int frame_start = 0; frame_start < data->frames * data->channels; frame_start += data->channels) {
		int left = frame_start;
		int right = frame_start + 1;
		
		float left_new = mono_biquad_update(&low_shelf_left, data->sample_data[left]);
		left_new = mono_biquad_update(&mid_dip_left, left_new);
		left_new = mono_clipper(left_new);
		
		data->sample_data[left] = left_new;
		
		// Only perform stereo processing if there are actually two channels.
		if(data->channels > 1) {
			float right_new = mono_biquad_update(&low_shelf_right, data->sample_data[right]);
			right_new = mono_biquad_update(&mid_dip_right, right_new);
			right_new = mono_clipper(right_new);
			
			data->sample_data[right] = right_new;
		}
	}
}

void get_command_args(int argc, char **argv, char **in_path, char **out_path) {
	assert(in_path);
	assert(out_path);
	
	if(argc < 3) {
		// We need two args; print usage otherwise.
		fprintf(stderr, "usage: %s <input.wav> <output.wav>\n", argv[0]);
		exit(-1);
	}
	
	*in_path = argv[1];
	*out_path = argv[2];
}

int main(int argc, char **argv) {
	// Read the command line arguments to determine input and output file paths.
	char *in_path;
	char *out_path;
	get_command_args(argc, argv, &in_path, &out_path);


	AudioDataError io_err;
	AudioData audio_data = audio_data_new();
	
	// Load the audio data
	io_err = audio_data_load(&audio_data, in_path);
	error_handle_audio_data(io_err);
	
	// Process the audio data
	process_audio(&audio_data);
	
	// Save the audio data
	io_err = audio_data_save(&audio_data, out_path);
	error_handle_audio_data(io_err);
	
	// Cleanup
	audio_data_cleanup(&audio_data);
   
    return 0;
}
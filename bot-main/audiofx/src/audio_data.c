#include "audio_data.h"

#include <assert.h>

// This file is responsible for all audio IO, and it is therefore
// the only use of dr_wav. As such, we can simply include dr_wav
// statically here.
#define DR_WAV_IMPLEMENTATION
#include <dr_wav.h>

AudioDataError audio_data_load(AudioData *out, const char *filepath) {
	assert(out); // Do not pass a null pointer here.
	
	// Use dr_wav to read the wav data.
	out->sample_data = drwav_open_file_and_read_pcm_frames_f32(
		filepath,
		&out->channels,
		&out->sample_rate,
		&out->frames,
		NULL);
		
	// NULL sample data = bad input file or otherwise couldn't read the file.
	if(out->sample_data == NULL) {
		return AUDIO_DATA_COULDNT_READ;
	}
	
	return AUDIO_DATA_OK;
}

AudioDataError audio_data_save(AudioData *in, const char *filepath) {
	assert(in); // Do not pass a null pointer here.
	
	// drwav object for writing output.
	drwav wav;
	
	// Setup the data format.
	drwav_data_format format;
    format.container     = drwav_container_riff;      // riff = normal data format.
    format.format        = DR_WAVE_FORMAT_IEEE_FLOAT; // FLOAT for 32 bit float data format. TODO: Consider if we want to write 16 bit wavs?
    format.channels      = in->channels;
    format.sampleRate    = in->sample_rate;
    format.bitsPerSample = 32; // 32 bit float
	
	// Open file for writing.
	drwav_bool32 successful = drwav_init_file_write_sequential_pcm_frames(
		&wav,
		filepath,
		&format,
		in->frames,
		NULL);
		
	if(!successful) {
		return AUDIO_DATA_COULDNT_OPEN_OUTPUT;
	}
	
	// Write frames.
	int frames_written = drwav_write_pcm_frames(&wav, in->frames, in->sample_data);
	
	// We want to be sure we write all the frames.
	if(frames_written != in->frames) {
		drwav_uninit(&wav); // Cleanup
		return AUDIO_DATA_COULDNT_WRITE;
	}

	// Cleanup.
	drwav_uninit(&wav);
	
	return AUDIO_DATA_OK;
}

void audio_data_cleanup(AudioData *ad) {
	assert(ad);
	
	drwav_free(ad->sample_data, NULL);
	
	// Reset everything to zero.
	*ad = audio_data_new();
}
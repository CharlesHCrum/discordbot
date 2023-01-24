#include "error_handler.h"

#include <stdio.h>
#include <stdlib.h>

#include "audio_data.h"

void error_handle_audio_data(AudioDataError err) {
	if(err == AUDIO_DATA_OK) return;
	
	switch(err) {
	case AUDIO_DATA_COULDNT_OPEN_INPUT:
		fputs("error: could not open input file.\n", stderr);
		break;
	case AUDIO_DATA_COULDNT_READ:
		fputs("error: unknown error when reading input file.\n", stderr);
		break;
	case AUDIO_DATA_COULDNT_OPEN_OUTPUT:
		fputs( "error: could not open output file.\n", stderr);
		break;
	case AUDIO_DATA_COULDNT_WRITE:
		fputs( "error: unknown error when writing output file.\n", stderr);
		break;
	default:
		fputs("error: unknown AudioData error.\n", stderr);
		break;
	}
	
	exit(err);
}
#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

// Centralize all error printing and exiting.
// Whenever there is an error, we will simply print and exit. Also, we
// often don't have much info about the error in the first place. So,
// it is possible to simply have functions for each stage in the audio
// processing pipeline that will print associated errors.

// In the future, if we need more detailed error messages (which will
// probably involve knowing more about the Python side of the code),
// we can change this.

void error_handle_audio_data(AudioDataError err);

#endif
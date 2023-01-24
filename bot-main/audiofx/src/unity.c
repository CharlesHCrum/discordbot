// Single C file for unity build.
// The digital signal processing component of the project should be as fast
// as possible. As such, it is built using a unity build--i.e. all files
// are combined into a single .c file which is passed to the compiler.
//
// This lets the compiler perform as much optimization as it possibly
// can. Of course, benchmarking a unity build versus a normal build would
// be the only real way to determine if it was actually faster.
//
// Either way, unity builds are also simpler to setup, so they are used
// for the time being.

#include "biquad.c"
#include "clipper.c"
#include "main.c"
#include "audio_data.c"
#include "error_handler.c"
SRC_FILES=\
unity.c\
main.c\
biquad.c\
audio_data.c\
error_handler.c

SRC_FILES:=$(addprefix src/,$(SRC_FILES))

fx_bassboost: $(SRC_FILES)
	gcc src/unity.c -o fx_bassboost -Wall -Werror -Ilib -O3
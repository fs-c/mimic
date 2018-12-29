#ifndef MIMIC_H
#define MIMIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

typedef unsigned char BYTE;

void set_stream(FILE *new_stream);

BYTE read_byte();
int16_t read_int16();
int32_t read_int32();
int64_t read_int64();

size_t read_osu_string(char *out_buffer);

int decompress_basic(BYTE *in_data, size_t in_len, BYTE **out_data,
	size_t *out_len);

#endif /* MIMIC_H */

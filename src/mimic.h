#ifndef MIMIC_H
#define MIMIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

typedef unsigned char BYTE;

struct replay_meta {
	BYTE mode;
	int32_t version;
	char *beatmap_hash;
	char *player_name;
	char *replay_hash;
	int32_t score;
	int32_t max_combo;
	BYTE perfect;
	int32_t mods_used;
	char *life_bar;
	int64_t timestamp;
};

struct replay_data {
	size_t raw_len;
	BYTE *raw;
};

void set_stream(FILE *new_stream);

BYTE read_byte();
int16_t read_int16();
int32_t read_int32();
int64_t read_int64();

size_t read_osu_string(char *out_buffer);

int decompress_basic(BYTE *in_data, size_t in_len, BYTE **out_data,
	size_t *out_len);

int parse_replay(FILE *file, struct replay_meta *meta,
	struct replay_data *data);

#endif /* MIMIC_H */

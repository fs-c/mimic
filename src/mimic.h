#ifndef MIMIC_H
#define MIMIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#ifdef DEBUG

  #define debug(...)\
      printf("[debug] [%s:%s] ", __FILE__, __func__);\
      printf(__VA_ARGS__);\
      putchar('\n');\

#else

  #define debug(...)\
      ;\

#endif /* DEBUG */

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

struct replay_action {
	int64_t offset;
	float x_coord;
	float y_coord;
	int32_t keys;

	int64_t time;	
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
	struct replay_action **actions, size_t *actions_len);

void free_replay_meta(struct replay_meta *meta);

#endif /* MIMIC_H */

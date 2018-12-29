#include "mimic.h"

static void assign_string(char **dest);

char string_buffer[2048];

int parse_replay(FILE *file, struct replay_meta *meta, struct replay_data *data)
{
	if (!file || !meta) {
		debug("got a null pointer");

		return 1;
	}

	set_stream(file);

	meta->mode = read_byte();
	meta->version = read_int32();

	assign_string(&meta->beatmap_hash);
	assign_string(&meta->player_name);
	assign_string(&meta->replay_hash);

	/* Discard the next 6 shorts */
	fread(string_buffer, sizeof(int16_t), 6, file);

	meta->score = read_int32();
	meta->max_combo = read_int16();
	meta->perfect = read_byte();
	meta->mods_used = read_int32();
	
	assign_string(&meta->life_bar);

	meta->timestamp = read_int64();

	/* Replay data parsing is optional */
	if (!data) {
		debug("skipping data parsing");

		return 0;
	}

	int32_t comp_len = read_int32();
	BYTE *comp = malloc(comp_len);

	if (!comp) {
		debug("failed allocating %d bytes for compressed data",
			comp_len);

		return 2;
	}

	for (int32_t i = 0, c; i < comp_len; i++) {
		if ((c = fgetc(file)) == EOF) {
			debug("reached EOF early, bailing out (%d)\n", i);

			break;
		}

		comp[i] = c;
	}

	BYTE *decomp = NULL;
	size_t decomp_len = 0;

	if (decompress_basic(comp, comp_len, &data->raw, &data->raw_len)) {
		debug("decompression failed\n");

		return 1;
	}

	return 0;
}

static void assign_string(char **dest)
{
	size_t string_len = read_osu_string(string_buffer);

	if (string_len <= 0) {
		*dest = NULL;

		return;
	}

	if (!(*dest = malloc(string_len))) {
		debug("falied allocating %d bytes for string", string_len);

		return;
	}

	strncpy(*dest, string_buffer, string_len);
}

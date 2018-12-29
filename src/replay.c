#include "mimic.h"

struct replay_raw {
	BYTE *data;
	size_t data_len;
};

static void assign_string(char **dest);

static int parse_raw(BYTE *in_data, size_t in_len,
	struct replay_action **out_actions, size_t *out_actions_len);
static int parse_action(char *segment, struct replay_action *out_action,
	int64_t *real_time);

char string_buffer[2048];

int parse_replay(FILE *file, struct replay_meta *meta,
	struct replay_action **actions, size_t *actions_len)
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

	int32_t comp_len = read_int32();
	BYTE *comp = malloc(comp_len);

	if (!comp) {
		debug("failed allocating %d bytes for compressed data",
			comp_len);

		return 2;
	}

	for (int32_t i = 0, c; i < comp_len; i++) {
		if ((c = fgetc(file)) == EOF) {
			debug("reached EOF early, bailing out (%d)", i);

			break;
		}

		comp[i] = c;
	}

	struct replay_raw raw = { 0 };

	if (decompress_basic(comp, comp_len, &raw.data, &raw.data_len)) {
		debug("decompression failed");

		return 1;
	}

	/* The last character should always be obsolete, so we're not losing
	   anything important here */
	raw.data_len -= 1;
	raw.data[raw.data_len] = '\0';

	if (parse_raw(raw.data, raw.data_len, actions, actions_len)) {
		debug("raw data parsing failed");
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
		debug("failed allocating %d bytes for string", string_len);

		return;
	}

	strncpy(*dest, string_buffer, string_len);
}

/* TODO: Clean up the implementation */
static int parse_raw(BYTE *in_data, size_t in_len,
	struct replay_action **out_actions, size_t *out_actions_len)
{
	*out_actions_len = 0;
	*out_actions = malloc(sizeof(struct replay_action));

	int64_t real_time = 0;

	char *token = strtok(in_data, ",");
	while (token) {
		*out_actions_len = (*out_actions_len) + 1;
		*out_actions = realloc(*out_actions,
			*out_actions_len * sizeof(struct replay_action));

		size_t offset = (*out_actions_len) - 1;
		if (parse_action(token, *out_actions + offset, &real_time)) {
			debug("failed parsing action (%s)", token);
		}

		token = strtok(NULL, ",");
	}

	/* Looks like the last token is always invalid, discard it */
	*out_actions_len = *out_actions_len - 1;

	return 0;
}

/* TODO: Find an alternative to __strtok_r that is portable */
static int parse_action(char *segment, struct replay_action *out_action,
	int64_t *real_time)
{
	int i = 0;
	char *safe;
	char *item = __strtok_r(segment, "|", &safe);
	while (item) {
		switch (i++) {
		case 0: out_action->offset = strtol(item, NULL, 10);
			break;
		case 1: out_action->x_coord = strtof(item, NULL);
			break;
		case 2: out_action->y_coord = strtof(item, NULL);
			break;
		case 3: out_action->keys = strtol(item, NULL, 10);
			break;
		}

		*real_time = (*real_time) + out_action->offset;
		out_action->time = *real_time;

		item = __strtok_r(NULL, "|", &safe);
	}

	return 0;
}

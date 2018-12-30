#include "mimic.h"

int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);

	if (argc < 2) {
		printf("usage: %s <path to .osr file>\n", argv[0]);

		return 1;
	}

	FILE *stream = NULL;

	if (!(stream = fopen(argv[1], "rb"))) {
		printf("couldn't open file %s for reading\n", argv[1]);

		return 1;
	} else printf("opened file %s for reading\n", argv[1]);

	struct replay_meta replay_meta = { 0 };

	size_t actions_len = 0;
	struct replay_action *actions = NULL;

	if (parse_replay(stream, &replay_meta, &actions, &actions_len)) {
		printf("something went wrong while parsing\n");

		return 1;
	}

	for (size_t i = 0; i < actions_len / 100; i++) {
		struct replay_action *a = actions + i;
		printf("time: %d (offset: %ld), x/y: %f/%f, keys: %d\n", a->time, 
			a->offset, a->x_coord, a->y_coord, a->keys);
	}

	fclose(stream);

	return 0;
}

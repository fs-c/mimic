#include "mimic.h"

int main(int argc, char *argv[])
{
	setbuf(stdout, NULL);

	if (argc < 2) {
		printf("usage: %s <path to .osr file> [<path to out file>]\n", argv[0]);

		return 1;
	}

	FILE *stream = NULL;

	if (!(stream = fopen(argv[1], "rb"))) {
		printf("couldn't open file %s for reading\n", argv[1]);

		return 1;
	} else printf("opened file %s for reading\n", argv[1]);

	struct replay_meta replay_meta = { 0 };
	struct replay_data replay_data = { 0 };
	if (parse_replay(stream, &replay_meta, &replay_data)) {
		printf("something went wrong while parsing\n");

		return 1;
	}

	fclose(stream);

	return 0;
}

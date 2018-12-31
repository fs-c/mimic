#include "mimic.h"

#define sread(dest, size, count)					\
	rd = fread(dest, size, count, stream);				\
	if (rd != count) {						\
		debug("fread failed (%d, %d)", (int)rd, (int)count); }	\

#define type_read(type)			\
	type val;			\
	sread(&val, sizeof(val), 1);	\
	return val;			\

static uint64_t read_uleb128();

/* Used by sread */
size_t rd = 0;
FILE *stream = NULL;

void set_stream(FILE *new_stream)
{
	stream = new_stream;
}

BYTE read_byte()
{
	type_read(BYTE);
}

int16_t read_int16()
{
	type_read(int16_t);
}

int32_t read_int32()
{
	type_read(int32_t);
}

int64_t read_int64()
{
	type_read(int64_t);
}

/* These are really just C# strings */
size_t read_osu_string(char *out_buffer)
{
	BYTE indicator;
	sread(&indicator, sizeof(BYTE), 1);

	if (indicator != 0x0b) {
		debug("string indicator set to invalid value");

		return 0;
	}

	uint64_t len = read_uleb128();

	for (size_t i = 0; i < len; i++) {
		sread(out_buffer++, sizeof(char), 1);
	}

	*out_buffer = '\0';

	return len;
}

/* Taken from Section 7.6 of the DWARF 3 spec, Appendix C */
static uint64_t read_uleb128()
{
	BYTE by = 0;
	int shift = 0;
	uint64_t result = 0;

	while (1) {
		sread(&by, sizeof(by), 1);

		result |= ((by & 0x7f) << shift);

		if (!(by & 0x80))
			break;

		shift += 7;
	}

	return result;
}

/* Just keeping these around in case they are ever needed again */
#ifdef NULL

void u16_switch_endian(uint16_t *x)
{
	*x = (*x >> 8) | (*x << 8);
}

void u32_switch_endian(uint32_t *x)
{
	*x = (*x >> 24) | 
		((*x << 8) & 0x00FF0000) |
		((*x >> 8) & 0x0000FF00) |
		(*x << 24);
}

#endif
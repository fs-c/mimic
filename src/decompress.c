/* Most of the code in this file was taken with minimal modifications from
 * https://github.com/lloyd/easylzma/blob/master/test/simple.c
 */

#include "mimic.h"
#include "easylzma/decompress.h"

static int input_callback(void *ctx, void *buf, size_t *size);
static size_t output_callback(void *ctx, const void *buf, size_t size);

struct data_stream
{
	BYTE *in_data;
	size_t in_len;

	BYTE *out_data;
	size_t out_len;
};

int decompress_basic(BYTE *in_data, size_t in_len, BYTE **out_data,
	size_t *out_len)
{
	int rc;
	elzma_decompress_handle hand;

	hand = elzma_decompress_alloc();

	struct data_stream ds;
	ds.in_data = in_data;
	ds.in_len = in_len;
	ds.out_data = NULL;
	ds.out_len = 0;

	rc = elzma_decompress_run(hand, input_callback, (void *)&ds,
		output_callback, (void *)&ds, ELZMA_lzma);
	
	elzma_decompress_free(&hand);	

	if (rc != ELZMA_E_OK) {
		debug("elzma_decompress_run error: %d", rc);

		if (ds.out_data != NULL)
			free(ds.out_data);

		return rc;
	}

	*out_data = ds.out_data;
	*out_len = ds.out_len;

	return rc;
}

static int input_callback(void *ctx, void *buf, size_t *size)
{
	size_t rd = 0;
	struct data_stream *ds = (struct data_stream *)ctx;

	rd = (ds->in_len < *size) ? ds->in_len : *size;

	if (rd > 0) {
		memcpy(buf, (void *)ds->in_data, rd);
		ds->in_data += rd;
		ds->in_len -= rd;
	}

	*size = rd;

	return 0;
}

static size_t output_callback(void *ctx, const void *buf, size_t size)
{
	struct data_stream *ds = (struct data_stream *)ctx;

	if (size > 0) {
		ds->out_data = realloc(ds->out_data, ds->out_len + size);
		memcpy((void *)(ds->out_data + ds->out_len), buf, size);
		ds->out_len += size;
	}

	return size;
}

/*
 * this code brute-forces an adler32 checksum
 *
 * there's probably some way to analytically derive solutions, but i'm too dumb
 * to figure it out.
 * */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

struct adler_data {
	size_t offset_count;
	uint32_t counts[4];
	uint32_t s1;
	uint32_t s2;
};

static void make_adler_data(FILE *in, struct adler_data *ret,
		size_t *offsets, size_t offsets_len) {
	uint32_t vel[4] = {0};

	memset(ret->counts, 0, sizeof(ret->counts));
	ret->s1 = 1;
	ret->s2 = 0;
	size_t offset_idx = 0;
	size_t curr_byte = 0;

	for (;;) {
		int c = fgetc(in);
		if (c == EOF) {
			break;
		}

		if (offset_idx+1 < offsets_len &&
				curr_byte == offsets[offset_idx+1]) {
			++offset_idx;
		}

		size_t chksum_offset = curr_byte - offsets[offset_idx];
		if (chksum_offset < 4) {
			vel[chksum_offset] += 1;
			vel[chksum_offset] %= 65521;
		}
		for (int i = 0; i < 4; ++i) {
			ret->counts[i] += vel[i];
			ret->counts[i] %= 65521;
		}
		if (chksum_offset >= 4) {
			ret->s1 += c;
			ret->s1 %= 65521;
		}
		ret->s2 += ret->s1;
		ret->s2 %= 65521;

		++curr_byte;
	}

	ret->offset_count = offsets_len;
}

static uint32_t adler32(uint32_t candidate, struct adler_data *data) {
	uint32_t s1 = ((candidate)       & 0xff) +
	              ((candidate >> 8)  & 0xff) +
	              ((candidate >> 16) & 0xff) +
	              ((candidate >> 24) & 0xff);
	s1 *= data->offset_count;
	s1 += data->s1;
	s1 %= 65521;

	uint32_t s2 = data->s2;
	s2 += ((candidate)       & 0xff) * data->counts[3];
	s2 += ((candidate >> 8)  & 0xff) * data->counts[2];
	s2 += ((candidate >> 16) & 0xff) * data->counts[1];
	s2 += ((candidate >> 24) & 0xff) * data->counts[0];
	s2 %= 65521;

	return (s2 << 16) | s1;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s [file] [offset 1] [offset 2] ...\n",
				argv[0]);
		return 1;
	}

	FILE *f = fopen(argv[1], "rb");
	if (f == NULL) {
		fprintf(stderr, "Failed to open file %s for reading\n",
				argv[1]);
	}

	size_t *offsets = alloca((argc - 2) * sizeof(size_t));
	for (int i = 2; i < argc; ++i) {
		offsets[i-2] = atol(argv[i]);
	}

	struct adler_data d;
	make_adler_data(f, &d, offsets, argc-2);

	uint32_t i = 0;
	do {
		if ((i & 0xffffff) == 0) {
			fprintf(stderr, "%u\n", i >> 24);
		}
		if (adler32(i, &d) == i) {
			printf("%08x\n", i);
			break;
		}
		++i;
	} while (i != 0);

	return 0;
}

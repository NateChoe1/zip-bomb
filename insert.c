#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
	if (argc != 5) {
		fprintf(stderr, "Usage: %s [input file] [byte offset] [target size] [byte]\n", argv[0]);
		return 1;
	}

	FILE *in = fopen(argv[1], "rb");
	if (in == NULL) {
		fputs("Failed to open file\n", stderr);
		return 1;
	}

	long size;
	if (fseek(in, 0, SEEK_END) == -1) {
		fputs("fseek() failed\n", stderr);
	}
	if ((size = ftell(in)) == -1) {
		fputs("ftell() failed\n", stderr);
	}
	if (fseek(in, 0, SEEK_SET) == -1) {
		fputs("fseek() failed\n", stderr);
	}

	long offset = atol(argv[2]);
	long target = atol(argv[3]);
	int byte = atoi(argv[4]);
	long insert = target - size;

	for (long i = 0; i < size; ++i) {
		int c = fgetc(in);
		if (c == EOF) {
			fputs("Early EOF!\n", stderr);
			return 1;
		}
		if (i == offset) {
			for (long j = 0; j < insert; ++j) {
				putchar(byte);
			}
		}
		putchar(c);
	}
	return 0;
}

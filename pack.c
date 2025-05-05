#include <stdio.h>
#include <stdint.h>

static uint8_t curr_byte;
static size_t bytes_written;
static int bits_read;
static int num_reports;

static enum {
	CANON,
	DEFLATE,
} curr_mode;

static uint8_t flip(uint8_t b);
static int copy(FILE *in, FILE *out);
static void waste(FILE *in);
static void write_bit(int b, FILE *out);
static void write_byte(FILE *out);
static void pad(FILE *out);

int main(int argc, char **argv) {
	FILE *in, *out;
	if (argc >= 2) {
		in = fopen(argv[1], "r");
	} else {
		in = stdin;
	}

	if (argc >= 3) {
		out = fopen(argv[2], "w");
	} else {
		out = stdout;
	}

	if (in == NULL || out == NULL) {
		return 1;
	}

	curr_byte = 0;
	bits_read = 0;
	curr_mode = CANON;
	num_reports = 0;
	bytes_written = 0;

	for (;;) {
		if (copy(in, out)) {
			break;
		}
	}
	if (bits_read != 0) {
		write_byte(out);
	}
	return 0;
}

static uint8_t flip(uint8_t b) {
	uint8_t ret = 0;
	for (int i = 0; i < 8; ++i) {
		ret <<= 1;
		ret |= b & 1;
		b >>= 1;
	}
	return ret;
}

static int copy(FILE *in, FILE *out) {
	int c = fgetc(in);
	ungetc(c, in);
	switch (c) {
	case '0': case '1':
		for (;;) {
			int c = fgetc(in);
			if (c != '0' && c != '1') {
				ungetc(c, in);
				break;
			}
			write_bit(c - '0', out);
		}
		break;
	case 'D':
		pad(out);
		curr_mode = DEFLATE;
		break;
	case 'C':
		pad(out);
		curr_mode = CANON;
		break;
	case 'P':
		pad(out);
		break;
	case 'R':
		fprintf(stderr, "Report #%d:\n", ++num_reports);
		fprintf(stderr, "  writing byte: %zu\n", bytes_written);
		fprintf(stderr, "  bit offset: %d\n", bits_read);
		break;
	case EOF:
		return 1;
	default:
		break;
	}
	waste(in);
	return 0;
}

static void waste(FILE *in) {
	for (;;) {
		int c = fgetc(in);
		if (c == '\n' || c == EOF) {
			return;
		}
	}
}

static void write_bit(int b, FILE *out) {
	curr_byte |= b << (bits_read++);
	if (bits_read == 8) {
		write_byte(out);
	}
}

static void write_byte(FILE *out) {
	bits_read = 0;
	uint8_t v;
	if (curr_mode == DEFLATE) {
		v = curr_byte;
	} else {
		v = flip(curr_byte);
	}
	curr_byte = 0;
	fputc(v, out);
	++bytes_written;
}

static void pad(FILE *out) {
	if (bits_read == 0) {
		return;
	}
	write_byte(out);
}

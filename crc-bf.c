/*
 * this code does some interesting computations with crcs, which have
 * historically only been done with brute force.
 *
 * pretty much all of these operations are done over GF(2^32), using the crc32
 * polynomial as a modulus. polynomials are stored as uint64_t's in big endian
 * format, so the lsb corresponds to 1 and the msb corresponds to x^63.
 * */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MODULUS 0x104c11db7lu

/* the highest possible bit set in a uint64_t */
#define HIGH_BIT 0x8000000000000000llu

/* multiplies two polynomials */
static uint64_t multiply_raw(uint64_t p1, uint64_t p2);

/* finds the "probe", or the highest set bit of n */
static uint64_t find_probe(uint64_t n);

/* multiplies two polynomials with a known probe, used as a helper for multiply
 * */
static uint64_t multiply_with_probe(uint64_t p1, uint64_t p2,
		uint64_t mod, uint64_t probe);

/* multiplies two polynomials, modulus MODULUS */
static uint64_t multiply(uint64_t p1, uint64_t p2, uint64_t mod);

/* divides two polynomials and calculates their quotient and remainder */
static void divmod_raw(uint64_t dividend, uint64_t divisor,
		uint64_t *quot, uint64_t *rem);

/* calculates the Bezout coefficients of two polynomials */
static void xgcd(uint64_t p1, uint64_t p2,
		uint64_t *c1, uint64_t *c2, uint64_t *gcf);

/* calculates the multiplicative inverse of a polynomial with a modulus */
static uint64_t invert(uint64_t p, uint64_t mod);

/* divides two polynomials with a modulus */
static uint64_t divide(uint64_t dividend, uint64_t divisor, uint64_t mod);

/* reverses the bit order of v */
static uint8_t reverse_bits8(uint8_t v);

/* reverses the bit order in each byte of v */
static uint32_t reverse_bytes(uint32_t v);

/* finds a "fixed point". if the current crc polynomial is f(x), and we see a
 * character c(x), the new crc polynomial should still be f(x). this function
 * calculates f(x) from c(x). */
static int fixed_point(int argc, char **argv);

/* given a file, a target value, and some offsets, this calculates a 4 byte word
 * such that replacing each offset with that word causes the crc to be equal to
 * the target value. */
static int force_crc(int argc, char **argv);

static uint64_t multiply_raw(uint64_t p1, uint64_t p2) {
	uint64_t ret = 0;

	for (int bit = 0; bit < 64; ++bit) {
		if (p2 & (1llu << bit)) {
			ret ^= p1 << bit;
		}
	}
	return ret;
}

static uint64_t find_probe(uint64_t n) {
	uint64_t ret = HIGH_BIT;
	if (n == 0) {
		fputs("Trying to find probe of 0, exiting\n", stderr);
		exit(EXIT_FAILURE);
	}
	while (!(ret & n)) {
		ret >>= 1;
	}
	return ret;
}

static uint64_t multiply_with_probe(uint64_t p1, uint64_t p2,
		uint64_t mod, uint64_t probe) {
	if (p2 == 0) {
		return 0;
	}

	uint64_t child = multiply_with_probe(p1, p2 >> 1, mod, probe) << 1;
	if (child & probe) {
		child ^= mod;
	}

	if (p2 & 1) {
		child ^= p1;
	}

	return child;
}

static uint64_t multiply(uint64_t p1, uint64_t p2, uint64_t mod) {
	return multiply_with_probe(p1, p2, mod, find_probe(mod));
}

static void divmod_raw(uint64_t dividend, uint64_t divisor,
		uint64_t *quot, uint64_t *rem) {
	uint64_t probe = find_probe(dividend);

	if (find_probe(divisor) > probe) {
		*quot = 0;
		*rem = dividend;
		return;
	}

	int bit = 0;
	while (!((divisor << bit) & probe)) {
		++bit;
	}

	uint64_t q = 0;
	uint64_t r = dividend;
	while (bit >= 0) {
		if (r & probe) {
			uint64_t positioned = divisor << bit;
			r ^= positioned;
			q |= 1llu << bit;
		}

		probe >>= 1;
		--bit;
	}

	*quot = q;
	*rem = r;
}

static void xgcd(uint64_t p1, uint64_t p2,
		uint64_t *c1, uint64_t *c2, uint64_t *gcf) {
	uint64_t probe1, probe2;

	if (p2 == 0) {
		*gcf = p1;
		*c1 = 1;
		*c2 = 0;
		return;
	}

	probe1 = find_probe(p1);
	probe2 = find_probe(p2);
	if (probe1 < probe2) {
		xgcd(p2, p1, c2, c1, gcf);
		return;
	}

	uint64_t ch1, ch2;
	uint64_t quot, mod;
	divmod_raw(p1, p2, &quot, &mod);
	xgcd(p2, mod, &ch1, &ch2, gcf);

	if (mod == *gcf) {
		*c1 = 1;
		*c2 = quot;
		return;
	}

	*c1 = ch2;
	*c2 = ch1 ^ multiply_raw(ch2, quot);
}

static uint64_t invert(uint64_t p, uint64_t mod) {
	uint64_t c1, c2, gcf;

	xgcd(p, mod, &c1, &c2, &gcf);

	if (gcf != 1) {
		fprintf(stderr, "Can't find multiplicative inverse of %lb\n",
				p);
		exit(EXIT_FAILURE);
	}

	/* note that for polynomials of degree p>2, we would have to normalize
	 * as the gcd may be any polynomial of degree zero, and not necessarily
	 * 1. luckily, when p=2 there's no other possible value it could be. */

	uint64_t quot, rem;
	divmod_raw(c1, mod, &quot, &rem);
	return rem;
}

static uint64_t divide(uint64_t dividend, uint64_t divisor, uint64_t mod) {
	return multiply(dividend, invert(divisor, mod), mod);
}

static uint8_t reverse_bits8(uint8_t v) {
	uint8_t ret = 0;
	for (int i = 0; i < 8; ++i) {
		ret <<= 1;
		ret |= v & 1;
		v >>= 1;
	}
	return ret;
}

static uint32_t reverse_bytes(uint32_t v) {
	return ((uint32_t) reverse_bits8(v & 0xff)) |
	       ((uint32_t) reverse_bits8((v >> 8) & 0xff))  << 8 |
	       ((uint32_t) reverse_bits8((v >> 16) & 0xff)) << 16 |
	       ((uint32_t) reverse_bits8((v >> 24) & 0xff)) << 24;
}

static int fixed_point(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s [byte value]\n", argv[0]);
		return 1;
	}

	char *e;
	long cl = strtol(argv[1], &e, 10);
	if (e == NULL || cl < 0 || cl >= 256) {
		fprintf(stderr, "Invalid byte value %s\n", argv[1]);
		return 1;
	}

	uint64_t c = reverse_bits8(cl);
	uint64_t f = divide(multiply(c, 1lu << 32, MODULUS), 257, MODULUS);

	printf("%08lx\n", f);
	return 0;
}

static int force_crc(int argc, char **argv) {
	int ret;

	if (argc < 4) {
		fprintf(stderr,
			"Usage: %s [file] [target] [offset 1] [offset 2] ...\n",
			argv[0]);
		return 1;
	}

	FILE *file = fopen(argv[1], "rb");
	if (file == NULL) {
		fprintf(stderr, "Failed to open file %s\n", argv[1]);
		return 1;
	}

	long target;
	char *e;
	target = strtol(argv[2], &e, 16);
	if (e == NULL || *e != '\0' || target < 0 || target > 0xffffffff) {
		fprintf(stderr, "Invalid target %s\n", argv[2]);
		ret = 1;
		goto end;
	}

	size_t offsets_len = argc - 3;
	size_t *offsets = alloca(offsets_len * sizeof(size_t));

	for (int i = 3; i < argc; ++i) {
		long offset = strtol(argv[i], &e, 10);
		if (e == NULL || offset < 0) {
			fprintf(stderr, "Invalid offset %s\n", argv[i]);
			ret = 1;
			goto end;
		}
		offsets[i-3] = offset;
	}

	size_t len = 0;
	size_t next_stop = 0;
	int curr_offset = 0;
	uint64_t msg_p;
	uint64_t offsets_p;

	msg_p = 0xffffffff;
	offsets_p = 0;

	for (;;) {
		int c = fgetc(file);
		if (c == EOF) {
			break;
		}

		if (len < next_stop) {
			goto next;
		}

		if (len == offsets[curr_offset]) {
			msg_p = multiply(msg_p, 1lu << 32, MODULUS);
			offsets_p ^= 1;
			offsets_p = multiply(offsets_p, 1lu << 32, MODULUS);
			next_stop = len+4;
			++curr_offset;
			goto next;
		}

		offsets_p = multiply(offsets_p, 1lu << 8, MODULUS);
		for (int i = 0; i < 8; ++i) {
			if (c & (1 << i)) {
				msg_p ^= 1lu << 31;
			}
			msg_p = multiply(msg_p, 2, MODULUS);
		}

next:
		++len;
	}

	uint64_t x;
	x = divide(((uint64_t) target) ^ msg_p, offsets_p, MODULUS);
	uint32_t native = reverse_bytes((uint32_t) x);
	printf("%02x %02x %02x %02x\n",
			(native >> 24) & 0xff,
			(native >> 16) & 0xff,
			(native >> 8) & 0xff,
			(native) & 0xff);

	ret = 0;
end:
	fclose(file);
	return ret;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s [command] [arguments]\n", argv[0]);
		return 1;
	}

	if (strcmp(argv[1], "fixed-point") == 0) {
		return fixed_point(argc-1, argv+1);
	} else if (strcmp(argv[1], "force-crc") == 0) {
		return force_crc(argc-1, argv+1);
	}

	fprintf(stderr, "Unrecognized command %s\n", argv[1]);
	return 1;
}

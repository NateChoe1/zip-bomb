#include <stdio.h>
#include <stdint.h>

int main(void) {
	uint32_t checksum = 0xffffffff;
	for (;;) {
		int c = getchar();
		if (c == EOF) {
			break;
		}
		checksum ^= c;
		for (int i = 0; i < 8; ++i) {
			uint32_t mask = (checksum & 1) ? 0xedb88320 : 0;
			checksum >>= 1;
			checksum ^= mask;
		}
	}
	checksum = ~checksum;
	printf("%02x%02x%02x%02x\n",
			(checksum)       & 0xff,
			(checksum >> 8)  & 0xff,
			(checksum >> 16) & 0xff,
			(checksum >> 24) & 0xff);
	return 0;
}

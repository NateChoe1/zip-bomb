#include <stdio.h>

int main() {
	int s1 = 1;
	int s2 = 0;
	for (;;) {
		int c = getchar();
		if (c == EOF) {
			break;
		}
		s1 += c;
		s1 %= 65521;
		s2 += s1;
		s2 %= 65521;
	}

	printf("%016b %016b\n", s2, s1);
	return 0;
}

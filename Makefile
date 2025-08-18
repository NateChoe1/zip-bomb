all: adler-bf adler32 crc-bf pack insert

%: %.c
	$(CC) -g -Wall -Wextra $< -o $@

all: adler-bf adler32 pack insert

%: %.c
	$(CC) $< -o $@

all: adler32 pack insert

%: %.c
	$(CC) $< -o $@

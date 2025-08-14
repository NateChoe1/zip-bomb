m4_include(`macros.m4')

C                     zlib: header
0001                  window size, 512 bytes
1000                  compression method, deflate

00                    compression level, fast algorithm
0                     fdict, no dictionary
11001                 checksum

D                     deflate: data start

L_b(0)                deflate: nonce blocks to increase the header length. we
L_b(0)                need these because later Rh+1 has to take up exactly 5
                      bytes. to make that possible, we insert these nonce bytes
                      in our compressed stream and treat them as part of our
                      header. if we didn't have these, the header would be too
                      short to take up 5 whole bytes of compressed data.

m4_define(`hlen', `12')     the header is 12 bytes
m4_define(`hlen_1', `17')   the header length plus one is 17 bytes

LL_b(hlen_1)          deflate: Lh+1 ..H.. Lh+1
C                     ..H..
00001000
00011101
D
L_b(0)
L_b(0)
LL_b(hlen_1)          Lh+1

Rh_1_b                deflate: Rh+1

L_b(1)                deflate: L1 Rh+1
Rh_1_b

L_b(1)                deflate: L1 L1
L_b(1)

L_b(4)                deflate: L4 Rh+1 L1 L1 L4
Rh_1_b
L_b(1)
L_b(1)
L_b(4)

R4_b                  deflate: R4

L_b(4)                deflate: L4 R4 L4 R4 L4
R4_b
L_b(4)
R4_b
L_b(4)

R4_b                  deflate: R4

m4_define(`bomb_start', `dnl   stolen from the primitive bomb
L_b(0)

0                     deflate: very large block
01                    compressed with dynamic huffman codes
10111                 number of literal/length codes (286)
00000                 number of distance codes (1)
0111                  number of code length codes (18)
000                   code length for 16 (0)
000                   code length for 17 (0)
010                   code length for 18 (2) ***
000                   code length for 0  (0)
010                   code length for 8  (2) *unused
010                   code length for 7  (2) *unused
000                   code length for 9  (0)
000                   code length for 6  (0)
000                   code length for 10 (0)
000                   code length for 5  (0)
000                   code length for 11 (0)
000                   code length for 4  (0)
000                   code length for 12 (0)
000                   code length for 3  (0)
000                   code length for 13 (0)
000                   code length for 2  (0)
000                   code length for 14 (0)
010                   code length for 1  (2) ***

                      deflate: literal/length alphabet
                      note: our code lengths are:
                      1: 1
                      18: 1
                      so our alphabet is:
                      1: 0
                      18: 1

                      a bunch of zeros to get to code number 256 (end of block)
                      (code 0)
11                    138 zeros
1111111

                      (code 138)
11                    118 zeros
1101011
                      (code 256)
00                    code 256 has bit length 1
                      (code 257)
11                    28 zeros
1000100
                      (code 285)
00                    code 285 has bit length 1

                      deflate: end of literal/length alphabet

                      deflate: distance alphabet
00                    distance length 0 has bit length 1
                      deflate: end of distance alphabet

                      at this point, our literal/length alphabet has these bit
                      lengths:

                      256: 1
                      285: 1

                      and these encodings

                      256: 0
                      285: 1

                      and our distance alphabet has this bit length

                      0: 1

                      and this encoding:

                      0: 0

10101010              begin data. at this point we are byte aligned, so we just
                      throw in a bunch of 10101010 bytes.
')

m4_define(`bomb_header_len', `19')
m4_define(`bomb_header_len_1', `26')

L_b(4)                deflate: L4 R4 L0 L0 Lt+1
R4_b
L_b(0)
L_b(0)
LL_b(bomb_header_len_1)

R4_b                  deflate: R4

L_b(0)                deflate: L0

L_b(0)                deflate: L0

LL_b(bomb_header_len_1)  deflate: Lt+1 Rt+1 ..T..
R26l7_b
bomb_start

R26l7_b               deflate: Rt+1

bomb_start            bomb data
R
 10101010             we need to insert 51793 repetitions of this byte
		      this specific number keeps the adler32 checksum constant
                      across decompression layers.

m4_define(`hlen2', 15)   this is the second header. it closes off the bomb and
                         adds some extra padding bytes.
m4_define(`tlen', `14')
m4_define(`tlen_1', `19')
L_b(0)                since `L_b(0)' starts with a 0 bit, this closes the bomb
L_b(0)
L_b(0)


L_b(4)                Lh+1 ..H.. Lh+1
L_b(0)
L_b(0)
L_b(0)
L_b(4)

R4_b                  Rh+1

L_b(1)                L1 Rh+1
R4_b

L_b(1)                L1 L1
L_b(1)

L_b(4)                L4 Rh+1 L1 L1 L4
R4_b
L_b(1)
L_b(1)
L_b(4)

R4_b                  R4

L_b(4)                L4 R4 L4 R4 L4
R4_b
L_b(4)
R4_b
L_b(4)

R4_b                  R4

L_b(4)                L4 R4 L0 L0 Lt+1
R4_b
L_b(0)
L_b(0)
LL_b(tlen_1)

R4_b                  R4

L_b(0)                L0

L_b(0)                L0

LL_b(tlen_1)          Lt+1 Rt+1 ..T..
Rt_1_b
L_b(0)                ..T..
end_b
C
R
0011111101001111      ..T.. adler32 checksum
1100111100010001
D

Rt_1_b                Rt+1

                      begin trailer

L_b(0)                padding
end_b

C                     adler32 checksum
R
0011111101001111      ..T.. adler32 checksum
1100111100010001

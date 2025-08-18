m4_include(`macros.m4')

C                     gzip: header
00011111              ID1
10001011              ID2
00001000              CM (deflate)
00000000              FLG (no flags)
0000000000000000      MTIME (no time available)
0000000000000000
00000010              XFL (max compression, slowest algorithm)
11111111              OS (unknown)

D                     deflate: data start

L_b(0)                deflate: nonce block to increase the header length. see
		      the zlib quine bomb for why this is here. i don't actually
                      know if this is necessary, i just wanted to reuse my r20
                      block.

m4_define(`hlen', `15')     the header is 15 bytes
m4_define(`hlen_1', `20')   the header length plus one is 20 bytes

LL_b(hlen_1)          Lh+1 ..H.. Lh+1
0001111110001011      ..H..
0000100000000000
0000000000000000
0000000000000000
0000001011111111
D
L_b(0)
LL_b(hlen_1)          Lh+1

R20l_b                Rh+1
L_b(1)                L1 Rh+1
R20l_b
L_b(1)                L1 L1
L_b(1)
L_b(4)                L4 Rh+1 L1 L1 L4
R20l_b
L_b(1)
L_b(1)
L_b(4)
R4_b                  R4
L_b(4)                L4 R4 L0 L0 Lt+1
R4_b
L_b(0)
L_b(0)
LL_b(20)

LL_b(20)              Lt+1 Rt+1 ..T..
R20l_b                Rt+1
L_b(0)                ..T..
L_b(0)
LL_b(4)

R20l_b
L_b(0)
L_b(0)
LL_b(4)
R
C
1100111110010001     magic bits to fix the crc at the bomb
0000001111101011
D

m4_define(`bomb_start', `dnl   stolen from the zlib bomb. note that theres an
                               extra bomb byte
L_b(0)
0011011100000011
1000000010000010
0100000000000000
0000000000000000
0000010111111111
1111010110011100
0100000010101010
10101010
')

m4_define(`bomb_header_len', `20')
m4_define(`bomb_header_len_1', `27')

L_b(0)                ..H.. nonce header for the second quine
L_b(0)
L_b(0)
L_b(4)                Lh+1 ..H.. Lh+1
L_b(0)
L_b(0)
L_b(0)
L_b(4)
L_b(1)                L1 Rh+1
R20l_b
L_b(1)                L1 L1
L_b(1)
L_b(4)                L4 Rh+1 L1 L1 L4
R20l_b
L_b(1)
L_b(1)
L_b(4)
R20l_b                R4
L_b(4)                L4 R4 L0 L0 Lt+1
R20l_b
L_b(0)
L_b(0)
LL_b(bomb_header_len_1)
R20l_b                R4
L_b(0)
L_b(0)
LL_b(bomb_header_len_1)  Lt+1 Rt+1 ..T..
R27l7_b
bomb_start
R27l7_b
bomb_start

R

L_b(0)                nonce header that also ends the bomb block
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
LL_b(18)

R4_b                 R4
L_b(0)               L0
L_b(0)               L0
LL_b(18)             Lt+1 Rt+1 ..T..
R18l_b
end_b
R
C
11100000111011111010110101001101    crc
11000000001011111010111011000100    isize
D

R18l_b               Rt+1

                     ..T..
end_b

R
C
11100000111011111010110101001101    crc
11000000001011111010111011000100    isize

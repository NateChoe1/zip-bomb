m4_include(`macros.m4')

C                     zlib: header
0000                  window size, 256 bytes
1000                  compression method, deflate

00                    compression level, fast algorithm
0                     fdict, no dictionary
11101                 checksum

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

m4_define(`tlen', `14')
m4_define(`tlen_1', `19')

L_b(4)                deflate: L4 R4 L0 L0 Lt+1
R4_b
L_b(0)
L_b(0)
LL_b(tlen_1)

R4_b                  deflate: R4

L_b(0)                deflate: L0

L_b(0)                deflate: L0

LL_b(tlen_1)          deflate: Lt+1 Rt+1 ..T..
Rt_1_b
L_b(0)                ..T..
end_b
C
R
1111000011110000      ..T.. adler32 checksum
1111111111111111
D

Rt_1_b                deflate: Rt+1

                      begin trailer


L_b(0)                deflate nonce blocks to pad the trailer
end_b                 see header for why this is necessary

C                     adler32 checksum
R
1111000011110000
1111111111111111

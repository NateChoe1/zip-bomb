  literal byte definitions
m4_define(`end_b', `  nonce with the BFINAL bit set
10000000
0000000000000000
1111111111111111
')dnl

m4_define(`binary_lsbf_norm',
`m4_ifelse(`$2', `0', ,
           `m4_eval(`$1 % 2')binary_lsbf_norm(m4_eval($1` >> 1'), m4_eval($2` - 1'))')')

m4_define(`binary_lsbf_inv',
`m4_ifelse(`$2', `0', ,
           `m4_eval(`1 - $1 % 2')binary_lsbf_inv(m4_eval($1` >> 1'), m4_eval($2` - 1'))')')

m4_define(`LL_b', `m4_dnl  expands to exactly $1 bytes
00000000
binary_lsbf_norm($1, 16)
binary_lsbf_inv($1, 16)
')

m4_define(`L_b', `m4_dnl   expands to $1 * 5 bytes
LL_b(m4_eval(`$1 * 5'))')

m4_define(`R4_b', `
0                     deflate: R4
10                    fixed huffman codes
0001000               length 10
01000110              distance 20
0001000               length 10
01000110              distance 20
0000000               end of data
')
m4_define(`Rh_1_b', `
0                     deflate: Rh+1 (17, 17) == (9, 17) + (8, 17)
10                    fixed huffman codes
0000111               length 9
01000000              distance 17
0000110               length 8
01000000              distance 17
0000000               end of data
')
m4_define(`Rt_1_b', `
0                     deflate: Rt+1 (19 bytes)
10                    fixed huffman codes
0001000               length 10
01000010              distance 19
0000111               length 9
01000010              distance 19
0000000               end of data
')

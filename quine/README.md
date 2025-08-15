# Quine bomb

This is a zlib quine bomb that infinitely expands exponentially based on the
number of decompression layers.

This would not have been possible without an [amazing
article](https://honno.dev/gzip-quine/) by Matthew Barber, which was largely
based on another [amazing article](https://research.swtch.com/zip) by Russ Cox.

## Build instructions

1. Pack

   ```shell
   ./pack bomb-quine.m4 > pack.z
   ```
1. Insert

   ```shell
   ./insert pack.z 216 -51793 85 > output.z
   ```

## Brute forcing the checksum

```shell
./adler-bf output.z 52194 52213
```

## Explanation

### High level explanation

In the gzip quine (see the [other](https://honno.dev/gzip-quine/)
[articles](https://research.swtch.com/zip)), we saw that it's possible to treat
DEFLATE payloads as code, and to write a quine which looks like this:

```
gzip header
magic deflate payload
gzip trailer
```

In the primitive bomb, we also saw that it's possible to create a very large
payload that looks like this:

```
zlib header
  L1 U
Begin dynamic block
  R(258, 1)
  R(258, 1)
  R(258, 1)
  R(258, 1)
  ...  note that each byte contains four repetitions, so we have a 1032x
       compression ratio
End dynamic block
zlib trailer
```

The key insight is that this payload uses two quine payloads:

```
quine 1
  R(258, 1)
  R(258, 1)
  R(258, 1)
  R(258, 1)
  ...
quine 2
```

Where quine 1 is as follows:

```
zlib header
magic deflate payload
  L1 U
begin dynamic block
```

And quine 2 is as follows:

```
end dynamic block
magic deflate payload
zlib trailer
```

We're basically treating the dynamic block headers and trailers as quine
trailers and headers respectively.

### Environment

For the primitive payload, I wrote a very simple bit packer which takes some
plaintext bits and converts them into actual binary. For this more advanced
payload I wanted some higher level abstractions, so I decided to use [m4
macros](https://en.wikipedia.org/wiki/M4_(computer_language)) to represent
higher order concepts like literal headers. This allows constructs like this to
become 5x more concise:

```m4
L_b(4)                deflate: L4 Rh+1 L1 L1 L4
Rh_1_b
L_b(1)
L_b(1)
L_b(4)
```

The macros themselves are defined in `macros.m4`. The general system goes like
this:

```m4
LL_b(n)   - Creates an uncompressed block header of n bytes.
L_b(n)    - Creates an uncompressed block header of 5n bytes. This is useful
            because every other block header is 5 bytes, so for example L_b(4)
            can store 4 other block headers.
Rx_b      - Repeats the last x blocks (5x bytes).
Rxl_b     - Repeats the last x bytes.
Rxly_b    - Repeats the last x bytes, but takes up y bytes.
end_b     - Equivalent to L_b(0) except the BFINAL bit is set.
```

### Checksum

The most theoretically interesting thing here is getting all of the checksums to
match. Zlib uses the Adler32 checksum, which can be implemented in pseudocode
like this:

```
s1 := 1
s2 := 0
for each byte:
    s1 += byte value
    s1 %= 65521
    s2 += s1
    s2 %= 65521
return s2 << 16 | s1
```

The checksum can also be calculated directly with [run-length
encoded](https://en.wikipedia.org/wiki/Run-length_encoding) data like
this:

```
s1 := 1
s2 := 0
for each run-length pair (c, l):
    s2 += s1 * l
    s2 += c * l*(l+1)/2    # https://en.wikipedia.org/wiki/Triangular_number
    s2 %= 65521
    s1 += c * l
    s1 %= 65521
```

Notably, since each of these computations are done modulo 65521, every congruent
run length modulo 65521 is equivalent. More formally, for all natural numbers
`n`, `k`, and for all characters `c`, the run `(c, k)` is equivalent to the run
`(c, k + 65521n)`. Less formally, if you have 65521 repeated copies of a single
character, they all cancel out in the final checksum.

If we have `n` characters in our bomb, the total number of characters after a
decompression is `1032n+1` (the extra 1 comes from the trailer of the previous
quine). We want this number to be congruent to `n` mod 65521.

```
  1032n + 1 = n (mod 65521)
Move terms around
  1031n = 65520 (mod 65521)
Divide both sides by 1031. Note that 1/1031 = 13727 mod 65521, since
1031*13727 = 1 mod 65521. The value 13727 comes from the Bezout coefficients of
1031 and 65521, and can be calculated with the extended Euclidean algorithm.
  n = 65520 * 13727 (mod 65521)
Evaluate
  n = 51794
```

If we put 51794 repeated characters in our bomb, it will decompress into
53451409 bytes, which is equivalent to 51794 mod 65521.

To actually calculate the checksum, I just brute-forced all 4.2 billion possible
values to see which one worked.

### Extra notes

My zlib header uses the "fast" decompression method because the when I used the
"fastest" method, there was no valid checksum.

The dynamic Huffman table in the bomb has 2 bit codes so that the first bit of
the actual payload is byte aligned. In retrospect, I don't think this was
necessary since I could just use different Bezout coefficients while calculating
the checksum.

The `R26l7_b` macro, used to expand the bomb header, is 7 bytes long instead of
5 because I couldn't find a working 40 bit sequence with fixed Huffman tables.

# Quine bomb

This is a zlib quine bomb that infinitely expands exponentially based on the
number of decompression layers.

This would not have been possible without an [amazing
article](https://honno.dev/gzip-quine/) by Matthew Barber.

## Build instructions

1. Unpack

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

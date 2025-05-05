# Pretty large zip bomb

This is a handcrafted, pretty large zip bomb, suitable for tearing down
[malicious web scrapers](https://idiallo.com/blog/zipbomb-protection).

The compressed file is just 1 MiB, but it decompresses to over a terabyte of
data, which is close to optimal with two layers of deflate compression.

## Instructions

1. Compile tools

   ```bash
   make
   ```

2. Build unpadded stage 1

   ```bash
   ./pack stage1.txt stage1.z
   ```

3. Build the rest of stage 1

   ```bash
   ./insert stage1.z 43 1048576 85 > final.z
   ```

4. Verify result

   ```bash
   cat final.z | openssl zlib -d > /dev/null
   cat final.z | openssl zlib -d | openssl zlib -d
   ```

   The first command should NOT output an error

   The second command should give a _very_ long string of `A`s.

## Serving

You can serve final.z with the header `Content-Encoding: deflate, deflate`

## Licensing

I'm donating this to the public domain. Use it however you'd like.

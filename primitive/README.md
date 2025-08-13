# Primitive zip bomb

This is a pretty basic handcrafted zip bomb that decompresses twice from 1048576
bytes to 1116699765055 bytes. I would

## Instructions

1. Build unpadded stage 1

   ```bash
   ./pack stage1.txt stage1.z
   ```

1. Build the rest of stage 1

   ```bash
   ./insert stage1.z 43 1048576 85 > final.z
   ```

1. Verify result

   ```bash
   cat final.z | openssl zlib -d > /dev/null
   cat final.z | openssl zlib -d | openssl zlib -d
   ```

   The first command should NOT output an error

   The second command should give a _very_ long string of `A`s.

## Serving

You can serve final.z with the header `Content-Encoding: deflate, deflate`

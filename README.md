# MRX - A suite of high speed hashing functions

This software includes a suite of high speed non-cryptographic hashing
algorithms utilising various different CPU instruction sets available on
Intel CPUs.

All of the hashing algorithms have been optimised for bulk hashing of large
files but can be used for hashing messages of any length.

All of the algorithms use the same fundamental block based design and use a
Merkle-Damgard style construction by adding an extra bit after the message
and including the message length in the last block for added strength.

All algorithms offer a single call interface as well as a streaming interface
such that data can be fed to the algorithm in multiple chunks of arbitrary
length.

All of these algorithms successfully pass all of the tests from the SMHasher
test suites available at https://github.com/aappleby/smhasher and
https://github.com/demerphq/smhasher.

MRXHash32
  - A 32-bit algorithm using multiple/rotate/xor (hence MRX) operations.

MRXHash64
  - A 64-bit algorithm using multiple/rotate/xor (hence MRX) operations.

MLXHash
  - Uses the 64x64 -> 128 bit mul instruction.  This is significantly slower
    than ordinary 64 bit multiplication but it doesn't need to rotate the
    bits and multiply again to get mixing in both directions.  Overall its
    faster than the MRX variants.

SPBHash
  - Uses only lookup tables and xor to simulate a substitution/permutation
    box.  Very slow but good distribution.  This was originally an experiment
    to use the pext/pdep instructions but it turned out a single lookup table
    was slightly faster.

SSEHash
  - Uses SSE instructions by combining the results of the low and high
    products of 16 bit multiplication and shifting bytes with the alignr
    instruction.  Using 16 bit multiplication was significantly faster than
    32 bit multiplication but the mixing effect is poor so it needs to do
    more work in the final mix to ensure all bits are mixed properly.

SSEHash2
  - Uses 32x32->64 bit multiplication which is slower than 16x16->16 and
    32x32->32 multiplication but it only needs half as many instructions
    since both the low and high orders of the product are provided in the
    one result.

CLMHash
  - Uses the pclmul instruction to perform 64x64->128 bit carry-less
    multiplication.  Very similar to MLXHash but this instruction is a lot
    faster.

AVXHash
  - Similar to SSEHash but doubles the register width for greater parallelism.

AVXHash2
  - Similar to SSEHash2 but doubles the register width for greater parallelism.

AESHash
  - Uses the AES-NI encrypt instruction to mix bits.  Each aesenc call has
    relatively poor mixing but the mixing effect propagates with subsequent
    calls as more input is processed.  The final mix needs to do a few more
    cycles to ensure mixing is complete.

Included in this software suite is a command line tool 'mrx' that can use
any of the above algorithms to generate hashes for a collection of files
and also verify hashes to check integrity.  The command also includes a
benchmarking feature.

Using a reference system of an Intel(R) Core(TM) i7-8650U CPU @ 1.90GHz
the following performance was achieved:

| Name      | Speed GB/s | Bytes/cycle |
|-----------|------------|-------------|
| MRXHash32 |     8.1    |     3.8     |
| MRXHash64 |    16.3    |     7.7     |
| MLXHash   |    27.9    |    13.2     |
| SPBHash   |     0.6    |     0.3     |
| SSEHash   |    21.8    |    10.3     |
| SSEHash2  |    22.8    |    10.8     |
| CLMHash   |    47.2    |    22.3     |
| AVXHash   |    56.5    |    26.7     |
| AVXHash2  |    66.9    |    32.0     |
| AESHash   |    64.6    |    30.6     |

Here is sample benchmark output using 8KB, 1MB, 4MB and 8MB message buffers.

```
$ ./mrx -b -x 8
 mrxhash32:  7834.96  7876.53  7942.40  7922.86  7933.29 -->  7942.40 MB/s (3.76 b/c)
 mrxhash64: 15627.96 15568.76 15282.62 15570.11 15589.10 --> 15627.96 MB/s (7.40 b/c)
   mlxhash: 25772.42 25361.01 25037.85 25014.75 25157.36 --> 25772.42 MB/s (12.20 b/c)
   spbhash:   667.06   669.09   667.44   673.81   671.09 -->   673.81 MB/s (0.32 b/c)
   ssehash: 17095.16 16920.69 16651.32 17250.67 17066.37 --> 17250.67 MB/s (8.17 b/c)
  ssehash2: 19717.96 19098.31 19416.93 19402.47 19414.92 --> 19717.96 MB/s (9.34 b/c)
   clmhash: 35183.67 35399.30 34901.83 34762.65 34706.27 --> 35399.30 MB/s (16.76 b/c)
   avxhash: 27982.59 27485.24 27262.47 27410.22 27621.56 --> 27982.59 MB/s (13.25 b/c)
  avxhash2: 39055.33 39446.11 38824.93 39545.56 39543.30 --> 39545.56 MB/s (18.72 b/c)
   aeshash: 51166.57 50725.92 50676.29 51484.04 50453.85 --> 51484.04 MB/s (24.38 b/c)

$ ./mrx -b -x 1024
 mrxhash32:  8096.45  8099.91  8121.63  8081.09  8092.31 -->  8121.63 MB/s (3.85 b/c)
 mrxhash64: 16341.66 16300.90 16190.74 16240.70 16272.29 --> 16341.66 MB/s (7.74 b/c)
   mlxhash: 27808.23 26977.04 27906.86 26905.14 27815.42 --> 27906.86 MB/s (13.21 b/c)
   spbhash:   680.89   678.84   676.37   665.93   677.87 -->   680.89 MB/s (0.32 b/c)
   ssehash: 21406.51 21316.40 21126.54 21178.43 21324.01 --> 21406.51 MB/s (10.14 b/c)
  ssehash2: 22676.27 22747.76 22827.79 22551.85 22334.07 --> 22827.79 MB/s (10.81 b/c)
   clmhash: 46375.67 46940.16 47138.33 46266.98 46219.06 --> 47138.33 MB/s (22.32 b/c)
   avxhash: 51526.06 54094.48 55757.37 51723.59 51789.00 --> 55757.37 MB/s (26.40 b/c)
  avxhash2: 64277.22 64683.75 65807.57 64072.70 64123.45 --> 65807.57 MB/s (31.16 b/c)
   aeshash: 63959.51 63711.63 63894.94 63702.08 62992.98 --> 63959.51 MB/s (30.28 b/c)

$ ./mrx -b -x 4096
 mrxhash32:  8039.21  8104.40  8159.16  8116.27  8059.39 -->  8159.16 MB/s (3.86 b/c)
 mrxhash64: 16346.09 16019.37 16168.70 16337.95 16310.66 --> 16346.09 MB/s (7.74 b/c)
   mlxhash: 27870.50 27971.48 27936.46 27617.73 27775.85 --> 27971.48 MB/s (13.24 b/c)
   spbhash:   685.83   683.89   676.84   685.31   681.30 -->   685.83 MB/s (0.32 b/c)
   ssehash: 21542.64 21277.45 21427.94 21180.52 21218.88 --> 21542.64 MB/s (10.20 b/c)
  ssehash2: 22657.39 22755.56 22507.44 22376.44 22735.31 --> 22755.56 MB/s (10.77 b/c)
   clmhash: 46048.10 47171.70 46597.21 47225.19 46851.45 --> 47225.19 MB/s (22.36 b/c)
   avxhash: 51804.96 53629.44 51710.87 54192.10 51860.64 --> 54192.10 MB/s (25.66 b/c)
  avxhash2: 66940.65 66826.64 65423.25 63910.99 66677.92 --> 66940.65 MB/s (31.70 b/c)
   aeshash: 63526.28 63363.59 64108.98 62064.40 62725.89 --> 64108.98 MB/s (30.35 b/c)

$ ./mrx -b -x 8192
 mrxhash32:  8072.83  8088.97  8028.13  8040.27  8074.44 -->  8088.97 MB/s (3.83 b/c)
 mrxhash64: 16327.42 16257.88 16118.34 16386.73 16223.30 --> 16386.73 MB/s (7.76 b/c)
   mlxhash: 27536.48 27577.33 27415.38 27213.34 27727.90 --> 27727.90 MB/s (13.13 b/c)
   spbhash:   683.68   682.33   685.02   680.37   683.38 -->   685.02 MB/s (0.32 b/c)
   ssehash: 21826.11 21154.68 21048.23 21298.11 21529.38 --> 21826.11 MB/s (10.33 b/c)
  ssehash2: 22421.39 22147.02 21760.53 22368.30 22677.66 --> 22677.66 MB/s (10.74 b/c)
   clmhash: 46819.66 46680.37 47216.87 47055.57 45625.67 --> 47216.87 MB/s (22.36 b/c)
   avxhash: 51242.55 56551.69 52739.94 54879.08 56251.50 --> 56551.69 MB/s (26.78 b/c)
  avxhash2: 67468.56 67661.88 64095.83 66327.23 66190.75 --> 67661.88 MB/s (32.04 b/c)
   aeshash: 64399.66 63600.38 64693.07 63441.17 62806.60 --> 64693.07 MB/s (30.63 b/c)
...

The software in the suite has only been tested on Intel CPUs that provide
the necessary CPU instruction sets.  Some or all of these algorithms may be
ported to other architectures that support equivalent instructions.  No
specific support has been included for endian conversion so if any of these
algorithms are run on big endian CPUs the hashes produced will be different.
Adding support for endian conversion should be trivial though.


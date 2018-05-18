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

Using a reference system of an Intel(R) Xeon(R) CPU E5-2620 v3 @ 2.40GHz the
following performance was achieved:

| Name      | Speed GB/s | Bytes/cycle |
|-----------|------------|-------------|
| MRXHash32 |     6.0    |     2.5     |
| MRXHash64 |    12.0    |     5.1     |
| MLXHash   |    17.5    |     7.5     |
| SPBHash   |     0.3    |     0.14    |
| SSEHash   |    17.0    |     7.1     |
| SSEHash2  |    18.5    |     7.8     |
| CLMHash   |    24.5    |    10.3     |
| AVXHash   |    40.0    |    16.8     |
| AVXHash2  |    49.0    |    20.6     |
| AESHash   |    49.0    |    20.6     |

Here is sample benchmark output using 8KB, 1MB and 4MB message buffers.

```
$ ./mrx -b -x 8
 mrxhash32:  5685.44  6009.34  6006.41  6006.76  5989.87 -->  6009.34 MB/s (2.51 b/c)
 mrxhash64: 11621.10 11652.66 11639.22 11606.84 11614.51 --> 11652.66 MB/s (4.87 b/c)
   mlxhash: 16601.01 16611.70 16571.14 15911.58 16636.52 --> 16636.52 MB/s (6.95 b/c)
   spbhash:   322.48   323.48   322.54   323.48   322.99 -->   323.48 MB/s (0.14 b/c)
   ssehash: 13744.86 13800.08 13785.56 13799.02 13773.91 --> 13800.08 MB/s (5.76 b/c)
  ssehash2: 15674.39 15569.50 15610.10 15607.99 15617.76 --> 15674.39 MB/s (6.55 b/c)
   clmhash: 18574.52 18613.30 18642.01 18604.70 18605.52 --> 18642.01 MB/s (7.79 b/c)
   avxhash: 21032.91 21011.63 20996.01 20995.38 20995.85 --> 21032.91 MB/s (8.78 b/c)
  avxhash2: 27863.55 27894.68 27902.65 27909.87 27909.81 --> 27909.87 MB/s (11.66 b/c)
   aeshash: 35366.90 35547.29 35623.70 35557.43 35622.66 --> 35623.70 MB/s (14.88 b/c)

$ ./mrx -b -x 1024
 mrxhash32:  5879.03  6186.39  6183.19  6212.28  6187.80 -->  6212.28 MB/s (2.59 b/c)
 mrxhash64: 12366.58 12353.59 12402.65 12349.51 12387.23 --> 12402.65 MB/s (5.18 b/c)
   mlxhash: 16985.33 17998.45 17968.92 17985.43 17905.13 --> 17998.45 MB/s (7.52 b/c)
   spbhash:   328.78   326.53   323.33   328.84   327.17 -->   328.84 MB/s (0.14 b/c)
   ssehash: 17144.39 17160.79 17171.18 17139.63 17191.89 --> 17191.89 MB/s (7.18 b/c)
  ssehash2: 18624.64 18637.20 18673.79 17814.89 17794.88 --> 18673.79 MB/s (7.80 b/c)
   clmhash: 24185.90 24642.67 24659.67 24683.41 24698.64 --> 24698.64 MB/s (10.31 b/c)
   avxhash: 40082.87 39467.88 40204.51 40156.17 40181.91 --> 40204.51 MB/s (16.79 b/c)
  avxhash2: 48494.58 47034.67 47194.61 49286.75 49079.49 --> 49286.75 MB/s (20.58 b/c)
   aeshash: 49246.84 49378.82 49343.89 49303.49 49299.76 --> 49378.82 MB/s (20.62 b/c)

$ ./mrx -b -x 4096
 mrxhash32:  5832.25  6183.87  6178.56  6187.86  6181.50 -->  6187.86 MB/s (2.58 b/c)
 mrxhash64: 12368.84 12331.04 12330.62 12345.63 12352.24 --> 12368.84 MB/s (5.17 b/c)
   mlxhash: 17885.77 17922.70 17925.01 17939.00 17965.64 --> 17965.64 MB/s (7.50 b/c)
   spbhash:   330.02   330.08   329.95   330.00   329.07 -->   330.08 MB/s (0.14 b/c)
   ssehash: 17170.76 17134.27 17184.45 17140.64 17142.80 --> 17184.45 MB/s (7.18 b/c)
  ssehash2: 18585.41 18638.09 18570.45 18651.51 18586.13 --> 18651.51 MB/s (7.79 b/c)
   clmhash: 24779.39 24692.48 24665.50 24700.58 24643.24 --> 24779.39 MB/s (10.35 b/c)
   avxhash: 40323.77 40261.59 40208.25 40202.01 40296.67 --> 40323.77 MB/s (16.84 b/c)
  avxhash2: 49299.64 49368.56 49380.14 49287.00 49257.81 --> 49380.14 MB/s (20.62 b/c)
   aeshash: 49413.29 49297.22 49316.43 49320.14 49418.86 --> 49418.86 MB/s (20.64 b/c)
```

The software in the suite has only been tested on Intel CPUs that provide
the necessary CPU instruction sets.  Some or all of these algorithms may be
ported to other architectures that support equivalent instructions.  No
specific support has been included for endian conversion so if any of these
algorithms are run on big endian CPUs the hashes produced will be different.
Adding support for endian conversion should be trivial though.


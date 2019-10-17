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

MRXHash32 (32-bit)
  - A 32-bit algorithm using multiple/rotate/xor (hence MRX) operations.

MRXHash64 (64-bit)
  - A 64-bit algorithm using multiple/rotate/xor (hence MRX) operations.

MLXHash (64-bit)
  - Uses the 64x64 -> 128 bit mul instruction.  This is significantly slower
    than ordinary 64 bit multiplication but it doesn't need to rotate the
    bits and multiply again to get mixing in both directions.  Overall its
    faster than the MRX variants.

MLXHash2 (256-bit)
  - Similar to MLXHash but mixes across the state to provide a 256-bit hash.

SPBHash (64-bit)
  - Uses only lookup tables and xor to simulate a substitution/permutation
    box.  Very slow but good distribution.  This was originally an experiment
    to use the pext/pdep instructions but it turned out a single lookup table
    was slightly faster.

SSEHash (128-bit)
  - Uses SSE instructions by combining the results of the low and high
    products of 16 bit multiplication and shifting bytes with the alignr
    instruction.  Using 16 bit multiplication was significantly faster than
    32 bit multiplication but the mixing effect is poor so it needs to do
    more work in the final mix to ensure all bits are mixed properly.

SSEHash2 (128-bit)
  - Uses 32x32->64 bit multiplication which is slower than 16x16->16 and
    32x32->32 multiplication but it only needs half as many instructions
    since both the low and high orders of the product are provided in the
    one result.

CLMHash (128-bit)
  - Uses the pclmul instruction to perform 64x64->128 bit carry-less
    multiplication.  Very similar to MLXHash but this instruction is a lot
    faster.

CLMHash2 (1024-bit)
  - Similar to CLMHash but mixes across the full state to provide a 1024-bit
    hash.

AVXHash (128-bit)
  - Similar to SSEHash but doubles the register width for greater parallelism.

AVXHash2 (128-bit)
  - Similar to SSEHash2 but doubles the register width for greater parallelism.

AVXHash3 (256-bit)
  - Similar to AVXHash2 but faster and uses the permute instruction to mix
    across lanes and provide a 256-bit hash.

AESHash (128-bit)
  - Uses the AES-NI encrypt instruction to mix bits.  Each aesenc call has
    relatively poor mixing but the mixing effect propagates with subsequent
    calls as more input is processed.  The final mix needs to do a few more
    cycles to ensure mixing is complete.

AESHash2 (1024-bit)
  - Similar to AESHash but also uses the aesenc instruction to mix across the
    full state to provide a 1024-bit hash.

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
| MLXHash   |    29.2    |    13.8     |
| MLXHash2  |    22.4    |    10.6     |
| SPBHash   |     0.6    |     0.3     |
| SSEHash   |    22.7    |    10.8     |
| SSEHash2  |    25.5    |    12.1     |
| CLMHash   |    49.9    |    23.6     |
| CLMHash2  |    45.3    |    21.5     |
| AVXHash   |    57.8    |    27.4     |
| AVXHash2  |    68.9    |    32.6     |
| AVXHash3  |    73.3    |    34.7     |
| AESHash   |    66.4    |    31.4     |
| AESHash2  |    33.1    |    15.7     |

The software in the suite has only been tested on Intel CPUs that provide
the necessary CPU instruction sets.  Some or all of these algorithms may be
ported to other architectures that support equivalent instructions.  No
specific support has been included for endian conversion so if any of these
algorithms are run on big endian CPUs the hashes produced will be different.
Adding support for endian conversion should be trivial though.


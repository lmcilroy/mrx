CC=gcc
CFLAGS=-O3 -march=native -fno-tree-vectorize -fno-tree-slp-vectorize -fPIC -funroll-loops -Wall -Werror -Wcast-align -Wstrict-overflow -Wstrict-aliasing -Wextra -pedantic -Wshadow

all:	mrx

mrx:	mrx.o mrx_hash32.o mrx_hash64.o mlx_hash.o mlx_hash2.o mlx_hash3.o mlx_hash4.o spb_hash.o aes_hash.o aes_hash2.o sse_hash.o sse_hash2.o sse_hash3.o clm_hash.o clm_hash2.o avx_hash.o avx_hash2.o avx_hash3.o avx_hash4.o

mrx.o:	mrx.c mrx_hash32.h mrx_hash64.h mlx_hash.h mlx_hash2.h mlx_hash3.h mlx_hash4.h spb_hash.h mem.h aes_hash.h aes_hash2.h sse_hash.h sse_hash2.h sse_hash3.h clm_hash.h clm_hash2.h avx_hash.h avx_hash2.h avx_hash3.h avx_hash4.h

mrx_hash32.o: mrx_hash32.c mrx_hash32.h mem.h

mrx_hash64.o: mrx_hash64.c mrx_hash64.h mem.h

mlx_hash.o: mlx_hash.c mlx_hash.h mem.h

mlx_hash2.o: mlx_hash2.c mlx_hash2.h mem.h

mlx_hash3.o: mlx_hash3.c mlx_hash3.h mem.h

mlx_hash4.o: mlx_hash4.c mlx_hash4.h

spb_hash.o: spb_hash.c spb_hash.h mem.h

aes_hash.o: aes_hash.c aes_hash.h

aes_hash2.o: aes_hash2.c aes_hash2.h

sse_hash.o: sse_hash.c sse_hash.h

sse_hash2.o: sse_hash2.c sse_hash2.h

sse_hash3.o: sse_hash3.c sse_hash3.h

clm_hash.o: clm_hash.c clm_hash.h

clm_hash2.o: clm_hash2.c clm_hash2.h

avx_hash.o: avx_hash.c avx_hash.h

avx_hash2.o: avx_hash2.c avx_hash2.h

avx_hash3.o: avx_hash3.c avx_hash3.h

avx_hash4.o: avx_hash4.c avx_hash4.h

clean:
	rm -f *.o mrx

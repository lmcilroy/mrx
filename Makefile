CFLAGS=-O3 -maes -msse4.2 -mavx2 -mpclmul -fPIC -march=native -funroll-loops -Wall -Werror -Wcast-align -Wstrict-overflow -Wstrict-aliasing -Wextra -pedantic -Wshadow

all:	mrx

mrx:	mrx.o mrx_hash32.o mrx_hash64.o mlx_hash.o spb_hash.o aes_hash.o sse_hash.o sse_hash2.o clm_hash.o avx_hash.o avx_hash2.o

mrx.o:	mrx.c mrx_hash32.h mrx_hash64.h mlx_hash.h spb_hash.h mem.h aes_hash.h sse_hash.h sse_hash2.h clm_hash.h avx_hash.h avx_hash2.h

mrx_hash32.o: mrx_hash32.c mrx_hash32.h mem.h

mrx_hash64.o: mrx_hash64.c mrx_hash64.h mem.h

mlx_hash.o: mlx_hash.c mlx_hash.h mem.h

spb_hash.o: spb_hash.c spb_hash.h mem.h

aes_hash.o: aes_hash.c aes_hash.h

sse_hash.o: sse_hash.c sse_hash.h

sse_hash2.o: sse_hash2.c sse_hash2.h

clm_hash.o: clm_hash.c clm_hash.h

avx_hash.o: avx_hash.c avx_hash.h

avx_hash2.o: avx_hash2.c avx_hash2.h

clean:
	rm -f *.o mrx

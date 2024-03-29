#define _GNU_SOURCE
#include <sched.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/mman.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fts.h>
#include <values.h>
#include <ctype.h>
#include <errno.h>

#include "mem.h"
#include "mrx_hash32.h"
#include "mrx_hash64.h"
#include "mlx_hash.h"
#include "mlx_hash2.h"
#include "mlx_hash3.h"
#include "mlx_hash4.h"
#include "spb_hash.h"
#include "sse_hash.h"
#include "sse_hash2.h"
#include "sse_hash3.h"
#include "clm_hash.h"
#include "clm_hash2.h"
#include "avx_hash.h"
#include "avx_hash2.h"
#include "avx_hash3.h"
#include "avx_hash4.h"
#include "aes_hash.h"
#include "aes_hash2.h"

#define BLOCK_SIZE	(1 << 20)
#define false		0
#define true		1

#define HASH_FUNCS	18
#define HASH_TYPE_DEF	15

typedef void (*hf_single)(const void * const data, const size_t len,
    void * const hash);
typedef void (*hf_start)(void * const state);
typedef void (*hf_update)(void * const state, const void * const data,
    const size_t len);
typedef void (*hf_end)(void * const state, void * const hash);

struct hash_func {
	const char *	hash_name;
	uint32_t	hash_size;
	hf_single	hash_single;
	hf_start	hash_start;
	hf_update	hash_update;
	hf_end		hash_end;
};

struct hash_func hash_funcs[HASH_FUNCS] = {
	{ "mrxhash32",
	    sizeof(mrx_hash_32_t),
	    (hf_single)mrx_hash_32,
	    (hf_start)mrx_hash_32_start,
	    (hf_update)mrx_hash_32_update,
	    (hf_end)mrx_hash_32_end },
	{ "mrxhash64",
	    sizeof(mrx_hash_64_t),
	    (hf_single)mrx_hash_64,
	    (hf_start)mrx_hash_64_start,
	    (hf_update)mrx_hash_64_update,
	    (hf_end)mrx_hash_64_end },
	{ "mlxhash",
	    sizeof(mlx_hash_t),
	    (hf_single)mlx_hash,
	    (hf_start)mlx_hash_start,
	    (hf_update)mlx_hash_update,
	    (hf_end)mlx_hash_end },
	{ "mlxhash2",
	    sizeof(mlx_hash2_t),
	    (hf_single)mlx_hash2,
	    (hf_start)mlx_hash2_start,
	    (hf_update)mlx_hash2_update,
	    (hf_end)mlx_hash2_end },
	{ "mlxhash3",
	    sizeof(mlx_hash3_t),
	    (hf_single)mlx_hash3,
	    (hf_start)mlx_hash3_start,
	    (hf_update)mlx_hash3_update,
	    (hf_end)mlx_hash3_end },
	{ "mlxhash4",
	    sizeof(mlx_hash4_t),
	    (hf_single)mlx_hash4,
	    NULL,
	    NULL,
	    NULL},
	{ "spbhash",
	    sizeof(spb_hash_t),
	    (hf_single)spb_hash,
	    (hf_start)spb_hash_start,
	    (hf_update)spb_hash_update,
	    (hf_end)spb_hash_end },
	{ "ssehash",
	    sizeof(sse_hash_t),
	    (hf_single)sse_hash,
	    (hf_start)sse_hash_start,
	    (hf_update)sse_hash_update,
	    (hf_end)sse_hash_end },
	{ "ssehash2",
	    sizeof(sse_hash2_t),
	    (hf_single)sse_hash2,
	    (hf_start)sse_hash2_start,
	    (hf_update)sse_hash2_update,
	    (hf_end)sse_hash2_end },
	{ "ssehash3",
	    sizeof(sse_hash3_t),
	    (hf_single)sse_hash3,
	    (hf_start)sse_hash3_start,
	    (hf_update)sse_hash3_update,
	    (hf_end)sse_hash3_end },
	{ "clmhash",
	    sizeof(clm_hash_t),
	    (hf_single)clm_hash,
	    (hf_start)clm_hash_start,
	    (hf_update)clm_hash_update,
	    (hf_end)clm_hash_end },
	{ "clmhash2",
	    sizeof(clm_hash2_t),
	    (hf_single)clm_hash2,
	    (hf_start)clm_hash2_start,
	    (hf_update)clm_hash2_update,
	    (hf_end)clm_hash2_end },
	{ "avxhash",
	    sizeof(avx_hash_t),
	    (hf_single)avx_hash,
	    (hf_start)avx_hash_start,
	    (hf_update)avx_hash_update,
	    (hf_end)avx_hash_end },
	{ "avxhash2",
	    sizeof(avx_hash2_t),
	    (hf_single)avx_hash2,
	    (hf_start)avx_hash2_start,
	    (hf_update)avx_hash2_update,
	    (hf_end)avx_hash2_end },
	{ "avxhash3",
	    sizeof(avx_hash3_t),
	    (hf_single)avx_hash3,
	    (hf_start)avx_hash3_start,
	    (hf_update)avx_hash3_update,
	    (hf_end)avx_hash3_end },
	{ "avxhash4",
	    sizeof(avx_hash4_t),
	    (hf_single)avx_hash4,
	    (hf_start)avx_hash4_start,
	    (hf_update)avx_hash4_update,
	    (hf_end)avx_hash4_end },
	{ "aeshash",
	    sizeof(aes_hash_t),
	    (hf_single)aes_hash,
	    (hf_start)aes_hash_start,
	    (hf_update)aes_hash_update,
	    (hf_end)aes_hash_end },
	{ "aeshash2",
	    sizeof(aes_hash2_t),
	    (hf_single)aes_hash2,
	    (hf_start)aes_hash2_start,
	    (hf_update)aes_hash2_update,
	    (hf_end)aes_hash2_end },
};

struct mrx_args {
	char *filename;
	struct stat *st;
	void *buffer;
	uint32_t benchmark;
	uint32_t block_size;
	uint32_t recurse;
	uint32_t verbose;
	uint32_t hash_type;
	uint32_t check;
	uint32_t dieharder;
	union {
	    mrx_hash_32_t mrx_hash_32_hash;
	    mrx_hash_64_t mrx_hash_64_hash;
	    mlx_hash_t mlx_hash_hash;
	    mlx_hash2_t mlx_hash2_hash;
	    mlx_hash3_t mlx_hash3_hash;
	    mlx_hash4_t mlx_hash4_hash;
	    spb_hash_t spb_hash_hash;
	    sse_hash_t sse_hash_hash;
	    sse_hash2_t sse_hash2_hash;
	    sse_hash3_t sse_hash3_hash;
	    clm_hash_t clm_hash_hash;
	    clm_hash2_t clm_hash2_hash;
	    avx_hash_t avx_hash_hash;
	    avx_hash2_t avx_hash2_hash;
	    avx_hash3_t avx_hash3_hash;
	    avx_hash4_t avx_hash4_hash;
	    aes_hash_t aes_hash_hash;
	    aes_hash2_t aes_hash2_hash;
	} hash;
};

static void
usage(void)
{
	uint32_t i;

	printf("usage: mrx [options] <files...>\n");
	printf("	-b			benchmark mode\n");
	printf("	-c			check mode\n");
	printf("	-d			dieharder mode\n");
	printf("	-h			this help message\n");
	printf("	-r			recurse into directories\n");
	printf("	-t <hash type>		hash algorithm type\n");
	printf("	-v			be verbose\n");
	printf("	-x <size>		block size (KB)\n");

	printf("\nhash types:\n");
	for (i = 0; i < HASH_FUNCS; i++)
		printf("	%s (%d-bit)\n",
		    hash_funcs[i].hash_name, hash_funcs[i].hash_size * NBBY);
}

static void
print_hash(const void * const hash, const uint32_t len)
{
	const unsigned char * const p = hash;
	uint32_t i;

	for (i = 0; i < len; i++)
		printf("%02x", p[i]);
}

static inline void
gendata(unsigned char * const buffer, const size_t size)
{
	size_t pos = 0;

	if (size > 3) {
		while (pos < size - 3) {
			writemem32(&buffer[pos], lrand48());
			pos += 4;
		}
	}

	while (pos < size)
		buffer[pos++] = lrand48();
}

#define	BENCH_TIME	3000000000
#define	BENCH_TESTS	5
#define	BENCH_ITERS	10000

static inline unsigned long
rdtsc(void)
{
	unsigned int a;
	unsigned int d;
	__asm__ volatile ("xorl %%eax,%%eax \n	cpuid"
	    ::: "%rax", "%rbx", "%rcx", "%rdx");
	__asm__ volatile ("rdtsc" : "=a" (a), "=d" (d));
	return (unsigned long)a | ((unsigned long)d << 32);
}

static inline unsigned long
gettime(void)
{
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
	return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

static inline void
benchmark_hash(struct mrx_args * const args)
{
	struct hash_func *hf;
	double rate;
	double best_rate;
	double bpc;
	double best_bpc;
	double cph;
	double best_cph;
	uint64_t bytes;
	uint64_t start;
	uint64_t end;
	uint64_t cycles;
	uint64_t ts_start;
	uint64_t iterations;
	uint64_t time;
	uint32_t test;
	uint32_t i;

	hf = &hash_funcs[args->hash_type];

	best_rate = 0;
	best_bpc = 0;
	best_cph = 0;
	printf("cooling...\r");
	fflush(stdout);
	sleep(30);
	printf("%*s: ", 10, hf->hash_name);
	fflush(stdout);

	for (test = 0; test < BENCH_TESTS; test++) {

		sleep(5);
		cycles = 0;
		iterations = 0;
		ts_start = gettime();

		do {
			start = rdtsc();

			for (i = 0; i < BENCH_ITERS; i++) {
				hf->hash_single(args->buffer,
				    args->block_size, &args->hash);
			}

			end = rdtsc();

			cycles += end - start;
			time = gettime() - ts_start;
			iterations += BENCH_ITERS;

		} while (time < BENCH_TIME);

		bytes = args->block_size * iterations;
		rate = (double)bytes / (double)(time / 1000);
		bpc = (double)bytes / (double)cycles;
		cph = (double)cycles / (double)iterations;
		printf("%8.2f ", rate);
		fflush(stdout);

		if (rate > best_rate) {
			best_rate = rate;
			best_bpc = bpc;
			best_cph = cph;
		}
	}

	printf("--> %9.2f MB/s (%5.2f b/c, %10.2f c/h, %9.2f MB/s @ 4GHz)\n",
	    best_rate, best_bpc, best_cph, best_bpc * 4000000000 / 1000000);
	fflush(stdout);
}

static unsigned int
benchmark(struct mrx_args * const args)
{
	cpu_set_t cpuset;
	int ret;

	CPU_ZERO(&cpuset);
	CPU_SET(1, &cpuset);
	ret = sched_setaffinity(0, sizeof(cpu_set_t), &cpuset);
	if (ret != 0) {
		ret = errno;
		fprintf(stderr, "Failed to set cpu affinity: %s\n",
		    strerror(ret));
		goto out;
	}

	gendata(args->buffer, args->block_size);

	if (args->hash_type != HASH_FUNCS)
		benchmark_hash(args);
	else {
		for (args->hash_type = 0;
		    args->hash_type < HASH_FUNCS;
		    args->hash_type++) {
			benchmark_hash(args);
		}
	}

 out:
	return ret;
}

static unsigned int
dieharder(struct mrx_args * const args)
{
	struct hash_func *hf;
	int ret = 0;
	int fd;

	if (args->hash_type == HASH_FUNCS) {
		ret = EINVAL;
		fprintf(stderr, "Need a hash type.\n");
		goto out;
	}

	fd = STDOUT_FILENO;
	if (isatty(fd)) {
		ret = EIO;
		fprintf(stderr, "Will not write to terminal\n");
		goto out;
	}

	hf = &hash_funcs[args->hash_type];
	memset(&args->hash, 0, hf->hash_size);

	for (;;) {
		hf->hash_single(&args->hash, hf->hash_size, &args->hash);
		ret = write(fd, &args->hash, hf->hash_size);
		if (ret < 0) {
			ret = errno;
			fprintf(stderr, "Failed to write hash to stdout\n");
			goto out;
		}

	}

 out:
	return ret;
}

static inline unsigned int
read_data(int fd, unsigned char *buffer, size_t *size)
{
	unsigned int resid;
	int ret;

	resid = *size;
	while (resid > 0) {
		ret = read(fd, buffer, resid);
		if (ret < 0)
			return errno;
		if (ret == 0)
			break;
		resid -= ret;
		buffer += ret;
	}

	*size -= resid;
	return 0;
}

static unsigned int
process_data(const int fd, struct mrx_args * const args)
{
	struct hash_func *hf;
	size_t size;
	union {
		mrx_hash_32_state_t mrx_hash_32_state;
		mrx_hash_64_state_t mrx_hash_64_state;
		mlx_hash_state_t mlx_hash_state;
		mlx_hash2_state_t mlx_hash2_state;
		mlx_hash3_state_t mlx_hash3_state;
		spb_hash_state_t spb_hash_state;
		sse_hash_state_t sse_hash_state;
		sse_hash2_state_t sse_hash2_state;
		sse_hash3_state_t sse_hash3_state;
		clm_hash_state_t clm_hash_state;
		clm_hash2_state_t clm_hash2_state;
		avx_hash_state_t avx_hash_state;
		avx_hash2_state_t avx_hash2_state;
		avx_hash3_state_t avx_hash3_state;
		avx_hash4_state_t avx_hash4_state;
		aes_hash_state_t aes_hash_state;
		aes_hash2_state_t aes_hash2_state;
	} state;
	unsigned int ret;

	hf = &hash_funcs[args->hash_type];

	hf->hash_start(&state);

	for (;;) {

		size = args->block_size;
		ret = read_data(fd, args->buffer, &size);
		if (ret != 0) {
			fprintf(stderr, "File %s: failed to read data: %s\n",
			    args->filename, strerror(ret));
			goto out;
		}

		if (size == 0)
			break;

		hf->hash_update(&state, args->buffer, size);
	}

	hf->hash_end(&state, &args->hash);

 out:

	if (ret == 0 && args->check == false) {
		print_hash(&args->hash, hf->hash_size);
		printf(" %s\n", args->filename);
	}

	return ret;
}

static unsigned int
process_file(struct mrx_args * const args)
{
	int fd = -1;
	int ret;

	fd = open(args->filename, O_RDONLY);
	if (fd < 0) {
		ret = errno;
		fprintf(stderr, "File %s: failed to open file: %s\n",
		    args->filename, strerror(ret));
		goto out;
	}

	posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);

	ret = process_data(fd, args);
	if (ret != 0)
		goto out;

 out:

	if (fd != -1)
		close(fd);

	return ret;
}

static unsigned int
process_dir(struct mrx_args * const args)
{
	char *path_argv[2];
	FTS *fts;
	FTSENT *entry;
	int ret = 0;
	int err;

	path_argv[0] = args->filename;
	path_argv[1] = NULL;

	fts = fts_open(path_argv, FTS_PHYSICAL | FTS_NOCHDIR, NULL);
	if (fts == NULL) {
		ret = errno;
		fprintf(stderr, "Dir %s: could not open: %s",
		    args->filename, strerror(ret));
		return ret;
	}

	while ((entry = fts_read(fts))) {
		switch(entry->fts_info) {
		case FTS_D:
		case FTS_DP:
		case FTS_DC:
		case FTS_SL:
		case FTS_SLNONE:
		case FTS_DOT:
			continue;

		case FTS_ERR:
		case FTS_DNR:
		case FTS_NS:
			err = errno;
			fprintf(stderr, "File %s: unable to access: %s",
			    entry->fts_path, strerror(err));
			if (ret == 0)
				ret = err;
			continue;

		case FTS_F:
			args->filename = entry->fts_path;
			args->st = entry->fts_statp;
			err = process_file(args);
			if (ret == 0)
				ret = err;
			continue;
		}
	}

	fts_close(fts);
	return ret;
}

static unsigned int
process_path(struct mrx_args * const args)
{
	struct stat st;
	int fd;
	int ret;

	if (args->filename[0] == '-' && args->filename[1] == 0) {
		args->filename = "(stdin)";
		fd = STDIN_FILENO;
		if (isatty(fd)) {
			ret = EIO;
			fprintf(stderr, "Will not read from terminal\n");
			goto out;
		}

		ret = process_data(fd, args);
		goto out;
	}

	ret = stat(args->filename, &st);
	if (ret < 0) {
		ret = errno;
		fprintf(stderr, "File %s: cannot stat: %s\n",
		    args->filename, strerror(ret));
		goto out;
	}

	if (S_ISDIR(st.st_mode)) {
		if (args->recurse == false) {
			ret = EISDIR;
			fprintf(stderr, "File %s: is a directory\n",
			    args->filename);
			goto out;
		}

		ret = process_dir(args);
		goto out;
	}

	if (!S_ISREG(st.st_mode)) {
		ret = EINVAL;
		fprintf(stderr, "File %s: not a regular file\n",
		    args->filename);
		goto out;
	}

	args->st = &st;
	ret = process_file(args);

 out:
	return ret;
}

static unsigned int
a2x(unsigned char x)
{
	unsigned int r = 0;

	if (x >= '0' && x <= '9')
		r = x - '0';
	else if (x >= 'a' && x <= 'f')
		r = x - 'a' + 10;
	else if (x >= 'A' && x <= 'F')
		r = x - 'A' + 10;

	return r;
}

static unsigned int
process_check(struct mrx_args * const args)
{
	FILE *fp;
	char *line;
	char *checkfile;
	unsigned long lineno;
	unsigned long failed;
	unsigned long passed;
	unsigned long errors;
	ssize_t line_len;
	size_t line_size;
	unsigned char hash[16];
	unsigned int i;
	unsigned int j;
	unsigned int ret;

	ret = 0;
	lineno = 0;
	failed = 0;
	passed = 0;
	errors = 0;
	line = NULL;
	fp = NULL;
	checkfile = args->filename;

	if (args->filename[0] == '-' && args->filename[1] == 0) {
		fp = stdin;
		checkfile = "(stdin)";
	} else {
		fp = fopen(args->filename, "r");
		if (fp == NULL) {
			ret = errno;
			fprintf(stderr, "File %s: failed to open file: %s\n",
			    args->filename, strerror(ret));
			goto out;
		}
	}

	line_size = PATH_MAX + 32;
	line = malloc(line_size);
	if (line == NULL) {
		ret = errno;
		fprintf(stderr, "Failed to allocate %ld bytes: %s\n",
		    line_size, strerror(ret));
		goto out;
	}

	do {
		lineno++;
		line_len = getline(&line, &line_size, fp);
		if (line_len <= 0)
			break;
		if (line[line_len - 1] == '\n')
			line[--line_len] = '\0';
		if (lineno == 1) {
			for (i = 0; i < HASH_FUNCS; i++) {
				if (strcmp(line,
				    hash_funcs[i].hash_name) == 0) {
					args->hash_type = i;
					break;
				}
			}
			if (i == HASH_FUNCS) {
				ret = EINVAL;
				fprintf(stderr,
				    "Error: unknown hash type, %s, line %lu\n",
				    line, lineno);
				goto out;
			}
			continue;
		}
		if (line[0] == '#')
			continue;
		for (i = 0; i < line_len && isxdigit(line[i]); i++);
		if (i != (hash_funcs[args->hash_type].hash_size << 1)) {
			ret = EINVAL;
			fprintf(stderr,
			    "Error: incorrect hash size, line %lu\n", lineno);
			goto out;
		}
		if ((i + 2) > line_len) {
			ret = EINVAL;
			fprintf(stderr,
			    "Error: entry too short, line %lu\n", lineno);
			goto out;
		}
		if (line[i] != ' ') {
			ret = EINVAL;
			fprintf(stderr,
			    "Error: format error, line %lu\n", lineno);
			goto out;
		}
		line[i] = '\0';
		args->filename = &line[i + 1];

		ret = process_file(args);
		if (ret == 0) {
			for (i = 0, j = 0;
			    i < hash_funcs[args->hash_type].hash_size;
			    i++, j += 2) {
				hash[i] = (a2x(line[j]) << 4) +
				    a2x(line[j + 1]);
			}

			if (memcmp(hash, &args->hash,
			    hash_funcs[args->hash_type].hash_size) != 0) {
				fprintf(stderr,
				    "Error: file %s, failed hash check\n",
				    args->filename);
				failed++;
			} else {
				if (args->verbose == true) {
					fprintf(stderr, "%s OK\n",
					    args->filename);
				}
				passed++;
			}
		} else {
			errors++;
		}
	} while (!feof(fp) && !ferror(fp));

	printf("%s: %lu files passed, %lu files failed, %lu errors\n",
	    checkfile, passed, failed, errors);

 out:

	if (line != NULL)
		free(line);
	if (fp != NULL && fp != stdin)
		fclose(fp);
	return ret;
}

int
main(int argc, char **argv)
{
	struct mrx_args args;
	char **file_list;
	char *stdin_file[1] = { "-" };
	unsigned long files;
	unsigned long fileno;
	int ret = 0;
	int err;
	int c;
	int i;

	args.buffer = NULL;
	args.recurse = false;
	args.benchmark = false;
	args.verbose = false;
	args.block_size = BLOCK_SIZE;
	args.hash_type = HASH_FUNCS;
	args.check = false;
	args.dieharder = false;

	while ((c = getopt(argc, argv, "bcdhrt:vx:")) != EOF) {
		switch (c) {
		case 'b':
			args.benchmark = true;
			break;
		case 'c':
			args.check = true;
			break;
		case 'd':
			args.dieharder = true;
			break;
		case 'r':
			args.recurse = true;
			break;
		case 't':
			for (i = 0; i < HASH_FUNCS; i++) {
				if (strcmp(hash_funcs[i].hash_name,
				    optarg) == 0) {
					args.hash_type = i;
					break;
				}
			}
			if (i == HASH_FUNCS) {
				printf("Unknown hash type: %s\n", optarg);
				exit(1);
			}
			break;
		case 'v':
			args.verbose = true;
			break;
		case 'x':
			args.block_size = strtoul(optarg, NULL, 0);
			if (args.block_size > (1 << 20)) {
				printf("Block size too large.\n");
				exit(1);
			}
			args.block_size <<= 10;
			break;
		case 'h':
		default:
			usage();
			exit(1);
			break;
		}
	}

	if (args.dieharder == true) {
		ret = dieharder(&args);
		goto out;
	}

	ret = posix_memalign((void **)&args.buffer, getpagesize(),
	    args.block_size);
	if (ret != 0) {
		ret = ENOMEM;
		fprintf(stderr, "Failed to allocate %d bytes: %s\n",
		    args.block_size, strerror(ret));
		goto out;
	}

	if (args.benchmark == true) {
		ret = benchmark(&args);
		goto out;
	}

	if (args.hash_type == HASH_FUNCS)
		args.hash_type = HASH_TYPE_DEF;

	if (hash_funcs[args.hash_type].hash_start == NULL) {
		ret = EINVAL;
		fprintf(stderr, "Hash type does not support streaming mode\n");
		goto out;
	}

	file_list = &argv[optind];
	files = argc - optind;

	if (files == 0) {
		file_list = stdin_file;
		files = 1;
	}

	if (args.check == false && !isatty(STDOUT_FILENO))
		printf("%s\n", hash_funcs[args.hash_type].hash_name);

	for (fileno = 0; fileno < files; fileno++) {
		args.filename = file_list[fileno];
		if (args.check == true)
			err = process_check(&args);
		else
			err = process_path(&args);
		if (ret == 0)
			ret = err;
	}

 out:
	if (args.buffer != NULL)
		free(args.buffer);

	return ret;
}

/*
 * Copyright 2015, International Business Machines
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __WRAPPER_H__
#define __WRAPPER_H__

/*
 * Switching between software and hardware implemenation of zlib. The
 * hardware implementation is not implementing the full set of
 * interfaces but enough to do commonly used functionality for
 * compression and decompression.
 *
 * The hardware implementation is using the h_ prefix, the software
 * implementation is using a z_ prefix.
 */

#include <stdint.h>
#include <pthread.h>
#include <zaddons.h>

#ifndef ARRAY_SIZE
#  define ARRAY_SIZE(a)	 (sizeof((a)) / sizeof((a)[0]))
#endif

#ifndef __unused
#  define __unused __attribute__((unused))
#endif

#define zlib_trace_enabled()       (zlib_trace & 0x1)
#define zlib_hw_trace_enabled()    (zlib_trace & 0x2)
#define zlib_sw_trace_enabled()    (zlib_trace & 0x4)
#define zlib_gather_statistics()   (zlib_trace & 0x8)

/* Use in case of an error */
#define pr_err(fmt, ...) do {						\
		fprintf(stderr, "%s:%u: Error: " fmt,			\
			__FILE__, __LINE__, ## __VA_ARGS__);		\
	} while (0)

/* Use in case of an warning */
#define pr_warn(fmt, ...) do {						\
		fprintf(stderr, "%s:%u: Warning: " fmt,			\
			__FILE__, __LINE__, ## __VA_ARGS__);		\
	} while (0)

/* Informational printouts */
#define pr_info(fmt, ...) do {						\
		fprintf(stderr, "%s:%u: Info: " fmt,			\
			__FILE__, __LINE__, ## __VA_ARGS__);		\
	} while (0)

/* Trace zlib wrapper code */
#define pr_trace(fmt, ...) do {						\
		if (zlib_trace_enabled())				\
			fprintf(stderr, "### " fmt, ## __VA_ARGS__);	\
	} while (0)

/* Trace zlib hardware implementation */
#define hw_trace(fmt, ...) do {						\
		if (zlib_hw_trace_enabled())				\
			fprintf(stderr, "hhh " fmt, ## __VA_ARGS__);	\
	} while (0)

/* Trace zlib software implementation */
#define sw_trace(fmt, ...) do {						\
		if (zlib_sw_trace_enabled())				\
			fprintf(stderr, "sss " fmt, ## __VA_ARGS__);	\
	} while (0)

#define Z_UNSUPPORTED (-7)

#define ZLIB_SIZE_SLOTS 256	/* Each slot represents 4KiB, the last
				   slot is represending everything
				   which larger or equal 1024KiB */

struct zlib_stats {
	unsigned long deflateInit;
	unsigned long deflate[ZLIB_MAX_IMPL];
	unsigned long   deflate_avail_in[ZLIB_SIZE_SLOTS];
	unsigned long   deflate_avail_out[ZLIB_SIZE_SLOTS];
	unsigned long deflateReset;
	unsigned long   deflate_total_in[ZLIB_SIZE_SLOTS];
	unsigned long   deflate_total_out[ZLIB_SIZE_SLOTS];
	unsigned long deflateSetDictionary;
	unsigned long deflateSetHeader;
	unsigned long deflateParams;
	unsigned long deflatePrime;
	unsigned long deflateCopy;
	unsigned long deflateEnd;

	unsigned long inflateInit;
	unsigned long inflate[ZLIB_MAX_IMPL];
	unsigned long   inflate_avail_in[ZLIB_SIZE_SLOTS];
	unsigned long   inflate_avail_out[ZLIB_SIZE_SLOTS];
	unsigned long inflateReset;
	unsigned long inflateReset2;
	unsigned long   inflate_total_in[ZLIB_SIZE_SLOTS];
	unsigned long   inflate_total_out[ZLIB_SIZE_SLOTS];
	unsigned long inflateSetDictionary;
	unsigned long inflateGetDictionary;
	unsigned long inflateGetHeader;
	unsigned long inflateSync;
	unsigned long inflatePrime;
	unsigned long inflateCopy;
	unsigned long inflateEnd;

	unsigned long adler32;
	unsigned long adler32_combine;
	unsigned long crc32;
	unsigned long crc32_combine;
};

/* Hardware implementation */
int h_deflateInit2_(z_streamp strm, int level, int method,
		    int windowBits, int memLevel,
		    int strategy, const char *version,
		    int stream_size);
int h_deflateParams(z_streamp strm, int level, int strategy);

int h_deflateReset(z_streamp strm);
int h_deflateSetDictionary(z_streamp strm, const Bytef *dictionary,
			   uInt  dictLength);
int h_deflateSetHeader(z_streamp strm, gz_headerp head);
int h_deflate(z_streamp strm, int flush);
int h_deflateEnd(z_streamp strm);

int h_inflateInit2_(z_streamp strm, int  windowBits, const char *version,
		    int stream_size);
int h_inflateReset(z_streamp strm);
int h_inflateReset2(z_streamp strm, int windowBits);

int h_inflateSetDictionary(z_streamp strm, const Bytef *dictionary,
			   uInt dictLength);
int h_inflateGetDictionary(z_streamp strm, Bytef *dictionary,
			   uInt *dictLength);
int h_inflateGetHeader(z_streamp strm, gz_headerp head);
int h_deflateCopy(z_streamp dest, z_streamp source);

int h_inflate(z_streamp strm, int flush);
int h_inflateEnd(z_streamp strm);

/* Software implementation */
int z_deflateInit2_(z_streamp strm, int level, int method,
		    int windowBits, int memLevel, int strategy,
		    const char *version, int stream_size);
int z_deflateParams(z_streamp strm, int level, int strategy);

int z_deflateReset(z_streamp strm);
int z_deflateSetDictionary(z_streamp strm, const Bytef *dictionary,
			   uInt  dictLength);
int z_deflateSetHeader(z_streamp strm, gz_headerp head);
int z_deflatePrime(z_streamp strm, int bits, int value);
int z_deflateCopy(z_streamp dest, z_streamp source);
int z_deflate(z_streamp strm, int flush);
int z_deflateEnd(z_streamp strm);

int z_inflateInit2_(z_streamp strm, int  windowBits, const char *version,
		    int stream_size);
int z_inflateReset(z_streamp strm);
int z_inflateReset2(z_streamp strm, int windowBits);

int z_inflateSetDictionary(z_streamp strm, const Bytef *dictionary,
			   uInt  dictLength);
int z_inflateGetDictionary(z_streamp strm, const Bytef *dictionary,
			   uInt *dictLength);

int z_inflateGetHeader(z_streamp strm, gz_headerp head);

int z_inflatePrime(z_streamp strm, int bits, int value);
int z_inflateSync(z_streamp strm);

int z_inflate(z_streamp strm, int flush);
int z_inflateEnd(z_streamp strm);

int z_inflateBackInit_(z_streamp strm, int windowBits, unsigned char *window,
			const char *version, int stream_size);
int z_inflateBack(z_streamp strm, in_func in, void *in_desc, out_func out,
		  void *out_desc);
int z_inflateBackEnd(z_streamp strm);

uLong z_adler32(uLong adler, const Bytef *buf, uInt len);
uLong z_adler32_combine(uLong adler1, uLong adler2, z_off_t len2);
uLong z_crc32(uLong crc, const Bytef *buf, uInt len);
uLong z_crc32_combine(uLong crc1, uLong crc2, z_off_t len2);

const char *z_zError(int err);

extern int zlib_trace;
extern int zlib_accelerator;
extern int zlib_card;
extern unsigned int zlib_inflate_impl;
extern unsigned int zlib_deflate_impl;
extern unsigned int zlib_inflate_flags;
extern unsigned int zlib_deflate_flags;

/* PCIe trigger function. Writes to register 0x0 which normally non-sense. */
void error_trigger(void);

/* Constructors/destructors */
void zedc_hw_init(void);
void zedc_hw_done(void);

void zedc_sw_init(void);
void zedc_sw_done(void);

/* Circumvention for missing prototypes */
const char *z_zlibVersion(void);
uLong z_zlibCompileFlags(void);

/* Misc helper functions */
uint64_t str_to_num(char *str);
const char *ret_to_str(int ret);
const char *flush_to_str(int flush);

#endif	/* __WRAPPER_H__ */

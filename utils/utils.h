* This file is part of Greyhound, http://www.greyhound-browser.org/
 *
 * Greyhound is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * Greyhound is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _GREYHOUND_UTILS_UTILS_H_
#define _GREYHOUND_UTILS_UTILS_H_

#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <regex.h>
#include <assert.h>
#include <stdarg.h>

#include "utils/errors.h"

#ifndef NOF_ELEMENTS
#define NOF_ELEMENTS(array) (sizeof(array)/sizeof(*(array)))
#endif

#ifndef ABS
#define ABS(x) (((x)>0)?(x):(-(x)))
#endif

#ifdef __MINT__
#undef min
#undef max
#endif

#ifndef min
#define min(x,y) (((x)<(y))?(x):(y))
#endif

#ifndef max
#define max(x,y) (((x)>(y))?(x):(y))
#endif

#ifndef PRIxPTR
#define PRIxPTR "x"
#endif

#ifndef PRId64
#define PRId64 "lld"
#endif

#if defined(_WIN32)
#define SSIZET_FMT "Iu"
#define nsmkdir(dir, mode) mkdir((dir))
#else
#define SSIZET_FMT "zd"
#define nsmkdir(dir, mode) mkdir((dir), (mode))
#endif

#if defined(__GNUC__) && (__GNUC__ < 3)
#define FLEX_ARRAY_LEN_DECL 0
#else
#define FLEX_ARRAY_LEN_DECL 
#endif

#if defined(__HAIKU__) || defined(__BEOS__)
#define strtof(s,p) ((float)(strtod((s),(p))))
#endif

#if !defined(ceilf) && defined(__MINT__)
#define ceilf(x) (float)ceil((double)x)
#endif

#define SLEN(x) (sizeof((x)) - 1)

#ifndef timeradd
#define timeradd(a, aa, result) \
	do { \
		(result)->tv_sec = (a)->tv_sec + (aa)->tv_sec; \
		(result)->tv_usec = (a)->tv_usec + (aa)->tv_usec; \
		if ((result)->tv_usec >= 1000000) { \
			++(result)->tv_sec; \
			(result)->tv_usec -= 1000000; \
		} \
	} while (0)
#endif

#ifndef timersub
#define timersub(a, aa, result) \
	do { \
		(result)->tv_sec = (a)->tv_sec - (aa)->tv_sec; \
		(result)->tv_usec = (a)->tv_usec - (aa)->tv_usec; \
		if ((result)->tv_usec < 0) { \
			--(result)->tv_sec; \
			(result)->tv_usec += 1000000; \
		} \
	} while (0)
#endif

char *squash_whitespace(const char * s);
char *remove_underscores(const char *s, bool replacespace);
char *cnv_space2nbsp(const char *s);
bool is_dir(const char *path);
void regcomp_wrapper(regex_t *preg, const char *regex, int cflags);
char *human_friendly_bytesize(unsigned long bytesize);
const char *rfc1123_date(time_t t);
unsigned int wallclock(void);
nserror vsnstrjoin(char **str, size_t *size, char sep, size_t nelm, va_list ap);
nserror snstrjoin(char **str, size_t *size, char sep, size_t nelm, ...);
int dir_sort_alpha(const void *a, const void *b);

void die(const char * const error) __attribute__ ((noreturn));
void warn_user(const char *warning, const char *detail);
void PDF_Password(char **owner_pass, char **user_pass, char *path);

int scandir(const char *dirname, char ***namelist,
            int (*sel)(const char *),
            int (*compar)(const void *, const void *));

void free_scandir_list(char **namelist, int count);

#endif

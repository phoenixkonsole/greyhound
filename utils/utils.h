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

char *squash_whitespace(const char *s);
char *remove_underscores(const char *s, bool replacespace);
char *cnv_space2nbsp(const char *s);
bool is_dir(const char *path);
void regcomp_wrapper(regex_t *preg, const char *regex, int cflags);
char *human_friendly_bytesize(unsigned long bytesize);
const char *rfc1123_date(time_t t);
unsigned int wallclock(void);
nserror vsnstrjoin(char **str, size_t *size, char sep, size_t nelm, va_list ap);
nserror snstrjoin(char **str, size_t *size, char sep, size_t nelm, ...);

/* Amiga-kompatibler Vergleichsfunktions-Prototyp */
int dir_sort_alpha(const void *a, const void *b);

/* Amiga-kompatibles scandir (liefert Liste mit char*-Einträgen) */
int scandir(const char *dirname, char ***namelist,
            int (*sel)(const char *),
            int (*compar)(const void *, const void *));

void free_scandir_list(char **namelist, int count);

void die(const char * const error) __attribute__ ((noreturn));
void warn_user(const char *warning, const char *detail);
void PDF_Password(char **owner_pass, char **user_pass, char *path);

#endif


/************************************************************************/
/* Copyright (c) 1988-1989  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

/*  <stdlib.h>	Microtec Research, Inc. ANSI C Compiler  */
/* @(#)stdlib.h	1.7 11/15/90 */

#ifndef __STDLIB_H
#define __STDLIB_H

typedef unsigned	size_t;
typedef short		wchar_t;

typedef struct div_t {int  quot, rem;} div_t;
typedef struct ldiv_t {long quot, rem;} ldiv_t;

#define EXIT_FAILURE	1
#define EXIT_SUCCESS	0
#define RAND_MAX	0x7FFF
#define MB_CUR_MAX	1
#define	NULL		((void*)0)

#if _FPU		/* Convert functions to '881 versions */

#undef	atof
#undef	strtod

#define	atof	_atof881
#define	strtod	_strtod881

#endif	/* _FPU */

#if __STDC__

#if __cplusplus
extern "C" {
#endif

extern double    atof(const char *nptr);
extern int       atoi(const char *nptr);
extern long      atol(const char *nptr);
extern double    strtod(const char *nptr, char **endptr);
extern long      strtol(const char *nptr, char **endptr, int base);
extern unsigned  long strtoul(const char *nptr, char **endptr, int base);
extern int       rand(void);
extern void      srand(unsigned seed);
extern void     *calloc(size_t nmemb, size_t size);
extern void      free(void *ptr);
extern void     *malloc(size_t size);
extern void     *realloc(void *ptr, size_t size);
extern void      abort(void);
extern int       atexit(void (*func)(void));
extern void      exit(int status);
extern char     *getenv(const char *name);
extern int       system(const char *string);
extern void     *bsearch(const void *key, const void *base, size_t nmemb,
			size_t size, int (*compar)(const void *, const void *));
extern void      qsort(void *base, size_t nmemb, size_t size,
			int (*compar)(const void *, const void *));
extern int       abs(int j);
extern div_t     div(int numer, int denom);
extern long      labs(long j);
extern ldiv_t    ldiv(long numer, long denom);
extern int       mblen(const char *s, size_t n);
extern int       mbtowc(wchar_t *pwc, const char *s, size_t n);
extern int       wctomb(char *s, wchar_t wchar);
extern size_t    mbstowcs(wchar_t *pwcs, const char *s, size_t n);
extern size_t    wcstombs(char *s, const wchar_t *pwcs, size_t n);

#if __cplusplus
}
#endif

#else /* __STDC__ */

extern double    atof();
extern int       atoi();
extern long      atol();
extern double    strtod();
extern long      strtol();
extern unsigned  long strtoul();
extern int       rand();
extern void      srand();
extern void     *calloc();
extern void      free();
extern void     *malloc();
extern void     *realloc();
extern void      abort();
extern int       atexit();
extern void      exit();
extern char     *getenv();
extern int       system();
extern void     *bsearch();
extern void      qsort();
extern int       abs();
extern div_t     div();
extern long      labs();
extern ldiv_t    ldiv();
extern int       mblen();
extern int       mbtowc();
extern int       wctomb();
extern size_t    mbstowcs();
extern size_t    wcstombs();

#endif	/* ! __STDC__ */

#endif /* end __STDLIB_H */


/************************************************************************/
/* Copyright (c) 1988-1991 Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/
/*
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
*/

/*  <time.h>	Microtec Research, Inc. ANSI C Compiler  */
/* %W% %G% */

#ifndef __TIME_H
#define __TIME_H	1

#if __cplusplus
    #define	NULL		(0)
#else
    #define	NULL		((void *)0)
#endif

#define CLOCKS_PER_SEC	1

#if __STDC__
    #ifndef	_SIZE_T_DEFINED
	#ifdef _sun
	    typedef int size_t;
	#else
	    typedef _SIZE_T size_t;
	#endif
	#define	_SIZE_T_DEFINED		1
    #endif
#endif /* __STDC__ */

typedef unsigned long	clock_t;
typedef long		time_t;

struct tm
	{
	int  tm_sec;	/* seconds (0 - 59) */
	int  tm_min;	/* minutes (0 - 59) */
	int  tm_hour;	/* hours (0 - 23) */
	int  tm_mday;	/* day of month (1 - 31) */
	int  tm_mon;	/* month of year (0 - 11) */
	int  tm_year;	/* year - 1900 */
	int  tm_wday;	/* day of week (Sunday = 0) */
	int  tm_yday;	/* day of year (0 - 365) */
	int  tm_isdst;	/* Daylight Savings Time (0, 1) */
	};

#if __STDC__

    #if __cplusplus
	extern "C" {
    #endif

    extern clock_t    clock(void);
    extern double     difftime(time_t, time_t);
    extern time_t     mktime(struct tm *);
    extern time_t     time(time_t *);
    extern char      *asctime(const struct tm *);
    extern char      *ctime(const time_t *);
    extern struct tm *gmtime(const time_t *);
    extern struct tm *localtime(const time_t *);
    extern size_t     strftime(char *, size_t, const char *, const struct tm *);

    #if __cplusplus
	}
    #endif

#else /* !__STDC__ */

    extern clock_t    clock();
    extern double     difftime();
    extern time_t     mktime();
    extern time_t     time();
    extern char      *asctime();
    extern char      *ctime();
    extern struct tm *gmtime();
    extern struct tm *localtime();
    extern int	      strftime();

#endif /* __STDC__ */

#endif /* end __TIME_H */

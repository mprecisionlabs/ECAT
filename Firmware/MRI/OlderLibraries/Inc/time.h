
/************************************************************************/
/* Copyright (c) 1988-1989  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

/*  <time.h>	Microtec Research, Inc. ANSI C Compiler  */
/* @(#)time.h	1.2 4/13/90 */

#ifndef __TIME_H
#define __TIME_H

#define NULL ((void*)0)

#define CLK_TCK	1

typedef unsigned	size_t;
typedef unsigned	clock_t;
typedef unsigned long	time_t;

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
extern double     difftime(time_t time1, time_t time0);
extern time_t     mktime(struct tm *timeptr);
extern time_t     time(time_t *timer);
extern char      *asctime(const struct tm *timeptr);
extern char      *ctime(const time_t *timer);
extern struct tm *gmtime(const time_t *timer);
extern struct tm *localtime(const time_t *timer);
extern size_t     strftime(char *s, size_t maxsize, const char *format,
			const struct tm *timeptr);

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
extern size_t     strftime();

#endif /* __STDC__ */

#endif /* end __TIME_H */

#include "lib_top.h"


#if EXCLUDE_ctime
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1991, 1992 Microtec Research, Inc.       */
/* All rights reserved                                                      */
/****************************************************************************/
/*
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
*/


/*	%W% %G%	*/

/************************************************************************
 *  ctime.c	-  convert date and time to string
 *
 *	ctime() converts a long integer, representing the time in seconds
 *		since 00:00:00 (local time), Jan 1, 1980, to a 26-char
 *		string in the form:
 *			Mon Feb 10:25:02 1985\n\0
 *
 *	localtime() converts the time in seconds to local time in broken-
 *		down values in a static struct "tm".  Daylight Savings Time
 *		is not accounted for in the time returned by time() because
 *		of PC-DOS, therefore, it is also ignored in localtime().
 *
 *	gmtime() is identical to localtime because PC-DOS does not know
 *		time zone.
 *
 *	asctime() converts a "tm" struct to a 26-char string as shown above.
 *
 *  02/09/89 HY - fixed asctime() not to modify tm_year
 *  09/22/89 jpc - Some oversights created when base was switched from '80 
 *		   to '70 were fixed.  Also made extern for 68K.  
 *		   Handling of mday changed to fit 68K better.
 *  10/06/89 jpc - Declare mdays and tp as "const"
 *  04/11/90 jpc - added _MCC88K to #if's
 *  04/01/91 jpc - static data now defined in csysXXX.c
 *  04/01/91 jpc - xtm -> _xtm; now defined in csysXXX.c
 *
 ************************************************************************/
/* (last mod 02/09/89  HY) */

#include <time.h>

extern	char  _ctbuf[];		/* return value points to this data */
extern  struct tm _xtm;		/* static area for return values */

static  char *ct_put2();

char *asctime(const struct tm *t)

{
	register char *cp, *xcp;
	register const int  *tp;
	register int yr;

	cp = _ctbuf;
	xcp = "SunMonTueWedThuFriSat";		/* day of week */
	xcp += t->tm_wday * 3;
	*cp = *xcp;
	*++cp = *++xcp;
	*++cp = *++xcp;
	xcp = " Mon 00 00:00:00 1900\n";	/* the standard form */
	while (*++cp = *xcp) xcp++;
	cp = &_ctbuf[4];
	xcp = "JanFebMarAprMayJunJulAugSepOctNovDec";	/* month of yr */
	tp = &(t->tm_mon);
	xcp += *tp * 3;
	*cp = *xcp;
	*++cp = *++xcp;
	*++cp = *++xcp;
	cp = ct_put2(++cp,*--tp+500);		/* day of month */
	cp = ct_put2(cp,*--tp);			/* hour */
	cp = ct_put2(cp,*--tp);			/* min. */
	cp = ct_put2(cp,*--tp);			/* sec. */
	if ((yr = t->tm_year) >= 100) {
	    *(cp+1) = '2'; *(cp+2) = '0';
	    yr -= 100;
	}
	cp += 2;
	cp = ct_put2(cp,yr);			/* year */
	return(_ctbuf);
}


static char *ct_put2(cp,n)	/* converts n to 2-digit decimal (ASCII) */
	register char *cp;
{
	cp++;
	if (n >= 500 && (n -= 500) < 10) *cp++ = ' ';	/* leading blank */
	else *cp++ = n/10 + '0';			/* leading zero */
	*cp++ = n%10 +'0';
	return(cp);
}

static const char noleap_days[12] = 
	{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
static const char leap_days[12] = 
	{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };


struct tm *gmtime(const time_t *clock)

{
	register unsigned days, n;
	long  seconds;
	const char * mdays;

	days = *clock / 86400;		/* # days since Jan 1, 1970 */
	seconds = *clock % 86400;	/* # seconds since mid-night */
	if (*clock < 0L)
	    {
	    days = 0;
	    seconds = 0L;
	    }

	_xtm.tm_sec = seconds % 60;	/* second (0 - 59) */
	n = seconds / 60;
	_xtm.tm_min = n % 60;		/* minute (0 - 59) */
	_xtm.tm_hour = n / 60;		/* hour (0 - 23) */
	_xtm.tm_wday = (days + 4) % 7;	/* day of week (1/1/70 was Thursday) */
					/* above 2 -> 4 -- jpc - 8/23/89 */
	n = days / 365;			/* number of years since 1970 */
	days %= 365;			/* day number */
	if ((int)(days -= (n+1)/4) < 0) /* account for leap years */
	    {
	    days += 365;			/* put into previous year */
	    if(((--n + 2) % 4) == 0) ++days;	/* add one for leap year */
	    }

	_xtm.tm_year = n + 70;		/* year - 1900 */
	_xtm.tm_yday = days;		/* day of year (0 - 365) */

	mdays = ((n+2) % 4) ? 
		noleap_days : leap_days; /* get correct days in months */
	for (n=0; days >= mdays[n]; n++)
	    days -= mdays[n];

	_xtm.tm_mon = n;			/* month of year (0 - 11) */
	_xtm.tm_mday = ++days;		/* day of month (1 - 31) */
	_xtm.tm_isdst = 0;		/* Daylight Savings Time (0, 1) */
	return (&_xtm);
}


struct tm *localtime(const time_t *clock)

{
	return(gmtime(clock));
}


char *ctime(const time_t *clock)

{
	return(asctime(localtime(clock)));
}

 
#endif /* EXCLUDE_ctime */
 

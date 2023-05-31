#include "lib_top.h"


#if EXCLUDE_strftime
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1992 Microtec Research, Inc.				    */
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

/****************************************************************************/
/*  strftime:	Convert (struct tm *) to formatted time string		    */
/*									    */
/*  size_t strftime (char *s, size_t maxsize, const char *format,	    */
/*	const struct tm *timeptr);					    */
/*									    */
/*	s	address of string into which the time string will be 	    */
/*		written.						    */
/*	maxsize	maximum number of characters to be written to s, including  */
/*		terminating null.					    */
/*	format	string containing format string				    */
/*	timeptr	pointer to "tm" struct which describes the time to be 	    */
/*		formatted						    */
/*									    */
/*  NOTE:								    */
/*	This routine only supports the C locale.			    */
/*									    */
/*  History:								    */
/*	02/19/92 jpc:	written						    */
/*	11/10/93 jpc:	DIFF is now unsigned                                */
/****************************************************************************/

#include <string.h>
#include <time.h>

#if _SIZEOF_PTR == 2
    typedef unsigned short	DIFF;
#else
    typedef unsigned long	DIFF;
#endif

static const char blank[] = " ";

static const char abb_weekday [][4] = {
				"Sun",
				"Mon",
				"Tue",
				"Wed",
				"Thu",
				"Fri",
				"Sat" };

static const char weekday [][10] = {
				"Sunday",
				"Monday",
				"Tuesday",
				"Wednesday",
				"Thursday",
				"Friday",
				"Saturday" };

static const char abb_month [][4] = {
				"Jan",
				"Feb",
				"Mar",
				"Apr",
				"May",
				"Jun",
				"Jul",
				"Aug",
				"Sep",
				"Oct",
				"Nov",
				"Dec" };

static const char month [][10] = {
				"January",
				"February",
				"March",
				"April",
				"May",
				"June",
				"July",
				"August",
				"September",
				"October",
				"November",
				"December" };


static const char am [] = "AM";
static const char pm [] = "PM";

/************************************************************************/
/*	Conversion table to convert 24-hr to 12-hr clock		*/
/************************************************************************/

/*				  0   1   2   3   4   5   6   7   8	*/
static const char hour_12 [] = { 12,  1,  2,  3,  4,  5,  6,  7,  8,

/*				  9  10  11  12  13  14  15  16  17	*/
				  9, 10, 11, 12,  1,  2,  3,  4,  5,

/*				 18, 19, 20, 21, 22, 23 		*/
				  6,  7,  8,  9, 10, 11 };

#define COPY_IN(x) 							\
    { 									\
    if (-1 == (returnVal = localCopy (outString, (x), maxsize)))	\
	return 0; 							\
    break;								\
    }

/************************************************************************/
/*	String copy routine						*/
/*									*/
/*	Returns number of characters copied or 0 if attempt is made to	*/
/*	copy too many characters.					*/
/************************************************************************/

static int localCopy (register char *target, register char *source,
			register size_t max)
{
register int    i = 0;

    --source;	++max;			/* allow pre-inc & pre-dec */

    while (--max) {
	if (*++target = *++source)
	    ++i;
	else
	    return i;			/* return number of char (not null) */
    }
    return -1;
}

/************************************************************************/
/*	Convert a digit less than 100 to a 2 byte null terminated	*/
/*	string.								*/
/************************************************************************/

static void toDec2 (register char * outString, register int input)
{
    *outString   = (input / 10) + '0';
    *++outString = (input % 10) + '0';
    *++outString = '\0';
}

/************************************************************************/
/*	Generate an asii date of the form mm/dd/yy			*/
/************************************************************************/

static void genDate (char * target, const struct tm * localPtr) 
{
    toDec2 (target, localPtr->tm_mon + 1);
    target [2] = '/';
    toDec2 (target + 3, localPtr->tm_mday);
    target [5] = '/';
    if (localPtr->tm_year < 100)
	toDec2 (target + 6, localPtr->tm_year);
    else
	toDec2 (target + 6, localPtr->tm_year - 100);
}

/************************************************************************/
/*	Generate an asii time of the form hr:mn:sc			*/
/************************************************************************/

static void getTime (char * target, const struct tm * localPtr)
{
    toDec2 (target, localPtr->tm_hour);
    target [2] = ':';
    toDec2 (target + 3, localPtr->tm_min);
    target [5] = ':';
    toDec2 (target + 6, localPtr->tm_sec);
}

/************************************************************************/
/*	Actual strftime routine						*/
/************************************************************************/

size_t strftime (register char *outString, register size_t maxsize,
			register const char *format, const struct tm *timeptr)
{
struct tm localPtr = * timeptr;		/* place in local stack space */
char *startString  = outString;		/* save original value of outString */
char  scratch[26];			/* scratch buffer */
long  tempInt;				/* scratch integer */
char  testChar;
register int returnVal;			/* return form localCopy */

    --outString;			/* preset to allow pre-increment */
    --format;
    
    while (testChar = *++format) {
	if (testChar != '%') {
	    *++outString = testChar;
	    if (-1 == --maxsize) return 0;
	    continue;			/* go to next character */
	    }

	switch (*++format) {
	    case 'a':
		COPY_IN ((char *) abb_weekday[localPtr.tm_wday]);
	    case 'A':
		COPY_IN ((char *) weekday[localPtr.tm_wday]);
	    case 'b':
		COPY_IN ((char *) abb_month[localPtr.tm_mon]);
	    case 'B':
		COPY_IN ((char *) month[localPtr.tm_mon]);
	    case 'c':
		genDate (scratch, &localPtr);
		scratch [8] = ' ';
		getTime (scratch + 9, &localPtr);
		goto copy_scratch;
	    case 'd':
		toDec2 (scratch, localPtr.tm_mday);
		goto copy_scratch;
	    case 'H':
		toDec2 (scratch, localPtr.tm_hour);
		goto copy_scratch;
	    case 'I':
		toDec2 (scratch, hour_12 [localPtr.tm_hour]);
		goto copy_scratch;
	    case 'j':
	        *scratch = ((localPtr.tm_yday + 1) / 100) + '0';
		toDec2 (scratch + 1, (localPtr.tm_yday + 1) % 100);
		goto copy_scratch;
	    case 'm':
		toDec2 (scratch, localPtr.tm_mon + 1);
		goto copy_scratch;
	    case 'M':
		toDec2 (scratch, localPtr.tm_min);
		goto copy_scratch;
	    case 'p':
		COPY_IN ((localPtr.tm_hour < 12) ? (char *) am : (char *) pm);
	    case 'S':
		toDec2 (scratch, localPtr.tm_sec);
		goto copy_scratch;
	    case 'U':				/* Jan. 1, 1900 was a Monday */
		tempInt = 365 * (long) localPtr.tm_year;/* # days since */
		if (localPtr.tm_year) {			/* leap years */
		    tempInt += (localPtr.tm_year-1) / 4;
		    tempInt -= (localPtr.tm_year-1) / 100;
		    }
		tempInt = (tempInt + 1) % 7;		/* day of Jan 1 */
		toDec2 (scratch,
			(localPtr.tm_yday + (tempInt ? tempInt : 7)) / 7);
		goto copy_scratch;
	    case 'w':
		*scratch = localPtr.tm_wday + '0';
		scratch[1] = '\0';
		goto copy_scratch;
	    case 'W':
		tempInt = 365 * (long) localPtr.tm_year;/* # days since */
		if (localPtr.tm_year) {			/* leap years */
		    tempInt += (localPtr.tm_year-1) / 4;
		    tempInt -= (localPtr.tm_year-1) / 100;
		    }
		tempInt = (tempInt) % 7;	/* day of Jan 1 (Mon = 0) */
		toDec2 (scratch,
			(localPtr.tm_yday + (tempInt ? tempInt : 7)) / 7);
		goto copy_scratch;
	    case 'x':
		genDate (scratch, &localPtr);
		goto copy_scratch;
	    case 'X':
		getTime (scratch, &localPtr);
		goto copy_scratch;
	    case 'y':
		toDec2 (scratch, localPtr.tm_year % 100);
		goto copy_scratch;
	    case 'Y':
		tempInt = localPtr.tm_year + 1900;
		toDec2 (scratch, tempInt / 100);
		toDec2 (scratch + 2, tempInt % 100);
copy_scratch:
		COPY_IN (scratch);
	    case 'Z':
		COPY_IN ("");
	    case '%':
		COPY_IN ("%");
	    default:			/* error condition */
		COPY_IN ("ERROR");
	    }
	outString += returnVal;		/* executed every pass thru switch */
	maxsize   -= returnVal;
	}

return (DIFF)outString - (DIFF)startString + 1;
}
 
#endif /* EXCLUDE_strftime */
 

#include "lib_top.h"


#if EXCLUDE_strstr
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.                         */
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
/*	strstr.c							    */
/*									    */
/*	strstr (const char *s1, const char *s2);			    */
/*		The strstr function located the first occurrence in the	    */
/*		string pointed to by s1 of the sequence of characters 	    */
/*		(excluding the terminating null character) in the string    */
/*		pointed to by s2.					    */
/*	returns --							    */
/*		The strstr function returns a pointer to the located string */
/*		or a null pointer if the string is not found.  If s2 points */
/*		to a string with zero length, the function returns s1.	    */
/*									    */
/*	Note --								    */
/*		While some of the constructs used in this routine might	    */
/*		seem to be flaky, they have been selected to generate	    */
/*		efficient assembly code.  Do not modify for efficiency	    */
/*		unless close examination of generated assembly is made	    */
/*		also.	-jpc						    */
/*									    */
/*	History --							    */
/*		02/19/91 jpc:	Written					    */
/*		02/21/91 jpc:	Tweeks for speed			    */
/*									    */
/****************************************************************************/

#include <string.h>

char * strstr (const char *s1, const char *s2)
{
    register const unsigned char * mainString;
    register const unsigned char * testString;
    register unsigned char tempChar;

    mainString = (const unsigned char *) s1;	/* Place addresses in regs */
    testString = (const unsigned char *) s2;

    if (*testString == '\0')			/* zero length string */
	return (char *) s1;

    --mainString;				/* Setup */
MainLoop:
    if (tempChar = *++mainString)
	{
	if (tempChar != *testString)
	    goto MainLoop;

	    {
	    register const unsigned char * scanMain = mainString;
	    register const unsigned char * scanTest = testString;

	    while (tempChar = *++scanTest)
	        if (tempChar != *++scanMain)
		    goto MainLoop;
	    }
	return (char *) mainString;
	}

    return NULL;
}
 
#endif /* EXCLUDE_strstr */
 

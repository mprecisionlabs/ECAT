#include "lib_top.h"


#if EXCLUDE_strtol
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
 *  strtol.c
 *			S T R T O L   function
 *
 *  long strtol(str,ptr,base)
 *		Converts a number of base 'base' which is an ASCII string
 *		pointed to by str to long integer.
 *
 *  05/21/85 - fixed bug for base 16 to allow "0x" or "0X"
 *  10/06/89 jpc: cast str to (char *)
 *
 ************************************************************************/
/* (last mod 05/22/85  HY) */

#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#define TRUE	1
#define FALSE	0
/* #define NULL	0 */


long strtol (const char *str, char **ptr, int base)

{
    register int ch;
    register long result = 0;
    int neg = FALSE;

    while  (*str == ' ' || (*str >= 9 && *str <= 13))	/* "white spaces" */
	++str;
    if (*str == '-') {
    	neg = TRUE;
	str++;
    } else if (*str == '+')
    	str++;

    if (base <= 1 || base > 36 || base == 16) {
	if (*str == '0') {
	    ++str;
	    if (*str == 'x' || *str == 'X')
		{ base = 16; ++str; }
	    else if (base != 16) base = 8;
	}
	else if (base != 16) base = 10;
    }

    for (; ;) {
	ch = *str++;
    	if (ch >= '0' && ch <= '9') {
	    if ((ch -= '0') >= base)
		break;
	} else if (ch >= 'a' && ch <= 'z') {
	    if ((ch -= 'a'-10) >= base)
		break;
	} else if (ch >= 'A' && ch <= 'Z') {
	    if ((ch -= 'A'-10) >= base)
		break;
	} else
	    break;

        {
        register long tempResult;      		/* Temp for overflow check */

        tempResult = result;                    /* Salt away result for a bit */
        result = base*result + ch;
        if ((tempResult & 0xfe000000) && (result/base != tempResult)) {
                                                /* Overflow occurred */
            do {                                /* skip out to end of number */
                ch = *str++;
            } while ((ch >= '0' && ch <= '9' && ch <= ('0' + base)) ||
                        (ch >= 'a' && ch <= ('a' - 10 + base)) ||
                        (ch >= 'A' && ch <= ('A' - 10 + base)));
            if (ptr != (char **)0)
                *ptr = (char *)str-1;
            errno = ERANGE;
            return neg ? LONG_MIN : LONG_MAX;
            }
        }  

    }
    if (ptr != (char **)0)
    	*ptr = (char *)str-1;
    if (neg)
	return (-result);
    return (result);
}

 
#endif /* EXCLUDE_strtol */


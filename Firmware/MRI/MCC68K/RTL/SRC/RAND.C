#include "lib_top.h"


#if EXCLUDE_rand
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
 *  rand.c
 *			R A N D    functions
 *
 *	Random-number generator using a multiplicative congruential
 *  method.  Its period is 2 to power 32.
 *
 *	Ref: __STDC__ draft proposal for C standard
 *
 *	rand() returns a random integer in the range from 0 to 32767.
 *	srand(seed) resets the generator with a new starting value.
 *
 *	Changes:
 *	07/07/92 jpc - modified to allow a 31 bit return value.
 *	04/11/90 jpc - added _MCC88K to #if's
 *	01/29/91 jpc - added _MCCSP to #if
 *		       #if __STDC__ for function def's removed
 *	04/01/91 jpc - removed #if for data declarations
 *	11/19/91 jpc - removed __STDC__ constructs
 ************************************************************************/
/* (last mod 03/29/85  HY) */

extern long _randx;			/* initialized in csys68k also  */

/************************************************************************/
/*									*/
/*	NOTE:	If this routine is to be modified to return a 31 bit	*/
/*		random number, change the "#if 0" below to "#if 1".	*/
/*		Also the value specified for RAND_MAX in <stdlib.h>	*/
/*		must be modified to 0x7FFFFFFF.				*/
/*									*/
/*		Do not modify this routine on any targets on which the	*/
/*		sizeof (int) is 2.					*/
/*									*/
/************************************************************************/

#if 0
  #define MASK	0x7FFFFFFF
#else
  #define MASK	0x7FFF
#endif

int rand(void)
{
	_randx = _randx * 1103515245 + 12345;
	return((int)(_randx >> 16) & MASK);
}


void srand (unsigned seed)
{
	_randx = seed;
}
 
#endif /* EXCLUDE_rand */
 

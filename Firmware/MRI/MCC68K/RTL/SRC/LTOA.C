#include "lib_top.h"


#if EXCLUDE_ltoa
    #pragma option -Qws			/* Prevent compiler warning */
#else

_kill_warning();	/* Eliminates compiler warnings */
 
#if _OPTION_Kt
  #informing -- The "-Kt" option is disregarded by this routine.
#endif

#if _OPTION_upd || _OPTION_us
  #error -- The "-upd" and "-us" options cannot be applied to this routine.
  #error -- Use ltoa.c from the "vanilla" directory.
#endif

#if _OPTION_utn
  #error -- The "-ut<number>" option cannot be applied to this routine.
  #error -- Use ltoa.c from the "vanilla" directory.
#endif

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/*----------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.                     */
/* All rights reserved							*/
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


#include <string.h>
#include "asm_addr.h"

/*
*
*	File name:		itoa.a86
*	Author:			Herbert Yuen
*	Date:			85-Jun-07 Friday
*
*	History:
*	05/10/90 jpc:	modified to simply jump to __ltoa
* %W% %G% *
*
*==============================================================================
*
*			i t o a ,  l t o a
*
*==============================================================================
*  int	itoa (N, Str)   int  N; char *Str;
*  int	ltoa (N, Str)	long N; char *Str;
*
*	Convert the (signed) integer N to a null-terminated ASCII string
*	of decimal digits and place in the area pointed to by Str.  If N is
*	negative, the output string begins with a '-'.  No leading blanks or
*	zeros are produced.  The function returns the number of characters
*	placed in the output string, excluding the null terminator.
*
*/

#pragma	asm

	TTL			'ltoa'
	SECTION	code
	XDEF	_ltoa
	XREF	__ltoa
*
_ltoa:
	jmp	C_ADDR(__ltoa)
	END

#pragma	endasm
 
#endif /* EXCLUDE_ltoa */
 

/*
 * @(SUBID) 05/10/88 /src2/nls/libraries/libm/s.breakup.i 1.6
 * COPYRIGHT  Hewlett-Packard Company  1987, 1988
 */

/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/*----------------------------------------------------------------------*/
/* Copyright (c) 1991 Microtec Research, Inc.				*/
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


/*	%W% %G%	*/

/*--------------------------------------------------------------------
     AUTHOR: Michael Fluegge		05/10/88

     This code segment takes a double number and decomposes it into a
   two parts.  One part is a double number, 'f', in the range of [0.5, 1.0),
   the second part is an integer, 'n', s.t. 2 raised to the 'n' power
   times 'f' is equal to the original value of 'f'.  It should be noted
   that the original sign of 'f' must be positive, if not then incorrect
   results will be computed.					*/

    n = (f.half.hi >> 20) - EXP_BIAS_MNS1;		/* isolate exponent */

    if (isDENORM(f)) {
	register Int32	tmp;
	register int	shftcnt;

 /* The operand is a denormal number so the exponent value is off by one. */
	++n;

 /*   Locate the left-most 1 bit.  This bit must be moved to the implicit
  *  leading bit.  Thus it will be removed from the representation and
  *  the trailing bits move left to their new positions.  The exponent
  *  variable, 'n', must be adjusted by the number of bits shifted.    */
	if (f.half.hi & 0xFFFFF) {		/* 1 bit in high word? */

 /* Calculate the shift count */
	    for (tmp = f.half.hi << 12, shftcnt = 1;  
		      tmp > 0;
			     ++shftcnt, tmp <<= 1)
		;
 /*   Shift significand bits of the high word.  Move appropriate
  *  number of low word bits to the high word.  Shift the low word. */
	    f.half.hi <<= shftcnt;
	    f.half.hi |= f.half.lo >> (32 - shftcnt);
	    f.half.lo <<= shftcnt;
	}
	else {			/* left-most 1 bit is in the low word */
 /* Do equivalent processing as in the above case */
	    for (tmp = f.half.lo, shftcnt = 1;
		      tmp > 0;
			     ++shftcnt, tmp <<= 1)
		;
	    f.half.lo <<= shftcnt;
	    f.half.hi |= f.half.lo >> 12;
	    f.half.lo <<= 20;
	    shftcnt += 20;	/* Don't forget the 20 bits of the high word */
	}
	n -= shftcnt;		/* Fix the exponent value */
    }
     
 /*    Give the number an unbiased exponenent of -1.  This forces 'f'
  *  to be in the range of [0.5, 1.0)			*/
    f.half.hi &= 0x000FFFFF;		/* scale to between 0.5 and 1.0 */
    f.half.hi |= (Int32) EXP_BIAS_MNS1 << 20;

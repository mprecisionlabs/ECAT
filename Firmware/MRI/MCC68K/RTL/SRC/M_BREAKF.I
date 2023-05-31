/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1986, 1991, 1992 Microtec Research, Inc.		    */
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

 /*--------------------------------------------------------------------
     AUTHOR: Michael Fluegge		10/21/86

     This code segment takes a float number and decomposes it into a
   two parts.  One part is a float number, 'f', in the range of [0.5, 1.0),
   the second part is an integer, 'n', s.t. 2 raised to the 'n' power
   times 'f' is equal to the original value of 'f'.  It should be noted
   that the original sign of 'f' must be positive, if not then the
   result will be incorrect.				*/

 /* Notice that this code assumes that 'f' is a positive number */
    n = (f.ivalue >> 23) - EXP_BIAS_MNS1_f;

    if (isDENORMf(f)) {
	register UInt32  tmp;
        register int     shftcnt;

 /*   Locate the left-most 1 bit.  This bit must be moved to the implicit
  *  leading bit.  Thus it will be removed from the representation and
  *  the trailing bits moved left to their new positions.  The exponent
  *  variable, 'n', must be adjusted by the number of bits shifted.  */

 /* Calculate the shift count */
 /*    Start the shift count at 0 instead of 1 since the power is
  *  already one too large because the number is a denormal.		*/
	for (tmp = (f.ivalue & 0x007FFFFF) << 1, shftcnt = 0;  
                   tmp < 0x00800000;
                         shftcnt++, tmp <<= 1)
	    ;
 /* Create the new value with the shifted bits. */
	f.ivalue = tmp & 0x007FFFFF;
	n -= shftcnt;
    }
    else
    	f.ivalue &= 0x7FFFFF;		/* Throw the exponenent bits away */

    f.ivalue |= (UInt32) EXP_BIAS_MNS1_f << 23;	/*  0.5 <= f.value < 1.0  */

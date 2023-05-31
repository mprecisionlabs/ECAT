/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) Microtec Research, Inc. 1991. All rights reserved. 	*/
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

#if _68000 || _68008 || _68010 || _68EC000 || _68HC000 || _68HC001 || _68302
    #define	SHORT_RELATIVE_ADDRESSES	1
#else
    #define	SHORT_RELATIVE_ADDRESSES	0
#endif




#if _PIC					/* define addressing modes */
    #if SHORT_RELATIVE_ADDRESSES		/* for function addresses */
        #define C_ADDR(x) 	(x).w(PC)
    #else
        #define C_ADDR(x) 	(x).l(PC)
    #endif
#else
    #define 	C_ADDR(x)	(x)
#endif





#if (! _STR_CMP (_PID_REG, "a"))		/* absolute data addressing */
 
    #define     D_ADDR(x)       (x)
 
#elif (! _STR_CMP (_PID_REG, "p"))		/* pc relative data */
 
    #if SHORT_RELATIVE_ADDRESSES
        #define D_ADDR(x) (x).w(pc)
    #else
        #define D_ADDR(x) (x).l(pc)
    #endif
 
#else						/* An relative data */

    #if SHORT_RELATIVE_ADDRESSES
        #define D_ADDR(x)       (x).w(_STRIP_QUOTES(_PID_REG))
    #else
        #define D_ADDR(x)       (x).l(_STRIP_QUOTES(_PID_REG))
    #endif

#endif


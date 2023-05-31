
/************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO					*/
/* MICROTEC RESEARCH, INC.						*/
/* 2350 Mission College Blvd.						*/
/* Santa Clara, CA 95054						*/
/* USA									*/
/*----------------------------------------------------------------------*/
/* Copyright (c) Microtec Research, Inc. 1991, 1992.			*/
/* All rights reserved.							*/
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

/************************************************************************
*                                                                       *
*	This header file defines several target dependent macros	*
*                                                                       *
************************************************************************/

#define	_MONITOR_WRITE_SYMBOL	( \
	(ASM ("	xdef __sim_out_break")), \
	(ASM ("__sim_out_break:")))

#define	_MONITOR_READ_SYMBOL	( \
	(ASM ("	xdef __sim_in_break")), \
	(ASM ("__sim_in_break:")))



#if (! _STR_CMP (_PID_REG, "a"))                /* absolute data addressing */

    #define OFFSET      (0)

#elif (! _STR_CMP (_PID_REG, "p"))              /* pc relative data */

    static const unsigned long address = (const unsigned long) &address;

    #define OFFSET      ((unsigned long) &address - address)

#else                                           /* An register addressing */

    #define OFFSET      (asm (unsigned long, \
                        "       xref    ?" _PID_REG, \
                        "       move.l  " _PID_REG ",d0",       /* run-time */\
                        "       sub.l   #?" _PID_REG ",d0"))    /* link-time */

#endif


#define	_HEAP_START		/* Beginning of heap segment*/ \
    ((ASM (char *, " move.l #.startof.(heap),d0")) + (OFFSET))

#define _HEAP_SIZE		/* Size of heap segment */ \
    (ASM (unsigned, " move.l #.sizeof.(heap),d0"))

extern	char * _HEAP;

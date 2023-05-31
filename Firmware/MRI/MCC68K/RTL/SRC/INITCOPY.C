#include "lib_top.h"


#if EXCLUDE_initcopy
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1987, 1988, 1989, 1990, 1991, 1992 Microtec Research, Inc. */
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


/*	%G% %W%	*/

/****************************************************************************
*
* Title		INITCOPY.C
*
* Abstract	Initialization routine to copy the initialized data to RAM  
*		space based on organized information stored in the special
*		section assigned through linker command INITDATA. 
*
* Functions	This routine will scan thorugh the section ??INITDATA and copy
*		data based on the special bytes indicated.  The content of
*		this section consists the content of each individual input
*		section specified in INITDATA command (i.e. even sections
*		are specified as output section).  Special bytes are:
*		Byte 'S' means the starting of a section;
*		Byte 'C' means operation COPY;
*		Byte 'E' meand end of section;
*		Four bytes after byte 'C' indicates the size of section;
*		Subsequent four bytes that follows indicates the address 
*		of copy destination.
* Returns	0 - Normal return
*		1 - ??INITDATA not initialized - No starting point
*		2 - ??INITDATA not initialized correctly - Unknown operation
*
****************************************************************************/

#include <string.h>

#if (! _STR_CMP (_PID_REG, "a"))		/* absolute data addressing */

    #define OFFSET	(0)

#elif (! _STR_CMP (_PID_REG, "p"))		/* pc relative data */

    static const unsigned long address = (const unsigned long) &address;

    #define OFFSET	((unsigned long) &address - address)

#else						/* An register addressing */

    #define OFFSET	(asm (unsigned long, \
			"       xref	?" _PID_REG, \
			"       move.l	" _PID_REG ",d0",	/* run-time */\
			"       sub.l	#?" _PID_REG ",d0"))	/* link-time */

#endif

#define TARGET	((dest) + (OFFSET))



int	_initcopy()
{
  register char 	*local;
  register unsigned 	long size, dest;
  
  if (ASM (unsigned, "\tmove.l\t#.sizeof.(??INITDATA),d0") <= 2)
    return 0;		/* if ??INITDATA is empty -- return w. no error */

  local = (char *) ASM("\tmove.l\t#.startof.(??INITDATA),d0") + OFFSET;

  if (*local != 'S')
       return 1;

  local++;
  while (*local != 'E')
       {
       switch (*local)
	 {
	 case 'C':
	   local++;
	   size = (unsigned long)(((unsigned char)*local << 24) |
			     ((unsigned char)*(local+1) << 16) |
			     ((unsigned char)*(local+2) << 8) |
			     (unsigned char)*(local+3));
	   local += 4;
	   dest = (unsigned long)(((unsigned char)*local << 24) |
			     ((unsigned char)*(local+1) << 16) |
			     ((unsigned char)*(local+2) << 8) |
			     (unsigned char)*(local+3));
	   local += 4;
	   memcpy ((void *) TARGET, (void *) local, (size_t) size);
	   local += size;
	   break;
	 default:
	   return 2;
	 }
       }
  return 0;					/* Normal return */
}
 
#endif /* EXCLUDE_initcopy */
 

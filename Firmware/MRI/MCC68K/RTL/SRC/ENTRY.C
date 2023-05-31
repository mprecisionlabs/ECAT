#include "lib_top.h"


#if EXCLUDE_entry
    #pragma option -Qws      /* Eliminates compiler warnings */
#else

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


/************************************************************************/
/*       	                                                     	*/
/*  ENTRY:  This is the main entry point routine, it does the following	*/
/*									*/
/*          1. Initializes SP as (.startof.(stack) + .sizeof.(stack))	*/
/*	       (defined at link time)					*/
/*	    2. Initializes __HEAP as starting address of SECTION heap	*/
/*          3. Initializes the frame pointer to zero for debugger	*/
/*	    4. Clears SECTION zerovars					*/
/*	    5. Initializes cache on 68040 and 68ec040.  The C		*/
/*	       pre-processor symbol ENABLE_68040_CACHE must be defined	*/
/*	       to enable this code.					*/
/*          6. Calls C initialization routine _START			*/
/*									*/
/*	This module also contains the following functions:		*/
/*	          _exit():  Called by exit() upon program termination	*/
/*	 __enable_cache():  Application specific function which enables	*/
/*			    68040 cache.  A sample routine is provided	*/
/*			    which is enabled by defining the		*/
/*			    preprocessor symbol ENABLE_68040_CACHE.	*/
/*									*/
/************************************************************************/
/*									*/
/*	note:   The special character "%" which appears after 		*/
/*		some of the comment indicators indicates code which	*/
/*		might be used in an environment requiring perfectly	*/
/*		reentrant libraries.					*/
/*	        							*/
/*		The "%" must be removed if the comment indicator	*/
/*		is removed.  It may be used to aid the user in 		*/
/*		modifying this routine for special applications.	*/
/*									*/
/*		     This code:						*/
/*			1.  Allocates memory for library static memory	*/
/*			    (554 bytes) and stack for one task.		*/
/*			2.  Zeros appropriate memory in the allocated	*/
/*			    _PID_REG relative memory.			*/
/*			3.  Zeros a section named "userzero" which	*/
/*			    might contain uninitialized static data	*/
/*			    for all user tasks.				*/
/*									*/
/*		     Also, the user must:				*/
/*			1.  Execute initialization of __HEAP only	*/
/*			    once, preferably in the kernel.		*/
/*			2.  Disable interrupts while code which		*/
/*			    allocates data and stack space is executing.*/
/*			3.  Disable interrupts whenever the routine	*/
/*			    sbrk() is running.				*/
/*			4.  Modify the linker command file		*/
/*			    appropriately.				*/
/*		     How interrupts are disabled for 2. and 3. above is	*/
/*		     left to the user.					*/
/*									*/
/*		     Please see the section "Multi-Tasking Environments"*/
/*		     in the chapter "Embedded Environments" in the	*/
/*		     "MCC68K Reference Manual" for more detail.	 Note 	*/
/*		     that the sample code in this file corresponds most */
/*		     closely to the approach described in		*/
/*		     "Multi-Tasking Environments" as opposed to		*/
/*		     "Multi-Tasking Environments - a Second Approach".	*/
/*									*/
/*		     The user must also manually remove the statement	*/
/*									*/
/*	MOVE.L	#?_STRIP_QUOTES(_PID_REG),_STRIP_QUOTES (_PID_REG)	*/
/*									*/
/*		     if either of the two above approaches is used.	*/
/************************************************************************/
           
/*
Change history:
	11/16/93 jpc -	Added labels "__simulated_halt" and "__mri_start"
	01/14/94 jpc -  "link-time pc" now procured with pc-relative address".
			code to initialize cache on 68040 and 68ec040.
*/

#pragma option -Qws		/* OPTION TO ELIMINATE COMPILER WARNING */

#include "asm_addr.h"

#pragma asm
              OPT	CASE
	      COMMON	stack,,D	    * DECLARE stack SECTION
              SECTION   code,,C		    * CODE SECTION
	      XDEF	__mri_start	    * ENTRY POINT SYMBOL USABLE BY C
	      XDEF	mri_start	    * ALTERNATE ENTRY POINT SYMBOL 
	      XDEF	__ENTRY		    * ENTRY POINT SYMBOL USABLE BY C
	      XDEF	ENTRY		    * TRADITIONAL ENTRY POINT
	      XDEF	__simulated_halt    * SYMBOL USED BY DEBUGGER TO LOCATE
					    * PROGRAM EXIT POINT
	      XDEF	__exit
*
              XREF      __START
              XREF      _exit
              XREF      _memset
#if _STR_CMP (_PID_REG, "a") && _STR_CMP (_PID_REG, "p")
	      XREF	?_STRIP_QUOTES(_PID_REG)
#endif
*
	      ALIGN	2
__mri_start:
mri_start:
__ENTRY:
ENTRY:
	      LEA.L	.STARTOF.(stack)+.SIZEOF.(stack),SP
 						* INITIALIZE STACK POINTER TO -
 						* BE POSITION INDEPENDENT
 	      MOVE.L	__PC_LABEL(PC),D2	* LINK TIME PC
 	      LEA.L	__PC_LABEL(PC),A0	* RUN-TIME PC
 	      SUB.L	A0,D2			* GET PC DIFFERENCE IF CHANGED -
	      					* D2 MUST NOT BE MODIFIED
 	      SUB.L	D2,SP			* ADJUST STACK POINTER
 
 	      LEA.L     __HEAP,A0	        * INITIALIZE HEAP POINTER TO -
 						* BE POSITION INDEPENDENT
 	      SUB.L	D2,A0			* ADJUST HEAP ADDRESS WITH -
 						* PREVIOUSLY CALCULATED PC -
 						* DIFFERENCE
	      MOVE.L	A0,A1			* SAVE A0
              ADDQ.L    #4,A0			* __HEAP POINTS TO ADDRESS -
              MOVE.L    A0,(A1)			* JUST BEYOND ITSELF
              MOVE.L    #0,A6			* FRAME POINTER INITIALIZATION
*%*
*%*	      ALLOCATE SPACE FOR LIBRARY DATA AND STACK FOR 1 TASK AND
*%*	      ADJUST POINTERS APPROPRIATELY 
*%*
*%*	      ALLOCATE MEMORY FOR DATA AND STACK SPACE FROM HEAP SPACE 
*%*
*%  	      MOVE.L	A0,_STRIP_QUOTES(_PID_REG)
*%  	      MOVE.L	_STRIP_QUOTES(_PID_REG),D0
*%	      ADD.L	#.SIZEOF.(zerovars)+.SIZEOF.(stack)+3,D0
*%	      LSR.L	#2,D0		* SHIFT TO LONG WORD ALIGN
*%	      LSL.L	#2,D0		* AND SHIFT BACK
*%	      MOVE.L	D0,SP		* PLACE IN STACK POINTER
*%	      MOVE.L	SP,(A1)		* ADJUST HEAP ADDRESS
*
*	      THE FOLLOWING CODE CLEARS THE zerovars SECTION
*
	      LEA.L	.STARTOF.(zerovars),A0	* START OF SECTION
	      SUB.L	D2,A0			* ADJUST HEAP ADDRESS WITH -
						* PREVIOUSLY CALCULATED PC -
						* DIFFERENCE
*
*%	      MOVE.L	_STRIP_QUOTES(_PID_REG),A0	* START OF THE SECTION
*%*							* ADD OFFSET TO _PID_REG
*%	      ADD.L	#?_STRIP_QUOTES(_PID_REG),_STRIP_QUOTES(_PID_REG)
*%*							* SUBTRACT SECTION START
*%	      ADD.L	#-.STARTOF.(zerovars),_STRIP_QUOTES(_PID_REG)
*
	      MOVE.L	#.SIZEOF.(zerovars),-(SP)	* LENGTH OF THE SECTION
	      CLR.L	-(SP)		    		* ZERO TO STACK
	      MOVE.L	A0,-(SP)			* START OF SECTION
	      JSR	C_ADDR(_memset)			* CALL MEMSET TO ZERO
	      ADD.L	#12,SP				* SHRINK STACK
*%*
*%*	      THE FOLLOWING CODE CLEARS THE userzero SECTION, IF IT EXISTS
*%*
*%	      MOVE.L	#.SIZEOF.(userzero),-(SP)	* LENGTH OF THE SECTION
*%	      CLR.L	-(SP)		    		* ZERO TO STACK
*%	      MOVE.L	#.STARTOF.(userzero),-(SP)	* SECTION START
*%	      SUB.L	D2,(SP)				* ADJUST ADDRESS AS -
*%							* BEFORE
*%	      JSR	C_ADDR(_memset)			* CALL MEMSET TO ZERO
*%	      ADD.L	#12,SP				* SHRINK STACK
*
*
#if _STR_CMP (_PID_REG, "a") && _STR_CMP (_PID_REG, "p")
*
*	      THE FOLLOWING STATEMENT SHOULD NOT BE USED IF POSITION
*	      INDEPENDENT ADDRESSING IS USED FOR FULLY RE-ENTRANT LIBRARIES.
*
							* PID_REG INITIALIZATION
	      MOVE.L	#?_STRIP_QUOTES(_PID_REG),_STRIP_QUOTES (_PID_REG)
#endif
*
*	      CALL TO ROUTINE WHICH ENABLES CACHE
*	      FOR 68040 AND 68EC040
*
#if ENABLE_68040_CACHE
              JSR       C_ADDR(__enable_cache)		* ENABLE CACHE
#endif
*
*	      THE ROUTINE __START INITIALIZES ALL NON-ZERO MEMORY,
*	      OPENS stdin, stdout, stderr, stdaux, AND stdprn.  IT THEN
*	      CALLS main.
*
              JSR       C_ADDR(__START)		* CALL C INITIALIZATION ROUTINE
*
*	      __START SHOULD NEVER RETURN.  HOWEVER, IF IT DOES, CALL exit()
*	      TO TERMINATE
*
	      MOVE.L	D2,-(SP)		* PUSH RETURN VALUE ON STACK
	      JSR	C_ADDR(_exit)		* CALL EXIT -- NO RETURN
*
*	      __exit IS CALLED BY exit().
*	      IT THEN EXECUTES A STOP INSTRUCTION.
*
__exit:
__simulated_halt:
	      STOP      #$2700
              BRA.S	__exit
              RTS
*
*	      DEFINE _enable_cache ROUTINE TO ENABLE CACHES
*	      FOR 68040 AND 68EC040.
*
#if ENABLE_68040_CACHE

__enable_cache:
	      MOVE.L	#007EC020H,D0		* LOGICAL ADDRESS BASE 0
                                		* LOGICAL ADDRESS MASK 
						*	0X3F (ALL ONES) 
						* ENABLED
                                		* IGNORE FC2 WHEN MATCHING
                                		* U1,U2 = 0 (IGNORE ALTERNATE
						*	ADDRESS SPACES)
                                 		* CACHABLE, COPYBACK
						* NOT WRITE PROTECTED
    #if _68040
	      MOVEC	D0,DTT0
    #else
              MOVEC	D0,DACR0
    #endif

              CINVA	IC
	      CINVA	DC
              MOVE.L	#80008000H,D0
	      MOVEC	D0,CACR
              RTS

#endif

*
*
*	DEFINE CODE LABEL THAT CONTAINS LINK TIME PROGRAM/LOCATION COUNTER
*	FOR CALCULATING POSITION INDEPENDENT STACK TOP AND HEAP
*
	      SECTION	code,,C
__PC_LABEL    DC.L	*
*
*	DEFINE THE SECTION heap AND DEFINE __HEAP SYMBOL
*
              SECTION   heap,,D			* HEAP SECTION
              XDEF      __HEAP            
__HEAP	      DS.L      1
              END       ENTRY
#pragma	endasm

#endif /* EXCLUDE_entry */

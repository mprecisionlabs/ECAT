#include "lib_top.h"


#pragma -nx


/*
; ----------------------------------------------------------------------------
; THIS INFORMATION IS PROPRIETARY TO
; MICROTEC RESEARCH, INC.
; 2350 Mission College Blvd.
; Santa Clara, CA 95054
; USA
; ----------------------------------------------------------------------------
; Copyright (c) 1993  Microtec Research, Inc.
; All Rights Reserved
; ----------------------------------------------------------------------------
;
;The software source code contained in this file and in the related
;header files is the property of Microtec Research, Inc. and may only be
;used under the terms and conditions defined in the Microtec Research
;license agreement. You may modify this code as needed but you must retain
;the Microtec Research, Inc. copyright notices, including this statement.
;Some restrictions may apply when using this software with non-Microtec
;Research software.  In all cases, Microtec Research, Inc. reserves all rights.
*/

/*
	This routine test a bug which was found by MRI engineering.  Namely
	the precision flag was not getting reset when a new format was
	encountered.  For example, in the case below, this would cause the
	0's prepended to the 1234 to not get written.
*/

/*
	Change history:
	12/17/93 jpc -	Written
*/


#include <stdio.h>
#include <string.h>
#include "message.h"

char output[100];
char correct[] = "        25  00001234";


main () {

    sprintf (output, "%10.0d  %08x", 25, 0x1234);
    
    if (strcmp (output, correct)) {
        printf ("%10.0d  %08x\n", 25, 0x1234);
	printf (ERRORS);
    }
    else {
	printf (NO_ERRORS);
    }
}

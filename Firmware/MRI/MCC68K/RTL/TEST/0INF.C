#include "lib_top.h"


#pragma	option	-nx

/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.       		    */
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

/*
**
**	Tests infinity and NAN cases
*/

#include <stdio.h>

#include "message.h"

#if _LITTLE_ENDIAN
#	define ADJUST(cn,type)	((sizeof(type) == sizeof(double)) ? cn+1 : cn)
#else
#	define ADJUST(cn,type)	(cn)
#endif

int errorCount;

main()
{
	double		da = 0.0, db = 0.0, dc = 0.0;
	float		fa = 0.0, fb = 0.0, fc = 0.0;
	long		*cn;

#if _MCC86		/* inifinity and NAN are not supported by '86 */
	printf (NO_ERRORS);
	return	0;
#endif

#if ! __OPTION_VALUE("-Kq")

	/*
	**	Converion testing
	*/
	cn = (long *)&fa;
	cn = ADJUST(cn,float);
	*cn = 0x7f800000;		/* set float to infinity */
	da = (double)fa;
	cn = (long *)&da;
	cn = ADJUST(cn,double);
	if (*cn != 0x7ff00000)
		Error("***** FAILED ***** double infinity = float infinity\n");

	cn = (long *)&da;
	cn = ADJUST(cn,double);
	*cn = 0x7ff00000;		/* set double to infinity */
	fa = (float)da;
	cn = (long *)&fa;
	cn = ADJUST(cn,float);
	if (*cn != 0x7f800000)
		Error("***** FAILED ***** float infinity = double infinity\n");


	cn = (long *)&fa;
	cn = ADJUST(cn,float);
	*cn = 0x7fc00f00;		/* set float to NAN */
	da = (double)fa;
	cn = (long *)&da;
	cn = ADJUST(cn,double);
	if (*cn <= 0x7ff00000)
		Error("***** FAILED ***** double NAN = float NAN\n");

	cn = (long *)&da;
	cn = ADJUST(cn,double);
	*cn = 0x7ff80f00;		/* set double to NAN */
	fa = (float)da;
	cn = (long *)&fa;
	cn = ADJUST(cn,float);
	if (*cn <= 0x7f800000)
		Error("***** FAILED ***** float NAN = double NAN\n");

#endif


	/*
	**	Test float infinity
	*/
	cn = (long *)&fa;
	cn = ADJUST(cn,float);
	*cn = 0x7f800000;		/* set to infinity */
	fb = 1.0;
	fc = fa + fb;
	cn = (long *)&fc;
	cn = ADJUST(cn,float);
	if (*cn != 0x7f800000)
		Error("***** FAILED ***** a + b = c float infinity\n");
	
	fc = fa - fb;
	cn = (long *)&fc;
	cn = ADJUST(cn,float);
	if (*cn != 0x7f800000)
		Error("***** FAILED ***** a - b = c float infinity\n");
	
	fc = fa * fb;
	cn = (long *)&fc;
	cn = ADJUST(cn,float);
	if (*cn != 0x7f800000)
		Error("***** FAILED ***** a * b = c float infinity\n");
	
	fc = fa / fb;
	cn = (long *)&fc;
	cn = ADJUST(cn,float);
	if (*cn != 0x7f800000)
		Error("***** FAILED ***** a / b = c float infinity\n");
	
	/*
	**	Test float NAN
	*/
	cn = (long *)&fa;
	cn = ADJUST(cn,float);
	*cn = 0x7fc00f00;		/* set to nan */
	fb = 1.0;
	fc = fa + fb;
	cn = (long *)&fc;
	cn = ADJUST(cn,float);
	if (*cn <= 0x7f800000)
		Error("***** FAILED ***** a + b = c float NAN\n");

	fc = fa - fb;
	cn = (long *)&fc;
	cn = ADJUST(cn,float);
	if (*cn <= 0x7f800000)
		Error("***** FAILED ***** a - b = c float NAN\n");

	fc = fa * fb;
	cn = (long *)&fc;
	cn = ADJUST(cn,float);
	if (*cn <= 0x7f800000)
		Error("***** FAILED ***** a * b = c float NAN\n");

	fc = fa / fb;
	cn = (long *)&fc;
	cn = ADJUST(cn,float);
	if (*cn <= 0x7f800000)
		Error("***** FAILED ***** a / b = c float NAN\n");

#if ! __OPTION_VALUE ("-Kq")

	/*
	**	Test double infinity
	*/
	cn = (long *)&da;
	cn = ADJUST(cn,double);
	*cn = 0x7ff00000;		/* set to infinity */
	db = 1.0;
	dc = da + db;
	cn = (long *)&dc;
	cn = ADJUST(cn,double);
	if (*cn != 0x7ff00000)
		Error("***** FAILED ***** a + b = c double infinity\n");
	
	dc = da - db;
	cn = (long *)&dc;
	cn = ADJUST(cn,double);
	if (*cn != 0x7ff00000)
		Error("***** FAILED ***** a - b = c double infinity\n");
	
	dc = da * db;
	cn = (long *)&dc;
	cn = ADJUST(cn,double);
	if (*cn != 0x7ff00000)
		Error("***** FAILED ***** a * b = c double infinity\n");
	
	dc = da / db;
	cn = (long *)&dc;
	cn = ADJUST(cn,double);
	if (*cn != 0x7ff00000)
		Error("***** FAILED ***** a / b = c double infinity\n");
	
#endif

	/*
	**	Test double NAN
	*/
	cn = (long *)&da;
	cn = ADJUST(cn,double);
	*cn = 0x7ff80f00;		/* set to nan */
	db = 1.0;
	dc = da + db;
	cn = (long *)&dc;
	cn = ADJUST(cn,double);
	if (*cn <= 0x7ff00000)
		Error("***** FAILED ***** a + b = c double NAN\n");

	dc = da - db;
	cn = (long *)&dc;
	cn = ADJUST(cn,double);
	if (*cn <= 0x7ff00000)
		Error("***** FAILED ***** a - b = c double NAN\n");

	dc = da * db;
	cn = (long *)&dc;
	cn = ADJUST(cn,double);
	if (*cn <= 0x7ff00000)
		Error("***** FAILED ***** a * b = c double NAN\n");

	dc = da / db;
	cn = (long *)&dc;
	cn = ADJUST(cn,double);
	if (*cn <= 0x7ff00000)
		Error("***** FAILED ***** a / b = c double NAN\n");

	if (errorCount)
	    printf (ERRORS);
	else
	    printf (NO_ERRORS);

}

Error (char * output) {
    printf (output);
    errorCount++;
    }
 

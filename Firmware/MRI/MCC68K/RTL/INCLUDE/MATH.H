
/************************************************************************/
/* Copyright (c) 1988-1991,1993 Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
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

/*  <math.h>	Microtec Research, Inc. ANSI C Compiler  */
/* %W% %G% */

#ifndef __MATH_H

    #define	__MATH_H	1

    #undef	HUGE_VAL
    #define	HUGE_VAL	_HUGEVAL()

    #if __STDC__

	#if __cplusplus
	    extern "C" {
	#endif

	extern double _HUGEVAL(void);
	extern double acos(double);
	extern double asin(double);
	extern double atan(double);
	extern double atan2(double, double);
	extern double cos(double);
	extern double sin(double);
	extern double tan(double);
	extern double cosh(double);
	extern double sinh(double);
	extern double tanh(double);
	extern double exp(double);
	extern double frexp(double, int *);
	extern double ldexp(double, int);
	extern double log(double);
	extern double log10(double);
	extern double modf(double, double *);
	extern double pow(double, double);
	extern double sqrt(double);
	extern double ceil(double);
	extern double fabs(double);
	extern double floor(double);
	extern double fmod(double, double);

	#if __cplusplus
	    }
	#endif

    #else /* !__STDC__ */

	extern double _HUGEVAL();
	extern double acos();
	extern double asin();
	extern double atan();
	extern double atan2();
	extern double cos();
	extern double sin();
	extern double tan();
	extern double cosh();
	extern double sinh();
	extern double tanh();
	extern double exp();
	extern double frexp();
	extern double ldexp();
	extern double log();
	extern double log10();
	extern double modf();
	extern double pow();
	extern double sqrt();
	extern double ceil();
	extern double fabs();
	extern double floor();
	extern double fmod();

    #endif /* __STDC__ */

#endif /* end __MATH_H */


/************************************************************************/
/* Copyright (c) 1988-1989  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

/*  <math.h>	Microtec Research, Inc. ANSI C Compiler  */
/* @(#)math.h	1.15 6/26/90 */

#ifndef __MATH_H
#define __MATH_H

#if ! _FPU && _MATHLIBF && __STDC__

#include <mathf.h>

#else			/* ! (! _FPU && _MATHLIBF && __STDC__) */

#undef	HUGE_VAL

#define HUGE_VAL 1.7976931348623157E+308 /* approx. largest F.P. number */

#if _FPU		/* Convert math functions to '881 versions */

#undef	atan2
#undef	ceil
#undef	floor
#undef	frexp
#undef	ldexp
#undef	modf
#undef	pow
#undef	fmod

#define	atan2	_atan2881
#define ceil	_ceil881
#define floor	_floor881
#define	frexp	_frexp881
#define ldexp	_ldexp881
#define modf	_modf881
#define	pow	_pow881
#define	fmod	_fmod881

#endif	/* _FPU */

#if __STDC__

#if __cplusplus
extern "C" {
#endif

extern double acos(double x);
extern double asin(double x);
extern double atan(double x);
extern double atan2(double y, double x);
extern double cos(double x);
extern double sin(double x);
extern double tan(double x);
extern double cosh(double x);
extern double sinh(double x);
extern double tanh(double x);
extern double exp(double x);
extern double frexp(double value, int *exp);
extern double ldexp(double x, int exp);
extern double log(double x);
extern double log10(double x);
extern double modf(double value, double *iptr);
extern double pow(double x, double y);
extern double sqrt(double x);
extern double ceil(double x);
extern double fabs(double x);
extern double floor(double x);
extern double fmod(double x, double y);

#if __cplusplus
}
#endif

#else /* !__STDC__ */

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

#endif /* ! (! _FPU && _MATHLIBF && __STDC__) */

#endif /* end __MATH_H */

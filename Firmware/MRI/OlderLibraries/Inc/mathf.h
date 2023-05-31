
/************************************************************************/
/* Copyright (c) 1988-1989  Microtec Research, Inc.			*/
/* ALL RIGHTS RESERVED							*/
/************************************************************************/

/*  <mathf.h>	Microtec Research, Inc. ANSI C Compiler  */
/* @(#)mathf.h	1.7 6/26/90 */

#ifndef __MATHF_H
#define __MATHF_H

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

#else	/* ! _FPU */

#if __STDC__

#undef	acos
#undef	asin
#undef	atan
#undef	atan2
#undef	cos
#undef	sin
#undef	tan
#undef	cosh
#undef	sinh
#undef	tanh
#undef	exp
#undef	frexp
#undef	ldexp
#undef	log
#undef	log10
#undef	modf
#undef	pow
#undef	sqrt
#undef	ceil
#undef	fabs
#undef	floor
#undef	fmod

#define	acos	acosf
#define	asin	asinf
#define	atan	atanf
#define	atan2	atan2f
#define	cos	cosf
#define	sin	sinf
#define	tan	tanf
#define	cosh	coshf
#define	sinh	sinhf
#define	tanh	tanhf
#define	exp	expf
#define	frexp	frexpf
#define	ldexp	ldexpf
#define	log	logf
#define	log10	log10f
#define	modf	modff
#define	pow	powf
#define	sqrt	sqrtf
#define	ceil	ceilf
#define	fabs	fabsf
#define	floor	floorf
#define	fmod	fmodf

#if __cplusplus
extern "C" {
#endif

extern float	acosf(float x);
extern float	asinf(float x);
extern float	atanf(float x);
extern float	atan2f(float y, float x);
extern float	cosf(float x);
extern float	sinf(float x);
extern float	tanf(float x);
extern float	coshf(float x);
extern float	sinhf(float x);
extern float	tanhf(float x);
extern float	expf(float x);
extern float	frexpf(float value, int *exp);
extern float	ldexpf(float x, int exp);
extern float	logf(float x);
extern float	log10f(float x);
extern float	modff(float value, float *iptr);
extern float	powf(float x, float y);
extern float	sqrtf(float x);
extern float	ceilf(float x);
extern float	fabsf(float x);
extern float	floorf(float x);
extern float	fmodf(float x, float y);

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

#endif	/* ! _FPU */

#endif /* end __MATHF_H */


/************************************************************************/
/* Copyright (c) 1988-1991  Microtec Research, Inc.			*/
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

#ifndef __STDARG_H
#define __STDARG_H	1

/************************************************************************/
/* Note: __save_args(p) is a special function recognized by the		*/
/*	compiler that causes "p" and all parameters following it to be	*/
/*	saved into the parameter block during the prolog.  It returns	*/
/*	the address of	p.  It causes no external code to be linked in.	*/
/************************************************************************/
/*  <stdarg.h> Microtec Research, Inc. ANSI C Compiler  */

/* %W% %G% */

#if defined(_MCCSP) || defined(_MCC88K)

    #if __cplusplus
	extern "C" char *__save_args(...);
	extern "C" char *__get_arg(...);
    #endif

    #ifndef _VA_LIST_DEFINED
	typedef char *va_list;
	#define	_VA_LIST_DEFINED	1
    #endif

    #define va_start(ap,p) ((void)((ap) = (char *)__save_args(p)))
    #define va_arg(ap, type) (*(type *)(void *)__get_arg(&ap, *(type *)ap))
    #define va_end(ap) ((void)0)

#elif defined(_MCC29K)

    #if __cplusplus
	extern "C" char *__save_args(...);
    #endif

    #ifndef _VA_LIST_DEFINED
	typedef char *va_list;
	#define	_VA_LIST_DEFINED	1
    #endif

    #define va_start(ap,p) ((void)((ap) = ((char *)__save_args(p) + sizeof(p))))
    #define va_arg(ap,type) ((type *)(void *)((ap) += (sizeof(type))))[-1]
    #define va_end(ap) ((void)0)

#elif defined(_MCC80)

    #if __cplusplus
	extern "C" int __save_args(...);
    #endif

    #ifndef _VA_LIST_DEFINED
	typedef struct	{
	    char *list;     	/* remaining arguments on the list */
	    int  arg;       	/* position on the stack */
	    } va_list;
	#define	_VA_LIST_DEFINED	1
    #endif

    #define va_start(ap,param) ((void)((ap.list)=(char*)(&param+1)), \
                                   ((ap.arg)=(int)__save_args()))

    #if __STDC__
	extern char *_va_arg(va_list *, int);
    #endif /* ! __STDC__ */

    #define va_arg(ap,type) ((ap.arg > 3) \
        ? ((type*)(void *)((ap.list)+=sizeof(type)))[-1] \
        : ((type*)(void *)(_va_arg(&ap,sizeof(type))))[-1])

    #define va_end(ap) ((void)0)

#elif defined(_MCC960)

    #if __cplusplus
	extern "C" int __save_args(...);
    #endif


    #ifndef _VA_LIST_DEFINED
	typedef struct {
	    char *list;		/* base address of parameter block */
	    int  arg;		/* offset of current parameter */
	    } va_list;
	#define	_VA_LIST_DEFINED	1
    #endif

    #define va_start(ap,p) \
        ((void)(ap.list=(char*)(&p+1)-(ap.arg=__save_args(p)+sizeof(p))))

    #define va_arg(ap,type) (((type*)(void *)(ap.list+__va_incr(ap,type)))[-1])

    #define va_end(ap) ((void)0)

    #define __va_incr(ap,type) \
	(((ap.arg<48) && ((ap.arg>48-sizeof(type)) || (sizeof(type)>16))) \
	? ap.arg = sizeof(type)+48 \
	: ap.arg = sizeof(type)+((ap.arg+__va_mask(type))&~__va_mask(type)))

    #define __va_mask(type) ((sizeof(type)<=4) ? 3 : (sizeof(type)<=8) ? 7 : 15)

#else

    #ifndef _VA_LIST_DEFINED
	typedef char *va_list;
	#define	_VA_LIST_DEFINED	1
    #endif

    #if _LITTLE_ENDIAN
	#define va_start(ap,param) ((void)((ap) = \
	    (char *)&param+((sizeof(param)+sizeof(int)-1)&~(sizeof(int)-1))))
    #else
	#define va_start(ap,param) ((void)((ap)=(char *)(&param+1)))
    #endif

    #if _BIG_ENDIAN && _CXFE
	#define va_arg(ap,type) (*(type *)((void *) \
	    (((ap)+=(sizeof(type)+sizeof(int)-1)&~(sizeof(int)-1)) \
	    -((sizeof(type)+sizeof(int)-1)&~(sizeof(int)-1)) \
	    +(__is_integral_type(type) * (sizeof(int)-sizeof(type))))))
    #else
	#define va_arg(ap,type) (*(type *)((void *) \
	    (((ap)+=(sizeof(type)+sizeof(int)-1)&~(sizeof(int)-1)) \
	    -((sizeof(type)+sizeof(int)-1)&~(sizeof(int)-1)))))
    #endif

    #define va_end(ap) ((void)0)

#endif

#endif /* end __STDARG_H */

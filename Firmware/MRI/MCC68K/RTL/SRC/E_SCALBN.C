#include "lib_top.h"


#if EXCLUDE_e_scalbn || _80960SB || _80960KB || _80960MC \
        || (_MCC68K && _FPU) || (__OPTION_VALUE ("-Kq")) || INCLUDE_FAST_POW
    #pragma option -Qws                 /* Prevent compiler warning */
#else


/* @(#)s_scalbn.c 5.1 93/09/24 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/* 
 * scalbn (double x, int n)
 * scalbn(x,n) returns x* 2**n  computed by  exponent  
 * manipulation rather than by actually performing an 
 * exponentiation or a multiplication.
 */

/* Change history (Microtec Research):
 *	03/10/94 jpc -	removed exception generation by calling _fp_error
 *			instead.  Used _LITTLE_ENDIAN to figure out how
 *			to crack doubles.
 */

#include "e_fdlibm.h"

#if _sun 
    #undef      HUGE            /* Zap sun #define */
    #undef	OVERFLOW
    #undef	UNDERFLOW
#endif 

#include "m_incl.h"

static const double
two54   =  1.80143985094819840000e+16, /* 0x43500000, 0x00000000 */
twom54  =  5.55111512312578270212e-17; /* 0x3C900000, 0x00000000 */

#if _LITTLE_ENDIAN || __i960_BIG_ENDIAN__
    #define     HI      1
    #define     LO      0
#else
    #define     HI      0
    #define     LO      1
#endif

#define ZOT_SIGN(x,y)   ((((UInt32 *)(&x))[HI]) |= \
				(((UInt32 *)(&y))[HI] & 0x80000000))

	double _scalbn (double x, int n)
{
	Int32  k,hx,lx;
	double temp;

	hx = ((Int32 *)&x)[HI];
	lx = ((Int32 *)&x)[LO];

        k = (hx&0x7ff00000)>>20;		/* extract exponent */
        if (k==0) {				/* 0 or subnormal x */
            if ((lx|(hx&0x7fffffff))==0) return x; /* +-0 */
	    x *= two54; 
	    hx = ((Int32 *)&x)[HI];
	    k = ((hx&0x7ff00000)>>20) - 54; 
            if (n< -50000) {
		temp = _fp_error((UInt32) (FP_pow | UNDERFLOW), 100, ZERO, 1);
		ZOT_SIGN (temp, x);
		return temp; 			/*underflow*/
		}
	    }

        if (k==0x7ff) return x;			/* NaN or Inf - no exception tho */

        k = k+n; 
        if (k >  0x7fe) {
	    temp = _fp_error((UInt32) (FP_pow | OVERFLOW));
	    ZOT_SIGN (temp, x);
	    return temp;			/* overflow  */
	    }


        if (k > 0) {				/* normal result */
	    ((Int32 *)&x)[HI] = (hx&0x800fffff)|(k<<20);
	    return x;
	    }

        if (k <= -54) {
            if (n > 50000) {	/* in case integer overflow in n+k */
		temp = _fp_error((UInt32) (FP_pow | OVERFLOW));
		ZOT_SIGN (temp, x);
		return temp;			/* overflow  */
	    }
	    else {
		temp = _fp_error((UInt32) (FP_pow | UNDERFLOW), 100, ZERO, 1);
		ZOT_SIGN (temp, x);
		return temp; 			/*underflow*/
	    }
	}

        k += 54;				/* subnormal result */
        ((Int32 *)&x)[HI] = (hx&0x800fffff)|(k<<20);

        return x*twom54;
}

#endif	/* EXCLUDE_e_scalbn */

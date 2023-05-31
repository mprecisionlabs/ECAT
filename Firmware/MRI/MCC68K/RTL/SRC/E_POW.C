#include "lib_top.h"


#if EXCLUDE_e_pow || _80960SB || _80960KB || _80960MC \
        || (_MCC68K && _FPU) || (__OPTION_VALUE ("-Kq")) || INCLUDE_FAST_POW
    #pragma option -Qws                 /* Prevent compiler warning */
#else

/* @(#)e_pow.c 5.1 93/09/24 */
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

/* __ieee754_pow(x,y) return x**y
 *
 *		      n
 * Method:  Let x =  2   * (1+f)
 *	1. Compute and return log2(x) in two pieces:
 *		log2(x) = w1 + w2,
 *	   where w1 has 53-24 = 29 bit trailing zeros.
 *	2. Perform y*log2(x) = n+y' by simulating muti-precision 
 *	   arithmetic, where |y'|<=0.5.
 *	3. Return x**y = 2**n*exp(y'*log2)
 *
 * Special cases:
 *	1.  (anything) ** 0  is 1
 *	2.  (anything) ** 1  is itself
 *	3.  (anything) ** NAN is NAN
 *	4.  NAN ** (anything except 0) is NAN
 *	5.  +-(|x| > 1) **  +INF is +INF
 *	6.  +-(|x| > 1) **  -INF is +0
 *	7.  +-(|x| < 1) **  +INF is +0
 *	8.  +-(|x| < 1) **  -INF is +INF
 *	9.  +-1         ** +-INF is NAN
 *	10. +0 ** (+anything except 0, NAN)               is +0
 *	11. -0 ** (+anything except 0, NAN, odd integer)  is +0
 *	12. +0 ** (-anything except 0, NAN)               is +INF
 *	13. -0 ** (-anything except 0, NAN, odd integer)  is +INF
 *	14. -0 ** (odd integer) = -( +0 ** (odd integer) )
 *	15. +INF ** (+anything except 0,NAN) is +INF
 *	16. +INF ** (-anything except 0,NAN) is +0
 *	17. -INF ** (anything)  = -0 ** (-anything)
 *	18. (-anything) ** (integer) is (-1)**(integer)*(+anything**integer)
 *	19. (-anything except 0 and inf) ** (non-integer) is NAN
 *
 * Accuracy:
 *	pow(x,y) returns x**y nearly rounded. In particular
 *			pow(integer,integer)
 *	always returns the correct integer provided it is 
 *	representable.
 *
 * Constants :
 * The hexadecimal values are the intended ones for the following 
 * constants. The decimal values may be used, provided that the 
 * compiler will convert from decimal to binary accurately enough 
 * to produce the hexadecimal values shown.
 */

/*
 * Modification history (Microtec Research) :
 *	03/10/94 jpc -	Converted from original SunPro source. Main changes
 *			are use of _fp_error to avoid exceptions and use of
 *			_LITTLE_ENDIAN symbol to determine how to access 
 *			pieces of a double.
 *	04/28/48 jpc -	Executes direct multiplications for smaller
 *			integral exponents.
 */


#include <math.h>

#if _sun
    #undef	HUGE		/* Zap sun #define */
#endif

#include "e_fdlibm.h"
#include "m_incl.h"

static const double 
bp[] = {1.0, 1.5,},
dp_h[] = { 0.0, 5.84962487220764160156e-01,}, /* 0x3FE2B803, 0x40000000 */
dp_l[] = { 0.0, 1.35003920212974897128e-08,}, /* 0x3E4CFDEB, 0x43CFD006 */
zero    =  0.0,
one	=  1.0,
two	=  2.0,
two53	=  9007199254740992.0,	/* 0x43400000, 0x00000000 */

	/* poly coefs for (3/2)*(log(x)-2s-2/3*s**3 */
L1  =  5.99999999999994648725e-01, /* 0x3FE33333, 0x33333303 */
L2  =  4.28571428578550184252e-01, /* 0x3FDB6DB6, 0xDB6FABFF */
L3  =  3.33333329818377432918e-01, /* 0x3FD55555, 0x518F264D */
L4  =  2.72728123808534006489e-01, /* 0x3FD17460, 0xA91D4101 */
L5  =  2.30660745775561754067e-01, /* 0x3FCD864A, 0x93C9DB65 */
L6  =  2.06975017800338417784e-01, /* 0x3FCA7E28, 0x4A454EEF */
P1   =  1.66666666666666019037e-01, /* 0x3FC55555, 0x5555553E */
P2   = -2.77777777770155933842e-03, /* 0xBF66C16C, 0x16BEBD93 */
P3   =  6.61375632143793436117e-05, /* 0x3F11566A, 0xAF25DE2C */
P4   = -1.65339022054652515390e-06, /* 0xBEBBBD41, 0xC5D26BF1 */
P5   =  4.13813679705723846039e-08, /* 0x3E663769, 0x72BEA4D0 */
lg2  =  6.93147180559945286227e-01, /* 0x3FE62E42, 0xFEFA39EF */
lg2_h  =  6.93147182464599609375e-01, /* 0x3FE62E43, 0x00000000 */
lg2_l  = -1.90465429995776804525e-09, /* 0xBE205C61, 0x0CA86C39 */
ovt =  8.0085662595372944372e-0017, /* -(1024-log2(ovfl+.5ulp)) */
cp    =  9.61796693925975554329e-01, /* 0x3FEEC709, 0xDC3A03FD =2/(3ln2) */
cp_h  =  9.61796700954437255859e-01, /* 0x3FEEC709, 0xE0000000 =(float)cp */
cp_l  = -7.02846165095275826516e-09, /* 0xBE3E2FE0, 0x145B01F5 =tail of cp_h*/
ivln2    =  1.44269504088896338700e+00, /* 0x3FF71547, 0x652B82FE =1/ln2 */
ivln2_h  =  1.44269502162933349609e+00, /* 0x3FF71547, 0x60000000 =24b 1/ln2*/
ivln2_l  =  1.92596299112661746887e-08; /* 0x3E54AE0B, 0xF85DDF44 =1/ln2 tail*/

#define	SQRT_LARGEST	(1.34078079299425956110e+154)
			/* Square root of largest non-infinite double */

#if _LITTLE_ENDIAN || __i960_BIG_ENDIAN__
    #define	HI	1
    #define 	LO	0
#else
    #define	HI	0
    #define	LO	1
#endif

#define	ZOT_SIGN(x,y)	((((UInt32 *)(&x))[HI]) |= \
				(((UInt32 *)(&y))[HI] & 0x80000000))

#define DIRECT_MULTIPLICATION_CUTOFF	(0xffffUL)	/* must never be set */
							/* to 0xffffffff */

double pow (double x, double y)
{
	double		z,ax,z_h,z_l,p_h,p_l;
	double		y1,t1,t2,r,s,t,u,v,w;
	double		temp;
	Int32		i,j,yisint,n;
	Int32		hx,hy,ix,iy;
	UInt32		lx,ly;
	int		k;


	hx = ((Int32*)&x)[HI];
	lx = ((Int32*)&x)[LO];
	hy = ((Int32*)&y)[HI];
	ly = ((Int32*)&y)[LO];

	ix = hx&0x7fffffff;
	iy = hy&0x7fffffff;

    /* y==zero: x**0 = 1 */
	if((iy|ly)==0) return one; 	

    /* +-NaN return x+y */
	if(ix > 0x7ff00000 || ((ix==0x7ff00000)&&(lx!=0)))
	    return  _fp_error((UInt32) (FP_pow | SIGNAN), x);
	if (iy > 0x7ff00000 || ((iy==0x7ff00000)&&(ly!=0))) 
	    return  _fp_error((UInt32) (FP_pow | SIGNAN), y);

    /* determine if y is an odd int when x < 0
     * yisint = 0	... y is not an integer
     * yisint = 1	... y is an odd int
     * yisint = 2	... y is an even int
     */
	yisint  = 0;
	if(hx<0) {	
	    if(iy>=0x43400000) yisint = 2; /* even integer y */
	    else if(iy>=0x3ff00000) {
		k = (iy>>20)-0x3ff;	   /* exponent */
		if(k>20) {
		    j = ly>>(52-k);
		    if((j<<(52-k))==ly) yisint = 2-(j&1);
		} else if(ly==0) {
		    j = iy>>(20-k);
		    if((j<<(20-k))==iy) yisint = 2-(j&1);
		}
	    }		
	} 

    /* special value of y */
	if(ly==0) { 	
	    if (iy==0x7ff00000) {	/* y is +-inf */
	        if(((ix-0x3ff00000)|lx)==0)
		    return  _fp_error((UInt32) (FP_pow | OPERROR));
							/* +-1**inf is NaN */
	        else if (ix >= 0x3ff00000)/* (|x|>1)**+-inf = inf,0 */
		    return (hy>=0) ? 
			_fp_error((UInt32) (FP_pow | OVERFLOW)) :
			_fp_error((UInt32) (FP_pow | UNDERFLOW), 100, ZERO, 1);
	        else			/* (|x|<1)**-,+inf = inf,0 */
		    return (hy<0) ?
			- _fp_error((UInt32) (FP_pow | OVERFLOW)) :
			_fp_error((UInt32) (FP_pow | UNDERFLOW), 100, ZERO, 1);
	    } 
	    if(iy==0x3ff00000) {	/* y is  +-1 */
		if(hy<0) return one/x; else return x;
	    }
	    if(hy==0x40000000) {	/* y is  2 */
		if (fabs (x) <= SQRT_LARGEST) {
		    return x * x;
		} else {
		    return _fp_error((UInt32) (FP_pow | OVERFLOW));
		}
	    }
	    if(hy==0x3fe00000) {	/* y is  0.5 */
		if(hx>=0)	/* x >= +0 */
		    return sqrt(x);	
	    }
	}

	ax   = fabs(x);
    /* special value of x */
	if(lx==0) {
	    if(ix==0x7ff00000||ix==0||ix==0x3ff00000){
		z = ax;			/*x is +-0,+-inf,+-1*/
		if(hy<0) z = one/z;	/* z = (1/|x|) */
		if(hx<0) {
		    if(((ix-0x3ff00000)|yisint)==0) {	/* (-1)**non-int is NaN */
			z = _fp_error((UInt32) (FP_pow | OPERROR));
		    } else if(yisint==1) 
			z = -z;		/* (x<0)**odd = -(|x|**odd) */
		}
		return z;
	    }
	}
    
    /* (x<0)**(non-int) is NaN */
	if((((hx>>31)+1)|yisint)==0)
	    return _fp_error((UInt32) (FP_pow | OPERROR));

    /*  If y_arg is convertable to an exact integral compute by */
    /*  simply multiplying. */

	{
	double		runningProd;
	unsigned long	mask;
	double		fraction, integer;
	unsigned long	long_integer;

	fraction = modf (fabs(y), &integer);

        if ((fraction == 0.0) &&		/* Is y an integer? */
		(integer <= DIRECT_MULTIPLICATION_CUTOFF)) {
						/* y in correct range? */
            mask = 				/* set mask */
		(DIRECT_MULTIPLICATION_CUTOFF + 1) >> 1;
            runningProd = 1.;
	    long_integer = integer;		/* set integral value */

            while (mask) {
                if (runningProd != 1.)
                    runningProd *= runningProd;
                if (mask & long_integer) {
                    if (runningProd == 1.) {
                        runningProd = x;
                    }
                    else {
                        runningProd *= x;
                    }
                }    
                mask >>= 1;
            }

            if (runningProd == HUGE_VAL)        /* check for overflow */
                runningProd = _fp_error((UInt32) (FP_pow | OVERFLOW));
            else if (runningProd == -HUGE_VAL)
                runningProd = - _fp_error((UInt32) (FP_pow | OVERFLOW));
                                                /* return other values */
            return (y < 0.0)? (1./runningProd) : runningProd;
        }
	}    


    /* |y| is huge */
	if(iy>0x41e00000) { /* if |y| > 2**31 */
	    if(iy>0x43f00000){	/* if |y| > 2**64, must o/uflow */
		if(ix<=0x3fefffff)
		    return (hy<0) ?
			_fp_error((UInt32) (FP_pow | OVERFLOW)) :
			_fp_error((UInt32) (FP_pow | UNDERFLOW), 100, ZERO, 1);

		if(ix>=0x3ff00000)
		    return (hy>0) ?
			_fp_error((UInt32) (FP_pow | OVERFLOW)) :
			_fp_error((UInt32) (FP_pow | UNDERFLOW), 100, ZERO, 1);
	    }

	/* over/underflow if x is not close to one */
	    if(ix<0x3fefffff)
		return (hy<0) ?
		    _fp_error((UInt32) (FP_pow | OVERFLOW)) :
		    _fp_error((UInt32) (FP_pow | UNDERFLOW), 100, ZERO, 1);

	    if(ix>0x3ff00000)
		return (hy>0)? 
		    _fp_error((UInt32) (FP_pow | OVERFLOW)) :
		    _fp_error((UInt32) (FP_pow | UNDERFLOW), 100, ZERO, 1);

	/* now |1-x| is tiny <= 2**-20, suffice to compute 

	   log(x) by x-x^2/2+x^3/3-x^4/4 */
	    t = x-1;		/* t has 20 trailing zeros */
	    w = (t*t)*(0.5-t*(0.3333333333333333333333-t*0.25));
	    u = ivln2_h*t;	/* ivln2_h has 21 sig. bits */
	    v = t*ivln2_l-w*ivln2;
	    t1 = u+v;
	    ((Int32 *)&t1)[LO] = 0;
	    t2 = v-(t1-u);
	} else {
	    double s2,s_h,s_l,t_h,t_l;
	    n = 0;
	/* take care subnormal number */
	    if(ix<0x00100000) {
		ax *= two53;
		n -= 53;
		ix = ((Int32*)&ax)[HI];
	    }
	    n  += ((ix)>>20)-0x3ff;
	    j  = ix&0x000fffff;
	/* determine interval */
	    ix = j|0x3ff00000;		/* normalize ix */
	    if(j<=0x3988E) k=0;		/* |x|<sqrt(3/2) */
	    else if(j<0xBB67A) k=1;	/* |x|<sqrt(3)   */
	    else {k=0;n+=1;ix -= 0x00100000;}
	    ((Int32*)&ax)[HI] = ix;

	/* compute s = s_h+s_l = (x-1)/(x+1) or (x-1.5)/(x+1.5) */
	    u = ax-bp[k];		/* bp[0]=1.0, bp[1]=1.5 */
	    v = one/(ax+bp[k]);
	    s = u*v;
	    s_h = s;
	    ((Int32 *)&s_h)[LO] = 0;
	/* t_h=ax+bp[k] High */
	    t_h = zero;
	    ((Int32 *)&t_h)[HI] =((ix>>1)|0x20000000)+0x00080000+((Int32)k<<18);
	    t_l = ax - (t_h-bp[k]);
	    s_l = v*((u-s_h*t_h)-s_h*t_l);
	/* compute log(ax) */
	    s2 = s*s;
	    r = s2*s2*(L1+s2*(L2+s2*(L3+s2*(L4+s2*(L5+s2*L6)))));
	    r += s_l*(s_h+s);
	    s2  = s_h*s_h;
	    t_h = 3.0+s2+r;
	    ((Int32 *)&t_h)[LO] = 0;
	    t_l = r-((t_h-3.0)-s2);
	/* u+v = s*(1+...) */
	    u = s_h*t_h;
	    v = s_l*t_h+t_l*s;
	/* 2/(3log2)*(s+...) */
	    p_h = u+v;
	    ((Int32 *)&p_h)[LO] = 0;
	    p_l = v-(p_h-u);
	    z_h = cp_h*p_h;		/* cp_h+cp_l = 2/(3*log2) */
	    z_l = cp_l*p_h+p_l*cp+dp_l[k];
	/* log2(ax) = (s+..)*2/(3*log2) = n + dp_h + z_h + z_l */
	    t = (double)n;
	    t1 = (((z_h+z_l)+dp_h[k])+t);
	    ((Int32 *)&t1)[LO] = 0;
	    t2 = z_l-(((t1-t)-dp_h[k])-z_h);
	}

	s = one; /* s (sign of result -ve**odd) = -1 else = 1 */
	if((((hx>>31)+1)|(yisint-1))==0) s = -one;/* (-ve)**(odd int) */

    /* split up y into y1+y2 and compute (y1+y2)*(t1+t2) */
	y1  = y;
	((Int32 *)&y1)[LO] = 0;
	p_l = (y-y1)*t1+y*t2;
	p_h = y1*t1;
	z = p_l+p_h;
	j = ((Int32 *)&z)[HI];
	i = ((Int32 *)&z)[LO];

	if (j>=0x40900000) {				/* z >= 1024 */
	    if(((j-0x40900000)|i)!=0) {			/* if z > 1024 */
		temp = _fp_error ((UInt32) (FP_pow | OVERFLOW));
		ZOT_SIGN (temp, s);			/* set sign */
		return temp;
	    }
	    else {
		if(p_l+ovt>z-p_h) {			/* overflow */
		    temp = _fp_error ((UInt32) (FP_pow | OVERFLOW));
		    ZOT_SIGN (temp, s);			/* set sign */
		    return temp;
		}
	    }
	} else if((j&0x7fffffff)>=0x4090cc00 ) {	/* z <= -1075 */
	    if(((j-0xc090cc00)|i)!=0) {			/* z < -1075 */
		temp = _fp_error((UInt32) (FP_pow | UNDERFLOW), 100, ZERO, 1);
		ZOT_SIGN (temp, s);			/* set sign */
		return temp;
	    }
	    else {
		if(p_l<=z-p_h) {			/* underflow */
		    temp = _fp_error((UInt32) (FP_pow | UNDERFLOW), 100, ZERO, 1);
		    ZOT_SIGN (temp, s);			/* set sign */
		    return temp;
		}
	    }
	}
    /*
     * compute 2**(p_h+p_l)
     */
	i = j&0x7fffffff;
	k = (i>>20)-0x3ff;
	n = 0;
	if(i>0x3fe00000) {		/* if |z| > 0.5, set n = [z+0.5] */
	    n = j+(0x00100000>>(k+1));
	    k = ((n&0x7fffffff)>>20)-0x3ff;	/* new k for n */
	    t = zero;
	    ((Int32 *)&t)[HI] = (n&~(0x000fffff>>k));
	    n = ((n&0x000fffff)|0x00100000)>>(20-k);
	    if(j<0) n = -n;
	    p_h -= t;
	} 
	t = p_l+p_h;
	((Int32 *)&t)[LO] = 0;
	u = t*lg2_h;
	v = (p_l-(t-p_h))*lg2+t*lg2_l;
	z = u+v;
	w = v-(z-u);
	t  = z*z;
	t1  = z - t*(P1+t*(P2+t*(P3+t*(P4+t*P5))));
	r  = (z*t1)/(t1-two)-(w+z*w);
	z  = one-(r-z);
	j  = ((Int32 *)&z)[HI];
	j += (n<<20);

	if((j>>20)<=0)
	    z = _scalbn(z,n);	/* subnormal output */
	else
	    ((Int32 *)&z)[HI] += (n<<20);

	ZOT_SIGN (z, s);
	return z;
}

#endif	/* EXCLUDE_e_pow ... */

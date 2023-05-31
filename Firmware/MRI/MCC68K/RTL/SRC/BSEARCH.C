#include "lib_top.h"


#if EXCLUDE_bsearch
    #pragma option -Qws			/* Prevent compiler warning */
#else
 
/****************************************************************************/
/* THIS INFORMATION IS PROPRIETARY TO                                       */
/* MICROTEC RESEARCH, INC.                                                  */
/*--------------------------------------------------------------------------*/
/* Copyright (c) 1991, 1992 Microtec Research, Inc.                         */
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

/****************************************************************************/
/*	bsearch.c							    */
/*									    */
/*	bsearch (const void *key, const void *base, size_t nmemb,	    */
/*		size_t size, int (*compar)(const void *, const void *));    */
/*									    */
/*		The bsearch function searches an array of nmemb objects, the*/
/*		initial element of which is pointed to by base, for an 	    */
/*		array element that matches the object pointed to by key.    */
/*		The size of each element of the array is specified by size. */
/*									    */
/*		The comparison function pointed to by compar is called with */
/*		two arguments that point to the key object and to an array  */
/*		element, in that order.  The function shall return an	    */
/*		integer less than, equal to, or greater than zero if the key*/
/*		object is considered, respectively, to be less than, to     */
/*		match, or to be greater than the array element.  The array  */
/*		shall consist of:  all the elements that compare less than, */
/*		all the elements that compare equal to, and all the 	    */
/*		elements that compare greater than the key object, in that  */
/*		order.							    */
/*									    */
/*	returns --							    */
/*		The bsearch function returns a pointer to a matching element*/
/*		of the array, or a null pointer if no match is found.  If   */
/*		two elements compare as equal, which element is matched is  */
/*		unspecified.						    */
/*									    */
/*	Note --								    */
/*		While some of the constructs used in this routine might	    */
/*		seem to be flaky, they have been selected to generate	    */
/*		efficient assembly code.  Do not modify for efficiency	    */
/*		unless close examination of generated assembly is made	    */
/*		also.  In particular, the value of the test address is	    */
/*		not stored in a temporary -- instead the value is recomputed*/
/*		immediately before return.  This will actuall yield faster  */
/*		results in all but searches of very small lists.  (Saving   */
/*		that address takes time.)				    */
/*									    */
/*	History --							    */
/*		02/20/90 jpc:	Written					    */
/*									    */
/****************************************************************************/

#include <stdlib.h>

/*	Run-time reduction in strength */
#define	MULT(x,y) \
	(((y) == sizeof(char *)) ? ((x) * sizeof(char *)) : ((x) * (y)))

void *bsearch (const void *key, const void *base,
	size_t nmemb, size_t size,
	int (*compar)(const void *, const void *))
{
    register unsigned	top = 1;
    register unsigned	bottom = nmemb;
    register int	returnVal;
    register unsigned	test;

    if (nmemb == 0)				/* Degenerative case */
	return NULL;

    while (bottom >= top)			/* Allow for nmemb == 0 */
	{
	test = (bottom + top) / 2;		/* Compute mid point */

	if ((returnVal = (*compar)
		(key, (char *) base + (MULT(test - 1, size)))) < 0)
	    bottom = test - 1;
	else if (returnVal > 0)
	    top = test + 1;
	else
	    return (void *) ((char *) base + (MULT(test - 1, size)));
	}
    return NULL;
}
 
#endif /* EXCLUDE_bsearch */
 

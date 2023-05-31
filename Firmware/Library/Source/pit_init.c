#include "period.h"
#include "vectors.h"

extern volatile ulong	vectors[];
extern volatile ushort	PITR;

/*
 *	dt, the time for the periodic interval
 *	as defined in the 68332 SIM handbook. 
 *	p, a pointer to a function of type INTERRUPT!
 */

void	PIT_init(
short	dt,
void	(*p)())
{
	PITR = dt;
	PICR = (TIME_IRQ_LEVEL * 256) + (TIME_VECTOR_BASE); 
	vectors[TIME_VECTOR_BASE] = (long)p;
	vectors[TIME_VECTOR_BASE+256] = (long)p;
}

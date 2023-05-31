#include "eeprom.h"

extern volatile char EEPROM[];

/*
 *  Write_memory -- Write a block of data to the nonvolatile memory.
 *
 *       Syntax: int write_memory(int start, void *p, int size);
 *  Description: Writes size bytes of the memory pointed to
 *               by p into the nonvolatile memory starting at
 *               the address start.
 *      Returns: The address of the next unused memory address in the
 *               nonvolatile memory. Returns 0 if the there is not enough
 *               memory left.
 *      Example: The following example writes the variables x,y and z in
 *               nonvolatile memory.
 *    {
 *       int   next;
 *       next = 0;
 *       next = write_memory(next,&x,sizeof(x));
 *       next = write_memory(next,&y,sizeof(y));
 *       next = write_memory(next,&z,sizeof(z));
 *       if(next == 0) then error......
 *    }
 */

int	write_memory(
int	start,
void	*p,
int	size)
{
	short	cntr;
	short	timer;
	short	tsize;
	char	*p_ptr;
	char	*e_ptr;

	p_ptr = (char *)p;
	e_ptr = (char *)EEPROM;
	tsize = start + size;
	for(cntr = start, timer = 0; cntr < tsize; cntr++)
	{
		if((cntr > EEPROMSIZE) || (timer >= 4000))
			return(0);
		*e_ptr = *p_ptr;
		for(timer = 0; ((*e_ptr != *p_ptr) && timer < 4000); timer++)
			;
		p_ptr++;
		e_ptr++;
	}
	return(cntr);
}

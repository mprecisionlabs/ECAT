#include "eeprom.h"

extern volatile char EEPROM[];

/*
 *  Read_memory -- Get a block of data from the nonvolatile memory.
 *
 *       Syntax: int  read_memory(int start, void *p, int size);
 *  Description: Reads size bytes of the memory from the nonvolatile memory
 *               starting at location start and stores the bytes into the
 *               memory pointed to by p.
 *      Returns: The address of the next unused memory address in the
 *               nonvolatile memory. Returns 0 if the get was bigger than
 *               the memory left.
 *      Example: The following example gets the variables x,y and z from
 *               nonvolatile memory.
 *    {
 *       unsigned short next;
 *       next = 0;
 *       next = read_memory(next,&x,sizeof(x));
 *       next = read_memory(next,&y,sizeof(y));
 *       next = read_memory(next,&z,sizeof(z));
 *       if(next == 0) then error......
 *    }
 */

int	read_memory(
int	start,
void	*p,
int	size)
{
	short	cntr;
	short	tsize;
	char	*p_ptr;
	char	*e_ptr;

	p_ptr = (char *)p;
	e_ptr = (char *)EEPROM;
	tsize = start + size;
	for(cntr = start; cntr < tsize; cntr++)
	{
		if(cntr > EEPROMSIZE)
			return(0);
		*p_ptr = *e_ptr;
		e_ptr++;
		p_ptr++;
	}
	return(cntr);
}

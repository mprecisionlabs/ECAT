#include "key.h"

extern int	karray[];
extern int	khead;
extern int	ktail;

int	KEY_getraw(void)
{
	int	i;

	if(khead == ktail)
		return(0xFF);
	i = karray[ktail++];
	if(ktail == 20)
		ktail = 0;
	return(i);
}

#include "display.h"

void	LCD_puts(
char	*s)
{
	char	*cp;

	cp = s;
	while(*cp)
	{
		LCD_putc(*cp);
		cp++;
	}
}

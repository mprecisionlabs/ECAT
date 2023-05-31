#include "display.h"

void	LCD_normal(
int	x,
int	y,
int	x2,
int	y2)
{
	int	i;
	int	j;

	for(i = x*6; i <= (x2*6+5); i++)
		for(j = y*8; j <= (y2*8+7); j++)
			LCD_pixel(i,j,0);
}

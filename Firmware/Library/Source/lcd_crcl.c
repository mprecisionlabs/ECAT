#include <math.h>
#include "display.h"

void	LCD_circle(
int	x,
int	y,
int	r)
{
	float	xf;
	int	i;
	int	yi;

	for(yi = 0; yi <= (r * 8 / 10); yi++) 
	{
		xf = sqrt(((r*r) - (yi*yi)));
		LCD_pixel(x+xf,y+yi,1);
		LCD_pixel(x-xf,y+yi,1);
		LCD_pixel(x+xf,y-yi,1);
		LCD_pixel(x-xf,y-yi,1);
		LCD_pixel(x+yi,y+xf,1);
		LCD_pixel(x-yi,y+xf,1);
		LCD_pixel(x+yi,y-xf,1);
		LCD_pixel(x-yi,y-xf,1);
	}
}

#include "display.h"

void	LCD_line(
int	x1,
int	y1,
int	x2,
int	y2,
char	color)
{
	int	dx,dy,ix,iy,inc;
	int	x,y;
	int	plotx,ploty;
	int	cnt;
	uchar	plot;

	color &= 0x01;
	dx = x2 - x1;
	dy = y2 - y1;
	ix = abs(dx);
	iy = abs(dy);
	inc = ((ix > iy)?(ix):(iy));

	plotx = x1;
	ploty = y1;
	x = 0;
	y = 0;

	if((plotx < 240) && (ploty < 64))
		LCD_pixel(plotx,ploty,color);
	for(cnt = 0; cnt <= inc; cnt++)
	{
		x += ix;
		y += iy;
		plot = 0;
		if(x > inc)
		{
			plot = 1;
			x -= inc;
			plotx += ((dx >= 0)?(1):(-1));
		}
		if(y > inc)
		{
			plot = 1;
			y -= inc;
			ploty += ((dy >= 0)?(1):(-1));
		}
		if((plot) && (plotx < 240) && (ploty < 64))
			LCD_pixel(plotx,ploty,color);
	}
}


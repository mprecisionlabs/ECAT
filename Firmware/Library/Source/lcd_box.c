#include "display.h"

void	LCD_box(
int	x1,
int	y1,
int	x2,
int	y2)
{
	LCD_line(x1,y1,x1,y2,1);
	LCD_line(x1,y2,x2,y2,1);
	LCD_line(x2,y2,x2,y1,1);
	LCD_line(x2,y1,x1,y1,1);
}


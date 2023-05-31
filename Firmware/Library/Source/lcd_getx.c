#include "display.h"

extern uchar	_DIS_curx;

int	LCD_getx(void)
{
	return(_DIS_curx);
}

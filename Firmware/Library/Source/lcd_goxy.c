#include "display.h"

extern uchar	_DIS_curx;
extern uchar	_DIS_cury;
extern uchar	_DIS_newpos;

void	LCD_gotoxy(
int	x,
int	y)
{
	_DIS_newpos	= 1;
	_DIS_curx 	= x;
	_DIS_cury	= y;
	LCD_Wcommand(SET_CURSOR_POS,_DIS_cury*256+_DIS_curx);
}

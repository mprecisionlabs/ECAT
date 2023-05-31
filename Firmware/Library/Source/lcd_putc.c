#include "display.h"

extern uchar	_DIS_curx;
extern uchar	_DIS_cury;
extern uchar	_DIS_newpos;
extern uchar	_DIS_pixels[];

void	LCD_putc(char c)
{
	if(_DIS_newpos)
	{
		LCD_Wcommand(SET_ADDRESS,0x1000+_DIS_curx+(_DIS_cury*40));
		_DIS_newpos = 0;
	}
	if(c == 10)
		return;
	if(c == 13)
	{
		while(_DIS_curx < 39) 
		{
			LCD_Bcommand(DATA_WRITE_INC,0);
			_DIS_pixels[0x1000+_DIS_curx+_DIS_cury*40] = 0;
			_DIS_curx++;
		}
		c = ' ';
	}
	LCD_Bcommand(DATA_WRITE_INC,c-0x20);
	_DIS_pixels[0x1000+_DIS_curx+_DIS_cury*40] = c - 0x20;
	_DIS_curx++;
	if(_DIS_curx == 40)
	{
		_DIS_cury++;
		_DIS_curx = 0;
	}
  if(_DIS_cury == 8)
		_DIS_cury = 0;
	LCD_Wcommand(SET_CURSOR_POS,_DIS_cury*256+_DIS_curx);
}


#include <stdio.h>
#include "key.h"
#include "timer.h"
#include "display.h"

extern volatile ushort	display_control;
extern volatile ushort	display_data;

ushort	_DIS_lastmode;
uchar	_DIS_curx;
uchar	_DIS_cury;
uchar	_DIS_newpos;
uchar	_DIS_pixels[4416];

void	LCD_reset(void)
{
	int	i;

	LCD_command(RESET_DISPLAY);
	LCD_command(MODE_TEXT_XOR);
	LCD_Wcommand(SET_TEXT_HOME,0x1000);
	LCD_Wcommand(SET_TEXT_AREA,40);
	LCD_Wcommand(SET_GRAPH_HOME,0x0000);
	LCD_Wcommand(SET_GRAPH_AREA,40);
	_DIS_lastmode = TEXT_ON | CURSOR_ON | CURSOR_BLINK | GRAPH_ON;
	LCD_command(DISPLAY_MODE | _DIS_lastmode);
	LCD_command(CURSOR_PATTERN+0);
	LCD_Wcommand(SET_CURSOR_POS,0x0000);
	LCD_Wcommand(SET_ADDRESS,0x0000);
	LCD_command(AUTO_WRITE);

	for(i = 0; i < 4416; i++)
	{
		while(!(display_data & 8))
			;
		_DIS_pixels[i]  = 0;
		display_control = 0x0;
	}
	LCD_command(AUTO_RESET);
	LCD_Wcommand(SET_CURSOR_POS,0x0000);
	LCD_Wcommand(SET_ADDRESS,0x1000);
	_DIS_curx = 0;
	_DIS_cury = 0;
}

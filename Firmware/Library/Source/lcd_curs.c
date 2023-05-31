#include "display.h"

extern ushort	_DIS_lastmode;

void	LCD_cursor(
uchar	state)
{
	if(state)
		_DIS_lastmode |= CURSOR_ON | CURSOR_BLINK;
	else
		_DIS_lastmode &= ~(CURSOR_ON|CURSOR_BLINK);
	_DIS_lastmode &= 0xff;
	LCD_command(DISPLAY_MODE | _DIS_lastmode);
}

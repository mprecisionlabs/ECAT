#include "display.h"

extern volatile ushort	key_control;
extern uchar	_DIS_newpos;
extern uchar	_DIS_pixels[];

void	LCD_pixel(
int	x,
int	y,
uchar	pix)
{
	int	addr;
	uchar	tvar;

	_DIS_newpos = 1;
	addr = y * 40 + (x / 6);
	if((addr < 0) || (addr > 0x0fff))
		return;

	tvar = 5 - (x % 6);
	LCD_Wcommand(SET_ADDRESS,addr);
	if(pix)
	{
		LCD_command(BIT_SET + tvar);
		_DIS_pixels[addr] |= (1 << tvar);
	}
	else
	{
		LCD_command(BIT_CLR + tvar);
		_DIS_pixels[addr] &= ~(1 << tvar);
	}
}

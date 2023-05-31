#include "display.h"

extern volatile ushort	display_control;
extern volatile ushort	display_data;
extern uchar	_DIS_pixels[];

void	LCD_bitmap(
int	x,
int	y,
uchar	*pi)
{
	int	addr;
	int	yy;
	int	i;
	int	cnt;
	int	value;
	int	next;
	uchar	*p;

	p  = pi;
	yy = y;
	do
	{
		addr = yy * 40 + (x / 6);
		LCD_Wcommand(SET_ADDRESS,addr);
		LCD_command(AUTO_WRITE);
		cnt = *p;
		p++;
		while(cnt)
		{
			value = *p;
			p++;
			for(i = 0; i < cnt; i++)
			{
				while(!(display_data & 8))
					;
				display_control = (value ^ 0xff);
            _DIS_pixels[addr++]=(value ^ 0xff);
			}
			cnt = *p;
			p++;
		}
		next = *p;
		LCD_command(AUTO_RESET);
		yy++;
	}
	while(next);
}

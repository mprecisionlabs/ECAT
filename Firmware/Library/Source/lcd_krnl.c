#include "display.h"

extern volatile ushort	display_control;
extern volatile ushort	display_data;

void	LCD_rawdata(
uchar	d)
{
	while(!(display_data & 1))
		;
	display_control = d;
}

void	LCD_command(
uchar	c)
{
	while(!(display_data & 1))
		;
	display_data = c;
}

void	LCD_Bcommand(
uchar	c,
uchar	d)
{
	LCD_rawdata(d);
	LCD_command(c);
}

void	LCD_Wcommand(
uchar	c,
ushort	d)
{
	LCD_rawdata((d & 0xff));
	LCD_rawdata((d / 256));
	LCD_command(c);
}

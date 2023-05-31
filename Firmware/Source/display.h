#ifndef __DISPLAY_H			/* Avoid multiple inclusions */
#define __DISPLAY_H

#include <shortdef.h>

#define	RESET_DISPLAY		0x80
#define	MODE_TEXT_OR		0x80
#define	MODE_TEXT_XOR		0x81
#define	MODE_TEXT_AND		0x83
#define	MODE_TEXT_ATTR		0x84
#define	SET_TEXT_HOME		0x40
#define	SET_TEXT_AREA		0x41
#define	SET_GRAPH_HOME		0x42
#define	SET_GRAPH_AREA		0x43
#define	DISPLAY_MODE		0x90
#define	TEXT_ON				0x04
#define	CURSOR_ON			0x02
#define	CURSOR_BLINK		0x01
#define	GRAPH_ON				0x08
#define	CURSOR_PATTERN		0xA0
#define	SET_CURSOR_POS		0x21
#define	SET_ADDRESS			0x24
#define	SET_OFFSET			0x22
#define	DATA_WRITE_INC		0xC0
#define	DATA_WRITE_DEC		0xC2
#define	DATA_WRITE			0xC6
#define	AUTO_WRITE			0xB0
#define	AUTO_RESET			0xB2
#define	BIT_SET				0xF8
#define	BIT_CLR				0xF0
#define	DISPLAY				(&_iob[9])

/*
 *	PRIVATE function definitions
 */
void	LCD_command(uchar);
void	LCD_Bcommand(uchar ,uchar);
void	LCD_Wcommand(uchar ,ushort);

/*
 *	PUBLIC function definitions
 *		Note:	Any routines using coordinates have a designator as to
 *				which type of coordinate system it uses.
 *			(graphic)	- X range is 0 - 239
 *							  Y range is 0 - 63
 *			(character)	- X range is 0 - 39
 *							  Y range is 0 - 7
 *
 *		LCD_reset	-- resets the display system and clears the screen
 *		LCD_cursor	-- Turn cursor ON or OFF
 *		LCD_gotoxy	-- Move cursor to specified X/Y location		(character)
 *		LCD_getx		-- Get the current X cursor coordinate			(character)
 *		LCD_gety		-- Get the current Y cursor coordinate			(character)
 *		LCD_putc		-- Write a character to the display
 *		LCD_puts		-- Write a character string to the display
 *		LCD_hilight	-- Invert a block of the screen					(graphic)
 *		LCD_normal	-- Normalize a block of the screen				(graphic)
 *		LCD_pixel	-- Set/Clear a pixel on the display				(graphic)
 *		LCD_line		-- Draw a line on the display						(graphic)
 *		LCD_box		-- Draw a frame box on the display				(graphic)
 *		LCD_circle	-- Draw a circle on the display					(graphic)
 *		LCD_bitmap	-- Blit a bitmap to the display					(graphic)
 */
void	LCD_reset(void);						/* LCD display functions	*/
void	LCD_cursor(uchar);
void	LCD_gotoxy(int,int);
int	LCD_getx(void);
int	LCD_gety(void);
void	KEY_beep(void);
void	LCD_putc(char);						/* LCD character functions	*/
void	LCD_puts(char *);
void	LCD_hilight(int,int,int,int);
void	LCD_normal(int,int,int,int);
void	LCD_pixel(int,int,uchar);			/* LCD graphic functions	*/
void	LCD_line(int,int,int,int,char);
void	LCD_box(int,int,int,int);
void	LCD_circle(int,int,int);
void	LCD_bitmap(int,int,unsigned char *);

#endif							/* ifndef __NEWDIS_H */


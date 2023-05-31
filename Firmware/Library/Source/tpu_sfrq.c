#include "timer.h"

extern volatile ushort	TPU_rams[16][8];
extern volatile uchar	happened[];

/*
 *	 This function sets up the asociated channel to measure frequency
 *	 This function takes two parameters : ch: channel number
 *	 and count, count is the number of cycles of the input transitions to
 *	 use for the calulation of frequency
 *	 The accuracy of the measurment is (neglecting crystal inaccuracies)
 *
 *	 for frequency F the reading can range from	1/((1/f)-477ns)
 *														to		1/((1/f)+477ns)
 *	 the value of count also sets limits the frequency measurment  range
 *	 The minimum measureable frequency = (0.1250/count)
 *	 To read the frequency measured see the function: TPU_read_freq(ch);
 */

void	TPU_start_freq(
char	ch,
char	count)
{
	mode[ch] = FREQ;
	happened[ch] = 0;
	TPU_rams[ch][0] = 0x000B;					/* Channel control 0 0000 1011 */
	TPU_rams[ch][1] = count * 256;
	TPU_rams[ch][4] = 0xff00;

	TPU_channel_init(ch,0x0F,0,0,3,0);
	TPU_rams[ch][4] = 0xff00;
	TPU_channel_init(ch,0x0F,2,0,3,1);
}

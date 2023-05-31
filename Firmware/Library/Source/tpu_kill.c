#include "timer.h"

extern volatile uchar	tpuFlag[];

void	TPU_kill(
uchar	ch)
{
	tpuFlag[(char)(ch)] = 0;
	mode[ch] = NONE;
	TPU_channel_init(ch,0x00,0,0,0,0);
	tpuFlag[(char)(ch)] = 1;
}

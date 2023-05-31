#include "key.h"

void	KEY_beepkill(void)
{
	DAC_set(AUDIO_DAC,0);
}

void	KEY_beep(void)
{
	DAC_set(AUDIO_DAC,255);
	TPU_delay_int(BEEP_DELAY,KEY_beepkill);
}

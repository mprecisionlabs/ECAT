#ifndef __EHV_H					/* Avoid multiple inclusions */
#define __EHV_H

#include "shortdef.h"

void	EHV_FindModules(void);
void	EHV_ResetModules(uchar);
char	EHV_HVProgram(int,uchar);

#endif								/* ifndef EHV_H */

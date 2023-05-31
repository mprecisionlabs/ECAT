#ifndef __DATA_H					/* Avoid multiple inclusions */
#define __DATA_H

#include <shortdef.h>

#define CLAMP_INT20V		(0x00)
#define CLAMP_INT220V		(0x01)
#define CLAMP_EXT			(0x02)

#define CMODE_ASYMMETRIC	(0x00)
#define CMODE_SYMMETRIC	(0x01)

#define CPL_TYPELINE		(0x00)
#define CPL_TYPEHV			(0x01)
#define CPL_TYPEDATA		(0x02)

/* Definitions for RELAY data */
#define RLY_INT20V			(0x00100000)
#define RLY_DATAOUT			(0x20000000)
#define CPL_DATA_S1LO0		(0x10000000)
#define CPL_DATA_S2LO0		(0x08000000)
#define CPL_DATA_S3LO0		(0x04000000)
#define CPL_DATA_S4LO0		(0x00200000)
#define CPL_DATA_PELO0		(0x00040000)
#define CPL_DATA_MASK0		(0x1C240000)

/* Definitions for EXPANSION data (Note these are translated to 0-31) */
#define RLY_DATA_ENABLE	(0x00000001)
#define RLY_DATA_E502A		(0x00000002)
#define CPL_DATA_S1HI0		(0x00000100)
#define CPL_DATA_S2HI0		(0x00000400)
#define CPL_DATA_S3HI0		(0x00004000)
#define CPL_DATA_S4HI0		(0x00010000)
#define CPL_DATA_S1HI1		(0x00000004)		/* For E508A */
#define CPL_DATA_S2HI1		(0x00000010)		/* For E508A */

#endif								/* ifndef DATA_H */

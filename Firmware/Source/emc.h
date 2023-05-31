#ifndef __EMC_H					/* Avoid multiple inclusions */
#define __EMC_H

#include <shortdef.h>

#define	EMC_SRGEFT				(0x00)
#define	EMC_FLICKER				(0x01)
#define	EMC_HARMONICS			(0x02)

#define	EMC_DIN4WAIT			(4)

typedef struct tag_emcdata
{
 	char	mode;											/* Mode for EMC process			*/
	char	modeFlag;									/* EMC mode has been set		*/
	char	EP7xAddr;									/* Location of EP7x Module		*/
	char	EP6xAddr;									/* Location of EP6x Module		*/
	char	ERIxAddr;									/* Location of ERIx Module		*/
	char	EOPxAddr;									/* Location of EOPx Module		*/
	char	EP7xFlag;									/* EP7x module present			*/
	char	EP6xFlag;									/* EP6x module present			*/
	char	ERIxFlag;									/* ERIx module present			*/
	char	EOPxFlag;									/* EOPx module present			*/
	uint	EP7xRelay;									/* EP7x relay states				*/
	uint	EP6xRelay;									/* EP6x relay states				*/
	uint	ERIxRelay;									/* ERIx relay states				*/
	uint	EOPxRelay;									/* EOPx relay states				*/
	uchar	EP7xPhase;									/* Single or three phase?		*/
	uchar	EP7xBypass;									/* EP73 Bypass mode?				*/
	uchar	EOPxSoftStart;								/* Flag for EOP SoftStart		*/
	uchar	EOPxSoftStartCh;							/* TPU chan for EOP SoftStart	*/
} EMCDATA;

/*
 * Function Prototypes
 */
int	EMC_BYpass_pgm(P_PARAMT *);
int	EMC_BYpass_rpt(P_PARAMT *);
int	EMC_EUT_pgm(P_PARAMT *);
int	EMC_MOde_pgm(P_PARAMT *);
int	EMC_PHase_pgm(P_PARAMT *);
int	EMC_PHase_rpt(P_PARAMT *);
int	EMC_REset_pgm(P_PARAMT *);
int	EMC_EXit_pgm(P_PARAMT *);
char	EMC_EP6xAuxInitialize(char);
char	EMC_EP6x100Initialize(char);
char	EMC_EP7xInitialize(char);
char	EMC_EOPxInitialize(void);
void	EMC_FindModules(void);
void	EMC_ResetModules(uchar);

#endif								/* ifndef EMC_H */

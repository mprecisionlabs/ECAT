#include "qsm.h"
#include "vectors.h"

extern volatile ulong	vectors[];
extern volatile ushort	QMCR;
extern volatile ushort	QILR_QIVR;
extern volatile ushort	QPDR;
extern volatile ushort	QPAR_QDDR;
extern volatile uchar	QSM_command[];
extern volatile ushort	QSM_tx[];
extern volatile ushort	SPCR0;
extern volatile ushort	SPCR1;
extern volatile ushort	SPCR2;
extern volatile ushort	SPCR3;

int	AD_MAX[8];						/* The maximum found */
int	AD_MIN[8];						/* The minimum found */
int	AD_RMS[8];						/* The MS of RMS		*/
int	AD_AVG[8];						/* The Average sum	*/
int	AD_WORKING_RMS[8];			/* The MS accumulators of RMS			*/
int	AD_WORKING_AVG[8];			/* The Average accumulator of sum	*/
int	AD_COUNTS[8];					/* The number of samples left in set*/
int	AD_SET_COUNTS[8];				/* The counts between AVG and RMS	*/
volatile uchar	AD_HIT;

/*
 *    The following configuration words are directly out of the LTC
 *    handbook for the LTC1290:
 *
 *                    Select0 ÄÄÄÄÄ¿ ÚÄÄÄÄÄ Unipolar/Bipolar
 *                    Select1 ÄÄÄÄ¿³ ³ÚÄÄÄÄ MSB first/LSB first
 *                Odd or Sign ÄÄÄ¿³³ ³³ÚÄÄÄ Word length 1
 *        Single/Differential ÄÄ¿³³³ ³³³ÚÄÄ Word length 0
 *                              ³³³³ ³³³³
 *                              ³³³³ ³³³³
 */
#undef	DO_OLD_CODE
#ifdef	DO_OLD_CODE
const AD0_CFG = 0x8E;        /* 1000 1110 */
const AD1_CFG = 0xCE;        /* 1100 1110 */
const AD2_CFG = 0x9E;        /* 1001 1110 */
const AD3_CFG = 0xDE;        /* 1101 1110 */
const AD4_CFG = 0xAE;        /* 1010 1110 */
const AD5_CFG = 0xEE;        /* 1110 1110 */
const AD6_CFG = 0xBE;        /* 1011 1110 */
const AD7_CFG = 0xFE;        /* 1111 1110 */
#else
const AD0_CFG = 0x86;        /* 1000 0110 */
const AD1_CFG = 0xC6;        /* 1100 0110 */
const AD2_CFG = 0x96;        /* 1001 0110 */
const AD3_CFG = 0xD6;        /* 1101 0110 */
const AD4_CFG = 0xA6;        /* 1010 0110 */
const AD5_CFG = 0xE6;        /* 1110 0110 */
const AD6_CFG = 0xB6;        /* 1011 0110 */
const AD7_CFG = 0xF6;        /* 1111 0110 */
#endif

/*
 *  The QSM is configured as follows:
 *    12 bit words
 *  then normal configuration of the outputs are :
 *
 *  PCS0:High
 *  PCS1:High
 *  PCS3:High
 *  The data rate maximums are:
 *    DAC-8043  4Mhz
 *    DAC-8800  8Mhz
 *    LTC1290   2Mhz
 *  So the selected data rate is 1 Mhz. (safety margin )
 *
 *  The wait between channels (DTL page 5-26) is set to 14usec  to allow the A/D to
 *  convert.
 *                                    Config  pg 5-34
 *                                 DSCK ÄÄÄÄÄ¿ÚÄÄÄÄÄ PCS3
 *                                   DT ÄÄÄÄ¿³³ÚÄÄÄÄ PCS2
 *                                BITSE ÄÄÄ¿³³³³ÚÄÄÄ PCS1
 *                                 CONT ÄÄ¿³³³³³³ÚÄÄ PCS0
 *                                        ³³³³³³³³
 *  Ch#   Name        Dout Value  Din     ³³³³³³³³
 *  CH0   A/D 0       AD0_CFG     A/D 7   11101110 = 0xEE
 *  CH1   A/D 1       AD1_CFG     A/D 0   11101110 = 0xEE
 *  CH2   A/D 2       AD2_CFG     A/D 1   11101110 = 0xEE
 *  CH3   A/D 3       AD3_CFG     A/D 2   11101110 = 0xEE
 *  CH4   A/D 4       AD4_CFG     A/D 3   11101110 = 0xEE
 *  CH5   A/D 5       AD5_CFG     A/D 4   11101110 = 0xEE
 *  CH6   A/D 6       AD6_CFG     A/D 5   11101110 = 0xEE
 *  CH7   A/D 7       AD7_CFG     A/D 6   11101110 = 0xEE
 *  CH8   A/D 7       AD7_CFG     A/D 7   11101110 = 0xEE
 *  CH9   12b DAC     Data        NA      01101111 = 0X6F
 *  CH10   8b DAC     Addr+Data   NA      11001111 = 0xCF
 *  CH11   8b Load    NA          NA      11001101 = 0XCD
 *  CH12   8b Finish  NA          NA      11001111 = 0xCF
 *
 *  The 12DAC and the 8 A/D's can run continuously , but the 8 bit Dac
 *  requires opposite clock polarity and thus requires a special cycle.
 *
 *  The system is usually setup to run locations 0 to 8 with
 *  positive clock polarity.
 *
 */


/*
 *  QSPI CONTROL REGISTER 0
 *     (SPCR0)  (p5-22)
 *
 *  CPHA ÄÄÄÄÄÄÄÄ¿
 *  CPOL ÄÄÄÄÄÄÄ¿³
 *  bits ÄÄÄÂÂÂ¿³³                 QSPI CONTROL REGISTER 1
 *          ³³³³³³                    (SPCR1)  (p5-25)
 *          ³³³³³³
 *          ³³³³³³                 DSCLK ÄÄÂÂÂÄÂÂÂ¿
 *  WOMQ ÄÄ¿³³³³³³                   SPE Ä¿³³³ ³³³³
 *  MSTR Ä¿³³³³³³³ Baud=1.05Mhz           ³³³³ ³³³³ DTL = 8 = 14usec
 *        10110000 00001000               1000 0000 00001000
 *
 *
 *  QSPI CONTROL REGISTER 2       QSPI CONTROL REGISTER 3
 *     (SPCR2)  (p5-27)              (SPCR3)  (p5-29)
 *                   ÚÄÄÄÄÄÄÄÄ 0
 *   ENDQP ÄÄÄÄÄÂÂÂ¿ ³ÚÄÄÄÄÄÄÄ 0
 *              ³³³³ ³³ÚÄÄÄÄÄÄ 0       0 ÄÄÄÄ¿ ÚÄÄÄÄ 0
 *              ³³³³ ³³³ÚÄÄÄÄÄ 0       0 ÄÄÄ¿³ ³ÚÄÄÄ LOOPQ
 *       0 ÄÄÄÄ¿³³³³ ³³³³              0 ÄÄ¿³³ ³³ÚÄÄ HIME
 *    WRTO ÄÄÄ¿³³³³³ ³³³³ÚÂÂÂÄ NEWQP   0 Ä¿³³³ ³³³ÚÄ HALT
 *    WREN ÄÄ¿³³³³³³ ³³³³³³³³             ³³³³ ³³³³
 *  SPIFIE Ä¿³³³³³³³ ³³³³³³³³             ³³³³ ³³³³
 *          11001000 00000000             0000 0000 xxxx xxxx
 */

/*
 *	QSM_init -- Initializes the Queued serial I/O system.(A/D,D/A)
 *
 *      Syntax: void QSM_init();
 * Description: This initializes the QSM (see SIM manual chapter 5)
 *              for automatic service of the A/D, and D/A's.
 *  Exceptions: This function must be called before using any of the
 *              A/D or D/A functions.
 *    See Also: A/D and D/A sections.
 * Source Code: QSM.C
 *     Include: QSM.H
 *      Status: Not complete.
 */

void	QSM_init(void)
{
int i;

    QMCR = QSM_ARB; /*QSM configuration register  pg 5-11*/
    i=QILR_QIVR;
    i&=0x0700;
    i|= (QSM_IRQ_LEVEL*256*8)+QSM_VECTOR_BASE; 
               /*QSM interrupt level Register pg 5-13,14*/
    QILR_QIVR=i;  
   /*IRQ vector */
   vectors[QSM_VECTOR_BASE|1]   =(long)QSM_trap; 
   vectors[(QSM_VECTOR_BASE|1)+256]   =(long)QSM_trap; 


	QPDR = 0x0078;						/* QSM port data register (p5-15)	*/
	QPAR_QDDR = 0x7BFE;				/* QSM data direction register		*/

/*
 *  Data Register QPDR
 *     PCS1 ÄÄÄÄÄ¿ ÚÄÄÄÄÄ PCS0
 *     PCS2 ÄÄÄÄ¿³ ³ÚÄÄÄÄ SCK
 *     PCS3 ÄÄÄ¿³³ ³³ÚÄÄÄ MOSI
 *       TX ÄÄ¿³³³ ³³³ÚÄÄ MISO
 *            ³³³³ ³³³³
 *   Unused   ³³³³ ³³³³
 *  0000 0000 0111 1000
 *
 *
 *  QSM Pin assignment register QPAR and
 *  QSM data direction register QDDR 5-15,16
 *
 *            QPAR                         QDDR
 *  PCS1 ÄÄÄÄÄ¿ ÚÄÄÄÄÄ PCS0      PCS1 ÄÄÄÄÄ¿ ÚÄÄÄÄÄ PCS0
 *  PCS2 ÄÄÄÄ¿³ ³ÚÄÄÄÄ 0         PCS2 ÄÄÄÄ¿³ ³ÚÄÄÄÄ SCK
 *  PCS3 ÄÄÄ¿³³ ³³ÚÄÄÄ MOSI      PCS3 ÄÄÄ¿³³ ³³ÚÄÄÄ MOSI
 *     0 ÄÄ¿³³³ ³³³ÚÄÄ MISO       TXD ÄÄ¿³³³ ³³³ÚÄÄ MISO
 *         ³³³³ ³³³³                    ³³³³ ³³³³
 *         ³³³³ ³³³³                    ³³³³ ³³³³
 *         0111 1011                    1111 1110
 *
 *
 *                      DSCKÄÄÄÄÄ¿ÚÄÄÄÄÄ PCS3  (p5-34)
 *                        DTÄÄÄÄ¿³³ÚÄÄÄÄ PCS2
 *                     BITSEÄÄÄ¿³³³³ÚÄÄÄ PCS1
 *                      CONTÄÄ¿³³³³³³ÚÄÄ PCS0
 *                            ³³³³³³³³
 *                            ³³³³³³³³
 */
	QSM_command[0]  = 0xEE; /* 11101110 */
	QSM_command[1]  = 0xEE; /* 11101110 */
	QSM_command[2]  = 0xEE; /* 11101110 */
	QSM_command[3]  = 0xEE; /* 11101110 */
	QSM_command[4]  = 0xEE; /* 11101110 */
	QSM_command[5]  = 0xEE; /* 11101110 */
	QSM_command[6]  = 0xEE; /* 11101110 */
	QSM_command[7]  = 0xEE; /* 11101110 */
	QSM_command[8]  = 0xEE; /* 11101110 */
	QSM_command[9]  = 0x6F; /* 01101111 */
	QSM_command[10] = 0xCF; /* 11001111 */
	QSM_command[11] = 0xCD; /* 11001101 */
	QSM_command[12] = 0xCF; /* 11001111 */

	QSM_tx[0]  = AD0_CFG * 16;
	QSM_tx[1]  = AD1_CFG * 16;
	QSM_tx[2]  = AD2_CFG * 16;
	QSM_tx[3]  = AD3_CFG * 16;
	QSM_tx[4]  = AD4_CFG * 16;
	QSM_tx[5]  = AD5_CFG * 16;
	QSM_tx[6]  = AD6_CFG * 16;
	QSM_tx[7]  = AD7_CFG * 16;
	QSM_tx[8]  = AD7_CFG * 16;
	QSM_tx[9]  = 0x0000;
	QSM_tx[10] = 0x0000;
	QSM_tx[11] = 0x0000;
	QSM_tx[12] = 0x0000;

	SPCR0 = 0xB008;					/* QSPI control register 0 */
	SPCR1 = 0x8008;					/* QSPI control register 1 */
	SPCR2 = 0xC800;					/* QSPI control register 2 */
	SPCR3 = 0x0000;					/* QSPI control register 3 */

	for(i = 0; i < 8; i++)
	{
		DAC_set(i,0);
		AD_SET_COUNTS[i] = 0;
		AD_COUNTS[i] = 0;
	}
}

/******************************************************************************

	MODULE.H --	Header file with ECAT module type and field codes

	History:		Start with 4.0A version.

		5/17/95		E521, E521C TYPES added.					JFL
		5/23/95		E509 added.										JFL
		7/12/95		EP91/2/34 added.								JFL
		8/30/95		CE40/50 added.									JFL
		9/12/95		E4556 added.									JFL
		4/19/96		KV versions of E45xx couplers				JFL
		4/29/96		Add KV TYPE attribute						JFL

******************************************************************************/

#define	 TYPE_SRG		0x0001			/* to allow for hybrid modules		*/
#define	 TYPE_EFT		0x0002
#define	 TYPE_AMP		0x0004
#define  TYPE_PQF       0x0008
#define	 TYPE_CPLSRG	0x0010
#define	 TYPE_CPLEFT	0x0020
#define	 TYPE_CPLTRI	0x0040
#define  TYPE_POWER     0x0080
#define	 TYPE_CPLIO     0x0100
#define	 TYPE_ESD       0x0200
#define	 TYPE_IMPED     0x0400
#define	 TYPE_CPLAC     0x0800
#define	 TYPE_CPLHV     0x1000
#define	 TYPE_CLAMP     0x2000
#define	 TYPE_CE		0X4000			// CE box identifier
#define	 TYPE_CPL10KV	0X8000			// KV coupler versions for 10KV redesign

// SURGE module identifications	
#define	 E501_BOXT			0			// 0x00	
#define	 CE50_BOXT			0			// CE50 like E501
#define	 E502_BOXT			1			// 0x01
#define	 E503_BOXT			2			// 0x02
#define	 E511_BOXT			3			// 0x03
#define	 E506_BOXT			4			// 0x04
#define	 E505_BOXT			5			// 0x05
#define  E510_BOXT          6			// 0x06
#define  E502H_BOXT         7			// 0x07
#define  E504_BOXT          8			// 0x08
#define  E513_BOXT          9			// 0x09
#define  E514_BOXT          10			// 0x0A
#define  E522_BOXT          11			// 0x0B
#define  E502K_BOXT			12			// 0x0C
#define  E501A_BOXT			13			// 0x0D
#define  E510A_BOXT			14			// 0x0E
#define  E508_BOXT			15			// 0x0F
#define  E502A_BOXT			16			// 0x10
#define  E504A_BOXT			17			// 0x11
#define	 E509_BOXT			18			// 0x12
#define	 E521_BOXT			19			// 0x13
#define  E502B_BOXT			20			// 0x14
#define  E509A_BOXT         21			// 0x15
#define	 E501B_BOXT		    22			// 0x16
#define	 E504B_BOXT		    23			// 0x17
#define  E506_4W_BOXT       24			// 0x18
#define  E515_BOXT          25			// 0x19
#define  E505A_BOXT         26			// 0x1A
#define  E518_BOXT          27			// 0x1B
#define  E505B_BOXT         28			// 0x1C
#define  E502C_BOXT			29			// 0x1D
#define	 E503A_BOXT			30			// 0x1E
#define  E507_BOXT          31          // 0x1F

/// **** to come?  #define  E502AK_BOXT			21

// COUPLER module identification
#define	 E551_BOXT			0				
#define	 E552_BOXT			1
#define	 E553_BOXT			2
#define	 E554_BOXT			3
#define	 E4551_BOXT			4
#define	 E4552_BOXT			5
#define	 E4553_BOXT			6
#define	 E4554_BOXT			7
#define	 E571_BOXT			8
#define   E50812P_BOXT		9
#define	 E521C_BOXT			10
#define	 E522C_BOXT			11
#define	 E4556_BOXT			12
#define	 E4551KV_BOXT		13				// 'KV' versions for 10KV redesign
#define	 E4552KV_BOXT       14
#define	 E4553KV_BOXT		15
#define	 E4554KV_BOXT		16
#define	 E4551A_BOXT		17				// 'A' boxes are identical to 'KV' boxes
#define	 E4552A_BOXT        18
#define	 E4553A_BOXT		19
#define	 E4554A_BOXT		20

// EFT module identification
#define	 E411_BOXT			0				
#define	 CE40_BOXT			0				// CE40 id'd like E411
#define	 E412_BOXT			1
#define	 E421_BOXT			2
#define	 E422_BOXT			3
#define	 E423_BOXT			4
#define	 E424_BOXT			5
#define	 E433_BOXT			6
#define	 E434_BOXT			7
#define	 E421P_BOXT			8

// PQF module identification
#define	 EP61_BOXT			0				
#define	 EP62_BOXT			1
#define	 EP3_BOXT			2

// EHV Id's 
#define	 EHV2_BOXT			0				
#define	 EHV10_BOXT			1
#define	 EHV2Q_BOXT			2				

#define  EP71_BOXT			0			   /* amp 										*/
#define  ERI1_BOXT			1				/* reference impendance					*/
#define  EP72_BOXT			2
#define  ERI3_BOXT			3
#define  EP73_BOXT			4
#define  EP91_BOXT			5           // amplifiers:  PPS
#define  EP92_BOXT			6
#define  EP93_BOXT			7
#define  EP94_BOXT			8

#define  EOP1_BOXT			0				/* Clamp                            */
#define  EOP3_BOXT			1

// Options
#define INSTR_5CH_BIT     0x00000001l
#define EHV_BIT           0x00000002l
#define EFT_8K_BIT        0x00000400l
#define EFT_2M_BIT        0x00000800l
#define E502_SW_BIT       0x00001000l
#define AC_OPT_BIT        0x00008000l	// Is 'EP Option' (diff.) in CE40/50
#define EFT_CHIRP_BIT     0x00002000l
#define DC_SURGE_BIT      0x00004000l


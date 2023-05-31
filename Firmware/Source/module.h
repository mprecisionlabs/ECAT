#ifndef __MODULE_H				/* Avoid multiple inclusions */
#define __MODULE_H

#include  <shortdef.h>

#define	MODNAMESIZE		(8)				/* Maximum module NAME length			*/
#define	CALINFOSIZE		(49)				/* Size of CALIBRATION data array	*/
#define	MAX_WAVEFORMS	(5)				/* Maximum SURGE waveforms				*/

#define	MAX_SRGBOXES	(32)    		/* Number of current SURGE modules	*/
#define	MAX_EHVBOXES	(3) 			/* Number of current POWER modules	*/
#define	MAX_CPLBOXES	(17)			/* Number of current COUPLER modules*/
#define	MAX_EFTBOXES	(9)				/* Number of current EFT modules		*/
#define	MAX_PQFBOXES	(3)				/* Number of current PQF modules		*/
#define	MAX_EMCBOXES	(9)				/* Number of current AMPLIFIERS		*/
#define	MAX_CLAMPBOXES	(2)				/* Number of current CLAMP modules	*/

/*
 *	The following defines ALL the module types used in the ECAT system
 */
#define	TYPE_GHOST		(0x00000000)	/* E000 Module			*/
#define	TYPE_SRG		(0x00000001)	/* SURGE Module		*/
#define	TYPE_EFT		(0x00000002)	/* EFT Module			*/
#define	TYPE_EMC		(0x00000004)	/* EMC Modules			*/
#define	TYPE_PQF		(0x00000008)	/* PQF Module			*/
#define	TYPE_CPLSRG		(0x00000010)	/* SURGE Coupler		*/
#define	TYPE_CPLEFT		(0x00000020)	/* EFT Coupler			*/
#define	TYPE_3PHASE		(0x00000040)	/* 3-Phase Module		*/
#define	TYPE_EHV		(0x00000080)	/* POWER Module		*/
#define	TYPE_CPLDATA	(0x00000100)	/* DATA Coupler		*/
#define	TYPE_ESD		(0x00000200)	/* ESD Module			*/
#define	TYPE_IMP		(0x00000400)	/* IMPEDANCE Module	*/
#define	TYPE_CPLLINE	(0x00000800)	/* AC LINE Coupler	*/
#define	TYPE_CPLHV		(0x00001000)	/* HV Coupler			*/
#define	TYPE_CPL10KV	(0x00008000)	/* 10KV Coupler		*/
#define	TYPE_CLAMP		(0x00002000)	/* Clamp Modules		*/
#define	TYPE_CPLGEN		(0x00001930)	/* ALL Couplers		*/

/*
 *	The following defines ALL the SURGE module IDs
 */
#define	GHOST_BOX		(0xFF)			/* GENERIC "Nothing" Module	*/
#define	E501_BOX		(0x00)
#define	E502_BOX		(0x01)
#define	E503_BOX		(0x02)
#define	E511_BOX		(0x03)
#define	E506_BOX		(0x04)
#define	E505_BOX		(0x05)
#define	E510_BOX		(0x06)
#define	E502H_BOX		(0x07)
#define	E504_BOX		(0x08)
#define	E513_BOX		(0x09)
#define	E514_BOX		(0x0A)
#define	E522S_BOX		(0x0B)
#define	E502K_BOX		(0x0C)
#define	E501A_BOX		(0x0D)
#define	E510A_BOX		(0x0E)
#define	E508_BOX		(0x0F)
#define	E502A_BOX		(0x10)
#define	E504A_BOX		(0x11)
#define	E509_BOX		(0x12)
#define	E521S_BOX		(0x13)
#define	E502B_BOX		(0x14)
#define E509A_BOX       (0x15)
#define	E501B_BOX		(0x16)
#define	E504B_BOX		(0x17)
#define E506_4W_BOX     (0x18)
#define E515_BOX        (0x19)
#define E505A_BOX       (0x1A)
#define E518_BOX        (0x1B)
#define E505B_BOX       (0x1C)
#define E502C_BOX       (0x1D)
#define	E503A_BOX		(0x1E)
#define E507_BOX        (0x1F)


/*
 *	The following defines ALL the POWER module IDs
 */
#define	EHV2_BOX		(0x00)
#define	EHV10_BOX		(0x01)
#define	EHV2Q_BOX		(0x02)

/*
 *	The following defines ALL the AC COUPLER module IDs
 */
#define	E551_BOX		(0x00)
#define	E552_BOX		(0x01)
#define	E553_BOX		(0x02)
#define	E554_BOX		(0x03)
#define	E4551_BOX		(0x04)
#define	E4552_BOX		(0x05)
#define	E4553_BOX		(0x06)
#define	E4554_BOX		(0x07)
#define	E571_BOX		(0x08)
#define	E508P_BOX		(0x09)
#define	E521C_BOX		(0x0A)
#define	E522C_BOX		(0x0B)
#define	E4556_BOX		(0x0C)
#define	E4551KV_BOX		(0x0D)
#define	E4552KV_BOX		(0x0E)
#define	E4553KV_BOX		(0x0F)
#define	E4554KV_BOX		(0x10)
#define	E4551A_BOX		(0x11)
#define	E4552A_BOX		(0x12)
#define	E4553A_BOX		(0x13)
#define	E4554A_BOX		(0x14)

/*
 *	The following defines ALL the EFT module IDs
 */
#define	E411_BOX		(0x00)
#define	E412_BOX		(0x01)
#define	E421_BOX		(0x02)
#define	E422_BOX		(0x03)
#define	E423_BOX		(0x04)
#define	E424_BOX		(0x05)
#define	E433_BOX		(0x06)
#define	E434_BOX		(0x07)
#define	E421P_BOX		(0x08)

/*
 *	The following defines ALL the PQF module IDs
 */
#define	EP61_BOX		(0x00)
#define	EP62_BOX		(0x01)
#define	EP3_BOX			(0x02)

/*
 *	The following defines ALL the AMPLIFIER module IDs
 */
#define	EP71_BOX		(0x00)
#define	ERI1_BOX		(0x01)
#define	EP72_BOX		(0x02)
#define	ERI3_BOX		(0x03)
#define	EP73_BOX		(0x04)
#define	EP91_BOX		(0x05)
#define	EP92_BOX		(0x06)
#define	EP93_BOX		(0x07)
#define	EP94_BOX		(0x08)

/*
 *	The following defines ALL the CLAMP module IDs
 */
#define	EOP1_BOX		(0x00)			/* Single-phase EMC clamp module		*/
#define	EOP3_BOX		(0x01)			/* Three-phase EMC clamp module		*/

/*
 *	The following defines ALL the SURGE waveform IDs
 */
#define	E501_02OHM		(0x00)			/* E501 Waveform selections			*/
#define	E501_12OHM		(0x01)
#define	E501_0212OHM	(0x02)
#define	E502_05700		(0x00)			/* E502 Waveform selections			*/
#define	E502_10700		(0x01)
#define	E502_100700		(0x02)
#define	E502K_1250US	(0x00)			/* Special 1.2/50 us waveform			*/
#define	E503_200A		(0x00)			/* E503 Waveform selections			*/
#define	E503_500A		(0x01)
#define	E504_500A		(0x00)			/* E504 Waveform selections			*/
#define	E504_750A		(0x01)
#define	E504_125A		(0x02)
#define	E505_1500V		(0x00)			/* E505 Waveform selections			*/
#define	E505_800V100A	(0x01)
#define	E505_800V200A	(0x02)
#define	E505_2500V		(0x03)
#define	E505A_1500V	    (0x00)			/* E505A Waveform selections	*/
#define	E505A_800V100A	(0x01)
#define	E505A_2500V	    (0x02)
#define	E505B_2500V	    (0x00)			/* E505B Waveform selections	*/
#define	E505B_1500V	    (0x01)
#define	E505B_800V100A	(0x02)
#define	E506_800V		(0x00)			/* E506 Waveform selections			*/
#define	E506_1500V		(0x01)
#define	E506_2500V		(0x02)
#define	E506_5000V		(0x03)
#define	E508_100A		(0x00)			/* E508 Waveform selections			*/
#define	E508_25A		(0x01)			/* E508-12P ONLY							*/
#define	E509_600V		(0x00)			/* E509 Waveform selections			*/
#define	E509_1000V		(0x01)
#define	E509_1500V		(0x02)
#define	E510_02OHM		(0x00)			/* E510 Waveform selections			*/
#define	E510_12OHM		(0x01)
#define	E511_1250US		(0x00)			/* E511 Waveform selections			*/
#define	E513_P1KVUS		(0x00)			/* E513 Waveform selections			*/
#define	E513_P5KVUS		(0x01)
#define	E513_1KVUS		(0x02)
#define	E513_5KVUS		(0x03)
#define	E513_10KVUS		(0x04)
#define	E514_015A    	(0x00)			/* E514 Waveform selections			*/
#define	E514_060A    	(0x01)
#define	E514_100A    	(0x02)
#define	E514_250A    	(0x03)
#define  E515_2000V     (0x00)         /* E515 Waveform selection          */
#define  E515_4000V     (0x01)
#define	E518_600V		(0x00)			/* E518 Waveform selections			*/
#define	E518_1000V		(0x01)		
#define	E518_2000V		(0x02)

/*
 *	Options can run through 0x00000000 through 0xFFFFFFFF for a total
 *	of 32 different options.  The legacy options will retain their
 *	original defines for consistency
 */
#define	OPT_5CHINST		(0x00000001)
#define	OPT_EHVCAPABLE	(0x00000002)
#define	OPT_EFT8KV		(0x00000400)
#define	OPT_EFT2MHZ		(0x00000800)
#define	OPT_SWE502		(0x00001000)
#define	OPT_ACRMS		(0x00008000)
#define	OPT_EFTCHIRP	(0x00002000)
#define	OPT_DC_SURGE	(0x00004000)

/*
 *	High Voltage charging definitions used to indicate the current
 *	sequence point.  Currently, these are relevant only to SURGE
 *	and EFT modes.  The following lists the sequence points:
 *
 *		Idle		-	Unit is idle so a charge sequence can be started at
 *						any point in time
 *		Ramping	-	High Voltage is ramping up to the programmed voltage
 *						which can take many seconds.  Any interference will
 *						shut down the high voltage and any storage capacitors
 *						will be dumped
 *		Ready		-	All high voltage storage items are fully charged to the
 *						programmed level.  The trigger sequence can be started
 *						at any point in time.  Note that the high voltage will
 *						only stay in this state for 10 seconds due to safety
 *		Delay		-	A high voltage trigger sequence has completed and all
 *						high voltage items are in an idle state.  This state
 *						is used to allow components to cool down before the
 *						IDLE state is entered
 */
#define	CHG_IDLE		(0x00)
#define	CHG_RAMP		(0x01)
#define	CHG_READY		(0x02)
#define	CHG_DELAY		(0x03)

/*
 *
 */
#define	MAX_SRGDELAY	(255)
#define	MIN_SRGDELAY	(15)

/*
 *	Peak detector relay definitions.  The peak detector has the following
 *	relays for interacting with the peak detection hardware:
 *
 *		Negative peak		-	Switches between the positive and negative
 *									peak detection hardware
 *		50 Current			-	Switches in a 50-Ohm terminating resistor
 *									to the E100 front panel current monitor
 *		50 Voltage			-	Switches in a 50-Ohm terminating resistor
 *									to the E100 front panel voltage monitor
 *		Allow Current		-	Enables the current peak detection circuitry
 *									by un-shorting the storage capacitor
 *		Allow Voltage		-	Enables the voltage peak detection circuitry
 *									by -un-shorting the storage capacitor
 *		Fourth Coaxial		-	Selects between the second and fourth coaxial
 *									inputs which then feed into the current peak
 *									detection circuitry
 *		Third Coaxial		-	Switches between the first and third coaxial
 *									inputs which then feed into the voltage peak
 *									detection circuitry
 *		Select Coaxial		-	Switches between the voltage coaxials or the
 *									external voltage connection which then feeds
 *									into the voltage peak detection circuitry
 */
#define	PK_NEGPEAK		(0x8000)
#define	PK_INOT50		(0x4000)
#define	PK_VNOT50		(0x2000)
#define	PK_IALLOW		(0x1000)
#define	PK_VALLOW		(0x0800)
#define	PK_COAX4		(0x0400)
#define	PK_SELCOAX		(0x0200)
#define	PK_COAX3		(0x0100)

/*
*       Surge calibration data offsets
*  
*       Offsets into the CALINFO struct for Surge calibration data
*       This data structure CANNOT CHANGE, except possibly to expand,
*           because we don't track revisions.
*  
*       The CALINFO struct is organized as follows for Surge calibration data:
*  
*           Offset from     Data
*           start of        in
*           CALINFO         this
*           in ushorts*     section
*           -------------   -------
*                   0       K5 line sync delay
*                   1       K6 delay (not sure how or if this value is used)
*  
*                   2       Start of waveform-specific cal data
*  
*                   2       Front Panel cal for Waveform 0
*                   3       Line Coupler cal for Waveform 0
*                   4       Front Panel cal for Waveform 1
*                   5       Line Coupler cal for Waveform 1
*                   6       Front Panel cal for Waveform 2
*                   7       Line Coupler cal for Waveform 2
*                   8       Front Panel cal for Waveform 3
*                   9       Line Coupler cal for Waveform 3
*                   10      Front Panel cal for Waveform 4
*                   11      Line Coupler cal for Waveform 4
*  
*                   12      Data Coupler cal for Waveform 0
*                   13      Data Coupler cal for Waveform 1
*                   14      Data Coupler cal for Waveform 2
*                   15      Data Coupler cal for Waveform 3
*                   16      Data Coupler cal for Waveform 4
*  
*                   17-29   Supplemental cal area
*                           - used only for E503A_BOX and E507_BOX
*                           - custom format, unique to each module's usage requirements
*  
*                           For E503A (all values are ushort):
*                               The next four values are cal voltages, similar to standard "positive" calibration values
*                               (note that polarities of these "negative" cal values don't matter, all are converted
*                               to positive values before use):
*                                   17  Negative Front Panel cal for Waveform 0 (200 A)
*                                   18  Negative Line Coupler cal for Waveform 0 (200 A)
*                                   19  Negative Front Panel cal for Waveform 1 (500 A)
*                                   20  Negative Line Coupler cal for Waveform 1 (500 A)
*                               The next four values are % change from Single Line to PE
*                               (standard "LINE Coupler" cal adjustment) positive or negative
*                               Line Coupler cal values.  For more than Two Lines to PE,
*                               the "Two Lines" adjustment is added for each new line.
*                               For example if Two Lines to PE is 1.5%, Three Lines to PE is 3%
*                               and Four Lines to PE is 4.5%.
*                               For Line to Line, all variations (1, 2, and 3 Lines to Line)
*                               use the specified adjustment value.
*                               Values can be positive or negative.
*                                   21  Two Lines to PE adjustment for Waveform 0 (200 A)
*                                   22  Line to Line adjustment for Waveform 0 (200 A)
*                                   23  Two Lines to PE adjustment for Waveform 1 (500 A)
*                                   24  Line to Line adjustment for Waveform 1 (500 A)
*  
*                           For E507:
*                               The two ushort values at CALINFO offsets 17 and 18 are voltage
*                               offsets, one of which is always added to the magnitude of the
*                               proportionally adjusted calibration value (that is, it's added
*                               to positive values and subtracted from negative values).  This
*                               compensates for very-low-voltage (mostly sub-3 Volt) effects in
*                               the circuitry that distort the otherwise linear voltage response.
*                               The value at offset 17 is for the 2 Ohm waveform, and the value
*                               at offset 18 is for the 12 Ohm waveform.
*  
*                   30      Reserved for future cal data version control
*  
*                   31      Max RMS limit
*  
*                   * Note that CAL array indices in the Module Setup Dialog in SurgeWare
*                       is ONE-based, whereas these numbers are ZERO-based offsets.
*                       So for example, "Front Panel cal for Waveform 0" (see above) is
*                       at offset 2, but is CAL[3] in the Module Setup Dialog.
*/
#define CAL_K5OFFSET        (0)  // absolute offset (from start of CALINFO struct)
#define CAL_K6OFFSET        (1)  // absolute offset (from start of CALINFO struct)
#define	WAVE_OFFSET         (2)
#define	FP_OFFSET           (0)  // relative to WAVE_OFFSET
#define	LINE_OFFSET         (1)  // relative to WAVE_OFFSET
#define	FP_LINE_SIZE        (2)
#define	DATA_OFFSET         (12) // relative to WAVE_OFFSET
#define	DATA_SIZE           (1)
#define SUPPLEMENTAL_OFFSET (17)  // absolute offset (from start of CALINFO struct)
#define CAL_RMSMAX_OFF      (31)  // absolute offset (from start of CALINFO struct)

// E503A-specific calibration data offsets
#define E503A_CAL_NEG_FP_200A           (SUPPLEMENTAL_OFFSET + 0)
#define E503A_CAL_NEG_LINE_200A         (SUPPLEMENTAL_OFFSET + 1)
#define E503A_CAL_NEG_FP_500A           (SUPPLEMENTAL_OFFSET + 2)
#define E503A_CAL_NEG_LINE_500A         (SUPPLEMENTAL_OFFSET + 3)
#define E503A_CAL_TWO_LINES_TO_PE_200A  (SUPPLEMENTAL_OFFSET + 4)
#define E503A_CAL_LINE_TO_LINE_200A     (SUPPLEMENTAL_OFFSET + 5)
#define E503A_CAL_TWO_LINES_TO_PE_500A  (SUPPLEMENTAL_OFFSET + 6)
#define E503A_CAL_LINE_TO_LINE_500A     (SUPPLEMENTAL_OFFSET + 7)

// E507-specific calibration data offsets
#define E507_CAL_OFFSET_02_OHM          (SUPPLEMENTAL_OFFSET + 0)
#define E507_CAL_OFFSET_12_OHM          (SUPPLEMENTAL_OFFSET + 1)


/*
 *     Structure: CALINFO
 * Expanded Name: CalibrationInformation
 *       Purpose: Stores ALL of a module's calibration data along with a
 *					 checksum for data integrity
 *      Elements:
 *					 data					(98 bytes)
 *						An array of data points.  A generic array is used since
 *						the format of the values are module specific.  However,
 *						each calibration value is limited to a 16-bit unsigned
 *						value
 *					 chksum				(2 bytes)
 *						A checksum used to ensure the integrity of the
 *						calibration data.  If the checksum fails, the data
 *						should be set to all zeros (0) for safety
 */
typedef struct tag_calinfo
{
	ushort	data[CALINFOSIZE];		/* Holds ALL module-dependent cal data	*/
	ushort	chksum;						/* Secondary checksum for cal data		*/
} CALINFO;

/*
 *     Structure: MODINFO
 * Expanded Name: ModuleInformation
 *       Purpose: Stores ALL the module information that is contained in
 *					 the 9346 Serial EEPROM on each module's interface card
 *      Elements:
 *					 name					(8 bytes)
 *						The textual name of the module which is used only for
 *					 	display purposes only.  Note that all names whould be
 *						padded with spaces to a length of 5 characters
 *					 typePrimary		(32 bits)
 *						Indicates a module's type(s).  A module can be
 *						comprised of multiple types so each bit corresponds
 *						to one module type
 *					 typeSecondary		(32 bits)
 *						Another storage element for module types.  This is
 *						identical to the typePrimary element except that it
 *						is currently unused and is reserved for future use.
 *						This element should always be set to zero (0)
 *					 id					(8 bits)
 *						Identifies the module out of a group of types. Id
 *						values are not mutually exclusive so a total of 255
 *						modules can be defined for each module type
 *					 viMonitors			(8 bits)
 *						Identifies the number of voltage an current monitors
 *						installed in a module.  The voltage monitor use the
 *						upper 4 bits while the current monitors use up the
 *						lower 4 bits
 *					 options				(32 bits)
 *						Identifies all options installed in a module. A
 *						module can contain multiple options so each bit
 *						corresponds to one option.
 *					 serialNumber		(32 bits)
 *						Holds the KeyTek serial number of a module which
 *						is used for informational purposes only.  Since the
 *						maximum serial number is 9912999, 32 bits are needed
 *						to store the serial number
 *					 calinfo				(CALINFO SIZE)
 *						Holds all calibration data pertaining to a module.
 *						See the CALINFO data structure for more information
 */
typedef struct tag_modinfo
{
	char		name[MODNAMESIZE];		/* Actual Name of module (i.e. "E501")	*/
	uint		typePrimary;				/* Primary Module Type						*/
	uint		typeSecondary;				/* Secondary Module Type (expansion)	*/
	uchar		id;							/* Module Identification					*/
	uchar		viMonitors;					/* V/I option info (VMON and IMON)		*/
	uint		options;						/* Module OPTION information				*/
	uint		serialNumber;				/* Module SERIAL information				*/
	CALINFO	calinfo;						/* Module CALIBRATION information		*/
} MODINFO;

/*
 *     Structure: MODDATA
 * Expanded Name: ModuleData
 *       Purpose: Stores ALL the module information that is contained in
 *					 the 9346 Serial EEPROM on each module's interface card
 *					 as well as data storage for a module's relay drivers.
 *					 Two elements are offered for relay driver data.  Also,
 *					 two flags are incorporated for easy checksum verification
 *      Elements:
 *					 modinfo				(MODINFO SIZE)
 *						Holds all information from the module's serial EEPROM.
 *						See the MODINFO data structure for more information
 *					 calsumflag			(8 bits)
 *						Flag which indicates the status of the calibration
 *						checksum.  This can be used to quickly verify the
 *						integrity of the calibration data
 *					 chksumflag			(8 bits)
 *						Flag which indicates the status of the module data
 *						checksum.  Since this checksum is not accessible to
 *						anybody, this is a good way of verifying the integrity
 *						of the serial EEPROM data.
 *					 relay				(32 bits)
 *						Storage for an image of the 32 relay drivers present on
 *						every interface card
 *					 expansion			(32 bits)
 *						Storage for data that pertains to the expansion connector
 *						on the interface card.  32 bits have been allocated due
 *						to the fact that in many instances, this connector is
 *						used for controlling more relay drivers
 */
typedef struct tag_moddata
{
	MODINFO	modinfo;
	uchar		calsumflag;
	uchar		chksumflag;
	uint		relay;
	uint		expansion;
} MODDATA;

#endif								/* ifndef MODULE_H */

#define	PHASE_TPU		(6)
#define	SEQ_TPU			(1)
#define	CLOCK_SAFETY	(720)
#define	mode_WAIT		(2)
#define	mode_TIME		(1)

#define	A_ZERO			(0x5200)
#define	A_40				(0x5000)
#define	A_50				(0x4200)
#define	A_70				(0x4000)
#define	A_80				(0x1100)
#define	A_90				(0x1000)
#define	A_110				(0x0100)
#define	A_150				(0x0000)
#define	A_MASK			(0x5300)

#define	B_ZERO			(0xA800)
#define	B_40				(0xA000)
#define	B_50				(0x8800)
#define	B_70				(0x8000)
#define	B_80				(0x2400)
#define	B_90				(0x2000)
#define	B_110				(0x0400)
#define	B_120				(0x0000)
#define	B_MASK			(0xAC00)
#define	AB_MASK			(0xFF00)

#define	S_OPEN			DO_OPEN 
#define	S_SHORT			DO_SHORT
#define	S_40				DO_40   
#define	S_50				DO_50   
#define	S_70				DO_70   
#define	S_80				DO_80   
#define	S_90				DO_90   
#define	S_100				DO_100  
#define	S_110				DO_110  
#define	S_120				DO_120  
#define	S_150				DO_150  
#define	SCR_ON			(128)

#define	IG_100			(0x04)
#define	IG_A				(0x08)
#define	IG_B				(0x10)
#define	SC_100			(0x20)
#define	SC_A				(0x40)
#define	SC_B				(0x80)
#define	A_USED			(0x48)
#define	B_USED			(0x90)
#define	T60MSEC			(125829)
#define	T10MSEC			(20971)
#define	T1DEG				(96)

#define DO_BRANCH  (0x80)
#define DO_BRANCHN (0x81)
#define DO_GOSUB   (0x82)
#define DO_GOSUBN  (0x83)
#define DO_RETURN  (0x84)
#define DO_STOP    (0x85)
#define DO_PAUSE   (0x86)

#define DO_OPEN    (0x00)
#define DO_SHORT   (0x01)
#define DO_40      (0x02)
#define DO_50      (0x03)
#define DO_70      (0x04)
#define DO_80      (0x05)
#define DO_90      (0x06)
#define DO_100     (0x07)
#define DO_110     (0x08)
#define DO_120     (0x09)
#define DO_150     (0x0A)
#define DO_CYCLES  (0x40)

#define DO_FIXED_TAPS (1)
#define DO_LONG       (2)
#define CANT_DO       (255)

#define BAD_TARGET  (-1) /*The target address was Bad */
#define BAD_RETURN  (-2) /*No Gosub coresponding to return */
#define STACK_OVER  (-3) /*Gosub stack was too deep */
#define SETUP_BAD   (-4)
#define BAD_ACTION  (-5) /*An unknown action was parsed */
#define NO_AC       (-6) /*An unknown action was parsed */
#define TOOMANYTAPS (-7)
#define NO_EUT      (-8)
#define NO_EUTR     (-9)
#define OVER_RMS    (-10)
#define OVER_PEAK   (-11)
#define OVER_TEMP   (-12)
#define IGBT_LIMIT  (-13)
#define SNUB_BUS    (-14)

#define STEP_OK     ( 0)
#define END_LIST    ( 1)

#define MAXIMUM_LIST (300)

#define PQF_DISPLAY_NORMAL  (1)
#define PQF_DISPLAY_SETUP   (2)
#define PQF_DISPLAY_NONE    (3)
#define PQF_DISPLAY_NUMBERS (4)
#define PQF_DISPLAY_STORE   (5)
#define PQF_DISPLAY_RECALL  (6)
#define PQF_DISPLAY_ERROR   (7)
#define PQF_REMOTE          (8)
#define PQF_STARTUP         (9)
#define PQF_DISPLAY_QUALIFY (10)

#define	EP3_L1			(0x00080000)
#define	EP3_L2			(0x00100000)
#define	EP3_L3			(0x00200000)

typedef struct PQF_LIST_ELEMENT_TAG
{
	unsigned long  duration; 				/* Loop Alias as well	*/
	unsigned short start_angle;
	unsigned short RMS_limit; 				/* Tenths of an Amp		*/
	unsigned short PEAK_limit;				/* In Amps					*/
	int calc_RMS;
	int calc_PEAK;
	unsigned char  action;
	unsigned char Switch_Values;
} LIST_RECORD;

int		PQF_status_rep(P_PARAMT *);
int		PQF_idle_pgm(P_PARAMT *);
int		PQF_nominal_pgm(P_PARAMT *);
int		PQF_cheat(P_PARAMT *);
void		raw_stop();
void		SetUp_Display(int dv);

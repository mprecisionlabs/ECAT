#define SEQ_RAM_SIZE0	(32760)
#define SEQ_RAM_SIZE1	(131060)

#define SEQ_F_16M		(0xA000)
#define SEQ_F_8M			(0x2000)
#define SEQ_F_4M			(0xC000)
#define SEQ_F_1M			(0x4000)
#define SEQ_F_250K		(0x8000)
#define SEQ_F_62K		(0x0000)
#define SEQ_F_TPU		(0x6000)
#define SEQ_F_PAL		(0xE000)

#define SEQ_M_NORM		(0x0000)
#define SEQ_M_RESET		(0x0100)
#define SEQ_M_INCAD		(0x0200)
#define SEQ_M_LRAW		(0x0300)
#define SEQ_M_CRT		(0x0400)
#define SEQ_M_CST		(0x0500)
#define SEQ_M_CR			(0x0600)
#define SEQ_M_CS			(0x0700)
#define SEQ_M_WIPE		(0x1000)

#define SEQ_TPU			(1)
#define CLAMP_TPU		(3)
#define CHIRP_TPU		(5)
#define PE_TPU			(7)

#define	EFT_HIVENABLE	(0x00040000)
#define	EFT_POSPOL		(0x00080000)
#define	EFT_NEGPOL		(0x00100000)
#define	EFT_2MHZENABLE	(0x00400000)
#define	EFT_OUT1ENABLE	(0x00800000)
#define	EFT_OUT2ENABLE	(0x01000000)
#define	EFT_CHIRP		(0x02000000)
#define	EFT_ACCRISE		(0x04000000)
#define	EFT_LOWRANGE	(0x10000000)
#define	EFT_HIGHRANGE	(0x20000000)

#define	CPL_EFTL1		(0x00040000)	/* EFT coupling modes				*/
#define	CPL_EFTL2		(0x10000000)
#define	CPL_EFTL3		(0x00080000)
#define	CPL_EFTNU		(0x01000000)
#define	CPL_EFTPE		(0x02000000)
#define	CPL_EFTMASK		(0x130C0000)

#define	CPL_412L1		(0x00000100)
#define	CPL_412L2		(0x00000010)
#define	CPL_412PE		(0x00000008)
#define	CPL_412MASK		(0x00000118)
#define	CPL_412EUT		(0x00002000)	/* EUT enable is SPECIAL for 412	*/

#define	CPL_422EUT		(0x0001)
#define	CPL_422CHOKE	(0x0002)
#define	CPL_422REF0		(0x0004)
#define	CPL_422REF1		(0x0008)
#define	CPL_422L1		(0x0100)
#define	CPL_422L2		(0x0200)
#define	CPL_422L3		(0x0400)
#define	CPL_422NU		(0x0800)
#define	CPL_422PE		(0x1000)
#define	CPL_422MASK		(0x1F00)

#define	EFT_MINDUR		(1)
#define	EFT_MAXDUR		(200)
#define	EFT_MINPER		(300)
#define	EFT_MAXPER		(5000)
#define	EFT_MINFRQ		(1000)
#define	EFT_MAXFRQ1000	(1000000)
#define	EFT_MAXFRQ2000	(2000000)
#define	EFT_MAXFRQ250	(250000)
#define	EFT_MINVLT		(200)
#define	EFT_MAXVLT2000	(2000)
#define	EFT_MAXVLT3000	(3000)
#define	EFT_MAXVLT4400	(4400)
#define	EFT_MAXVLT8000	(8000)
#define	EFT_MINWAV		(0)
#define	EFT_MAXWAV		(1)

#define	CAL_LDELTAY		(1729000)
#define	CAL_HDELTAY		(1500000)
#define	CAL_LDAC			(104)
#define	CAL_HDAC			(1833)

#define CAL_CLAMP_OFFSET  (0)  /* front panel cal data is at start of serial EEPROM memory */
#define CAL_CPL_OFFSET  (6)  /* coupler cal data is in block after front panel data */
#define CAL_LV_OFFSET   (0)  /* offset for non-HV module data (E411), within each group (OUTPUT1 and coupler groups) */
#define CAL_HV_OFFSET   (3)  /* offset for HV module data (E421), within each group (OUTPUT1 and coupler groups) */
/* front panel cal data offsets */
#define	CAL_LLOOFF_CLAMP  (CAL_CLAMP_OFFSET + CAL_LV_OFFSET + 0)
#define	CAL_LHIOFF_CLAMP  (CAL_CLAMP_OFFSET + CAL_LV_OFFSET + 1)
#define	CAL_LINTOFF_CLAMP (CAL_CLAMP_OFFSET + CAL_LV_OFFSET + 2)
#define	CAL_HLOOFF_CLAMP  (CAL_CLAMP_OFFSET + CAL_HV_OFFSET + 0)
#define	CAL_HHIOFF_CLAMP  (CAL_CLAMP_OFFSET + CAL_HV_OFFSET + 1)
#define	CAL_HINTOFF_CLAMP (CAL_CLAMP_OFFSET + CAL_HV_OFFSET + 2)
/* coupler cal data offsets */
#define	CAL_LLOOFF_CPL  (CAL_CPL_OFFSET + CAL_LV_OFFSET + 0)
#define	CAL_LHIOFF_CPL  (CAL_CPL_OFFSET + CAL_LV_OFFSET + 1)
#define	CAL_LINTOFF_CPL (CAL_CPL_OFFSET + CAL_LV_OFFSET + 2)
#define	CAL_HLOOFF_CPL  (CAL_CPL_OFFSET + CAL_HV_OFFSET + 0)
#define	CAL_HHIOFF_CPL  (CAL_CPL_OFFSET + CAL_HV_OFFSET + 1)
#define	CAL_HINTOFF_CPL (CAL_CPL_OFFSET + CAL_HV_OFFSET + 2)

void		EFT_HVoff(void);
void		EFT_HVon(void);
void		TPU_toggle(uchar, int);
void		DO_BURST(void);
void		setup_chirp(void);
void		fill_seq_ram(uchar);
void		write_seq_ram(uchar);
void		seq_reset(uchar);
void		setup_burst(uint, uint);
uchar	EFT_StartSystem(uchar);
uchar	EFT_StopSystem(void);
void		EFT_CPLvalidate(void);
ushort	EFT_CalCompute(CALINFO *,int,uchar);
int 		E421P_MaxDuration(int voltage,uint freq,int period);
char		EFT_ClearCouplerHardware(void);

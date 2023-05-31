#ifndef __FRONT_H					/* Avoid multiple inclusions */
#define __FRONT_H

#include  <shortdef.h>

#define	DISPLAY_NOTHING				(0x00)
#define	DISPLAY_LOCAL					(0x01)
#define	DISPLAY_WARMUP					(0x02)
#define	DISPLAY_ESD						(0x03)
#define	DISPLAY_EFT						(0x04)
#define	DISPLAY_SRG						(0x05)
#define	DISPLAY_PQF						(0x06)
#define	DISPLAY_MEASURE				(0x07)
#define	DISPLAY_BAD_AC					(0x08)
#define	DISPLAY_BAD_CHARGE			(0x09)
#define	DISPLAY_REMOTE					(0x0a)
#define	DISPLAY_CALSRG					(0x0b)
#define	DISPLAY_CALEFT					(0x0c)
#define	DISPLAY_BAD_DC					(0x0d)
#define	DISPLAY_BAD_BLUE				(0x0e)
#define	DISPLAY_BAD_HV					(0x0f)
#define	DISPLAY_BAD_FRQ				(0x10)
#define	DISPLAY_BAD_RMS				(0x11)
#define	DISPLAY_BAD_AUX				(0x12)
#define	DISPLAY_BAD_BURST				(0x13)

#define	DISPLAY_COVER_INT				(0x20)
#define	DISPLAY_EXT_INT				(0x21)
#define	DISPLAY_THERM_INT				(0x22)
#define	DISPLAY_PCB_INT				(0x23)
#define	DISPLAY_MAINS_INT				(0x24)

#define	GO_LED				(0x0a)
#define	STOP_LED				(0x02)
#define	DISPLAY_DAC			(0x00)

#define	curxp()				(LCD_getx()+1)
#define	curyp()				(LCD_gety()+1)

#define	POF_BUF				(((NUMERIC_REC *)(pof->ref_rec))->buf)
#define	POF_NUMERIC			(((NUMERIC_REC *)(pof->ref_rec))->f)
#define	POF_MAX				(((NUMERIC_REC *)(pof->ref_rec))->maxv)
#define	POF_MIN				(((NUMERIC_REC *)(pof->ref_rec))->minv)
#define	POF_MINUS			(((NUMERIC_REC *)(pof->ref_rec))->minus)

#define	IL_ILOCK0			(0x1e)
#define	IL_ILOCK2			(0x1c)
#define	IL_ILOCK4			(0x18)
#define	IL_ILOCK6			(0x10)

typedef struct one_field
{
	uchar	x_pos;
	uchar	y_pos;
	uchar	x_end;
	uchar	y_end;
	uchar	xNext;
	uchar	xPrev;
	uchar	yNext;
	uchar	yPrev;
	char	(*process)(char c, int x, int y,struct one_field *pof);
	void	(*changed)();
	void	(*entered)(struct one_field *pof);
	void	(*left)(struct one_field *pof);
	uchar	valid;
	void	*ref_rec;
	char	(*leftfunc)(uchar);
} ONE_FIELD;

typedef struct numeric_rec
{
	int	f;
	int	maxv;
	int	minv;
	int	field;
	uchar	minus;
	char	buf[40];
} NUMERIC_REC;

typedef struct bool_rec
{
	int	field;
	char	bv;
	char	*false_label;
	char	*true_label;
	char	loc_len;
	char	dir;
} BOOL_REC;

typedef struct list_rec
{
	int	field;
	char	lv;
	char	**labels;
	char	loc_len;
	char	dir;
	uchar	dflag;
	uchar	valid;
} LIST_REC;

typedef	struct
{
	char	*listSrgNames;
	char	*listSrgIndex;
} LISTSRG;

void	field_validation(ONE_FIELD	*,uchar);
void	field_horizontal(uchar, uchar, uchar);
void	field_vertical(uchar, uchar, uchar);
void	list_entered(ONE_FIELD *);
void	list_left(ONE_FIELD *);
char	list_process(char, int, int, ONE_FIELD	*);
char	numeric_process(char	,int, int, ONE_FIELD	*);
void	numeric_entered(ONE_FIELD *);
void	numeric_left(ONE_FIELD *);
char	bool_process(char, int, int, ONE_FIELD *);
void	bool_entered(ONE_FIELD *);
void	bool_left(ONE_FIELD *);
void	init_numeric_field(int, int, int, NUMERIC_REC *, void (*fch)(ONE_FIELD *pof), char (*)(uchar),uchar);
void	init_bool_field(int, int, BOOL_REC *, void (*fch)(ONE_FIELD *pof), char (*)(uchar), char *, char *,uchar);
void	init_list_field(int, int, uchar, LIST_REC *, void (*fch)(ONE_FIELD *), char (*)(uchar), char **, uchar);
void	init_front(uchar);
void	parse_front(char);
char	translate_key(int);

uchar	main_funcs(char);
uchar	srg_funcs(char);
uchar	eft_funcs(char);
uchar	cal_funcs(char);
uchar	meas_funcs(char);
uchar	none_funcs(char);
uchar	eftcal_funcs(char);
void		show_state_srg(void);
void		show_state_eft(void);
void		show_state_srgmeas(void);
void		show_state_eftmeas(void);
void		show_state_none(char *, char *, uchar);
void		show_state_bad(char *);
void		show_state_chgbad(void);
void		show_state_calsrg(void);
void		show_state_caleft(void);

char	translate_key(int i);
void	init_front(uchar i);
void	parse_front(char c);
void	FP_UpdateRemote(void);
void	FP_UpdateWarmup(ushort);
void	SRG_UpdateCharge(ushort);
char	SRG_StartSystem(void);
char	SRG_SurgeSystem(void);
char	SRG_StopSystem(void);
void	Show_State(int);
void	ReportInterlockError(uint, FILE *);

#endif								/* ifndef __FRONT_H */




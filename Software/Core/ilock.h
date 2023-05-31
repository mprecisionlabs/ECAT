#ifndef ILOCK_H
#define ILOCK_H

#define CMD_ABORT_ILOCK			1102
#define CMD_PAUSE				1103

#define ILOCK_OK				0
#define ILOCK_POWERUP			1
#define ILOCK_ECAT				2
#define ILOCK_NOCOMM			3	 
#define ILOCK_SURGE_BASE		4
#define ILOCK_NO_SRG_IDLE		5 //1
#define ILOCK_SRG_BAD_NET		6 //2
#define ILOCK_NO_EUT_POW		7 //3
#define ILOCK_NO_EUT_BLUE		8 //4
#define ILOCK_CHG_NREADY		9 //5
#define ILOCK_SRG_ILOCK			10 //6
#define ILOCK_ANALOG_FAILED		11 //7
#define ILOCK_NO_DC_POW			12 //8
#define ILOCK_NO_HV_POW			13 //9
#define ILOCK_MISC				20
#define ILOCK_INTERNAL			21
#define ILOCK_EFT_BASE			30
#define ILOCK_EFT_NOT_IDLE		31
#define ILOCK_EFT_BAD_MODULE	32
#define ILOCK_EFT_AC_ERROR		33
#define ILOCK_EFT_NOT_BLUE		34
#define ILOCK_EFT_NOT_SET		35
#define ILOCK_EFT_INTERLOCK		36

class far ILOCK
{
public:
	ILOCK();
	~ILOCK();
	void Init(HWND Parent);
	void Show(BOOL on);
	void Check_Interlocks( long status);

protected:
	BOOL Interlock_Showing;
	HWND IlockWnd;
	HWND hParrent;
	FARPROC lpfnIlock;
	BOOL Created;

};
#endif























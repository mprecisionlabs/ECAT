#ifndef CALINFO_H
#define CALINFO_H
BOOL check_type();
void Cal_Read();
void Choose_Coupler(HWND hwnd);
void Choose_EFT_Coupler(HWND hwnd);
void Choose_EFT_Module(HWND hwnd);
void Get_Calinfo_Name(int i,LPSTR BayName);
void show_cal(HINSTANCE ghinst, HWND hMainDlg);
#endif


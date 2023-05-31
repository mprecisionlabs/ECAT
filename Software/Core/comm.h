#ifndef COMM_INFO
#define COMM_INFO

// ECAT comm exception class
class CEcatCommEx
{
public:
    CEcatCommEx(LPSTR lpszReason = NULL);
    virtual ~CEcatCommEx();

	UINT ShowReason(UINT uiMsgBoxFlags = MB_OK | MB_ICONWARNING);

private:
	LPSTR m_strReason;
};

//The comunications DLL will operate as follows:

//Start the communications
int InitComm(int port, DWORD dwBaudRate);

//Shut down the comunications system
void CloseComm();

//Trys to communicate with the ECAT system.
BOOL OpenEcatComm();

//Send a String out
//who and where are currently unused!
int SEND(int who,int where, const LPSTR what);

//Find out if chars are availible
//returns th number of chars recieved
int CHAR_RXED();

//Find out if the port was initialized prevoiusly
//Returns true when ever comm is already started
int Started(); 

BOOL PrintfECAT (int, char *,...);
int ECAT_EXIST(int Device);
long ECAT_Ilock();
BOOL QueryECAT(int device, LPSTR command, LPSTR buffer, int len, int delay=0);
BOOL QueryPrintfEcat(LPSTR tobuff,int len,char *,...);
int QueryPrintfEcatInt(int & err,char *,...);
int ECAT_OPC();
LPSTR ECAT_NAME(int bay);
int ECAT_SURGE_COUPLE();
void ECAT_sync(long dly = 1000);
void PURGE(void);
void GETRESPONSE(const LPSTR CMD,LPSTR copybuff,int maxlen, int delay=0);

#endif


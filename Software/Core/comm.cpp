///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "globals.h"
#include "message.H"
#include "comm.h"

static BOOL sync_hung = FALSE;
static HANDLE ComHandle = INVALID_HANDLE_VALUE;

#define COMM_IN_BUFF		1000
#define MAXTIMEOUT			1000

///////////////////////////////////////////////////////////////////////////////
// Uncomment the following string to prevent comm routines from spitting out a lot
// of communication crap
///////////////////////////////////////////////////////////////////////////////
#define HALT_COMM_DBGOUT

///////////////////////////////////////////////////////////////////////////////
// CEcatCommEx class
///////////////////////////////////////////////////////////////////////////////
CEcatCommEx::CEcatCommEx(LPSTR lpszReason/* = NULL*/) :
	m_strReason(NULL)
{
	if ((lpszReason != NULL) && (*lpszReason != '\0'))
	{
		m_strReason = new char[strlen(lpszReason)];
		if (m_strReason != NULL)
			strcpy(m_strReason, lpszReason);
	}
}

///////////////////////////////////////////////////////////////////////////////
CEcatCommEx::~CEcatCommEx()
{
	if (m_strReason != NULL)
		delete[] m_strReason;
}

///////////////////////////////////////////////////////////////////////////////
UINT CEcatCommEx::ShowReason(UINT uiMsgBoxFlags/* = MB_OK | MB_ICONWARNING*/)
{
	if (m_strReason != NULL)
		return MessageBox(NULL, m_strReason, "ECAT communication error", uiMsgBoxFlags);

	return IDCANCEL;
}

///////////////////////////////////////////////////////////////////////////////
static BOOL CheckComHandle()
{
	if (!SIMULATION && ComHandle == INVALID_HANDLE_VALUE) 
	{
		MessageBox(NULL, "Error: COMM handle is invalid", "COMM Error", MB_OK | MB_ICONINFORMATION);
		return FALSE;
	}

	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// Start the communications
///////////////////////////////////////////////////////////////////////////////
int InitComm(int port, DWORD dwBaudRate)
{
	// Make sure it's not open yet. If open, return true
	if (ComHandle != INVALID_HANDLE_VALUE) return 1;

	char comName[25];
	if ((port < 1) || (port > 4)) return 0;
	switch (dwBaudRate)
	{
	case CBR_2400:
	case CBR_4800:
	case CBR_9600:
	case CBR_19200:
		break;

	default:
		return 0;
	}
	sprintf(comName,"COM%u", port);

	// Standard Win32 way of opening the COM port
	ComHandle=CreateFile (
				comName,						// comm port string
				GENERIC_READ | GENERIC_WRITE,	// read/write types
				0, 								// comm devices must be opened with exclusive access
				NULL,							// no security attributes
				OPEN_EXISTING,					// comm devices must use OPEN_EXISTING
				FILE_ATTRIBUTE_NORMAL,			// always
				0);								// template must be 0 for comm devices
	if (ComHandle == INVALID_HANDLE_VALUE) return 0;

	sync_hung=FALSE;

	// Set timeouts
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout        = 0;
	timeouts.ReadTotalTimeoutMultiplier = 0;
	timeouts.ReadTotalTimeoutConstant   = 250;
	timeouts.WriteTotalTimeoutMultiplier= 0;
	timeouts.WriteTotalTimeoutConstant  = 250;
	int retOk = SetCommTimeouts(ComHandle, &timeouts);
	if (!retOk) goto CleanUp;

	// Build DCB parameters
	DCB dcb;
	memset(&dcb, 0, sizeof(dcb));
	dcb.DCBlength = sizeof(dcb);
	dcb.XonChar = 0x11;	       				// specify the Xon character
	dcb.XoffChar = 0x13;	       			// specify the Xoff character
	dcb.fNull = TRUE;	       				// strip null characters
	dcb.XonLim = 30;	       				// distance from queue empty to Xon
	dcb.XoffLim = (COMM_IN_BUFF/2) + 1; 	// distance from queue full to Xoff
	dcb.fBinary = TRUE;	        			// put into binary mode, pass all chars.
	dcb.fParity = TRUE;	        			// put into binary mode, pass all chars.
	dcb.fRtsControl = RTS_CONTROL_ENABLE;	// enable hardware flow ctl
	dcb.fDtrControl = DTR_CONTROL_ENABLE;   // enable hardware flow ctl
	dcb.BaudRate = dwBaudRate;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;
	dcb.ByteSize = 8;
	dcb.fAbortOnError = TRUE;

	// Set updated params
	retOk = SetCommState(ComHandle, &dcb);
	if (!retOk) goto CleanUp;

	PURGE();

CleanUp:
	// Clean up if an error has occurred (retOk is false)
	if (!retOk)
	{
		// Reset everything back to default state
		sync_hung = FALSE;
		if (ComHandle != INVALID_HANDLE_VALUE)
		{
			CloseHandle(ComHandle);
			ComHandle = INVALID_HANDLE_VALUE;
		}
	}

	return retOk;
}

///////////////////////////////////////////////////////////////////////////////
// Send a String out (will throw CEcatCommEx exception on error)
///////////////////////////////////////////////////////////////////////////////
int SEND(int who,int where, const LPSTR what)
{
	DWORD len = (DWORD) strlen(what);
	DWORD sent = 0;
#ifndef HALT_COMM_DBGOUT
	DBGOUT("TX:!%s!\n", what);
#endif
	if (SIMULATION)
		sent = len;
	else
	{
		if (!CheckComHandle())
			goto OnError;

		if (sync_hung) 
			PURGE();
		
		int iCnt = 0;
		while ((sent != len) && (iCnt < 3))
		{
			if (!::WriteFile(ComHandle, what, len, &sent, NULL))
				goto OnError;

			if (sent != len) 
				PURGE();
			iCnt++;
		}
	}

	if (sent != len) 
		goto OnError;

	return (int) sent;

OnError:
	//MessageBox(NULL, "Send error!", "COMM Error", MB_OK | MB_ICONEXCLAMATION);
	PURGE();
	throw CEcatCommEx("Send error!");
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// Find out if the port was initialized prevoiusly
///////////////////////////////////////////////////////////////////////////////
int Started() 
{
	return (ComHandle != 0);
}

///////////////////////////////////////////////////////////////////////////////
// Shut down the comunications system
///////////////////////////////////////////////////////////////////////////////
void CloseComm()
{
	if (ComHandle != INVALID_HANDLE_VALUE)
	{
		CloseHandle(ComHandle);
		ComHandle = INVALID_HANDLE_VALUE;
	}
}

///////////////////////////////////////////////////////////////////////////////
// Clean up the buffers
///////////////////////////////////////////////////////////////////////////////
void PURGE()
{
#ifndef HALT_COMM_DBGOUT
	DBGOUT("Purging COM.\n");
#endif
	// Clear the port
	COMSTAT	comStat;
	DWORD dwTemp;
	::ClearCommError(ComHandle, &dwTemp, &comStat);
	::PurgeComm(ComHandle, PURGE_TXCLEAR | PURGE_RXCLEAR);
	sync_hung = FALSE;
}

///////////////////////////////////////////////////////////////////////////////
static BOOL IsCompleteReply(LPCSTR lpBuf, DWORD dwEnd)
{
	for (DWORD i = dwEnd; i > 0; i--)
	{
		if (lpBuf[i] == ']')
			return TRUE;
	}
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
DWORD GetData(LPSTR lpBuf, int iBufLen, int iDelay)
{
	DWORD dwBytesRead = 0;
	while ((DWORD) iDelay > GetTickCount())
	{
		// Waiting for RX event
		DWORD dwTemp;
		COMSTAT	comStat;
		if (::ClearCommError(ComHandle, &dwTemp, &comStat) && (comStat.cbInQue > 0))
		{	
			// Collect the data
			DWORD dwBytesToRead = comStat.cbInQue;
			//DBGOUT("GetData. dwBytesToRead: %u.\n", dwBytesToRead);

			// Make sure there is no buffer overrun
			if ((dwBytesRead + dwBytesToRead) > (DWORD) iBufLen)
			{
				dwBytesToRead = (DWORD) iBufLen - dwBytesRead;
				if ((int) dwBytesToRead <= 0)
				{
					lpBuf[0] = '\0';
					return 0;
				}
			}

			DWORD dwBytesCnt = 0;
			while (dwBytesToRead > 0)
			{
				if (::ReadFile(ComHandle, (PBYTE) (lpBuf + dwBytesRead), dwBytesToRead, &dwBytesCnt, NULL))
				{
					dwBytesToRead -= dwBytesCnt;
					dwBytesRead += dwBytesCnt;
                    //DBGOUT("GetData(reading). dwBytesToRead: %u; dwBytesRead: %u; lpBuf: %s.\n", dwBytesToRead, dwBytesRead, lpBuf);
					
					if (IsCompleteReply(lpBuf, min(dwBytesRead, (DWORD) iBufLen)) || (dwBytesRead >= (DWORD) iBufLen))
					{
						//DBGOUT("GetData - complete reply.\n");
						return min(dwBytesRead, (DWORD) iBufLen);
					}
				}
				else
				{
					lpBuf[0] = '\0';
					return 0;
				}
			}
		}
	}
	//DBGOUT("GetData - timed out.\n");

	return dwBytesRead;
}

///////////////////////////////////////////////////////////////////////////////
void ECAT_sync(long tout)
{
	if (SIMULATION || COMMERROR) return;

	char lcl[MAX_PATH];
	memset(lcl, 0, sizeof(lcl));
	DWORD tc = GetTickCount() + tout;
	DWORD rxed = GetData(lcl, MAX_PATH, tc);
	if ((strstr(lcl,"]")))
	{
#ifndef HALT_COMM_DBGOUT
		DBGOUT("Sync OK\n");
#endif
		sync_hung = FALSE;
	}
	else  
	{
        #ifndef HALT_COMM_DBGOUT
		        DBGOUT("Missed Sync *****\n");
        #endif
		COMMERROR = TRUE;
        #ifndef HALT_COMM_DBGOUT
		        DBGOUT("set COMMERROR to TRUE [3]\n");
        #endif
		sync_hung = TRUE;
	}
	_strupr(lcl);

	if (strstr(lcl,"ERR"))
	{
		Log.logprintf("\r\nCommand Error: %Fs\r\n",(LPSTR)lcl);
		Message("Command Error:","%s",lcl);
	}
}

///////////////////////////////////////////////////////////////////////////////
void GETRESPONSE(const LPSTR CMD, LPSTR copybuff, int maxlen, int delay)
{
	// GETRESPONSE logic:
	// 1. Make sure we are not in simulation mode;
	// 2. Make sure there is no COMM error;
	// 3. If there is data in the buffer, read it
	// 4. Make sure the data we received has closing bracket;
	// 5. If not, read the port again, appending to the end of the last read;
	// 6. Is there a proper response CMD [xxxxxx];
	// 7. If not, set COMERROR flag

	memset(copybuff, 0, maxlen);
	if (SIMULATION || COMMERROR) return;
	
	char lcl[MAX_PATH + 1];
	memset(lcl, 0, sizeof(lcl));

	DWORD tc = GetTickCount() + MAXTIMEOUT + delay;
	DWORD rxed = GetData(lcl, MAX_PATH, tc);

#ifndef HALT_COMM_DBGOUT
	if (rxed > 0)
		DBGOUT("GR (1st try). Bytes cnt: %d. Data: <<%s>>.\n", rxed, lcl);
	else
		DBGOUT("GR (1st try): NO data.\n");
#endif

	// See if ECAT finished pesponding
	if (!(strstr(lcl, "]")))
	{
		// OK, try once more
		tc = GetTickCount() + MAXTIMEOUT;
		rxed = GetData(&lcl[rxed], MAX_PATH - rxed, tc);
#ifndef HALT_COMM_DBGOUT
		if (rxed > 0)
			DBGOUT("GR (2nd try). Bytes cnt: %d. Data: <<%s>>.\n", rxed, lcl);
		else
			DBGOUT("GR (2nd try): NO data.\n");
#endif
	}
#ifndef HALT_COMM_DBGOUT
	else
		DBGOUT("GR (complete). RX:!%s!\n", lcl);
#endif

	// Now, try to extract ECAT's reply from the data buffer
	
	// Move past the command literal (make sure it's the command we want)
	LPSTR lpszReply = strstr(lcl, CMD);
	if (lpszReply != NULL)
	{ 
		// Make sure we have opening bracket
		lpszReply = strchr(lpszReply, '[');
		if (lpszReply != NULL) 
		{
			lpszReply++;

			// Make sure we have closing bracket
			LPSTR lpszEnd = strchr(lpszReply, ']');
			if (lpszEnd != NULL)
			{
				strncpy(copybuff, lpszReply, min(lpszEnd - lpszReply, maxlen));
				return;
			}
		}
	}

	sync_hung = TRUE;
	COMMERROR = TRUE; 
#ifndef HALT_COMM_DBGOUT
	DBGOUT("GR - BAD reply.\n");
#endif
}

///////////////////////////////////////////////////////////////////////////////
BOOL OpenEcatComm()
{
	try
	{
		SEND(0, 0, "\r");

		DWORD tc = GetTickCount() + MAXTIMEOUT;
		char rxbuff[MAX_PATH];
		memset(rxbuff, 0, sizeof(rxbuff));
		DWORD rxed = GetData(rxbuff, MAX_PATH, tc);

		PURGE();
		QueryECAT(0, "*idn?", rxbuff, MAX_PATH);
		_strupr(rxbuff);
		return (strstr(rxbuff, "KEYTEK") != NULL); 
	}
	catch(CEcatCommEx ex)
	{
		return FALSE; 
	}
}

///////////////////////////////////////////////////////////////////////////////
long ECAT_Ilock()
{
	int err;
	int rv;
	if (SIMULATION) 
		return ILOCK_OK;

	if (COMMERROR) 
		return ILOCK_NOCOMM;

	rv = QueryPrintfEcatInt(err,"SYS:ILOCK?");
	if (err) 
		rv = QueryPrintfEcatInt(err,"SYS:ILOCK?");

	if (err) 
		return ILOCK_NOCOMM;
	
	return (rv) ? ILOCK_ECAT : ILOCK_OK;
}

///////////////////////////////////////////////////////////////////////////////
BOOL PrintfECAT (int device, char * szFormat,...)
{
	try
	{
		char szBuffer[MAX_PATH + 1];
		memset(szBuffer, 0, sizeof(szBuffer));
		va_list argList;
		va_start(argList, szFormat);
		_vsnprintf(szBuffer, MAX_PATH, szFormat, argList);
		va_end(argList);
		SEND(device,0,szBuffer);
		SEND(device,0,"\r");
		return TRUE;
	}
	catch(CEcatCommEx ex)
	{
		ex.ShowReason();
		return FALSE;
	}
}	// end of PrintfECAT().

///////////////////////////////////////////////////////////////////////////////
BOOL QueryECAT(int device, LPSTR command, LPSTR buffer, int len, int delay)
{
	try
	{
		memset(buffer, 0, len);
		PURGE();
		DBGOUT("QueryECAT() command: %s\n", command);
		SEND(device, 0, command);
		SEND(device, 0, " \r");
		if (!SIMULATION)
			GETRESPONSE(command, buffer, len, delay);
		return TRUE;
	}
	catch(CEcatCommEx ex)
	{
		return FALSE;
	}
}

///////////////////////////////////////////////////////////////////////////////
int ECAT_OPC()
{
	try
	{
		char buff[100];
		int iv=255;
		if (sync_hung) PURGE();
		strcpy(buff,"*OPC? \r");
		SEND(0,0,buff);
		if (SIMULATION) return 0;
		GETRESPONSE("*OPC?",buff,100);
		sscanf(buff,"%d",&iv);
		return iv;
	}
	catch(CEcatCommEx ex)
	{
		return 255;
	}
}

///////////////////////////////////////////////////////////////////////////////
BOOL QueryPrintfEcat(LPSTR tobuff,int len,char *szFormat,...)
{
	char szBuffer[MAX_PATH + 1];
	memset(szBuffer, 0, sizeof(szBuffer));
	va_list argList;
	va_start(argList, szFormat);
	_vsnprintf(szBuffer, MAX_PATH, szFormat, argList);
	va_end(argList);
	return QueryECAT(0,szBuffer,tobuff,len); 
}

///////////////////////////////////////////////////////////////////////////////
int QueryPrintfEcatInt(int & err,char *szFormat,...)
{
	char rxBuffer[40];
	int rv;
	int i;
	char szBuffer[MAX_PATH + 1];
	memset(szBuffer, 0, sizeof(szBuffer));
	va_list argList;
	va_start(argList, szFormat);
	_vsnprintf(szBuffer, MAX_PATH, szFormat, argList);
	va_end(argList);
	BOOL bOK = QueryECAT(0,szBuffer,rxBuffer,40); 
	if (SIMULATION) return 0;

	if (!bOK)
	{
		err = 1;
		return 255;
	}

	rv=sscanf(rxBuffer,"%d",&i);
	if (rv==1) 
		err=0; 
	else 
		err=1;
	return i;
}

 

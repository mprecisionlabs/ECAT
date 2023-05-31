#include "stdafx.h"

void  Message (char * szCaption, char *szFormat,...)
{
	char szBuffer[MAX_PATH + 1];
	memset(szBuffer, 0, sizeof(szBuffer));
	va_list argList;
	va_start(argList, szFormat);
	_vsnprintf(szBuffer, MAX_PATH, szFormat, argList);
	va_end(argList);
	MessageBox(NULL, szBuffer, szCaption, MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
}

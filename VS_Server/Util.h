#pragma once

#include "Com_Def.h"
class CUtil
{
public:
	CUtil(void);
	~CUtil(void);
	
	void ConvTimeToString( time_t Time, CString &str );
	void ConvTimeToIntDay(UINT32 nsec, int *ptrday );
	void ConvTimeToDirtPath(int nType, UINT32 nsec, TCHAR * ptrSTR );

	void GetCurPacketTime( char * Buf, int Size);
	void WcsToMcs( TCHAR * Org, char * Dest, int OrgSize );
	void McsToWcs( char * Org, TCHAR * Dest, int OrgSize );
	BOOL RAW8ToBMP8(BYTE* pBmp, BYTE* pRaw, DWORD dwLen , int ImageWidth  );	
	BOOL GetMyIP( char * szIP );
	void GetBeforTime( int Interval ,TCHAR * Dest );
	
	void KillProcess( TCHAR * ProcessName );
	BOOL RunProcess( TCHAR * Path, TCHAR * ProcessName);
	BOOL CreateDir( TCHAR  * ptrPath );
	int StringToInt( CString & strInt);

	void ConvertStringToTime( char *szTime, time_t * Time );
	void ConvertWStringToTime( TCHAR *csTime, time_t * Time );

	BOOL FindProcess( TCHAR * ProcessName );

private:
	
};


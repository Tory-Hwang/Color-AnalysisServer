#include "StdAfx.h"
#include "Util.h"
#include <tlhelp32.h>

#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")

CUtil::CUtil(void)
{
}

CUtil::~CUtil(void)
{
}

/*
void CUtil::GetCurTime( volatile time_t * Cur_Time )
{
	struct tm t_time;
	CTime t = CTime::GetCurrentTime();

	t_time.tm_year = ( int )( t.GetYear() - 1900 );
	t_time.tm_mon  = ( int )( t.GetMonth() - 1 );
	t_time.tm_mday = ( int )( t.GetDay() );
	t_time.tm_hour = ( int )( t.GetHour()  );
	t_time.tm_min  = ( int )( t.GetMinute() );
	t_time.tm_sec  = ( int )( t.GetSecond() );

	*Cur_Time = mktime( &t_time );	
}
*/
void CUtil::ConvTimeToString( time_t Time, CString &str )
{
	struct tm today ;
	CString strTime;

	if ( Time >0 )
	{
		localtime_s( &today, &Time ) ;
		strTime.Format(L"%02d%02d-%02d%02d%02d", 
			        today.tm_mon + 1,today.tm_mday, today.tm_hour, today.tm_min, today.tm_sec);		
		str = strTime;
	}
}

void CUtil::ConvTimeToDirtPath(int nType, UINT32 nsec, TCHAR * ptrSTR )
{
	struct tm today ;	
	__time32_t Time =0;
	
#if 0
	// 2015_0223
	nsec -=( 600 * 6 * 9 );
#endif
	Time = nsec;
	if ( Time >0 )
	{		
		_localtime32_s( &today, &Time ) ;
		wsprintf(ptrSTR, L"%04d%02d%02d", today.tm_year + 1900,today.tm_mon + 1,today.tm_mday);
	}
}


void CUtil::ConvTimeToIntDay( UINT32 nsec, int *ptrday )
{
	struct tm today ;	
	__time32_t Time =0;
#if 0
	// 2015_0223
	nsec -=( 600 * 6 * 9 );
#endif

	Time = nsec;
	if ( Time >0 )
	{
		if ( ptrday != NULL )
		{
			_localtime32_s( &today, &Time );
			*ptrday = today.tm_mday;
		}
	}
}

void CUtil::GetCurPacketTime( char * Buf , int Size )
{
	time_t Cur_Time;
	struct tm today ;

	Cur_Time = time( NULL );
	localtime_s(&today, &Cur_Time ) ;
	sprintf_s( Buf, Size,"%d%02d%02d%02d%02d%02d",
		today.tm_year + 1900,today.tm_mon + 1,today.tm_mday, today.tm_hour, today.tm_min, today.tm_sec);

}

BOOL CUtil::RAW8ToBMP8(BYTE* pBmp, BYTE* pRaw, DWORD dwLen, int ImageWidth )
{
	DWORD dwHeight = 0;

	/* 이미지 가로크기는 고정사이즈라 가정한다. */
	dwHeight = ( dwLen / ImageWidth );

	/* 비트맵 파일 헤더 */
	PBITMAPFILEHEADER pBmpFileHeader = (PBITMAPFILEHEADER)pBmp;
	pBmpFileHeader->bfType = ( (WORD)('M'<<8) | 'B');
	pBmpFileHeader->bfOffBits = (DWORD)(sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ( sizeof(RGBQUAD) * 256 ));
	pBmpFileHeader->bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ( 256 * sizeof(RGBQUAD) ) + ( ImageWidth * dwHeight );
	pBmpFileHeader->bfReserved1 = 0;
	pBmpFileHeader->bfReserved2 = 0;


	/* 비트맵 인포헤더 */
	LPBITMAPINFO pBmpInfo = (LPBITMAPINFO)(pBmp + sizeof(BITMAPFILEHEADER));

	pBmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	pBmpInfo->bmiHeader.biWidth = ImageWidth;
	pBmpInfo->bmiHeader.biHeight = dwHeight;
	pBmpInfo->bmiHeader.biPlanes = 1;
	pBmpInfo->bmiHeader.biBitCount = 8;
	pBmpInfo->bmiHeader.biCompression = BI_RGB;
	pBmpInfo->bmiHeader.biSizeImage = ImageWidth * dwHeight;
	pBmpInfo->bmiHeader.biXPelsPerMeter = 0;
	pBmpInfo->bmiHeader.biYPelsPerMeter = 0;
	pBmpInfo->bmiHeader.biClrUsed = 256;
	pBmpInfo->bmiHeader.biClrImportant = 0;

	/* 팔레트 설정 */
	for(int i=0; i<256; i++)
	{
		pBmpInfo->bmiColors[i].rgbRed = (BYTE)i;
		pBmpInfo->bmiColors[i].rgbGreen = (BYTE)i;
		pBmpInfo->bmiColors[i].rgbBlue = (BYTE)i;
		pBmpInfo->bmiColors[i].rgbReserved = 0x00;
	}

	BYTE* pBuffer = pBmp + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + ( sizeof(RGBQUAD) * 256 );
	BYTE* pImage = pRaw + ( ImageWidth * (dwHeight - 1) );

	for( DWORD i=0; i< ( dwHeight - 0 ); i++)
	{
		memcpy(pBuffer, pImage, ImageWidth);
		pBuffer += ImageWidth;
		pImage -= ImageWidth;
	}

	return TRUE;
}
BOOL CUtil::GetMyIP( char * szIP )
{
	DWORD Err;

	PIP_ADAPTER_INFO pAdapterInfo, pAdapt;
	DWORD AdapterInfoSize;
	PIP_ADDR_STRING pAddrStr;
	char szSubNet[ MAX_IP_SIZE * 10 ];

	memset( szSubNet, 0, sizeof(szSubNet));
	AdapterInfoSize = 0;

	if ((Err = GetAdaptersInfo(NULL, &AdapterInfoSize)) != 0)
	{
		if (Err != ERROR_BUFFER_OVERFLOW)
		{
			TRACE("GetAdaptersInfo sizing failed with error %d\n", Err);
			return FALSE;
		}
	}

	// Allocate memory from sizing information
	if ((pAdapterInfo = (PIP_ADAPTER_INFO) GlobalAlloc(GPTR, AdapterInfoSize)) == NULL)
	{
		TRACE("Memory allocation error\n");
		return FALSE;
	}

	// Get actual adapter information
	if ((Err = GetAdaptersInfo(pAdapterInfo, &AdapterInfoSize)) != 0)
	{
		printf("GetAdaptersInfo failed with error %d\n", Err);
		return 0;
	}

	pAdapt = pAdapterInfo;
	while (pAdapt)
	{
		pAddrStr = &(pAdapt->IpAddressList);

		while(pAddrStr)
		{
			//TRACE("\tIP Address. . . . . . . . . : %s\n", pAddrStr->IpAddress.String);
			//TRACE("\tSubnet Mask . . . . . . . . : %s\n", pAddrStr->IpMask.String);
			sprintf_s( szSubNet,sizeof( szSubNet), "%s", pAddrStr->IpMask.String);

			if ( strlen( szSubNet) > 10 )
			{
				//pAddrStr->IpAddress.String;
				memset( szSubNet, 0, sizeof( szSubNet ));
				sprintf_s( szSubNet,sizeof( szSubNet), "%s", pAddrStr->IpAddress.String );
				memcpy( szIP, szSubNet, strlen( szSubNet) ); 

				return TRUE;
			}
			pAddrStr = pAddrStr->Next;
		}

		pAdapt = pAdapt->Next;
	}

	return FALSE;
}

void CUtil::WcsToMcs( TCHAR * Org, char * Dest, int OrgSize)
{
	USES_CONVERSION;
	strcpy_s( Dest, OrgSize + 1, W2A( Org) );
}

void CUtil::McsToWcs( char * Org, TCHAR * Dest, int OrgSize)
{
	USES_CONVERSION;	
	wcscpy_s( Dest, OrgSize + 1 , A2W( Org) );	
}

void CUtil::GetBeforTime( int Interval ,TCHAR * Dest)
{
	time_t Cur_Time;
	struct tm today ;

	Cur_Time = time( NULL );

	Cur_Time -= Interval;

	localtime_s(&today, &Cur_Time ) ;

	wsprintf(Dest, L"%d%02d%02d%02d%02d%02d", today.tm_year + 1900,today.tm_mon + 1,today.tm_mday, today.tm_hour, today.tm_min, today.tm_sec);		

}


void CUtil::KillProcess( TCHAR * ProcessName )
{
	HANDLE			hProcessSnap = NULL; 
	PROCESSENTRY32	pe32         = {0};
	DWORD Code = 0;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //TH32CS_SNAPMODULE
	if (hProcessSnap != INVALID_HANDLE_VALUE) 
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hProcessSnap, &pe32))
		{ 
			do 
			{
				HANDLE hProcess = NULL; 
				hProcess = OpenProcess (PROCESS_ALL_ACCESS,  FALSE, pe32.th32ProcessID);             
				TCHAR * Temp = pe32.szExeFile;

				if (_wcsnicmp(Temp, ProcessName, wcslen( ProcessName )) == 0 )
				{
					if (TerminateProcess(hProcess, 0))
					{
						GetExitCodeProcess(hProcess, &Code);
						CloseHandle( hProcess );
						break;
					}
				}

				CloseHandle (hProcess);
			} 
			while (Process32Next(hProcessSnap, &pe32));
		} 

		CloseHandle(hProcessSnap);
	}
}




BOOL CUtil::RunProcess( TCHAR * Path, TCHAR * ProcessName)
{
	CString strRun;

	if ( Path == NULL || ProcessName == NULL )
		return FALSE;

	if(wcslen(ProcessName) < 1)
		return FALSE;
	strRun.Format(L"%s\\%s", Path, ProcessName );
	strRun.ReleaseBuffer();
#if 0
	STARTUPINFO stInfo;
	PROCESS_INFORMATION prInfo;

	ZeroMemory( &prInfo, sizeof(prInfo) );
	ZeroMemory( &stInfo, sizeof(stInfo) );

	stInfo.cb			= sizeof(stInfo);
	stInfo.dwFlags		= STARTF_USEPOSITION;
	stInfo.wShowWindow  = SW_SHOW;
	stInfo.dwX			= 100;
	stInfo.dwY			= 100;

	return CreateProcess( strRun, L"open",	NULL, NULL, TRUE, NORMAL_PRIORITY_CLASS, NULL, NULL,&stInfo, &prInfo );
#else

	SHELLEXECUTEINFO sInfo;
	BOOL bResult;
	bResult = FALSE;
	ZeroMemory(&sInfo, sizeof(SHELLEXECUTEINFO ));
	sInfo.cbSize  = sizeof(SHELLEXECUTEINFO);
	// sInfo.lpFile  = _T("mmsys.cpl");
	sInfo.lpFile  = strRun;
	sInfo.lpParameters = _T("");
	sInfo.nShow   = SW_HIDE;
	sInfo.fMask   = SEE_MASK_NOCLOSEPROCESS;
	//sInfo.lpVerb  = "open";
	//sInfo.hwnd  = m_hWnd;

	bResult = ShellExecuteEx(&sInfo); 
	//return bResult;
#endif
	return bResult;
}

BOOL CUtil::CreateDir( TCHAR  * ptrPath )
{
	WIN32_FIND_DATA Info;
	HANDLE HDL;	
	BOOL bRet = FALSE;

	HDL = NULL;	
	HDL = FindFirstFile(ptrPath, &Info);	

	do
	{			
		if (Info.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY || Info.dwFileAttributes == 0x00002010) 
		{
			FindClose( HDL );
			return TRUE;
		}

	}while(FindNextFile(HDL, &Info));

	if (HDL != INVALID_HANDLE_VALUE)
		FindClose( HDL );
	
	bRet = CreateDirectory( ptrPath,NULL);	
	return  bRet;

}

int CUtil::StringToInt( CString & strInt)
{
	USES_CONVERSION;
	char szTemp[ MAX_PATH ];

	if ( strInt.GetLength() == 0 )
		return -1;

	memset(szTemp, 0 , sizeof( szTemp ) );

	strcpy_s( szTemp, MAX_PATH, W2A( strInt) );
	return atoi( szTemp );
}

void CUtil::ConvertStringToTime( char *szTime, time_t * Time )
{
	char szYear[ 10 ];
	char szMonth[ 10 ];
	char szDay[ 10 ];
	char szHour[ 10 ];
	char szMin[ 10 ];
	char szSec[ 10 ];
	char * ptr;
	int nYear, nMonth, nDay, nHour, nMin, nSec;	
	struct tm t_time;		
	time_t TempTime =0;

	ptr = szTime;
	memset( szYear, 0, sizeof( szYear ));
	memset( szMonth, 0, sizeof( szMonth ));
	memset( szDay, 0, sizeof( szDay ));

	memset( szHour, 0, sizeof( szHour ));
	memset( szMin, 0, sizeof( szMin ));
	memset( szSec, 0, sizeof( szSec ));

	memcpy( szYear , ptr, 4 );
	nYear = atoi( szYear );
	ptr +=4;

	memcpy(szMonth, ptr, 2 );
	nMonth = atoi( szMonth );
	ptr +=2;

	memcpy(szDay, ptr, 2 );
	nDay = atoi( szDay );
	ptr +=2;

	memcpy(szHour, ptr, 2 );
	nHour = atoi( szHour );
	ptr +=2;

	memcpy(szMin, ptr, 2 );
	nMin = atoi( szMin );
	ptr +=2;

	memcpy(szSec, ptr, 2 );
	nSec = atoi( szSec );
	ptr +=2;

	if ( nYear < 2010 )
		return ;	

	t_time.tm_year = nYear - 1900;
	t_time.tm_mon  = nMonth -1;
	t_time.tm_mday = nDay;
	t_time.tm_hour = nHour;
	t_time.tm_min  = nMin;
	t_time.tm_sec  = nSec;

	
	TempTime = mktime( &t_time );
	*Time = TempTime;

}


void CUtil::ConvertWStringToTime( TCHAR *csTime, time_t * Time )
{
	char szTime[ MAX_PATH ];

	memset( szTime, 0, sizeof( szTime ));

	WcsToMcs( csTime, szTime, wcslen( csTime ));
	ConvertStringToTime( szTime, Time );

}

BOOL CUtil::FindProcess( TCHAR * ProcessName )
{
	HANDLE			hProcessSnap = NULL; 
	PROCESSENTRY32	pe32         = {0};
	DWORD Code = 0;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0); //TH32CS_SNAPMODULE
	if (hProcessSnap != INVALID_HANDLE_VALUE) 
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hProcessSnap, &pe32))
		{ 
			do 
			{
				HANDLE hProcess = NULL; 
				hProcess = OpenProcess (PROCESS_ALL_ACCESS,  FALSE, pe32.th32ProcessID);             
				TCHAR * Temp = pe32.szExeFile;

				if (_wcsnicmp(Temp, ProcessName, wcslen( ProcessName )) == 0 )
				{
					GetExitCodeProcess(hProcess, &Code);
					CloseHandle( hProcess );
					CloseHandle(hProcessSnap);
					return TRUE;
				}

				CloseHandle (hProcess);
			} 
			while (Process32Next(hProcessSnap, &pe32));
		} 

		CloseHandle(hProcessSnap);
	}

	return FALSE;
}
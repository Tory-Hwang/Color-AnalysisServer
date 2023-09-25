#include "StdAfx.h"
#include "FileManager.h"
#include "SVR_Doc.h"
#include <atlimage.h>


/* static 함수를 위한 instance */
CFileManager * pFileManager = NULL;

CFileManager::CFileManager(void)
:m_Controller( NULL ),
m_strCurPath ( NULL ),
m_szCreatedRootDir( NULL)
{
	
	pFileManager = this;
}

CFileManager::~CFileManager(void)
{

}
/* =====================================================================
Internal Functions 
=======================================================================*/

void CFileManager::DumpStr( CString &str )
{
#if DEBUG_FILEMANAGER
	CString strtxt;
	strtxt.Format(L"VS_SERVER::CFileManager : %s \n", str);
	strtxt.ReleaseBuffer();
	OutputDebugString( strtxt );
#endif
}

BOOL CFileManager::CreateDir( CString & strPath )
{
	WIN32_FIND_DATA Info;
	HANDLE HDL;	

	if (strPath.Right(1) == "\\") 
	{
		strPath = strPath.Mid(0, strPath.GetLength()-1);
	}

	HDL = NULL;	
	HDL = FindFirstFile(strPath, &Info);
	
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

	return  CreateDirectory( strPath,NULL);

}
void CFileManager::InitClientDir()
{
	int i;

	memset( m_CreatedClientDir, 0, sizeof( ClientDir_t ) * MAX_TCP_CLIENT_CNT );
	
	for( i = 0 ; i < MAX_TCP_CLIENT_CNT ; i++ )
	{
		m_CreatedClientDir[ i ].Cur_Hour = -1;
	}

}

BOOL CFileManager::isFindRootDirectory( char * szTime, TCHAR * strTime  )
{
	
	CString strRootDir;
	BOOL bResult = FALSE;

	if ( szTime != NULL )
	{
		m_Util.McsToWcs(szTime, strTime ,strlen(szTime));
	}
	
	if( strncmp(szTime, m_szCreatedRootDir, strlen( szTime)) == 0 )
	{
		return TRUE;	
	}
	
	m_Util.McsToWcs( szTime, strTime, strlen(szTime) );
	strRootDir.Format(L"%s\\%s", m_stSvrInfo.csImageSavePath, strTime );
	strRootDir.ReleaseBuffer();

	bResult = CreateDir( strRootDir );	

	if ( bResult == TRUE )
	{
		memcpy( m_szCreatedRootDir, szTime , strlen( szTime ));
		/* Root 디렉토리가 새로생성되면 Client Directory도 새로 생성함 */
		InitClientDir();
	}

	return bResult;
}



BOOL CFileManager::isFindClientDirectory( int ClientID  , TCHAR * strTime, int Day, int Hour, int *Basic_Hour )
{
	int i;
	int Id = 0;
	BOOL Result = FALSE;
	BOOL Result2 = TRUE;

	CString strClientDir;	
	int Cur_Dir = 0;
	int Cur_Day = 0;
	CString strMsg;
	int Temp=0;

	if ( Hour < MAX_DIR_HOUR )
	{
		Temp = 0;
	}
	else if ( Hour >= 4 && Hour < 8)
	{
		Temp = 4;
	}
	else if ( Hour >= 8 && Hour < 12)
	{
		Temp = 8;
	}
	else if ( Hour >= 12 && Hour < 16)
	{
		Temp = 12;
	}
	else if ( Hour >= 16 && Hour < 20)
	{
		Temp = 16;
	}
	else if ( Hour >= 20 )
	{
		Temp = 20;
	}
	else
		return FALSE;

	
	for( i = 0 ; i < MAX_TCP_CLIENT_CNT ; i++ )
	{
		Id = m_CreatedClientDir[ i ].ID;
		Cur_Day = m_CreatedClientDir[ i ].Cur_Day;
		Cur_Dir = m_CreatedClientDir[ i ].Cur_Hour;

		if ( Id == 0)
		{	
			break;
		}
		else if ( ( Id == ClientID ) && ( Cur_Day == Day ) && ( Cur_Dir == Temp)  )
		{			
			Result = TRUE;
			break;
		}
	}

	strMsg.Format(L" ClientID( %d ), Cur_Day_RecvDay( %d, %d ), Cur_Hour( %d, %d  )", ClientID, Cur_Day, Day, Cur_Dir, Temp  );
	DumpStr( strMsg );

	if ( Result == FALSE && ClientID != 0 )
	{
		if ( Cur_Day != Day )
		{
			strClientDir.Format(L"%s\\%s\\%010d", m_stSvrInfo.csImageSavePath, strTime , ClientID );
			strClientDir.ReleaseBuffer();
			Result2 = CreateDir( strClientDir );	

		}

		if ( Result2 == TRUE  && ( Cur_Dir != Temp ))
		{
			strClientDir.Format(L"%s\\%s\\%010d\\%02d", m_stSvrInfo.csImageSavePath, strTime , ClientID , Temp );
			strClientDir.ReleaseBuffer();
			Result2 = CreateDir( strClientDir );	
		}
		
		if ( Result2 == TRUE && ( i < MAX_TCP_CLIENT_CNT ) )
		{			
			m_CreatedClientDir[ i ].ID = ClientID;
			m_CreatedClientDir[ i ].Cur_Day = Day;
			m_CreatedClientDir[ i ].Cur_Hour = Temp;
		}

		Result = Result2;
	}

	*Basic_Hour = Temp;
	return Result;
}

int CFileManager::MakeDirectory( DummyMsg_t * DummMsg )
{
	int DataSize;
	void * ptrData;
	void * EtcData;

	VSImage_t * VSImage;
	int ClientID = 0;

	TCHAR strTime[ MAX_PATH ];
	char szTime[ MAX_PATH ];
	char szFileName[ MAX_PATH ];
	char szHour[ MAX_PATH ];
	int Cur_Dir = 0;

	//char szCurPath[ MAX_PATH ];
	
	DataSize = DummMsg->DataSize;
	ptrData = DummMsg->ptrData;
	EtcData = DummMsg->ptrEtcInfo;


	memset( szTime, 0, sizeof( szTime ));
	memset( strTime, 0, sizeof( strTime ) );
	memset( szFileName, 0, sizeof( szFileName ));
	memset( szHour, 0, sizeof( szHour ));

	//memset( szCurPath, 0, sizeof( szCurPath ));

	VSImage = ( VSImage_t *)ptrData;
	ClientID = VSImage->ClientID;
	
	memcpy( szTime, VSImage->ImageTime, 8);
	memcpy( szHour, &VSImage->ImageTime[ 8 ], 2 );

	if ( pFileManager->isFindRootDirectory( szTime, strTime ) == TRUE )
	{
		if ( pFileManager->isFindClientDirectory( ClientID , strTime , atoi( szTime ), atoi( szHour ) ,&Cur_Dir ) == TRUE )
		{
			//pFileManager->m_Util.WcsToMcs( pFileManager->m_strCurPath, szCurPath , wcslen( pFileManager->m_strCurPath) );
			sprintf_s(szFileName, sizeof( szFileName)  , "%s\\%010d\\%02d\\%s-%s.bmp", szTime, ClientID, Cur_Dir, VSImage->ImageTime, VSImage->CarNum );
			memcpy(VSImage->ImagePath, szFileName, strlen( szFileName ));
			return 0;
		}
	}

	return -1;
}

int CFileManager::SaveImageFile( DummyMsg_t * DummMsg )
{

	int DataSize;
	void * ptrData;
	void * EtcData;

	BYTE * BmpBuf = NULL;
	CFile *fileDest = NULL;
	CFileException  ex;
	char * szFileName;
	TCHAR strFileName[ MAX_PATH ];
	TCHAR strFileName2[ MAX_PATH ];
	CWnd * ptrdc;

	DataSize = DummMsg->DataSize;
	ptrData = DummMsg->ptrData;
	EtcData = DummMsg->ptrEtcInfo;

	BmpBuf = ( BYTE * )ptrData;
	szFileName = ( char *)EtcData;
	fileDest = ( CFile *)DummMsg->ptrEtcInfo2;
	ptrdc = ( CWnd *)DummMsg->ptrEtcInfo3;


	if ( BmpBuf == NULL || DataSize == 0 || szFileName == NULL || fileDest == NULL  )
	{
		return -1;
	}

	memset( strFileName, 0, sizeof( strFileName) );
	memset( strFileName2, 0, sizeof( strFileName2) );

	pFileManager->m_Util.McsToWcs( szFileName, strFileName , strlen( szFileName ) );
	wsprintf( strFileName2, L"%s\\%s", pFileManager->m_stSvrInfo.csImageSavePath, strFileName );

	if( !fileDest->Open( strFileName2, CFile::modeWrite | CFile::shareExclusive | CFile::modeCreate, &ex) )
	{			
		return -1 ;
	}
	else
	{
		fileDest->Write( BmpBuf, DataSize );
	}

	fileDest->Close();			

	
	return 0;
}

void CFileManager::GetServerInfo( void * ptr )
{
	CString strInit;
	TCHAR strID[ MAX_TIME_LEN ];

	TCHAR strIP[ MAX_TIME_LEN ];
	TCHAR strNetmask[ MAX_TIME_LEN ];
	TCHAR strHeight[ MAX_TIME_LEN ];
	TCHAR strWidth[ MAX_TIME_LEN ];
	TCHAR strStorage[ MAX_TIME_LEN * 3 ];

	TCHAR strDSN[ MAX_TIME_LEN ];
	TCHAR strDBId[ MAX_TIME_LEN ];
	TCHAR strDBPwd[ MAX_TIME_LEN ];
	TCHAR strTcpPort[ MAX_TIME_LEN ];
	TCHAR strUdpPort[ MAX_TIME_LEN ];
	TCHAR strMonitorPrg[ MAX_TIME_LEN ];

	TCHAR strDBUse[ MAX_TIME_LEN ];
	TCHAR strRunMonitor[ MAX_TIME_LEN ];
	TCHAR strMinCarSeeTime[ MAX_TIME_LEN ];
	TCHAR strForceUpdateTime[ MAX_TIME_LEN ];

	TCHAR strWIFI_IP[ MAX_WIFI_LEN ];
	TCHAR strSSID[ MAX_WIFI_LEN ];
	TCHAR strKEY[ MAX_WIFI_LEN ];

	TCHAR strAWS_HOST[ MAX_AWS_HOST_SIZE ];
	TCHAR strAWS_PORT[ MAX_TIME_LEN ];
	TCHAR strAWS_ID[ MAX_AWS_ID_SIZE ];


	char szTemp[ MAX_TIME_LEN];
	CString strTemp;
	int nID = 0;
	int Size ;
	VSSERVERINFO_t * ptrServer = NULL;

	ptrServer = ( VSSERVERINFO_t * )ptr;
	
	if ( ptrServer == NULL )
		return ;
	
	Size = sizeof( strID);
	memset( strID, 0,  Size );
	memset( strDSN, 0,  Size );
	memset( strDBId, 0,  Size );
	memset( strDBPwd, 0,  Size );
	memset( strTcpPort, 0,  Size );
	memset( strUdpPort, 0,  Size );
	memset( strMonitorPrg, 0,  Size );
	
	memset( strIP, 0,  Size );
	memset( strNetmask, 0,  Size );
	memset( strHeight, 0,  Size );
	memset( strWidth, 0,  Size );
	memset( strStorage, 0,  Size );
	memset( strDBUse, 0,  Size );
	memset( strRunMonitor, 0,  Size );
	memset( strMinCarSeeTime, 0,  Size );
	memset( strForceUpdateTime, 0,  Size );

	memset( strWIFI_IP, 0, sizeof( strWIFI_IP ));
	memset( strSSID, 0, sizeof( strSSID ));
	memset( strKEY, 0, sizeof( strKEY ));
	
	memset( strAWS_HOST, 0, sizeof( strAWS_HOST ));
	memset( strAWS_PORT, 0, sizeof( strAWS_PORT ));
	memset( strAWS_ID, 0, sizeof( strAWS_ID ));
	

	strInit.Format(L"%s\\ServerConf.ini", m_strCurPath );
	strInit.ReleaseBuffer();

	GetPrivateProfileString(_T("SERVER_CONF"),_T("MYID"),_T(""),strID, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("DB_DSN"),_T(""),strDSN, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("DB_ONE"),_T(""),strDBId, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("DB_TWO"),_T(""),strDBPwd, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("TCP_PORT"),_T(""),strTcpPort, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("UDP_PORT"),_T(""),strUdpPort, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("MONITOR_PRG"),_T(""),strMonitorPrg, Size ,strInit);

	GetPrivateProfileString(_T("SERVER_CONF"),_T("MYIP"),_T(""),strIP, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("NETMASK"),_T(""),strNetmask, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("IMG_HEIGHT"),_T(""),strHeight, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("IMG_WIDTH"),_T(""),strWidth, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("STORAGE_DIR"),_T(""),strStorage, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("DBUSE"),_T(""),strDBUse, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("RUNMONITOR"),_T(""),strRunMonitor, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("MIN_CARSEE_TIME"),_T(""),strMinCarSeeTime, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("FORCE_UPDATE_TIME"),_T(""),strForceUpdateTime, Size ,strInit);

	GetPrivateProfileString(_T("SERVER_CONF"),_T("WIFI_IP"),_T(""),strWIFI_IP, Size ,strInit);
	Size = MAX_WIFI_LEN;
	GetPrivateProfileString(_T("SERVER_CONF"),_T("WIFI_SSID"),_T(""),strSSID, Size ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("WIFI_KEY"),_T(""),strKEY, Size ,strInit);



	GetPrivateProfileString(_T("SERVER_CONF"),_T("AWS_HOST"),_T(""),strAWS_HOST, MAX_AWS_HOST_SIZE ,strInit);	
	GetPrivateProfileString(_T("SERVER_CONF"),_T("AWS_PORT"),_T(""),strAWS_PORT, 50 ,strInit);
	GetPrivateProfileString(_T("SERVER_CONF"),_T("AWS_ID"),_T(""),strAWS_ID, MAX_AWS_HOST_SIZE ,strInit);



	
	if (wcslen( strTcpPort ) == 0)	{
		
		strTemp.Format(L"%d", TCP_DEFAULT_PORT );
		strTemp.ReleaseBuffer();
		WritePrivateProfileString(_T("SERVER_CONF"),_T("TCP_PORT"),strTemp,strInit);
		ptrServer->nTcpPort = TCP_DEFAULT_PORT;		
	}
	else
	{
		memset( szTemp, 0, sizeof( szTemp));
		m_Util.WcsToMcs( strTcpPort, szTemp, wcslen( strTcpPort));		
		ptrServer->nTcpPort = ( UINT16)atoi( szTemp );
	}

	if (wcslen( strUdpPort ) == 0)	{

		strTemp.Format(L"%d", UDP_DEFAULT_PORT );
		strTemp.ReleaseBuffer();
		WritePrivateProfileString(_T("SERVER_CONF"),_T("UDP_PORT"),strTemp,strInit);
		ptrServer->nUdpPort =UDP_DEFAULT_PORT;
	}
	else
	{
		memset( szTemp, 0, sizeof( szTemp));
		m_Util.WcsToMcs( strUdpPort, szTemp, wcslen( strUdpPort));		
		ptrServer->nUdpPort = ( UINT16)atoi( szTemp );
	}
	
	if (wcslen( strIP ) == 0)
	{
		memset( szTemp, 0, sizeof( szTemp ));

		if ( m_Util.GetMyIP( szTemp )== TRUE )
		{
			m_Util.McsToWcs( szTemp, strIP, (int)strlen( szTemp ));
			WritePrivateProfileString(_T("SERVER_CONF"),_T("MYIP"),strIP ,strInit);
			wcscpy_s( ptrServer->csIP, strIP);
		}
	}
	else
	{		
		wcscpy_s( ptrServer->csIP, strIP);
	}

	if (wcslen( strNetmask ) == 0)
	{
		wcscpy_s( strStorage,  DEFAULT_NETMASK );
		WritePrivateProfileString(_T("SERVER_CONF"),_T("NETMASK"),strStorage ,strInit);
		wcscpy_s( ptrServer->csNetmask, strStorage );
	}
	else
	{		
		wcscpy_s( ptrServer->csNetmask, strNetmask);
	}

	
	if (wcslen( strStorage ) == 0)
	{
		wcscpy_s( strStorage,  DEFAULT_STORAGE_DIR );
		WritePrivateProfileString(_T("SERVER_CONF"),_T("STORAGE_DIR"),strStorage ,strInit);
		wcscpy_s( ptrServer->csImageSavePath, strStorage );
	}
	else
	{		
		wcscpy_s( ptrServer->csImageSavePath, strStorage );
	}

		

	if (wcslen( strWIFI_IP ) == 0)	
	{
		WritePrivateProfileString(_T("SERVER_CONF"),_T("WIFI_IP"),DEFAULT_WIFI_IP,strInit);
		wcscpy_s( ptrServer->csWIFI_IP, DEFAULT_WIFI_IP );
	}
	else
	{
		wcscpy_s( ptrServer->csWIFI_IP, strWIFI_IP );
	}


	if (wcslen( strSSID ) == 0)	
	{
		WritePrivateProfileString(_T("SERVER_CONF"),_T("WIFI_SSID"),DEFAULT_WIFI_SSID,strInit);
		wcscpy_s( ptrServer->csSSID, DEFAULT_WIFI_SSID );
	}
	else
	{
		wcscpy_s( ptrServer->csSSID, strSSID );
	}

	if (wcslen( strKEY ) == 0)	
	{
		WritePrivateProfileString(_T("SERVER_CONF"),_T("WIFI_KEY"),DEFAULT_WIFI_KEY,strInit);
		wcscpy_s( ptrServer->csKEY, DEFAULT_WIFI_KEY );
	}
	else
	{
		wcscpy_s( ptrServer->csKEY, strKEY );
	}

	
	if (wcslen( strAWS_HOST ) == 0)	
	{
		WritePrivateProfileString(_T("SERVER_CONF"),_T("AWS_HOST"),DEFUALT_AWS_HOST,strInit);
		wcscpy_s( ptrServer->csAWS_HOST, DEFUALT_AWS_HOST );
	}
	else
	{
		wcscpy_s( ptrServer->csAWS_HOST, strAWS_HOST );
	}

	if (wcslen( strAWS_PORT ) == 0)	{

		strTemp.Format(L"%d", DEFAULT_AWS_PORT );
		strTemp.ReleaseBuffer();
		WritePrivateProfileString(_T("SERVER_CONF"),_T("AWS_PORT"),strTemp,strInit);
		ptrServer->nAWS_Port =DEFAULT_AWS_PORT;
	}
	else
	{
		memset( szTemp, 0, sizeof( szTemp));
		m_Util.WcsToMcs( strAWS_PORT, szTemp, wcslen( strAWS_PORT));		
		ptrServer->nAWS_Port = ( UINT16)atoi( szTemp );
	}
	

	memset( &m_stSvrInfo, 0, sizeof( VSSERVERINFO_t));
	memcpy( &m_stSvrInfo, ptrServer, sizeof( VSSERVERINFO_t));
	
}

void CFileManager::SetAlert( void )
{
	/////////////////////////////////////////////////////////////////////////////////////////////////
	/*
	2010-07-29 추가 백승범
	시스템 상태 경보 발생 적용
	*/

	int nCPUAlert, nRAMAlert, nRecvAlert, nSendAlert, nHDDAlert, nDefault = 0;
	CString strSectionName, strKeyName, strInit, strTemp;

	strInit.Format(L"%s\\ServerConf.ini", m_strCurPath );
	strInit.ReleaseBuffer();

	strSectionName = L"SYSTEM_ALERT";
	strTemp.Empty();

	strKeyName = L"CPUALERT";
	nCPUAlert = GetPrivateProfileInt(strSectionName, strKeyName, nDefault, strInit);
	if(nCPUAlert == 0)
	{
		strTemp = L"80";
		//WritePrivateProfileString(strSectionName, strKeyName, strTemp, strInit);
		nCPUAlert = DEFAULT_CPU_ALERT;
	}

	strKeyName = L"RAMALERT";
	nRAMAlert = GetPrivateProfileInt(strSectionName, strKeyName, nDefault, strInit);
	if(nRAMAlert == 0)
	{
		strTemp = L"80";
		//WritePrivateProfileString(strSectionName, strKeyName, strTemp, strInit);
		nRAMAlert = DEFUALT_RAM_ALERT;
	}

	strKeyName = L"RECVALERT";
	nRecvAlert = GetPrivateProfileInt(strSectionName, strKeyName, nDefault, strInit);
	if(nRecvAlert == 0)
	{
		strTemp = L"80";
		//WritePrivateProfileString(strSectionName, strKeyName, strTemp, strInit);
		nRecvAlert = DEFAULT_RECV_ALERT;
	}

	strKeyName = L"SENDALERT";
	nSendAlert = GetPrivateProfileInt(strSectionName, strKeyName, nDefault, strInit);
	if(nSendAlert == 0)
	{
		strTemp = L"80";
		//WritePrivateProfileString(strSectionName, strKeyName, strTemp, strInit);
		nSendAlert = DEFAULT_SEND_ALERT;
	}

	strKeyName = L"HDDALERT";
	nHDDAlert = GetPrivateProfileInt(strSectionName, strKeyName, nDefault, strInit);
	if(nHDDAlert == 0)
	{
		strTemp = L"150";
		//WritePrivateProfileString(strSectionName, strKeyName, strTemp, strInit);
		nHDDAlert = DEFAULT_HDD_ALERT;
	}


	/* Doc 에 저장 */	
	CSVR_Doc* pDoc = (CSVR_Doc*)this->m_Controller->GetSVR_DocHandler();
	pDoc->SetCPUAlert(nCPUAlert);
	pDoc->SetMemAlert(nRAMAlert);
	pDoc->SetRecvAlert(nRecvAlert);
	pDoc->SetSentAlert(nSendAlert);
	pDoc->SetHddAlert(nHDDAlert);

	/////////////////////////////////////////////////////////////////////////////////////////////////
}

/* =====================================================================
Export Functions 
=======================================================================*/

int CFileManager::Initialize( void * ptr )
{
	CString strMsg;	
	TCHAR strBlank[ MAX_PATH ];
	

	m_strCurPath = 	( TCHAR * )malloc( sizeof( TCHAR ) * MAX_PATH );
	m_szCreatedRootDir = (char *)malloc( sizeof( char ) * MAX_PATH );;		

	memset( m_strCurPath, 0, sizeof( TCHAR ) * MAX_PATH );
	memset( m_szCreatedRootDir, 0, MAX_PATH);	
	InitClientDir();
	memset( strBlank, 0, sizeof( strBlank) );

	if (GetModuleFileName( AfxGetInstanceHandle( ), strBlank, sizeof( strBlank) ) == 0 )
	{
		return -1;
	}		

	strMsg.Format(L"%s", strBlank );
	strMsg.ReleaseBuffer();
	strMsg = strMsg.Left(strMsg.ReverseFind('\\'));
	wcscpy_s(m_strCurPath, strMsg.GetLength() + 1 , strMsg );

	/* 등록 */
	m_MsgController.AddDummyFunctions( DUMMY_MSG_MAKE_DIR, MakeDirectory );
	m_MsgController.AddDummyFunctions( DUMMY_MSG_SAVE_FILE, SaveImageFile);

	/* 기본 설정 파일 가져 오기 */
	GetServerInfo( ptr );

	strMsg.Format(L"Init CFileManager");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );

	return 0;
}

BOOL CFileManager::Open( ModelConf_t * ModelConf )
{
	( void )ModelConf;
	return TRUE;
}

int CFileManager::Close( void )
{
	return 0;
}
int CFileManager::Release( void )
{
	CString strMsg;

	if ( m_strCurPath != NULL )
	{
		free( m_strCurPath );
		m_strCurPath = NULL;
	}
	if ( m_szCreatedRootDir != NULL )
	{
		free( m_szCreatedRootDir );
		m_szCreatedRootDir = NULL;
	}
	
	strMsg.Format(L"Release CFileManager");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );

	return 0;
}
int CFileManager::Idle( void )
{
#if 0
	CString strMsg;

	strMsg.Format(L"Idle CFileManager");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );
#endif

	return 0;
}
BOOL CFileManager::SetControllerHandler( void *  ControllerHandle )
{
	m_Controller = (IController *)ControllerHandle;
	
	SetAlert();

	return TRUE;
}

int CFileManager::OnRecvMsg( MsgInfo_t * MsgInfo )
{
	return m_MsgController.OnRecvMsg( MsgInfo );
}

int CFileManager::OnRecvSubModelMsg( void * SubModelMsg )
{
	( void )SubModelMsg;
	return 0;
}

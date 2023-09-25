#include "StdAfx.h"
#include "UDPClient.h"
#include "Com_Def.h"
#include "DB_Def.h"
#include "SVR_Doc.h"
#include "VS_Server.h"

#define TEST_DLG

CUDPClient::CUDPClient(void)
:m_Controller( NULL),
m_ServerID ( 0 ),
m_LastSendTime( 0 ),
m_LastRecvTime( 0 ),
m_bConnected ( FALSE ), 
bFirsetSend( TRUE)
{
}

CUDPClient::~CUDPClient(void)
{
}

void CUDPClient::DumpStr( CString &str )
{
#if DEBUG_TCPSERVER
	CString strtxt;
	strtxt.Format(L"VS_SERVER::CUDPClient : %s \n", str);
	strtxt.ReleaseBuffer();
	OutputDebugString( strtxt );
#endif
}
#define MAX_RERUN_TIME ( 80 )

void CUDPClient::ReStartMonitoringPrg( void )
{
	TCHAR * strMonitorPrgName= NULL;
	CSVR_Doc * ptrDoc = NULL;
	VSSERVERINFO_t * ptrServerInfo = NULL;
	CString strRun;
	MsgInfo_t MsgInfo;
	QueryMsg_t QueryMsg;
	VST_MONITORING MonitorTbl;
	static time_t Kill_time =0;
	time_t Cur_time = time( NULL );
	CString strErr;

	if ( m_Controller != NULL )
	{
		ptrDoc = (CSVR_Doc*)m_Controller->GetSVR_DocHandler();
		if ( ptrDoc != NULL )
		{
			ptrServerInfo = ptrDoc->GetServerInfo();
			if ( ptrServerInfo != NULL )
			{
				strMonitorPrgName = ( TCHAR *)&ptrServerInfo->csMonitorPrgName[ 0 ];
				if ( strMonitorPrgName != NULL )
				{
					CString strMsg;
					strMsg.Format( IDS_RERUN_MONITORING_SERVER);
					strMsg.ReleaseBuffer();
					
					if ( Kill_time == 0 )
					{
						m_Util.KillProcess( strMonitorPrgName );
						Kill_time = Cur_time;
						strErr.Format(L"Kill Process (%s )", strMonitorPrgName );
						DumpStr( strErr );
						strErr.ReleaseBuffer();

						return ;
					}
					else if ( ( Cur_time - Kill_time ) >= MAX_RERUN_TIME )
					{
						strErr.Format(L"Path( %s ), Run Process (%s )", m_strCurPath, strMonitorPrgName );
						DumpStr( strErr );
						strErr.ReleaseBuffer();

						m_Util.RunProcess( m_strCurPath, strMonitorPrgName );
						memset( &MsgInfo, 0, sizeof( MsgInfo_t));
						memset( &QueryMsg, 0, sizeof( QueryMsg_t));
						memset( &MonitorTbl, 0, sizeof( VST_MONITORING ));

						MonitorTbl.nServerID = m_ServerID;
						MonitorTbl.cSYSCode = SYS_SEVER;
						wcscpy_s( MonitorTbl.csWorkDesc, strMsg.GetLength() + 1, strMsg );

						QueryMsg.Msg = QUERY_MONITORING_INSERT;
						QueryMsg.DataSize = sizeof( VST_MONITORING ) ;
						QueryMsg.ptrData = &MonitorTbl;

						MsgInfo.SendMsgType = SEND_QUERY_MSG;
						MsgInfo.ptrMsg = &QueryMsg;

						if ( m_Controller != NULL )
						{
							m_Controller->SendMsg( TARGET_DBMANAGER, &MsgInfo );
						}

						SendHeartbitMsg();
						Kill_time = 0;
					}
					
				}
			}
		}
	}		
}

void CUDPClient::SendHeartbitMsg( void)
{
	
}

void CUDPClient::ProcHeartbit( void )
{
	time_t Cur_time;	

	
	Cur_time = time( NULL );	

	/* 지정된 시간이 되면 Monitoring Server에 Heart Bit 을 보낸다 */
	if ( m_bConnected && (( Cur_time - m_LastSendTime ) >=  MONITERING_HEARTBIT_MESSAGE_INTERVAL ))
	{
		SendHeartbitMsg();
	}

	/* 지정된 시간보다 크면 Monitoring Server를 재 실행 한다. */
	if ( bFirsetSend && ( ( Cur_time - m_LastRecvTime ) >= ( MAX_RECV_MESSAGE_INTERVAL ) ))
	{
		m_LastRecvTime = Cur_time;
		//ReStartMonitoringPrg();
	}
}

BOOL CUDPClient::ParsePacket( int Size, BYTE * Buf )
{
	short Msg;		

	memset(&m_RecvMsg, 0, sizeof( ClientRecvMsg_t ));		
	m_LastRecvTime = time( NULL );

	if ( m_Parser.ParsePacket( NO_CRC, Size, Buf, &m_RecvMsg ) == TRUE )
	{		
		Msg = m_RecvMsg.Msg;
#if 0		
		switch( Msg )
		{
			case MSG_VS_RESP_HEARTBIT :
			{				

				CString strMsg;
				strMsg.Format(L" Recv from Monitoring Server Msg( 0x%02x )", Msg );
				DumpStr( strMsg );

				return TRUE;
			}
		}
#endif
	}

	return FALSE;
}

DWORD CUDPClient::UDPRead_Thread(LPVOID Param)
{
	CUDPClient *pThis = ( CUDPClient * )Param;	
	//char RecvBuf[ MAX_RECV_BUF_SIZE ];	
	char * RecvBuf= NULL;	
	int RecvSize;
	CString strMsg;

	RecvBuf = new char [MAX_RECV_BUF_SIZE  ];

	/*UDP --> TCP 로 변경 함 */	
	while ( pThis->m_bConnected  && pThis->m_ServSock != NULL )
	{		
		memset( RecvBuf, 0, MAX_RECV_BUF_SIZE);
		RecvSize = recv( pThis->m_ServSock , RecvBuf, MAX_RECV_BUF_SIZE, 0 );

		if ( RecvSize <= 0 )
		{
			strMsg.Format(L"Recv Data is Zero ");
			pThis->DumpStr( strMsg );
			break;
		}
		
		pThis->ParsePacket( RecvSize, (BYTE*)RecvBuf );

	}
	pThis->UDPClientClose();

	if ( RecvBuf != NULL )
	{
		delete [] RecvBuf;
	}

	return 0;
}

BOOL CUDPClient::UDPClientOpen( char * szServerIP, UINT16 SendPort )
{
	int nreuse = 1;	
	struct linger ling;
	CString strMsg;

	static volatile int Opening = 0;

	/* 모든 버퍼를 버리고 즉시 종료 */
	ling.l_onoff =1;
	ling.l_linger = 0;

	if ( szServerIP == NULL || SendPort == 0 )
	{
		return TRUE;
	}

	if ( Opening == 1 )
		return TRUE;

	Opening = 1;

	if ( m_bConnected == TRUE )
	{
		strMsg.Format(L"Already UDP Connected\n");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );		
		Opening = 0;
		return TRUE;
	}

	if (WSAStartup( MAKEWORD( 2, 2 ), &m_WsaData ) != 0)
	{		
		strMsg.Format(L"Error UDP WSAStartup");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );
		Opening = 0;
		return TRUE;
	}
	
	m_ServSock = socket( PF_INET, SOCK_STREAM, 0 );

	setsockopt( m_ServSock, SOL_SOCKET ,SO_REUSEADDR,  (char*)&nreuse, sizeof(nreuse) );
	setsockopt( m_ServSock, SOL_SOCKET ,SO_LINGER,  (char*)&ling, sizeof(struct linger) );

	if ( m_ServSock == INVALID_SOCKET )
	{		
		strMsg.Format(L"ERROR UDP SOCKET ");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );
		Opening = 0;
		return TRUE;
	}

	memset( &m_ServAddr, 0, sizeof( m_ServAddr));
	m_ServAddr.sin_family		= AF_INET;
	m_ServAddr.sin_addr.s_addr = inet_addr( szServerIP );

	m_ServAddr.sin_port			= htons( SendPort );

	if ( connect( m_ServSock, ( struct sockaddr*)&m_ServAddr, sizeof(m_ServAddr)) == SOCKET_ERROR )
	{			
		shutdown( m_ServSock, SD_BOTH );
		closesocket( m_ServSock );
		WSACleanup();
		strMsg.Format(L"UDP Connect Error...");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );
		Opening = 0;
		return TRUE;
	}

	if ( m_dwRead == 0 )
	{
		m_hThRead = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)UDPRead_Thread, this, 0,&m_dwRead);
	}
	
	m_bConnected = TRUE;

	/* Socket이 생성되자 마자 Monitoring Server에 Heart bit 메세지를 보낸다 */
	
	SendHeartbitMsg();
	
	strMsg.Format(L"Success CUDPClient Open");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );

	Opening = 0;
	return TRUE;
}

void CUDPClient::UDPClientClose( void )
{
	CString strMsg;

	if ( m_bConnected == FALSE)
		return ;

	m_bConnected	= FALSE;
	if ( m_dwRead != 0 )
	{
		m_dwRead	= 0;
		::WaitForSingleObject(m_hThRead, 100 );
		CloseHandle(m_hThRead);
		m_hThRead	= NULL;		
	}

	if ( m_ServSock != NULL )
	{			
		shutdown( m_ServSock, SD_BOTH );
		closesocket( m_ServSock );
		WSACleanup();
		m_ServSock		= NULL;	
		m_hThRead	= NULL;
		m_dwRead	= 0;		
	}	

	strMsg.Format(L"CUDPClient Close");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );
}

/* =====================================================================
Export Functions 
=======================================================================*/

int CUDPClient::Initialize( void )
{	
	CString strMsg;	
	TCHAR strBlank[ MAX_PATH ];
	
	memset( m_strCurPath, 0, sizeof( m_strCurPath ) );
	memset( strBlank, 0, sizeof( strBlank) );

	m_dwRead = 0;
	m_hThRead = NULL;
	m_bConnected = FALSE;

	if (GetModuleFileName( AfxGetInstanceHandle( ), strBlank, sizeof( strBlank) ) == 0 )
	{
		return -1;
	}		

	strMsg.Format(L"%s", strBlank );
	strMsg.ReleaseBuffer();
	strMsg = strMsg.Left(strMsg.ReverseFind('\\'));
	wcscpy_s( m_strCurPath, strMsg.GetLength() + 1 , strMsg );

	strMsg.Format(L"Init CUDPClient");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );

	return 0;
}

BOOL CUDPClient::SetTinyWindow( ITinyWindow * TinyWindow )
{	
	( void)TinyWindow;

	return 0;
}

int CUDPClient::Open( TransferConf_t * TransferConf )
{	
	int Rect;
	static int Flag = 0;

	return TRUE;

	if ( Flag == 0 )
	{
		Flag = 1;
		memset( &m_TransferConf, 0, sizeof(TransferConf_t ));
		memcpy( &m_TransferConf, TransferConf,sizeof(TransferConf_t ));		
		m_LastRecvTime = time( NULL);
	}

	Rect = UDPClientOpen( TransferConf->szUDPServerIP, TransferConf->UDPSendPort );	
	return Rect;
}

int CUDPClient::Close( void )
{
	UDPClientClose();
	return 0;
}

int CUDPClient::Release( void )
{
	CString strMsg;
	
	UDPClientClose();
	strMsg.Format(L"Release CUDPClient");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );

	return 0;
}

int CUDPClient::Idle( void )
{	

	static time_t Idle_time = 0;
	time_t Cur_Time = time( NULL );

	if ( m_Controller->GetRunMonitor() == FALSE )
		return 0;

	if ( Idle_time == 0 )
	{
		Idle_time = Cur_Time;
	}
	else if ( Cur_Time - Idle_time >= 3 )
	{
		if ( m_bConnected == FALSE )
		{
			Open( &m_TransferConf);
		}

		ProcHeartbit();
		Idle_time = time( NULL );
	}
	
	return 0;
}

BOOL CUDPClient::SetControllerHandler( void *  ControllerHandle )
{
	m_Controller = (IController *)ControllerHandle;

	if ( m_Controller != NULL )
	{
		m_ServerID = m_Controller->GetMyServerID();
	}

	return TRUE;
}

int CUDPClient::OnRecvMsg( MsgInfo_t * MsgInfo )
{
	( void )MsgInfo;
	return 0;
}


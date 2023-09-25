#include "StdAfx.h"
#include "TCPServer.h"
#include "SVR_Doc.h"

#include <mstcpip.h>

/* static 함수를 위한 instance */
CTCPServer * pTCPServer = NULL;

CTCPServer::CTCPServer(void)
:m_TinyWindow( NULL ), 
m_Controller( NULL ),
m_bConnected( FALSE ),
m_ServerID( 0 )
{
	pTCPServer = this;
}

CTCPServer::~CTCPServer(void)
{
}
void CTCPServer::DumpStr( CString &str )
{
#if DEBUG_TCPSERVER
	CString strtxt;
	strtxt.Format(L"VS_SERVER::CTCPServer : %s \n", str);
	strtxt.ReleaseBuffer();
	OutputDebugString( strtxt );
#endif
}


int CTCPServer::SendController( BYTE TagetObject, short Msg, void * ptr )
{
	MsgInfo_t MsgInfo;

	memset( &MsgInfo, 0, sizeof( MsgInfo_t ));	
	if ( ptr !=NULL )
	{
		MsgInfo.SendMsgType = Msg;
		MsgInfo.ptrMsg = ptr;

		if ( m_Controller != NULL )
		{
			return m_Controller->SendMsg( TagetObject, &MsgInfo );
		}
	}

	return -1;
}

void CTCPServer::InitSocket()
{
	m_ServSock		= NULL;	
	m_hTh_Accepter	= NULL;
	m_dwAccepter	= 0;
	m_bConnected	= FALSE;

}

BOOL CTCPServer::SetTCPServerListen(char *szServerIP, UINT16 ServerPort)
{

	int nreuse = 1;	
	struct linger ling;

	/* 모든 버퍼를 버리고 즉시 종료 */
	ling.l_onoff =1;
	ling.l_linger = 0;

	if ( szServerIP == NULL || ServerPort == 0 )
	{
		return FALSE;
	}

	// init version...
	if (WSAStartup( MAKEWORD( 2, 2 ), &m_WsaData ) != 0)
	{
		CString strMsg;
		strMsg.Format(L"Error TCP WSAStartup");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );
		return FALSE;
	}

	// create socket..
	m_ServSock = socket( PF_INET, SOCK_STREAM, 0 );
	if ( m_ServSock == INVALID_SOCKET )
	{
		CString strMsg2;
		strMsg2.Format(L"ERROR SERVER SOCKET");
		strMsg2.ReleaseBuffer();
		DumpStr( strMsg2 );
		return FALSE;
	}

	// bind...
	memset( &m_ServAddr, 0, sizeof( m_ServAddr));
	m_ServAddr.sin_family		= AF_INET;

#if 1
	m_ServAddr.sin_addr.s_addr	= inet_addr( szServerIP );
#else
	m_ServAddr.sin_addr.s_addr	= htonl( INADDR_ANY );
#endif

	m_ServAddr.sin_port			= htons( ServerPort );

	// Option
	setsockopt( m_ServSock, SOL_SOCKET ,SO_REUSEADDR,  (char*)&nreuse, sizeof(nreuse) );
	setsockopt( m_ServSock, SOL_SOCKET ,SO_LINGER,  (char*)&ling, sizeof(struct linger) );
	
	if ( bind( m_ServSock, (SOCKADDR *)&m_ServAddr, sizeof(m_ServAddr)) == SOCKET_ERROR )
	{
		CString strMsg;
		strMsg.Format(L"Error SERVER Bind");
		strMsg.ReleaseBuffer();
		shutdown( m_ServSock, SD_BOTH );
		closesocket( m_ServSock );
		DumpStr( strMsg );
		return FALSE;
	}

	
	// listen...
	if ( listen( m_ServSock, 50 ) == SOCKET_ERROR )
	{
		CString strMsg2;
		shutdown( m_ServSock, SD_BOTH );
		closesocket( m_ServSock );
		strMsg2.Format(L"Error SERVER Listen ...");
		strMsg2.ReleaseBuffer();
		DumpStr( strMsg2 );
		return FALSE;
	}

	return TRUE;
}

DWORD CTCPServer::Thread_Accepter( LPVOID pParam)
{
	SOCKADDR_IN ClientAddr;		
	SOCKET	ClientScok;
	int ClientAddrSize;
	struct linger ling;
	tcp_keepalive KeepAlive;
	DWORD dwKeep =0;
	CString strMsg;
	TCHAR strIP[ MAX_IP_SIZE + 2 ];
	int optval;
	int optlen = sizeof(optval);

	/* 모든 버퍼를 버리고 즉시 종료 */
	ling.l_onoff =1;
	ling.l_linger = 0;

	KeepAlive.onoff				= 1;						/*Use keepalive */
	KeepAlive.keepalivetime		= ( 30 * 1000 );			/*60sec */
	KeepAlive.keepaliveinterval = 2000;						/*5sec*/

	CTCPServer * pThis = ( CTCPServer *)pParam;

	while ( pThis->m_bConnected  && pThis->m_ServSock != NULL )
	{		
		ClientAddrSize = sizeof( ClientAddr );
		ClientScok = accept( pThis->m_ServSock, (SOCKADDR *)&ClientAddr, &ClientAddrSize );		

 		if ( ClientScok == INVALID_SOCKET )
		{
			
			memset( strIP, 0, sizeof( strIP ));
			pThis->m_Util.McsToWcs( inet_ntoa( ClientAddr.sin_addr ), strIP, strlen( inet_ntoa( ClientAddr.sin_addr ) ));

			strMsg.Format(L"Accept Error IP:%s ", strIP );
			strMsg.ReleaseBuffer();
		}
		else
		{			
			/* Option */
			setsockopt( ClientScok, SOL_SOCKET ,SO_LINGER,  (char*)&ling, sizeof(struct linger) );
#if 1
			/*KeepAlive Time Change*/
			setsockopt(ClientScok, SOL_SOCKET, SO_KEEPALIVE, (char*)&optval, optlen);
			if ( WSAIoctl( ClientScok, SIO_KEEPALIVE_VALS, &KeepAlive, sizeof(KeepAlive),0, 0, &dwKeep, NULL, NULL ) != 0 )
			{
				shutdown( ClientScok, SD_BOTH );
				closesocket( ClientScok );
			}
#endif			
			int rect = getsockopt(ClientScok, SOL_SOCKET, SO_KEEPALIVE, (char*)&optval, &optlen);
			if ( rect != SOCKET_ERROR )
			{
				strMsg.Format(L"KeepAlive Set On %s", ( optval ? L"ON " : L"OFF" ));
				pThis->DumpStr( strMsg );
			}
			pThis->m_TCPClientManager.AddTCPClient( ClientScok , ClientAddr );
		}
	}

	return 0L ;
}

/* =====================================================================
Export Functions 
=======================================================================*/
BOOL CTCPServer::TCPServerOpen( char * szServerIP, UINT16 ServerPort )
{
	CString strMsg;

	if ( m_bConnected )
		return TRUE;

	InitSocket();

	if ( SetTCPServerListen( szServerIP, ServerPort ) != TRUE )
	{		
		return FALSE;
	}

	m_bConnected = TRUE;

	if (m_dwAccepter == 0) 
	{	
		m_hTh_Accepter = CreateThread (NULL, 0, Thread_Accepter, this, 0, &m_dwAccepter);
	}

	if ( m_hTh_Accepter == INVALID_HANDLE_VALUE)
	{
		TCPServerClose();
		return FALSE;
	}

	SendSocketState( DUMMY_MSG_CONNECT_TCP );
	strMsg.Format(L"Success TCPServerOpen");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );
	return TRUE;

}
void CTCPServer::TCPServerClose( void )
{
	CString strMsg;
	

	if ( m_bConnected == FALSE)
		return ;

	SendSocketState( DUMMY_MSG_CLOSE_TCP );

	m_TCPClientManager.Release();

	m_bConnected	= FALSE;
	if ( m_dwAccepter != 0 )
	{
		m_dwAccepter	= 0;
		::WaitForSingleObject(m_hTh_Accepter, 100 );
		CloseHandle(m_hTh_Accepter);
		m_hTh_Accepter	= NULL;		
	}

	if ( m_ServSock != NULL )
	{	
		shutdown( m_ServSock, SD_BOTH );
		closesocket( m_ServSock );
		WSACleanup();
		InitSocket();		
	}	

	strMsg.Format(L"TCPServer Close");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );
}

int CTCPServer::SendTCPManager( ClientSendMsg_t *ClientSendMsg )
{
	MsgInfo_t MsgInfo;	
	int Rect = -1;

	memset( &MsgInfo, 0, sizeof(MsgInfo_t));
	
	/* COMMAND 요청 내용을 VST_NETWORK_LOG에 등록 */
	MsgInfo.NonBlock = NONBLOCK;
	MsgInfo.SendMsgType = SEND_VS_SEND_MSG;
	MsgInfo.ptrMsg = ClientSendMsg;	
	pTCPServer->m_Controller->SendMsg( TARGET_DBMANAGER, &MsgInfo);

	/* TCP Client에 전송 */
	Rect = pTCPServer->m_TCPClientManager.SendTCPClient( ClientSendMsg );

	return Rect;
}

int CTCPServer::DummySendTCPManager( DummyMsg_t *DummyMsg )
{	
	(void)DummyMsg;

	return 0;
}

int CTCPServer::SendSocketState( BYTE State )
{

	DummyMsg_t DummyMsg;

	memset( &DummyMsg, 0, sizeof( DummyMsg_t ));
	DummyMsg.Msg = State;
	DummyMsg.SysCode = SYS_SEVER;
	DummyMsg.ClientID = m_ServerID;
	DummyMsg.Postion = 0;

	return SendController( TARGET_DBMANAGER ,SEND_DUMMY_MSG, &DummyMsg );
}

/* =====================================================================
Export Functions 
=======================================================================*/

int CTCPServer::Initialize( void )
{
	CString strMsg;

	m_TCPClientManager.Initialize();

	m_MsgController.AddClientSendFunctions( MSG_NETWORK_ALL_BYPASS, SendTCPManager );

	strMsg.Format(L"Init CTCPServer");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );
	
	return 0;
}

BOOL CTCPServer::SetTinyWindow( ITinyWindow * TinyWindow )
{
	m_TinyWindow= TinyWindow;
	m_TCPClientManager.SetTinyWindow( TinyWindow );

	return 0;
}

int CTCPServer::Open( TransferConf_t * TransferConf )
{	
	int Rect;
	
	/* TCP Server Open을 호출하기 전 Server Configure 값이 설정 되어 있어야 함 */
	
	Rect = TCPServerOpen( TransferConf->szServerIP, TransferConf->ServerPort );
	
	return Rect;
}

int CTCPServer::Close( void )
{
	TCPServerClose();
	return 0;
}

int CTCPServer::Release( void )
{
	CString strMsg;

	m_TinyWindow = NULL;
	m_MsgController.ReleaseFuncstions();

	TCPServerClose();
	strMsg.Format(L"Release CTCPServer");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );

	return 0;
}

int CTCPServer::Idle( void )
{
	m_TCPClientManager.Idle();

#if 0
	CString strMsg;
	strMsg.Format(L"Idle CTCPServer");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );
#endif

	return 0;
}

BOOL CTCPServer::SetControllerHandler( void *  ControllerHandle )
{
	m_Controller = (IController *)ControllerHandle;
	m_TCPClientManager.SetControllerHandler( m_Controller );

	if ( m_Controller != NULL )
	{
		m_ServerID = m_Controller->GetMyServerID();
	}

	return TRUE;
}

int CTCPServer::OnRecvMsg( MsgInfo_t * MsgInfo )
{
	return m_MsgController.OnRecvMsg( MsgInfo );
}

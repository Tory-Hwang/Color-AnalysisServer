#include "StdAfx.h"

#include "Com_Def.h"
#include "TCPClient.h"
#include "TCPClientManager.h"
#include "QueryProcessor.h"


// #define CONVERT_TO_JPG		1

#define DONE_PACKET		0x10

/*	=======================================================================================================
	m_MySocket			: TCP 통신에 사용될 Socket
	m_TinyWindow		: TCP 통신을 통해 전달받은 팻킷을 Controller에 보낼 Window( Window Queue 사용 목적 )
	m_TCPClientManager	: 자신을 포함한 여러 Client를 저장하는 객체
	m_MyPosition		: m_TCPClientManager에 저장되어 있는 자신의 위치
	m_MySIPID			: 원격 Client가 보유한 ID( Vision Sensor ID ) 또는 기타 원격 Client 의 ID
	m_LastRecvMsgTime	: 마지막 Hot bit을 받은 시간 
	m_hThread			: TCP Read을 위한 Thread Handle
	m_dwThread			: 생성된 Thread ID
 ========================================================================================================== */

CTCPClient::CTCPClient(void)
:m_MySocket( NULL ),
m_TinyWindow( NULL ),
m_TCPClientManager( NULL ),
m_MyPosition( 0 ),
m_MySIPID( 0 ),
m_hThread( NULL ),
m_dwThread( 0 ),
m_isFirstPacket( FALSE ),
m_Queue ( NULL ), 
m_ImageQueue( NULL ),
m_Packet( NULL),	
m_DummyBuf( NULL),
m_hIdleThread( NULL),
m_dwIdleThread( 0 ),
m_CloseSocketTime ( 0 )
{
	memset( &m_MyClientDir, 0, sizeof( ClientDir_t));
	m_MyClientDir.Cur_Hour = -1;
	g_pre_time = 0;
}

CTCPClient::~CTCPClient(void)
{
	CString strMsg;

	Release();

	strMsg.Format(L"Destroy TCP Client myPosition( %d ), myClientID( %d )", m_MyPosition, m_MySIPID );
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );
}

void CTCPClient::DumpStr( CString &str )
{
#if DEBUG_TCPCLIENT
	CString strtxt;
	strtxt.Format(L"VS_SERVER::CTCPClient : %s \n", str);
	strtxt.ReleaseBuffer();
	OutputDebugString( strtxt );
#endif
}

/* =====================================================================
Internal Functions 
=======================================================================*/
void CTCPClient::SendTinyWindowMsg( ClientRecvMsg_t * ClientRecvMsg )
{	
#if 0
	HWND hWnd = NULL;
	ClientRecvMsg_t * pTemp;

		/* TinyWindow를 통해 Network Message를 넘긴다 */
		if ( m_TinyWindow != NULL )
		{
			hWnd = m_TinyWindow->GetHWND();
			if ( ::IsWindow( hWnd ) )
			{
				/* malloc에 대한 free는 Controller 에서 처리 함 */
				pTemp = (ClientRecvMsg_t *)malloc( sizeof( ClientRecvMsg_t ));
				memset( pTemp, 0, sizeof( ClientRecvMsg_t ));
				memcpy( pTemp, ClientRecvMsg, sizeof( ClientRecvMsg_t ));
				::PostMessage( hWnd, WM_TRANSFER_MSG,(WPARAM)sizeof( ClientRecvMsg_t ), (LPARAM)pTemp);
			}
		}	
#else
	
	if ( m_TCPClientManager != NULL )
	{
		m_TCPClientManager->byPassExigentNetworkMsg( ClientRecvMsg );
	}	
#endif
}

int CTCPClient::MySelfKIll( SOCKET TempSock )
{
	( void )TempSock;
	DummyMsg_t DummyMsg;

	m_bRuned= FALSE;
	memset( &DummyMsg, 0, sizeof( DummyMsg_t ));
	DummyMsg.Msg = DUMMY_MSG_CLOSE_TCP;
	DummyMsg.SysCode = SYS_VISION;
	DummyMsg.ClientID = m_MySIPID;
	DummyMsg.ptrEtcInfo = this;			

	return SendTCPClientManager( SEND_DUMMY_MSG ,&DummyMsg );
}



BOOL CTCPClient::CreateThrHandle( void )
{
	if ( m_dwThread == 0) 
	{	
		m_hThread = CreateThread (NULL, 0, ThreadRead, this, 0, &m_dwThread);
	}

	if ( m_hThread == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	return TRUE;
}

/* Client의 상태 정보를 TCPClient Manager로 알림 */
int CTCPClient::SendTCPClientManager( short Msg ,void * ptr )
{
	MsgInfo_t MsgInfo;

	memset( &MsgInfo, 0, sizeof( MsgInfo_t ));
	MsgInfo.SendMsgType = Msg;
	
	if ( ptr !=NULL )
	{
		MsgInfo.ptrMsg = ptr;
	}
	
	if ( m_TCPClientManager != NULL )
	{
		return m_TCPClientManager->OnRecvTCPClient( &MsgInfo );
	}

	return -1;

}

DWORD CTCPClient::ThreadRead(LPVOID pParam)
{
	int RecvSize =0;
	CTCPClient * pThis = ( CTCPClient *)pParam;
	//char RecvBuf[ MAX_RECV_BUF_SIZE  ];
	char * RecvBuf;

	int Len = 0;	
	BYTE * Packet;		
	CString strMsg;

	RecvBuf = new char [MAX_RECV_BUF_SIZE  ];

	while( pThis->m_MySocket != NULL && pThis->m_bRuned == TRUE )
	{
		memset( RecvBuf, 0, MAX_RECV_BUF_SIZE);		
		
		RecvSize = recv( pThis->m_MySocket , RecvBuf, MAX_RECV_BUF_SIZE, 0 );
#if 0
		//if ( RecvSize > 200 )
		{
			TRACE( L"\r\nRecv(%d)=============================\r\n", RecvSize );
			for( int i = 0 ; i < RecvSize ; i++ )
			{
				TRACE( L"0x%02x ",  RecvBuf[ i ] & 0xFF );
				if ( i != 0 && i % 16 == 0 )
					TRACE( L"\r\n");
			}

			TRACE( L"\r\n==============================\r\n");

		}
#endif
		
		if ( RecvSize <= 0 )
		{	
			strMsg.Format(L"Recv Size( %d ) id( %d )",RecvSize, pThis->m_MySIPID );
			strMsg.ReleaseBuffer();
			pThis->DumpStr( strMsg );
			pThis->m_bRuned = FALSE;
			break;
		}
		else
		{	
			/* 받은 팻킷을 Queue에 적재 */
			pThis->PutQueue( RecvSize, (BYTE*)RecvBuf );
			
			/* 하나의 팻킷을 받았다면 */
			Packet = pThis->PopQueue( &Len );		

			while( Packet != NULL )
			{
				/* 팻킷을 파싱하고 */
				if ( pThis->ValifyPacket( Len, Packet ) == FALSE )
				{
					pThis->m_bRuned = FALSE;
					strMsg.Format(L"ParsePacket Err id( %d )", pThis->m_MySIPID );
					strMsg.ReleaseBuffer();
					pThis->DumpStr( strMsg );
					break;
				}

				/* 다른 팻킷이 있는지 확인하고 */
				Packet = pThis->PopQueue( &Len );
			}
		}
	}

	if ( pThis->m_MySocket != NULL )
	{
		shutdown( pThis->m_MySocket, SD_BOTH );
		closesocket( pThis->m_MySocket );
		pThis->m_MySocket = NULL;		
	}
#if 1
	if ( pThis->m_dwThread != 0 )
	{
		pThis->m_dwThread	= 0;
		::WaitForSingleObject( pThis->m_hThread, 150);
		CloseHandle( pThis->m_hThread );
		pThis->m_hThread	= NULL;		
	}
#endif

	pThis->m_bRuned= FALSE;

// Not used 
#if 1
	if ( pThis->MySelfKIll( 0 ) != 0 )
	{

		pThis->m_CloseSocketTime = time( NULL );
		if ( pThis->m_dwIdleThread == 0 )
		{
			pThis->m_hIdleThread = CreateThread (NULL, 0, ThreadIdle, pThis, 0, &pThis->m_dwIdleThread );
		}
	}
#endif

	if ( RecvBuf != NULL )
	{
		delete [] RecvBuf;
	}
	return 0L ;
}


DWORD CTCPClient::ThreadIdle(LPVOID pParam)
{
	CTCPClient * pThis = ( CTCPClient *)pParam;

	CString strMsg;
	time_t Cur_Time;
	time_t Last_Time;		
	time_t inter_time;
	BOOL bTimeOut = FALSE;

	while( 1 == 1 )
	{		
		Cur_Time	= time( NULL );
		Last_Time	= pThis->m_CloseSocketTime;
		inter_time	= Cur_Time - Last_Time;
		
		//if ( Last_Time != 0 && pThis->m_MySIPID != 0 && inter_time > MAX_RECV_MESSAGE_INTERVAL  )
		if ( Last_Time != 0 && inter_time > MAX_RECV_MESSAGE_INTERVAL  )
		{	
			bTimeOut = TRUE;
			strMsg.Format(L"ClientID( %d ) InterTime ( %d )...MSG_RECV_TIMEOUT..pThis ( %x )",  pThis->m_MySIPID , inter_time , pThis );
			strMsg.ReleaseBuffer();
			 pThis->DumpStr( strMsg );
			break;
		}
		strMsg.Format(L"Last_Time( %d ), ClientID( %d ), InterTime( %d ) ", Last_Time, pThis->m_MySIPID,inter_time );
		pThis->DumpStr( strMsg );
		Sleep( 1000 );
	}
#if 0
	if ( pThis->m_dwIdleThread != 0 )
	{
		pThis->m_dwIdleThread	= 0;
		::WaitForSingleObject( pThis->m_hIdleThread, 150);
		CloseHandle( pThis->m_hIdleThread );
		pThis->m_hThread	= NULL;		
	}
#endif
	if (  bTimeOut )
	{
		pThis->MySelfKIll( 0 );
	}
		
	return 0L;
}

int CTCPClient::PutQueue( int Size, BYTE * Buf )
{
	int T_Size = 0;
	
	if ( m_Queue == NULL )
		return 0;

	T_Size = m_Queue->Tail + Size;
	
	/* 최대로 받을 수 있는 팻킷 버퍼를 10개가 다 찼는지 */
	if ( T_Size >= ( MAX_RECV_BUF_SIZE  * 3 ) )
	{
		CString strMsg;
		strMsg.Format(L"Overflow Queue Size( %d )", T_Size );
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );
		memset( m_Queue, 0, sizeof( Queue_t ));
		return -1;
	}

	if ( Size > 0 )
	{
		memcpy( &m_Queue->Buf[ m_Queue->Tail ], Buf, Size );
	}
	m_Queue->Tail +=Size;

	return 0;
}


BYTE * CTCPClient::PopQueue(int *Size )
{
	unsigned short  HeadSize ;
	unsigned short PacketSize;
	int QueueSize, TotalSize;

	TotalSize =0;
	HeadSize = sizeof( Head_t);
	
	if ( m_Queue == NULL || m_Packet == NULL )
	{
		*Size = 0;
		return NULL;
	}

	QueueSize = m_Queue->Tail - m_Queue->Head;

	if ( QueueSize > 0 && ( QueueSize >= HeadSize ))
	{
		memcpy(&PacketSize, &m_Queue->Buf[ PACKET_SIZE_POS ], 2 );		
		TotalSize = PacketSize + HeadSize;

		if ( QueueSize >= TotalSize )
		{
			memset( m_Packet, 0,  MAX_RECV_BUF_SIZE );
			memcpy( m_Packet, m_Queue->Buf, TotalSize );

			if ( QueueSize == TotalSize )
			{
				memset( m_Queue, 0, sizeof( Queue_t ));
			}
			else
			{
				memmove( &m_Queue->Buf[ m_Queue->Head ] , &m_Queue->Buf[ TotalSize ], ( QueueSize - TotalSize ));
				m_Queue->Tail -= TotalSize;
			}
			*Size = TotalSize;
			return m_Packet;
		}
	}

	*Size = 0;
	return NULL;
}

#define MAX_DIR_NAME_SIZE 2



BOOL CTCPClient::ValifyPacket( int Size, BYTE * Buf )
{
	
	memset(&m_RecvMsg, 0, sizeof( ClientRecvMsg_t ));
	memset(m_DummyBuf, 0, ONE_IMAGE_PACKET_SIZE );

	if ( m_DummyBuf == NULL )
		return TRUE;
	
	m_RecvMsg.DummyBuf = m_DummyBuf;

	if ( m_Parser.ParsePacket( NO_CRC, Size, Buf, &m_RecvMsg ) == TRUE )
	{
		//m_MySIPID			= 1;
		m_RecvMsg.SIPID =   m_MySIPID;
		wcscpy_s( m_RecvMsg.ClientIP, wcslen( m_MyIP) + 1, m_MyIP  );
		wcscpy_s( m_RecvMsg.ClientMAC, wcslen( m_MyMAC) + 1, m_MyMAC );

		if ( m_RecvMsg.Msg == SOCK_SEND_IMAG_MSG )
		{	
			m_isFirstPacket			= 1;
		}

		SendTinyWindowMsg( &m_RecvMsg );		
	}

	return TRUE;
}

/* =====================================================================
Export Functions 
=======================================================================*/

int CTCPClient::Initialize( int Postion, SOCKET &ClientSock )
{
	CString strMsg;
		
	m_BmpBufSize =0;
	m_MySocket = ClientSock;
	m_MyPosition = Postion;
	m_MySIPID = Postion;

	m_bRuned = TRUE;
	
	/* Queue 초기화 */
	m_Queue = ( Queue_t * )malloc( sizeof( Queue_t ));
	m_ImageQueue = ( ImageQueue_t * )malloc( sizeof( ImageQueue_t ));
	m_Packet = ( BYTE *)malloc( sizeof( BYTE ) * MAX_RECV_BUF_SIZE );	
	m_DummyBuf=( BYTE *)malloc( sizeof( BYTE ) * ONE_IMAGE_PACKET_SIZE);		

	memset( m_Queue, 0, sizeof(Queue_t));
	memset( m_ImageQueue, 0 ,sizeof( ImageQueue_t ));
	memset( &m_LastCarInfo, 0, sizeof( LastCarInfo_t ))	;

	strMsg.Format(L"Init CTCP_Client Position( %d )", Postion);
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );

	return 0;
}

int CTCPClient::Release( void )
{
	CString strMsg;

	m_bRuned = FALSE;	

	m_TinyWindow = NULL;
	m_TCPClientManager = NULL;

	if ( m_MySocket != NULL )
	{
		shutdown( m_MySocket, SD_BOTH );
		closesocket( m_MySocket );
		m_MySocket = NULL;
	}

	if ( m_dwIdleThread != 0 )
	{
		m_dwIdleThread	= 0;
		::WaitForSingleObject( m_hIdleThread, 50);
		CloseHandle( m_hIdleThread );
		m_hIdleThread	= NULL;
	}

	if ( m_dwThread != 0 )
	{
		m_dwThread	= 0;
		::WaitForSingleObject( m_hThread, 50);
		CloseHandle( m_hThread );
		m_hThread	= NULL;		
	}

	
		
	if ( m_Queue != NULL )
	{
		free( m_Queue );
		m_Queue = NULL;
	}
	
	if ( m_ImageQueue != NULL )
	{
		free( m_ImageQueue );
		m_ImageQueue = NULL;
	}
	
	if ( m_Packet != NULL )
	{
		free( m_Packet);
		m_Packet = NULL;
	}
	if ( m_DummyBuf != NULL )
	{
		free( m_DummyBuf );
		m_DummyBuf = NULL;
	}

	if ( m_BmpBuf != NULL )
	{
		free ( m_BmpBuf );
		m_BmpBuf = NULL;
	}
	
	strMsg.Format(L"Release CTCPClient ClientID( %d )", m_MySIPID );
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );

	return 0;
}

#define INTERVAL_SEND_HEART_BEAT ( 60 * 30 )
int CTCPClient::Idle( void )
{
	time_t cur_time;
	int check_time = 0;

	if ( m_isFirstPacket == FALSE && m_MySocket != NULL && m_bRuned == TRUE)
	{
		//SendBaseinfoPacket();
	}
	check_time = m_stSvrInfo.nForceUpdateTime;
	if ( check_time == 0 )
		check_time = INTERVAL_SEND_HEART_BEAT;

	cur_time = time( NULL );

	if ( g_pre_time == 0 )
	{
		g_pre_time = cur_time;
		return 0;
	}

	if ( cur_time - g_pre_time >= ( check_time ))
	{
		//SendBaseinfoPacket();
		g_pre_time = cur_time;
	}

	return 0;
}

void CTCPClient::SendBaseinfoPacket( void )
{

	ClientSendMsg_t ClientSendMsg;		
	
	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));

	ClientSendMsg.SIPID = m_MySIPID;

	if ( m_Message.MakeReqHeartBeat(MSG_VS_REQ_HB, m_MySIPID , &ClientSendMsg) == TRUE )
	{		
		SendMsg( &ClientSendMsg );		
	}

	return;
}

int CTCPClient::Close( void )
{
	if ( m_MySocket != NULL )
	{
		shutdown( m_MySocket, SD_BOTH );
		closesocket( m_MySocket );
		m_MySocket = NULL;
	}
	return 0;
}

int CTCPClient::GetMySIPID( void )
{
	return m_MySIPID;
}
int CTCPClient::GetMyPostion( void )
{
	return m_MyPosition;
}
int CTCPClient::SetRePostion( int Postion )
{
	m_MyPosition = Postion;
	return 0;
}

BOOL CTCPClient::SetClientManagerHandler( void * TCP_Client_Manager )
{
	m_TCPClientManager = ( CTCP_ClientManager *)TCP_Client_Manager;
	return TRUE;
}

BOOL CTCPClient::SetTinyWindow( ITinyWindow * pTinyWindo )
{
	m_TinyWindow = pTinyWindo;
	return CreateThrHandle();
}


int CTCPClient::SendMsg( ClientSendMsg_t * ClientSendMsg )
{	
	int Ret = -1;
	BYTE *ptr;
	int SendSize =0;
	int nMsg;

	ptr = NULL;
	nMsg = ClientSendMsg->Msg & 0xFFFF;
	
	if ( nMsg != SOCK_TEST_STRIP_RESULT_MSG )
	{
		if ( m_MySocket == NULL || ClientSendMsg == NULL || ClientSendMsg->SIPID != m_MySIPID )
		{
			CString strMsg;
			strMsg.Format(L"Can not Send Buf TargetID( %d )",m_MySIPID );
			strMsg.ReleaseBuffer();
			return -1;
		}
	}
	ptr = ClientSendMsg->SendBuf;
	SendSize = ClientSendMsg->Size;

	Ret = send( m_MySocket, ( char *)ptr, SendSize, 0);	

#if 0
	TRACE( L"\r\nSend(%d)=============================\r\n", SendSize);
	for( int i = 0 ; i < SendSize ; i++ )
	{
		TRACE( L"0x%02x ",  ptr[ i ] & 0xFF );
		if ( i != 0 && i % 16 == 0 )
			TRACE( L"\r\n");
	}

	TRACE( L"\r\n==============================\r\n");
#endif

	if ( SendSize == Ret )
		return Ret;

	return -1;
}

int CTCPClient::SetSerConf( void * ptrServerConf )
{
	if ( ptrServerConf == NULL )
		return -1;

	memset( &m_stSvrInfo, 0, sizeof( VSSERVERINFO_t));
	memcpy( &m_stSvrInfo, ptrServerConf,sizeof( VSSERVERINFO_t) );
	
	m_BmpBufSize = ( m_stSvrInfo.nImageWidth * m_stSvrInfo.nImageHeight ) + sizeof(BITMAPFILEHEADER) + ( sizeof(RGBQUAD) * 256 ) + sizeof(BITMAPINFOHEADER) + 1 ;
	m_BmpBuf = ( BYTE *)malloc( sizeof( BYTE ) * m_BmpBufSize );

	return 0;
}

int CTCPClient::SetMyIP( TCHAR * csIP , TCHAR * csMAC )
{
	ClientRecvMsg_t Client_Msg;

	memset( m_MyIP, 0, sizeof( m_MyIP ));
	memset( m_MyMAC, 0, sizeof( m_MyMAC ));

	wcscpy_s( m_MyIP, wcslen( csIP) + 1, csIP  );
	wcscpy_s( m_MyMAC, wcslen( csMAC) + 1, csMAC  );
	
	wcscpy_s( Client_Msg.ClientIP, wcslen( m_MyIP) + 1, m_MyIP  );
	wcscpy_s( Client_Msg.ClientMAC, wcslen( m_MyMAC) + 1, m_MyMAC  );
	Client_Msg.SIPID= m_MySIPID;
	Client_Msg.IsFirstMsg = 1;
	Client_Msg.Msg = SOCK_ADD_MSG;

	SendTinyWindowMsg(&Client_Msg);

	return 0;
}
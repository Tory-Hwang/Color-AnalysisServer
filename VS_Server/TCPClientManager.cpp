#include "StdAfx.h"
#include "TCPClientManager.h"
#include "TCPClient.h"
#include "SVR_Doc.h"
#include "iphlpapi.h"   //For SendARP 
#include "conio.h" 
#pragma comment(lib , "iphlpapi.lib") //For iphlpapi 
#pragma comment(lib , "ws2_32.lib") //For winsock 

/* Multi Thread간 동기화 문제 해결 */
static CRITICAL_SECTION _ClientManagerLock = { 0 };


CTCP_ClientManager * pTCPManager = NULL;


CTCP_ClientManager::CTCP_ClientManager(void)
:m_TinyWindow( NULL ),
m_Controller( NULL ),
m_DBManager( NULL ),
m_bReleasing( FALSE )
{
	InitializeCriticalSection(&_ClientManagerLock);
	pTCPManager = this;
}

CTCP_ClientManager::~CTCP_ClientManager(void)
{
	DeleteCriticalSection(&_ClientManagerLock);
	Release();
}

/* =====================================================================
Internal Functions 
=======================================================================*/

void CTCP_ClientManager::DumpStr( CString &str )
{
#if DEBUG_TCP_CLIENT_MANAGER
	CString strtxt;
	strtxt.Format(L"VS_SERVER::CTCP_ClientManager : %s \n", str);
	strtxt.ReleaseBuffer();
	OutputDebugString( strtxt );
#endif
}

void CTCP_ClientManager::GetManagerIP( void )
{
	CString strMsg;
	CString curLine;
	CStdioFile sF;
	int i =0;
	TCHAR strBlank[ MAX_PATH ];	
	TCHAR strIP[ MAX_IP_SIZE + 2 ];
	TCHAR strFullPaht[ MAX_PATH ];	

	
	memset( strBlank, 0, sizeof( strBlank) );
	memset( strFullPaht, 0, sizeof( strFullPaht) );

	if (GetModuleFileName( AfxGetInstanceHandle( ), strBlank, sizeof( strBlank) ) == 0 )
	{
		return ;
	}		

	strMsg.Format(L"%s", strBlank );
	strMsg.ReleaseBuffer();
	strMsg = strMsg.Left(strMsg.ReverseFind('\\'));

	wsprintf( strFullPaht, L"%s\\ManagerIP.txt", strMsg );

	if (sF.Open(strFullPaht, CFile::modeRead | CFile::typeText) == FALSE)
	{
		return ;
	}

	while(sF.ReadString( curLine ))
	{		
		curLine.TrimRight();

		memset( strIP, 0, sizeof( strIP ) );
		
		wsprintf(strIP, L"%s", curLine);

		if ( wcslen( strIP ) > 5 )
		{
			m_Util.WcsToMcs( strIP, m_MultiAcceptIP[ i ].szIP , wcslen( strIP ));
			i++;
		}
	}

	sF.Close();
}

BOOL CTCP_ClientManager::IsManagerIP( char *ClientIP )
{
	int i;
	char * ptrIP;

	for( i = 0 ; i < MAX_MANAGER_IP_CNT ; i++ )
	{
		ptrIP = (char*)&m_MultiAcceptIP[ i ].szIP;
		if ( strlen( ptrIP ) == 0 )
		{
			break;
		}
		else
		{
			if ( strcmp( ptrIP, ClientIP) == 0 )
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

int CTCP_ClientManager::SendController( BYTE TagetObject, short Msg, void * ptr )
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

int CTCP_ClientManager::Lock()
{
	EnterCriticalSection(&_ClientManagerLock);
	return 0;
}

int CTCP_ClientManager::UnLock()
{
	LeaveCriticalSection(&_ClientManagerLock);
	return 0;
}	

int CTCP_ClientManager::GetClientCnt( void )
{
	int TmpCnt =0;

	Lock();
	TmpCnt = m_ClientList.Cnt;
	UnLock();

	return TmpCnt;
}

int CTCP_ClientManager::SetClientCnt(int Cnt )
{
	Lock();
	m_ClientList.Cnt = Cnt ;
	UnLock();

	return 0;
}

/*     Get the mac address of a given ip */ 
int  CTCP_ClientManager::GetMacAddress(unsigned char *mac , struct in_addr destip)
{
	DWORD ret;
	IPAddr srcip;
	ULONG MacAddr[2];
	ULONG PhyAddrLen = 6;  /* default to length of six bytes */
	int i, bMac;
	BYTE *bMacAddr = NULL;
	
	memset( MacAddr, 0, sizeof( MacAddr));
	srcip = 0;
	bMac = 0;

	
	ret = SendARP((IPAddr) destip.S_un.S_addr , srcip , MacAddr , &PhyAddrLen);
	for( i = 0 ; i < 5 ; i++ )
	{
		Sleep( 1000 );
		if ( MacAddr[ 0 ] != 0 || MacAddr[ 1 ] != 0 )
		{
			bMac =1;
			break;			
		}
	}
		
	if(bMac)
	{
		bMacAddr = (BYTE *) & MacAddr;
		for (i = 0; i < (int) PhyAddrLen; i++)         
		{
			mac[i] = (char)bMacAddr[i];
		}
	} 
	return bMac;
}

/* TCP Client Object 생성 */
BOOL CTCP_ClientManager::CreateTCPClient( SOCKET &ClientSock, SOCKADDR_IN &ClientAddr )
{
	BOOL bResult = FALSE;
	CString strMsg;
	int i =0;
	IClient *list_Client, * ptrClient= NULL;
	SOCKET List_Sock = NULL;
	char * list_IP, * szClientIP = NULL;
	TCHAR strIP[ MAX_IP_SIZE + 2 ];
	TCHAR strMAC[ MAX_MAC_SIZE ];

	unsigned char mac[6];     
	struct in_addr srcip;     
			
	static int Position = 0;

	szClientIP = inet_ntoa( ClientAddr.sin_addr );
	memset( strIP, 0, sizeof( strIP ));
	memset( strMAC, 0, sizeof( strMAC ));

	m_Util.McsToWcs( szClientIP, strIP, strlen( szClientIP ));		

	if ( szClientIP == NULL )
	{
		shutdown( ClientSock, SD_BOTH );
		closesocket( ClientSock ); 
		strMsg.Format(L"Not have addr");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );		
		return FALSE;
	}

	memset( mac, 0,sizeof( mac ));
	srcip.s_addr = inet_addr(szClientIP); 
	if ( GetMacAddress(mac , srcip) )
	{
		//wprintf(strMAC, L"%02x%02x%02x%02x%02x%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5] );
		_stprintf_s(strMAC, _countof(strMAC), L"%02x:%02x:%02x:%02x:%02x:%02x", mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	}

	/*관리자 용으로 등록된 IP가 아닐경우 중복 접속 할 수 없음 */
	Lock();
	if ( IsManagerIP( szClientIP ) == FALSE )
	{
		for( i =0 ; i < ( MAX_TCP_CLIENT_CNT  ) ; i++ )
		{		
			if ( m_ClientList.Clients[ i ].ClientSock == NULL )
			{
				break;
			}
			else
			{
				list_IP = m_ClientList.Clients[ i ].szIP;

				if ( list_IP != NULL && strcmp( list_IP , szClientIP ) == 0 )
				{
					m_ClientList.Clients[ i ].ClientObj->Close();
					UnLock();

					shutdown( ClientSock, SD_BOTH );
					closesocket( ClientSock );
					memset( strIP, 0, sizeof( strIP ));
					m_Util.McsToWcs( szClientIP, strIP, strlen( szClientIP ));
					strMsg.Format(L"IP( %s )is Already existence so self killed : ", strIP );
					strMsg.ReleaseBuffer();
					DumpStr( strMsg );

					QueryMsg_t QueryMsg;
					VST_ERR_LOG ErrLog;

					memset( &QueryMsg, 0, sizeof( QueryMsg_t));
					memset( &ErrLog, 0, sizeof( VST_ERR_LOG));

					wcscpy_s( ErrLog.csErrCode, wcslen( ERR_SAME_IP )+1 , ERR_SAME_IP);
					wcscpy_s( ErrLog.csInfo, wcslen( strIP) + 1 , strIP);
					ErrLog.cSYSCode = SYS_VISION;
					ErrLog.nID = 0;
					ErrLog.nServerID = m_Controller->GetMyServerID();
					
					QueryMsg.Msg = QUERY_ERR_LOG_INSERT;
					QueryMsg.DataSize =sizeof( ErrLog );
					QueryMsg.ptrData = &ErrLog;					
					
					QueryProc( &QueryMsg );

					
					return FALSE;
				}
			}
		}
	}

	ptrClient = new CTCPClient();
	if( ptrClient == NULL)
	{
		shutdown( ClientSock, SD_BOTH );
		closesocket( ClientSock ); 
		strMsg.Format(L"Can not Create TCP Client");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );
		UnLock();
		return bResult;
	}
	
	/* Client Position은 m_ClientList의 +1의 위치다 */	
	Position++;

	ClientRecvMsg_t ClientRecvMsg;

	memset( &ClientRecvMsg, 0, sizeof( ClientRecvMsg ));

	for( i =0 ; i < ( MAX_TCP_CLIENT_CNT  ) ; i++ )
	{
		list_Client = NULL;
		List_Sock = m_ClientList.Clients[ i ].ClientSock;
		list_Client = m_ClientList.Clients[ i ].ClientObj;		

		if ( List_Sock == NULL && list_Client == NULL )
		{
			//Position = i + 1;
			ptrClient->Initialize( Position, ClientSock );
			ptrClient->SetClientManagerHandler( this );
			ptrClient->SetSerConf( &m_stSvrInfo );			
			ptrClient->SetMyIP( strIP, strMAC );

			if ( ptrClient->SetTinyWindow( m_TinyWindow ) == FALSE )
			{
				shutdown( ClientSock, SD_BOTH );
				closesocket( ClientSock ); 
				ptrClient->Release();
				delete  ptrClient;
			}
			else
			{
				
				m_ClientList.Clients[ i ].ClientObj = ptrClient;
				m_ClientList.Clients[ i ].ClientSock = ClientSock;
				memcpy( m_ClientList.Clients[ i ].szIP, szClientIP, strlen( szClientIP ));		

				m_ClientList.Cnt++;
				bResult = TRUE;

				strMsg.Format(L"Add TCP Client Result(%d), TotalCnt (%d ) ", bResult , m_ClientList.Cnt );
				strMsg.ReleaseBuffer();
				DumpStr( strMsg );
			}
			break;
		}
	}
	UnLock();
	return bResult;
}

/* 특정 TCP Client Object 삭제 */
int CTCP_ClientManager::DummyDeleteTCPClient( DummyMsg_t *DummyMsg )
{
	CString strMsg;
	int i, Cnt =0;
	IClient *DummyClient, * DelClient = NULL;
	int Pos =-1;
	int ClientID = 0;

	/*서버에서 종료 처리 시 Client 가 자체적으로 Release 않게 함 */
	if ( pTCPManager->m_bReleasing == TRUE )
		return 0;

	pTCPManager->Lock();

	DummyClient = NULL;
	DummyClient = (IClient *)DummyMsg->ptrEtcInfo;
	ClientID = DummyMsg->ClientID;

#if 0
	Cnt = pTCPManager->m_ClientList.Cnt;	
#endif

	for( i = 0 ; i < MAX_TCP_CLIENT_CNT ; i++  )
	{
		if ( pTCPManager->m_ClientList.Clients[ i ].ClientObj != NULL )
		{
			if ( DummyClient != NULL && ( DummyClient == pTCPManager->m_ClientList.Clients[ i ].ClientObj) )
			{
				Pos = i;			
			}
			Cnt++;
		}
		else
			break;		
	}

	strMsg.Format(L"DummyDeleteTCPClient :: ClientID( %d ), TotalClientCnt( %d ), Pos( %d ), DummyClient( %x )", ClientID, Cnt, Pos, DummyClient );
	pTCPManager->DumpStr( strMsg );		

	if ( Cnt == 0 || Pos < 0 )
	{
		/*
		for( i = 0 ; i < Cnt ; i++  )
		{
			if ( DummyClient != NULL && ( DummyClient == pTCPManager->m_ClientList.Clients[ i ].ClientObj) )
			{
				Pos = i;			
			}
		}
		*/
		
		if ( Cnt == 0 || Pos < 0 )
		{
			strMsg.Format(L"DummyDeleteTCPClient :: Not have Client ClientID( %d ), ClientCnt( %d ), Pos( %d ), DummyClient( %x )", ClientID, Cnt, Pos, DummyClient );
			pTCPManager->DumpStr( strMsg );		
			pTCPManager->UnLock();
			return -1;
		}
	}

	strMsg.Format(L"TotalCnt %d , Delete Pos %d ", Cnt , Pos );
	pTCPManager->DumpStr( strMsg );

	/* Client Position에서 -1 값이 m_ClientList의 Client 위치다 */	
	DelClient = pTCPManager->m_ClientList.Clients[ Pos ].ClientObj;	

	if ( DelClient != NULL )
	{	
		if ( Cnt == 1  )
		{
			memset( &pTCPManager->m_ClientList, 0, sizeof( ClientList_t ));
		}
		else
		{
			pTCPManager->m_ClientList.Clients[ Pos ].ClientObj = NULL;
			pTCPManager->m_ClientList.Clients[ Pos ].ClientSock = NULL;
			memset( pTCPManager->m_ClientList.Clients[ Pos ].szIP, 0, MAX_IP_SIZE + 2 );

			/*마지막 리스트는 이동 하지 않는다 */
			if ( Cnt !=  ( Pos + 1 ) )
			{
				memmove( &pTCPManager->m_ClientList.Clients[ Pos ] , &pTCPManager->m_ClientList.Clients[ Pos + 1 ], ( Cnt - ( Pos + 1 ))* sizeof( Clients_t ) );

				pTCPManager->m_ClientList.Clients[ Cnt -1 ].ClientObj = NULL;
				pTCPManager->m_ClientList.Clients[ Cnt -1 ].ClientSock = NULL;
				memset( pTCPManager->m_ClientList.Clients[ Cnt -1 ].szIP, 0, MAX_IP_SIZE + 2 );

			}

			pTCPManager->m_ClientList.Cnt = ( Cnt - 1 );
		}

		DelClient->Release();
		delete  DelClient;
		
	}
	else
	{
		return -1;
	}	
	
	pTCPManager->UnLock();

	pTCPManager->SendController( TARGET_DBMANAGER,SEND_DUMMY_MSG, DummyMsg );
	pTCPManager->SendController( TARGET_MAINVIEWER,SEND_DUMMY_MSG, DummyMsg);

	strMsg.Format(L"Delete TCP Client ID( %d ), TotalCnt( %d )", ClientID ,  Cnt - 1 );
	strMsg.ReleaseBuffer();
	pTCPManager->DumpStr( strMsg );	


	return 0;
}

int CTCP_ClientManager::DummySendFileManager( DummyMsg_t *DummyMsg)
{	
	int Ret=0;

	if ( DummyMsg == NULL )
	{
		return -1;
	}	

	if ( DummyMsg->Msg == DUMMY_MSG_MAKE_DIR )
	{
		//pTCPManager->Lock();
		Ret = pTCPManager->SendController( TARGET_FILEMANAGER, SEND_DUMMY_MSG, DummyMsg );	
		//pTCPManager->UnLock();
	}
	else
	{
		Ret = pTCPManager->SendController( TARGET_FILEMANAGER, SEND_DUMMY_MSG, DummyMsg );	
	}
	
	return Ret;
}

int CTCP_ClientManager::DummyClearTimeOut( DummyMsg_t *DummyMsg )
{
	int Ret=0;

	if ( DummyMsg == NULL )
	{
		return -1;
	}	

	Ret = pTCPManager->SendController( TARGET_DBMANAGER, SEND_DUMMY_MSG, DummyMsg );
	return Ret;
}

int CTCP_ClientManager::QueryProc( QueryMsg_t *QueryMsg )
{
	int Ret=0;

	if ( QueryMsg == NULL )
	{
		return -1;
	}	

	Ret = pTCPManager->SendController( TARGET_DBMANAGER, SEND_QUERY_MSG, QueryMsg );
	return Ret;
}

int CTCP_ClientManager::ErrMsgProc( ErrMsg_t * ErrMsg )
{	
	if ( ErrMsg == NULL )
	{
		return -1;
	}	

	pTCPManager->SendController( TARGET_DBMANAGER, SEND_ERR_MSG, ErrMsg );
	//pTCPManager->SendController( TARGET_MAINVIEWER, SEND_ERR_MSG, ErrMsg );
	
	return 0;
}

/* TCP Client에서 Postmessage로 보낼 수 없을 경우 대비 */
int CTCP_ClientManager::byPassExigentNetworkMsg( ClientRecvMsg_t * ClientRecvMsg )
{
	MsgInfo_t MsgInfo;

	MsgInfo.SendMsgType = SEND_VS_RECV_MSG;
	MsgInfo.ptrMsg = ClientRecvMsg;

#if 1
	if ( m_Controller != NULL )
	{
		m_Controller->SendMsg( TARGET_DBMANAGER , &MsgInfo );
	}
#endif

	if ( m_Controller != NULL )
	{
		m_Controller->SendMsg( TARGET_MAINVIEWER , &MsgInfo );
	}
	
	return 0;

}

BOOL CTCP_ClientManager::CreateDir( CString &strPath )
{
	BOOL bResult = FALSE;
	TCHAR sPath[ MAX_PATH ];

	memset( sPath, 0, sizeof( sPath ));
	Lock();
	
	wcscpy_s(sPath, strPath.GetLength() + 1 , strPath );
	bResult = m_Util.CreateDir( sPath );
	
	UnLock();

	return bResult;
}


/* =====================================================================
Export Functions 
=======================================================================*/

int CTCP_ClientManager::Initialize( void )
{
	CString strMsg;

	memset( &m_ClientList, 0, sizeof( ClientList_t ));
	memset( &m_MultiAcceptIP, 0, sizeof( m_MultiAcceptIP ));
	GetManagerIP();

	m_MsgController.AddDummyFunctions( DUMMY_MSG_MAKE_DIR , DummySendFileManager );
	m_MsgController.AddDummyFunctions( DUMMY_MSG_SAVE_FILE , DummySendFileManager );
	m_MsgController.AddDummyFunctions(DUMMY_MSG_CLOSE_TCP, DummyDeleteTCPClient);
	m_MsgController.AddDummyFunctions(DUMMY_MSG_CLEAR_TIMEOUT, DummyClearTimeOut);

	
	m_MsgController.AddErrFunctions( ERR_MSG_ALL_BYPASS, ErrMsgProc);
	m_MsgController.AddQueryFunctions( QUERY_MSG_ALL_BYPASS , QueryProc );

	strMsg.Format(L"Init CTCP_ClientManager");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );

	return 0;
}

int CTCP_ClientManager::Release( void )
{
	CString strMsg;
	int i;
	IClient * ptrClient = NULL;
	DummyMsg_t DummyMsg;

	/*서버에서 종료 처리 시 Client 가 자체적으로 Release 않게 함 */
	m_bReleasing = TRUE;

	m_DBManager = NULL;
	m_Controller = NULL;
	m_TinyWindow = NULL;		
	m_MsgController.ReleaseFuncstions();

	/* 모든 TCP Client의 Release 함수 호출 */
	Lock();
	/* TCP Manager에 등록된 TCP Client 의 메모리 해제 */

	for( i =0 ; i< MAX_TCP_CLIENT_CNT ; i++ )
	{			
		ptrClient = m_ClientList.Clients[ i ].ClientObj;
		if (ptrClient != NULL )
		{
			memset( &DummyMsg, 0, sizeof( DummyMsg_t ));
			DummyMsg.Msg = DUMMY_MSG_CLOSE_TCP;
			DummyMsg.SysCode = SYS_VISION;
			DummyMsg.ClientID = ptrClient->GetMySIPID();

			ptrClient->Release();
			Sleep(10);
			delete ptrClient;				
			
			SendController( TARGET_DBMANAGER,SEND_DUMMY_MSG, &DummyMsg );
			SendController( TARGET_MAINVIEWER,SEND_DUMMY_MSG, &DummyMsg );
		}
		else
			break;
	}

	memset( &m_ClientList, 0, sizeof( ClientList_t ));
	UnLock();
	strMsg.Format(L"Release CTCP_ClientManager");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );

	return 0;
}

int CTCP_ClientManager::Idle( void )	
{

#if 1
	int i,Cnt;
	IClient * ptrClient = NULL;

	
	Cnt = m_ClientList.Cnt;

	/* 모든 TCP Client의 Idle 함수 호출 */
	
	for( i =0 ; i< Cnt ; i++ )
	{	
		Lock();
		ptrClient = m_ClientList.Clients[ i ].ClientObj;
		if (ptrClient != NULL )
		{
			ptrClient->Idle();
		}		
		UnLock();
	}
	
#if 0
	CString strMsg;
	strMsg.Format(L"Idle CTCP_ClientManager");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );
#endif
#endif
	return 0;
}

BOOL CTCP_ClientManager::SetControllerHandler( IController * ControllerHandle )
{
	CSVR_Doc * ptrDoc = NULL;
	VSSERVERINFO_t * ptrServerInfo = NULL;

	m_Controller = ControllerHandle;	
	if ( m_Controller != NULL)
	{
		memset( &m_stSvrInfo, 0, sizeof( VSSERVERINFO_t ));
	
		ptrDoc = (CSVR_Doc * )m_Controller->GetSVR_DocHandler();
		if ( ptrDoc != NULL )
		{
			ptrServerInfo = ptrDoc->GetServerInfo();
			if ( ptrServerInfo != NULL )
			{
				memcpy( &m_stSvrInfo, ptrServerInfo, sizeof( VSSERVERINFO_t ));				
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CTCP_ClientManager::SetDBManagerHandler( IModel * DBManager )
{
	m_DBManager = DBManager;
	return TRUE;
}

BOOL CTCP_ClientManager::SetTinyWindow( ITinyWindow * TinyWindo  )
{
	m_TinyWindow = TinyWindo;
	return TRUE;
}

/* TCP Server로 부터 Accept 된 Client의 등록 */
BOOL CTCP_ClientManager::AddTCPClient( SOCKET &ClientSock, SOCKADDR_IN &ClientAddr )
{
	int Cnt;
	CString strMsg;

	Cnt = m_ClientList.Cnt;
	
	if ( Cnt >= ( MAX_TCP_CLIENT_CNT  ) || m_bReleasing == TRUE )
	{	
		shutdown( ClientSock, SD_BOTH );
		closesocket( ClientSock );

		strMsg.Format(L"Err AddTCPClient Cnt >= ( MAX_TCP_CLIENT_CNT -1 ) || m_bReleasing == TRUE");
		DumpStr( strMsg );

		return FALSE;
	}	
	
	if ( CreateTCPClient( ClientSock , ClientAddr ) != TRUE )
	{		
		return FALSE;
	}

	return TRUE;
}

/* TCP Client로 받은 Message처리 */
int CTCP_ClientManager::OnRecvTCPClient( MsgInfo_t * MsgInfo )
{
	return m_MsgController.OnRecvMsg( MsgInfo );
}

int CTCP_ClientManager::SendTCPClient( ClientSendMsg_t * ClientSendMsg )
{

	int i,Cnt;
	IClient * ptrClient = NULL;
	int nMsg =0;

	if ( ClientSendMsg == NULL)
	{
		return -1;
	}

	/* MsgInfo의 TargetID와 TCP Client의 myClientID가 일치 할경우 보냄 */
#if 0
	Cnt = GetClientCnt();
#else
	Cnt = m_ClientList.Cnt;
#endif

	nMsg = ClientSendMsg->Msg & 0xFFFF;

	for( i =0 ; i< Cnt ; i++ )
	{
		//Lock();
		ptrClient = m_ClientList.Clients[ i ].ClientObj;
		//UnLock();
		if ( nMsg == SOCK_TEST_STRIP_RESULT_MSG )
		{
			if ( ptrClient != NULL && (ptrClient->GetMySIPID() == ClientSendMsg->SIPID) )
			{
				ptrClient->SendMsg( ClientSendMsg );
			}
		}
		else
		{
			if (ptrClient != NULL && ( ptrClient->GetMySIPID() == ClientSendMsg->SIPID ))
			{
				return ptrClient->SendMsg( ClientSendMsg );
			}
		}
	}

	return 0;
}
BOOL CTCP_ClientManager::CheckSameClientID( IClient * ptrCliendObj )
{

	int i,Cnt;
	IClient * ptrListCliendObj;
	int ListClientID, ClientID;
	

	if ( ptrCliendObj == NULL)
	{
		return -1;
	}

	/* MsgInfo의 TargetID와 TCP Client의 myClientID가 일치 할경우 보냄 */	

	Cnt = m_ClientList.Cnt;

	for( i =0 ; i< Cnt ; i++ )
	{
		Lock();
		ptrListCliendObj = m_ClientList.Clients[ i ].ClientObj;

		if ( ptrListCliendObj != NULL )
		{
			ListClientID = ptrListCliendObj->GetMySIPID();
			ClientID = ptrCliendObj->GetMySIPID();

			if ( ( ptrListCliendObj != ptrCliendObj ) && ( ListClientID != 0 && ClientID != 0 ) &&  ( ListClientID == ClientID ))
			{
#if 0
				DummyMsg.ClientID = ListClientID;
				DummyMsg.ptrEtcInfo = ptrListCliendObj;
				DummyDeleteTCPClient( &DummyMsg );
				ptrListCliendObj->Release();
#else
				ptrListCliendObj->Close();
#endif
				UnLock();
				return TRUE;
			}
		}
		UnLock();
		
	}
	
	return FALSE;
}

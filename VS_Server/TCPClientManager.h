#pragma once

#include "IController.h"
#include "IClient.h"
#include "IModel.h"
#include "ITinyWindow.h"
#include "Com_Def.h"
#include "MsgController.h"
#include "CONTROLS_DEF.h"
#include "Util.h"
#include <winsock2.h>

#define MAX_MANAGER_IP_CNT	15

typedef struct Clients_
{
	SOCKET ClientSock;
	IClient * ClientObj;
	char szIP[ MAX_IP_SIZE + 2 ];
}Clients_t;

typedef struct ClientList_
{
	volatile int Cnt;
	Clients_t Clients[ MAX_TCP_CLIENT_CNT ];
}ClientList_t ;

typedef struct MultiAcceptIP_
{
	char szIP[ MAX_IP_SIZE + 2 ];
}MultiAcceptIP_t;

class CTCP_ClientManager
{
public:
	CTCP_ClientManager(void);
	~CTCP_ClientManager(void);

public:
	int Initialize( void );
	int Release( void );
	int Idle( void );	
	BOOL SetControllerHandler( IController * ControllerHandle);
	BOOL SetDBManagerHandler( IModel * DBManager );	

	BOOL SetTinyWindow( ITinyWindow * TinyWindo );

	BOOL AddTCPClient( SOCKET &ClientSock ,SOCKADDR_IN &ClientAddr );
	int SendTCPClient( ClientSendMsg_t * ClientSendMsg );
	int OnRecvTCPClient( MsgInfo_t * MsgInfo );
	int byPassExigentNetworkMsg( ClientRecvMsg_t * ClientRecvMsg );	
	
	BOOL CreateDir( CString &strPath );
	BOOL CheckSameClientID( IClient * ptrCliendObj );
	int  GetMacAddress(unsigned char *mac , struct in_addr destip) ;


private:
	ITinyWindow * m_TinyWindow;
	IController * m_Controller;
	CUtil m_Util;
	ClientList_t m_ClientList;
	CMsgController m_MsgController;	
	VSSERVERINFO_t	m_stSvrInfo;
	volatile int m_nCreatedRootDir;	
	volatile BOOL m_bReleasing;
	IModel * m_DBManager;
	MultiAcceptIP_t m_MultiAcceptIP[ MAX_MANAGER_IP_CNT ];

	BOOL CreateTCPClient( SOCKET &ClientSock , SOCKADDR_IN &ClientAddr );
	int GetClientCnt( void );
	int SetClientCnt(int Cnt );
	int SendController( BYTE TagetObject, short Msg, void * ptr );

	int Lock();
	int UnLock();

	void GetManagerIP( void );
	BOOL IsManagerIP( char *ClientIP );

	/* MessageController용 등록 함수 */
	static int DummyDeleteTCPClient( DummyMsg_t *DummyMsg );
	static int DummySendFileManager( DummyMsg_t *DummyMsg );
	static int DummyClearTimeOut( DummyMsg_t *DummyMsg );
	static int QueryProc( QueryMsg_t *QueryMsg );
	static int ErrMsgProc( ErrMsg_t * ErrMsg );
	void DumpStr( CString &str );
	
};

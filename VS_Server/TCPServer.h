#pragma once

#include "Com_Def.h"
#include "ITransfer.h"
#include "IController.h"
#include "TCPClientManager.h"
#include "MsgController.h"
#include "Util.h"
#include <winsock2.h>

class CTCPServer: public ITransfer
{
public:
	CTCPServer(void);
	~CTCPServer(void);

	virtual int Initialize( void );
	virtual int Open( TransferConf_t * TransferConf );
	virtual int Close( void );
	virtual	int Release( void );
	virtual int Idle( void );
	virtual BOOL SetControllerHandler( void *  ControllerHandle );
	virtual BOOL SetTinyWindow( ITinyWindow * TinyWindow );
	virtual int OnRecvMsg( MsgInfo_t * MsgInfo );
	

private:
	CTCP_ClientManager m_TCPClientManager;
	ITinyWindow * m_TinyWindow;
	IController * m_Controller;
	BOOL m_bConnected;
	CMsgController m_MsgController;

	WSADATA		m_WsaData;
	SOCKET		m_ServSock;
	SOCKADDR_IN	m_ServAddr;
	HANDLE		m_hTh_Accepter;
	DWORD		m_dwAccepter;
	int m_ServerID;
	CUtil m_Util;

	void DumpStr( CString &str );
	void InitSocket();
	BOOL SetTCPServerListen( char * szServerIP, UINT16 ServerPort );
	static DWORD WINAPI Thread_Accepter(LPVOID pParam);

	BOOL TCPServerOpen( char * szServerIP, UINT16 ServerPort );
	void TCPServerClose( void );

	int SendController( BYTE TagetObject, short Msg, void * ptr );
	int SendSocketState( BYTE State );

	/* MessageController용 등록 함수 */
	static int SendTCPManager( ClientSendMsg_t *ClientSendMsg );
	static int DummySendTCPManager( DummyMsg_t *DummyMsg );
};


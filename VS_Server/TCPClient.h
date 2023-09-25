#pragma once

#include "Com_Def.h"
#include "IClient.h"
#include "ITinyWindow.h"
#include "Packet.h"
#include "Message.h"
#include "Util.h"
#include "CONTROLS_DEF.h"
#include "QueryProcessor.h"
#include "IModel.h"
#include "IController.h"
//#include "./Include/ximage.h"

#include <winsock2.h>

class CTCP_ClientManager;


class CTCPClient: public IClient
{
public:
	CTCPClient(void);
	~CTCPClient(void);

	virtual int Initialize( int Position, SOCKET &ClientSock );
	virtual	int Release( void );
	virtual int Idle( void );
	virtual int Close( void );
	virtual int GetMySIPID( void );
	virtual int GetMyPostion( void );
	virtual int SetRePostion( int Postion );
	virtual BOOL SetClientManagerHandler( void * TCPClientManager );
	virtual BOOL SetTinyWindow( ITinyWindow * pTinyWindo );
	virtual int SendMsg( ClientSendMsg_t * ClientSendMsg );
	virtual int SetSerConf( void * ptrServerConf );
	virtual int SetMyIP( TCHAR * csIP, TCHAR * csMAC  );
	
private:
	int m_MySIPID;
	int m_MyPosition;	

	volatile BOOL m_bRuned;
	SOCKET m_MySocket;
	volatile time_t m_CloseSocketTime;
	TCHAR m_MyIP[ MAX_IP_SIZE + 2 ];
	TCHAR m_MyMAC[ MAX_MAC_SIZE ];

	ITinyWindow * m_TinyWindow;
	CTCP_ClientManager * m_TCPClientManager;
	VSSERVERINFO_t	m_stSvrInfo;

	HANDLE m_hThread;
	DWORD m_dwThread;	
	
	HANDLE m_hIdleThread;
	DWORD m_dwIdleThread;
	time_t g_pre_time;

#if 0	
	BYTE m_Packet[ ONE_IMAGE_PACKET_SIZE ];	
	BYTE m_DummyBuf[ ONE_IMAGE_PACKET_SIZE ];	
	Queue_t m_Queue;
	ImageQueue_t m_ImageQueue;
#else
	BYTE * m_Packet;
	BYTE * m_DummyBuf;	
	Queue_t * m_Queue;
	ImageQueue_t * m_ImageQueue;
	BYTE * m_BmpBuf;
	DWORD m_BmpBufSize;	
	LastCarInfo_t m_LastCarInfo;

#endif
	CPacket m_Parser;
	CMessage m_Message;
	ClientRecvMsg_t m_RecvMsg;
	
	CUtil m_Util;
	volatile BOOL m_isFirstPacket;
	
	CFile m_FileObject;
	CFileException  m_FileEX;
	ClientDir_t m_MyClientDir;
	
	
	static DWORD WINAPI ThreadRead(LPVOID pParam);
	static DWORD WINAPI ThreadIdle(LPVOID pParam);

	BOOL CreateThrHandle( void );		
	int SendTCPClientManager( short Msg, void * ptr );
	void SendTinyWindowMsg( ClientRecvMsg_t * ClientRecvMsg );
	
	int PutQueue( int Size, BYTE * Buf );
	BYTE *PopQueue(int *Size );
	BOOL ValifyPacket( int Size, BYTE * Buf );		

	int MySelfKIll( SOCKET TempSock  );

	void DumpStr( CString &str );

	void SendBaseinfoPacket( void );
};

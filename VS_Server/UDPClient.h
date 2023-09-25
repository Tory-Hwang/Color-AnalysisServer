#pragma once
#include "ITransfer.h"
#include "Message.h"
#include "Packet.h"
#include "Util.h"
class CUDPClient : public ITransfer
{
public:
	CUDPClient(void);
	~CUDPClient(void);

	virtual int Initialize( void );
	virtual int Open( TransferConf_t * TransferConf );
	virtual int Close( void );
	virtual	int Release( void );
	virtual int Idle( void );
	virtual BOOL SetControllerHandler( void *  ControllerHandle );
	virtual BOOL SetTinyWindow( ITinyWindow * TinyWindow );
	virtual int OnRecvMsg( MsgInfo_t * MsgInfo );
	
private:

	IController * m_Controller;

	WSADATA wsaData;
	SOCKET  hsocket;
	SOCKADDR_IN	send_servAddr;	
	SOCKADDR_IN	recv_servAddr;

	WSADATA		m_WsaData;
	SOCKET		m_ServSock;
	SOCKADDR_IN	m_ServAddr;
	SOCKADDR_IN	m_ReadAddr;

	HANDLE		m_hThRead;
	DWORD		m_dwRead;
	CMessage	m_Message;
	CPacket		m_Parser;
	CUtil	    m_Util;

	int m_ServerID;
	volatile time_t m_LastSendTime;
	volatile time_t m_LastRecvTime;
	ClientRecvMsg_t m_RecvMsg;
	TransferConf_t m_TransferConf;

	BOOL bFirsetSend;
	BOOL m_bConnected;
	TCHAR m_strCurPath[ MAX_PATH ];

	void DumpStr( CString &str );
	BOOL UDPClientOpen( char * szServerIP, UINT16 SendPort );
	void UDPClientClose( void );
	void ProcHeartbit( void );
	void SendHeartbitMsg( void);
	void ReStartMonitoringPrg( void );
	BOOL ParsePacket( int Size, BYTE * Buf );
	

	static DWORD UDPRead_Thread( LPVOID );


};

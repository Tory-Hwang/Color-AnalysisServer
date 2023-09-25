#pragma once

#include "ITinyWindow.h"
#include "Com_Def.h"

class IClient
{
public :
	virtual int Initialize( int Position, SOCKET &ClientSock ) =0;
	virtual	int Release( void ) =0;
	virtual int Idle( void )=0;
	virtual int Close( void ) = 0;

	virtual int GetMySIPID( void ) = 0;
	virtual int GetMyPostion( void ) = 0;
	virtual int SetRePostion( int Postion ) =0 ;

	virtual BOOL SetClientManagerHandler( void * TCPClientManager )=0;
	virtual BOOL SetTinyWindow( ITinyWindow * pTinyWindo ) =0;
	virtual int SendMsg( ClientSendMsg_t * ClientSendMsg ) = 0;
	virtual int SetSerConf( void * ptrServerConf )=0;
	virtual int SetMyIP( TCHAR * csIP , TCHAR * csMAC )=0;
};
#pragma once

#include "ITinyWindow.h"
#include "Com_Def.h"

class ITransfer
{
public :
	virtual int Initialize( void ) =0;
	virtual BOOL Open( TransferConf_t * TransferConf ) =0;
	virtual int Close( void ) =0;
	virtual	int Release( void ) =0;
	virtual int Idle( void ) = 0;

	virtual BOOL SetControllerHandler( void *  ControllerHandle )=0;
	virtual BOOL SetTinyWindow( ITinyWindow * TinyWindow )= 0;
	

	virtual int OnRecvMsg( MsgInfo_t * MsgInfo ) = 0;
};
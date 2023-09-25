#pragma once

#include "Com_Def.h"

class IModel
{
public :
	virtual int Initialize(void * ptr) =0;
	virtual BOOL Open( ModelConf_t * ModelConf ) =0;
	virtual int Close( void ) =0;
	virtual	int Release( void ) =0;
	virtual int Idle( void )=0;

	virtual BOOL SetControllerHandler( void *  ControllerHandle )=0;
	virtual int OnRecvSubModelMsg( void * SubModelMsg )=0;	
	virtual int OnRecvMsg( MsgInfo_t * MsgInfo ) = 0;

	virtual void * GetSubModelObject( void ) = 0;


};
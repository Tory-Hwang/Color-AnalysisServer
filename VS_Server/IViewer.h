#pragma once

#include "Com_Def.h"

class IViewer
{
public :
	virtual BOOL SetControllerHandler( void *  ControllerHandle )=0;	
	virtual int OnRecvMsg( MsgInfo_t *MsgInfo ) = 0;
	virtual BOOL AddModelHandler( int ModelKind, void * ModelHandle ) =0;	

};
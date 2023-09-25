#pragma once

#include "Com_Def.h"

class IController
{
public :

	virtual BOOL ITinyWindow_OnWndProc(void * pWindow, UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;
	virtual int Initialize( void ) =0;
	virtual	int Release( void ) =0;
	virtual int Idle( void )=0;	
	virtual int Close( void )=0;

	virtual BOOL OpenTransfer( TransferConf_t * TransferConf ) =0;
	virtual BOOL OpenModel( ModelConf_t * ModelConf ) =0;	
	
	virtual void * GetSVR_DocHandler( void )=0;
	virtual int GetMyServerID( void )=0;
	virtual int GetDBUseState( void )=0;
	virtual int GetRunMonitor( void )=0;

	virtual BOOL AddModelHandler( BYTE ModelKind, void * ModelHandle )=0;	
	virtual BOOL AddViewerHandler( BYTE ViewerKind, void * ViewerHandle )=0;
	virtual BOOL AddTransferHandler( BYTE TransferKind, void * TransferHandle)=0;

	virtual int SendMsg( BYTE TargetObject, MsgInfo_t * MsgInfo )=0;
	
};
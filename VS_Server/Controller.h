#pragma once

#include "Com_Def.h"
#include "Com_Object_Def.h"
//#include "ITinyWindow.h"
#include "SVR_Doc.h"
#include "UDPClient.h"

class CController : public IController
{
public:
	CController(void);
	~CController(void);

	/* ITinyWindow Inheritance function */
	virtual BOOL ITinyWindow_OnWndProc(void * pWindow, UINT uMsg, WPARAM wParam, LPARAM lParam);

	/* IController Inheritance functions */
	virtual int Initialize( void );
	virtual	int Release( void );
	virtual int Idle( void );
	virtual int Close( void );

	virtual BOOL OpenTransfer( TransferConf_t * TransferConf );
	virtual BOOL OpenModel( ModelConf_t * ModelConf );	

	virtual void * GetSVR_DocHandler( void );	
	virtual int GetMyServerID( void );
	virtual int GetDBUseState( void );
	virtual int GetRunMonitor( void );

	virtual BOOL AddModelHandler( BYTE ModelKind, void * ModelHandle );	
	virtual BOOL AddViewerHandler( BYTE ViewerKind, void * Viewer_Handle );
	virtual BOOL AddTransferHandler( BYTE TransferKind, void * TransferHandle );
	virtual int SendMsg( BYTE TargetObject, MsgInfo_t * MsgInfo );	

private:
	
	ObjectManager_t m_ObjectManager;
	CSVR_Doc m_SVR_Doc;
	CUDPClient m_UdpClient;

	int SendTransfer( BYTE KindTransfer, MsgInfo_t * MsgInfo );
	int SendModel( BYTE KindModel, MsgInfo_t * MsgInfo );	
	int SendViewer( BYTE KindViewer,MsgInfo_t * MsgInfo );
	int SetServerInfo( void * ptr );

	void DumpStr( CString &str );
	

};



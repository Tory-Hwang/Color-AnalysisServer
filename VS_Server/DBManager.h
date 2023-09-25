#pragma once

#include "Com_Def.h"
#include "IModel.h"
#include "IController.h"
#include "QueryProcessor.h"
//#include "MsgController.h"
#include "ADO.h"
#include "Util.h"

class CDBManager:public IModel
{
public:
	CDBManager(void);
	~CDBManager(void);

	virtual int Initialize(  void * ptr );
	virtual BOOL Open( ModelConf_t * ModelConf );
	virtual int Close( void );
	virtual	int Release( void );
	virtual int Idle( void );

	virtual BOOL SetControllerHandler( void *  ControllerHandle );
	virtual int OnRecvSubModelMsg( void * SubModelMsg );
	virtual int OnRecvMsg( MsgInfo_t * MsgInfo );
	virtual _ConnectionPtr  GetSpecialHandler1( void );
	virtual _ConnectionPtr  GetSpecialHandler2( void );

	virtual void * GetSubModelObject(void ){ return &m_BlockQueryProc ;};

private:

	CQueryProcessor m_BlockQueryProc;	

	IController * m_Controller;	
	_ConnectionPtr	m_pCONCmd; 
	_ConnectionPtr	m_pCONRead; 
	ModelConf_t m_ModelConf;
	CUtil m_Util;
	BOOL m_bConnect;
	CADO m_ADO;

	void DumpStr( CString &str );	
	BOOL InitADO( void );
	BOOL DBOpen( ModelConf_t * ModelConf );
	void DBClose( void );
	void CheckADO();

	int Lock();
	int UnLock();
	
	void SendSetDBConnect();

	static int SendQueryMsgProc( QueryMsg_t * QueryMsg);
	static int SendFileMsgProc( FileMsg_t * FileMsg);
	static int SendClientRecvMsgProc( ClientRecvMsg_t * ClientRecvMsg);
	static int SendClientSendMsgProc( ClientSendMsg_t * ClientSendMsg);
	static int SendErrMsgProc( ErrMsg_t * ErrMsg);


};


#pragma once

#include "Com_Def.h"
#include "IModel.h"
#include "IController.h"
#include "Util.h"
#include "MsgController.h"
#include "CONTROLS_DEF.h"



class CFileManager :public IModel
{
public:
	CFileManager(void);
	~CFileManager(void);

	virtual int Initialize( void * ptr );
	virtual BOOL Open( ModelConf_t * ModelConf );
	virtual int Close( void );
	virtual	int Release( void );
	virtual int Idle( void );

	virtual BOOL SetControllerHandler( void *  ControllerHandle );
	virtual int OnRecvSubModelMsg( void * SubModelMsg );
	virtual int OnRecvMsg( MsgInfo_t * MsgInfo );
	virtual void * GetSubModelObject( void ){ return NULL ;};

private:

	IController * m_Controller;
	CUtil m_Util;
	CMsgController m_MsgController;

#if 0
	TCHAR m_strCurPath[ MAX_PATH ];
	char m_szCreatedRootDir[ MAX_PATH ];	
	int m_CreatedClientDir[ MAX_TCP_CLIENT_CNT ];
#else
	TCHAR * m_strCurPath;
	char * m_szCreatedRootDir;	
	ClientDir_t m_CreatedClientDir[ MAX_TCP_CLIENT_CNT ];

#endif
	VSSERVERINFO_t	m_stSvrInfo;

	//CFile m_fileDest;
	//CFileException  m_ex;

	void DumpStr( CString &str );
	BOOL CreateDir( CString & strPath );
	BOOL isFindRootDirectory( char * szTime, TCHAR * strTime  );
	BOOL isFindClientDirectory( int ClientID  , TCHAR * strTime, int Day, int Hour, int *Basic_Hour  );
	void GetServerInfo( void * ptr );

	void InitClientDir( void );
	void SetAlert( void );

	/* MessageController용 등록 함수 */
	static int MakeDirectory( DummyMsg_t * DummMsg );
	static int SaveImageFile( DummyMsg_t * DummMsg );

};

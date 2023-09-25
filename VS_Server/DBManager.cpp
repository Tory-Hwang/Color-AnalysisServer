#include "StdAfx.h"
#include "DBManager.h"

static CRITICAL_SECTION _QueryProcessLock = { 0 };

CDBManager::CDBManager(void)
:m_Controller( NULL ), 
m_pCONCmd( NULL ), 
m_pCONRead( NULL ), 
m_bConnect ( FALSE )
{
	CoInitialize(NULL);
	InitializeCriticalSection(&_QueryProcessLock);

}

CDBManager::~CDBManager(void)
{
	DeleteCriticalSection(&_QueryProcessLock);
	CoUninitialize();
}

/* =====================================================================
Internal Functions 
=======================================================================*/

void CDBManager::DumpStr( CString &str )
{
#if DEBUG_DBMANAGER
	CString strtxt;
	strtxt.Format(L"VS_SERVER::CDBManager : %s \n", str);
	strtxt.ReleaseBuffer();
	OutputDebugString( strtxt );
#endif
}


int CDBManager::Lock()
{
	EnterCriticalSection(&_QueryProcessLock);
	return 0;
}

int CDBManager::UnLock()
{
	LeaveCriticalSection(&_QueryProcessLock);
	return 0;
}

BOOL CDBManager::InitADO( void )
{
	if ( m_pCONCmd != NULL )
		return TRUE;

	if ( m_pCONRead != NULL )
		return TRUE;

	m_pCONCmd.CreateInstance(__uuidof(Connection));
	m_pCONRead.CreateInstance(__uuidof(Connection));

	if ( m_pCONCmd  == NULL  || m_pCONRead == NULL )
	{
		CString strMsg;
		strMsg.Format(L"Init_ADO Error ");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg	);
		return FALSE;
	}

	return TRUE;
}

void CDBManager::CheckADO()
{
	if ( m_pCONCmd == NULL || m_pCONCmd->GetState() != adStateOpen  || m_pCONRead == NULL || m_pCONRead->GetState() != adStateOpen )
	{
		CString strMsg;
		DBClose( );
		DBOpen( &m_ModelConf );
		strMsg.Format(L"DB Reconnect because GetSate != adStateOpen");
		DumpStr( strMsg );
	}	
}

BOOL CDBManager::DBOpen( ModelConf_t * ModelConf )
{
	CString strDBCon;
	CString strMsg;
	
	if ( wcslen( ModelConf-> strDSN ) ==0 || wcslen( ModelConf-> strUserID ) ==0 || wcslen( ModelConf-> strUserPwd ) ==0)
		return FALSE;

	if ( InitADO() == TRUE && m_bConnect == FALSE )
	{
		strDBCon.Format(L"Provider=MSDASQL;DSN=%s;User ID=%s;PASSWORD=%s", ModelConf->strDSN, ModelConf->strUserID, ModelConf->strUserPwd );		
		strDBCon.ReleaseBuffer();

		try
		{
			if ( m_pCONCmd->Open( _bstr_t( strDBCon ),"","",adConnectUnspecified) == S_OK )
			{
				m_bConnect = TRUE;
				strMsg.Format(L"DB Connect Success %s", strDBCon);
				strMsg.ReleaseBuffer();
				DumpStr( strMsg	);			
				m_BlockQueryProc.SetDBCmdConnecter( m_pCONCmd );				

				//return TRUE;
			}
		}
		catch (...)
		{
			strMsg.Format(L"Can not DB Connect %s", strDBCon);
			strMsg.ReleaseBuffer();
			DumpStr( strMsg	);
		}		 

		try
		{
			if ( m_pCONRead->Open( _bstr_t( strDBCon ),"","",adConnectUnspecified) == S_OK )
			{
				m_bConnect = TRUE;
				strMsg.Format(L"DB Connect Success %s", strDBCon);
				strMsg.ReleaseBuffer();
				DumpStr( strMsg	);			
				m_BlockQueryProc.SetDBReadConnecter( m_pCONRead );				
				SendSetDBConnect();

				return TRUE;
			}
		}
		catch (...)
		{
			DBClose();
			strMsg.Format(L"Can not DB Connect %s", strDBCon);
			strMsg.ReleaseBuffer();
			DumpStr( strMsg	);
		}		 
	}

	return FALSE;
}

void CDBManager::DBClose( void )
{
	if (m_pCONCmd != NULL )
	{		
		if ( m_ADO.IsOpenDB( m_pCONCmd ) == TRUE)
		{
			m_pCONCmd->Close();
		}

		m_pCONCmd.Release();
		m_pCONCmd = NULL;
	}

	if (m_pCONRead != NULL )
	{		
		if ( m_ADO.IsOpenDB( m_pCONRead ) == TRUE)
		{
			m_pCONRead->Close();
		}

		m_pCONRead.Release();
		m_pCONRead = NULL;
	}


	m_BlockQueryProc.SetDBCmdConnecter( NULL );
	m_BlockQueryProc.SetDBReadConnecter( NULL );	

	m_pCONCmd = NULL;
	m_bConnect = FALSE;	


}

void CDBManager::SendSetDBConnect()
{
	MsgInfo_t MsgInfo;
	DummyMsg_t DummyMsg;

	memset( &MsgInfo, 0, sizeof( MsgInfo_t ));
	memset( &DummyMsg, 0, sizeof(DummyMsg_t ));

	DummyMsg.Msg = DUMMY_MSG_RECONNECT_DB;
	MsgInfo.SendMsgType = SEND_DUMMY_MSG;
	MsgInfo.ptrMsg = &DummyMsg;

	if ( m_Controller != NULL )
	{
		m_Controller->SendMsg( TARGET_TCPSERVER, &MsgInfo );
	}

}
/* =====================================================================
Export Functions 
=======================================================================*/

int CDBManager::Initialize( void * ptr )
{
	CString strMsg;
	
	( void )ptr;
	memset(&m_ModelConf, 0, sizeof(ModelConf_t ));

	m_BlockQueryProc.SetDBManager( this );
	strMsg.Format(L"Init CDBManager");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );
	return 0;
}

BOOL CDBManager::Open( ModelConf_t * ModelConf )
{
	BOOL bRect = FALSE;

	if ( m_Controller->GetDBUseState() == FALSE )
		return TRUE;


	if ( ModelConf == NULL )
	{
		return FALSE;
	}

	memcpy( &m_ModelConf, ModelConf, sizeof( ModelConf_t));	

	bRect = DBOpen( ModelConf );
	m_BlockQueryProc.InitQueryProcess( 0x00 );
	
	return bRect;
}

int CDBManager::Close( void )
{
	DBClose();

	return 0;
}
int CDBManager::Release( void )
{
	CString strMsg;

	DBClose();

	m_BlockQueryProc.Release();
	
	strMsg.Format(L"Release CDBManager");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );

	return 0;
}
int CDBManager::Idle( void )
{
	if ( m_Controller->GetDBUseState() == FALSE )
		return 0;


	CheckADO();
	return 0;
}
BOOL CDBManager::SetControllerHandler( void *  ControllerHandle )
{
	m_Controller = (IController *)ControllerHandle;
	m_BlockQueryProc.SetControllerHandler( m_Controller );
	return TRUE;
}

int CDBManager::OnRecvMsg( MsgInfo_t * MsgInfo )
{
	int nRect;	
	nRect = m_BlockQueryProc.OnRecvMsg( MsgInfo );
	
	return nRect;
}

int CDBManager::OnRecvSubModelMsg( void * SubModelMsg )
{
	( void )SubModelMsg;
	

	/* QueryProcess로 부터 받는 메세지는 Connector가 끊어졌을때만 넘어옴 */
	DBClose( );
	DBOpen( &m_ModelConf );
	
	

	return 0;
}

_ConnectionPtr  CDBManager::GetSpecialHandler1( void )
{
	return m_pCONCmd;
}

_ConnectionPtr  CDBManager::GetSpecialHandler2( void )
{
	return m_pCONRead;
}
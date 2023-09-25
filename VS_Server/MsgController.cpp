#include "StdAfx.h"
#include "MsgController.h"

CMsgController::CMsgController(void)
:m_DummyFuncCnt ( 0 ),
m_ErrFuncCnt( 0 ),
m_ClientRecvFuncCnt( 0 ),
m_ClientSendFuncCnt( 0 ),
m_QueryFuncCnt( 0 ),
m_FileFuncCnt( 0 )
{
	memset( &m_DummyFuncs, 0, sizeof( m_DummyFuncs ));
	memset( &m_ErrFuncs, 0, sizeof( m_ErrFuncs ));
	memset( &m_ClientRecvFuncs, 0, sizeof( m_ClientRecvFuncs ));
	memset( &m_ClientSendFuncs, 0, sizeof( m_ClientSendFuncs ));
	memset( &m_QueryFuncs, 0, sizeof( m_QueryFuncs ));
	memset( &m_FileFuncs, 0, sizeof( m_FileFuncs ));

}

CMsgController::~CMsgController(void)
{
}


int CMsgController::DummyMsgProcess( MsgInfo_t * MsgInfo )
{
	DummyMsg_t * DummyMsg = NULL;
	short Msg;
	int i;

	DummyMsg = ( DummyMsg_t *)MsgInfo->ptrMsg;
	
	if ( DummyMsg == NULL )
	{
		return -1;
	}

	Msg = DummyMsg->Msg;

	for( i = 0 ; i < m_DummyFuncCnt ; i++)
	{
		if ( m_DummyFuncs[ i ].Msg == DUMMY_MSG_ALL_BYPASS || Msg == m_DummyFuncs[ i ].Msg ) 
		{
			return m_DummyFuncs[ i ].CallbackFunc( DummyMsg );
		}
	}

	return -1;
}

int CMsgController::ErrMsgProcess( MsgInfo_t * MsgInfo )
{
	ErrMsg_t * ErrMsg = NULL;
	short Msg;
	int i;

	ErrMsg = ( ErrMsg_t *)MsgInfo->ptrMsg;

	if ( ErrMsg == NULL )
	{
		return -1;
	}

	Msg = ErrMsg->Msg;

	for( i = 0 ; i < m_ErrFuncCnt ; i++)
	{
		if ( m_ErrFuncs[ i ].Msg == ERR_MSG_ALL_BYPASS || Msg == m_ErrFuncs[ i ].Msg ) 
		{
			return m_ErrFuncs[ i ].CallbackFunc( ErrMsg );
		}
	}

	return -1;
}

int CMsgController::ClientRecvMsgProcess( MsgInfo_t * MsgInfo )
{
	ClientRecvMsg_t * ClientRecvMsg = NULL;
	short Msg;
	int i;

	ClientRecvMsg = ( ClientRecvMsg_t *)MsgInfo->ptrMsg;

	if ( ClientRecvMsg == NULL )
	{
		return -1;
	}

	Msg = ClientRecvMsg->Msg;

	for( i = 0 ; i < m_ClientRecvFuncCnt ; i++)
	{
		if ( m_ClientRecvFuncs[ i ].Msg == MSG_NETWORK_ALL_BYPASS || Msg == m_ClientRecvFuncs[ i ].Msg ) 
		{
			return m_ClientRecvFuncs[ i ].CallbackFunc( ClientRecvMsg );
		}
	}

	return -1;	
}

int CMsgController::ClientSendMsgProcess( MsgInfo_t * MsgInfo )
{
	ClientSendMsg_t * ClientSendMsg = NULL;
	short Msg;
	int i;

	ClientSendMsg = ( ClientSendMsg_t *)MsgInfo->ptrMsg;

	if ( ClientSendMsg == NULL )
	{
		return -1;
	}

	Msg = ClientSendMsg->Msg;

	for( i = 0 ; i < m_ClientSendFuncCnt ; i++)
	{
		if ( m_ClientSendFuncs[ i ].Msg == MSG_NETWORK_ALL_BYPASS || Msg == m_ClientSendFuncs[ i ].Msg ) 
		{
			return m_ClientSendFuncs[ i ].CallbackFunc( ClientSendMsg );
		}
	}

	return -1;	
}

int CMsgController::QueryMsgProcess(  MsgInfo_t * MsgInfo  )
{
	QueryMsg_t * QueryMsg = NULL;
	short Msg;
	int i;

	QueryMsg = ( QueryMsg_t *)MsgInfo->ptrMsg;

	if ( QueryMsg == NULL )
	{
		return -1;
	}

	Msg = QueryMsg->Msg;

	for( i = 0 ; i < m_QueryFuncCnt ; i++)
	{
		if ( m_QueryFuncs[ i ].Msg == QUERY_MSG_ALL_BYPASS || Msg == m_QueryFuncs[ i ].Msg ) 
		{
			return m_QueryFuncs[ i ].CallbackFunc( QueryMsg );
		}
	}

	return -1;
}

int CMsgController::FileMsgProcess( MsgInfo_t * MsgInfo)
{
	FileMsg_t * FileMsg = NULL;
	short Msg;
	int i;

	FileMsg = ( FileMsg_t *)MsgInfo->ptrMsg;

	if ( FileMsg == NULL )
	{
		return -1;
	}

	Msg = FileMsg->Msg;

	for( i = 0 ; i < m_FileFuncCnt ; i++)
	{
		if (m_FileFuncs[ i ].Msg == FILE_MSG_ALL_BYPASS ||  Msg == m_FileFuncs[ i ].Msg ) 
		{
			return m_FileFuncs[ i ].CallbackFunc( FileMsg );
		}
	}

	return -1;
}

/* =====================================================================
Export Functions 
=======================================================================*/

int CMsgController::OnRecvMsg( MsgInfo_t * MsgInfo )
{
	short Msg;

	if ( MsgInfo == NULL || MsgInfo->ptrMsg == NULL )
	{
		return -1;
	}

	Msg = MsgInfo->SendMsgType;

	switch ( Msg )		
	{
	case SEND_DUMMY_MSG:
		{
			return DummyMsgProcess( MsgInfo );
		}
	case SEND_VS_RECV_MSG:
		{
			return ClientRecvMsgProcess( MsgInfo);
		}
	case SEND_ERR_MSG:
		{
			return ErrMsgProcess( MsgInfo);
		}
	case SEND_VS_SEND_MSG:
		{
			return ClientSendMsgProcess( MsgInfo );
		}
	case SEND_QUERY_MSG:
		{
			return QueryMsgProcess( MsgInfo );
		}
	case SEND_FILE_MSG:
		{
			return FileMsgProcess( MsgInfo );
		}
		break;
	}

	return -1;
}


int CMsgController::AddDummyFunctions( short Msg, DummyFunc Func )
{
	int i;
	short L_Msg;

	if ( m_DummyFuncCnt >= MAX_FUNC_CNT )
	{
		return -1;
	}

	for( i = 0 ; i < MAX_FUNC_CNT ; i++ )
	{
		L_Msg = m_DummyFuncs[ i ].Msg;
		if ( L_Msg == 0x00 )
		{
			m_DummyFuncs[ i ].Msg = Msg;
			m_DummyFuncs[ i ].CallbackFunc = Func;
			m_DummyFuncCnt++;
			return 0;
		}
	}

	return -1;
}

int CMsgController::AddErrFunctions( short Msg, ErrFunc Func )
{
	int i;
	short L_Msg;

	for( i = 0 ; MAX_FUNC_CNT ; i++ )
	{
		L_Msg = m_ErrFuncs[ i ].Msg;
		if ( L_Msg == 0x00 )
		{
			m_ErrFuncs[ i ].Msg = Msg;
			m_ErrFuncs[ i ].CallbackFunc = Func;
			m_ErrFuncCnt++;
			return 0;
		}
	}
	return -1;
}

int CMsgController::AddClientRecvFunctions( short Msg, ClientRecvFunc Func )
{
	int i;
	short L_Msg;
	
	for( i = 0 ; i < MAX_FUNC_CNT ; i++ )
	{
		L_Msg = m_ClientRecvFuncs[ i ].Msg;
		if ( L_Msg == 0x00 )
		{
			m_ClientRecvFuncs[ i ].Msg = Msg;
			m_ClientRecvFuncs[ i ].CallbackFunc = Func;
			m_ClientRecvFuncCnt++;
			return 0;
		}
	}
	return -1;
}

int CMsgController::AddClientSendFunctions( short Msg, ClientSendFunc Func )
{
	int i;
	short L_Msg;

	for( i = 0 ; i < MAX_FUNC_CNT ; i++ )
	{
		L_Msg = m_ClientSendFuncs[ i ].Msg;
		if ( L_Msg == 0x00 )
		{
			m_ClientSendFuncs[ i ].Msg = Msg;
			m_ClientSendFuncs[ i ].CallbackFunc = Func;
			m_ClientSendFuncCnt++;
			return 0;
		}
	}

	return -1;
}

int CMsgController::AddQueryFunctions( short Msg, QueryFunc Func )
{
	int i;
	short L_Msg;

	for( i = 0 ; i < MAX_QUERY_FUNC_CNT ; i++ )
	{
		L_Msg = m_QueryFuncs[ i ].Msg;
		if ( L_Msg == 0x00 )
		{
			m_QueryFuncs[ i ].Msg = Msg;
			m_QueryFuncs[ i ].CallbackFunc = Func;
			m_QueryFuncCnt++;
			return 0;
		}
	}

	return -1;
}

int CMsgController::AddFileFunctions( short Msg, FileFunc Func )
{
	int i;
	short L_Msg;

	for( i = 0 ; i < MAX_FUNC_CNT ; i++ )
	{
		L_Msg = m_FileFuncs[ i ].Msg;
		if ( L_Msg == 0x00 )
		{
			m_FileFuncs[ i ].Msg = Msg;
			m_FileFuncs[ i ].CallbackFunc = Func;
			m_FileFuncCnt++;
			return 0;
		}
	}

	return -1;
}

int CMsgController::ReleaseFuncstions( void )
{
	memset( &m_DummyFuncs, 0, sizeof( m_DummyFuncs ));
	memset( &m_ErrFuncs, 0, sizeof( m_ErrFuncs ));
	memset( &m_ClientRecvFuncs, 0, sizeof( m_ClientRecvFuncs ));
	memset( &m_ClientSendFuncs, 0, sizeof( m_ClientSendFuncs ));
	memset( &m_QueryFuncs, 0, sizeof( m_QueryFuncs ));
	memset( &m_FileFuncs, 0, sizeof( m_FileFuncs ));
	return 0;
}
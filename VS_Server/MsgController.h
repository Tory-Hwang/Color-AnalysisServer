#pragma once
#include "Com_Def.h"

#define	MAX_FUNC_CNT	20
#define MAX_QUERY_FUNC_CNT	200

typedef int ( * DummyFunc )( DummyMsg_t * );
typedef int ( * ErrFunc )( ErrMsg_t * );
typedef int ( * ClientRecvFunc )( ClientRecvMsg_t * );
typedef int ( * ClientSendFunc )( ClientSendMsg_t * );
typedef int ( * QueryFunc )( QueryMsg_t * );
typedef int ( * FileFunc )( FileMsg_t * );


typedef struct DummyFunc_
{
	short Msg;
	DummyFunc CallbackFunc;
}DummyFunc_t ;

typedef struct ErrFunc_
{
	short Msg;
	ErrFunc CallbackFunc;
}ErrFunc_t;

typedef struct ClientRecvFunc_
{
	short Msg;
	ClientRecvFunc CallbackFunc;
}ClientRecvFunc_t;

typedef struct ClientSendFunc_
{
	short Msg;
	ClientSendFunc CallbackFunc;
}ClientSendFunc_t;

typedef struct QueryFunc_
{
	short Msg;
	QueryFunc CallbackFunc;
}QueryFunc_t;

typedef struct FileFunc_
{
	short Msg;
	FileFunc CallbackFunc;
}FileFunc_t;

class CMsgController
{
public:
	CMsgController(void);
	~CMsgController(void);

	int OnRecvMsg( MsgInfo_t * MsgInfo );

	int AddDummyFunctions( short Msg, DummyFunc Func);
	int AddErrFunctions( short Msg, ErrFunc Func );
	int AddClientRecvFunctions( short Msg, ClientRecvFunc Func );
	int AddClientSendFunctions( short Msg, ClientSendFunc Func );
	int AddQueryFunctions( short Msg, QueryFunc Func );
	int AddFileFunctions( short Msg, FileFunc Func );
	int ReleaseFuncstions( void );

private:
	
	DummyFunc_t m_DummyFuncs[ MAX_FUNC_CNT ];
	ErrFunc_t m_ErrFuncs[ MAX_FUNC_CNT ];
	ClientRecvFunc_t m_ClientRecvFuncs[ MAX_FUNC_CNT ];
	ClientSendFunc_t m_ClientSendFuncs[ MAX_FUNC_CNT ];
	QueryFunc_t m_QueryFuncs[ MAX_QUERY_FUNC_CNT ];
	FileFunc_t m_FileFuncs[ MAX_FUNC_CNT ];

	volatile int m_DummyFuncCnt;
	volatile int m_ErrFuncCnt;
	volatile int m_ClientRecvFuncCnt;
	volatile int m_ClientSendFuncCnt;
	volatile int m_QueryFuncCnt;
	volatile int m_FileFuncCnt;

	int DummyMsgProcess( MsgInfo_t * MsgInfo );
	int ErrMsgProcess( MsgInfo_t * MsgInfo );
	int ClientRecvMsgProcess( MsgInfo_t * MsgInfo );
	int ClientSendMsgProcess( MsgInfo_t * MsgInfo );
	int QueryMsgProcess(  MsgInfo_t * MsgInfo  );
	int FileMsgProcess( MsgInfo_t * MsgInfo);
	
	
};


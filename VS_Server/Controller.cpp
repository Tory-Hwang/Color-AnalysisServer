#include "StdAfx.h"
#include "Controller.h"




CController::CController(void)
{
	memset( &m_ObjectManager, 0, sizeof( ObjectManager_t ));	
}

CController::~CController(void)
{
	CString str;

	str.Format(L"Destroy Controller");
	DumpStr( str );

}

void CController::DumpStr( CString &str )
{
#if DEBUG_CONTROLLER
	CString strtxt;
	strtxt.Format(L"VS_SERVER::CController : %s \n", str);
	strtxt.ReleaseBuffer();
	OutputDebugString( strtxt );
#endif
}

int CController::SendTransfer( BYTE KindTransfer, MsgInfo_t * MsgInfo )
{
	int i, TransferCnt;
	TransferInfo_t * ptr = NULL;
	
	TransferCnt = m_ObjectManager.TransferCnt;

	/* 등록된 모든 Model의 OnRecv_Msg Function 호출 */
	for( i = 0 ;i < TransferCnt ; i++ )
	{
		ptr = ( TransferInfo_t *)&m_ObjectManager.TransferObjects[ i ];
		if ( ptr->ptrTransfer != NULL && ptr->Kind == KindTransfer )
		{
			return ptr->ptrTransfer->OnRecvMsg( MsgInfo );
		}
	}
	return -1;
}

int CController::SendModel( BYTE KindModel, MsgInfo_t * MsgInfo )
{
	int i, ModelCnt;
	ModelInfo_t * ptr = NULL;

	ModelCnt = m_ObjectManager.ModelCnt;
	
	/* 등록된 모든 Model의 OnRecv_Msg Function 호출 */
	for( i = 0 ;i < ModelCnt ; i++ )
	{
		ptr = ( ModelInfo_t *)&m_ObjectManager.ModelObjects[ i ];

		if ( ptr->ptrModel != NULL && ptr->Kind == KindModel  )
		{
			return ptr->ptrModel->OnRecvMsg( MsgInfo );			
		}
	}

	return -1;
}

int CController::SendViewer( BYTE KindViewer,MsgInfo_t * MsgInfo )
{
	int i, Viewer_Cnt;
	ViewerInfo_t * ptr = NULL;

	Viewer_Cnt = m_ObjectManager.ViewerCnt;

	/* 등록된 모든 Viewer의 OnRecv_Msg Function 호출 */
	for( i = 0 ;i < Viewer_Cnt ; i++ )
	{
		ptr = ( ViewerInfo_t *)&m_ObjectManager.ViewerObjects[ i ];
		
		if ( ptr->ptrViewer != NULL  && ptr->Kind == KindViewer )
		{
			return ptr->ptrViewer->OnRecvMsg( MsgInfo );			
		}
	}

	return -1;
}

int CController::SetServerInfo( void * ptr )
{	
	if ( ptr == NULL )
		return -1;

	m_SVR_Doc.SetServerInfo( (VSSERVERINFO_t *)ptr );
	return 0;
}
/* =====================================================================
Export Functions 
=======================================================================*/

BOOL CController::ITinyWindow_OnWndProc(void * pWindow, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bResult= TRUE;
	ClientRecvMsg_t * pEvt_Msg, Evt_Msg;
	MsgInfo_t MsgInfo;

	( void )wParam;
	( void )pWindow;
	
	

	if (uMsg == WM_TRANSFER_MSG )
	{

		memset( &Evt_Msg, 0 , sizeof( ClientRecvMsg_t ));
		pEvt_Msg = ( ClientRecvMsg_t* )lParam;		
		memcpy( &Evt_Msg, pEvt_Msg, sizeof( ClientRecvMsg_t) );
		free( pEvt_Msg );

		MsgInfo.SendMsgType = SEND_VS_RECV_MSG;
		MsgInfo.ptrMsg = &Evt_Msg;

		SendMsg( TARGET_DBMANAGER , &MsgInfo );
		SendMsg( TARGET_FILEMANAGER , &MsgInfo );
		SendMsg( TARGET_MAINVIEWER , &MsgInfo );
	}

	return bResult;
}


int CController::Initialize( void )
{
	CString strMsg;

	int i, TransCnt , ModelCnt;
	TransferInfo_t * ptr = NULL;
	ModelInfo_t * ptr1 = NULL;
	VSSERVERINFO_t ServerConf;

	AddTransferHandler( TRANSFER_KIND_UDP, &m_UdpClient );
	m_UdpClient.SetControllerHandler( this );

	TransCnt = m_ObjectManager.TransferCnt;
	ModelCnt = m_ObjectManager.ModelCnt;
	memset( &ServerConf, 0, sizeof( VSSERVERINFO_t));

	/* 등록된 모든 Transfer의 Release Function 호출 */
	for( i = 0 ;i < TransCnt ; i++ )
	{
		ptr = ( TransferInfo_t *)&m_ObjectManager.TransferObjects[ i ];
		if ( ptr->ptrTransfer != NULL )
		{
			ptr->ptrTransfer->Initialize();			
		}
	}

	/* 등록된 모든 Model의 Release Function 호출 */
	for( i = 0 ;i < ModelCnt ; i++ )
	{
		ptr1 = ( ModelInfo_t *)&m_ObjectManager.ModelObjects[ i ];
		if ( ptr1->ptrModel != NULL )
		{
			ptr1->ptrModel->Initialize( &ServerConf );
			if( ptr1->Kind == MODEL_KIND_FILE )
			{
				SetServerInfo( &ServerConf );
			}
		}
	}

	strMsg.Format(L"Init CController");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );

	return 0;
}


int CController::Release( void )
{
	CString strMsg;

	int i, TransCnt , ModelCnt;
	TransferInfo_t * ptr = NULL;
	ModelInfo_t * ptr1 = NULL;

	TransCnt = m_ObjectManager.TransferCnt;
	ModelCnt = m_ObjectManager.ModelCnt;
		

	/* 등록된 모든 Transfer의 Release Function 호출 */
	for( i = 0 ;i < TransCnt ; i++ )
	{
		ptr = ( TransferInfo_t *)&m_ObjectManager.TransferObjects[ i ];
		if ( ptr->ptrTransfer != NULL )
		{
			ptr->ptrTransfer->Close();
			ptr->ptrTransfer->Release();			
		}
	}

	/* 등록된 모든 Model의 Release Function 호출 */
	for( i = 0 ;i < ModelCnt ; i++ )
	{
		ptr1 = ( ModelInfo_t *)&m_ObjectManager.ModelObjects[ i ];
		if ( ptr1->ptrModel != NULL )
		{
			ptr1->ptrModel->Close();
			ptr1->ptrModel->Release();			
		}
	}

	strMsg.Format(L"Release CController");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );

	this->m_SVR_Doc.ClearVSList();

	return 0;
}

int CController::Idle( void )
{

	int i, TransCnt , ModelCnt;
	TransferInfo_t * ptr = NULL;
	ModelInfo_t * ptr1 = NULL;

	TransCnt = m_ObjectManager.TransferCnt;
	ModelCnt = m_ObjectManager.ModelCnt;
	
	/* 등록된 모든 Transfer의 Idle Function 호출 */
	for( i = 0 ;i < TransCnt ; i++ )
	{
		ptr = ( TransferInfo_t *)&m_ObjectManager.TransferObjects[ i ];

		/* TCP Transfer만 Main Viewer의 idle을 통해 idle함수를 호출한다 */
		//if ( ptr->ptrTransfer != NULL  && ptr->Kind == TRANSFER_KIND_TCP )
		if ( ptr->ptrTransfer != NULL )
		{
			ptr->ptrTransfer->Idle();			
		}
	}

	/* 등록된 모든 Model의 Idle Function 호출 */
	for( i = 0 ;i < ModelCnt ; i++ )
	{
		ptr1 = ( ModelInfo_t *)&m_ObjectManager.ModelObjects[ i ];
		if ( ptr1->ptrModel != NULL )
		{
			ptr1->ptrModel->Idle();			
		}
	}
#if 0
	CString strMsg;
	strMsg.Format(L"Idle CController");
	DumpStr( strMsg );
#endif

	return 0;
}

int CController::Close( void )
{
	CString strMsg;

	int i, TransCnt , ModelCnt;
	TransferInfo_t * ptr = NULL;
	ModelInfo_t * ptr1 = NULL;

	TransCnt = m_ObjectManager.TransferCnt;
	ModelCnt = m_ObjectManager.ModelCnt;

	
	/* 등록된 모든 Transfer의 Close Function 호출 */
	for( i = 0 ;i < TransCnt ; i++ )
	{
		ptr = ( TransferInfo_t *)&m_ObjectManager.TransferObjects[ i ];
		if ( ptr->ptrTransfer != NULL )
		{
			ptr->ptrTransfer->Close();			
		}
	}

	/* 등록된 모든 Model의 Close Function 호출 */
	for( i = 0 ;i < ModelCnt ; i++ )
	{
		ptr1 = ( ModelInfo_t *)&m_ObjectManager.ModelObjects[ i ];
		if ( ptr1->ptrModel != NULL )
		{
			ptr1->ptrModel->Close();			
		}
	}

	strMsg.Format(L"Close CController");
	strMsg.ReleaseBuffer();

	DumpStr( strMsg );
	return 0;

}


BOOL CController::OpenTransfer( TransferConf_t * TransferConf )
{
	int i, TransCnt ;
	TransferInfo_t * ptr = NULL;
	
	TransCnt = m_ObjectManager.TransferCnt;	

	/* 등록된 모든 Transfer의 Close Function 호출 */
	for( i = 0 ;i < TransCnt ; i++ )
	{
		ptr = ( TransferInfo_t *)&m_ObjectManager.TransferObjects[ i ];
		if ( ptr->ptrTransfer != NULL )
		{
			if ( ptr->ptrTransfer->Open( TransferConf ) == FALSE )
			{				
				return FALSE;
			}
		}
	}

	return TRUE;
}

BOOL CController::OpenModel( ModelConf_t * ModelConf )
{	
	int i, ModelCnt;	
	ModelInfo_t * ptr = NULL;
	
	ModelCnt = m_ObjectManager.ModelCnt;

	/* 등록된 모든 Model의 Close Function 호출 */
	for( i = 0 ;i < ModelCnt ; i++ )
	{
		ptr = ( ModelInfo_t *)&m_ObjectManager.ModelObjects[ i ];
		if ( ptr->ptrModel != NULL )
		{
			if ( ptr->ptrModel->Open( ModelConf) == FALSE )
			{
				return FALSE;
			}
		}
	}

	return TRUE;
}

void * CController::GetSVR_DocHandler( void )
{
	return &m_SVR_Doc;	
}

int CController::GetMyServerID( void )
{	
	VSSERVERINFO_t * VSServerInfo = NULL;
	VSServerInfo = m_SVR_Doc.GetServerInfo();
	if ( VSServerInfo != NULL )
	{
		return VSServerInfo->nID;
	}
	return -1;
}

int CController::GetDBUseState( void )
{
	VSSERVERINFO_t * VSServerInfo = NULL;
	VSServerInfo = m_SVR_Doc.GetServerInfo();
	if ( VSServerInfo != NULL )
	{
		return VSServerInfo->nDBUse;
	}

	return 1;
}

int CController::GetRunMonitor( void )
{
	VSSERVERINFO_t * VSServerInfo = NULL;
	VSServerInfo = m_SVR_Doc.GetServerInfo();
	if ( VSServerInfo != NULL )
	{
		return VSServerInfo->nRunMonitor;
	}
	return 1;
}

BOOL CController::AddModelHandler( BYTE ModelKind, void * ModelHandle )
{
	int Cnt =0;
	ModelInfo_t * ptr = NULL;
	
	/* Model Handler 등록 */
	Cnt = m_ObjectManager.ModelCnt;
	
	if (Cnt >= MAX_MODEL_CNT )
	{
		return FALSE;
	}

	ptr = ( ModelInfo_t *)&m_ObjectManager.ModelObjects[ Cnt ];
	ptr->Kind = ModelKind;
	ptr->ptrModel = ( IModel *)ModelHandle;	
	m_ObjectManager.ModelCnt++;

	return TRUE;
}

BOOL CController::AddViewerHandler( BYTE ViewerKind, void * ViewerHandle )
{
	int Cnt =0;
	ViewerInfo_t * ptr = NULL;
	
	/* Viewer Handler 등록 */
	Cnt = m_ObjectManager.ViewerCnt;
	
	if ( Cnt >= MAX_VIEWER_CNT )
	{
		return FALSE;
	}

	ptr = ( ViewerInfo_t *)&m_ObjectManager.ViewerObjects[ Cnt ];
	ptr->Kind = ViewerKind;
	ptr->ptrViewer = ( IViewer *)ViewerHandle;
	m_ObjectManager.ViewerCnt++;

	return TRUE;
}

BOOL CController::AddTransferHandler( BYTE TransferKind, void * TransferHandle)
{
	int Cnt =0;
	TransferInfo_t * ptr = NULL;

	/* Transfer Handler 등록 */
	Cnt = m_ObjectManager.TransferCnt;

	if ( Cnt >= MAX_TRANSFER_CNT )
	{
		return FALSE;
	}

	ptr = ( TransferInfo_t *)&m_ObjectManager.TransferObjects[ Cnt ];
	ptr->Kind = TransferKind;
	ptr->ptrTransfer = ( ITransfer *)TransferHandle;
	m_ObjectManager.TransferCnt++;

	return TRUE;
}

int CController::SendMsg( BYTE TargetObject, MsgInfo_t * MsgInfo )
{
	if ( MsgInfo == NULL )
	{
		return -1;
	}	
	
	switch( TargetObject )
	{
	case TARGET_TCPSERVER:
		{
			return SendTransfer( TRANSFER_KIND_TCP, MsgInfo );
		}
	case TARGET_DBMANAGER:
		{
			return SendModel( MODEL_KIND_DB,MsgInfo );
		}
	case TARGET_GUI_DBMANAGER:
		{
			return SendModel( MODEL_KIND_GUI_DB,MsgInfo );
		}
	case TARGET_FILEMANAGER:
		{
			return SendModel( MODEL_KIND_FILE, MsgInfo );
		}
	case TARGET_MAINVIEWER:
		{
			return SendViewer( VIEWER_KIND_MAIN, MsgInfo );
		}
	}

	return -1;
}

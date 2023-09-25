// VS_Server.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "VS_Server.h"
#include "VS_ServerDlg.h"

#include "Com_Def.h"
#include "TCPServer.h"
#include "DBManager.h"
#include "FileManager.h"
#include "Controller.h"
#include "TinyWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CVS_ServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


CVS_ServerApp::CVS_ServerApp()
{	
}

CVS_ServerApp theApp;


BOOL CVS_ServerApp::CreateTinyWindow( void )
{
	BOOL bResult = TRUE;		
	
	m_TinyWnd = NULL;
	m_TinyWnd = new CTinyWindow();

	if (m_TinyWnd->InitTinyWindow( AfxGetInstanceHandle() ) == FALSE )
	{
		m_TinyWnd->ReleaseTinyWindow();
		if (m_TinyWnd != NULL)
		{
			delete m_TinyWnd;
			m_TinyWnd = NULL;
			bResult = FALSE;
			return bResult;
		}
	}
	
	m_TinyWnd->Create(::GetDesktopWindow());
	m_TinyWnd->SetEvents( (IController *)m_ControllerHandler );
	return bResult;

}

BOOL CVS_ServerApp::CreateHandlers( void )
{
	m_TCPServer = NULL;	
	m_FileManager = NULL;
	m_DBManager = NULL;
	m_GUI_DBManager = NULL;
	m_ControllerHandler = NULL;
	m_MainViewer = NULL;
	
	/* Create CTCPServer Instance */
	m_TCPServer = new CTCPServer();
	if ( m_TCPServer == NULL )
	{
		return FALSE;
	}

	/* Create CFileManager Instance */
	m_FileManager = new CFileManager();
	if ( m_FileManager == NULL )
	{
		ReleaseHandlers();
		return FALSE;
	}

	/* Create CDBManager Instance */
	m_DBManager = new CDBManager();
	m_GUI_DBManager = new CDBManager();

	if ( m_DBManager == NULL  || m_GUI_DBManager == NULL )
	{
		ReleaseHandlers();
		return FALSE;
	}

	/* Create CControllerHandler Instance */
	m_ControllerHandler = new CController();
	if ( m_ControllerHandler == NULL )
	{
		ReleaseHandlers();
		return FALSE;
	}
	
	if ( CreateTinyWindow() != TRUE )
	{
		return FALSE;
	}


	return TRUE;
}

void CVS_ServerApp::ReleaseHandlers( void )
{

	if ( m_ControllerHandler != NULL )
	{
		m_ControllerHandler->Release();

		if ( m_TinyWnd != NULL )
		{
			m_TinyWnd->ReleaseTinyWindow();
			m_TinyWnd->DestroyWindow();
			delete m_TinyWnd;
			m_TinyWnd = NULL;
		}

		if ( m_TCPServer != NULL )
		{
			delete m_TCPServer;
		}
		
		if( m_FileManager != NULL )
		{
			delete m_FileManager;
		}
		
		if ( m_DBManager != NULL )
		{
			delete m_DBManager;
		}
		
		if ( m_GUI_DBManager != NULL )
		{
			delete m_GUI_DBManager;
		}

		delete m_ControllerHandler;
	}
	
	
	if ( m_hMutex != NULL )
	{
		::ReleaseMutex( m_hMutex );
		::CloseHandle( m_hMutex );
		m_hMutex = NULL;
	}
}

BOOL CVS_ServerApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	
/* ===========================================================================
	by tory45 2010-05-17
	for CreateMutex( 동시에 서버 프로그램을 1개 이상 실행 시키지 못하도록 )
=============================================================================*/

	DWORD dwError;
	CString strMsg;
	BOOL bSuccess = FALSE;

	m_hMutex = NULL;
	m_hMutex	= ::CreateMutex( NULL, TRUE, L"VSSERVER_WIZARD" );
	dwError		= ::GetLastError();

	if( m_hMutex == NULL )
		return FALSE;

	if( dwError == ERROR_ALREADY_EXISTS ) {
		::ReleaseMutex( m_hMutex );
		::CloseHandle( m_hMutex );
		m_hMutex = NULL;

		CString strParm=m_lpCmdLine;

		if(strParm.IsEmpty() || strParm.Find(L"-setup",0) < 0)
		{			
			strMsg.Format( IDS_ERR_MUTEX );
			AfxMessageBox(strMsg);			
		}
		return FALSE;
	}

	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	AfxEnableControlContainer();
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	
/* ===============================================================
	by tory45 2010-05-17
	Start Factory, Command, iterate Pattern ( Create Objects )
==================================================================*/

 	if ( CreateHandlers() == TRUE )
	{
		/* Transfer, Model Instance 등록 */
		if ( m_ControllerHandler != NULL && m_ControllerHandler->AddTransferHandler( TRANSFER_KIND_TCP, m_TCPServer ) == TRUE ) 
		{
			if (m_ControllerHandler->AddModelHandler( MODEL_KIND_DB, m_DBManager ) == TRUE )
			{				
				if (m_ControllerHandler->AddModelHandler( MODEL_KIND_GUI_DB, m_GUI_DBManager ) == TRUE )
				{				

					if (m_ControllerHandler->AddModelHandler( MODEL_KIND_FILE, m_FileManager ) == TRUE )
					{				
						bSuccess = TRUE;
					}
				}
			}
		}

		if ( bSuccess != TRUE )
		{
			ReleaseHandlers();
			strMsg.Format( IDS_ERR_NOT_CREATE_OBJECT );
			AfxMessageBox(strMsg);			
			return FALSE;
		}
	}
	else
	{
		ReleaseHandlers();
		strMsg.Format( IDS_ERR_NOT_CREATE_OBJECT );
		AfxMessageBox(strMsg);			
		return FALSE;
	}

	CVS_ServerDlg dlg;
	m_pMainWnd = &dlg;

	/* dynamic_cast : 다중 상속에 의한 메모리 이상 현상 방지 */
#if 1
	m_MainViewer = dynamic_cast< IViewer *>( &dlg );
#else
	m_MainViewer = (IViewer *)&dlg;
#endif
	if ( m_ControllerHandler != NULL )
	{
		m_ControllerHandler->AddViewerHandler( VIEWER_KIND_MAIN, m_MainViewer );	
		m_ControllerHandler->Initialize();
	}
	

	/* Controller Instance 등록 */
	/* TinyWindow Instance 등록 */
	if ( m_TCPServer != NULL )
	{
		m_TCPServer->SetControllerHandler( m_ControllerHandler );	
		m_TCPServer->SetTinyWindow( m_TinyWnd );
	}
	if ( m_DBManager != NULL )
	{
		m_DBManager->SetControllerHandler( m_ControllerHandler );
	}
	
	if ( m_GUI_DBManager != NULL )
	{
		m_GUI_DBManager->SetControllerHandler( m_ControllerHandler );
	}

	if ( m_FileManager != NULL )
	{
		m_FileManager->SetControllerHandler( m_ControllerHandler );
	}	
		
	/* Transfer, Model Instance 등록 */
#if 0
	m_MainViewer->AddModelHandler( MODEL_KIND_DB, m_DBManager );
	m_MainViewer->AddModelHandler(MODEL_KIND_FILE, m_FileManager );
#endif	
	m_MainViewer->SetControllerHandler( m_ControllerHandler );

/* ===============================================================
End Factory , Command, iterate Pattern
==================================================================*/	

	INT_PTR nResponse = dlg.DoModal();

	if (nResponse == IDOK)
	{		
	}
	else if (nResponse == IDCANCEL)
	{	
	}
	
	ReleaseHandlers();

	if ( m_MainViewer != NULL )
	{		
		m_MainViewer = NULL;
	}
	
	return FALSE;
}
int CVS_ServerApp::ExitInstance() 
{	
	if ( m_TinyWnd != NULL )
	{
		m_TinyWnd->ReleaseTinyWindow();
		m_TinyWnd->DestroyWindow();
		delete m_TinyWnd;
		m_TinyWnd = NULL;
	}

	return CWinApp::ExitInstance();
}
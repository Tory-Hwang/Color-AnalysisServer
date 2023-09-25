// VS_Server.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include "ITransfer.h"
#include "IViewer.h"
#include "IModel.h"
#include "IController.h"
#include "ITinyWindow.h"

class CVS_ServerApp : public CWinApp
{
public:
	CVS_ServerApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

private:
	BOOL CreateHandlers( void );
	void ReleaseHandlers( void );
	BOOL CreateTinyWindow( void );

	HANDLE m_hMutex;
	ITransfer * m_TCPServer;	
	IModel * m_FileManager;
	IModel * m_DBManager;
	IModel * m_GUI_DBManager;
	IViewer * m_MainViewer;
	IController * m_ControllerHandler;
	ITinyWindow * m_TinyWnd;

	DECLARE_MESSAGE_MAP()
};

extern CVS_ServerApp theApp;
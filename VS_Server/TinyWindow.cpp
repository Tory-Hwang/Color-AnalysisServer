#include "StdAfx.h"
#include "TinyWindow.h"
#include "VS_Server.h"
#include <rpcdce.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static LRESULT CALLBACK _WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CTinyWindow * pWnd = NULL;
	
	if ( hWnd == NULL )
	{
		return 0;
	}

	pWnd = (CTinyWindow *)GetProp(hWnd, _T("ptrVS_Server"));

	
	if(pWnd && pWnd->WndProc(uMsg, wParam, lParam))
	{
		return 0;
	}	

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


CTinyWindow::CTinyWindow(void)
{
	m_pEvents			= NULL;
	m_hWnd				= NULL;
	_hTinyWindowInstance = NULL;

}

CTinyWindow::~CTinyWindow(void)
{
	DestroyWindow();
}


BOOL CTinyWindow::InitTinyWindow(HINSTANCE hInstance)
{
	if(_hTinyWindowInstance)
		return FALSE;

	_hTinyWindowInstance = hInstance;

	wchar_t * paucUUIDString;
	RPC_STATUS lUUIDResult;
	UUID oUUID;

	lUUIDResult=UuidCreate(&oUUID);
	memset(acInit_WindowClassName, 0 , sizeof(acInit_WindowClassName));
	

	if( _hTinyWindowInstance!=NULL && (lUUIDResult==RPC_S_OK || lUUIDResult==RPC_S_UUID_LOCAL_ONLY) )
	{
		if( UuidToString( &oUUID, (unsigned short **) &paucUUIDString)==RPC_S_OK )

		{
			WNDCLASSEX wc;
			wc.cbSize=sizeof(wc);
	
	
			wcscpy_s( acInit_WindowClassName, /* wcslen( L"TinyVS_Server") +1 */  L"TinyVS_Server");
			wcscat_s( acInit_WindowClassName, /* wcslen(paucUUIDString) */  paucUUIDString);
			
			wc.style			= 0;
			wc.lpfnWndProc		= (WNDPROC)_WindowProc;
			wc.cbClsExtra		= 0;
			wc.cbWndExtra		= 0;

			/*  Handle to the instance that the window procedure of this
			*  class is within. */
			wc.hInstance		= _hTinyWindowInstance;    /*  not Global class  */

			wc.hIcon			= NULL;
			wc.hIconSm			= NULL;
			wc.hCursor			= LoadCursor(NULL, IDC_ARROW);
			wc.hbrBackground	= (HBRUSH)(COLOR_WINDOW + 1);
			wc.lpszMenuName		= 0;			
			wc.lpszClassName	= acInit_WindowClassName;

			RpcStringFree((unsigned short **)&paucUUIDString);
			return RegisterClassEx(&wc) != NULL;

		}

	}
	return FALSE;

}

void CTinyWindow::ReleaseTinyWindow()
{
	if(_hTinyWindowInstance)
	{
		UnregisterClass(acInit_WindowClassName, _hTinyWindowInstance);
		_hTinyWindowInstance = NULL;
	}
	m_pEvents = NULL;
}


BOOL CTinyWindow::Create(HWND hParent, DWORD dwStyle, RECT * pRect)
{
	return CreateEx(hParent, dwStyle, 0, pRect);
}


BOOL CTinyWindow::CreateEx(HWND hParent, DWORD dwStyle, DWORD dwStyleEx, RECT * pRect)
{
	if(m_hWnd)
		return FALSE;

	RECT rtWindow = {0, 0, 0, 0};
	if(pRect)
		rtWindow = *pRect;

	m_hWnd = CreateWindowEx(dwStyleEx, 
		acInit_WindowClassName, 
		_T(""), 
		dwStyle,
		rtWindow.left, 
		rtWindow.top, 
		rtWindow.right - rtWindow.left, 
		rtWindow.bottom - rtWindow.top,
		hParent, 
		NULL, 
		_hTinyWindowInstance, 
		NULL);
	if(m_hWnd)
		SetProp(m_hWnd, _T("ptrVS_Server"), (HANDLE)this);

	return m_hWnd != NULL;
}

HWND CTinyWindow::GetHWND()
{
	return m_hWnd;
}

void CTinyWindow::DestroyWindow()
{
	if(m_hWnd)
	{
		RemoveProp(m_hWnd, _T("ptrVS_Server"));
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

void CTinyWindow::Delete()
{
	delete this;
}

void CTinyWindow::SetEvents(IController * pEvents)
{
	m_pEvents = pEvents;
}

BOOL CTinyWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

	BOOL bResult;

	bResult = FALSE;
	if( uMsg == WM_PAINT )
		return OnPaint( uMsg, wParam, lParam );

	if(m_pEvents)
	{
		if ( uMsg >= ( WM_USER + 800 ))
		{
			bResult = m_pEvents->ITinyWindow_OnWndProc(this, uMsg, wParam, lParam);
		}

		return bResult;
	}

	return FALSE;
}

BOOL CTinyWindow::OnPaint( UINT , WPARAM , LPARAM  )
{
	::ValidateRect( m_hWnd, NULL );
	return TRUE;
}

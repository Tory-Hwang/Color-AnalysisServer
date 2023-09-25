#pragma once

#include "ITinyWindow.h"
#include "IController.h"

class CTinyWindow :	public ITinyWindow
{
private:
	HWND m_hWnd;
	IController * m_pEvents;

public:
	CTinyWindow(void);
	~CTinyWindow(void);

	BOOL WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
private:
	HINSTANCE _hTinyWindowInstance;
	wchar_t acInit_WindowClassName[MAX_PATH];
protected:
	virtual BOOL Create(HWND hParent, DWORD dwStyle, RECT * pRect);
	virtual BOOL CreateEx(HWND hParent, DWORD dwStyle, DWORD dwStyleEx, RECT * pRect);
	virtual HWND GetHWND();
	virtual void DestroyWindow();
	virtual void Delete();
	virtual void SetEvents(IController * pEvents);	
	virtual BOOL OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam );
public:
	virtual BOOL InitTinyWindow(HINSTANCE hInstance);
	virtual void ReleaseTinyWindow();	

};
#pragma once
#include "IController.h"

class ITinyWindow
{
public:
	virtual BOOL Create( HWND hParent, DWORD dwStyle = WS_POPUP, RECT * pRect = NULL) = 0;
	virtual BOOL CreateEx(HWND hParent, DWORD dwStyle = WS_POPUP, DWORD dwStyleEx = 0, RECT * pRect = NULL) = 0;
	virtual HWND GetHWND() = 0;
	virtual void DestroyWindow() = 0;
	virtual void Delete() = 0;
	virtual void SetEvents(IController * pEvents) = 0;

	virtual BOOL InitTinyWindow(HINSTANCE hInstance) = 0;
	virtual void ReleaseTinyWindow() = 0;

protected:	

};
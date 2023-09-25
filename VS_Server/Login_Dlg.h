#pragma once


// CLogin_Dlg ��ȭ �����Դϴ�.
#include "Com_Def.h"
#include "MsgController.h"

class CLogin_Dlg : public CDialog, public IViewer
{
	DECLARE_DYNAMIC(CLogin_Dlg)

public:
	CLogin_Dlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CLogin_Dlg();

	/* Inheritance Interface */
	virtual BOOL SetControllerHandler( void *  ControllerHandle );
	virtual BOOL AddModelHandler( int ModelKind, void * Model_Handle );	
	virtual int OnRecvMsg( MsgInfo_t *MsgInfo );

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_Login };
private:
	
	CMsgController m_MsgController;

	void DumpStr( CString &str );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

#pragma once

#include "Com_Def.h"
#include "MsgController.h"

// CSearch_Dlg ��ȭ �����Դϴ�.

class CSearch_Dlg : public CDialog, public IViewer
{
	DECLARE_DYNAMIC(CSearch_Dlg)

public:
	CSearch_Dlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CSearch_Dlg();

	/* Inheritance Interface */
	virtual BOOL SetControllerHandler( void *  ControllerHandle );
	virtual BOOL AddModelHandler( int ModelKind, void * ModelHandle );	
	virtual int OnRecvMsg( MsgInfo_t *MsgInfo );

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_Search };

private:
	
	CMsgController m_MsgController;

	void DumpStr( CString &str );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

#pragma once


// CAdmin_Dlg ��ȭ �����Դϴ�.
#include "Com_Def.h"
#include "MsgController.h"


class CAdmin_Dlg : public CDialog, public IViewer
{
	DECLARE_DYNAMIC(CAdmin_Dlg)

public:
	CAdmin_Dlg(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CAdmin_Dlg();

	/* Inheritance Interface */
	virtual BOOL SetControllerHandler( void *  ControllerHandle );
	virtual BOOL AddModelHandler( int ModelKind, void * ModelHandle );	
	virtual int OnRecvMsg( MsgInfo_t *MsgInfo );

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_Admin };
private:
	CMsgController m_MsgController;

	void DumpStr( CString &str );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

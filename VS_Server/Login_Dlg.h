#pragma once


// CLogin_Dlg 대화 상자입니다.
#include "Com_Def.h"
#include "MsgController.h"

class CLogin_Dlg : public CDialog, public IViewer
{
	DECLARE_DYNAMIC(CLogin_Dlg)

public:
	CLogin_Dlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CLogin_Dlg();

	/* Inheritance Interface */
	virtual BOOL SetControllerHandler( void *  ControllerHandle );
	virtual BOOL AddModelHandler( int ModelKind, void * Model_Handle );	
	virtual int OnRecvMsg( MsgInfo_t *MsgInfo );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_Login };
private:
	
	CMsgController m_MsgController;

	void DumpStr( CString &str );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

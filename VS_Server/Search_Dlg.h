#pragma once

#include "Com_Def.h"
#include "MsgController.h"

// CSearch_Dlg 대화 상자입니다.

class CSearch_Dlg : public CDialog, public IViewer
{
	DECLARE_DYNAMIC(CSearch_Dlg)

public:
	CSearch_Dlg(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CSearch_Dlg();

	/* Inheritance Interface */
	virtual BOOL SetControllerHandler( void *  ControllerHandle );
	virtual BOOL AddModelHandler( int ModelKind, void * ModelHandle );	
	virtual int OnRecvMsg( MsgInfo_t *MsgInfo );

// 대화 상자 데이터입니다.
	enum { IDD = IDD_Search };

private:
	
	CMsgController m_MsgController;

	void DumpStr( CString &str );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

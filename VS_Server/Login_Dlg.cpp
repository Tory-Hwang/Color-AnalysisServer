// Login_Dlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VS_Server.h"
#include "Login_Dlg.h"




// CLogin_Dlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CLogin_Dlg, CDialog)

CLogin_Dlg::CLogin_Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogin_Dlg::IDD, pParent)
{

}

CLogin_Dlg::~CLogin_Dlg()
{
}

void CLogin_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CLogin_Dlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLogin_Dlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CLogin_Dlg 메시지 처리기입니다.

void CLogin_Dlg::DumpStr( CString &str )
{
#if DEBUG_LOGINVIEWER
	CString strtxt;
	strtxt.Format(L"VS_SERVER::CLogin_Dlg : %s \n", str);
	OutputDebugString( strtxt );
#endif
}

BOOL CLogin_Dlg::SetControllerHandler( void *  ControllerHandle )
{
	( void )ControllerHandle;
	return TRUE;
}

BOOL CLogin_Dlg::AddModelHandler( int ModelKind, void * ModelHandle )
{
	( void )ModelKind;
	( void )ModelHandle;
	return TRUE;
}

int CLogin_Dlg::OnRecvMsg( MsgInfo_t *MsgInfo )
{
	return m_MsgController.OnRecvMsg( MsgInfo );
}

void CLogin_Dlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}

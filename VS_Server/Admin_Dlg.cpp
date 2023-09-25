// Admin_Dlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VS_Server.h"
#include "Admin_Dlg.h"


// CAdmin_Dlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CAdmin_Dlg, CDialog)

CAdmin_Dlg::CAdmin_Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAdmin_Dlg::IDD, pParent)
{

}

CAdmin_Dlg::~CAdmin_Dlg()
{
}

void CAdmin_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAdmin_Dlg, CDialog)
	ON_BN_CLICKED(IDOK, &CAdmin_Dlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CAdmin_Dlg 메시지 처리기입니다.

void CAdmin_Dlg::DumpStr( CString &str )
{
#if DEBUG_ADMINVIEWER
	CString strtxt;
	strtxt.Format(L"VS_SERVER::CAdmin_Dlg : %s \n", str);
	OutputDebugString( strtxt );
#endif
}

BOOL CAdmin_Dlg::SetControllerHandler( void *  ControllerHandle )
{
	( void )ControllerHandle;
	return TRUE;
}

BOOL CAdmin_Dlg::AddModelHandler( int ModelKind, void * ModelHandle )
{
	( void )ModelKind;
	( void )ModelHandle;

	return TRUE;
}

int CAdmin_Dlg::OnRecvMsg( MsgInfo_t *MsgInfo )
{
	return m_MsgController.OnRecvMsg( MsgInfo );
}

void CAdmin_Dlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}

// Search_Dlg.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "VS_Server.h"
#include "Search_Dlg.h"



// CSearch_Dlg 대화 상자입니다.

IMPLEMENT_DYNAMIC(CSearch_Dlg, CDialog)

CSearch_Dlg::CSearch_Dlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSearch_Dlg::IDD, pParent)
{

}

CSearch_Dlg::~CSearch_Dlg()
{
}

void CSearch_Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSearch_Dlg, CDialog)
	ON_BN_CLICKED(IDOK, &CSearch_Dlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CSearch_Dlg 메시지 처리기입니다.

void CSearch_Dlg::DumpStr( CString &str )
{
#if DEBUG_SEARCHVIEWER
	CString strtxt;
	strtxt.Format(L"VS_SERVER::CSearch_Dlg : %s \n", str);
	OutputDebugString( strtxt );
#endif
}


BOOL CSearch_Dlg::SetControllerHandler( void *  ControllerHandle )
{
	( void )ControllerHandle;
	return TRUE;
}

BOOL CSearch_Dlg::AddModelHandler( int ModelKind, void * ModelHandle )
{
	( void )ModelKind;
	( void )ModelHandle;
	return TRUE;
}

int CSearch_Dlg::OnRecvMsg( MsgInfo_t *MsgInfo )
{
	return m_MsgController.OnRecvMsg( MsgInfo );
}

void CSearch_Dlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	OnOK();
}

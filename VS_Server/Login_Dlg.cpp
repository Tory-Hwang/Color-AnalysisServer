// Login_Dlg.cpp : ���� �����Դϴ�.
//

#include "stdafx.h"
#include "VS_Server.h"
#include "Login_Dlg.h"




// CLogin_Dlg ��ȭ �����Դϴ�.

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


// CLogin_Dlg �޽��� ó�����Դϴ�.

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	OnOK();
}

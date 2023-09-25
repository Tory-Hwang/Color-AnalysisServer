// VS_ServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "VS_Server.h"
#include "VS_ServerDlg.h"
#include "DB_Def.h"
#include "ADO.h"
#include "SVR_Doc.h"
#include "SortString.h"
#include "Certify.h"
#include <atlimage.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MAX_LIST_CNT	500
#define RUN_MODE_REAL	0x01
#define RUN_MODE_TEST	0x02
#define  DEFAULT_X		300
#define DEFAULT_Y		400
#define DEFAULT_IMAGE	L"emp.jpg"


/* static 함수를 위한 instance */
CVS_ServerDlg * pMainViewe = NULL;

#define REQ_CUR_IMAGE_TIMER		2000
#define REQ_CUR_IMAGE_INTERVAL	1000

#define PROCESS_TIMER			2001
#define PROCESS_TIME_INTERVAL	1000
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CVS_ServerDlg dialog


CVS_ServerDlg::CVS_ServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVS_ServerDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVS_ServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);	
	DDX_Control(pDX, IDC_SERVERIP, m_CtlServerIP);
	DDX_Control(pDX, IDC_TCPSTATE, m_TCPConnectState);	
	DDX_Control(pDX, IDC_LOGLIST, m_CtlLogList);
	DDX_Control(pDX, IDC_SERVER_ID, m_CtlServerID);
	DDX_Control(pDX, IDC_CLIENT_CNT, m_CtlClientCnt);
	DDX_Control(pDX, IDC_CLIENT_LIST, m_CtlClientList);
	DDX_Control(pDX, IDC_SELECTED_CLIENT, m_CtlSelectedID);					

	DDX_Control(pDX, IDC_CHK_SAVE, m_ctl_savelog);
	DDX_Control(pDX, IDC_TIME, m_ctl_time);
	DDX_Control(pDX, IDC_VERSION, m_ctl_version);
	DDX_Control(pDX, IDC_LIST_DISCONNECT, m_list_discon);
	DDX_Control(pDX, IDC_WACH_ALL, m_ctl_mac_all);
	DDX_Control(pDX, IDC_COM_VOL, m_com_vol);
	DDX_Control(pDX, IDC_COM_ONOFF, m_com_onoff);
	DDX_Control(pDX, IDC_LED_ALL, m_ctl_led_all);
	DDX_Control(pDX, IDC_LED_ON_CNT, m_ctl_on_cnt);
	DDX_Control(pDX, IDC_LED_OFF_CNT, m_ctl_off_cnt);
	DDX_Control(pDX, IDC_COM_GPIO_TYPE, m_com_gpio_type);
	DDX_Control(pDX, IDC_COM_GPIO_ONOFF, m_com_gpio_onoff);
	DDX_Control(pDX, IDC_E_KEY, m_ctl_key);
	DDX_Control(pDX, IDC_E_SSID, m_ctl_ssid);
	DDX_Control(pDX, IDC_E_PORT, m_ctl_port);
	DDX_Control(pDX, IDC_E_PORT2, m_ctl_udp_port);
	DDX_Control(pDX, IDC_SERVERIP2, m_ctl_updserverip);
	DDX_Control(pDX, IDC_COM_ONOFF2, m_ctlwifi_type);
	DDX_Control(pDX, IDC_COM_ONOFF3, m_ctl_testmode);
	DDX_Control(pDX, IDC_EDIT1, m_motor_pwm);
	DDX_Control(pDX, IDC_EDIT2, m_ctl_light_pwm);
	DDX_Control(pDX, IDC_CHECK2, m_ctl_image);
	DDX_Control(pDX, IDC_IMAGE_BOX2, m_ctl_img_box);
	DDX_Control(pDX, IDC_EDIT4, m_motor_time);
	DDX_Control(pDX, IDC_EDIT5, m_ctl_plate_pwm);
	DDX_Control(pDX, IDC_EDIT6, m_ctl_plate_time);
	DDX_Control(pDX, IDC_EDIT7, m_ctl_temp_pwm);
	DDX_Control(pDX, IDC_EDIT8, m_ctl_temp_time);
	DDX_Control(pDX, IDC_COM_ONOFF4, m_mult_onoff);
	DDX_Control(pDX, IDC_EDIT9, m_plt1_pwm);
	DDX_Control(pDX, IDC_EDIT10, m_plt1_time);
	DDX_Control(pDX, IDC_EDIT11, m_plt2_pwm);
	DDX_Control(pDX, IDC_EDIT12, m_plt2_time);
	DDX_Control(pDX, IDC_COM_ONOFF5, m_mullt1_onoff);
	DDX_Control(pDX, IDC_EDIT13, m_plt2_1_pwm);
	DDX_Control(pDX, IDC_EDIT14, m_plt2_1_time);
	DDX_Control(pDX, IDC_EDIT15, m_plt2_2_pwm);
	DDX_Control(pDX, IDC_EDIT16, m_plt2_2_time);
	DDX_Control(pDX, IDC_COM_ONOFF6, m_mullt2_onoff);
	DDX_Control(pDX, IDC_EDIT17, m_plt3_1_pwm);
	DDX_Control(pDX, IDC_EDIT18, m_plt3_1_time);
	DDX_Control(pDX, IDC_EDIT19, m_plt3_2_pwm);
	DDX_Control(pDX, IDC_EDIT20, m_plt3_2_time);
	DDX_Control(pDX, IDC_COM_ONOFF7, m_mullt3_onoff);
	DDX_Control(pDX, IDC_EDIT21, m_plt4_1_pwm);
	DDX_Control(pDX, IDC_EDIT22, m_plt4_1_time);
	DDX_Control(pDX, IDC_EDIT23, m_plt4_2_pwm);
	DDX_Control(pDX, IDC_EDIT24, m_plt4_2_time);
	DDX_Control(pDX, IDC_COM_ONOFF8, m_mullt4_onoff);
	DDX_Control(pDX, IDC_EDIT25, m_plt5_1_pwm);
	DDX_Control(pDX, IDC_EDIT26, m_plt5_1_time);
	DDX_Control(pDX, IDC_EDIT27, m_plt5_2_pwm);
	DDX_Control(pDX, IDC_EDIT28, m_plt5_2_time);
	DDX_Control(pDX, IDC_COM_ONOFF9, m_mullt5_onoff);
	DDX_Control(pDX, IDC_COM_ONOFF10, m_mullt2_1_onoff);
	DDX_Control(pDX, IDC_COM_ONOFF11, m_mullt2_2_onoff);
	DDX_Control(pDX, IDC_COM_ONOFF12, m_mullt2_3_onoff);
	DDX_Control(pDX, IDC_COM_ONOFF13, m_mullt2_4_onoff);
	DDX_Control(pDX, IDC_COM_ONOFF14, m_mullt2_5_onoff);
	DDX_Control(pDX, IDC_COM_ONOFF15, m_mult2_onoff);
	DDX_Control(pDX, IDC_PLAY_AUDIO, m_kind_audio);
	DDX_Control(pDX, IDC_EDIT3, m_ctl_img_num);
	DDX_Control(pDX, IDC_E_AWS_PORT, m_ctl_aws_port);
	DDX_Control(pDX, C_E_AWS_HOST, m_ctl_aws_host);
	DDX_Control(pDX, C_E_AWS_ID, m_ctl_aws_id);
	DDX_Control(pDX, IDC_PUB, m_ctl_pub);
	DDX_Control(pDX, IDC_DEV_NUM, m_ctl_dev_num);
	DDX_Control(pDX, IDC_EDIT29, m_plt6_1_pwm);
	DDX_Control(pDX, IDC_EDIT31, m_plt6_2_pwm);
	DDX_Control(pDX, IDC_EDIT33, m_plt7_1_pwm);
	DDX_Control(pDX, IDC_EDIT35, m_plt7_2_pwm);
	DDX_Control(pDX, IDC_EDIT37, m_plt8_1_pwm);
	DDX_Control(pDX, IDC_EDIT39, m_plt8_2_pwm);
	DDX_Control(pDX, IDC_EDIT30, m_plt6_1_time);
	DDX_Control(pDX, IDC_EDIT32, m_plt6_2_time);
	DDX_Control(pDX, IDC_EDIT34, m_plt7_1_time);
	DDX_Control(pDX, IDC_EDIT36, m_plt7_2_time);
	DDX_Control(pDX, IDC_EDIT38, m_plt8_1_time);
	DDX_Control(pDX, IDC_EDIT40, m_plt8_2_time);
	DDX_Control(pDX, IDC_COM_ONOFF16, m_mullt6_onoff);
	DDX_Control(pDX, IDC_COM_ONOFF19, m_mullt2_6_onoff);
	DDX_Control(pDX, IDC_COM_ONOFF17, m_mullt7_onoff);
	DDX_Control(pDX, IDC_COM_ONOFF20, m_mullt2_7_onoff);
	DDX_Control(pDX, IDC_COM_ONOFF18, m_mullt8_onoff);
	DDX_Control(pDX, IDC_COM_ONOFF21, m_mullt2_8_onoff);
	DDX_Control(pDX, IDC_EDIT41, m_ctrl_org_file);
	DDX_Control(pDX, IDC_EDIT42, m_ctrl_new_file);
}


BOOL CVS_ServerDlg::PreTranslateMessage(MSG* pMsg) 
{
	if ( (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)  || (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE) )
		return TRUE;


	return CDialog::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CVS_ServerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CVS_ServerDlg::OnBnClickedBtnClose)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CVS_ServerDlg::OnBnClickedBtnClear)
	ON_BN_CLICKED(IDC_BTN_ETC3, &CVS_ServerDlg::OnBnClickedBtnEtc3)	
	ON_NOTIFY(NM_CLICK, IDC_CLIENT_LIST, &CVS_ServerDlg::OnNMClickClientList)	
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_CLIENT_LIST, &CVS_ServerDlg::OnLvnItemchangedClientList)
	ON_NOTIFY(NM_DBLCLK, IDC_CLIENT_LIST, &CVS_ServerDlg::OnNMDblclkClientList)		
	ON_BN_CLICKED(IDC_BTN_ETC4, &CVS_ServerDlg::OnBnClickedBtnEtc4)
	ON_BN_CLICKED(IDC_BTN_ETC5, &CVS_ServerDlg::OnBnClickedBtnEtc5)
	ON_BN_CLICKED(IDC_BTN_ETC6, &CVS_ServerDlg::OnBnClickedBtnEtc6)
	ON_BN_CLICKED(IDC_CHK_SAVE, &CVS_ServerDlg::OnBnClickedChkSave)
	ON_BN_CLICKED(IDC_BTN_ETC7, &CVS_ServerDlg::OnBnClickedBtnEtc7)
	ON_BN_CLICKED(IDC_BTN_WATCH, &CVS_ServerDlg::OnBnClickedBtnWatch)
	ON_BN_CLICKED(IDC_BTN_MAC, &CVS_ServerDlg::OnBnClickedBtnMac)
	ON_BN_CLICKED(IDC_BTN_SORT, &CVS_ServerDlg::OnBnClickedBtnSort)
	ON_BN_CLICKED(IDC_BTN_VOL, &CVS_ServerDlg::OnBnClickedBtnVol)
	ON_BN_CLICKED(IDC_BTN_LED_GET, &CVS_ServerDlg::OnBnClickedBtnLedGet)
	ON_BN_CLICKED(IDC_BTN_LED_SET, &CVS_ServerDlg::OnBnClickedBtnLedSet)
	ON_BN_CLICKED(IDC_BTN_CALL_READY, &CVS_ServerDlg::OnBnClickedBtnCallReady)
	ON_BN_CLICKED(IDC_BTN_GPIO_GET, &CVS_ServerDlg::OnBnClickedBtnGpioGet)
	ON_BN_CLICKED(IDC_BTN_GPIO_SET, &CVS_ServerDlg::OnBnClickedBtnGpioSet)
	ON_CBN_SELCHANGE(IDC_COM_GPIO_TYPE, &CVS_ServerDlg::OnCbnSelchangeComGpioType)
	ON_CBN_SELCHANGE(IDC_COM_ONOFF2, &CVS_ServerDlg::OnCbnSelchangeComOnoff2)
	ON_BN_CLICKED(IDC_BTN_ETC8, &CVS_ServerDlg::OnBnClickedBtnEtc8)
	ON_BN_CLICKED(IDC_BUTTON3, &CVS_ServerDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_BUTTON4, &CVS_ServerDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BTN_CALL_READY_AWS, &CVS_ServerDlg::OnBnClickedBtnCallReadyAws)
	ON_BN_CLICKED(IDC_BTN_ETC9, &CVS_ServerDlg::OnBnClickedBtnEtc9)
	ON_BN_CLICKED(IDC_BUTTON5, &CVS_ServerDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON7, &CVS_ServerDlg::OnBnClickedButton7)
	ON_BN_CLICKED(IDC_BUTTON6, &CVS_ServerDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON8, &CVS_ServerDlg::OnBnClickedButton8)
	ON_BN_CLICKED(IDC_BUTTON9, &CVS_ServerDlg::OnBnClickedButton9)
END_MESSAGE_MAP()


// CVS_ServerDlg message handlers

BOOL CVS_ServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	InitializeCriticalSection(&_DLGLock);

	//ShowWindow(SW_MAXIMIZE);
	SetTimer( INIT_DLG_TIMER , 500, NULL );
	SetTimer( IDLE_DLG_TIMER, IDLE_DLG_INTERVAL, NULL );

	/* MsgController 등록 */
	pMainViewe = this;

	m_MsgController.AddClientRecvFunctions( SOCK_SEND_IMAG_MSG, ClientRecvImg);
	m_MsgController.AddClientRecvFunctions( SOCK_TEST_PLATE_GET_MSG, ClientRecvPlate);
	m_MsgController.AddClientRecvFunctions( SOCK_TEST_LOG_MSG, ClientRecvLOG);
	m_MsgController.AddClientRecvFunctions( SOCK_ADD_MSG, ClientAdd );	
	m_MsgController.AddDummyFunctions( DUMMY_MSG_CLOSE_TCP, DummyTCPCloseMsg);	
	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAboutDlg::OnClose()
{	

	CDialog::OnClose();
}


void CVS_ServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.


int CVS_ServerDlg::Lock()
{
	EnterCriticalSection(&_DLGLock);
	return 0;
}

int CVS_ServerDlg::UnLock()
{
	LeaveCriticalSection(&_DLGLock);	
	return 0;
}

void CVS_ServerDlg::OnPaint()
{
//	int i;
	int X, Y ;
	DWORD dwWidth,dwHeight;
	CRect rectClient;
	CString strMsg;
	TCHAR csYear[ 6 ];
	TCHAR csMon[ 6 ];
	TCHAR csDay[ 6 ];
	TCHAR csHour[ 6 ];
	TCHAR csMin[ 6 ];
	TCHAR csSec[ 6 ];
	CString strPos;
//	int X_Interval =0;
//  int Y_Interval =0;

//	int LineX, LineY, LineWidth, LineHeight;
	CPoint TextPoint;
	

	GetClientRect(rectClient);

	X = Y = dwWidth = dwHeight =0;

	CPaintDC dc(this); // device context for painting
	if (IsIconic())
	{
		
		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);		
	}
	else
	{	
		memset( csYear, 0, sizeof( csYear ));
		memset( csMon, 0, sizeof( csYear ));
		memset( csDay, 0, sizeof( csYear ));

		memset( csHour, 0, sizeof( csYear ));
		memset( csMin, 0, sizeof( csYear ));
		memset( csSec, 0, sizeof( csYear ));

		CClientDC TempDC( this );
//		CPen xpen, *pxOldPen;
//		CPen ypen, *pyOldPen;
//		int k;
#if 0
		for( i = 0 ; i < MAX_IMAGE_VIEWER_CNT ;i++ )
		{
			if ( wcslen( m_ImageViewer[ i ].csFullPath ) > 0 )
			{
				if ( m_ImageViewer[ i ].cxImage.Load( m_ImageViewer[ i ].csFullPath, CXIMAGE_FORMAT_JPG) == TRUE )
				{
					Y = rectClient.top + (( m_ImageViewer[ i ].cxImage.GetHeight() * i ) + 100 + ( i * 40 ) );
					X = rectClient.left + 270;

					dwHeight = m_ImageViewer[ i ].cxImage.GetHeight();
					dwWidth = m_ImageViewer[ i ].cxImage.GetWidth();
					m_ImageViewer[ i ].cxImage.Draw(dc.m_hDC, X, Y, dwWidth, dwHeight, 0, TRUE);					
					
					if ( m_LineCnt > 1 )
					{
						Y_Interval = dwHeight / ( m_LineCnt  );
						ypen.CreatePen(PS_SOLID, 1, RGB(255, 51, 0));
						pyOldPen = TempDC.SelectObject(&ypen);					
						for(  k = 0 ; k < m_LineCnt + 1 ;k++)
						{
							LineX = X;
							LineY = Y + ( k * Y_Interval );
							LineWidth = ( X + dwWidth  );
							LineHeight = Y + (  k * Y_Interval );

							strPos.Format( L"%d", k * Y_Interval );
							TempDC.SetTextColor( RGB(255,0,0) );
							TempDC.SetBkColor( RGB(128,68,0) );
							TempDC.SetBkMode( TRANSPARENT );
							TempDC.TextOut( LineX - 25, LineY - 15, strPos );

							if ( k > 0  && k < m_LineCnt )
							{
								TempDC.MoveTo(LineX , LineY);
								TempDC.LineTo( LineWidth , LineHeight );
							}
						}
						
						TempDC.SelectObject(pyOldPen);
						ypen.DeleteObject();
					}

					if ( m_XLineCnt > 1 )
					{
						X_Interval = dwWidth / ( m_XLineCnt  );
						xpen.CreatePen(PS_SOLID, 1, RGB(255, 51, 0));
						pxOldPen = TempDC.SelectObject(&xpen);					

						for( k = 0 ; k < m_XLineCnt + 1 ;k++)
						{
							LineX = X + ( k * X_Interval );
							LineY = Y ;
							LineWidth = X + ( k * X_Interval ) ;
							LineHeight = Y + dwHeight ;

							strPos.Format( L"%d", k * X_Interval );
							TempDC.SetTextColor( RGB(255,0,0) );
							TempDC.SetBkColor( RGB(128,68,0) );
							TempDC.SetBkMode( TRANSPARENT );
							TempDC.TextOut( LineX -15, LineY + dwHeight +2, strPos );

							if ( k > 0  && k < m_XLineCnt )
							{
								TempDC.MoveTo(LineX , LineY);
								TempDC.LineTo( LineWidth , LineHeight );
							}
						}

						TempDC.SelectObject(pxOldPen);
						xpen.DeleteObject();
					}

					if (wcslen( m_ImageViewer[ i ].csSeeTime ) > 0 )
					{
						strMsg.Format(L"%d X %d", dwWidth, dwHeight );
						m_CtlImgSize.SetWindowText( strMsg );

						memcpy( csYear, &m_ImageViewer[ i ].csSeeTime[ 0 ], 4 * 2 );
						memcpy( csMon, &m_ImageViewer[ i ].csSeeTime[ 4 ], 2 * 2);
						memcpy( csDay, &m_ImageViewer[ i ].csSeeTime[ 6  ],  2 * 2);

						memcpy( csHour, &m_ImageViewer[ i ].csSeeTime[ 8], 2 * 2);
						memcpy( csMin, &m_ImageViewer[ i ].csSeeTime[ 10], 2 * 2);
						memcpy( csSec, &m_ImageViewer[ i ].csSeeTime[ 12], 2 * 2);

						strMsg.Format(L"%s-%s-%s %s:%s:%s", csYear, csMon, csDay , csHour, csMin, csSec);
						
					}
					
				}
			}		
		}
#endif
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVS_ServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CVS_ServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	if ( nIDEvent == INIT_DLG_TIMER )
	{
		KillTimer( INIT_DLG_TIMER );
		InitDlg();
	}
	else if ( nIDEvent == IDLE_DLG_TIMER )
	{
		KillTimer( IDLE_DLG_TIMER );
		ChkLEDCnt();
		SetTimer( IDLE_DLG_TIMER, IDLE_DLG_INTERVAL, NULL );
	}
	else if ( nIDEvent == PROCESS_TIMER )
	{
		Set_Processtime();
	}

	__super::OnTimer(nIDEvent);
}
void CVS_ServerDlg::OnClose()
{	

	m_Controller->Close();
	Sleep( 2000 );

	m_Red.Detach();
	m_Green.Detach();
	m_ImgClass.Proc_Release();
	
	DeleteCriticalSection(&_DLGLock);
	m_MsgController.ReleaseFuncstions();

	m_Util.KillProcess(L"VS_Server.exe");
	
	ReleaseDlg();

	if ( m_dwThread != 0 )
	{
		m_dwThread	= 0;
		::WaitForSingleObject( m_hThread, 500);
		CloseHandle( m_hThread );
		m_hThread	= NULL;		
	}

	__super::OnClose();
}


/* 녹음 요청 */
void CVS_ServerDlg::OnBnClickedBtnEtc3()
{	
	ClientSendMsg_t ClientSendMsg;

	if ( isCheckSelected() == FALSE )
	{
		return;
	}

	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	ClientSendMsg.Msg   = SOCK_TEST_REC_REQ_MSG;

	ClientSendMsg.SIPID = m_SelectedID;
	
	if ( m_Message.MakeReqTEST_RECODER(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
	}
}


/* 음원 플레이 */
void CVS_ServerDlg::OnBnClickedBtnEtc6()
{
	ClientSendMsg_t ClientSendMsg;
	short kind ;
	BYTE audio_kind ;

	if ( isCheckSelected() == FALSE )
	{
		return;
	}

	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	ClientSendMsg.Msg   = SOCK_TEST_PLAY_REQ_MSG;

	audio_kind = (BYTE)m_kind_audio.GetCurSel();
	/*
	m_kind_audio.AddString(L"버튼");
	m_kind_audio.AddString(L"배변감지");
	m_kind_audio.AddString(L"LOCK");
	m_kind_audio.AddString(L"녹취");		

	초기값 beep 버튼 요청 
	버튼		: 0x0102
	배변감지	: 0x0103
	lock상태	: 0x0104
	녹취파일	: 0x0105
	*/

	kind = 0x0102;
	
	/* 버튼beep*/
	kind += audio_kind; 
	
	ClientSendMsg.SIPID = m_SelectedID;
	
	if ( m_Message.MakeReqTEST_PLAY(ClientSendMsg.Msg, m_SelectedID ,kind, &ClientSendMsg) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
	}
}

/* 카메라 캡처 */
void CVS_ServerDlg::OnBnClickedBtnEtc7()
{
	ClientSendMsg_t ClientSendMsg;

	if ( isCheckSelected() == FALSE )
	{
		return;
	}

	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	ClientSendMsg.Msg   = SOCK_TEST_CAM_REQ_MSG;

	ClientSendMsg.SIPID = m_SelectedID;
	
	if ( m_Message.MakeReqTEST_CAPTURE(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
	}
	AfxMessageBox(L"20초 후 이미지가 전송됩니다.!!");

}

/* TEST MODE */
void CVS_ServerDlg::OnBnClickedBtnEtc5()
{
	ClientSendMsg_t ClientSendMsg;		
	short mode = 0;


	if ( isCheckSelected() == FALSE )
	{
		return;
	}

	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	ClientSendMsg.Msg   = SOCK_TEST_RUN_REQ_MSG;

	ClientSendMsg.SIPID = m_SelectedID;
	mode = (short)m_ctl_testmode.GetCurSel();

	mode = pow(2, mode);

	if ( m_Message.MakeReqTEST_MODE(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg, mode ) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
	}
}


/* PUB 송출 */
void CVS_ServerDlg::OnBnClickedBtnEtc9()
{
	ClientSendMsg_t ClientSendMsg;
	short cmd;
	BYTE dev_num = 0;

	if ( isCheckSelected() == FALSE )
	{
		return;
	}

	cmd = (short)m_ctl_pub.GetCurSel()+1;	
	dev_num = ( BYTE ) m_ctl_dev_num.GetCurSel() +1;

	if ( cmd == AWS_PUB_WIFI + 1  )
	{
		cmd = AWS_PUB_UPDATE;
	}
	else if ( cmd == AWS_PUB_WIFI + 2 )
	{
		cmd = AWS_PUB_KIT_UPDATE;
	}
		
	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	

	ClientSendMsg.Msg   = SOCK_TEST_AWS_PUB_REQ_MSG;
	ClientSendMsg.SIPID = m_SelectedID;
		
	if ( m_Message.MakeReqTEST_PUB(ClientSendMsg.Msg, m_SelectedID , dev_num,cmd, &ClientSendMsg) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
	}
}



/* LED 제어  */
void CVS_ServerDlg::OnBnClickedBtnEtc4()
{
	ClientSendMsg_t ClientSendMsg;
	led_info_t led_info;
	BYTE led_kind;
	BYTE onoff;

	if ( isCheckSelected() == FALSE )
	{
		return;
	}

	led_kind = (BYTE)m_com_gpio_type.GetCurSel()+1;
	onoff = (BYTE)m_com_gpio_onoff.GetCurSel();

	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	memset(&led_info, 0, sizeof( led_info ));

	led_info.cnt = 1;
	led_info.list[ 0 ].kind = led_kind;
	led_info.list[ 0 ].color = 1;
	led_info.list[ 0 ].onoff = onoff;

	ClientSendMsg.Msg   = SOCK_TEST_LED_REQ_MSG;
	ClientSendMsg.SIPID = m_SelectedID;
	
	if ( m_Message.MakeReqTEST_LED(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg, &led_info ) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
	}
}


/* 모터 제어  */
void CVS_ServerDlg::OnBnClickedBtnWatch()
{
	ClientSendMsg_t ClientSendMsg;
	motor_info_t motor_info;
	BYTE motor_kind;
	BYTE onoff;

	TCHAR strpwm[ 10 ];
	char szpwm[ 10 ];
	BYTE pwm = 0;

	TCHAR strtime[ 10 ];
	char sztime[ 10 ];
	BYTE time = 0;

	if ( isCheckSelected() == FALSE )
	{
		return;
	}

	motor_kind = (BYTE)m_com_vol.GetCurSel()+1;
	onoff = (BYTE)m_com_onoff.GetCurSel();

	/* pwm */
	memset( strpwm, 0, sizeof( strpwm ));
	memset( szpwm, 0, sizeof( szpwm ));
	m_motor_pwm.GetWindowText( strpwm,sizeof( strpwm ));
	m_Util.WcsToMcs(strpwm, szpwm, wcslen( strpwm ));
	pwm = (BYTE)atoi( szpwm);

	/* time */
	memset( strtime, 0, sizeof( strtime ));
	memset( sztime, 0, sizeof( sztime ));
	m_motor_time.GetWindowText( strtime,sizeof( strtime ));
	m_Util.WcsToMcs(strtime, sztime, wcslen( strtime ));
	time = (BYTE)atoi( sztime);
	
	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	memset(&motor_info, 0, sizeof( motor_info ));

	motor_info.cnt = 1;
	motor_info.list[ 0 ].kind = motor_kind;	
	motor_info.list[ 0 ].onoff = onoff;
	motor_info.list[ 0 ].pwm = pwm;
	motor_info.list[ 0 ].time = time;

	ClientSendMsg.Msg   = SOCK_TEST_MOTOR_REQ_MSG;
	ClientSendMsg.SIPID = m_SelectedID;
	
	if ( m_Message.MakeReqTEST_MOTOR(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg, &motor_info ) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
	}

}

void CVS_ServerDlg::OnBnClickedButton3()
{
	ClientSendMsg_t ClientSendMsg;
	motor_info_t motor_info;
	
	TCHAR strpwm[ 10 ];
	char szpwm[ 10 ];
	BYTE pwm = 0;

	TCHAR strtime[ 10 ];
	char sztime[ 10 ];
	BYTE time = 0;
	BYTE onoff1;
	BYTE onoff2;

#if 1
	if ( isCheckSelected() == FALSE )
	{
		return;
	}
#endif

	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	memset(&motor_info, 0, sizeof( motor_info ));
	onoff1 = (BYTE)m_mult_onoff.GetCurSel();
	onoff2 = (BYTE)m_mult2_onoff.GetCurSel();


	motor_info.cnt = 2;
	/********************* PLATE 1*******************/
	motor_info.list[ 0 ].kind = 1;	
	motor_info.list[ 0 ].onoff = onoff1;
	/* PLATE pwm */
	memset( strpwm, 0, sizeof( strpwm ));
	memset( szpwm, 0, sizeof( szpwm ));
	m_ctl_plate_pwm.GetWindowText( strpwm,sizeof( strpwm ));
	m_Util.WcsToMcs(strpwm, szpwm, wcslen( strpwm ));
	pwm = (BYTE)atoi( szpwm );

	/* PLATE time */
	memset( strtime, 0, sizeof( strtime ));
	memset( sztime, 0, sizeof( sztime ));
	m_ctl_plate_time.GetWindowText( strtime,sizeof( strtime ));
	m_Util.WcsToMcs(strtime, sztime, wcslen( strtime ));
	time = (BYTE)atoi( sztime);
	motor_info.list[ 0 ].pwm = pwm;
	motor_info.list[ 0 ].time = time;

	/********************* PLATE 2 *******************/

	motor_info.list[ 1 ].kind = 2;	
	motor_info.list[ 1 ].onoff = onoff2;
	/* TEMP pwm */
	memset( strpwm, 0, sizeof( strpwm ));
	memset( szpwm, 0, sizeof( szpwm ));
	m_ctl_temp_pwm.GetWindowText( strpwm,sizeof( strpwm ));
	m_Util.WcsToMcs(strpwm, szpwm, wcslen( strpwm ));
	pwm = (BYTE)atoi( szpwm );

	/* TEMP time */
	memset( strtime, 0, sizeof( strtime ));
	memset( sztime, 0, sizeof( sztime ));
	m_ctl_temp_time.GetWindowText( strtime,sizeof( strtime ));
	m_Util.WcsToMcs(strtime, sztime, wcslen( strtime ));
	time = (BYTE)atoi( sztime);
	motor_info.list[ 1 ].pwm = pwm;
	motor_info.list[ 1 ].time = time;
	
	ClientSendMsg.Msg   = SOCK_TEST_MOTOR_REQ_MSG;
	ClientSendMsg.SIPID = m_SelectedID;
	
	if ( m_Message.MakeReqTEST_MOTOR(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg, &motor_info ) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
	}
}

int CVS_ServerDlg::SendMulty_Motor( TCHAR * ptrPWM1, TCHAR *ptrTime1, TCHAR * ptrPWM2, TCHAR *ptrTime2, BYTE bonoff1, BYTE bonoff2 )
{
	ClientSendMsg_t ClientSendMsg;
	motor_info_t motor_info;
	
	TCHAR strpwm[ 10 ];
	char szpwm[ 10 ];
	BYTE pwm = 0;

	TCHAR strtime[ 10 ];
	char sztime[ 10 ];
	BYTE time = 0;
	
	int max_time =0;

	if ( ptrPWM1 == NULL || ptrTime1 == NULL || ptrPWM2 == NULL || ptrTime2 == NULL )
	{
		AfxMessageBox(L"Data Fail");
		return -1;
	}
	
	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	memset(&motor_info, 0, sizeof( motor_info ));
	


	motor_info.cnt = 2;
	/********************* PLATE1 *******************/
	motor_info.list[ 0 ].kind = 1;	
	motor_info.list[ 0 ].onoff = bonoff1;

	/* PLATE pwm */
	memset( strpwm, 0, sizeof( strpwm ));
	memset( szpwm, 0, sizeof( szpwm ));	
	m_Util.WcsToMcs(ptrPWM1, szpwm, wcslen( ptrPWM1 ));
	pwm = (BYTE)atoi( szpwm );

	/* PLATE time */
	memset( strtime, 0, sizeof( strtime ));
	memset( sztime, 0, sizeof( sztime ));	
	m_Util.WcsToMcs(ptrTime1, sztime, wcslen( ptrTime1 ));
	time = (BYTE)atoi( sztime);
	motor_info.list[ 0 ].pwm = pwm;
	motor_info.list[ 0 ].time = time;

	if ( time > max_time )
		max_time = time;

	/********************* PLATE2 *******************/

	motor_info.list[ 1 ].kind = 2;	
	motor_info.list[ 1 ].onoff = bonoff2;
	/* TEMP pwm */
	memset( strpwm, 0, sizeof( strpwm ));
	memset( szpwm, 0, sizeof( szpwm ));
	
	m_Util.WcsToMcs(ptrPWM2, szpwm, wcslen( ptrPWM2 ));
	pwm = (BYTE)atoi( szpwm );

	/* TEMP time */
	memset( strtime, 0, sizeof( strtime ));
	memset( sztime, 0, sizeof( sztime ));
	
	m_Util.WcsToMcs(ptrTime2, sztime, wcslen( ptrTime2 ));
	time = (BYTE)atoi( sztime);
	motor_info.list[ 1 ].pwm = pwm;
	motor_info.list[ 1 ].time = time;
	
	if ( time > max_time )
		max_time = time;
		
	ClientSendMsg.Msg   = SOCK_TEST_MOTOR_REQ_MSG;
	ClientSendMsg.SIPID = m_SelectedID;
	
	if ( m_Message.MakeReqTEST_MOTOR(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg, &motor_info ) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
	}

	max_time +=2;

	return max_time;
}

int CVS_ServerDlg::SendResult_TCP( int Err, BYTE * ptrResult )
{
	ClientSendMsg_t ClientSendMsg;	
	

	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	
	ClientSendMsg.Msg   = SOCK_TEST_STRIP_RESULT_MSG;
	ClientSendMsg.SIPID = m_SelectedID;

	if ( m_Message.MakeTEST_STRIP_RESULT(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg, MAX_LPR_CHAR_CNT, ptrResult ) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
	}

	return ERROR_SUCCESS;
}



/* 정보 가져오기  */
void CVS_ServerDlg::OnBnClickedBtnLedSet()
{
	ClientSendMsg_t ClientSendMsg;		

	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	
	ClientSendMsg.Msg   = SOCK_TEST_PLATE_GET_MSG;
	ClientSendMsg.SIPID = m_SelectedID;

	if ( m_Message.MakeTEST_PLATE_GET(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg ) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
	}

	
	return ;
}


void CVS_ServerDlg::OnBnClickedButton4()
{
	
	TCHAR strpwm1[ 10 ];	
	TCHAR strtime1[ 10 ];	

	TCHAR strpwm2[ 10 ];	
	TCHAR strtime2[ 10 ];	
	
	BYTE onoff1;
	BYTE onoff2;

	int delay_time = 0;
#if 1
	if ( isCheckSelected() == FALSE )
	{
		return;
	}
#endif

	/* 1번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	delay_time = 0;

	m_plt1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt2_time.GetWindowText( strtime2, sizeof( strtime2 ));

	onoff1 = (BYTE)m_mullt1_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_1_onoff.GetCurSel();

	delay_time = SendMulty_Motor( strpwm1,strtime1,strpwm2,strtime2,onoff1, onoff2 );

	if (delay_time > 0 )
		Sleep( delay_time * 100 );


	/* 2번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	delay_time = 0;

	m_plt2_1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt2_1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt2_2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt2_2_time.GetWindowText( strtime2, sizeof( strtime2 ));
	onoff1 = (BYTE)m_mullt2_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_2_onoff.GetCurSel();


	delay_time = SendMulty_Motor( strpwm1,strtime1,strpwm2,strtime2,onoff1, onoff2 );

	if (delay_time > 0 )
		Sleep( delay_time * 100 );

	/* 3번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	delay_time = 0;

	m_plt3_1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt3_1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt3_2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt3_2_time.GetWindowText( strtime2, sizeof( strtime2 ));
	onoff1 = (BYTE)m_mullt3_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_3_onoff.GetCurSel();


	delay_time = SendMulty_Motor( strpwm1,strtime1,strpwm2,strtime2,onoff1, onoff2 );

	if (delay_time > 0 )
		Sleep( delay_time * 100 );


	/* 4번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	delay_time = 0;

	m_plt4_1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt4_1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt4_2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt4_2_time.GetWindowText( strtime2, sizeof( strtime2 ));
	onoff1 = (BYTE)m_mullt4_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_4_onoff.GetCurSel();

	delay_time = SendMulty_Motor( strpwm1,strtime1,strpwm2,strtime2,onoff1, onoff2 );

	if (delay_time > 0 )
		Sleep( delay_time * 100 );


	/* 5번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	delay_time = 0;

	m_plt5_1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt5_1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt5_2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt5_2_time.GetWindowText( strtime2, sizeof( strtime2 ));
	onoff1 = (BYTE)m_mullt5_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_5_onoff.GetCurSel();

	delay_time = SendMulty_Motor( strpwm1,strtime1,strpwm2,strtime2,onoff1, onoff2 );

	if (delay_time > 0 )
		Sleep( delay_time * 100 );

}


void CVS_ServerDlg::OnBnClickedButton7()
{
	TCHAR strpwm1[ 10 ];	
	TCHAR strtime1[ 10 ];	

	TCHAR strpwm2[ 10 ];	
	TCHAR strtime2[ 10 ];	
	
	BYTE onoff1;
	BYTE onoff2;

	int delay_time = 0;
#if 1
	if ( isCheckSelected() == FALSE )
	{
		return;
	}
#endif

	/* 6번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	delay_time = 0;

	m_plt6_1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt6_1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt6_2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt6_2_time.GetWindowText( strtime2, sizeof( strtime2 ));
	onoff1 = (BYTE)m_mullt6_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_6_onoff.GetCurSel();


	delay_time = SendMulty_Motor( strpwm1,strtime1,strpwm2,strtime2,onoff1, onoff2 );

	if (delay_time > 0 )
		Sleep( delay_time * 100 );

	/* 7번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	delay_time = 0;

	m_plt7_1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt7_1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt7_2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt7_2_time.GetWindowText( strtime2, sizeof( strtime2 ));
	onoff1 = (BYTE)m_mullt7_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_7_onoff.GetCurSel();


	delay_time = SendMulty_Motor( strpwm1,strtime1,strpwm2,strtime2,onoff1, onoff2 );

	if (delay_time > 0 )
		Sleep( delay_time * 100 );


	/* 8번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	delay_time = 0;

	m_plt8_1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt8_1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt8_2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt8_2_time.GetWindowText( strtime2, sizeof( strtime2 ));
	onoff1 = (BYTE)m_mullt8_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_8_onoff.GetCurSel();

	delay_time = SendMulty_Motor( strpwm1,strtime1,strpwm2,strtime2,onoff1, onoff2 );

	if (delay_time > 0 )
		Sleep( delay_time * 100 );
}


void CVS_ServerDlg::OnBnClickedButton5()
{
	TCHAR strpwm1[ 10 ];	
	TCHAR strtime1[ 10 ];	
	char szpwm1[ 10 ];
	char sztime1[ 10 ];

	TCHAR strpwm2[ 10 ];	
	TCHAR strtime2[ 10 ];	
	char szpwm2[ 10 ];
	char sztime2[ 10 ];

	BYTE onoff1;
	BYTE onoff2;	
		
	BYTE time1 = 0;
	BYTE pwm1 = 0;
	BYTE time2 = 0;
	BYTE pwm2 = 0;

	BYTE buf[ 50 ];
	int pos =0;
	

	memset( buf, 0, sizeof( buf ));

	buf[ pos++ ] = 1;

	/* 1번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	
	memset( szpwm1, 0, sizeof( szpwm1 ));		
	memset( sztime1, 0, sizeof( sztime1 ));
	memset( szpwm2, 0, sizeof( szpwm2 ));		
	memset( sztime2, 0, sizeof( sztime2 ));

	onoff1 = (BYTE)m_mullt1_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_1_onoff.GetCurSel();
	m_plt1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt2_time.GetWindowText( strtime2, sizeof( strtime2 ));

	m_Util.WcsToMcs(strpwm1, szpwm1, wcslen( strpwm1 ));
	m_Util.WcsToMcs(strtime1, sztime1, wcslen( strtime1 ));
	m_Util.WcsToMcs(strpwm2, szpwm2, wcslen( strpwm2 ));
	m_Util.WcsToMcs(strtime2, sztime2, wcslen( strtime2 ));
		

	pwm1 = (BYTE)atoi( szpwm1 );
	time1 = (BYTE)atoi( sztime1);
	pwm2 = (BYTE)atoi( szpwm2 );
	time2 = (BYTE)atoi( sztime2);
	
	buf[ pos++ ] = pwm1;
	buf[ pos++ ] = time1;
	buf[ pos++ ] = onoff1;
	buf[ pos++ ] = pwm2;
	buf[ pos++ ] = time2;
	buf[ pos++ ] = onoff2;


	/* 2번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	
	memset( szpwm1, 0, sizeof( szpwm1 ));		
	memset( sztime1, 0, sizeof( sztime1 ));
	memset( szpwm2, 0, sizeof( szpwm2 ));		
	memset( sztime2, 0, sizeof( sztime2 ));

	m_plt2_1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt2_1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt2_2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt2_2_time.GetWindowText( strtime2, sizeof( strtime2 ));
	onoff1 = (BYTE)m_mullt2_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_2_onoff.GetCurSel();
	
	m_Util.WcsToMcs(strpwm1, szpwm1, wcslen( strpwm1 ));
	m_Util.WcsToMcs(strtime1, sztime1, wcslen( strtime1 ));
	m_Util.WcsToMcs(strpwm2, szpwm2, wcslen( strpwm2 ));
	m_Util.WcsToMcs(strtime2, sztime2, wcslen( strtime2 ));
		

	pwm1 = (BYTE)atoi( szpwm1 );
	time1 = (BYTE)atoi( sztime1);
	pwm2 = (BYTE)atoi( szpwm2 );
	time2 = (BYTE)atoi( sztime2);
	
	buf[ pos++ ] = pwm1;
	buf[ pos++ ] = time1;
	buf[ pos++ ] = onoff1;
	buf[ pos++ ] = pwm2;
	buf[ pos++ ] = time2;
	buf[ pos++ ] = onoff2;

	/* 3번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	
	memset( szpwm1, 0, sizeof( szpwm1 ));		
	memset( sztime1, 0, sizeof( sztime1 ));
	memset( szpwm2, 0, sizeof( szpwm2 ));		
	memset( sztime2, 0, sizeof( sztime2 ));

	m_plt3_1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt3_1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt3_2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt3_2_time.GetWindowText( strtime2, sizeof( strtime2 ));
	onoff1 = (BYTE)m_mullt3_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_3_onoff.GetCurSel();
	
	m_Util.WcsToMcs(strpwm1, szpwm1, wcslen( strpwm1 ));
	m_Util.WcsToMcs(strtime1, sztime1, wcslen( strtime1 ));
	m_Util.WcsToMcs(strpwm2, szpwm2, wcslen( strpwm2 ));
	m_Util.WcsToMcs(strtime2, sztime2, wcslen( strtime2 ));
		

	pwm1 = (BYTE)atoi( szpwm1 );
	time1 = (BYTE)atoi( sztime1);
	pwm2 = (BYTE)atoi( szpwm2 );
	time2 = (BYTE)atoi( sztime2);
	
	buf[ pos++ ] = pwm1;
	buf[ pos++ ] = time1;
	buf[ pos++ ] = onoff1;
	buf[ pos++ ] = pwm2;
	buf[ pos++ ] = time2;
	buf[ pos++ ] = onoff2;

	/* 4번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	
	memset( szpwm1, 0, sizeof( szpwm1 ));		
	memset( sztime1, 0, sizeof( sztime1 ));
	memset( szpwm2, 0, sizeof( szpwm2 ));		
	memset( sztime2, 0, sizeof( sztime2 ));

	m_plt4_1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt4_1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt4_2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt4_2_time.GetWindowText( strtime2, sizeof( strtime2 ));
	onoff1 = (BYTE)m_mullt4_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_4_onoff.GetCurSel();
	
	m_Util.WcsToMcs(strpwm1, szpwm1, wcslen( strpwm1 ));
	m_Util.WcsToMcs(strtime1, sztime1, wcslen( strtime1 ));
	m_Util.WcsToMcs(strpwm2, szpwm2, wcslen( strpwm2 ));
	m_Util.WcsToMcs(strtime2, sztime2, wcslen( strtime2 ));
		

	pwm1 = (BYTE)atoi( szpwm1 );
	time1 = (BYTE)atoi( sztime1);
	pwm2 = (BYTE)atoi( szpwm2 );
	time2 = (BYTE)atoi( sztime2);
	
	buf[ pos++ ] = pwm1;
	buf[ pos++ ] = time1;
	buf[ pos++ ] = onoff1;
	buf[ pos++ ] = pwm2;
	buf[ pos++ ] = time2;
	buf[ pos++ ] = onoff2;
	
	/* 5번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	
	memset( szpwm1, 0, sizeof( szpwm1 ));		
	memset( sztime1, 0, sizeof( sztime1 ));
	memset( szpwm2, 0, sizeof( szpwm2 ));		
	memset( sztime2, 0, sizeof( sztime2 ));

	m_plt5_1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt5_1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt5_2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt5_2_time.GetWindowText( strtime2, sizeof( strtime2 ));
	onoff1 = (BYTE)m_mullt5_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_5_onoff.GetCurSel();
	
	m_Util.WcsToMcs(strpwm1, szpwm1, wcslen( strpwm1 ));
	m_Util.WcsToMcs(strtime1, sztime1, wcslen( strtime1 ));
	m_Util.WcsToMcs(strpwm2, szpwm2, wcslen( strpwm2 ));
	m_Util.WcsToMcs(strtime2, sztime2, wcslen( strtime2 ));
		

	pwm1 = (BYTE)atoi( szpwm1 );
	time1 = (BYTE)atoi( sztime1);
	pwm2 = (BYTE)atoi( szpwm2 );
	time2 = (BYTE)atoi( sztime2);
	
	buf[ pos++ ] = pwm1;
	buf[ pos++ ] = time1;
	buf[ pos++ ] = onoff1;
	buf[ pos++ ] = pwm2;
	buf[ pos++ ] = time2;
	buf[ pos++ ] = onoff2;


	

	ClientSendMsg_t ClientSendMsg;
	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	
	ClientSendMsg.Msg   = SOCK_TEST_PLATE_SET_MSG;
	ClientSendMsg.SIPID = m_SelectedID;

	if ( m_Message.MakeReqTEST_PLATE_SET(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg, buf, pos ) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
		AfxMessageBox(L"UP 플랫 설정값이 저장되었습니다.");
	}

}

void CVS_ServerDlg::OnBnClickedButton6()
{
	TCHAR strpwm1[ 10 ];	
	TCHAR strtime1[ 10 ];	
	char szpwm1[ 10 ];
	char sztime1[ 10 ];

	TCHAR strpwm2[ 10 ];	
	TCHAR strtime2[ 10 ];	
	char szpwm2[ 10 ];
	char sztime2[ 10 ];

	BYTE onoff1;
	BYTE onoff2;	
		
	BYTE time1 = 0;
	BYTE pwm1 = 0;
	BYTE time2 = 0;
	BYTE pwm2 = 0;

	BYTE buf[ 50 ];
	int pos =0;
	
	/* off 설정 */
	memset( buf, 0, sizeof( buf ));
	buf[ pos++ ] = 0;

	/* 6번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	
	memset( szpwm1, 0, sizeof( szpwm1 ));		
	memset( sztime1, 0, sizeof( sztime1 ));
	memset( szpwm2, 0, sizeof( szpwm2 ));		
	memset( sztime2, 0, sizeof( sztime2 ));

	m_plt6_1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt6_1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt6_2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt6_2_time.GetWindowText( strtime2, sizeof( strtime2 ));
	onoff1 = (BYTE)m_mullt6_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_6_onoff.GetCurSel();
	
	m_Util.WcsToMcs(strpwm1, szpwm1, wcslen( strpwm1 ));
	m_Util.WcsToMcs(strtime1, sztime1, wcslen( strtime1 ));
	m_Util.WcsToMcs(strpwm2, szpwm2, wcslen( strpwm2 ));
	m_Util.WcsToMcs(strtime2, sztime2, wcslen( strtime2 ));
		
	pwm1 = (BYTE)atoi( szpwm1 );
	time1 = (BYTE)atoi( sztime1);
	pwm2 = (BYTE)atoi( szpwm2 );
	time2 = (BYTE)atoi( sztime2);
	
	buf[ pos++ ] = pwm1;
	buf[ pos++ ] = time1;
	buf[ pos++ ] = onoff1;
	buf[ pos++ ] = pwm2;
	buf[ pos++ ] = time2;
	buf[ pos++ ] = onoff2;
	
	/* 7번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	
	memset( szpwm1, 0, sizeof( szpwm1 ));		
	memset( sztime1, 0, sizeof( sztime1 ));
	memset( szpwm2, 0, sizeof( szpwm2 ));		
	memset( sztime2, 0, sizeof( sztime2 ));

	m_plt7_1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt7_1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt7_2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt7_2_time.GetWindowText( strtime2, sizeof( strtime2 ));
	onoff1 = (BYTE)m_mullt7_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_7_onoff.GetCurSel();
	
	m_Util.WcsToMcs(strpwm1, szpwm1, wcslen( strpwm1 ));
	m_Util.WcsToMcs(strtime1, sztime1, wcslen( strtime1 ));
	m_Util.WcsToMcs(strpwm2, szpwm2, wcslen( strpwm2 ));
	m_Util.WcsToMcs(strtime2, sztime2, wcslen( strtime2 ));
		

	pwm1 = (BYTE)atoi( szpwm1 );
	time1 = (BYTE)atoi( sztime1);
	pwm2 = (BYTE)atoi( szpwm2 );
	time2 = (BYTE)atoi( sztime2);
	
	buf[ pos++ ] = pwm1;
	buf[ pos++ ] = time1;
	buf[ pos++ ] = onoff1;
	buf[ pos++ ] = pwm2;
	buf[ pos++ ] = time2;
	buf[ pos++ ] = onoff2;

	/* 8번째 제어 */
	memset(strpwm1, 0, sizeof(strpwm1 ));
	memset(strtime1, 0, sizeof(strtime1 ));
	memset(strpwm2, 0, sizeof(strpwm2 ));
	memset(strtime2, 0, sizeof(strtime2 ));
	
	memset( szpwm1, 0, sizeof( szpwm1 ));		
	memset( sztime1, 0, sizeof( sztime1 ));
	memset( szpwm2, 0, sizeof( szpwm2 ));		
	memset( sztime2, 0, sizeof( sztime2 ));

	m_plt8_1_pwm.GetWindowText( strpwm1,sizeof( strpwm1 ));
	m_plt8_1_time.GetWindowText( strtime1, sizeof( strtime1 ));
	m_plt8_2_pwm.GetWindowText( strpwm2,sizeof( strpwm2 ));
	m_plt8_2_time.GetWindowText( strtime2, sizeof( strtime2 ));
	onoff1 = (BYTE)m_mullt8_onoff.GetCurSel();
	onoff2 = (BYTE)m_mullt2_8_onoff.GetCurSel();
	
	m_Util.WcsToMcs(strpwm1, szpwm1, wcslen( strpwm1 ));
	m_Util.WcsToMcs(strtime1, sztime1, wcslen( strtime1 ));
	m_Util.WcsToMcs(strpwm2, szpwm2, wcslen( strpwm2 ));
	m_Util.WcsToMcs(strtime2, sztime2, wcslen( strtime2 ));
		

	pwm1 = (BYTE)atoi( szpwm1 );
	time1 = (BYTE)atoi( sztime1);
	pwm2 = (BYTE)atoi( szpwm2 );
	time2 = (BYTE)atoi( sztime2);
	
	buf[ pos++ ] = pwm1;
	buf[ pos++ ] = time1;
	buf[ pos++ ] = onoff1;
	buf[ pos++ ] = pwm2;
	buf[ pos++ ] = time2;
	buf[ pos++ ] = onoff2;

	ClientSendMsg_t ClientSendMsg;
	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	
	ClientSendMsg.Msg   = SOCK_TEST_PLATE_SET_MSG;
	ClientSendMsg.SIPID = m_SelectedID;

	if ( m_Message.MakeReqTEST_PLATE_SET(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg, buf, pos ) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
		AfxMessageBox(L"DOWN 플랫 설정값이 저장되었습니다.");
	}

}



/* LIGHT PWM 제어 */
void CVS_ServerDlg::OnBnClickedBtnEtc8()
{
	ClientSendMsg_t ClientSendMsg;
	motor_info_t motor_info;
	TCHAR strpwm[ 10 ];
	char szpwm[ 10 ];
	BYTE pwm = 0x00;
	
	if ( isCheckSelected() == FALSE )
	{
		return;
	}	

	memset( strpwm, 0, sizeof( strpwm ));
	memset( szpwm, 0, sizeof( szpwm ));
	m_ctl_light_pwm.GetWindowText( strpwm,sizeof( strpwm ));
	m_Util.WcsToMcs(strpwm, szpwm, wcslen( strpwm ));
	pwm = (BYTE)atoi( szpwm);
	
	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	
	ClientSendMsg.Msg   = SOCK_TEST_LIGHT_PWM_REQ_MSG;
	ClientSendMsg.SIPID = m_SelectedID;
	
	if ( m_Message.MakeReqTEST_LIGHT_PWM(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg, pwm ) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
	}
}


/* WIFI 모드 */
void CVS_ServerDlg::OnBnClickedBtnMac()
{
	ClientSendMsg_t ClientSendMsg;
	net_info_t net_info;
	BYTE wifi_kind;
	CString strMsg;
	TCHAR ssid[ MAX_SSID_SIZE ];
	TCHAR key[ MAX_KEY_SIZE ];
	TCHAR ip[ MAX_IP_SIZE ];
	TCHAR port [ MAX_IP_SIZE ];
	char szport[ MAX_IP_SIZE ];


	if ( isCheckSelected() == FALSE )
	{
		return;
	}

	memset( ssid, 0, sizeof( ssid ));
	memset( key, 0, sizeof( key ));
	memset( ip, 0, sizeof( ip ));
	memset( port, 0, sizeof( port ));
	memset( szport, 0, sizeof( szport ));
	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	memset( &net_info, 0, sizeof(net_info)); 

	wifi_kind = (BYTE)m_ctlwifi_type.GetCurSel()+1;
	
	/* Client */
	if( wifi_kind == WIFI_STA )
	{	
		m_ctl_ssid.GetWindowText( ssid, sizeof( ssid ));
		m_ctl_key.GetWindowText( key, sizeof( key ));
		strMsg.Format(L"Device가 Client 모드로 운영됩니다. 진행 하시겠습니까?");
	}
	/* AP */
	else
	{
		wcscpy_s( ssid,  L"1" );
		wcscpy_s( key,  L"1" );
		strMsg.Format(L"Device가 AP 모드로 운영됩니다. 진행 하시겠습니까?");
	}

	if ( MessageBox(strMsg,L"확인", MB_YESNO ) == IDNO )
	{
		return;
	}

	m_CtlServerIP.GetWindowText( ip,sizeof( ip ));
	m_ctl_port.GetWindowText( port, sizeof( port ));			

	m_Util.WcsToMcs(ip,net_info.server_ip, wcslen( ip ));
	m_Util.WcsToMcs(port,szport, wcslen( port ));
	net_info.server_port = (short)atoi( szport);

	m_Util.WcsToMcs(ssid,net_info.ssid, wcslen( ssid ));
	m_Util.WcsToMcs(key,net_info.key, wcslen( key ));
		
	ClientSendMsg.Msg   = SOCK_TEST_WIFI_REQ_MSG;
	ClientSendMsg.SIPID = m_SelectedID;
	
	if ( m_Message.MakeReqTEST_WIFI(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg, &net_info ) == TRUE )
	{
		Internal_SendMsg( &ClientSendMsg );
		Init_Processtime( -1 );
	}
}


/* AP 모드 */
void CVS_ServerDlg::OnBnClickedBtnCallReady()
{
	ClientSendMsg_t ClientSendMsg;
	net_info_t net_info;
	
	CString strMsg;
	TCHAR ssid[ MAX_SSID_SIZE ];
	TCHAR key[ MAX_KEY_SIZE ];
	TCHAR ip[ MAX_IP_SIZE ];
	TCHAR port [ MAX_IP_SIZE ];
	char szport[ MAX_IP_SIZE ];

	
	memset( ssid, 0, sizeof( ssid ));
	memset( key, 0, sizeof( key ));
	memset( ip, 0, sizeof( ip ));
	memset( port, 0, sizeof( port ));
	memset( szport, 0, sizeof( szport ));
	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	memset( &net_info, 0, sizeof(net_info)); 
		
	m_ctl_ssid.GetWindowText( ssid, sizeof( ssid ));
	m_ctl_key.GetWindowText( key, sizeof( key ));
	m_CtlServerIP.GetWindowText( ip,sizeof( ip ));
	m_ctl_port.GetWindowText( port, sizeof( port ));			
	
	strMsg.Format(L"디바이스가 AP 모드로 운영될 경우에만 사용될 수 있습니다.\r\n먼저 디바이스의 AP로 WIFI를 연결 하시기 바랍니다. \r\n진행하시겠습니까?");
	
	if ( MessageBox(strMsg,L"확인", MB_YESNO ) == IDNO )
	{
		return;
	}
		
	m_Util.WcsToMcs(ip,net_info.server_ip, wcslen( ip ));
	m_Util.WcsToMcs(port,szport, wcslen( port ));
	net_info.server_port = (short)atoi( szport);

	m_Util.WcsToMcs(ssid,net_info.ssid, wcslen( ssid ));
	m_Util.WcsToMcs(key,net_info.key, wcslen( key ));
		
	ClientSendMsg.Msg   = SOCK_NET_INFO_RESP_MSG;
	ClientSendMsg.SIPID = m_SelectedID;
	
	if ( m_Message.MakeReqTEST_AP(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg, &net_info ) == TRUE )
	{
		if ( SendAPData( ClientSendMsg.Size, ClientSendMsg.SendBuf ) == TRUE )
		{
			Init_Processtime( -1 );	
			AfxMessageBox(L"전송 성공");
		}
		else
		{
			Init_Processtime( -1 );	
			AfxMessageBox(L"전송 실패");
		}
	}
}


/* AWS 모드 */
void CVS_ServerDlg::OnBnClickedBtnCallReadyAws()
{
	ClientSendMsg_t ClientSendMsg;
	net_info_t net_info;
	
	CString strMsg;
	TCHAR ssid[ MAX_SSID_SIZE ];
	TCHAR key[ MAX_KEY_SIZE ];
	TCHAR aws_host[ MAX_AWS_HOST_SIZE ];
	TCHAR aws_id[ MAX_AWS_ID_SIZE ];
	TCHAR port [ MAX_IP_SIZE ];

	char szport[ MAX_IP_SIZE ];

	
	memset( ssid, 0, sizeof( ssid ));
	memset( key, 0, sizeof( key ));
	memset( aws_host, 0, sizeof( aws_host ));
	memset( aws_id, 0, sizeof( aws_id ));
	memset( port, 0, sizeof( port ));
	memset( szport, 0, sizeof( szport ));

	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
	memset( &net_info, 0, sizeof(net_info)); 
		
	m_ctl_ssid.GetWindowText( ssid, sizeof( ssid ));
	m_ctl_key.GetWindowText( key, sizeof( key ));
	m_ctl_aws_host.GetWindowText( aws_host,sizeof( aws_host ));
	m_ctl_aws_id.GetWindowText( aws_id,sizeof( aws_id ));
	m_ctl_aws_port.GetWindowText( port, sizeof( port ));			
	
	strMsg.Format(L"디바이스가 AP 모드로 운영될 경우에만 사용될 수 있습니다.\r\n먼저 디바이스의 AP로 WIFI를 연결 하시기 바랍니다. \r\n진행하시겠습니까?");
	
	if ( MessageBox(strMsg,L"확인", MB_YESNO ) == IDNO )
	{
		return;
	}
		
	m_Util.WcsToMcs(aws_host,net_info.aws_host, wcslen( aws_host ));
	m_Util.WcsToMcs(aws_id,net_info.aws_id, wcslen( aws_id ));
	m_Util.WcsToMcs(port,szport, wcslen( port ));
	net_info.aws_port = (short)atoi( szport);

	m_Util.WcsToMcs(ssid,net_info.ssid, wcslen( ssid ));
	m_Util.WcsToMcs(key,net_info.key, wcslen( key ));
		
	ClientSendMsg.Msg   = SOCK_AWS_INFO_REQ_MSG;
	ClientSendMsg.SIPID = m_SelectedID;
	
	if ( m_Message.MakeReqTEST_AWS_AP(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg, &net_info ) == TRUE )
	{
		if ( SendAPData( ClientSendMsg.Size, ClientSendMsg.SendBuf ) == TRUE )
		{
			Init_Processtime( -1 );	
			AfxMessageBox(L"전송 성공");
		}
		else
		{
			Init_Processtime( -1 );	
			AfxMessageBox(L"전송 실패");
		}
	}
}


/* Volume Set */
void CVS_ServerDlg::OnBnClickedBtnVol()
{
	TCHAR strnum[ 100 ];
	char sznum[100];
	
	memset( strnum, 0, sizeof( strnum ));
	memset( sznum, 0, sizeof( sznum ));

	m_ctl_img_num.GetWindowText( strnum, sizeof( strnum ));
	m_Util.WcsToMcs(strnum,sznum, wcslen( strnum ));
	m_ctl_image.SetCheck( 1 );
	DisplayImage( sznum );
	m_ctl_image.SetCheck( 0 );
}

/* check mac */
int CVS_ServerDlg::CompareMAC( int org_num, CString &strOrgMAC )
{
	
	int nCnt ,i = 0;			
	int same = 0;
	int nID =-1;

	CString strID;
	CString strMsg;
	CString strMAC;

	nCnt = m_CtlClientList.GetItemCount();

	for ( i = 0 ; i < nCnt ; i++)
	{			
		if ( i != org_num )
		{
			strID = m_CtlClientList.GetItemText( i, 1 );
			strMAC = m_CtlClientList.GetItemText( i, 3 );

			if (strMAC.GetLength() > 10 )
			{
				same = strOrgMAC.CompareNoCase( strMAC );
				if ( same == 0)
				{
					nID = m_Util.StringToInt( strID );
					break;
				}
			}
		}
	}

	return nID;
}


/* LED 상태 표시 */
int CVS_ServerDlg::DisplayLEDStatus( int ClientID, BYTE onoff )
{
	int i, nCnt, nID ;
	CString strID;
	CString strOn;

	nCnt = m_CtlClientList.GetItemCount();

	if ( onoff == 0 ) 
		strOn.Format(L"OFF" );
	else
		strOn.Format(L"ON" );

	nID = 0;

	for ( i = 0 ; i < nCnt ; i++)
	{					
		strID = m_CtlClientList.GetItemText( i, 1 );

		if ( strID.GetLength() > 3 )
		{
			nID = m_Util.StringToInt( strID );
		}

		if ( nID == ClientID )
		{
			m_CtlClientList.SetItemText( i, 4 , L"");
			m_CtlClientList.SetItemText( i, 4 , strOn);
			break;
		}
	}

	return 0;
}

/* LED GET */
void CVS_ServerDlg::OnBnClickedBtnLedGet()
{
	
}

/* GPIO */

/* GPIO 상태 표시 */
int CVS_ServerDlg::DisplayGPIOStatus( int ClientID, BYTE type , BYTE onoff )
{
	int i, nCnt, nID ;
	CString strID;
	CString strOn;
	CString strType;

	nCnt = m_CtlClientList.GetItemCount();

	if ( onoff == 0 ) 
		strOn.Format(L"OFF" );
	else
		strOn.Format(L"ON" );

	if ( type == 1 ) 
		strOn.Format(L"음성인식" );
	else
		strOn.Format(L"Reserved" );
	
	nID = 0;

	for ( i = 0 ; i < nCnt ; i++)
	{					
		strID = m_CtlClientList.GetItemText( i, 1 );

		if ( strID.GetLength() > 3 )
		{
			nID = m_Util.StringToInt( strID );
		}

		if ( nID == ClientID )
		{
			m_CtlClientList.SetItemText( i, 4 , L"");
			m_CtlClientList.SetItemText( i, 4 , strOn);
			break;
		}
	}

	return 0;
}

void CVS_ServerDlg::OnBnClickedBtnGpioGet()
{
	
}


void CVS_ServerDlg::OnBnClickedBtnGpioSet()
{
	
}

void CVS_ServerDlg::OnBnClickedBtnClose()
{	
	OnClose();
	PostMessage( WM_CLOSE, 0, 0);
}

void CVS_ServerDlg::OnBnClickedBtnClear()
{
	Statusinfo_t Status;

	memset( &Status, 0, sizeof( Status ));

	m_ImageViewer[ 0 ].nClientID = 0;
	m_SelectedID = 0;
	m_CtlSelectedID.SetWindowText(L"");		
	m_ctl_version.SetWindowText(L"");
	m_ctl_time.SetWindowText(L"0");
	
	ClearLogList();
	Display_Sts( &Status );
	Invalidate(true);
	m_ctrl_org_file.SetWindowTextW(L"");
	m_ctrl_new_file.SetWindowTextW(L"");
}

void CVS_ServerDlg::OnNMClickClientList(NMHDR *pNMHDR, LRESULT *pResult)
{
	int nCnt=0;
	int nCnt2 = 0;
	CString strMsg;
	int nSelectID = 0;
	CString strSelectID ;

	NM_LISTVIEW* pNMListView =(NM_LISTVIEW*)pNMHDR;
	nCnt = pNMListView->iItem;

	if (nCnt >= 0)
	{
		nCnt2 = m_CtlClientList.GetItemCount();		
		for ( int i = 0 ; i < nCnt2 ; i++)
		{
			if ( m_CtlClientList.GetItemState(i, LVIS_SELECTED) !=0 )
			{
				strSelectID = m_CtlClientList.GetItemText( i, 1 );
				nSelectID = m_Util.StringToInt( strSelectID );
#if 0
				m_SelectedID = m_AliveClient[ i ].ID;
#else
				m_SelectedID =  nSelectID;
#endif
				strMsg.Format(L"%d", m_SelectedID);
				m_CtlSelectedID.SetWindowText( strMsg );
				m_ImageViewer[ 0 ].nClientID = m_SelectedID;
				
				break;
			}
		}
	}

	*pResult = 0;
}

/*============================================================================================
Export functions
==============================================================================================*/
void CVS_ServerDlg::DumpStr( CString &str )
{

	CString strtxt;
	strtxt.Format(L"VS_SERVER::CVS_ServerDlg : %s \n", str);
	strtxt.ReleaseBuffer();
	
#if DEBUG_MAINVIEWER
	OutputDebugString( strtxt );
#endif
}


void CVS_ServerDlg::InitDlg( void )
{

	TransferConf_t TransferConf;
	ModelConf_t ModelConf;
	CString strMsg;
	UINT dwErrID = 0;
	BOOL bResult = FALSE;
	
	m_nHeight	= 0;
	m_SaveLog	= 0;
	m_Preday	= 0;

	m_TCPConnectState.SetWindowText(L"TCP Server Open");
	
	//m_AliveClient = 0;
	memset( &m_AliveClient, 0, sizeof( m_AliveClient ));	

	m_ClientCnt = 0;
	m_SelectedID =0;
	DisplayClientCnt();
	m_ctl_image.SetCheck( 1 );
	InitLogList();

	if ( m_Controller != NULL )
	{
		if ( GetModelConfigure( &ModelConf) == TRUE )
		{
			//if (m_Controller->OpenModel( &ModelConf ) == TRUE )
			{
				if ( GetTransferConfigure( &TransferConf ) == TRUE )
				{
					if ( m_Controller->OpenTransfer( &TransferConf ) == TRUE )
					{
						if ( GetServerConfigure( ) != TRUE )
						{
							dwErrID = IDS_ERR_VIEWER_CONF;
						}
					}
					else
					{
						dwErrID = IDS_ERR_TRANSFER_OPEN;
					}
				}
				else
				{
					dwErrID = IDS_ERR_TRANSFER_CONF;
				}
			}
			//else
			//{
			//	dwErrID = IDS_ERR_DB_OPEN;
			//}
		}
		else
		{
			dwErrID = IDS_ERR_DB_COF;
		}
	}
	else
	{
		dwErrID = IDS_ERR_CONTROL;
	}

	if ( bResult != TRUE && dwErrID != 0 )
	{
		strMsg.LoadString( dwErrID );
		strMsg.ReleaseBuffer();
		AfxMessageBox(strMsg);
	//	PostMessage( WM_CLOSE, NULL, NULL );
	//	return;
	}	
	
	LargeFont.CreateFont(20, 0, 0, 0, FW_EXTRABOLD, 0, 0, 0,
		DEFAULT_CHARSET, 
		OUT_CHARACTER_PRECIS,
		CLIP_CHARACTER_PRECIS,
		DEFAULT_QUALITY,DEFAULT_PITCH, 
		_T("Arial"));

	m_ctl_time.SetWindowText(L"0");
	CStatic *sStatic = ((CStatic*)GetDlgItem(IDC_SELECTED_CLIENT));	
	CStatic *sStatic2 = ((CStatic*)GetDlgItem(IDC_CLIENT_CNT));
	CStatic *sStatic3 = ((CStatic*)GetDlgItem(IDC_TIME));
	CStatic *sStatic4 = ((CStatic*)GetDlgItem(IDC_VERSION));


	sStatic->SetFont(&LargeFont,TRUE);
	sStatic2->SetFont(&LargeFont,TRUE);
	sStatic3->SetFont(&LargeFont,TRUE);
	sStatic4->SetFont(&LargeFont,TRUE);


	m_Red.LoadBitmap( IDB_BIT_RED );
	m_Green.LoadBitmap( IDB_BIT_GREEN );


	m_ImageViewer[ 0 ].nClientID = 0;
#if 0
	m_ImageViewer[ 1 ].nClientID = 0;	
#endif
	
	strMsg.Format(L"Init MainViewer");
	strMsg.ReleaseBuffer();
	DumpStr( strMsg );

	m_hThread = NULL;
	m_dwThread = 0;
	m_Process_time = 0;

	if ( m_dwThread == 0) 
	{	
		m_hThread = CreateThread (NULL, 0, ThreadIdle, this, 0, &m_dwThread);
	}

	if ( m_hThread == INVALID_HANDLE_VALUE)
	{
		strMsg.Format(L"쓰레드 생성 에러");
		AfxMessageBox( strMsg );
		PostMessage( WM_CLOSE, 0, 0);
	}

	MakeLogDir();
	m_ptrImageBuf = ( BYTE *)malloc( sizeof( BYTE ) * MAX_IMAGE_BUF_SIZE );	
	m_cnt = 0;

	m_x = 0;
	m_x1 = 0;
	m_x2 = 0;
	m_x3 = 0;
	m_x4 = 0;

	m_motor_pwm.SetWindowText( L"50" );
	m_motor_time.SetWindowText( L"50" );
	m_ctl_light_pwm.SetWindowText( L"50" );

	m_ctl_plate_pwm.SetWindowText( L"50" );
	m_ctl_plate_time.SetWindowText( L"50" );

	m_ctl_temp_pwm.SetWindowText( L"50" );
	m_ctl_temp_time.SetWindowText( L"50" );
	
	m_ImgClass.Proc_Init();
	memset( &m_strip_name, 0, sizeof( m_strip_name ));
	wsprintfW(m_strip_name[ 0 ].NAME, L"잠혈");
	wsprintfW(m_strip_name[ 1 ].NAME, L"빌리루빈");
	wsprintfW(m_strip_name[ 2 ].NAME, L"우로빌리노겐");
	wsprintfW(m_strip_name[ 3 ].NAME, L"케톤체");
	wsprintfW(m_strip_name[ 4 ].NAME, L"단백질");
	wsprintfW(m_strip_name[ 5 ].NAME, L"아질산염");
	wsprintfW(m_strip_name[ 6 ].NAME, L"포도당");
	wsprintfW(m_strip_name[ 7 ].NAME, L"산성");
	wsprintfW(m_strip_name[ 8 ].NAME, L"비중");
	wsprintfW(m_strip_name[ 9 ].NAME, L"백혈구");	


	
#if 0
	//static int rgb_to_hsi( BYTE R, BYTE G, BYTE B, double * ptrH, double * ptrS, double *ptrI )

	BYTE R = 198;
	BYTE G = 206;
	BYTE B = 173;
	
	double H, S, I;
	double min_rgb;
	double temp;
	const double PI = acos(-1.0);

	H = S = I = 0.0;
	
	I = ( R + G + B ) / 3;

	/* GrayScale */
	if((  R == G ) && ( G == B ) )
	{
		S = 0;
		H = 0 ;
	}
	else
	{
		min_rgb = min( min( R, G), B );
		S = 1 - ( min_rgb /I );
		temp = (( R-G) + ( R-B ))/ ( 2 * sqrt(( R-G )* (R-G ) + ( R-B )*(R-B ) ));
		H = acos( temp ) * 180 / PI;

		if ( B > G )
			H = 360 - H;

		// H /= 360;
	}	
	double temp2;

#endif


}

void CVS_ServerDlg::InitLogList( void )
{	

	m_CtlClientList.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_CtlClientList.InsertColumn(0, _T("SEQ")		, LVCFMT_CENTER, 50);
	m_CtlClientList.InsertColumn(1, _T("SID")	, LVCFMT_CENTER, 50);
	m_CtlClientList.InsertColumn(2, _T("IP")		, LVCFMT_CENTER, 100);
	m_CtlClientList.InsertColumn(3, _T("MAC")		, LVCFMT_CENTER, 140);
	m_CtlClientList.InsertColumn(4, _T("LED")		, LVCFMT_CENTER, 80);


	m_CtlLogList.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_CtlLogList.InsertColumn(0, _T("SEQ")			, LVCFMT_RIGHT, 20);
	m_CtlLogList.InsertColumn(1, _T("Receive Time")	, LVCFMT_LEFT, 80);
	m_CtlLogList.InsertColumn(2, _T("Receive Data")	, LVCFMT_LEFT, 750);

	m_list_discon.SetExtendedStyle(LVS_EX_FLATSB | LVS_EX_FULLROWSELECT);
	m_list_discon.InsertColumn(0, _T("SEQ")			, LVCFMT_CENTER, 50);
	m_list_discon.InsertColumn(1, _T("Receive Time"), LVCFMT_LEFT, 100);
	m_list_discon.InsertColumn(2, _T("Receive Data"), LVCFMT_LEFT, 420);
	
	m_LineCnt	= 2;
	m_XLineCnt	= 2;

	
	/* LED */
	m_com_gpio_type.AddString(L"PWR WHITE");
	m_com_gpio_type.AddString(L"PWR RED");	
	m_com_gpio_type.AddString(L"AUTO");
	m_com_gpio_type.AddString(L"CLEAN");
	m_com_gpio_type.AddString(L"SNACK");
	m_com_gpio_type.AddString(L"URINE_WHITE");
	m_com_gpio_type.AddString(L"URINE_RED");
	m_com_gpio_type.AddString(L"UVA_WHITE");
	m_com_gpio_type.SetCurSel( 0 );

	m_com_gpio_onoff.AddString(L"OFF");
	m_com_gpio_onoff.AddString(L"ON");
	m_com_gpio_onoff.AddString(L"BLINK");
	m_com_gpio_onoff.SetCurSel( 0 );

	/* WIFI */
	m_ctlwifi_type.AddString(L"Client Mode");
	m_ctlwifi_type.AddString(L"AP Mode");
	m_ctlwifi_type.SetCurSel( 0 );

	/* TEST */
	m_ctl_testmode.AddString(L"TEST_NONE");
	m_ctl_testmode.AddString(L"TEST_AUDIO");
	m_ctl_testmode.AddString(L"TEST_CAMERA");
	m_ctl_testmode.AddString(L"TEST_WIFI");
	m_ctl_testmode.AddString(L"TEST_UART");
	m_ctl_testmode.AddString(L"TEST_AUTO");
	m_ctl_testmode.AddString(L"TEST_AWS");
	m_ctl_testmode.AddString(L"TEST_UVA");

	m_ctl_testmode.SetCurSel( 0 );

		
	/* aws */
	m_ctl_pub.AddString(L"POWER_ON");
	m_ctl_pub.AddString(L"POWER_OFF");
	m_ctl_pub.AddString(L"MOVE_UP");
	m_ctl_pub.AddString(L"MOVE_DOWN");
	m_ctl_pub.AddString(L"FEED");
	m_ctl_pub.AddString(L"PLAY");
	m_ctl_pub.AddString(L"PICTURE");
	m_ctl_pub.AddString(L"DISCONNECT");
	m_ctl_pub.AddString(L"URL");
	m_ctl_pub.AddString(L"KIT");
	m_ctl_pub.SetCurSel( 0 );


	/* dev_num */
	m_ctl_dev_num.AddString(L"1");
	m_ctl_dev_num.AddString(L"2");
	m_ctl_dev_num.AddString(L"3");
	m_ctl_dev_num.AddString(L"4");
	m_ctl_dev_num.AddString(L"5");
	m_ctl_dev_num.AddString(L"6");
	m_ctl_dev_num.AddString(L"7");
	m_ctl_dev_num.AddString(L"8");
	m_ctl_dev_num.AddString(L"9");
	m_ctl_dev_num.AddString(L"10");
	m_ctl_dev_num.SetCurSel( 0 );

	/* MOTOR */
	m_com_vol.AddString(L"LIFT");
	m_com_vol.AddString(L"SHIFT");
	m_com_vol.AddString(L"SNACK");
	m_com_vol.AddString(L"STRIP");

	m_com_vol.SetCurSel( 0 );

	m_com_onoff.AddString(L"OFF");
	m_com_onoff.AddString(L"ON");
	m_com_onoff.SetCurSel( 0 );
	
	m_mult_onoff.AddString(L"OFF");
	m_mult_onoff.AddString(L"ON");
	m_mult_onoff.SetCurSel( 0 );

	m_mult2_onoff.AddString(L"OFF");
	m_mult2_onoff.AddString(L"ON");
	m_mult2_onoff.SetCurSel( 0 );
	
	m_plt1_pwm.SetWindowText( L"0" );
	m_plt1_time.SetWindowText( L"0" );
	m_plt2_pwm.SetWindowText( L"0" );
	m_plt2_time.SetWindowText( L"0" );	
	m_mullt1_onoff.AddString(L"OFF");
	m_mullt1_onoff.AddString(L"ON");
	m_mullt1_onoff.SetCurSel( 0 );
	m_mullt2_1_onoff.AddString(L"OFF");
	m_mullt2_1_onoff.AddString(L"ON");
	m_mullt2_1_onoff.SetCurSel( 0 );

	m_plt2_1_pwm.SetWindowText( L"0" );
	m_plt2_1_time.SetWindowText( L"0" );
	m_plt2_2_pwm.SetWindowText( L"0" );
	m_plt2_2_time.SetWindowText( L"0" );	
	m_mullt2_onoff.AddString(L"OFF");
	m_mullt2_onoff.AddString(L"ON");
	m_mullt2_onoff.SetCurSel( 0 );
	m_mullt2_2_onoff.AddString(L"OFF");
	m_mullt2_2_onoff.AddString(L"ON");
	m_mullt2_2_onoff.SetCurSel( 0 );

	m_plt3_1_pwm.SetWindowText( L"0" );
	m_plt3_1_time.SetWindowText( L"0" );
	m_plt3_2_pwm.SetWindowText( L"0" );
	m_plt3_2_time.SetWindowText( L"0" );	
	m_mullt3_onoff.AddString(L"OFF");
	m_mullt3_onoff.AddString(L"ON");
	m_mullt3_onoff.SetCurSel( 0 );
	m_mullt2_3_onoff.AddString(L"OFF");
	m_mullt2_3_onoff.AddString(L"ON");
	m_mullt2_3_onoff.SetCurSel( 0 );


	m_plt4_1_pwm.SetWindowText( L"0" );
	m_plt4_1_time.SetWindowText( L"0" );
	m_plt4_2_pwm.SetWindowText( L"0" );
	m_plt4_2_time.SetWindowText( L"0" );	
	m_mullt4_onoff.AddString(L"OFF");
	m_mullt4_onoff.AddString(L"ON");
	m_mullt4_onoff.SetCurSel( 0 );
	m_mullt2_4_onoff.AddString(L"OFF");
	m_mullt2_4_onoff.AddString(L"ON");
	m_mullt2_4_onoff.SetCurSel( 0 );

	m_plt5_1_pwm.SetWindowText( L"0" );
	m_plt5_1_time.SetWindowText( L"0" );
	m_plt5_2_pwm.SetWindowText( L"0" );
	m_plt5_2_time.SetWindowText( L"0" );	
	m_mullt5_onoff.AddString(L"OFF");
	m_mullt5_onoff.AddString(L"ON");
	m_mullt5_onoff.SetCurSel( 0 );
	m_mullt2_5_onoff.AddString(L"OFF");
	m_mullt2_5_onoff.AddString(L"ON");
	m_mullt2_5_onoff.SetCurSel( 0 );


	m_plt6_1_pwm.SetWindowText( L"0" );
	m_plt6_1_time.SetWindowText( L"0" );
	m_plt6_2_pwm.SetWindowText( L"0" );
	m_plt6_2_time.SetWindowText( L"0" );	
	m_mullt6_onoff.AddString(L"OFF");
	m_mullt6_onoff.AddString(L"ON");
	m_mullt6_onoff.SetCurSel( 0 );
	m_mullt2_6_onoff.AddString(L"OFF");
	m_mullt2_6_onoff.AddString(L"ON");
	m_mullt2_6_onoff.SetCurSel( 0 );

	m_plt7_1_pwm.SetWindowText( L"0" );
	m_plt7_1_time.SetWindowText( L"0" );
	m_plt7_2_pwm.SetWindowText( L"0" );
	m_plt7_2_time.SetWindowText( L"0" );	
	m_mullt7_onoff.AddString(L"OFF");
	m_mullt7_onoff.AddString(L"ON");
	m_mullt7_onoff.SetCurSel( 0 );
	m_mullt2_7_onoff.AddString(L"OFF");
	m_mullt2_7_onoff.AddString(L"ON");
	m_mullt2_7_onoff.SetCurSel( 0 );

	m_plt8_1_pwm.SetWindowText( L"0" );
	m_plt8_1_time.SetWindowText( L"0" );
	m_plt8_2_pwm.SetWindowText( L"0" );
	m_plt8_2_time.SetWindowText( L"0" );	
	m_mullt8_onoff.AddString(L"OFF");
	m_mullt8_onoff.AddString(L"ON");
	m_mullt8_onoff.SetCurSel( 0 );
	m_mullt2_8_onoff.AddString(L"OFF");
	m_mullt2_8_onoff.AddString(L"ON");
	m_mullt2_8_onoff.SetCurSel( 0 );


	m_kind_audio.AddString(L"버튼");
	m_kind_audio.AddString(L"배변감지");
	m_kind_audio.AddString(L"LOCK");	
	m_kind_audio.AddString(L"녹취");	
	m_kind_audio.SetCurSel( 0 );

}

BOOL CVS_ServerDlg::GetModelConfigure( ModelConf_t * ModelConf )
{
	CSVR_Doc * ptrDoc = NULL;
	VSSERVERINFO_t * ptrServerInfo = NULL;
	CString strMsg;
	
	memset( ModelConf, 0, sizeof( ModelConf_t ));
	if ( m_Controller != NULL )
	{
		ptrDoc = (CSVR_Doc * )m_Controller->GetSVR_DocHandler();
		if ( ptrDoc != NULL )
		{
			ptrServerInfo = ptrDoc->GetServerInfo();
			if ( ptrServerInfo != NULL )
			{
				strMsg.Format(L"%d", ptrServerInfo->nID );
				strMsg.ReleaseBuffer();
				m_CtlServerID.SetWindowText( strMsg );

				/* 차후 INI 파일로 대처 할것  */
				wcscpy_s( ModelConf->strDSN,  ptrServerInfo->csDBDSN );
				wcscpy_s( ModelConf->strUserID, ptrServerInfo->csDBUser );
				wcscpy_s( ModelConf->strUserPwd, ptrServerInfo->csDBPass );
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CVS_ServerDlg::GetTransferConfigure( TransferConf_t * TransferConf )
{	
	CString strMsg;	
	CString strMsg2;	

	CSVR_Doc * ptrDoc = NULL;
	VSSERVERINFO_t * ptrServerInfo = NULL;
	
	if ( m_Controller != NULL )
	{
		ptrDoc = (CSVR_Doc * )m_Controller->GetSVR_DocHandler();
		if ( ptrDoc != NULL )
		{
			ptrServerInfo = ptrDoc->GetServerInfo();
			if ( ptrServerInfo != NULL )
			{
				strMsg.Format(L"%d", ptrServerInfo->nID );
				strMsg.ReleaseBuffer();
				m_CtlServerID.SetWindowText( strMsg );
				
				strMsg.Format(L"%d", ptrServerInfo->nTcpPort );
				strMsg2.Format(L"%d", ptrServerInfo->nUdpPort );

				m_Util.WcsToMcs( ptrServerInfo->csIP ,TransferConf->szServerIP,  (int)wcslen(ptrServerInfo->csIP));
				m_Util.WcsToMcs( ptrServerInfo->csNetmask ,TransferConf->szNetmask,  (int)wcslen(ptrServerInfo->csNetmask));
				m_Util.WcsToMcs( ptrServerInfo->csWIFI_IP ,TransferConf->szUDPServerIP,  (int)wcslen(ptrServerInfo->csWIFI_IP));

				m_CtlServerIP.SetWindowText( ptrServerInfo->csIP );

				m_ctl_updserverip.SetWindowText( ptrServerInfo->csWIFI_IP );
				m_ctl_port.SetWindowText( strMsg );
				m_ctl_udp_port.SetWindowText( strMsg2 );
				m_ctl_ssid.SetWindowText( ptrServerInfo->csSSID );
				m_ctl_key.SetWindowText( ptrServerInfo->csKEY );

				m_ctl_aws_host.SetWindowText( ptrServerInfo->csAWS_HOST);
				strMsg2.Format(L"%d", ptrServerInfo->nAWS_Port );
				m_ctl_aws_port.SetWindowText( strMsg2);
				m_ctl_aws_id.SetWindowText( ptrServerInfo->csAWS_ID);

				TransferConf->ServerPort = ptrServerInfo->nTcpPort;
				TransferConf->UDPSendPort = ptrServerInfo->nUdpPort;		
				
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL CVS_ServerDlg::GetServerConfigure( void )
{
	CSVR_Doc * ptrDoc = NULL;
	VSSERVERINFO_t * ptrServerInfo = NULL;
	CString strMsg;
	TCHAR strPath[ MAX_PATH ];
	
	memset( &m_ServerConf, 0, sizeof( ServerConf_t));	
	memset( strPath, 0, sizeof( strPath ));

	if ( m_Controller != NULL )
	{
		ptrDoc = (CSVR_Doc * )m_Controller->GetSVR_DocHandler();
		if ( ptrDoc != NULL )
		{
			ptrServerInfo = ptrDoc->GetServerInfo();
			if ( ptrServerInfo != NULL )
			{
				strMsg.Format( L"%s", ptrServerInfo->csImageSavePath);
				wcscpy_s(strPath, strMsg.GetLength() + 1 , strMsg );
				m_Util.CreateDir( strPath );
				m_nHeight = ptrServerInfo->nImageHeight;
				return TRUE;
			}
		}
	}

	return FALSE;
}

void CVS_ServerDlg::Internal_SendMsg( ClientSendMsg_t * ClientSendMsg )
{
	MsgInfo_t MsgInfo;	
	int Result = -1;	

	memset( &MsgInfo, 0, sizeof( MsgInfo ));
	
	if ( m_Controller == NULL )
	{
		return ;
	}
	
	//ClientSendMsg->SIPID = m_SelectedID;
	MsgInfo.SendMsgType = SEND_VS_SEND_MSG;
	MsgInfo.ptrMsg = ClientSendMsg;
	
	Result = m_Controller->SendMsg( TARGET_TCPSERVER, &MsgInfo );

	if ( Result > 0 )
	{
		CString strMsg2;
		SetTimer( CHECK_RESP_MSG_TIMER , 3000, NULL );
		strMsg2.Format(L"Send to Client-SIP(%d)Msg(0x%04x)SendMsg Size(%d)", ClientSendMsg->SIPID,ClientSendMsg->Msg,  Result);
		strMsg2.ReleaseBuffer();		
		//AddLogList( strMsg2 );
	}
}


void CVS_ServerDlg::MakeLogDir( )
{
	CString strPath;
	CString strCh;
	TCHAR strBlank[ MAX_PATH ];	
	TCHAR strtmp[ MAX_PATH ];
	TCHAR strCurPath[ MAX_PATH ];

	memset( m_strmLogDir, 0, sizeof( m_strmLogDir));
	memset( strtmp, 0, sizeof( strtmp));	
	memset( strBlank, 0, sizeof( strBlank) );
	memset( strCurPath, 0, sizeof( strCurPath ));

	if (GetModuleFileName( AfxGetInstanceHandle( ), strBlank, sizeof( strBlank) ) == 0 )
	{
		AfxMessageBox(L"저장 디렉토리를 찾을 수 없습니다.한글 경로에 있는 경우 영문으로 변경해 주시기 바랍니다.");	
	}

	strPath.Format(L"%s", strBlank );
	strPath.ReleaseBuffer();

	strPath = strPath.Left(strPath.ReverseFind('\\'));
	wcscpy_s(strCurPath, strPath.GetLength() + 1 , strPath );
	wsprintf(m_strmLogDir,L"%s\\%s", strCurPath, L"LOG");
	
	if ( m_Util.CreateDir( m_strmLogDir ) != TRUE )
	{
		AfxMessageBox(L"저장 디렉토리를 찾을 수 없습니다.한글 경로에 있는 경우 영문으로 변경해 주시기 바랍니다.");
		return ;
	}
}


BOOL CVS_ServerDlg::Savebin_Append( TCHAR *prtPath , TCHAR * ptrstr)
{
	BOOL ret = TRUE;
    FILE * fp = NULL;    
	char sztemp[ MAX_PATH * 3 ];		
	CString strErr;

    if ( prtPath == NULL || ptrstr == NULL)
        return FALSE;

	memset( sztemp, 0, sizeof( sztemp ));	
	m_Util.WcsToMcs( prtPath, sztemp, (int)wcslen( prtPath ));
    fopen_s( &fp,sztemp, "at");

    if ( fp == NULL )
    {
		strErr.Format(L"파일을 저장할 수 없습니다.(%s)", prtPath );
		AfxMessageBox( strErr );
        return FALSE;
    }    

    //print_buf( nSize, ptrdata );
	memset( sztemp, 0, sizeof( sztemp ));	
	m_Util.WcsToMcs( ptrstr, sztemp, (int)wcslen( ptrstr ));
    fprintf(fp, "%s\r\n", sztemp);

    fflush( fp );

    if ( fp != NULL ) 
    {    
        fclose( fp );
    }

    Sleep( 0 );
	
	return ret;
}

int CVS_ServerDlg::SaveLog( UINT32 Cur_Time, TCHAR * strLog )
{
	TCHAR strTime[ MAX_PATH ];
	TCHAR strPath[ MAX_PATH *2 ];
	TCHAR strFile[ MAX_PATH *2 ];

	int Cur_Day = 0;
		
	memset( strTime, 0, sizeof( strTime ));
	memset( strPath, 0, sizeof( strPath ));
	memset( strFile, 0, sizeof( strFile ));

	m_Util.ConvTimeToIntDay( Cur_Time, &Cur_Day );
	m_Util.ConvTimeToDirtPath( 0, Cur_Time, strTime );

	wsprintf(strPath,L"%s\\%s", m_strmLogDir, strTime);
	wsprintf(strFile,L"%s\\%s\\%s.txt", m_strmLogDir, strTime, strTime );
		
	if ( m_Preday != Cur_Day )
	{
		if ( m_Util.CreateDir( strPath ) != TRUE )
		{
			AfxMessageBox(L"저장 디렉토리를 찾을 수 없습니다.한글 경로에 있는 경우 영문으로 변경해 주시기 바랍니다.");
			return -1;
		}
	}	

	Savebin_Append( strFile, strLog );
	return 0;
}

void CVS_ServerDlg::AddLogList( CString & strMsg )
{
	TCHAR strIndex[ MAX_PATH ];
	CString strTime;	
	//CString strSaveMsg;
	TCHAR strSave[ MAX_PATH * 2 ];
	TCHAR strTemp[ MAX_PATH * 2 ];

	int iIdx;	
	time_t Cur_Time;	

	LV_ITEM lvitem;
	static volatile int Cnt =0;	

	if ( Cnt == 1 )
		return;

	Cnt = 1;


	memset( strIndex, 0, sizeof( strIndex ) );

	iIdx = m_CtlLogList.GetItemCount();

	if (iIdx >= 100 )
	{
		ClearLogList();
		iIdx = m_CtlLogList.GetItemCount();
	}
	
	Cur_Time = time( NULL );	

	m_Util.ConvTimeToString( Cur_Time, strTime );
	

	strTime.ReleaseBuffer();
	lvitem.iItem = iIdx;
	
	wsprintf( strIndex,L"%d", iIdx+1 );
	
	lvitem.mask			= LVIF_TEXT ;
	lvitem.iSubItem		= 0;
	lvitem.pszText		= (LPWSTR)&strIndex;
	m_CtlLogList.InsertItem(&lvitem);	
	m_CtlLogList.SetItemText(iIdx, 1, strTime);
	m_CtlLogList.SetItemText(iIdx, 2, strMsg);
	m_CtlLogList.EnsureVisible( iIdx, TRUE);

	Cnt =0;

}

void CVS_ServerDlg::AddDisLogList( CString & strMsg )
{
	TCHAR strIndex[ MAX_PATH ];
	CString strTime;	
	//CString strSaveMsg;	

	int iIdx;	
	time_t Cur_Time;	

	LV_ITEM lvitem;
	static volatile int Cnt =0;
	

	if ( Cnt == 1 )
		return;

	Cnt = 1;


	memset( strIndex, 0, sizeof( strIndex ) );

	iIdx = m_list_discon.GetItemCount();

	if (iIdx >= MAX_LIST_CNT )
	{
		ClearLogList();
		iIdx = m_list_discon.GetItemCount();
	}
	
	Cur_Time = time( NULL );	

	m_Util.ConvTimeToString( Cur_Time, strTime );
	

	strTime.ReleaseBuffer();
	lvitem.iItem = iIdx;
	
	wsprintf( strIndex,L"%d", iIdx+1 );
	
	lvitem.mask			= LVIF_TEXT ;
	lvitem.iSubItem		= 0;
	lvitem.pszText		= (LPWSTR)&strIndex;
	m_list_discon.InsertItem(&lvitem);	
	m_list_discon.SetItemText(iIdx, 1, strTime);
	m_list_discon.SetItemText(iIdx, 2, strMsg);
	m_list_discon.EnsureVisible( iIdx, TRUE);
	Cnt =0;
	
}


void CVS_ServerDlg::AddClientList( int ID, TCHAR * strIP,   TCHAR *strMAC )
{
	TCHAR strIndex[ 50 ];
	TCHAR strID[ 50 ];
	int iIdx;	
	LV_ITEM lvitem;
	

	memset( strIndex, 0, sizeof( strIndex ) );
	memset( strID, 0, sizeof( strID ) );

	Lock();

	iIdx = m_CtlClientList.GetItemCount();
	lvitem.iItem = iIdx;

	wsprintf( strIndex,L"%d", iIdx+1 );
	wsprintf( strID ,L"%d", ID );
		
	lvitem.mask = LVIF_TEXT ;
	lvitem.iSubItem = 0;
	lvitem.pszText = (LPWSTR)&strIndex;
	m_CtlClientList.InsertItem(&lvitem);	
	m_CtlClientList.SetItemText(iIdx, 1, strID);		
	m_CtlClientList.SetItemText(iIdx, 2, strIP);		
	m_CtlClientList.SetItemText(iIdx, 3,  strMAC);
	m_CtlClientList.EnsureVisible( iIdx, TRUE);

	UnLock();

}

void CVS_ServerDlg::ClearLogList( void )
{
	m_CtlLogList.DeleteAllItems();
	m_list_discon.DeleteAllItems();
}

void CVS_ServerDlg::ReleaseDlg( void )
{
	m_Util.KillProcess( L"VS_Server.exe");
	m_Controller = NULL;
}

BOOL CVS_ServerDlg::SetControllerHandler( void *  ControllerHandle )
{
	m_Controller = NULL;
	m_Controller = (IController *)ControllerHandle;
	return TRUE;
}

BOOL CVS_ServerDlg::AddModelHandler( int ModelKind, void * ModelHandle )
{
	( void )ModelKind;
	( void )ModelHandle;

	return TRUE;
}

int CVS_ServerDlg::OnRecvMsg( MsgInfo_t *MsgInfo )
{
	return m_MsgController.OnRecvMsg( MsgInfo );
}

int CVS_ServerDlg::DummyTCPCloseMsg( DummyMsg_t * DummyMsg )
{
	CString strMsg;

	pMainViewe->DelAlivedClient( DummyMsg->ClientID );

	if ( pMainViewe->m_SelectedID == DummyMsg->ClientID )
	{
		pMainViewe->OnBnClickedBtnClear();
	}

	strMsg.Format(L"Disconnected TCP Client( %d )", DummyMsg->ClientID );	
	//pMainViewe->AddLogList( strMsg );
	pMainViewe->AddDisLogList( strMsg );

	strMsg.ReleaseBuffer();
	
	return 0;
}

void CVS_ServerDlg::Init_Processtime( int val  )
{
	CString strMsg;		
	
	if ( val < 0 )
	{
		strMsg.Format(L"즉시");
	}
	else
	{
		strMsg.Format(L"%d", val );		
	}

	m_Process_time = val;
	m_ctl_time.SetWindowText( strMsg );

	if ( val > 0 )
	{		
		SetTimer( PROCESS_TIMER, PROCESS_TIME_INTERVAL, NULL );
	}
	
}

void CVS_ServerDlg::Set_Processtime( )
{
	int val =0;
	CString strMsg;

	KillTimer( PROCESS_TIMER );
	
	val = m_Process_time -1;
	if ( val >= 0 )
	{
		m_Process_time = val;
		strMsg.Format(L"%d", val );	
		m_ctl_time.SetWindowText( strMsg );
		SetTimer( PROCESS_TIMER, PROCESS_TIME_INTERVAL, NULL );
	}
}

void CVS_ServerDlg::Display_RUN( int val )
{
	CStatic * ptrspk = (CStatic*)GetDlgItem( IDC_SPK_STS );
	
	if (  val == 0 )
	{
		ptrspk->SetBitmap( m_Green );
	}
	else
	{
		ptrspk->SetBitmap( m_Red );
	}
}

void CVS_ServerDlg::Display_LINK( int val )
{
	CStatic * ptrmic = (CStatic*)GetDlgItem( IDC_MIC_STS );
	
	if (  val == 0 )
	{
		ptrmic->SetBitmap( m_Green );
	}
	else
	{
		ptrmic->SetBitmap( m_Red );
	}
}


void CVS_ServerDlg::Display_Sts( Statusinfo_t * ptrSTS )
{
	CStatic * ptrspk = (CStatic*)GetDlgItem( IDC_SPK_STS );
	CStatic * ptrmic = (CStatic*)GetDlgItem( IDC_MIC_STS );
	CStatic * ptrbtn = (CStatic*)GetDlgItem( IDC_BTN_STS );
	CStatic * ptretc = (CStatic*)GetDlgItem( IDC_GPIO_STS );	

	if (  ptrSTS->Speak_sts == 0 )
	{
		ptrspk->SetBitmap( m_Green );
	}
	else
	{
		ptrspk->SetBitmap( m_Red );
	}

	if (  ptrSTS->MIC_sts == 0 )
	{
		ptrmic->SetBitmap( m_Green );
	}
	else
	{
		ptrmic->SetBitmap( m_Red );
	}

	if (  ptrSTS->Button_sts == 0 )
	{
		ptrbtn->SetBitmap( m_Green );
	}
	else
	{
		ptrbtn->SetBitmap( m_Red );
	}

	if ( ptrSTS->ETC_sts == 0 )
	{
		ptretc->SetBitmap( m_Green );
	}
	else
	{
		ptretc->SetBitmap( m_Red );
	}
}

#define SEND_START	0x01
#define SEND_CONTINUE	0x02
#define SEND_END 0x03


// tory
int  CVS_ServerDlg::ClientRecvImg( ClientRecvMsg_t  * ClientRecvMsg )
{
	CString strMsg;
	int total_size  = 0;
	BYTE send_flag = 0x00;
	BYTE * ptr = NULL;	
	int len = 0;
	int recv_size ;
	MsgInfo_t MsgInfo;	
	DummyMsg_t DummMsg;
	CFile cFil;
	char szfilename[ 100 ];
	CString strTime;
	TCHAR * ptrTime = NULL;
	char sztime[ 50 ];
	if ( pMainViewe->m_Message.ParseHeartbeatMsg( ClientRecvMsg, NULL ) == TRUE )
	{
		pMainViewe->Display_LINK( 0 );
		Sleep( 200 );
		pMainViewe->Display_LINK( 1 );
		

		ptr = ClientRecvMsg->DummyBuf;

		len = sizeof( short );
		memcpy( &total_size , ptr, len);		
		ptr += len;

		len = sizeof( char );
		send_flag = *ptr;
		ptr += len;

		recv_size = ClientRecvMsg->VAR1 - sizeof( short ) - sizeof( char );

		//pMainViewe->AddAlivedClient( ClientRecvMsg->SIPID , ClientRecvMsg->ClientIP,  ClientRecvMsg->ClientMAC );
		//strMsg.Format(L"Recv Image From TotalSize:%d, Send_Flag:%x", total_size,  send_flag );		
		//pMainViewe->AddLogList( strMsg );
		//strMsg.ReleaseBuffer();

		if ( send_flag == SEND_START )
		{
			if ( pMainViewe->m_ptrImageBuf != NULL )
			{
				memset( pMainViewe->m_ptrImageBuf, 0, MAX_IMAGE_BUF_SIZE ); 
				pMainViewe->m_revc_size = 0;

				memcpy( &pMainViewe->m_ptrImageBuf[ pMainViewe->m_revc_size ], ptr, recv_size );
				pMainViewe->m_revc_size += recv_size;
			}
		}
		else if ( send_flag == SEND_CONTINUE )
		{
			if ( pMainViewe->m_ptrImageBuf != NULL )
			{
				memcpy( &pMainViewe->m_ptrImageBuf[ pMainViewe->m_revc_size ], ptr, recv_size );
				pMainViewe->m_revc_size += recv_size;
			}
		}
		else if ( send_flag == SEND_END )
		{
			if ( pMainViewe->m_ptrImageBuf != NULL )
			{
				memset( sztime, 0, sizeof( sztime ));

				memcpy( &pMainViewe->m_ptrImageBuf[ pMainViewe->m_revc_size ], ptr, recv_size );
				pMainViewe->m_revc_size += recv_size;
				
				strMsg.Format(L"Recv Image From SendSize:%d, RecvSize:%d", total_size, pMainViewe->m_revc_size);		
				//pMainViewe->AddLogList( strMsg );
				strMsg.ReleaseBuffer();
				pMainViewe->m_cnt +=1;

				pMainViewe->m_Util.ConvTimeToString( time(NULL), strTime);
				ptrTime =  (TCHAR*)(LPCTSTR)strTime;
				if ( ptrTime != NULL )
					pMainViewe->m_Util.WcsToMcs(ptrTime, sztime, wcslen( ptrTime ));

			//	pMainViewe->SaveImageFile( DummyMsg_t * DummMsg )
				memset(szfilename, 0, sizeof( szfilename ));
				
				sprintf_s( szfilename, sizeof(szfilename), "img_%s.jpg", sztime);
				memset(&MsgInfo, 0, sizeof( MsgInfo ));
				memset(&DummMsg, 0, sizeof( DummMsg ));
				MsgInfo.SendMsgType = SEND_DUMMY_MSG;
				MsgInfo.ptrMsg = &DummMsg;
				
				DummMsg.Msg = DUMMY_MSG_SAVE_FILE;
				DummMsg.ptrData = pMainViewe->m_ptrImageBuf;
				DummMsg.DataSize = pMainViewe->m_revc_size;
				DummMsg.ptrEtcInfo = &szfilename[ 0 ];
				DummMsg.ptrEtcInfo2 = &cFil;
				DummMsg.ptrEtcInfo3 = pMainViewe;

				if ( pMainViewe->m_Controller->SendMsg( TARGET_FILEMANAGER, &MsgInfo ) == 0 )
				{
					pMainViewe->DisplayImage( szfilename );
				}
			}
		}
	}
	
	return 0;
}

int  CVS_ServerDlg::ClientRecvPlate( ClientRecvMsg_t  * ClientRecvMsg )
{
	CString strpwm1, strtime1, strpwm2, strtime2;
	int onoff1, onoff2;

	BYTE * ptr = NULL;	
	int len = 0;	
	DummyMsg_t DummMsg;
	plate_onoff_t plate_onoff;	
	int i;

	if ( pMainViewe->m_Message.ParseHeartbeatMsg( ClientRecvMsg, NULL ) == TRUE )
	{
		ptr = ClientRecvMsg->DummyBuf;
		memset( &plate_onoff, 0, sizeof( plate_onoff ));

		len = sizeof( plate_onoff_t );
		memcpy( &plate_onoff , ptr, len);

		
		/* 1번째 제어 */
		i = 0;
		strpwm1.Format(L"%d", plate_onoff.on[ i ].item[ 0 ]);
		strtime1.Format(L"%d", plate_onoff.on[ i ].item[ 1 ]);
		onoff1 = plate_onoff.on[ i ].item[ 2 ];
		strpwm2.Format(L"%d", plate_onoff.on[ i ].item[ 3 ]);
		strtime2.Format(L"%d", plate_onoff.on[ i ].item[ 4 ]);
		onoff2 = plate_onoff.on[ i ].item[ 5 ];

		pMainViewe->m_plt1_pwm.SetWindowText( strpwm1);
		pMainViewe->m_plt1_time.SetWindowText( strtime1);
		pMainViewe->m_plt2_pwm.SetWindowText( strpwm2);
		pMainViewe->m_plt2_time.SetWindowText( strtime2);
		pMainViewe->m_mullt1_onoff.SetCurSel( onoff1 );
		pMainViewe->m_mullt2_1_onoff.SetCurSel( onoff2);

		/* 2번째 제어 */
		i = 1;
		strpwm1.Format(L"%d", plate_onoff.on[ i ].item[ 0 ]);
		strtime1.Format(L"%d", plate_onoff.on[ i ].item[ 1 ]);
		onoff1 = plate_onoff.on[ i ].item[ 2 ];
		strpwm2.Format(L"%d", plate_onoff.on[ i ].item[ 3 ]);
		strtime2.Format(L"%d", plate_onoff.on[ i ].item[ 4 ]);
		onoff2 = plate_onoff.on[ i ].item[ 5 ];
		pMainViewe->m_plt2_1_pwm.SetWindowText( strpwm1);
		pMainViewe->m_plt2_1_time.SetWindowText( strtime1);
		pMainViewe->m_plt2_2_pwm.SetWindowText( strpwm2);
		pMainViewe->m_plt2_2_time.SetWindowText( strtime2);
		pMainViewe->m_mullt2_onoff.SetCurSel( onoff1 );
		pMainViewe->m_mullt2_2_onoff.SetCurSel( onoff2);

		/* 3번째 제어 */
		i = 2;
		strpwm1.Format(L"%d", plate_onoff.on[ i ].item[ 0 ]);
		strtime1.Format(L"%d", plate_onoff.on[ i ].item[ 1 ]);
		onoff1 = plate_onoff.on[ i ].item[ 2 ];
		strpwm2.Format(L"%d", plate_onoff.on[ i ].item[ 3 ]);
		strtime2.Format(L"%d", plate_onoff.on[ i ].item[ 4 ]);
		onoff2 = plate_onoff.on[ i ].item[ 5 ];

		pMainViewe->m_plt3_1_pwm.SetWindowText( strpwm1);
		pMainViewe->m_plt3_1_time.SetWindowText( strtime1);
		pMainViewe->m_plt3_2_pwm.SetWindowText( strpwm2);
		pMainViewe->m_plt3_2_time.SetWindowText( strtime2);
		pMainViewe->m_mullt3_onoff.SetCurSel( onoff1 );
		pMainViewe->m_mullt2_3_onoff.SetCurSel( onoff2);

		/* 4번째 제어 */
		i = 3;
		strpwm1.Format(L"%d", plate_onoff.on[ i ].item[ 0 ]);
		strtime1.Format(L"%d", plate_onoff.on[ i ].item[ 1 ]);
		onoff1 = plate_onoff.on[ i ].item[ 2 ];
		strpwm2.Format(L"%d", plate_onoff.on[ i ].item[ 3 ]);
		strtime2.Format(L"%d", plate_onoff.on[ i ].item[ 4 ]);
		onoff2 = plate_onoff.on[ i ].item[ 5 ];

		pMainViewe->m_plt4_1_pwm.SetWindowText( strpwm1);
		pMainViewe->m_plt4_1_time.SetWindowText( strtime1);
		pMainViewe->m_plt4_2_pwm.SetWindowText( strpwm2);
		pMainViewe->m_plt4_2_time.SetWindowText( strtime2);
		pMainViewe->m_mullt4_onoff.SetCurSel( onoff1 );
		pMainViewe->m_mullt2_4_onoff.SetCurSel( onoff2);

		/* 5번째 제어 */
		i = 4;
		strpwm1.Format(L"%d", plate_onoff.on[ i ].item[ 0 ]);
		strtime1.Format(L"%d", plate_onoff.on[ i ].item[ 1 ]);
		onoff1 = plate_onoff.on[ i ].item[ 2 ];
		strpwm2.Format(L"%d", plate_onoff.on[ i ].item[ 3 ]);
		strtime2.Format(L"%d", plate_onoff.on[ i ].item[ 4 ]);
		onoff2 = plate_onoff.on[ i ].item[ 5 ];

		pMainViewe->m_plt5_1_pwm.SetWindowText( strpwm1);
		pMainViewe->m_plt5_1_time.SetWindowText( strtime1);
		pMainViewe->m_plt5_2_pwm.SetWindowText( strpwm2);
		pMainViewe->m_plt5_2_time.SetWindowText( strtime2);
		pMainViewe->m_mullt5_onoff.SetCurSel( onoff1 );
		pMainViewe->m_mullt2_5_onoff.SetCurSel( onoff2);


		/* 6번째 제어( OFF 첫단계 ) */
		i = 0;
		strpwm1.Format(L"%d", plate_onoff.off[ i ].item[ 0 ]);
		strtime1.Format(L"%d", plate_onoff.off[ i ].item[ 1 ]);
		onoff1 = plate_onoff.off[ i ].item[ 2 ];
		strpwm2.Format(L"%d", plate_onoff.off[ i ].item[ 3 ]);
		strtime2.Format(L"%d", plate_onoff.off[ i ].item[ 4 ]);
		onoff2 = plate_onoff.off[ i ].item[ 5 ];

		pMainViewe->m_plt6_1_pwm.SetWindowText( strpwm1);
		pMainViewe->m_plt6_1_time.SetWindowText( strtime1);
		pMainViewe->m_plt6_2_pwm.SetWindowText( strpwm2);
		pMainViewe->m_plt6_2_time.SetWindowText( strtime2);
		pMainViewe->m_mullt6_onoff.SetCurSel( onoff1 );
		pMainViewe->m_mullt2_6_onoff.SetCurSel( onoff2);

		/* 7번째 제어 */
		i = 1;
		strpwm1.Format(L"%d", plate_onoff.off[ i ].item[ 0 ]);
		strtime1.Format(L"%d", plate_onoff.off[ i ].item[ 1 ]);
		onoff1 = plate_onoff.off[ i ].item[ 2 ];
		strpwm2.Format(L"%d", plate_onoff.off[ i ].item[ 3 ]);
		strtime2.Format(L"%d", plate_onoff.off[ i ].item[ 4 ]);
		onoff2 = plate_onoff.off[ i ].item[ 5 ];

		pMainViewe->m_plt7_1_pwm.SetWindowText( strpwm1);
		pMainViewe->m_plt7_1_time.SetWindowText( strtime1);
		pMainViewe->m_plt7_2_pwm.SetWindowText( strpwm2);
		pMainViewe->m_plt7_2_time.SetWindowText( strtime2);
		pMainViewe->m_mullt7_onoff.SetCurSel( onoff1 );
		pMainViewe->m_mullt2_7_onoff.SetCurSel( onoff2);

		/* 8번째 제어 */
		i = 2;
		strpwm1.Format(L"%d", plate_onoff.off[ i ].item[ 0 ]);
		strtime1.Format(L"%d", plate_onoff.off[ i ].item[ 1 ]);
		onoff1 = plate_onoff.off[ i ].item[ 2 ];
		strpwm2.Format(L"%d", plate_onoff.off[ i ].item[ 3 ]);
		strtime2.Format(L"%d", plate_onoff.off[ i ].item[ 4 ]);
		onoff2 = plate_onoff.off[ i ].item[ 5 ];

		pMainViewe->m_plt8_1_pwm.SetWindowText( strpwm1);
		pMainViewe->m_plt8_1_time.SetWindowText( strtime1);
		pMainViewe->m_plt8_2_pwm.SetWindowText( strpwm2);
		pMainViewe->m_plt8_2_time.SetWindowText( strtime2);
		pMainViewe->m_mullt8_onoff.SetCurSel( onoff1 );
		pMainViewe->m_mullt2_8_onoff.SetCurSel( onoff2);

		AfxMessageBox(L"플랫 설정값을 가져왔습니다");
	}
	
	return 0;
}

int  CVS_ServerDlg::ClientRecvLOG( ClientRecvMsg_t  * ClientRecvMsg )
{
	CString strMsg;
	CString strMsg2;

	char * ptr = NULL;	
	int len = 0;	

	if ( pMainViewe->m_Message.ParseHeartbeatMsg( ClientRecvMsg, NULL ) == TRUE )
	{
		ptr = (char*)ClientRecvMsg->DummyBuf;	

		len = strlen( ptr );
		
		strMsg = ptr;
		strMsg2.Format(L"[%d],Data:%s", ClientRecvMsg->SIPID, strMsg);
		//strMsg.Format("SID:%d,%s", ClientRecvMsg->SIPID, ptr );
		pMainViewe->AddLogList( strMsg2 );
		
	}
	
	return 0;
}
void CVS_ServerDlg::DisplayImage( char *ptrszname )
{
	CClientDC dc( pMainViewe );
	CImage Image;
	IMAGE_DATA_t ImageData, *ptrCharImage, * ptrTempImageData= NULL;
	VSSERVERINFO_t * ptrServerInfo = NULL;
	TCHAR strFileName[ MAX_PATH ];
	TCHAR strName[ MAX_PATH];
	LPR_DATA_t LPRData;	
	IMAGE_DATA_t OrgImageData;
	CAMERA_DATA_t CameraData;
	int i, Runmoitor = 0;
	int ret = 0;
	int err = 0;
	CString strMsg;

	/* JPG 파일을 사용 할 경우만  ijl15.lib을 사용한다. */
	ptrServerInfo= (VSSERVERINFO_t*)( GetSeverInfo());
	memset( &ImageData, 0, sizeof( IMAGE_DATA_t ));
	memset( &LPRData, 0, sizeof( LPRData ));
	memset( &OrgImageData, 0, sizeof( IMAGE_DATA_t ));
	memset( &CameraData, 0, sizeof( CAMERA_DATA_t ));
	memset( m_strip_result, 0xFF, sizeof( m_strip_result ));
	memset( strName, 0, sizeof( strName ));

	if (  ptrServerInfo != NULL )
	{
		memset(strFileName, 0, sizeof( strFileName ));
		m_Util.McsToWcs(ptrszname, strName, strlen( ptrszname)+1);
		wsprintf( strFileName, L"%s\\%s", ptrServerInfo->csImageSavePath, strName );
		m_ctrl_org_file.SetWindowTextW( strName );

		Runmoitor =  ptrServerInfo->nRunMonitor;
#if 0
		ret = m_WizardHelper.ReadBmp( tcPath , &ImageData);
#else
		ret = m_WizardHelper.ReadJpeg( strFileName, &ImageData );
#endif
		//DisplayImageData( strFileName, &ImageData, nArea );
	}
	if ( ret != ERR_SUCCESS )
	{
		AfxMessageBox(L"파일이 존재하지 않습니다");
		return ;
	}

	if ( m_ctl_image.GetCheck() == 0 )
	{
		Proc_NewImage( strFileName, &ImageData, NULL, NULL, Runmoitor );
		strMsg.Format(L"영상을 정상적으로 수신 하였습니다");
	}
	else
	{
		ret = m_ImgClass.Proc_LPR( &ImageData, &LPRData, &OrgImageData, &CameraData, m_strip_result, CENTER_AREA );
		if (  ret == TRUE  )
		{
			/* 영역 존재 */
			//SendResult_TCP( 0, m_strip_result );
			strMsg.Format(L"영상 분석이 정상적으로 종료 되었습니다.");
		}
		else
		{
			/* 영역 미 존재 */
			if( ret == -3 )
			{
				//SendResult_TCP( 1, m_strip_result );
				strMsg.Format(L"영상 분석이 종료되었으나 스트립이 존재 하지 않는것으로 분석 되었습니다.");
			}			
		}
		
		ptrTempImageData = &OrgImageData;
		Proc_NewImage( strFileName, ptrTempImageData, &LPRData.LPRResult[ 0 ].PlateRect[ 0 ], &LPRData.LPRResult[ 0 ].LPRChars, Runmoitor );

		/* memory 해제 */
		if ( CameraData.ptrSrcImage1D != NULL )
			delete[] CameraData.ptrSrcImage1D;

		m_WizardHelper.FreeImageAllocData( &OrgImageData );		

		for( i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
		{
			ptrCharImage = NULL;
			ptrCharImage = &LPRData.LPRResult[ 0 ].LPRChars.Chars[ i ].ImageData;
			if (ptrCharImage != NULL )
			{
				m_WizardHelper.FreeImageAllocDataWithSize( ptrCharImage,  STRIP_H_SIZE,  STRIP_W_SIZE );
			}
		}
	}
	
	/* 기본 버펀 해제 */
	m_WizardHelper.FreeImageAllocData( &ImageData );
	m_WizardHelper.FreeImageAllocData( &LPRData.LPRResult[ 0 ].TempImdateData );
	//AfxMessageBox(strMsg);
	Beep( 1600, 500 );
	Beep( 1600, 500 );
	Beep( 1600, 500 );
	
}

void CVS_ServerDlg::Proc_NewImage( TCHAR * ptrtcFileName, IMAGE_DATA_t * ptrImageData, CVRECT_t * ptrRect, pLPRCHAR ptrLPRChars, int Runmoitor )
{
	int i;
	int nExtX =0;
	int nExtY =0;
	CClientDC dc( pMainViewe );
	IMAGE_DATA_t ImageData;
	CVRECT_t Rect[ MAX_LPR_AREA_CNT ];
	LPR_CHAR_t LPRChars;
	RECT box_rect ;
	RECT box_rect_p ;

	/* Copy m_ImageData */		

	memset( &ImageData, 0, sizeof( IMAGE_DATA_t ));
	memset( &Rect, 0, sizeof( Rect ));
	memset( &LPRChars, 0, sizeof( LPR_CHAR_t ));

	m_WizardHelper.FreeImageAllocData( &ImageData );
	m_WizardHelper.CopyImageData( &ImageData, ptrImageData, TRUE );
	
	/* Copy PlateRect */
	if ( ptrRect != NULL )
	{
		memcpy( &Rect, ptrRect, sizeof( CVRECT_t) * MAX_LPR_AREA_CNT);
	}

	/* Copy Char  */
	for( i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
	{
		m_WizardHelper.FreeImageAllocData( &LPRChars.Chars[ i ].ImageData );
	}
#if 0
	if ( ptrLPRChars != NULL )
	{	
		if ( ptrLPRChars->nCharCnt > 0 )
		{				
			for( i = 0 ; i < MAX_LPR_CHAR_CNT ; i++ )
			{	
				m_WizardHelper.CopyImageData( &ptrLPRChars->Chars[ i ].ImageData,  &ptrLPRChars->Chars[ i ].ImageData , TRUE);
				memcpy( &ptrLPRChars->Chars[ i ].ChRect , &ptrLPRChars->Chars[ i ].ChRect, sizeof( CVRECT_t) );
			}
		}			
	}
#endif

	//nExtX = box_rect.left + 1;
	//nExtY = box_rect.top;
	nExtX = DLG_LEFT;
	nExtY = DLG_TOP;

	m_WizardHelper.DrawWin32( FALSE, dc.m_hDC, &ImageData, nExtX, nExtY);
	m_WizardHelper.DrawRectLine( dc.m_hDC, ImageData.nH, &Rect[ 0 ],1, nExtX, nExtY);	
	//m_WizardHelper.DrawCharRectLine( dc.m_hDC, ImageData.nH, LPRChars.nCharCnt, &Rect[ 0 ], &LPRChars, 1, nExtX, nExtY );
	DrawCharsImageDlg( dc.m_hDC , ptrLPRChars );
	m_WizardHelper.FreeImageAllocData( &ImageData );
}

void CVS_ServerDlg::DrawCharsImageDlg( HDC hdc, LPR_CHAR_t *ptrLPRChars )
{
	int i;
	int nPreCharY = 0;
	int nPreCharX = 0;
	int nExtX =0;
	int nExtY =0;

	HPEN current_pen, old_pen;
	int nLeft, nRight, nTop, nBottom;	

	RECT TextRect;
	int nHeight, nWidth;
	TCHAR tcText[ MAX_PATH ];
	
	current_pen=CreatePen(PS_SOLID, 1, RGB(255, 0, 0) );
	old_pen=(HPEN)SelectObject(hdc, current_pen);
	
	SelectObject(hdc, GetStockObject(NULL_BRUSH) );	
		
	nExtX = DLG_LEFT + 5;		
	nExtY = DLG_TOP + MAX_IMG_H + 5;
	
	if ( ptrLPRChars == NULL )
		return ;

	if ( ptrLPRChars->nCharCnt > 0 )
	{
		memset( tcText, 0, sizeof( tcText));			
		SetRect(&TextRect, nExtX, nExtY , nExtX + STRIP_W_SIZE * 10 ,nExtY+STRIP_H_SIZE );			
		DrawText( hdc, L"<< STRIP 색상 정보 >>",-1, &TextRect, DT_WORDBREAK);

		nPreCharY = STRIP_H_SIZE + 3;

		for( i = 0 ; i < ptrLPRChars->nCharCnt ; i++ )
		{
			if ( i == 5 )
			{
				nExtX = DLG_LEFT + 5;		
				nExtY = DLG_TOP + MAX_IMG_H + 5;
				nPreCharY = STRIP_H_SIZE + 3;
				nPreCharX = nRight +  STRIP_W_SIZE;
			}

			
			m_WizardHelper.DrawWin32( TRUE, hdc, &ptrLPRChars->Chars[ i ].ImageData, nExtX + nPreCharX , nExtY + nPreCharY );			

			nLeft	= nExtX + nPreCharX + STRIP_W_SIZE + 3;
			nRight	= nLeft + nPreCharX + STRIP_W_SIZE * 8;
			nTop	= nExtY + nPreCharY ;
			nBottom = nExtY + nPreCharY + STRIP_H_SIZE;

			nHeight =  nBottom - nTop;
			nWidth	= nRight - nLeft;

			memset( tcText, 0, sizeof( tcText));
			wsprintfW(tcText, L"[IDX : %02d, H:%d, %s - Level: %d ]" ,i+1, (int)ptrLPRChars->Chars[ i ].ChRect.HSI.H , m_strip_name[ i ].NAME, (int)ptrLPRChars->Chars[ i ].ChRect.HSI.L);	
						
			SetRect(&TextRect, nLeft, nTop, nRight, nBottom );			
			DrawText( hdc, tcText,-1, &TextRect, DT_WORDBREAK);

			nPreCharY += ( ptrLPRChars->Chars[ i ].ImageData.nH + 20 );

		}
	}	

	DeleteObject( SelectObject(hdc, old_pen) );
	DeleteObject( current_pen );
}

int  CVS_ServerDlg::ClientAdd( ClientRecvMsg_t  * ClientRecvMsg )
{
	pMainViewe->AddAlivedClient( ClientRecvMsg->SIPID , ClientRecvMsg->ClientIP,  ClientRecvMsg->ClientMAC );	
	return 0;
}

void CVS_ServerDlg::ChkLEDCnt( void )
{	
	/* tory45 */
	if ( m_x == 0 )
	{
		Display_RUN( m_x );
		m_x = 1;
	}
	else if ( m_x == 1 )
	{
		Display_RUN( m_x );
		m_x = 0;
	}	
}

void * CVS_ServerDlg::GetSeverInfo()
{
	CSVR_Doc * ptrDoc = NULL ;
	VSSERVERINFO_t * ptrServerInfo = NULL ;

	if ( m_Controller != NULL )
	{
		ptrDoc = (CSVR_Doc * )m_Controller->GetSVR_DocHandler();
		if ( ptrDoc != NULL )
		{
			ptrServerInfo = ptrDoc->GetServerInfo();
		}
	}

	return ptrServerInfo;
}

void CVS_ServerDlg::MainIdleProc( void )
{

	if ( m_Controller != NULL )
	{
		m_Controller->Idle();
	}
}

void CVS_ServerDlg::DisplayClientCnt( void )
{
	CString strMsg;
	
	strMsg.Format(L"%d", m_ClientCnt );
	m_CtlClientCnt.SetWindowText( strMsg );
}

void CVS_ServerDlg::AddAlivedClient( int ClientID, TCHAR * strIP, TCHAR * strMAC )
{
	int i ;
	CString strMsg;
	CString stMac;
	CString stReadMac;

	stMac.Format(L"ae:35:ee:ee:be:f6");

	for( i = 0 ; i < MAX_TCP_CLIENT_CNT ; i++ )
	{
		if ( m_AliveClient[ i ].ID == 0 )
		{
			m_AliveClient[ i ].ID = ClientID;
			_tcscpy_s( m_AliveClient[ i ].strIP, strIP );
			_tcscpy_s( m_AliveClient[ i ].strMAC, strMAC );
			stReadMac.Format(L"%s", strMAC );
			
			AddClientList( ClientID , strIP, strMAC );
			m_ClientCnt++;
			DisplayClientCnt();

			if (stMac.CompareNoCase(stReadMac) == 0 )
			{
				m_SelectedID =  ClientID;
			}

//			if ( m_ClientCnt == 1 )
			{
					
				strMsg.Format(L"%d", m_SelectedID);
				//m_CtlSelectedID.SetWindowText( strMsg );
				//m_ImageViewer[ 0 ].nClientID = m_SelectedID;				
			}

			return ;
		}
		else if ( m_AliveClient[ i ].ID == ClientID )
		{
			return ;
		}
	}	
}

void CVS_ServerDlg::DelAlivedClient( int ClientID )
{
	int i, Pos = -1;
	int TotalSize;
	int Cnt =0;

	for( i = 0 ; i < MAX_TCP_CLIENT_CNT ; i++ )
	{
		if ( m_AliveClient[ i ].ID == 0 )
		{
			break;
		}
		else 
		{
			if ( m_AliveClient[ i ].ID == ClientID )
			{
				Pos = i;
			}
			Cnt++;			
		}
	}

	if ( Pos < 0 )
		return;

	TotalSize = ( Cnt - Pos ) * sizeof( AliveClient_t );
	if ( Cnt == 1 )
		memset( &m_AliveClient, 0, sizeof( m_AliveClient ));
	else
		memmove( &m_AliveClient[ Pos ], &m_AliveClient[ Pos + 1 ], TotalSize );

	m_ClientCnt--;
	DisplayClientCnt();

	m_CtlClientList.DeleteAllItems();

	for( i = 0 ; i < MAX_TCP_CLIENT_CNT ; i++ )
	{

		if ( m_AliveClient[ i ].ID == 0 )
		{
			return;
		}
		else
		{
			AddClientList( m_AliveClient[ i ].ID , m_AliveClient[ i ].strIP,  m_AliveClient[ i ].strMAC);
		}
	}	
	
}

BOOL CVS_ServerDlg::isCheckSelected( void )
{
	//m_SelectedID = 1;

	if ( m_SelectedID == 0  )
	{	
		AfxMessageBox(L"통신 할 단말을 먼저 선택하세요");
		return FALSE;
	}

	return TRUE;
}

int CVS_ServerDlg::StringToInt( CString & strInt)
{
	USES_CONVERSION;
	char szTemp[ MAX_PATH ];

	if ( strInt.GetLength() == 0 )
		return -1;

	memset(szTemp, 0 , sizeof( szTemp ) );

	strcpy_s( szTemp, MAX_PATH, W2A( strInt) );
	return atoi( szTemp );
}

DWORD CVS_ServerDlg::ThreadIdle(LPVOID pParam)
{
	CVS_ServerDlg * pThis = ( CVS_ServerDlg *)pParam;

	while( 1 == 1 )
	{
		/*파일 삭제 로직은 서버 일괄처리 서비스에 등록 */
		pThis->MainIdleProc();
		Sleep( 500 );
	}

	return 0L ;
}

void CVS_ServerDlg::OnLvnItemchangedClientList(NMHDR *pNMHDR, LRESULT *pResult)
{
	
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	(void ) pNMLV;
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	*pResult = 0;
}

void CVS_ServerDlg::OnNMDblclkClientList(NMHDR *pNMHDR, LRESULT *pResult)
{	
	int nCnt=0;
	int nCnt2 = 0;
	CString strMsg;
	int nSelectID = 0;
	CString strSelectID ;
	CString strIP;

	NM_LISTVIEW* pNMListView =(NM_LISTVIEW*)pNMHDR;
	nCnt = pNMListView->iItem;

	if (nCnt >= 0)
	{
		nCnt2 = m_CtlClientList.GetItemCount();		
		for ( int i = 0 ; i < nCnt2 ; i++)
		{
			if ( m_CtlClientList.GetItemState(i, LVIS_SELECTED) !=0 )
			{
				strSelectID = m_CtlClientList.GetItemText( i, 1 );
				strIP = m_CtlClientList.GetItemText( i, 2 );

				nSelectID = m_Util.StringToInt( strSelectID );
#if 0
				m_SelectedID = m_AliveClient[ i ].ID;
#else
				m_SelectedID =  nSelectID;
#endif

				strMsg.Format(L"%d", m_SelectedID);
				m_CtlSelectedID.SetWindowText( strMsg );
				m_ImageViewer[ 0 ].nClientID = m_SelectedID;		

				break;
			}
		}
	}

	*pResult = 0;
}


void CVS_ServerDlg::OnBnClickedChkSave()
{
	if ( m_ctl_savelog.GetCheck() )
		m_SaveLog =1;
	else
		m_SaveLog =0;

}


void CVS_ServerDlg::OnBnClickedBtnSort()
{
	int i,j ;
	int Cnt ;
	Cnt = 0;

	int nCur , nNext ;
	nCur = nNext = 0;	

	for( i = 0 ; i < MAX_TCP_CLIENT_CNT ; i++ )
	{
		if ( m_AliveClient[ i ].ID == 0 )
		{
			break;
		}
		else 
		{
			Cnt++;
		}
	}	

	if ( Cnt < 2 )
		return ;

	if ( MessageBox(L"모든 단말이 연결 되었습니까?",L"확인", MB_YESNO ) == IDNO )
	{
		return;
	}

	for ( i = ( Cnt  -1 ) ; i > 0 ; -- i )
	{
		for( j = 0 ; j < i ; ++j )
		{
			nCur = m_AliveClient[ j ].ID;
			nNext = m_AliveClient[ j + 1 ].ID;

			if ( nCur > nNext )
			{
				memset( &m_AliveClient_Temp, 0, sizeof( m_AliveClient_Temp ));
				
				m_AliveClient_Temp[ 0 ].ID = m_AliveClient[ j ].ID;				
				_tcscpy_s( m_AliveClient_Temp[ 0 ].strIP, m_AliveClient[ j ].strIP );
				_tcscpy_s( m_AliveClient_Temp[ 0 ].strMAC, m_AliveClient[ j ].strMAC );
				
				m_AliveClient[ j ].ID = m_AliveClient[ j+1 ].ID;				
				_tcscpy_s( m_AliveClient[ j ].strIP, m_AliveClient[ j+1 ].strIP );
				_tcscpy_s( m_AliveClient[ j ].strMAC, m_AliveClient[ j+1].strMAC );

				m_AliveClient[ j+1 ].ID = m_AliveClient_Temp[ 0 ].ID;				
				_tcscpy_s( m_AliveClient[ j+1 ].strIP, m_AliveClient_Temp[ 0 ].strIP );
				_tcscpy_s( m_AliveClient[ j+1].strMAC, m_AliveClient_Temp[ 0 ].strMAC );

			}
		}
	}

	m_ClientCnt = 0;
	m_CtlClientList.DeleteAllItems();

	for( i = 0 ; i < Cnt ; i++ )
	{
		AddClientList( m_AliveClient[ i ].ID  , m_AliveClient[ i ].strIP,  m_AliveClient[ i ].strMAC );
		m_ClientCnt++;
		DisplayClientCnt();
	}	
}

int CVS_ServerDlg::SendAPData( int size, BYTE * ptrdata )
{
	int nreuse = 1;	
	struct linger ling;
	CString strMsg;	
	WSADATA		m_WsaData;
	SOCKET		m_ServSock;
	SOCKADDR_IN	m_ServAddr;
	TransferConf_t TransferConf;
	struct timeval optVal = {5, 0 };
	int optLen = sizeof( optVal );
	int nTimeOutValue = 5000;

	char recvbuf[ 100 ];

	/* 모든 버퍼를 버리고 즉시 종료 */
	ling.l_onoff =1;
	ling.l_linger = 0;

	if ( ptrdata == NULL || size <=0 )
	{
		return FALSE;
	}

	memset(&TransferConf, 0, sizeof( TransferConf ));

	if (WSAStartup( MAKEWORD( 2, 2 ), &m_WsaData ) != 0)
	{		
		strMsg.Format(L"Error UDP WSAStartup");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );		
		return FALSE;
	}
	if ( GetTransferConfigure( &TransferConf ) != TRUE )
	{
		strMsg.Format(L"Error UDP Info");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );		
		return FALSE;
	}

	m_ServSock = socket( PF_INET, SOCK_DGRAM, 0 );

	//setsockopt( m_ServSock, SOL_SOCKET ,SO_REUSEADDR,  (char*)&nreuse, sizeof(nreuse) );
	//setsockopt( m_ServSock, SOL_SOCKET ,SO_LINGER,  (char*)&ling, sizeof(struct linger) );
	setsockopt( m_ServSock, SOL_SOCKET ,SO_RCVTIMEO, (char*)&nTimeOutValue,  sizeof(nTimeOutValue) );


	if ( m_ServSock == INVALID_SOCKET )
	{		
		strMsg.Format(L"ERROR UDP SOCKET ");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );		
		return FALSE;
	}

	memset( &m_ServAddr, 0, sizeof( m_ServAddr));
	m_ServAddr.sin_family		= AF_INET;
	m_ServAddr.sin_addr.s_addr = inet_addr( TransferConf.szUDPServerIP );
	m_ServAddr.sin_port			= htons( TransferConf.UDPSendPort );

#if 1
	if ( connect( m_ServSock, ( struct sockaddr*)&m_ServAddr, sizeof(m_ServAddr)) == SOCKET_ERROR )
	{			
		shutdown( m_ServSock, SD_BOTH );
		closesocket( m_ServSock );
		WSACleanup();
		strMsg.Format(L"UDP Connect Error...");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );		
		return FALSE;
	}
#endif

	int Rect = send( m_ServSock, ( char *)ptrdata, size, 0);
	int recvSize =0;
	int i;

	if ( Rect >0 )
	{
		memset( recvbuf, 0, 100);	
		recvSize = recv( m_ServSock , recvbuf, 100, 0 );	

		if ( recvSize <= 0 )
		{
			return FALSE;						
		}
		
	}
	
	return TRUE;
}


int CVS_ServerDlg::SendBroadCastData( int size, BYTE * ptrdata )
{
	int nreuse = 1;	
	struct linger ling;
	CString strMsg;	
	WSADATA		m_WsaData;
	SOCKET		m_ServSock;
	SOCKADDR_IN	m_ServAddr;
	TransferConf_t TransferConf;
	struct timeval optVal = {5, 0 };
	int optLen = sizeof( optVal );
	int fBroadcast = 1;

	char recvbuf[ 100 ];
	DWORD nMyIP, nMySubMask, nTemp1, nTemp2;

	/* 모든 버퍼를 버리고 즉시 종료 */
	ling.l_onoff =1;
	ling.l_linger = 0;

	if ( ptrdata == NULL || size <=0 )
	{
		return FALSE;
	}

	memset(&TransferConf, 0, sizeof( TransferConf ));

	if (WSAStartup( MAKEWORD( 2, 2 ), &m_WsaData ) != 0)
	{		
		strMsg.Format(L"Error UDP WSAStartup");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );		
		return FALSE;
	}
	if ( GetTransferConfigure( &TransferConf ) != TRUE )
	{
		strMsg.Format(L"Error UDP Info");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );		
		return FALSE;
	}

	m_ServSock = socket( PF_INET, SOCK_DGRAM, IPPROTO_UDP );

	//setsockopt( m_ServSock, SOL_SOCKET ,SO_REUSEADDR,  (char*)&nreuse, sizeof(nreuse) );
	//setsockopt( m_ServSock, SOL_SOCKET ,SO_LINGER,  (char*)&ling, sizeof(struct linger) );
	setsockopt( m_ServSock, SOL_SOCKET ,SO_BROADCAST, (char*)&fBroadcast,  sizeof(fBroadcast) );


	if ( m_ServSock == INVALID_SOCKET )
	{		
		strMsg.Format(L"ERROR UDP SOCKET ");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );		
		return FALSE;
	}
	
	nMyIP = inet_addr(TransferConf.szServerIP);
	//nMySubMask	= inet_addr("255.255.255.0");
	nMySubMask	= inet_addr(TransferConf.szNetmask);
	nTemp1		= nMyIP & nMySubMask;			
	nTemp2		= nMySubMask ^ 0xFFFFFFFF;

	memset( &m_ServAddr, 0, sizeof( m_ServAddr));
	m_ServAddr.sin_family		= AF_INET;	
	m_ServAddr.sin_port			= htons( TransferConf.UDPSendPort );
	//m_ServAddr.sin_addr.s_addr = inet_addr( "255.255.255.255" );
	m_ServAddr.sin_addr.s_addr = nTemp1 + nTemp2;


#if 1
	if ( connect( m_ServSock, ( struct sockaddr*)&m_ServAddr, sizeof(m_ServAddr)) == SOCKET_ERROR )
	{			
		shutdown( m_ServSock, SD_BOTH );
		closesocket( m_ServSock );
		WSACleanup();
		strMsg.Format(L"UDP Connect Error...");
		strMsg.ReleaseBuffer();
		DumpStr( strMsg );		
		return FALSE;
	}
#endif

	int Rect = send( m_ServSock, ( char *)ptrdata, size, 0);
	int recvSize =0;
	int i;
#if 0
	if ( Rect >0 )
	{
		memset( recvbuf, 0, 100);	
		recvSize = recv( m_ServSock , recvbuf, 100, 0 );	

		if ( recvSize <= 0 )
		{
			return FALSE;						
		}
		
	}
#endif
	return TRUE;
}

void CVS_ServerDlg::OnBnClickedButton8()
{
	VSSERVERINFO_t * ptrServerInfo = NULL;
	TCHAR strOrgFile[ MAX_PATH ];
	TCHAR strNewFile[ MAX_PATH ];
	CString strOrg;
	CString strNew;
	CClientDC dc( pMainViewe );

	ptrServerInfo= (VSSERVERINFO_t*)( GetSeverInfo());
	
	memset( strOrgFile, 0, sizeof( strOrgFile));
	memset( strNewFile, 0, sizeof( strNewFile));

	m_ctrl_org_file.GetWindowTextW(strOrg);
	m_ctrl_new_file.GetWindowTextW(strNew);

	if (  ptrServerInfo != NULL )
	{
		wsprintf( strOrgFile, L"%s\\%s", ptrServerInfo->csImageSavePath,  (TCHAR*)(LPCTSTR)strOrg );
		wsprintf( strNewFile, L"%s\\%s.jpg", ptrServerInfo->csImageSavePath,  (TCHAR*)(LPCTSTR)strNew );
		if ( wcslen( strNewFile ) > 0 )
		{
			if ( _waccess_s(strNewFile, 0) != 0 )
			{
				CFile::Rename( strOrgFile, strNewFile );
				//Invalidate(true);
			}
			else
				AfxMessageBox(L"변경할 파일명이 이미지 존재합니다");

		}
	}
}



void CVS_ServerDlg::OnBnClickedButton9()
{
	ClientSendMsg_t ClientSendMsg;
	net_info_t net_info;

	CString strMsg;
	TCHAR ip[ MAX_IP_SIZE ];
	TCHAR port [ MAX_IP_SIZE ];
	char szport[ MAX_IP_SIZE ];

	memset(&net_info, 0, sizeof( net_info ));
	memset( ip, 0, sizeof( ip ));
	memset( port, 0, sizeof( port ));
	memset( szport, 0, sizeof( szport ));
	memset( &ClientSendMsg, 0, sizeof( ClientSendMsg_t));
		
	m_CtlServerIP.GetWindowText( ip,sizeof( ip ));
	m_ctl_port.GetWindowText( port, sizeof( port ));			
	
	m_Util.WcsToMcs(ip,net_info.server_ip, wcslen( ip ));
	m_Util.WcsToMcs(port,szport, wcslen( port ));
	net_info.server_port = (short)atoi( szport);

		
	ClientSendMsg.Msg   = SOCK_TEST_MNG_CON_MSG;
	ClientSendMsg.SIPID = m_SelectedID;
	
	if ( m_Message.MakeReqTEST_MNG_CON(ClientSendMsg.Msg, m_SelectedID , &ClientSendMsg, &net_info ) == TRUE )
	{
		if ( SendBroadCastData( ClientSendMsg.Size, ClientSendMsg.SendBuf ) == TRUE )
		{
			Init_Processtime( -1 );	
			AfxMessageBox(L"전송 성공");
		}
		else
		{
			Init_Processtime( -1 );	
			AfxMessageBox(L"전송 실패");
		}
	}
}

void CVS_ServerDlg::OnCbnSelchangeComGpioType()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CVS_ServerDlg::OnCbnSelchangeComOnoff2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}



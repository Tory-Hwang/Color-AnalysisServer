// VS_ServerDlg.h : header file
//

#pragma once

#include "Com_Def.h"
#include "Admin_Dlg.h"
#include "Login_Dlg.h"
#include "Search_Dlg.h"

#include "IViewer.h"
#include "IController.h"
#include "IModel.h"
#include "ITransfer.h"
#include "Util.h"
#include "Message.h"
#include "MsgController.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "../ImageProcessing/Img_Proc.h"
#include "../ImageProcessing/UTIL/WizardHelper.h"

//#include "xImage.h"

#define MAX_IMAGE_VIEWER_CNT	1

// CVS_ServerDlg dialog

typedef struct TempClient_
{
	TCHAR cID[ 10 ];
	TCHAR cState[ 20 ];
}TempClient_t;

typedef struct TempClientList_
{
	int nCnt;
	TempClient_t TempClistList[ MAX_TCP_CLIENT_CNT ];
}TempClientList_t;


typedef struct AliveClient_
{
	int ID;	
	TCHAR strIP[ MAX_IP_SIZE ];
	TCHAR strMAC[ MAX_MAC_SIZE ];

}AliveClient_t;

typedef struct ImageViewr_
{
	int nClientID;
	//CxImage cxImage;
	TCHAR csFullPath[ MAX_PATH ];
	TCHAR csSeeTime[ MAX_TIME_SIZE + 10 ];	
}ImageViewer_t;

class CVS_ServerDlg : public CDialog, public IViewer
{
// Construction
public:
	CVS_ServerDlg(CWnd* pParent = NULL);	// standard constructor


	enum { IDD = IDD_VS_SERVER_DIALOG };

	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


public:

	/* Inheritance Interface */
	virtual BOOL SetControllerHandler( void *  ControllerHandle );
	virtual BOOL AddModelHandler( int ModelKind, void * ModelHandle );	
	virtual int OnRecvMsg( MsgInfo_t *MsgInfo );

private:

	/* Dialog */
	CAdmin_Dlg m_AdminDlg;
	CLogin_Dlg m_LoginDlg;
	CSearch_Dlg m_SearchDlg;
	
	IController * m_Controller;
	ServerConf_t m_ServerConf;
	CUtil m_Util;
	CMessage m_Message;
	CMsgController m_MsgController;		
	AliveClient_t m_AliveClient[ MAX_TCP_CLIENT_CNT ];
	AliveClient_t m_AliveClient_Temp[ 1 ];

	/* image processing */
	CImg_Proc m_ImgClass;
	CWizardHelper m_WizardHelper;
	STRIN_NAME_t  m_strip_name[ MAX_LPR_CHAR_CNT  ];
	BYTE m_strip_result[  MAX_LPR_CHAR_CNT ];

	void Proc_NewImage( TCHAR * ptrtcFileName, IMAGE_DATA_t * ptrImageData, CVRECT_t * ptrRect, pLPRCHAR ptrLPRChars, int Runmoitor );	
	void DrawCharsImageDlg( HDC hdc , LPR_CHAR_t *ptrLPRChars  );

	int SendMulty_Motor(  TCHAR * ptrPWM1, TCHAR *ptrTime1, TCHAR * ptrPWM2, TCHAR *ptrTime2, BYTE bonoff1, BYTE bonoff2 );
	int SendResult_TCP( int Err, BYTE * ptrResult );

	volatile int m_ClientCnt;
	int m_SelectedID;
	int m_nHeight ;
	CFont LargeFont;
	CBitmap m_Red;
	CBitmap m_Green;
	int m_Process_time;
	BYTE * m_ptrImageBuf;
	int m_revc_size;
	UINT32 m_cnt;

	BYTE m_x;
	BYTE m_x1;
	BYTE m_x2;
	BYTE m_x3;
	BYTE m_x4;

	volatile int m_LineCnt;
	volatile int m_XLineCnt;	
	ImageViewer_t m_ImageViewer[ MAX_IMAGE_VIEWER_CNT ];
	
	int m_SaveLog;
	int m_Preday;
	TCHAR m_strmLogDir[ MAX_PATH ];

	TempClientList_t m_TempClients;
	
	void DumpStr( CString &str );
	void InitDlg( void );

	void InitLogList( void );
	void AddLogList( CString & strMsg );
	void AddDisLogList( CString & strMsg );

	void ClearLogList( void );

	void ReleaseDlg( void );
	void DisplayClientCnt( void );

	void AddAlivedClient( int ClientID, TCHAR * szIP, TCHAR *strMAC );
	void DelAlivedClient( int ClientID );

	BOOL GetModelConfigure( ModelConf_t * ModelConf );
	BOOL GetTransferConfigure( TransferConf_t * TransferConf );
	BOOL GetServerConfigure( void );	
	void Internal_SendMsg( ClientSendMsg_t * ClientSendMsg );

	void AddClientList( int ID, TCHAR *strIP,   TCHAR *strMAC);
	void MainIdleProc( void );
	
	BOOL isCheckSelected( void );	
	int StringToInt( CString & strInt);		
	void Display_Sts( Statusinfo_t * ptrSTS );
	void Display_LINK( int val );
	void Display_RUN( int val );

	void Init_Processtime( int val );
	void Set_Processtime( );

	/* VOIP Server */
	int SaveLog( UINT32 Cur_Time, TCHAR * strLog );
	void MakeLogDir( void );
	BOOL Savebin_Append( TCHAR *prtPath , TCHAR * ptrstr);
	int CompareMAC( int org_num, CString &strOrgMAC );
	int DisplayLEDStatus( int ClientID, BYTE onoff );

	int DisplayGPIOStatus( int ClientID, BYTE type, BYTE onoff );
	
	void ChkLEDCnt( void );
	void * GetSeverInfo( void );
	void DisplayImage( char *ptrszname );
	int SendAPData( int size, BYTE * ptrdata );
	int SendBroadCastData( int size, BYTE * ptrdata );

	/* MessageController용 등록 함수 */
	static int DummyTCPCloseMsg( DummyMsg_t * DummyMsg );
	static int ClientAdd( ClientRecvMsg_t  * ClientRecvMsg );
	static int ClientRecvImg( ClientRecvMsg_t  * ClientRecvMsg );
	static int ClientRecvPlate( ClientRecvMsg_t  * ClientRecvMsg );
	static int ClientRecvLOG( ClientRecvMsg_t  * ClientRecvMsg );

	//static int ClientRespMsg( ClientRecvMsg_t * ClientRecvMsg );
	static DWORD WINAPI ThreadIdle(LPVOID pParam);
	HANDLE m_hThread;
	DWORD m_dwThread;
	
	CRITICAL_SECTION _DLGLock;
	int Lock();
	int UnLock();	

protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	
	
	afx_msg void OnBnClickedBtnClose();
	afx_msg void OnClose();
	
	CIPAddressCtrl m_CtlServerIP;
	CStatic m_TCPConnectState;
	
	CListCtrl m_CtlLogList;
	afx_msg void OnBnClickedBtnClear();	
	afx_msg void OnBnClickedBtnEtc3();	
	CStatic m_CtlServerID;
	CStatic m_CtlClientCnt;
	CListCtrl m_CtlClientList;
	afx_msg void OnNMClickClientList(NMHDR *pNMHDR, LRESULT *pResult);
	CStatic m_CtlSelectedID;
	
	afx_msg void OnLvnItemchangedClientList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkClientList(NMHDR *pNMHDR, LRESULT *pResult);	
	
	afx_msg void OnBnClickedBtnEtc4();
	afx_msg void OnBnClickedBtnEtc5();
	afx_msg void OnBnClickedBtnEtc6();
	afx_msg void OnBnClickedChkSave();
	CButton m_ctl_savelog;
	afx_msg void OnBnClickedBtnEtc7();
	afx_msg void OnBnClickedBtnWatch();
	afx_msg void OnBnClickedBtnMac();
	CEdit m_ctl_time;
	CStatic m_ctl_version;
	afx_msg void OnBnClickedBtnSort();
	CListCtrl m_list_discon;
	CButton m_ctl_mac_all;
	CComboBox m_com_vol;
	afx_msg void OnBnClickedBtnVol();
	CComboBox m_com_onoff;
	afx_msg void OnBnClickedBtnLedGet();
	CButton m_ctl_led_all;
	afx_msg void OnBnClickedBtnLedSet();
	CStatic m_ctl_on_cnt;
	CStatic m_ctl_off_cnt;
	afx_msg void OnBnClickedBtnCallReady();
	CComboBox m_com_gpio_type;
	CComboBox m_com_gpio_onoff;
	afx_msg void OnBnClickedBtnGpioGet();
	afx_msg void OnBnClickedBtnGpioSet();
	afx_msg void OnCbnSelchangeComGpioType();
	CEdit m_ctl_key;
	CEdit m_ctl_ssid;
	CEdit m_ctl_port;
	CEdit m_ctl_udp_port;
	CIPAddressCtrl m_ctl_updserverip;
	CComboBox m_ctlwifi_type;
	afx_msg void OnCbnSelchangeComOnoff2();
	CComboBox m_ctl_testmode;
	CEdit m_motor_pwm;
	CEdit m_ctl_light_pwm;
	afx_msg void OnBnClickedBtnEtc8();
	CButton m_ctl_image;
	CStatic m_ctl_img_box;
	CEdit m_motor_time;
	afx_msg void OnBnClickedButton3();
	CEdit m_ctl_plate_pwm;
	CEdit m_ctl_plate_time;
	CEdit m_ctl_temp_pwm;
	CEdit m_ctl_temp_time;
	CComboBox m_mult_onoff;
	afx_msg void OnBnClickedButton4();
	CEdit m_plt1_pwm;
	CEdit m_plt1_time;
	CEdit m_plt2_pwm;
	CEdit m_plt2_time;
	CComboBox m_mullt1_onoff;
	CEdit m_plt2_1_pwm;
	CEdit m_plt2_1_time;
	CEdit m_plt2_2_pwm;
	CEdit m_plt2_2_time;
	CComboBox m_mullt2_onoff;
	CEdit m_plt3_1_pwm;
	CEdit m_plt3_1_time;
	CEdit m_plt3_2_pwm;
	CEdit m_plt3_2_time;
	CComboBox m_mullt3_onoff;
	CEdit m_plt4_1_pwm;
	CEdit m_plt4_1_time;
	CEdit m_plt4_2_pwm;
	CEdit m_plt4_2_time;
	CComboBox m_mullt4_onoff;
	CEdit m_plt5_1_pwm;
	CEdit m_plt5_1_time;
	CEdit m_plt5_2_pwm;
	CEdit m_plt5_2_time;
	CComboBox m_mullt5_onoff;
	CComboBox m_mullt2_1_onoff;
	CComboBox m_mullt2_2_onoff;
	CComboBox m_mullt2_3_onoff;
	CComboBox m_mullt2_4_onoff;
	CComboBox m_mullt2_5_onoff;
	CComboBox m_mult2_onoff;
	CComboBox m_kind_audio;
	CEdit m_ctl_img_num;
	afx_msg void OnBnClickedBtnCallReadyAws();
	CEdit m_ctl_aws_port;
	CEdit m_ctl_aws_host;
	CEdit m_ctl_aws_id;
	CComboBox m_ctl_pub;
	afx_msg void OnBnClickedBtnEtc9();
	CComboBox m_ctl_dev_num;
	CEdit m_plt6_1_pwm;
	CEdit m_plt6_2_pwm;
	CEdit m_plt7_1_pwm;
	CEdit m_plt7_2_pwm;
	CEdit m_plt8_1_pwm;
	CEdit m_plt8_2_pwm;
	CEdit m_plt6_1_time;
	CEdit m_plt6_2_time;
	CEdit m_plt7_1_time;
	CEdit m_plt7_2_time;
	CEdit m_plt8_1_time;
	CEdit m_plt8_2_time;
	CComboBox m_mullt6_onoff;
	CComboBox m_mullt2_6_onoff;
	CComboBox m_mullt7_onoff;
	CComboBox m_mullt2_7_onoff;
	CComboBox m_mullt8_onoff;
	CComboBox m_mullt2_8_onoff;
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton7();
	afx_msg void OnBnClickedButton6();
	CEdit m_ctrl_org_file;
	CEdit m_ctrl_new_file;
	afx_msg void OnBnClickedButton8();
	afx_msg void OnBnClickedButton9();
};

#pragma once

#define MAX_DB_NAME_SIZE		20
#define MAX_DB_CODE_SIZE		20
#define MAX_DB_ID_SIZE			20
#define MAX_DB_PWD_SIZE			20
#define MAX_DB_TEL_SIZE			20
#define MAX_DB_INFO_SIZE		200
#define MAX_DB_DATE_SIZE		25
#define MAX_DB_ERR_CODE_SIZE	5
#define MAX_DB_CAR_NUM_SIZE		12
#define MAX_DB_IP_SIZE			16
#define MAX_DB_FLOOR_SIZE		4
#define MAX_DB_ZONE_SIZE		3
#define MAX_DB_SIDE_SIZE		3
#define MAX_DB_STRING_CONDITION_SIZE	2048
#define MAX_PRE_LOCATION_CNT	10

// ���� ��ġ ���� �ý��� ��� ��ü ����
typedef struct _VST_COMPANY
{
	int		nID;								// ��ü ID
	TCHAR	csName[MAX_DB_NAME_SIZE];			// ��ü �̸�
	TCHAR	csTel[MAX_DB_TEL_SIZE];				// ��ü ��ǥ ��ȣ
	_RecordsetPtr  RS;
}VST_COMPANY, *PVST_COMPANY;

// ���� ��ġ ���� ���� ȯ�� ���� ����
typedef struct _VST_SVR_CONF
{
	int	nID;								// ���� ID
	int	nViewCnt;							// ������ Viewer View ����(1, 4, 9, 16 )
	TCHAR csTCPIP[MAX_DB_IP_SIZE];			// TCP IP
	int	nTCPPort;							// TCP Listen Port
	int	nRunType;							// 1:��ȹ����, 2:�����
	int nCompanyID;							// ��� ��ü ���̵�
	_RecordsetPtr  RS;
}VST_SVR_CONF, *PVST_SVR_CONF;

// ���� �Ǵ� �� ��ġ ����
typedef struct _VST_LOCATION
{
	TCHAR	csCode[MAX_DB_CODE_SIZE];			// ��ġ ���� �ڵ�. ��-����-ũ�� (P01-AA-01)
	TCHAR	csFloor[MAX_DB_FLOOR_SIZE];			// �� ���� (����:M, ����:P) + (0~99)
	TCHAR	csZone[MAX_DB_ZONE_SIZE];			// ���� ����(AA~ZZ)
	TCHAR	csSide[MAX_DB_SIDE_SIZE];			// �� ���� (01~99)
	int		nMaxParkingCnt;						// �ִ� ���� ���(����� ���)
	int		nCurParkingCnt;						// ���� ���� ���(����� ���)
	int		nLocationType;						// Location Type ( 1 : �Ա�, 2 : ��� : 3 : �ⱸ )
	TCHAR	csPreCode1[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csPreCode2[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csPreCode3[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csPreCode4[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csPreCode5[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csPreCode6[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csPreCode7[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csPreCode8[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csPreCode9[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csPreCode10[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csActive1[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csActive2[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csActive3[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csActive4[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csActive5[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csActive6[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csActive7[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csActive8[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csActive9[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csActive10[MAX_DB_CODE_SIZE];		// ���� ���� ��ġ ��-����-ũ�� (P01-AA-01)
	TCHAR	csInfo[MAX_DB_INFO_SIZE];			// ��ġ ����
	_RecordsetPtr  RS;
}VST_LOCATION, *PVST_LOCATION;

// ���� ��ġ ���� �ý��� ����� ����
typedef struct _VST_USER
{
	TCHAR	csID[MAX_DB_ID_SIZE];				// ������ ID
	TCHAR	csPassword[MAX_DB_PWD_SIZE];			// ������ Password
	int		nLevel;								// ������ ���
	TCHAR	csName[MAX_DB_NAME_SIZE];			// ������ �̸�
	TCHAR	csTel[MAX_DB_NAME_SIZE];			// ������ ��ȭ��ȣ
	TCHAR	csCreaterID[MAX_DB_ID_SIZE];		// ������ ID
	_RecordsetPtr  RS;
}VST_USER, *PVST_USER;

// ���� �ڵ� ����
typedef struct _VST_ERR_CODE
{
	TCHAR	csCode[MAX_DB_ERR_CODE_SIZE];		// �����ڵ�
	TCHAR	csInfo[MAX_DB_INFO_SIZE];			// ���� ����
	_RecordsetPtr  RS;
}VST_ERR_CODE, *PVST_ERR_CODE;

// ���� ��ġ���� �ý��� ���� ����
typedef struct _VST_SYS_CODE
{
	TCHAR	cCode;								// System �ڵ�(S:����, V:Vision Sensor, K:KIOSK)
	TCHAR	csInfo[MAX_DB_INFO_SIZE];			// �ý��� ����
	_RecordsetPtr  RS;
}VST_SYS_CODE, *PVST_SYS_CODE;

// Vision Sensor ����
typedef struct _VST_VS
{
	int		nID;								// Vision Sensor ID(1~60,000)
	int		nServerID;							// ������ ���� ID
	int		nType;								// ����(1:��ȹ, 2:��)
	TCHAR	csLocationCode[MAX_DB_CODE_SIZE];	// ��ġ ����
	TCHAR	csIP[MAX_DB_IP_SIZE];				// Vision Sensor IP
	TCHAR	csCreaterID[ MAX_DB_ID_SIZE ];		// ������ ID
	_RecordsetPtr  RS;
}VST_VS, *PVST_VS;

// KIOSK ����
typedef struct _VST_KIOSK
{
	int		nID;								// KIOSK ID(1~60,000)
	int		nServerID;							// ������ ���� ID
	TCHAR	csLocationCode[MAX_DB_CODE_SIZE];	// ��ġ ����
	TCHAR	csIP[MAX_DB_IP_SIZE];				// KIOSK IP
	TCHAR	csCreaterID[MAX_DB_ID_SIZE];		// ������ ID
	int		nCPURate;							// ���� CPU ����(0~100)
	int		nRAMRate;							// ���� RAM ����(0~100)
	_RecordsetPtr  RS;
}VST_KIOSK, *PVST_KIOSK;

// Vision Sensor Event �̷� ����
typedef struct _VST_VS_CAR
{
	int		nID;								// Vision Sensor ID
	int		nServerID;
	TCHAR	csCarNum[MAX_DB_CAR_NUM_SIZE];		// �νĵ� ���� ��ȣ
	TCHAR	csSeeTime[MAX_DB_DATE_SIZE];		// ���� �ν� �ð�
	TCHAR	csIMGPath[ MAX_PATH ];				// ���� �ν� �̹��� ���	
	TCHAR   csVSLocation[ MAX_DB_CODE_SIZE];
	TCHAR	csFirstInputTime[MAX_DB_DATE_SIZE];
	TCHAR   csVSLocationInfo[ MAX_DB_INFO_SIZE];
	TCHAR   csDBCar_SeeTime[MAX_DB_DATE_SIZE ];
	_RecordsetPtr  RS;
}VST_VS_CAR, *PVST_VS_CAR;


// Vision Sensor Event �̷� ����
typedef struct _VST_VS_EVT_LOG
{
	int		nID;								// Vision Sensor ID
	int		nServerID;
	TCHAR	csCarNum[MAX_DB_CAR_NUM_SIZE];		// �νĵ� ���� ��ȣ
	TCHAR	csSeeTime[MAX_DB_DATE_SIZE];		// ���� �ν� �ð�
	TCHAR	csIMGPath[ MAX_PATH ];				// ���� �ν� �̹��� ���
	int		nIMGLocation;	
	TCHAR   csVSLocation[ MAX_DB_CODE_SIZE];
	TCHAR	csFirstInputTime[MAX_DB_DATE_SIZE];
	TCHAR	csDeleteTime[MAX_DB_DATE_SIZE];
	TCHAR	CreateTime[MAX_DB_DATE_SIZE];
	TCHAR	csStartTime[MAX_DB_DATE_SIZE];
	TCHAR	csEndTime[MAX_DB_DATE_SIZE];
	TCHAR   csFloor[ MAX_DB_CODE_SIZE];
	TCHAR   csZone[ MAX_DB_CODE_SIZE];
	int     nPageStart;
	int		nPageCount;
	_RecordsetPtr  RS;
}VST_VS_EVT_LOG, *PVST_VS_EVT_LOG;

// KIOSK Event �̷� ����
typedef struct _VST_KIOSK_EVT_LOG
{
	int		nID;								// KIOSK ID; (0~100)
	int		nServerID;
	int		nCPURate;							// CPU����(0~100)
	int		nRAMRate;							// RAM����(0~100)	
	TCHAR	csCreateTime;						// ���� �ð�
	_RecordsetPtr  RS;
}VST_KIOSK_EVT_LOG, *PVST_KIOSK_EVT_LOG;

// ����� �α��� �̷� ����
typedef struct _VST_LOGIN_LOG
{
	int		nServerID;
	int		nSessionKey;						// �α��� ���� ����(4byte nand��)
	TCHAR	csUserID[MAX_DB_ID_SIZE];			// ����� ID
	TCHAR	csLoginTime[MAX_DB_DATE_SIZE];		// �α��� �ð�
	TCHAR	csLogoffTime[MAX_DB_DATE_SIZE];		// �α׿��� �ð�
	_RecordsetPtr  RS;
}VST_LOGIN_LOG, *PVST_LOGIN_LOG;

// ���� ���� �̷� ����
typedef struct _VST_NETWORK_LOG
{
	int		nServerID;
	TCHAR	cSYSCode;							// SYSTEM �ڵ�(S:����, V:VisionSensor, K:KIOSK)
	int		nID;								// �ý��� �� ID(Server == 0)
	TCHAR	cConnectState;								// C:Connect, D:Disconnect
	short	btMessageType;						// �޼��� ����
	TCHAR	csDate[MAX_DB_DATE_SIZE];			// ���� �ð�
	_RecordsetPtr  RS;
}VST_NETWORK_LOG, *PVST_NETWORK_LOG;

// �ֱ� ���� ����
typedef struct _VST_ERR
{	
	int		nServerID;
	TCHAR	cSYSCode;							// SYSTEM �ڵ�(S:����, V:VisionSensor, K:KIOSK)
	int		nID;								// �ý��� �� ID(Server == 0)
	TCHAR	csErrCode[MAX_DB_ERR_CODE_SIZE];			// ���� �ڵ�
	_RecordsetPtr  RS;
}VST_ERR, *PVST_ERR;

// ���� �̷� ����
typedef struct _VST_ERR_LOG
{
	int		nServerID;
	TCHAR	cSYSCode;							// SYSTEM �ڵ�(S:����, V:VisionSensor, K:KIOSK)
	int		nID;								// �ý��� �� ID(Server == 0)
	TCHAR	csErrCode[MAX_DB_ERR_CODE_SIZE];	// ���� �ڵ�
	TCHAR	csInfo[ MAX_DB_INFO_SIZE ];
	TCHAR	csCreateTime[MAX_DB_DATE_SIZE];		// ���� �ð�
	_RecordsetPtr  RS;
}VST_ERR_LOG, *PVST_ERR_LOG;

/* ��¥ ��ȸ, ������ ���� ���� ����ü */
typedef struct _VST_DATE_CONDITION
{
	int		nServerID;
	TCHAR	csBeginTime[MAX_DB_DATE_SIZE];		// ���� �ð�
	TCHAR	csEndTime[MAX_DB_DATE_SIZE];		// ���� �ð�
	_RecordsetPtr  RS;
}VST_DATE_CONDITION, *PVST_DATE_CONDITION;

/* ���� ��ȸ�� ���� ���� ����ü */
typedef struct _VST_VS_CAR_CONDITION
{	
	TCHAR csCarNum[ MAX_DB_CAR_NUM_SIZE ];	
	TCHAR csBeginTime[MAX_DB_DATE_SIZE];		// ���� �ð�
	TCHAR csEndTime[MAX_DB_DATE_SIZE];		// ���� �ð�
	int nPageStart;
	int nPageCount;	
	_RecordsetPtr  RS;
}VST_VS_CAR_CONDITION, *PVST_VS_CAR_CONDITION;

/* ���� ��ȸ�� ���� ���� ����ü */
typedef struct _VST_MONITORING
{	
	int		nServerID;
	TCHAR	cSYSCode;							// SYSTEM �ڵ�(S:����, V:VisionSensor, K:KIOSK)
	TCHAR	csWorkDesc[MAX_DB_INFO_SIZE ];		// ���� �ð�
	_RecordsetPtr  RS;
}VST_MONITORING, *PVST_MONITORING;

typedef struct LocationCode_
{
	TCHAR Code[ MAX_DB_CODE_SIZE ];
}LocationCode_t;

typedef struct PreLocation_
{
	int LocationType;
	int PreCnt;
	LocationCode_t Pre[ MAX_PRE_LOCATION_CNT ];
}PreLocation_t;


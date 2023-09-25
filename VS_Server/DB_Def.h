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

// 차량 위치 정보 시스템 사용 업체 정보
typedef struct _VST_COMPANY
{
	int		nID;								// 업체 ID
	TCHAR	csName[MAX_DB_NAME_SIZE];			// 업체 이름
	TCHAR	csTel[MAX_DB_TEL_SIZE];				// 업체 대표 번호
	_RecordsetPtr  RS;
}VST_COMPANY, *PVST_COMPANY;

// 차량 위치 정보 서버 환경 설정 정보
typedef struct _VST_SVR_CONF
{
	int	nID;								// 서버 ID
	int	nViewCnt;							// 차량용 Viewer View 개수(1, 4, 9, 16 )
	TCHAR csTCPIP[MAX_DB_IP_SIZE];			// TCP IP
	int	nTCPPort;							// TCP Listen Port
	int	nRunType;							// 1:구획단위, 2:면단위
	int nCompanyID;							// 사용 업체 아이디
	_RecordsetPtr  RS;
}VST_SVR_CONF, *PVST_SVR_CONF;

// 구역 또는 면 위치 정보
typedef struct _VST_LOCATION
{
	TCHAR	csCode[MAX_DB_CODE_SIZE];			// 위치 정보 코드. 층-구역-크기 (P01-AA-01)
	TCHAR	csFloor[MAX_DB_FLOOR_SIZE];			// 층 정보 (지하:M, 지상:P) + (0~99)
	TCHAR	csZone[MAX_DB_ZONE_SIZE];			// 구역 정보(AA~ZZ)
	TCHAR	csSide[MAX_DB_SIDE_SIZE];			// 면 정보 (01~99)
	int		nMaxParkingCnt;						// 최대 주차 대수(면단위 사용)
	int		nCurParkingCnt;						// 현재 주차 대수(면단위 사용)
	int		nLocationType;						// Location Type ( 1 : 입구, 2 : 통로 : 3 : 출구 )
	TCHAR	csPreCode1[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csPreCode2[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csPreCode3[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csPreCode4[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csPreCode5[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csPreCode6[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csPreCode7[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csPreCode8[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csPreCode9[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csPreCode10[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csActive1[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csActive2[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csActive3[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csActive4[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csActive5[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csActive6[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csActive7[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csActive8[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csActive9[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csActive10[MAX_DB_CODE_SIZE];		// 이전 진입 위치 층-구역-크기 (P01-AA-01)
	TCHAR	csInfo[MAX_DB_INFO_SIZE];			// 위치 설명
	_RecordsetPtr  RS;
}VST_LOCATION, *PVST_LOCATION;

// 차량 위치 정보 시스템 사용자 정보
typedef struct _VST_USER
{
	TCHAR	csID[MAX_DB_ID_SIZE];				// 관리자 ID
	TCHAR	csPassword[MAX_DB_PWD_SIZE];			// 관리자 Password
	int		nLevel;								// 관리자 등급
	TCHAR	csName[MAX_DB_NAME_SIZE];			// 관리자 이름
	TCHAR	csTel[MAX_DB_NAME_SIZE];			// 관리자 전화번호
	TCHAR	csCreaterID[MAX_DB_ID_SIZE];		// 생성자 ID
	_RecordsetPtr  RS;
}VST_USER, *PVST_USER;

// 에러 코드 정보
typedef struct _VST_ERR_CODE
{
	TCHAR	csCode[MAX_DB_ERR_CODE_SIZE];		// 에러코드
	TCHAR	csInfo[MAX_DB_INFO_SIZE];			// 에러 설명
	_RecordsetPtr  RS;
}VST_ERR_CODE, *PVST_ERR_CODE;

// 차량 위치정보 시스템 종류 정보
typedef struct _VST_SYS_CODE
{
	TCHAR	cCode;								// System 코드(S:서버, V:Vision Sensor, K:KIOSK)
	TCHAR	csInfo[MAX_DB_INFO_SIZE];			// 시스템 설명
	_RecordsetPtr  RS;
}VST_SYS_CODE, *PVST_SYS_CODE;

// Vision Sensor 정보
typedef struct _VST_VS
{
	int		nID;								// Vision Sensor ID(1~60,000)
	int		nServerID;							// 접속할 서버 ID
	int		nType;								// 운영모드(1:구획, 2:면)
	TCHAR	csLocationCode[MAX_DB_CODE_SIZE];	// 위치 정보
	TCHAR	csIP[MAX_DB_IP_SIZE];				// Vision Sensor IP
	TCHAR	csCreaterID[ MAX_DB_ID_SIZE ];		// 생성자 ID
	_RecordsetPtr  RS;
}VST_VS, *PVST_VS;

// KIOSK 정보
typedef struct _VST_KIOSK
{
	int		nID;								// KIOSK ID(1~60,000)
	int		nServerID;							// 접속할 서버 ID
	TCHAR	csLocationCode[MAX_DB_CODE_SIZE];	// 위치 정보
	TCHAR	csIP[MAX_DB_IP_SIZE];				// KIOSK IP
	TCHAR	csCreaterID[MAX_DB_ID_SIZE];		// 생성자 ID
	int		nCPURate;							// 현재 CPU 사용률(0~100)
	int		nRAMRate;							// 현재 RAM 사용률(0~100)
	_RecordsetPtr  RS;
}VST_KIOSK, *PVST_KIOSK;

// Vision Sensor Event 이력 정보
typedef struct _VST_VS_CAR
{
	int		nID;								// Vision Sensor ID
	int		nServerID;
	TCHAR	csCarNum[MAX_DB_CAR_NUM_SIZE];		// 인식된 차량 번호
	TCHAR	csSeeTime[MAX_DB_DATE_SIZE];		// 차량 인식 시각
	TCHAR	csIMGPath[ MAX_PATH ];				// 차량 인식 이미지 경로	
	TCHAR   csVSLocation[ MAX_DB_CODE_SIZE];
	TCHAR	csFirstInputTime[MAX_DB_DATE_SIZE];
	TCHAR   csVSLocationInfo[ MAX_DB_INFO_SIZE];
	TCHAR   csDBCar_SeeTime[MAX_DB_DATE_SIZE ];
	_RecordsetPtr  RS;
}VST_VS_CAR, *PVST_VS_CAR;


// Vision Sensor Event 이력 정보
typedef struct _VST_VS_EVT_LOG
{
	int		nID;								// Vision Sensor ID
	int		nServerID;
	TCHAR	csCarNum[MAX_DB_CAR_NUM_SIZE];		// 인식된 차량 번호
	TCHAR	csSeeTime[MAX_DB_DATE_SIZE];		// 차량 인식 시각
	TCHAR	csIMGPath[ MAX_PATH ];				// 차량 인식 이미지 경로
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

// KIOSK Event 이력 정보
typedef struct _VST_KIOSK_EVT_LOG
{
	int		nID;								// KIOSK ID; (0~100)
	int		nServerID;
	int		nCPURate;							// CPU사용률(0~100)
	int		nRAMRate;							// RAM사용률(0~100)	
	TCHAR	csCreateTime;						// 생성 시각
	_RecordsetPtr  RS;
}VST_KIOSK_EVT_LOG, *PVST_KIOSK_EVT_LOG;

// 사용자 로그인 이력 정보
typedef struct _VST_LOGIN_LOG
{
	int		nServerID;
	int		nSessionKey;						// 로그인 세션 정보(4byte nand수)
	TCHAR	csUserID[MAX_DB_ID_SIZE];			// 사용자 ID
	TCHAR	csLoginTime[MAX_DB_DATE_SIZE];		// 로그인 시각
	TCHAR	csLogoffTime[MAX_DB_DATE_SIZE];		// 로그오프 시각
	_RecordsetPtr  RS;
}VST_LOGIN_LOG, *PVST_LOGIN_LOG;

// 소켓 연결 이력 정보
typedef struct _VST_NETWORK_LOG
{
	int		nServerID;
	TCHAR	cSYSCode;							// SYSTEM 코드(S:서버, V:VisionSensor, K:KIOSK)
	int		nID;								// 시스템 별 ID(Server == 0)
	TCHAR	cConnectState;								// C:Connect, D:Disconnect
	short	btMessageType;						// 메세지 종류
	TCHAR	csDate[MAX_DB_DATE_SIZE];			// 생성 시각
	_RecordsetPtr  RS;
}VST_NETWORK_LOG, *PVST_NETWORK_LOG;

// 최근 에러 정보
typedef struct _VST_ERR
{	
	int		nServerID;
	TCHAR	cSYSCode;							// SYSTEM 코드(S:서버, V:VisionSensor, K:KIOSK)
	int		nID;								// 시스템 별 ID(Server == 0)
	TCHAR	csErrCode[MAX_DB_ERR_CODE_SIZE];			// 에러 코드
	_RecordsetPtr  RS;
}VST_ERR, *PVST_ERR;

// 에러 이력 정보
typedef struct _VST_ERR_LOG
{
	int		nServerID;
	TCHAR	cSYSCode;							// SYSTEM 코드(S:서버, V:VisionSensor, K:KIOSK)
	int		nID;								// 시스템 별 ID(Server == 0)
	TCHAR	csErrCode[MAX_DB_ERR_CODE_SIZE];	// 에러 코드
	TCHAR	csInfo[ MAX_DB_INFO_SIZE ];
	TCHAR	csCreateTime[MAX_DB_DATE_SIZE];		// 생성 시각
	_RecordsetPtr  RS;
}VST_ERR_LOG, *PVST_ERR_LOG;

/* 날짜 조회, 삭제를 위한 조건 구조체 */
typedef struct _VST_DATE_CONDITION
{
	int		nServerID;
	TCHAR	csBeginTime[MAX_DB_DATE_SIZE];		// 생성 시각
	TCHAR	csEndTime[MAX_DB_DATE_SIZE];		// 생성 시각
	_RecordsetPtr  RS;
}VST_DATE_CONDITION, *PVST_DATE_CONDITION;

/* 차량 조회를 위한 조건 구조체 */
typedef struct _VST_VS_CAR_CONDITION
{	
	TCHAR csCarNum[ MAX_DB_CAR_NUM_SIZE ];	
	TCHAR csBeginTime[MAX_DB_DATE_SIZE];		// 생성 시각
	TCHAR csEndTime[MAX_DB_DATE_SIZE];		// 생성 시각
	int nPageStart;
	int nPageCount;	
	_RecordsetPtr  RS;
}VST_VS_CAR_CONDITION, *PVST_VS_CAR_CONDITION;

/* 차량 조회를 위한 조건 구조체 */
typedef struct _VST_MONITORING
{	
	int		nServerID;
	TCHAR	cSYSCode;							// SYSTEM 코드(S:서버, V:VisionSensor, K:KIOSK)
	TCHAR	csWorkDesc[MAX_DB_INFO_SIZE ];		// 생성 시각
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


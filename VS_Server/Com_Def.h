#pragma once

#define CONVERTER_JPG		

#define STR_UPDATER_PRG		L"VS_Updater.exe"
#define MIN_UPDATE_PORT		22000
#define MAX_UPDATE_PORT		22100

#define CUR_VERSION			0x01
#define NONBLOCK			0x01
#define MAX_DIR_HOUR		4

#define ACK					0x00
#define NAK					0x01
#define NO_CRC				0x00
#define CHK_CRC				0x01


#define BEGIN_IMAGE			0x01
#define CONNTINUE_IMAGE		0x02
#define LAST_IMAGE			0x03
#define TEXT_ONLY			0x10

/* Debug for Object */
#define DEBUG_TCPCLIENT		1
#define DEBUG_MESSAGE		1
#define DEBUG_TCP_CLIENT_MANAGER	1
#define DEBUG_TCPSERVER		1
#define DEBUG_CONTROLLER	1
#define DEBUG_DBMANAGER	1
#define DEBUG_FILEMANAGER	1
#define DEBUG_ADMINVIEWER		1
#define	DEBUG_LOGINVIEWER	1
#define DEBUG_SEARCHVIEWER		1
#define DEBUG_MAINVIEWER	1
#define DEBUG_PACKET		1

#define WM_TRANSFER_MSG		( WM_USER + 900 )
#define MESSAGE_HAVE_CRC	0

#define INIT_DLG_TIMER		1001
#define IDLE_DLG_TIMER		1002
#define CHECK_RESP_MSG_TIMER 1003

#define IDLE_DLG_INTERVAL	1000

#define VS_CAR_TIMEINTERVAL	( 60 )

#define DEFAULT_DB_USER_ID		L"vsserver1"
#define DEFAULT_DB_USER_PWD		L"vsserver2"
#define DEFAULT_DB_DSN			L"vsserver_dsn"
#define TCP_DEFAULT_PORT		20000
#define UDP_DEFAULT_PORT		30000
#define DEFAULT_MONITOR_PRG		L"VS_Monitoring.exe"

#define DEFAULT_WIFI_IP			L"192.168.100.1"
#define DEFAULT_WIFI_SSID		L"Wifi_Laonz"
#define DEFAULT_WIFI_KEY		L"laonz6634"
#define DEFUALT_AWS_HOST		L"a2u8k7v8aknoge-ats.iot.us-east-2.amazonaws.com"
#define DEFAULT_AWS_PORT		8883
#define DEFAULT_AWS_ID			L"MySecondThing"

#define DEFAULT_IMAGE_WIDTH		720
#define DEFAULT_IMAGE_HEIGHT	240
#define DEFAULT_STORAGE_DIR		L"D:\\ImageFile"
#define DEFAULT_NETMASK			L"255.255.255.0"

#define SYS_SEVER			'S'
#define SYS_KIOSK_SEVER		'O'
#define SYS_KIOSK			'K'
#define SYS_VISION			'V'
#define SYS_MONITOR			'M'

#define NET_CONNECTED		'C'
#define NET_DISCONNECTED	'D'
#define LIKE				L"%"
#define NOT_ZONE			L"00"

#define ERR_LAN_CONNECT		L"ET01"
#define ERR_RECV_TIMEOUT	L"ET02"
#define ERR_SAME_IP			L"ET03"
#define ERR_SAME_ID			L"ET04"

#define ERR_CPU_OVER		L"ES01"
#define ERR_HD_OVER			L"ES02"
#define ERR_RAM_OVER		L"ES03"

#define ERR_FILE_OBJECT		L"EF01"
#define ERR_MAKE_DIR		L"EF02"
#define ERR_MAKE_FILE		L"EF03"
#define ERR_DEL_FILE		L"EF04"

#define SVR_RUN_LINE		1
#define SVR_RUN_SIDE		2

#define LOCATION_IN			1
#define LOCATION_LINE		2
#define LOCATION_OUT		3

#define USER_READ_WRITE_LEVEL	20
#define USER_READ_LEVEL			10

#define STR_CLIENTID_SIZE	11	
#define MAX_TCP_CLIENT_CNT	1000
#define MAX_SEND_BUF_SIZE	500

/* 생성된 객체 정보 */
#define MAX_MODEL_CNT		3
#define MAX_VIEWER_CNT		1
#define MAX_TRANSFER_CNT	2

/* TCP Client Packet Size */
#define ONE_IMAGE_PACKET_SIZE	( 1024 )
#define	MAX_RECV_BUF_SIZE	( ONE_IMAGE_PACKET_SIZE + 200 )
#define MAX_IMAGE_BUF_SIZE	( ONE_IMAGE_PACKET_SIZE * 30 )

/* TCP Client에서 TCP Client Manager로 보내는 메세지 */

/* Message의 사이즈 위치 */
#define PACKET_SIZE_POS		2
#define DATA_POS			6

/* Message 에 포함된 Tag의 처리 */
#define MAX_TAG_DATA_SIZE		50
#define MAX_TAG_CNT				20


/* VOIP DATA ========================================================== */
#define MAX_SIPID_SIZE			8
#define MAX_DISPLAY_SIP_SIZE	10
/*===================================================================*/


#define HEARTBIT_MESSAGE_INTERVAL	20
#define MONITERING_HEARTBIT_MESSAGE_INTERVAL	20

#define MAX_RECV_MESSAGE_INTERVAL	( HEARTBIT_MESSAGE_INTERVAL * 3 )

#define MAX_TIME_SIZE				14
#define MAX_IP_SIZE					16
#define MAX_MAC_SIZE				25
#define MAX_CAR_NUM_SIZE			20
#define MAX_LOCATION_SIZE			9
#define MAX_AWS_HOST_SIZE			250
#define MAX_AWS_ID_SIZE				50



/*Message Info Struct를 위한 정의 */
#define TARGET_TCPSERVER	0x01
#define TARGET_DBMANAGER	0x02
#define TARGET_FILEMANAGER	0x03
#define TARGET_MAINVIEWER	0x04
#define TARGET_GUI_DBMANAGER 0x05

#define SEND_VS_RECV_MSG	0x01
#define SEND_VS_SEND_MSG	0x02
#define SEND_QUERY_MSG		0x03
#define SEND_FILE_MSG		0x04
#define SEND_ERR_MSG		0x05
#define SEND_DUMMY_MSG		0x06

#define MODEL_KIND_DB		0x01
#define MODEL_KIND_FILE		0x02
#define MODEL_KIND_GUI_DB	0x03

#define TRANSFER_KIND_TCP	0x01
#define TRANSFER_KIND_UDP	0x02
#define VIEWER_KIND_MAIN	0x01




/* Server 내부용 메세지 */
#define MSG_NETWORK_ALL_BYPASS	0xFF		/* MsgCotroller 등록시에만 사용됨 */

/* Network Tag Type */
#define TAG_RESERVED			0x01
#define TAG_ACK_NAK				0x02
#define TAG_TIME				0x03
#define TAG_SERVER_IP			0x04
#define TAG_SERVER_PORT			0x05
#define TAG_LOCAL_IP			0x06
#define TAG_IMAGE_FULL_SIZE		0x07
#define TAG_IMAGE_TYPE			0x08
#define TAG_IMAGE_LOCATION		0x09
#define TAG_IMAGE				0x0A
#define TAG_VS_ID				0x0B
#define TAG_CAR_NUM				0x0C
#define TAG_CAR_EXTEN_NUM		0x0D
#define TAG_IMAGE_PATH			0x0E
#define TAG_RUN_MODE			0x0F
#define TAG_LOCAL_SUBNET		0x10
#define TAG_LOCAL_GATE			0x11

#define TAG_UPDATER_IP			0x14
#define TAG_UPDATER_PORT		0x15


#define TAG_SYSCODE				0xF0

/*Query에 사용되는 메시지 */
#define QUERY_COMPANY_INSERT		0x01
#define QUERY_COMPANY_DELETE		( QUERY_COMPANY_INSERT + 0x01 )
#define QUERY_COMPANY_UPDATE		( QUERY_COMPANY_INSERT + 0x02 )
#define QUERY_COMPANY_SELECT		( QUERY_COMPANY_INSERT + 0x03 )

#define QUERY_SVR_CONF_INSERT		05
#define QUERY_SVR_CONF_DELETE		( QUERY_SVR_CONF_INSERT + 0x01)
#define QUERY_SVR_CONF_UPDATE		( QUERY_SVR_CONF_INSERT + 0x02)
#define QUERY_SVR_CONF_SELECT		( QUERY_SVR_CONF_INSERT + 0x03)

#define QUERY_LOCATION_INSERT		0x09
#define QUERY_LOCATION_DELETE		( QUERY_LOCATION_INSERT + 0x01)
#define QUERY_LOCATION_UPDATE		( QUERY_LOCATION_INSERT + 0x02)
#define QUERY_LOCATION_SELECT		( QUERY_LOCATION_INSERT + 0x03)

#define QUERY_USER_INSERT			0x0D
#define QUERY_USER_DELETE			( QUERY_USER_INSERT + 0x01)
#define QUERY_USER_UPDATE			( QUERY_USER_INSERT + 0x02)
#define QUERY_USER_SELECT			( QUERY_USER_INSERT + 0x03)

#define QUERY_ERR_CODE_INSERT		0x11
#define QUERY_ERR_CODE_DELETE		( QUERY_ERR_CODE_INSERT + 0x01)
#define QUERY_ERR_CODE_UPDATE		( QUERY_ERR_CODE_INSERT + 0x02)
#define QUERY_ERR_CODE_SELECT		( QUERY_ERR_CODE_INSERT + 0x03)

#define QUERY_SYS_CODE_INSERT		0x15
#define QUERY_SYS_CODE_DELETE			( QUERY_SYS_CODE_INSERT + 0x01)
#define QUERY_SYS_CODE_UPDATE			( QUERY_SYS_CODE_INSERT + 0x02)
#define QUERY_SYS_CODE_SELECT			( QUERY_SYS_CODE_INSERT + 0x03)

#define QUERY_VS_INSERT				0x19
#define QUERY_VS_DELETE				( QUERY_VS_INSERT + 0x01)
#define QUERY_VS_UPDATE				( QUERY_VS_INSERT + 0x02)
#define QUERY_VS_SELECT				( QUERY_VS_INSERT + 0x03)

#define QUERY_KIOSK_INSERT			0x1D
#define QUERY_KIOSK_DELETE			( QUERY_KIOSK_INSERT + 0x01)
#define QUERY_KIOSK_UPDATE			( QUERY_KIOSK_INSERT + 0x02)
#define QUERY_KIOSK_SELECT			( QUERY_KIOSK_INSERT + 0x03)

#define QUERY_VS_EVT_LOG_INSERT		0x21
#define QUERY_VS_EVT_LOG_DELETE			( QUERY_VS_EVT_LOG_INSERT + 0x01)
#define QUERY_VS_EVT_LOG_UPDATE			( QUERY_VS_EVT_LOG_INSERT + 0x02)
#define QUERY_VS_EVT_LOG_SELECT			( QUERY_VS_EVT_LOG_INSERT + 0x03)

#define QUERY_KIOSK_EVT_LOG_INSERT	0x25
#define QUERY_KIOSK_EVT_LOG_DELETE		( QUERY_KIOSK_EVT_LOG_INSERT + 0x01)
#define QUERY_KIOSK_EVT_LOG_UPDATE		( QUERY_KIOSK_EVT_LOG_INSERT + 0x02)
#define QUERY_KIOSK_EVT_LOG_SELECT		( QUERY_KIOSK_EVT_LOG_INSERT + 0x03)

#define QUERY_LOGIN_LOG_INSERT		0x29
#define QUERY_LOGIN_LOG_DELETE		( QUERY_LOGIN_LOG_INSERT + 0x01)
#define QUERY_LOGIN_LOG_UPDATE		( QUERY_LOGIN_LOG_INSERT + 0x02)
#define QUERY_LOGIN_LOG_SELECT		( QUERY_LOGIN_LOG_INSERT + 0x03)

#define QUERY_NETWORK_LOG_INSERT	0x2D
#define QUERY_NETWORK_LOG_DELETE	( QUERY_NETWORK_LOG_INSERT + 0x01)
#define QUERY_NETWORK_LOG_UPDATE	( QUERY_NETWORK_LOG_INSERT + 0x02)
#define QUERY_NETWORK_LOG_SELECT	( QUERY_NETWORK_LOG_INSERT + 0x03)

#define QUERY_ERR_INSERT			0x31
#define QUERY_ERR_DELETE			( QUERY_ERR_INSERT + 0x01)
#define QUERY_ERR_UPDATE			( QUERY_ERR_INSERT + 0x02)
#define QUERY_ERR_SELECT			( QUERY_ERR_INSERT + 0x03)

#define QUERY_ERR_LOG_INSERT		0x35
#define QUERY_ERR_LOG_DELETE		( QUERY_ERR_LOG_INSERT + 0x01)
#define QUERY_ERR_LOG_UPDATE		( QUERY_ERR_LOG_INSERT + 0x02)
#define QUERY_ERR_LOG_SELECT		( QUERY_ERR_LOG_INSERT + 0x03)

#define QUERY_VS_CAR_INSERT			0x39
#define QUERY_VS_CAR_DELETE		( QUERY_VS_CAR_INSERT + 0x01)
#define QUERY_VS_CAR_UPDATE		( QUERY_VS_CAR_INSERT + 0x02)
#define QUERY_VS_CAR_SELECT		( QUERY_VS_CAR_INSERT + 0x03)

#define QUERY_MONITORING_INSERT		0x3D
#define QUERY_MONITORING_DELETE		( QUERY_MONITORING_INSERT + 0x01)
#define QUERY_MONITORING_UPDATE		( QUERY_MONITORING_INSERT + 0x02)
#define QUERY_MONITORING_SELECT		( QUERY_MONITORING_INSERT + 0x03)


/*All Delete Msg Define */
#define QUERY_NETWORK_LOG_DATE_SELECT	0x80
#define QUERY_LOGIN_LOG_DATE_SELECT		( QUERY_NETWORK_LOG_DATE_SELECT + 0x01)
#define QUERY_ERR_LOG_DATE_SELECT		( QUERY_NETWORK_LOG_DATE_SELECT + 0x02)
#define QUERY_VS_EVT_LOG_DATE_SELECT	( QUERY_NETWORK_LOG_DATE_SELECT + 0x03)
#define QUERY_KIOSK_EVT_LOG_DATE_SELECT	( QUERY_NETWORK_LOG_DATE_SELECT + 0x04)

#define QUERY_VS_EVT_LOG_ALL_DEL	0x90
#define QUERY_KIOSK_EVT_LOG_ALL_DEL	0x91
#define QUERY_LOGIN_LOG_ALL_DEL		0x92
#define QUERY_NETWORK_LOG_ALL_DEL	0x93
#define QUERY_ERR_LOG_ALL_DEL		0x94

#define	QUERY_VS_SELECT_WITH_LOCATION			0x95
#define QUERY_VS_EVT_LOG_SELECT_WITH_LOCATION	0x96
#define QUERY_LOCATION_UPDATE_INFO				0x97

/* page, % 조회 */
#define QUERY_VS_EVT_LOG_COUNT_SELECT			0x98
#define QUERY_VS_EVT_LOG_MULTI_SELECT			0x99

#define QUERY_VS_CAR_COUNT_SELECT				0x9A
#define QUERY_VS_CAR_MULTI_SELECT				0x9B


#define QUERY_MSG_ALL_BYPASS		0xFF	/* MsgCotroller 등록시에만 사용됨 */

/*File에서 사용되는 메시지 */

#define FILE_MSG_ALL_BYPASS			0xFF	/* MsgCotroller 등록시에만 사용됨 */

/*에러에 사용되는 메시지 */
#define ERR_MSG_RECV_TIMEOUT		0x01

#define ERR_MSG_ALL_BYPASS			0xFF	/* MsgCotroller 등록시에만 사용됨 */

/* Dummy 사용되는 메시지 (0xE0 ~ 0xFF)*/
#define DUMMY_MSG_CONNECT_TCP		0x01
#define DUMMY_MSG_CLOSE_TCP			0x02
#define DUMMY_MSG_MAKE_DIR			0x03
#define DUMMY_MSG_SAVE_FILE			0x04
#define DUMMY_MSG_CLEAR_TIMEOUT		0x05
#define DUMMY_MSG_RECONNECT_DB		0x06

#define DUMMY_MSG_ALL_BYPASS		0xFF	/* MsgCotroller 등록시에만 사용됨 */



/* VOIP DATA ========================================================== */
#define MSG_VS_NOTIFY_STATUS	0x3001
#define MSG_VS_NOTIFY_ERR_STS	0x3003

#define MSG_VS_NOTIFY_LED_STS	0x3004
#define MSG_VS_NOTIFY_GPIO_STS	0x3005

#define MSG_VS_REQ_STATUS		0x0001
#define MSG_VS_REQ_BASIC		0x0002
#define MSG_VS_REQ_HB			0x0003
#define MSG_VS_REQ_RESET		0x0004
#define MSG_VS_REQ_CALL_END		0x0005
#define MSG_VS_REQ_WATCHDOG		0x0006
#define MSG_VS_REQ_VOLUME		0x0007

#define MSG_VS_REQ_LED_SET		0x0008
#define MSG_VS_REQ_LED_GET		0x0009

#define MSG_VS_REQ_EMG_CALL		0x000A

#define MSG_VS_REQ_GPIO_SET		0x000B
#define MSG_VS_REQ_GPIO_GET		0x000C

#define MSG_VS_REQ_CALL_READY	0x4001

#define MSG_VS_RESP_STATUS		( MSG_VS_REQ_STATUS		+ 0x1000 )
#define MSG_VS_RESP_BASIC		( MSG_VS_REQ_BASIC		+ 0x1000 )
#define MSG_VS_RESP_HB			( MSG_VS_REQ_HB			+ 0x1000 )
#define MSG_VS_RESP_RESET		( MSG_VS_REQ_RESET		+ 0x1000 )
#define MSG_VS_RESP_CALL_END	( MSG_VS_REQ_CALL_END	+ 0x1000 )
#define MSG_VS_RESP_WATCHDOG	( MSG_VS_REQ_WATCHDOG	+ 0x1000 )
#define MSG_VS_RESP_VOLUME		( MSG_VS_REQ_VOLUME		+ 0x1000 )

#define MSG_VS_RESP_LED_SET		( MSG_VS_REQ_LED_SET	+ 0x1000 )
#define MSG_VS_RESP_LED_GET		( MSG_VS_REQ_LED_GET	+ 0x1000 ) 

#define MSG_VS_RESP_EMG_CALL	( MSG_VS_REQ_EMG_CALL   + 0x1000 )
#define MSG_VS_RESP_CALL_READY	( MSG_VS_REQ_CALL_READY + 0x1000 )

#define MSG_VS_RESP_GPIO_SET	( MSG_VS_REQ_GPIO_SET	+ 0x1000 )
#define MSG_VS_RESP_GPIO_GET	( MSG_VS_REQ_GPIO_GET	+ 0x1000 ) 


#define SOCK_STX					0x4A57


#define AWS_PUB_POWER_ON        1
#define AWS_PUB_POWER_OFF       2
#define AWS_PUB_MOVE_UP         3
#define AWS_PUB_MOVE_DOWN       4
#define AWS_PUB_FEED            5
#define AWS_PUB_PLAY            6
#define AWS_PUB_PICTURE         7
#define AWS_PUB_WIFI            8
#define AWS_PUB_UPDATE          0x0100
#define AWS_PUB_KIT_UPDATE          0x0101


#define SOCK_NET_INFO_REQ_MSG       0x0001                                                    
#define SOCK_NET_INFO_RESP_MSG      0x1001                                                    
#define SOCK_SEND_IMAG_MSG          0x0002

#define SOCK_AWS_INFO_REQ_MSG		0x0005
                                                                                              
#define SOCK_TEST_REC_REQ_MSG       0x9001                                                    
#define SOCK_TEST_PLAY_REQ_MSG      0x9002                                                    
#define SOCK_TEST_LED_REQ_MSG       0x9003                                                    
#define SOCK_TEST_MOTOR_REQ_MSG     0x9004                                                    
#define SOCK_TEST_RUN_REQ_MSG       0x9005                                                    
#define SOCK_TEST_CAM_REQ_MSG       0x9006                                                    
#define SOCK_TEST_WIFI_REQ_MSG      0x9007
#define SOCK_TEST_WIFI_REQ_MSG      0x9007
#define SOCK_TEST_LIGHT_PWM_REQ_MSG 0x9008
#define SOCK_TEST_AWS_PUB_REQ_MSG   0x9009
#define SOCK_TEST_PLATE_SET_MSG		0x900A
#define SOCK_TEST_PLATE_GET_MSG		0x900B
#define SOCK_TEST_MNG_CON_MSG		0x900C
#define SOCK_TEST_LOG_MSG			0x900D

#define SOCK_TEST_STRIP_RESULT_MSG	0x9901

#define SOCK_ADD_MSG				0x9999

#define MAX_LED_CNT					6
#define MAX_MOTOR_CNT				4
#define MAX_SSID_SIZE				50
#define MAX_KEY_SIZE				50

#define WIFI_STA					1
#define WIFI_AP						2

/* ==================================================================== */

#pragma pack(push, 1)
#if 0
typedef struct Head_
{
	BYTE Version;
	short Msg;
	unsigned short ClientID;
	unsigned short ServerID;
	unsigned short Size;
}Head_t;
#else
typedef struct Head_
{	
	unsigned short STX;
	unsigned short LEN;	
	unsigned short CMD;		
}Head_t;
#endif

typedef struct TransferConf_
{	
	char szServerIP[ MAX_IP_SIZE ];
	char szNetmask[ MAX_IP_SIZE ];
	char szUDPServerIP[ MAX_IP_SIZE ];
	unsigned short ServerPort;
	unsigned short UDPSendPort;
	unsigned short UDPReadPort;
}TransferConf_t;

typedef struct ModelConf_
{
	TCHAR strUserID[ MAX_PATH ];
	TCHAR strUserPwd[ MAX_PATH ];
	TCHAR strDSN[ MAX_PATH];
}ModelConf_t;

typedef struct ServerConf_
{	
	int MyID;
	char szServerIP[ MAX_IP_SIZE ];
	UINT16 ServerPort;
}ServerConf_t;

typedef struct Heartbit_
{	
	BYTE Tmp;
}Heartbit_t;

typedef struct MonitoringHeartbit_
{	
	char SysCode;
}MonitoringHeartbit_t;

typedef struct VSImage_
{	
	int ClientID;
	DWORD ImageFullSize;
	BYTE ImageType;
	BYTE ImageLocation;
	int NowImageSize;
	char CarNum[ MAX_CAR_NUM_SIZE + 2];
	char ExtCarNum[ MAX_CAR_NUM_SIZE + 2];
	char ImagePath[ MAX_PATH * 2];
	char ImageTime[ MAX_TIME_SIZE + 2 ];
}VSImage_t;

typedef struct VSConf_
{	
	UINT16 ClientID;
	BYTE RunMode;	
	char szIP[ MAX_IP_SIZE + 2];
	char szSubnet[ MAX_IP_SIZE + 2];
	char szGate[ MAX_IP_SIZE + 2];
}VSConf_t;

typedef struct Ack_Nak_
{
	BYTE Val;
}Ack_Nak_t;

typedef struct Tag_
{
	BYTE Tag;
	unsigned short Size;
	BYTE Data[ MAX_TAG_DATA_SIZE ];	
}Tag_t;

typedef struct ClientSendMsg_
{	
	short Msg;
	int SIPID;							/* Target Vision Sensor ID */
	int Size;							/* send Size */
	BYTE SendBuf[ MAX_SEND_BUF_SIZE ];	/* Send Buffer Pointer */
}ClientSendMsg_t;

typedef struct ClientRecvMsg_
{	
	short Msg;
	short VAR1;
	int SIPID;	
	BOOL IsFirstMsg;
	TCHAR ClientIP[ MAX_IP_SIZE + 2];
	TCHAR ClientMAC[ MAX_MAC_SIZE ];
	BYTE * DummyBuf;	
}ClientRecvMsg_t;

typedef struct DummyMsg_
{
	UINT_PTR Postion;			/* TCP Client 만 사용 */
	int ClientID;				/* TCP Client 만 사용 */
	TCHAR SysCode;
	short Msg;	
	int DataSize;
	void * ptrData;
	void * ptrEtcInfo;
	void * ptrEtcInfo2;
	void * ptrEtcInfo3;	
}DummyMsg_t;

typedef struct ErrMsg_
{
	int ClientID;
	TCHAR SysCode;
	short Msg;
	int DataSize;
	void * ptrData;
}ErrMsg_t;

typedef struct QueryMsg_
{
	short Msg;
	int DataSize;
	void * ptrData;
}QueryMsg_t;

typedef struct FileMsg_
{
	short Msg;
}FileMsg_t;

typedef struct MsgInfo_
{					
	BYTE NonBlock;
	short SendMsgType;
	void * ptrMsg;
}MsgInfo_t;

typedef struct Queue_
{
	int Head;
	int Tail;
	BYTE Buf[ ONE_IMAGE_PACKET_SIZE  * 3 ];
}Queue_t;

typedef struct ImageQueue_
{	
	DWORD Tail;
	BYTE ImageBuf[ MAX_IMAGE_BUF_SIZE ];
}ImageQueue_t;

typedef struct ClientDir_
{
	int ID;
	int Cur_Day;
	int Cur_Hour;	
}ClientDir_t;

typedef struct LastCarInfo_
{
	int Last_CarNum;
	time_t Last_SeeTime;
}LastCarInfo_t;

typedef struct Updaterinfo_
{
	char szIP[ MAX_IP_SIZE ];
	UINT16 nPort;
}UpdaterInfo_t;


typedef struct Statusinfo_
{
	__time32_t chk_time;
	BYTE Speak_sts;
	BYTE MIC_sts;
	BYTE Button_sts;
	BYTE ETC_sts;
	BYTE LED_sts;
	BYTE Reserved[ 2 ];
}Statusinfo_t;

typedef struct Errorinfo_
{
	BYTE Code;
}Errorinfo_t;

typedef struct Basicinfo_
{
	char display_name[ MAX_DISPLAY_SIP_SIZE ];	
	char version[ MAX_DISPLAY_SIP_SIZE ];	
	BYTE volume;
}Basicinfo_t;

typedef struct Heartbeatinfo_
{
	__time32_t chk_time;	
}Heartbeatinfo_t;

typedef struct Volume_
{
	BYTE vol;
}Volume_t;

typedef struct LED_
{
	BYTE onoff;		/* off : 0 , on : 1 */
}LED_t;

typedef struct GPIO_
{
	BYTE type;		/* 0x01 : 음성인식 , 0x02 ~ 0x05 : Reserved */
	BYTE onoff;		/* off : 0 , on : 1 */
}GPIO_t;

typedef struct led_data_
{
    unsigned char kind;
    unsigned char color;
    unsigned char onoff;
}led_data_t;

typedef struct motor_data_
{
    unsigned char kind;
    unsigned char onoff;
	unsigned char pwm;
	unsigned char time;
}motor_data_t;


typedef struct led_info_
{
    unsigned char cnt;
    led_data_t list[ MAX_LED_CNT ];
}led_info_t;

typedef struct motor_info_
{
    unsigned char cnt;
    motor_data_t list[ MAX_MOTOR_CNT ];
}motor_info_t;

typedef struct net_info_
{
    char ssid [ MAX_SSID_SIZE ];                    /* RTU IP */
    char key[ MAX_KEY_SIZE ];               /* RTU NETMASK */
    char server_ip[ MAX_IP_SIZE ];              /* RTU gateway */
    unsigned short server_port;
	char aws_host[ MAX_AWS_HOST_SIZE ];
	unsigned short aws_port;
	char aws_id[ MAX_AWS_ID_SIZE];
}net_info_t;


#define ONOFF_ITEM_CNT 6
#define PLATE_ON_CNT 5
#define PLATE_OFF_CNT 3


typedef struct onoff_item_
{
    unsigned char item[ ONOFF_ITEM_CNT ];
}onoff_item_t;
/* plate_onoff data */
typedef struct plate_onoff_
{
    onoff_item_t on[ PLATE_ON_CNT];
    onoff_item_t off[ PLATE_OFF_CNT];
}plate_onoff_t;


#pragma pack(pop)
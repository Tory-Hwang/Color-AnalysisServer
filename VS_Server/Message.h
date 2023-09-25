#pragma once

#include "Com_Def.h"
#include "Util.h"

class CMessage
{
public:
	CMessage(void);
	~CMessage(void);
	
	/* 필요에 의해 Server단에서 Client 용 메시지를 만들 경우  */	
	BOOL MakeDummyVSImageMsg( short Msg, int ClientID, VSImage_t * VS_Image, ClientRecvMsg_t * ClientRecvMsg);
	BOOL MakeReqStatus( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg);
	BOOL MakeReqBasicinfo( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg);
	BOOL MakeReqHeartBeat( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg);
	BOOL MakeReqReset( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg);
	BOOL MakeReqWatchdog( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg);
	BOOL MakeReqVolume( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, BYTE vol );

	BOOL MakeReqLED_SET( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, BYTE onoff );
	BOOL MakeReqLED_GET( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg );
	
	BOOL MakeReqGPIO_SET( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, BYTE nType, BYTE onoff );
	BOOL MakeReqGPIO_GET( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, BYTE Type );
	
	BOOL MakeRespEMG_CALL( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg );
	BOOL MakeReqCALL_Ready( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg );
	

	BOOL MakeReqTEST_MODE( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, short mode );
	BOOL MakeReqTEST_RECODER( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg );
	BOOL MakeReqTEST_PLAY( short Msg, int ClientID, short kind, ClientSendMsg_t * ClientSendMsg );
	BOOL MakeReqTEST_PUB( short Msg, int ClientID, BYTE num, short kind, ClientSendMsg_t * ClientSendMsg);
	BOOL MakeReqTEST_CAPTURE( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg );
	BOOL MakeReqTEST_LED( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg , led_info_t * ptrled );
	BOOL MakeReqTEST_MOTOR( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg , motor_info_t * ptrmotor );
	BOOL MakeReqTEST_WIFI( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg , net_info_t * ptrnets );
	BOOL MakeReqTEST_AP( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg , net_info_t * ptrnets );
	BOOL MakeReqTEST_MNG_CON( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg , net_info_t * ptrnets );
	BOOL MakeReqTEST_AWS_AP( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg , net_info_t * ptrnets );
	BOOL MakeReqTEST_LIGHT_PWM( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg , BYTE pwm );
	BOOL MakeReqTEST_PLATE_SET( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg , BYTE * ptr, int cnt );
	BOOL MakeTEST_PLATE_GET( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg );
	

	BOOL MakeTEST_STRIP_RESULT( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg , int size, BYTE *ptrResult );


	/*Parse Message Function */
	BOOL ParseStatusMsg( ClientRecvMsg_t * ClientRecvMsg, Statusinfo_t * Status);
	BOOL ParseBasicInfoMsg( ClientRecvMsg_t * ClientRecvMsg, Basicinfo_t * Basic);
	BOOL ParseHeartbeatMsg( ClientRecvMsg_t * ClientRecvMsg, Heartbeatinfo_t * Heartbeat);
	BOOL ParseResetMsg( ClientRecvMsg_t * ClientRecvMsg );
	BOOL ParseWatchdogMsg( ClientRecvMsg_t * ClientRecvMsg );

	BOOL ParseLED_SETMsg( ClientRecvMsg_t * ClientRecvMsg, LED_t * LED);
	BOOL ParseLED_GETMsg( ClientRecvMsg_t * ClientRecvMsg, LED_t * LED);
	
	BOOL ParseGPIO_SETMsg( ClientRecvMsg_t * ClientRecvMsg, GPIO_t * GPIO );
	BOOL ParseGPIO_GETMsg( ClientRecvMsg_t * ClientRecvMsg, GPIO_t * GPIO );
	
	BOOL ParseVolumeInfoMsg( ClientRecvMsg_t * ClientRecvMsg, Volume_t * Volume);
	BOOL ParseErrorNotiMsg( ClientRecvMsg_t * ClientRecvMsg, Errorinfo_t * Err );
	
	BOOL ParseEMG_CALL( ClientRecvMsg_t * ClientRecvMsg  );
	BOOL ParseCALL_Ready( ClientRecvMsg_t * ClientRecvMsg );
	
private:
	CUtil m_Util;
	BOOL MakeHead( short Msg, int PayloadSize, int SIPID, Head_t * Head );		


};
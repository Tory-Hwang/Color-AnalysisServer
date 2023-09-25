#include "StdAfx.h"
#include "Message.h"

CMessage::CMessage(void)
{
}

CMessage::~CMessage(void)
{
}

/* =====================================================================
Internal Functions 
=======================================================================*/

BOOL CMessage::MakeHead( short Msg, int PayloadSize, int SIPID, Head_t * Head  )
{	
	( void )SIPID;

	if ( Head != NULL )
	{
		memset( Head, 0, sizeof( Head_t ));	
		Head->STX = SOCK_STX;
		Head->CMD = Msg;
		Head->LEN = (unsigned short)PayloadSize;
	}
	else
		return FALSE;

	return TRUE;
}

/* =====================================================================
Export Functions 
=======================================================================*/

/*======================================================================
Make Functions
========================================================================*/

BOOL CMessage::MakeReqStatus( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg)
{
	Head_t Head;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	if ( MakeHead( Msg, 0, ClientID, &Head )  )
	{
		
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));
		ClientSendMsg->Msg = Msg;
		ClientSendMsg->SIPID = ClientID;
		ClientSendMsg->Size = sizeof( Head );
		return TRUE;
	}
	return FALSE;
	
}

BOOL CMessage::MakeReqBasicinfo( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg)
{
	Head_t Head;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	if ( MakeHead( Msg, 0, ClientID, &Head )  )
	{
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));
		ClientSendMsg->Msg = Msg;
		ClientSendMsg->SIPID = ClientID;
		ClientSendMsg->Size = sizeof( Head );
		return TRUE;
	}
	return FALSE;
}



BOOL CMessage::MakeReqHeartBeat( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg)
{
	Head_t Head;
	__time32_t cur_time=0;
	UINT32 u32 =0;
	struct tm t_time;
	CTime t = CTime::GetCurrentTime();

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	if ( MakeHead( Msg, 4, ClientID, &Head )  )
	{
#if 0
		cur_time = time(NULL);
		u32 = htonl( cur_time );
#else
	

	t_time.tm_year = ( int )( t.GetYear() - 1900 );
	t_time.tm_mon  = ( int )( t.GetMonth() - 1 );
	t_time.tm_mday = ( int )( t.GetDay() );
	t_time.tm_hour = ( int )( t.GetHour()  );
	t_time.tm_min  = ( int )( t.GetMinute() );
	t_time.tm_sec  = ( int )( t.GetSecond() );

	cur_time = mktime( &t_time );
	u32 = htonl( cur_time );

#endif
		
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));
		memcpy( &ClientSendMsg->SendBuf[ sizeof( Head ) ], &u32, sizeof(int));

		ClientSendMsg->Msg = Msg;
		ClientSendMsg->SIPID = ClientID;
		ClientSendMsg->Size = sizeof( Head ) + 4 ;
		return TRUE;
	}
	
	return FALSE;
}

BOOL CMessage::MakeReqReset( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg)
{
	Head_t Head;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	if ( MakeHead( Msg, 0, ClientID, &Head )  )
	{
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));
		ClientSendMsg->Msg = Msg;
		ClientSendMsg->SIPID = ClientID;
		ClientSendMsg->Size = sizeof( Head );
		return TRUE;
	}
	
	return FALSE;
}


BOOL CMessage::MakeReqWatchdog( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg)
{
	Head_t Head;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	if ( MakeHead( Msg, 0, ClientID, &Head )  )
	{
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));
		ClientSendMsg->Msg = Msg;
		ClientSendMsg->SIPID = ClientID;
		ClientSendMsg->Size = sizeof( Head );
		return TRUE;
	}
	
	return FALSE;
}



BOOL CMessage::MakeReqVolume( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, BYTE vol )
{
	Head_t Head;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	if ( MakeHead( Msg, 1, ClientID, &Head )  )
	{
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));
		ClientSendMsg->Msg = Msg;
		ClientSendMsg->SIPID = ClientID;
		ClientSendMsg->Size = sizeof( Head ) + sizeof( BYTE );
		ClientSendMsg->SendBuf[ sizeof( Head )  ] = vol;

		return TRUE;
	}
	
	return FALSE;
}

/*GPIO 제어 */
BOOL CMessage::MakeReqGPIO_SET( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, BYTE nType, BYTE onoff )
{
	Head_t Head;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	/* Payload size 반드시 변경 할것  */
	if ( MakeHead( Msg, 2, ClientID, &Head )  )
	{
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));
		ClientSendMsg->Msg = Msg;
		ClientSendMsg->SIPID = ClientID;
		ClientSendMsg->Size = sizeof( Head ) + ( sizeof( BYTE ) * 2 );
		ClientSendMsg->SendBuf[ sizeof( Head )  ] = nType;
		ClientSendMsg->SendBuf[ sizeof( Head )+1  ] = onoff;

		return TRUE;
	}

	return TRUE;
}

BOOL CMessage::MakeReqGPIO_GET( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, BYTE Type )
{
	Head_t Head;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	if ( MakeHead( Msg, 1, ClientID, &Head )  )
	{
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));
		ClientSendMsg->Msg = Msg;
		ClientSendMsg->SIPID = ClientID;
		ClientSendMsg->Size = sizeof( Head ) + 1;
		ClientSendMsg->SendBuf[ sizeof( Head )  ] = Type;

		return TRUE;
	}
	
	return FALSE;
}

/* LED 제어 */
BOOL CMessage::MakeReqLED_SET( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, BYTE onoff )
{
	Head_t Head;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	/* Payload size 반드시 변경 할것  */
	if ( MakeHead( Msg, 1, ClientID, &Head )  )
	{
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));
		ClientSendMsg->Msg = Msg;
		ClientSendMsg->SIPID = ClientID;
		ClientSendMsg->Size = sizeof( Head ) + sizeof( BYTE );
		ClientSendMsg->SendBuf[ sizeof( Head )  ] = onoff;

		return TRUE;
	}

	return TRUE;
}

BOOL CMessage::MakeReqLED_GET( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg )
{
	Head_t Head;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	if ( MakeHead( Msg, 0, ClientID, &Head )  )
	{
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));
		ClientSendMsg->Msg = Msg;
		ClientSendMsg->SIPID = ClientID;
		ClientSendMsg->Size = sizeof( Head );
		return TRUE;
	}
	
	return FALSE;
}

BOOL CMessage::MakeRespEMG_CALL( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg )
{
	Head_t Head;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	if ( MakeHead( Msg, 0, ClientID, &Head )  )
	{
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));
		ClientSendMsg->Msg = Msg;
		ClientSendMsg->SIPID = ClientID;
		ClientSendMsg->Size = sizeof( Head );
		return TRUE;
	}	
	
	return FALSE;
}

BOOL CMessage::MakeReqCALL_Ready( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg )
{
	Head_t Head;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	if ( MakeHead( Msg, 0, ClientID, &Head )  )
	{
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));
		ClientSendMsg->Msg = Msg;
		ClientSendMsg->SIPID = ClientID;
		ClientSendMsg->Size = sizeof( Head );
		return TRUE;
	}	
	
	return FALSE;
}

BOOL CMessage::MakeReqTEST_MODE( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, short mode )
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	
	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	len = sizeof( short );
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	

		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));
		memcpy( &ClientSendMsg->SendBuf[Head_Size], &mode, len);
		return TRUE;
	}
	
	return FALSE;
}

BOOL CMessage::MakeReqTEST_RECODER( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg)
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	
	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	len = 0;
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	

		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));		
		return TRUE;
	}
	
	return FALSE;
}


BOOL CMessage::MakeReqTEST_PLAY( short Msg, int ClientID, short kind, ClientSendMsg_t * ClientSendMsg)
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	
	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	len = 2;
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	

		memcpy( ClientSendMsg->SendBuf, &Head, Head_Size );		
		memcpy( &ClientSendMsg->SendBuf[ Head_Size ],  &kind, len );

		return TRUE;
	}
	
	return FALSE;
}


BOOL CMessage::MakeReqTEST_PUB( short Msg, int ClientID, BYTE num, short kind, ClientSendMsg_t * ClientSendMsg)
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	
	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	len = 3;
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	

		memcpy( ClientSendMsg->SendBuf, &Head, Head_Size );	
		ClientSendMsg->SendBuf[ Head_Size ]=  num;
		memcpy( &ClientSendMsg->SendBuf[ Head_Size + 1 ],  &kind, len );

		return TRUE;
	}
	
	return FALSE;
}

BOOL CMessage::MakeReqTEST_CAPTURE( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg)
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	
	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	len = 0;
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	

		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));		
		return TRUE;
	}
	
	return FALSE;
}


BOOL CMessage::MakeReqTEST_LED( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, led_info_t * ptrled)
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	
	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	len = sizeof( led_info_t );
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));	
		memcpy( &ClientSendMsg->SendBuf[ Head_Size ] , ptrled, len);	

		return TRUE;
	}
	
	return FALSE;
}


BOOL CMessage::MakeReqTEST_MOTOR( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, motor_info_t * ptrmotor)
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	
	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	len = sizeof( motor_info_t );
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));	
		memcpy( &ClientSendMsg->SendBuf[ Head_Size ] , ptrmotor, len);	

		return TRUE;
	}
	
	return FALSE;
}


BOOL CMessage::MakeReqTEST_WIFI( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, net_info_t * ptrnet)
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	char * ptr = NULL;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	len = MAX_SSID_SIZE + MAX_KEY_SIZE + MAX_IP_SIZE + sizeof( short );
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));	
		ptr = (char*)&ClientSendMsg->SendBuf[ Head_Size ];

		memcpy( ptr, ptrnet->ssid,  MAX_SSID_SIZE );
		ptr += MAX_SSID_SIZE;

		memcpy( ptr, ptrnet->key,  MAX_KEY_SIZE );
		ptr += MAX_KEY_SIZE;

		memcpy( ptr, ptrnet->server_ip,  MAX_IP_SIZE );
		ptr += MAX_IP_SIZE;

		memcpy( ptr, &ptrnet->server_port,  sizeof( short ) );
		ptr += sizeof( short );	

		return TRUE;
	}
	
	return FALSE;
}

BOOL CMessage::MakeReqTEST_AP( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, net_info_t * ptrnet)
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	char * ptr = NULL;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	len = MAX_SSID_SIZE + MAX_KEY_SIZE + MAX_IP_SIZE + sizeof( short );
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));	
		ptr = (char*)&ClientSendMsg->SendBuf[ Head_Size ];

		memcpy( ptr, ptrnet->ssid,  MAX_SSID_SIZE );
		ptr += MAX_SSID_SIZE;

		memcpy( ptr, ptrnet->key,  MAX_KEY_SIZE );
		ptr += MAX_KEY_SIZE;

		memcpy( ptr, ptrnet->server_ip,  MAX_IP_SIZE );
		ptr += MAX_IP_SIZE;

		memcpy( ptr, &ptrnet->server_port,  sizeof( short ) );
		ptr += sizeof( short );


		return TRUE;
	}
	
	return FALSE;
}


BOOL CMessage::MakeReqTEST_MNG_CON( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, net_info_t * ptrnet)
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	char * ptr = NULL;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	len = MAX_IP_SIZE + sizeof( short );
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));	
		ptr = (char*)&ClientSendMsg->SendBuf[ Head_Size ];

		memcpy( ptr, ptrnet->server_ip,  MAX_IP_SIZE );
		ptr += MAX_IP_SIZE;

		memcpy( ptr, &ptrnet->server_port,  sizeof( short ) );
		ptr += sizeof( short );


		return TRUE;
	}
	
	return FALSE;
}


BOOL CMessage::MakeReqTEST_AWS_AP( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg, net_info_t * ptrnet)
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	char * ptr = NULL;

	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	len = MAX_SSID_SIZE + MAX_KEY_SIZE + MAX_AWS_HOST_SIZE	+ sizeof( short ) ;
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));	
		ptr = (char*)&ClientSendMsg->SendBuf[ Head_Size ];

		memcpy( ptr, ptrnet->ssid,  MAX_SSID_SIZE );
		ptr += MAX_SSID_SIZE;

		memcpy( ptr, ptrnet->key,  MAX_KEY_SIZE );
		ptr += MAX_KEY_SIZE;

		memcpy( ptr, ptrnet->aws_host,  MAX_AWS_HOST_SIZE );
		ptr += MAX_AWS_HOST_SIZE;

		memcpy( ptr, &ptrnet->aws_port,  sizeof( short ) );
		ptr += sizeof( short );

		return TRUE;
	}
	
	return FALSE;
}
BOOL CMessage::MakeReqTEST_LIGHT_PWM( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg , BYTE pwm )
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	
	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	len = sizeof( char );
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));	
		ClientSendMsg->SendBuf[ Head_Size ]= pwm;	
		return TRUE;
	}
	
	return FALSE;
}

BOOL CMessage::MakeReqTEST_PLATE_SET( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg , BYTE * ptr, int cnt )
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	
	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	len = cnt;
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));
		memcpy( &ClientSendMsg->SendBuf[ Head_Size ], ptr, cnt );	
		return TRUE;
	}
	return FALSE;
}


BOOL CMessage::MakeTEST_PLATE_GET( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg )
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	
	if ( ClientSendMsg == NULL )
	{
		return FALSE;
	}
	
	len = 0;
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));			
		return TRUE;
	}
	return FALSE;
}

BOOL CMessage::MakeTEST_STRIP_RESULT( short Msg, int ClientID, ClientSendMsg_t * ClientSendMsg ,int size, BYTE *ptrResult  )
{
	Head_t Head;
	int len =0;
	int Head_Size = 0;
	
	if ( ClientSendMsg == NULL || ptrResult == NULL )
	{
		return FALSE;
	}
	
	len = size;
	Head_Size = sizeof( Head_t );	

	if ( MakeHead( Msg, len, ClientID, &Head )  )
	{
		ClientSendMsg->Size = len + Head_Size;	
		memcpy( ClientSendMsg->SendBuf, &Head, sizeof( Head ));	
		memcpy( &ClientSendMsg->SendBuf[ Head_Size ], ptrResult, len );		
		return TRUE;
	}
	
	return FALSE;
}

/*======================================================================
Parse Functions
========================================================================*/
/*
typedef struct ClientRecvMsg_
{	
	short Msg;
	int SIPID;	
	BOOL IsFirstMsg;
	TCHAR ClientIP[ MAX_IP_SIZE + 2];
	BYTE * DummyBuf;	
}ClientRecvMsg_t;
*/
BOOL CMessage::ParseStatusMsg( ClientRecvMsg_t * ClientRecvMsg, Statusinfo_t * Status)
{	
	BYTE * ptr = NULL;	
	UINT32 u32;

	if ( ClientRecvMsg == NULL || Status == NULL )
	{
		return FALSE;
	}

	ptr = ClientRecvMsg->DummyBuf;
	memcpy( &u32, ptr, sizeof( int ));
	ptr +=4;

	Status->chk_time = ntohl( u32 );	
	Status->Speak_sts	= *ptr++;
	Status->MIC_sts		= *ptr++;
	Status->Button_sts	= *ptr++;
	Status->ETC_sts		= *ptr++;
	Status->LED_sts		= *ptr++;

			
	return TRUE;
}

BOOL CMessage::ParseErrorNotiMsg( ClientRecvMsg_t * ClientRecvMsg, Errorinfo_t * Err)
{	
	BYTE * ptr = NULL;	

	if ( ClientRecvMsg == NULL || Err == NULL )
	{
		return FALSE;
	}

	ptr = ClientRecvMsg->DummyBuf;	
	Err->Code = *ptr;	
			
	return TRUE;
}


BOOL CMessage::ParseBasicInfoMsg( ClientRecvMsg_t * ClientRecvMsg, Basicinfo_t * Basic)
{
	BYTE * ptr = NULL;
	if ( ClientRecvMsg == NULL || Basic == NULL )
	{
		return FALSE;
	}

	ptr = ClientRecvMsg->DummyBuf;
	memcpy( Basic->display_name , ptr, MAX_DISPLAY_SIP_SIZE );
	ptr += MAX_DISPLAY_SIP_SIZE;
	memcpy( Basic->version , ptr, MAX_DISPLAY_SIP_SIZE );
	ptr += MAX_DISPLAY_SIP_SIZE;
	Basic->volume = ( *ptr & 0xFF );

	return TRUE;
}
BOOL CMessage::ParseHeartbeatMsg( ClientRecvMsg_t * ClientRecvMsg, Heartbeatinfo_t * Heartbeat)
{
	return TRUE;
}

BOOL CMessage::ParseResetMsg( ClientRecvMsg_t * ClientRecvMsg)
{
	return TRUE;
}
BOOL CMessage::ParseWatchdogMsg( ClientRecvMsg_t * ClientRecvMsg)
{
	return TRUE;
}

BOOL CMessage::ParseVolumeInfoMsg( ClientRecvMsg_t * ClientRecvMsg, Volume_t * Volume)
{
	BYTE * ptr = NULL;
	if ( ClientRecvMsg == NULL || Volume == NULL )
	{
		return FALSE;
	}

	ptr = ClientRecvMsg->DummyBuf;
	Volume->vol = ( *ptr & 0xFF);
	
	return TRUE;
}

BOOL CMessage::ParseLED_SETMsg( ClientRecvMsg_t * ClientRecvMsg,  LED_t * LED)
{
	BYTE * ptr = NULL;
	if ( ClientRecvMsg == NULL || LED == NULL )
	{
		return FALSE;
	}

	ptr = ClientRecvMsg->DummyBuf;
	LED->onoff = ( *ptr & 0xFF);

	return TRUE;
}

BOOL CMessage::ParseLED_GETMsg( ClientRecvMsg_t * ClientRecvMsg, LED_t * LED )
{
	BYTE * ptr = NULL;
	if ( ClientRecvMsg == NULL || LED == NULL )
	{
		return FALSE;
	}

	ptr = ClientRecvMsg->DummyBuf;
	LED->onoff = ( *ptr & 0xFF);
	
	return TRUE;
}


BOOL CMessage::ParseGPIO_SETMsg( ClientRecvMsg_t * ClientRecvMsg,  GPIO_t * GPIO )
{
	BYTE * ptr = NULL;
	if ( ClientRecvMsg == NULL || GPIO == NULL )
	{
		return FALSE;
	}

	ptr = ClientRecvMsg->DummyBuf;
	GPIO->type = ( *ptr++ & 0xFF);
	GPIO->onoff = ( *ptr++ & 0xFF);

	return TRUE;
}

BOOL CMessage::ParseGPIO_GETMsg( ClientRecvMsg_t * ClientRecvMsg, GPIO_t * GPIO )
{
	BYTE * ptr = NULL;
	if ( ClientRecvMsg == NULL || GPIO == NULL )
	{
		return FALSE;
	}

	ptr = ClientRecvMsg->DummyBuf;
	GPIO->type = ( *ptr++ & 0xFF);
	GPIO->onoff = ( *ptr++ & 0xFF);
	
	return TRUE;
}

BOOL CMessage::ParseEMG_CALL( ClientRecvMsg_t * ClientRecvMsg  )
{
	return TRUE;
}

BOOL CMessage::ParseCALL_Ready( ClientRecvMsg_t * ClientRecvMsg )
{
	return TRUE;
}



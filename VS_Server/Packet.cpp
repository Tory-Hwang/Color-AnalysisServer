#include "StdAfx.h"
#include "Packet.h"
#include "Com_Def.h"

CPacket::CPacket(void)
{
}

CPacket::~CPacket(void)
{
}


void CPacket::DumpStr( CString &str )
{
#if DEBUG_PACKET
	CString strtxt;
	strtxt.Format(L"VS_SERVER::CPacket : %s \n", str);
	OutputDebugString( strtxt );
#endif
}

int CPacket::GetSIPID( BYTE * Buf )
{
	Head_t Head;
	int SIPID;
	char szID[ MAX_SIPID_SIZE + 1 ];
	
	memset( &Head, 0, sizeof( Head_t ));
	memcpy( &Head, Buf, sizeof( Head_t ));

	SIPID = atoi( szID );


	return SIPID;
}

int CPacket::isValidPacket( int CHKCRC, BYTE * pData, int Size )
{
	int HeadSize=0;
	unsigned short PacketSize =0;
	int TotalSize =0;
	Head_t Head;
	
	int ClientID=0;

	HeadSize = sizeof( Head_t );
	memset( &Head, 0, HeadSize );
	memcpy( &Head, pData, HeadSize );

	PacketSize = Head.LEN;
	ClientID = 1;

	TotalSize = ( PacketSize + HeadSize );

	if (  TotalSize != Size )
	{
		CString strMsg;
		strMsg.Format(L"Packet is Not Valid..ClientID( %d ), PacketSize( %d ), TotalSize( %d ), RecvSize( %d )", ClientID, PacketSize, TotalSize, Size);
		DumpStr( strMsg );
		return -1;
	}

	if ( CHKCRC == CHK_CRC )
	{
#if 0
		if ( isvalid_crc( pdata, TotalSize ) != 1 )
		{
			return -1;
		}
#endif
	}


	/* minus crc size  */	
	return PacketSize;

}

BOOL CPacket::ParsePacket( int CHKCRC, int Size, BYTE * Buf, ClientRecvMsg_t * ClientRecvMsg )
{
	int HeadSize =0;
	unsigned short PacketSize =0;
	BYTE * Packet, * Ptr;
	short Msg =0;


	if ( Buf == NULL || ClientRecvMsg == NULL )
	{
		return FALSE;
	}

	HeadSize	= sizeof( Head_t );
	Packet		= Buf;	
	PacketSize	= ( short )isValidPacket( CHKCRC, Packet, Size );

	if ( PacketSize < 0 )
	{	
		return FALSE;
	}
	
	Ptr = (BYTE*)&Buf[ 0 ];	
	memcpy(&Msg, &Buf[4], 2 );	

	ClientRecvMsg->Msg		= Msg;
	ClientRecvMsg->VAR1		= PacketSize;
	ClientRecvMsg->SIPID	= 1;	
	
	if( PacketSize > 0 )
		memcpy( ClientRecvMsg->DummyBuf, (BYTE*)&Buf[ DATA_POS ], PacketSize );
	
	return TRUE;
}
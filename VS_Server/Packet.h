#pragma once

#include "Com_def.h"

class CPacket
{
public:
	CPacket(void);
	~CPacket(void);

	BOOL ParsePacket( int CHKCRC, int Size, BYTE * Buf, ClientRecvMsg_t * ClientRecvMsg );	
	BOOL MakeInternalMsg( ClientRecvMsg_t * ClientRecvMsg, BYTE Mesg );

private:
	int isValidPacket( int CHKCRC, BYTE * pData, int Size );
	int GetSIPID( BYTE * Buf );
	void DumpStr( CString &str );

};

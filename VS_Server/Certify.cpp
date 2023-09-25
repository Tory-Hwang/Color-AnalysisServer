#include "StdAfx.h"
#include "Certify.h"
#include <iphlpapi.h>
#pragma comment(lib, "IPHLPAPI.lib")


CCertify::CCertify(void)
{
	memset( &m_MyMacList, 0, sizeof( m_MyMacList ));	
	memset( m_szNMTKey, 0, sizeof ( m_szNMTKey ));
}

CCertify::~CCertify(void)
{
}

/* =====================================================
Internal Function
=======================================================*/
void CCertify::CopyKeyVal( void )
{	
	memcpy( m_szNMTKey, CR_CERTIFY_KEY, CR_MAX_KEY_VAL_CNT);
}

int CCertify::StringToInt( CString & strInt)
{
	USES_CONVERSION;
	char szTemp[ MAX_PATH ];

	if ( strInt.GetLength() == 0 )
		return -1;

	memset(szTemp, 0 , sizeof( szTemp ) );

	strcpy_s( szTemp, MAX_PATH, W2A( strInt) );
	return atoi( szTemp );
}

void CCertify::ReadNowTime( void )
{
	char szTime[ 10 ];
	time_t Cur_Time;
	struct tm t ;

	memset( szTime, 0, sizeof( szTime));
	Cur_Time = time( NULL );
	localtime_s(&t, &Cur_Time );

	sprintf_s( szTime, sizeof( szTime ), "%04d%02d%02d", t.tm_year + 1900, t.tm_mon + 1,t.tm_mday);
	m_nNowTime = atol( szTime );
}

void CCertify::ReadCertifyDir( void )
{
	CString strMsg;	
	TCHAR strBlank[ MAX_PATH ];

	memset( m_tcCurPath, 0, MAX_PATH );
	memset( strBlank, 0, sizeof( strBlank) );

	if (GetModuleFileName( AfxGetInstanceHandle( ), strBlank, sizeof( strBlank) ) == 0 )
	{
		return;
	}		

	strMsg.Format(L"%s", strBlank );
	strMsg.ReleaseBuffer();
	strMsg = strMsg.Left(strMsg.ReverseFind('\\'));
	wsprintf(m_tcCurPath, L"%s", strMsg );

}

void CCertify::ReadMyMac( void )
{
	DWORD Err;
	PIP_ADAPTER_INFO pAdapterInfo, pAdapt;
	DWORD AdapterInfoSize;
	int i;

	AdapterInfoSize = 0;

	if ((Err = GetAdaptersInfo(NULL, &AdapterInfoSize)) != 0)
	{
		if (Err != ERROR_BUFFER_OVERFLOW)
		{
			TRACE("GetAdaptersInfo sizing failed with error %d\n", Err);
			return ;
		}
	}

	if ((pAdapterInfo = (PIP_ADAPTER_INFO) GlobalAlloc(GPTR, AdapterInfoSize)) == NULL)
	{
		TRACE("Memory allocation error\n");
		return ;
	}

	if ((Err = GetAdaptersInfo(pAdapterInfo, &AdapterInfoSize)) != 0)
	{
		printf("GetAdaptersInfo failed with error %d\n", Err);
		return ;
	}

	pAdapt = pAdapterInfo;
	while (pAdapt)
	{
		if ( pAdapt->AddressLength == CR_MAX_MAC_VAL_CNT)
		{
			for( i = 0 ; i < CR_MAX_MAC_VAL_CNT ; i++ )
			{
				m_MyMacList.Macs[ m_MyMacList.nCnt ].nMac[ i ] = pAdapt->Address[ i ];
				/* Encryption ( XOR Data ) */
				m_MyMacList.Macs[ m_MyMacList.nCnt ].nMacXor[ i ] = (BYTE)( pAdapt->Address[ i ] ^ m_szNMTKey[ i ]);
			}

			m_MyMacList.nCnt++;
		}
		pAdapt = pAdapt->Next;
	}
	free( pAdapt );
}

void CCertify::WcsToMcs( TCHAR * Org, char * Dest, int OrgSize)
{
	USES_CONVERSION;
	strcpy_s( Dest, OrgSize + 1, W2A( Org) );
}

void CCertify::McsToWcs( char * Org, TCHAR * Dest, int OrgSize)
{
	USES_CONVERSION;	
	wcscpy_s( Dest, OrgSize + 1 , A2W( Org) );	
}

/* =====================================================
Export Function
=======================================================*/
void CCertify::InitCertify( void )
{
	ReadCertifyDir();
	ReadNowTime();
	CopyKeyVal();
	ReadMyMac();
}

int CCertify::IsValidCertify( void )
{
	CString strInit;
	TCHAR strCertify[ CR_MAX_ITEM_SIZE + 2];
	TCHAR strStart[ CR_MAX_ITEM_SIZE + 2];
	TCHAR strEnd[ CR_MAX_ITEM_SIZE + 2];
	TCHAR tcTemp[ 6 ];
	char *ptr, szTemp[ 6 ];

	int i, Size=0;
	int nPos=0;

	CString strTemp;
	CERTIFY_t Certify;

	Size = sizeof( strCertify);

	memset( strCertify, 0, Size );
	memset( strStart, 0, Size );
	memset( strEnd, 0, Size );
	memset( &Certify, 0, sizeof( CERTIFY_t ));

	strInit.Format(L"%s\\NMT_Certify.der", m_tcCurPath );

	GetPrivateProfileString(_T("NMT_CERTIFY_DATA"),_T("CERTIFY1"),_T(""),strCertify, Size ,strInit);
	GetPrivateProfileString(_T("NMT_CERTIFY_DATA"),_T("CERTIFY2"),_T(""),strStart, Size ,strInit);
	GetPrivateProfileString(_T("NMT_CERTIFY_DATA"),_T("CERTIFY3"),_T(""),strEnd, Size ,strInit);

	if ( wcslen( strCertify ) == 0 || wcslen( strStart )== 0 || wcslen( strEnd ) ==0  )
	{
		//AfxMessageBox(L"프로그램 실행에 필요한 인증 정보를 얻을 수 없습니다.");
		return CR_ERR_NOT_FILE;
	}

	for( nPos = 0 , i = 0 ; i < CR_MAX_MAC_VAL_CNT * 2 ; nPos++, i += 2 ) 
	{
		memset( tcTemp, 0, sizeof( tcTemp ));
		memset( szTemp, 0, sizeof( szTemp ));

		memcpy( tcTemp, &strCertify[ i ], sizeof( TCHAR)*2 );
		WcsToMcs( tcTemp, szTemp, (int)wcslen( tcTemp ));

		Certify.Mac.nMacXor[ nPos ] = ( BYTE )strtol( szTemp, &ptr, 16 );
	}

	for( nPos = 0 ,i = 0 ; i < CR_MAX_KEY_VAL_CNT*2 ; nPos++, i +=2 )
	{
		memset( tcTemp, 0, sizeof( tcTemp ));
		memset( szTemp, 0, sizeof( szTemp ));

		memcpy( tcTemp, &strStart[ i ], sizeof( TCHAR)*2 );
		WcsToMcs( tcTemp, szTemp, (int)wcslen( tcTemp ));

		Certify.nValidStatTime[ nPos ] = ( BYTE )strtol( szTemp, &ptr, 16 );
	}

	for( nPos = 0 ,i = 0 ; i < CR_MAX_KEY_VAL_CNT*2 ;nPos++,  i += 2 )
	{
		memset( tcTemp, 0, sizeof( tcTemp ));
		memset( szTemp, 0, sizeof( szTemp ));
		memcpy( tcTemp, &strEnd[ i ], sizeof( TCHAR)*2 );

		WcsToMcs( tcTemp, szTemp, (int)wcslen( tcTemp ));		
		Certify.nValidEndTime[ nPos ] = ( BYTE )strtol( szTemp, &ptr, 16 );
	}

	return 0;
}

BOOL CCertify::RegCertify( CERTIFY_t * ptrCertify )
{
	CString strInit;
	TCHAR strCertify[ CR_MAX_ITEM_SIZE + 2];
	TCHAR strStart[ CR_MAX_ITEM_SIZE + 2];
	TCHAR strEnd[ CR_MAX_ITEM_SIZE + 2];
	int Size=0;
	CString strTemp;
	CERTIFY_t Certify;

	Size = sizeof( strCertify);

	if ( ptrCertify == NULL )
	{
		return CR_ERR_NOT_VALID;
	}

	memset( strCertify, 0, Size );
	memset( strStart, 0, Size );
	memset( strEnd, 0, Size );
	memset( &Certify, 0, sizeof( CERTIFY_t ));

	wsprintf( strCertify, L"%02x%02x%02x%02x%02x%02x", 
		ptrCertify->Mac.nMac[  0  ]^ m_szNMTKey[ 0 ] ,
		ptrCertify->Mac.nMac[  1  ]^ m_szNMTKey[ 1 ] ,
		ptrCertify->Mac.nMac[  2  ]^ m_szNMTKey[ 2 ] ,
		ptrCertify->Mac.nMac[  3  ]^ m_szNMTKey[ 3 ] ,
		ptrCertify->Mac.nMac[  4  ]^ m_szNMTKey[ 4 ] ,
		ptrCertify->Mac.nMac[  5  ]^ m_szNMTKey[ 5 ] ,
		ptrCertify->Mac.nMac[  6  ]^ m_szNMTKey[ 6 ] 
	);

	wsprintf( strStart, L"%02x%02x%02x%02x%02x%02x%02x%02x", 
		ptrCertify->nValidStatTime[  0  ]^ m_szNMTKey[ 0 ],
		ptrCertify->nValidStatTime[  1  ]^ m_szNMTKey[ 1 ],
		ptrCertify->nValidStatTime[  2  ]^ m_szNMTKey[ 2 ],
		ptrCertify->nValidStatTime[  3  ]^ m_szNMTKey[ 3 ],
		ptrCertify->nValidStatTime[  4  ]^ m_szNMTKey[ 4 ],
		ptrCertify->nValidStatTime[  5  ]^ m_szNMTKey[ 5 ],
		ptrCertify->nValidStatTime[  6  ]^ m_szNMTKey[ 6 ],
		ptrCertify->nValidStatTime[  7  ]^ m_szNMTKey[ 7 ]		
	);

	wsprintf( strEnd, L"%02x%02x%02x%02x%02x%02x%02x%02x", 
		ptrCertify->nValidEndTime[  0  ]^ m_szNMTKey[ 0 ],
		ptrCertify->nValidEndTime[  1  ]^ m_szNMTKey[ 1 ],
		ptrCertify->nValidEndTime[  2  ]^ m_szNMTKey[ 2 ],
		ptrCertify->nValidEndTime[  3  ]^ m_szNMTKey[ 3 ],
		ptrCertify->nValidEndTime[  4  ]^ m_szNMTKey[ 4 ],
		ptrCertify->nValidEndTime[  5  ]^ m_szNMTKey[ 5 ],
		ptrCertify->nValidEndTime[  6  ]^ m_szNMTKey[ 6 ],
		ptrCertify->nValidEndTime[  7  ]^ m_szNMTKey[ 7 ]		
	);

	strInit.Format(L"%s\\NMT_Certify.der", m_tcCurPath );
	WritePrivateProfileString(_T("NMT_CERTIFY_DATA"),_T("CERTIFY1"),strCertify, strInit);
	WritePrivateProfileString(_T("NMT_CERTIFY_DATA"),_T("CERTIFY2"),strStart, strInit);
	WritePrivateProfileString(_T("NMT_CERTIFY_DATA"),_T("CERTIFY3"),strEnd, strInit);

	return TRUE;
}

BOOL CCertify::GetMac( int nPos, MAC_t * ptrMac)
{
	if ( nPos > m_MyMacList.nCnt-1 )
		return FALSE;

	memcpy( ptrMac, &m_MyMacList.Macs[ nPos ] , sizeof( MAC_t ));
	return TRUE;
}

Mac_Manager_t * CCertify::GetMacManager( void )
{
	return &m_MyMacList;
}

CERTIFY_t * CCertify::GetValidCertify( void )
{
	return &m_ValidCertify;
}

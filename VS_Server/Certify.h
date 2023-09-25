#pragma once


#define CR_ERR_SUCCESS			0
#define CR_ERR_NOT_VALID		-1
#define CR_ERR_OVER_TIME		-2
#define CR_ERR_NOT_FILE			-3

#define CR_ERR_NOT_VALID_STR	L"인증서의 인증정보가 맞지 않습니다.\r프로그램을 종료 합니다"
#define CR_ERR_OVER_TIME_STR	L"인증서의 유효기간이 초과 하였습니다.\r프로그램을 종료 합니다"
#define CR_ERR_NOT_FILE_STR		L"인증서 파일을 찾을 수 없습니다.\r프로그램을 종료 합니다"
#define CR_ERR_ETC_STR			L"인증서에러 \r프로그램을 종료 합니다"
#define CR_ERR_SUCCESS_STR		L"정상적인 인증서 입니다."


#define CR_MAX_ITEM_SIZE		50
#define CR_MAX_MAC_CNT			5
#define CR_MAX_MAC_VAL_CNT		6
#define CR_MAX_KEY_VAL_CNT		8

#define CR_CERTIFY_KEY			"NMT7HJC8"
#define CR_CERTIFY_NAME			"NMTCERTIFY.dat"

typedef struct MAC_
{
	BYTE nMac[ CR_MAX_MAC_VAL_CNT +1];
	BYTE nMacXor[ CR_MAX_MAC_VAL_CNT +1];
}MAC_t;

typedef struct MAC_Manager_
{
	int nCnt;
	MAC_t Macs[ CR_MAX_MAC_CNT ];
}Mac_Manager_t;

typedef struct CERTIFY_
{
	MAC_t Mac;	
	BYTE nValidStatTime[CR_MAX_KEY_VAL_CNT + 1];
	BYTE nValidEndTime[CR_MAX_KEY_VAL_CNT + 1];
}CERTIFY_t;

class CCertify
{
public:
	CCertify(void);
	~CCertify(void);

	void InitCertify( void );
	BOOL IsValidCertify( void );
	BOOL RegCertify( CERTIFY_t * ptrCertify );
	BOOL GetMac( int nPos, MAC_t * ptrMac);
	void WcsToMcs( TCHAR * Org, char * Dest, int OrgSize );
	Mac_Manager_t * GetMacManager( void );
	CERTIFY_t * GetValidCertify( void );

private:
	void ReadMyMac( void );
	void CopyKeyVal( void );	
	void ReadNowTime( void );
	void ReadCertifyDir( void );

	void McsToWcs( char * Org, TCHAR * Dest, int OrgSize );
	int StringToInt( CString & strInt);

	long m_nNowTime;
	char m_szNMTKey[ CR_MAX_KEY_VAL_CNT + 1 ];
	TCHAR m_tcCurPath[ MAX_PATH ];
	Mac_Manager_t m_MyMacList;
	CERTIFY_t m_ValidCertify;

};

#include "StdAfx.h"
#include "SVR_Doc.h"

CSVR_Doc::CSVR_Doc(void)
{
	this->m_nVSCount = 0;
	memset(&m_stSvrInfo, 0, sizeof(VSSERVERINFO_t));
	
}

CSVR_Doc::~CSVR_Doc(void)
{
	this->ClearVSList();
}

const PVSSERVERINFO_t CSVR_Doc::GetServerInfo()
{/* VS Server 정보 얻기 */
	
	return &m_stSvrInfo;
}

BOOL CSVR_Doc::SetServerInfo(VSSERVERINFO_t * stSvrInfo)
{

	/* VS Server 정보 세팅 */
	if(stSvrInfo == NULL)
		return FALSE;
#if 0
	memcpy(m_stSvrInfo.csIP, stSvrInfo->csIP, MAX_IP_SIZE * 2);
	m_stSvrInfo.nID = stSvrInfo->nID;
	memcpy(m_stSvrInfo.csDBDSN, stSvrInfo->csDBDSN, sizeof(stSvrInfo->csDBDSN));
	memcpy(m_stSvrInfo.csDBPass, stSvrInfo->csDBPass, sizeof(stSvrInfo->csDBPass));
	memcpy(m_stSvrInfo.csDBUser, stSvrInfo->csDBUser, sizeof(stSvrInfo->csDBUser));
	memcpy(m_stSvrInfo.csIP, stSvrInfo->csIP, sizeof(stSvrInfo->csIP));
	memcpy(m_stSvrInfo.csMonitorPrgName, stSvrInfo->csMonitorPrgName, sizeof(stSvrInfo->csMonitorPrgName));
	m_stSvrInfo.nTcpPort = stSvrInfo->nTcpPort;
	m_stSvrInfo.nUdpPort = stSvrInfo->nUdpPort;

	/* 임시 데이터 세팅 */
	m_stSvrInfo.nVSCount = 50;
	m_stSvrInfo.nFloorCount = 4;
	memcpy(m_stSvrInfo.stFloor[0].csCaption, L"B1", 2 * 2);
	m_stSvrInfo.stFloor[0].csCaption[5] = '\0';
	memcpy(m_stSvrInfo.stFloor[0].csFloorName, L"M01", 4 * 2);
	m_stSvrInfo.stFloor[0].csCaption[4] = '\0';
	memcpy(m_stSvrInfo.stFloor[1].csCaption, L"B2", 2 * 2);
	m_stSvrInfo.stFloor[1].csCaption[5] = '\0';
	memcpy(m_stSvrInfo.stFloor[1].csFloorName, L"M02", 4 * 2);
	m_stSvrInfo.stFloor[1].csCaption[4] = '\0';
	memcpy(m_stSvrInfo.stFloor[2].csCaption, L"B3", 2 * 2);
	m_stSvrInfo.stFloor[2].csCaption[5] = '\0';
	memcpy(m_stSvrInfo.stFloor[2].csFloorName, L"M03", 4 * 2);
	m_stSvrInfo.stFloor[2].csCaption[4] = '\0';
	memcpy(m_stSvrInfo.stFloor[3].csCaption, L"B4", 2 * 2);
	m_stSvrInfo.stFloor[3].csCaption[5] = '\0';
	memcpy(m_stSvrInfo.stFloor[3].csFloorName, L"M04", 4 * 2);
	m_stSvrInfo.stFloor[3].csCaption[4] = '\0';
	for(int i=1; i<5; i++)
	{
		m_stSvrInfo.stFloor[i].nIndex = i;
	}
#else
	memset( &m_stSvrInfo, 0, sizeof( VSSERVERINFO_t ));
	memcpy( &m_stSvrInfo, stSvrInfo, sizeof( VSSERVERINFO_t ));
#endif
	/* VSIconSetting.ini 의 층 설정 정보 얻어옴 */
	CString strSectionName, strKeyName, strDefault = L"";
	TCHAR csVal[10] = {0,};
	int nDefault = 0, nVal = 0;

	CString strIniPath;
	::GetModuleFileNameW(NULL, strIniPath.GetBuffer(MAX_PATH), MAX_PATH * 2);
	strIniPath.ReleaseBuffer();
	int nPos = strIniPath.ReverseFind('\\');
	strIniPath = strIniPath.Left(nPos);
	strIniPath += L"\\VSIconSetting.ini";

	strSectionName = L"FieldInfo";
	strKeyName = L"VSCount";
	nVal = GetPrivateProfileInt(strSectionName, strKeyName, nDefault, strIniPath);
	m_stSvrInfo.nVSCount = nVal;
	
	strKeyName = L"FloorCount";
	nVal = GetPrivateProfileInt(strSectionName, strKeyName, nDefault, strIniPath);
	m_stSvrInfo.nFloorCount = nVal;

	strKeyName = L"FloorCode1";
	GetPrivateProfileString(strSectionName, strKeyName, strDefault, csVal, sizeof(csVal), strIniPath);
	memcpy(m_stSvrInfo.stFloor[0].csFloorName, csVal, sizeof(m_stSvrInfo.stFloor[0].csFloorName));
	strKeyName = L"FloorCode2";
	GetPrivateProfileString(strSectionName, strKeyName, strDefault, csVal, sizeof(csVal), strIniPath);
	memcpy(m_stSvrInfo.stFloor[1].csFloorName, csVal, sizeof(m_stSvrInfo.stFloor[0].csFloorName));
	strKeyName = L"FloorCode3";
	GetPrivateProfileString(strSectionName, strKeyName, strDefault, csVal, sizeof(csVal), strIniPath);
	memcpy(m_stSvrInfo.stFloor[2].csFloorName, csVal, sizeof(m_stSvrInfo.stFloor[0].csFloorName));
	strKeyName = L"FloorCode4";
	GetPrivateProfileString(strSectionName, strKeyName, strDefault, csVal, sizeof(csVal), strIniPath);
	memcpy(m_stSvrInfo.stFloor[3].csFloorName, csVal, sizeof(m_stSvrInfo.stFloor[0].csFloorName));

	strKeyName = L"FloorCaption1";
	GetPrivateProfileString(strSectionName, strKeyName, strDefault, csVal, sizeof(csVal), strIniPath);
	memcpy(m_stSvrInfo.stFloor[0].csCaption, csVal, sizeof(m_stSvrInfo.stFloor[0].csCaption));
	strKeyName = L"FloorCaption2";
	GetPrivateProfileString(strSectionName, strKeyName, strDefault, csVal, sizeof(csVal), strIniPath);
	memcpy(m_stSvrInfo.stFloor[1].csCaption, csVal, sizeof(m_stSvrInfo.stFloor[1].csCaption));
	strKeyName = L"FloorCaption3";
	GetPrivateProfileString(strSectionName, strKeyName, strDefault, csVal, sizeof(csVal), strIniPath);
	memcpy(m_stSvrInfo.stFloor[2].csCaption, csVal, sizeof(m_stSvrInfo.stFloor[2].csCaption));
	strKeyName = L"FloorCaption4";
	GetPrivateProfileString(strSectionName, strKeyName, strDefault, csVal, sizeof(csVal), strIniPath);
	memcpy(m_stSvrInfo.stFloor[3].csCaption, csVal, sizeof(m_stSvrInfo.stFloor[3].csCaption));

	for(int i=0; i<m_stSvrInfo.nFloorCount; i++)
	{
		m_stSvrInfo.stFloor[i].nIndex = i + 1;
	}
	return TRUE;
}

vector <PVSENSOR_t> CSVR_Doc::GetVSList()
{
	return m_vecVSList;
}

void CSVR_Doc::SetVSList(vector <PVSENSOR_t> pList)
{
	m_vecVSList = pList;
	m_nVSCount = m_vecVSList.size();
}

void CSVR_Doc::ClearVSList()
{/* 모든 VS 리스트 메모리 해제, 리스트 비움 */
	PVSENSOR_t pstVS;
	vector <PVSENSOR_t>	::iterator it;
	for(it = m_vecVSList.begin(); it != m_vecVSList.end(); it++)
	{
		pstVS = *it;
		if(pstVS)
		{
			delete pstVS;
			pstVS = NULL;
		}
	}
	m_vecVSList.erase(m_vecVSList.begin(), m_vecVSList.end());
}

int CSVR_Doc::GetVSCount()
{
	return m_nVSCount;
}

void CSVR_Doc::SetVSListReserve(int nSize)
{
	m_vecVSList.reserve(nSize);
}

void CSVR_Doc::SetTempVSList()
{/* 임시로 50개 VS 정보 입력 */
	
	ClearVSList();

	for(int i=1; i<21; i++)
	{
		PVSENSOR_t pstVS = new VSENSOR_t;
		memset(pstVS, 0, sizeof(VSENSOR_t));
		pstVS->nVSID = i;
		CString strIP;
		strIP.Format(L"192.168.100.%d", i);
		strIP.ReleaseBuffer();
		memcpy(pstVS->csIP, strIP.GetBuffer(), strIP.GetLength() * 2);
		pstVS->csIP[strIP.GetLength()] = '\0';
		
		CString strLocation = L"M01-AA-01";
		memcpy(pstVS->csLocation, strLocation.GetBuffer(), strLocation.GetLength() * 2);
		strLocation.ReleaseBuffer();
		pstVS->csLocation[strLocation.GetLength()] = '\0';
		CString strFloor = strLocation.Left(3);
		memcpy(pstVS->csFloor, strFloor.GetBuffer(), strFloor.GetLength() * 2);
		strFloor.ReleaseBuffer();

		this->m_vecVSList.push_back(pstVS);
	}
	for(int i=21; i<41; i++)
	{
		PVSENSOR_t pstVS = new VSENSOR_t;
		memset(pstVS, 0, sizeof(VSENSOR_t));
		pstVS->nVSID = i;
		CString strIP;
		strIP.Format(L"192.168.100.%d", i);
		strIP.ReleaseBuffer();
		memcpy(pstVS->csIP, strIP.GetBuffer(), strIP.GetLength() * 2);
		pstVS->csIP[strIP.GetLength()] = '\0';
		
		CString strLocation;
		strLocation.Format( L"M02-BB-%02d", i);
		memcpy(pstVS->csLocation, strLocation, strLocation.GetLength() * 2);
		pstVS->csLocation[strLocation.GetLength()] = '\0';
		CString strFloor = strLocation.Left(3);
		memcpy(pstVS->csFloor, strFloor.GetBuffer(), strFloor.GetLength() * 2);
		strFloor.ReleaseBuffer();


		this->m_vecVSList.push_back(pstVS);
	}
	for(int i=41; i<61; i++)
	{
		PVSENSOR_t pstVS = new VSENSOR_t;
		memset(pstVS, 0, sizeof(VSENSOR_t));
		pstVS->nVSID = i;
		CString strIP;
		strIP.Format(L"192.168.100.%d", i);
		strIP.ReleaseBuffer();
		memcpy(pstVS->csIP, strIP.GetBuffer(), strIP.GetLength() * 2);
		pstVS->csIP[strIP.GetLength()] = '\0';
		
		CString strLocation;
		strLocation.Format( L"M03-CC-%02d", i );
		strLocation.ReleaseBuffer();
		memcpy(pstVS->csLocation, strLocation, strLocation.GetLength() * 2);
		pstVS->csLocation[strLocation.GetLength()] = '\0';
		CString strFloor = strLocation.Left(3);
		memcpy(pstVS->csFloor, strFloor.GetBuffer(), strFloor.GetLength() * 2);
		strFloor.ReleaseBuffer();


		this->m_vecVSList.push_back(pstVS);
	}
	for(int i=61; i<81; i++)
	{
		PVSENSOR_t pstVS = new VSENSOR_t;
		memset(pstVS, 0, sizeof(VSENSOR_t));
		pstVS->nVSID = i;
		CString strIP;
		strIP.Format(L"192.168.100.%d", i);
		strIP.ReleaseBuffer();
		memcpy(pstVS->csIP, strIP.GetBuffer(), strIP.GetLength() * 2);
		pstVS->csIP[strIP.GetLength()] = '\0';
		
		CString strLocation;
		strLocation.Format( L"M04-DD-%02d", i);
		memcpy(pstVS->csLocation, strLocation, strLocation.GetLength() * 2);
		pstVS->csLocation[strLocation.GetLength()] = '\0';
		CString strFloor = strLocation.Left(3);
		memcpy(pstVS->csFloor, strFloor.GetBuffer(), strFloor.GetLength() * 2);
		strFloor.ReleaseBuffer();


		this->m_vecVSList.push_back(pstVS);
	}
	m_nVSCount = m_vecVSList.size();
	
}

void CSVR_Doc::SetCurUser(VST_USER* pstUser)
{
	memset(&m_stCurUser, 0x00, sizeof(VST_USER));
	memcpy(&m_stCurUser, pstUser, sizeof(VST_USER));
}

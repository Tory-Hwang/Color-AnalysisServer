#pragma once

#include "COM_DEF.h"
#include "db_def.h"
#include "Controls_def.h"

#include <vector>
#include <algorithm>

using namespace std;

/* 공통으로 사용되는 데이터 관리 */

class CSVR_Doc
{
public:
	CSVR_Doc(void);
public:
	virtual ~CSVR_Doc(void);

	VST_USER		m_stCurUser;						/* 현재 로그인한 사용자 */
	BOOL			m_bLogin;
	long			m_nCurSessionKey;						/* 현재 로그인 세션의 Unique KEY */

private:
	VSSERVERINFO_t	m_stSvrInfo;						/* VS Serser 정보 */
	int				m_nVSCount;							/* VS 개수 */
	vector <PVSENSOR_t>	m_vecVSList;					/* VS List */

	/* 현재 시스템 정보 */
	int				m_nCPUUsage;
	int				m_nMemUsage;
	int				m_nRecv;
	int				m_nSent;
	int				m_nHDDUsage;

	/* 경보음 알려줄 시스템 상태 */
	int				m_nCPUAlert;
	int				m_nMemAlert;
	int				m_nRecvAlert;
	int				m_nSendAlert;
	int				m_nHDDAlert;

public:
	const PVSSERVERINFO_t GetServerInfo();				/* VS Server 정보 얻기 */
	BOOL SetServerInfo(VSSERVERINFO_t * stSvrInfo);		/* VS Server 정보 세팅 */
	vector <PVSENSOR_t> GetVSList();					/* VS List 포인터 리턴 */
	void SetVSList(vector <PVSENSOR_t> pList);			/* VS List 리스트 세팅 */
	void ClearVSList();									/* VS List Clear */
	void SetVSListReserve(int nSize);					/* VS List Reserve */
	int GetVSCount();									
	BOOL GetLogin() { return m_bLogin; };
	void SetLogin(BOOL bLogin) { m_bLogin = bLogin; };
	void SetCurUser(VST_USER* pstUser);
	PVST_USER GetCurUser() {return &m_stCurUser;};
	void SetSessionKey(long nSessionKey) {m_nCurSessionKey = nSessionKey;};
	long GetSessionKey() {return m_nCurSessionKey;};

	void SetCPUUsage(int nVal) { m_nCPUUsage = nVal; };
	void SetMemUsage(int nVal) {m_nMemUsage = nVal;};
	void SetRecv(int nVal) {m_nRecv = nVal;};
	void SetSent(int nVal) {m_nSent = nVal;};
	void SetHddUsage(int nVal) {m_nHDDUsage = nVal;};

	int GetCPUUsage() { return m_nCPUUsage; };
	int GetMemUsage() { return m_nMemUsage; };
	int GetRecv() { return m_nRecv; };
	int GetSent() { return m_nSent; };
	int GetHDDsage() { return m_nHDDUsage; };

	void SetCPUAlert(int nVal) { m_nCPUAlert = nVal; };
	void SetMemAlert(int nVal) {m_nMemAlert = nVal;};
	void SetRecvAlert(int nVal) {m_nRecvAlert = nVal;};
	void SetSentAlert(int nVal) {m_nSendAlert = nVal;};
	void SetHddAlert(int nVal) {m_nHDDAlert = nVal;};

	int GetCPUAlert() { return m_nCPUAlert; };
	int GetMemAlert() { return m_nMemAlert; };
	int GetRecvAlert() { return m_nRecvAlert; };
	int GetSentAlert() { return m_nSendAlert; };
	int GetHDDAlert() { return m_nHDDAlert; };

	void SetTempVSList();								/* Test용 데이터 입력 */
};

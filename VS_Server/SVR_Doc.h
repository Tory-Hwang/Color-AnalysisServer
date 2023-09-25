#pragma once

#include "COM_DEF.h"
#include "db_def.h"
#include "Controls_def.h"

#include <vector>
#include <algorithm>

using namespace std;

/* �������� ���Ǵ� ������ ���� */

class CSVR_Doc
{
public:
	CSVR_Doc(void);
public:
	virtual ~CSVR_Doc(void);

	VST_USER		m_stCurUser;						/* ���� �α����� ����� */
	BOOL			m_bLogin;
	long			m_nCurSessionKey;						/* ���� �α��� ������ Unique KEY */

private:
	VSSERVERINFO_t	m_stSvrInfo;						/* VS Serser ���� */
	int				m_nVSCount;							/* VS ���� */
	vector <PVSENSOR_t>	m_vecVSList;					/* VS List */

	/* ���� �ý��� ���� */
	int				m_nCPUUsage;
	int				m_nMemUsage;
	int				m_nRecv;
	int				m_nSent;
	int				m_nHDDUsage;

	/* �溸�� �˷��� �ý��� ���� */
	int				m_nCPUAlert;
	int				m_nMemAlert;
	int				m_nRecvAlert;
	int				m_nSendAlert;
	int				m_nHDDAlert;

public:
	const PVSSERVERINFO_t GetServerInfo();				/* VS Server ���� ��� */
	BOOL SetServerInfo(VSSERVERINFO_t * stSvrInfo);		/* VS Server ���� ���� */
	vector <PVSENSOR_t> GetVSList();					/* VS List ������ ���� */
	void SetVSList(vector <PVSENSOR_t> pList);			/* VS List ����Ʈ ���� */
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

	void SetTempVSList();								/* Test�� ������ �Է� */
};

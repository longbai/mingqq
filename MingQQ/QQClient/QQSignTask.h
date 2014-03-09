#pragma once

#include "QQUser.h"
#include "QQProtocol.h"
#include "ThreadPool.h"

enum QQSIGN_OP_TYPE		// ��������
{
	QQSIGN_OP_TYPE_GET,	// ��ȡ
	QQSIGN_OP_TYPE_SET	// ����
};

struct QQSIGN_GET_PARAM
{
	BOOL bIsGMember;	// ��Ⱥ��Ա���Ǻ���
	UINT nGroupCode;	// Ⱥ����
	UINT nQQUin;		// ����Uin����Ⱥ��ԱUin
};

struct QQSIGN_SET_PARAM
{
	tstring m_strSign;	// ��Ҫ���õĸ���ǩ��
};

class CQQSignTask : public CThreadPoolTask
{
public:
	CQQSignTask(void);
	~CQQSignTask(void);

public:
	virtual int Run();
	virtual int Stop();
	virtual void TaskFinish();

	BOOL AddGetParam(BOOL bIsGMember, UINT nGroupCode, UINT nQQUin);
	BOOL AddSetParam(LPCTSTR lpSign);
	void DelAllItems();

public:
	CQQUser * m_lpQQUser;
	CQQProtocol * m_lpQQProtocol;
	QQSIGN_OP_TYPE m_nType;
	std::vector<QQSIGN_GET_PARAM> m_arrGetParam;
	std::vector<QQSIGN_SET_PARAM> m_arrSetParam;

private:
	BOOL m_bStop;
	CHttpClient m_HttpClient;
};

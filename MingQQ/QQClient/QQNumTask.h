#pragma once

#include "QQUser.h"
#include "QQProtocol.h"
#include "ThreadPool.h"

enum QQNUM_OP_TYPE			// ��������
{
	OP_TYPE_GETBUDDYNUM,	// ��ȡ���Ѻ���
	OP_TYPE_GETGMEMBERNUM,	// ��ȡȺ��Ա����
	OP_TYPE_GETGROUPNUM,	// ��ȡȺ����
};

struct QQNUM_PARAM
{
	QQNUM_OP_TYPE nType;	// ��������
	UINT nGroupCode;		// Ⱥ����
	UINT nQQUin;			// ����Uin����Ⱥ��ԱUin
};

class CQQNumTask : public CThreadPoolTask
{
public:
	CQQNumTask(void);
	~CQQNumTask(void);

public:
	virtual int Run();
	virtual int Stop();
	virtual void TaskFinish();

	BOOL AddParam(QQNUM_OP_TYPE nType, UINT nGroupCode, UINT nQQUin);
	void DelAllItems();

public:
	CQQUser * m_lpQQUser;
	CQQProtocol * m_lpQQProtocol;
	std::vector<QQNUM_PARAM> m_arrParam;

private:
	BOOL m_bStop;
	CHttpClient m_HttpClient;
};

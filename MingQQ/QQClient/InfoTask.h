#pragma once

#include "QQUser.h"
#include "QQProtocol.h"
#include "ThreadPool.h"

enum INFO_OP_TYPE			// ��������
{
	OP_TYPE_GETBUDDYINFO,	// ��ȡ������Ϣ
	OP_TYPE_GETGMEMBERINFO,	// ��ȡȺ��Ա��Ϣ
	OP_TYPE_GETGROUPINFO,	// ��ȡȺ��Ϣ
	OP_TYPE_SETBUDDYINFO	// ���ú�����Ϣ
};

class CInfoTask : public CThreadPoolTask
{
public:
	CInfoTask(void);
	~CInfoTask(void);

public:
	virtual int Run();
	virtual int Stop();
	virtual void TaskFinish();

public:
	CQQUser * m_lpQQUser;
	CQQProtocol * m_lpQQProtocol;
	INFO_OP_TYPE m_nType;	// ��������
	UINT m_nGroupCode;		// Ⱥ����
	UINT m_nQQUin;			// ����Uin����Ⱥ��ԱUin

private:
	BOOL m_bStop;
	CHttpClient m_HttpClient;
};

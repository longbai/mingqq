#pragma once

#include "QQUser.h"
#include "QQProtocol.h"
#include "ThreadPool.h"
#include "SendMsgTask.h"
#include "RecvMsgTask.h"
#include "PollTask.h"
#include "QQNumTask.h"
#include "QQSignTask.h"
#include "GetHeadPicTask.h"

class CLoginTask : public CThreadPoolTask
{
public:
	CLoginTask(void);
	~CLoginTask(void);

public:
	virtual int Run();
	virtual int Stop();
	virtual void TaskFinish();

private:
	CBuddyInfoResult * GetUserInfo();			// ��ȡ�û���Ϣ
	CGetSignResult * GetUserSign();				// ��ȡ�û�ǩ��
	CBuddyListResult * GetBuddyList();			// ��ȡ�����б�
	CGroupListResult * GetGroupList();			// ��ȡȺ�б�
	CRecentListResult * GetRecentList();		// ��ȡ�����ϵ���б�
	BOOL StartSendMsgTask();					// ����������Ϣ����
	BOOL StartRecvMsgTask();					// ����������Ϣ����
	BOOL StartPollTask();						// ������ѯ����
	BOOL StartGetBuddyNumberTask(CBuddyListResult * lpBuddyListResult, 
		CGroupListResult * lpGroupListResult);	// ������ȡ����QQ��������
	BOOL StartGetBuddySignTask(CBuddyListResult * lpBuddyListResult);	// ������ȡ����QQǩ������
	BOOL StartGetHeadPicTask(CBuddyListResult * lpBuddyListResult, 
		CGroupListResult * lpGroupListResult);	// ������ȡͷ��ͼƬ����

public:
	CQQUser * m_lpQQUser;
	CQQProtocol * m_lpQQProtocol;
	CThreadPool * m_lpThreadPool;
	CSendMsgTask * m_lpSendMsgTask;
	CRecvMsgTask * m_lpRecvMsgTask;
	CPollTask * m_lpPollTask;
	
private:
	BOOL m_bStop;
	CHttpClient m_HttpClient;
};

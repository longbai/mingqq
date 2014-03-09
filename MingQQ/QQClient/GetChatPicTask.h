#pragma once

#include "ThreadPool.h"
#include "QQUser.h"
#include "QQProtocol.h"

enum GETCHATPIC_OP_TYPE
{
	OP_TYPE_BUDDY_PIC,
	OP_TYPE_GROUP_PIC,
	OP_TYPE_SESS_PIC	// WebQQЭ��Ŀǰ��֧����ʱ�Ự����/�����Զ������
};

class CGetChatPicTask : public CThreadPoolTask
{
public:
	CGetChatPicTask(void);
	~CGetChatPicTask(void);

public:
	virtual int Run();
	virtual int Stop();
	virtual void TaskFinish();

private:
	BOOL SavePic(LPCTSTR lpszFileName, const BYTE * lpData, DWORD dwSize);	// ����ͼƬ

public:
	CQQUser * m_lpQQUser;
	CQQProtocol * m_lpQQProtocol;
	GETCHATPIC_OP_TYPE m_nType;
	void * m_lpMsg;

private:
	BOOL m_bStop;
	CHttpClient m_HttpClient;
};

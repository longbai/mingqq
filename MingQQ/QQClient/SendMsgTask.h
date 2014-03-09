#pragma once

#include "ThreadPool.h"
#include "QQUser.h"
#include "QQProtocol.h"

// д��һ��������Ϣ��¼
void WriteBuddyMsgLog(CQQUser * lpQQUser, UINT nQQNum, LPCTSTR lpNickName, 
					  BOOL bSelf, CBuddyMessage * lpMsg);

// д��һ��Ⱥ��Ϣ��¼
void WriteGroupMsgLog(CQQUser * lpQQUser, UINT nGroupNum, UINT nQQNum, 
					  LPCTSTR lpNickName, CGroupMessage * lpMsg);

// д��һ����ʱ�Ự(Ⱥ��Ա)��Ϣ��¼
void WriteSessMsgLog(CQQUser * lpQQUser, UINT nQQNum, LPCTSTR lpNickName, 
					 BOOL bSelf, CSessMessage * lpMsg);

class CMsgItem
{
public:
	CMsgItem(void);
	~CMsgItem(void);

public:
	QQ_MSG_TYPE m_nType;
	void * m_lpMsg;
	UINT m_nGroupNum;
	UINT m_nQQNum;
	tstring m_strNickName;
	tstring m_strGroupSig;
};

class CSendMsgTask : public CThreadPoolTask
{
public:
	CSendMsgTask(void);
	~CSendMsgTask(void);

public:
	virtual int Run();
	virtual int Stop();
	virtual void TaskFinish();
	
	BOOL AddBuddyMsg(UINT nToUin, time_t nTime, LPCTSTR lpMsg);
	BOOL AddGroupMsg(UINT nGroupId, time_t nTime, LPCTSTR lpMsg);
	BOOL AddSessMsg(UINT nGroupId, UINT nToUin, time_t nTime, LPCTSTR lpMsg);

private:
	BOOL HandleFontInfo(LPCTSTR& p, tstring& strText, std::vector<CContent *>& arrContent);
	BOOL HandleSysFaceId(LPCTSTR& p, tstring& strText, std::vector<CContent *>& arrContent);
	BOOL HandleCustomPic(LPCTSTR& p, tstring& strText, std::vector<CContent *>& arrContent);
	BOOL CreateMsgContent(LPCTSTR lpMsg, std::vector<CContent *>& arrContent);
	BOOL UploadCustomFace(LPCTSTR lpszFileName, CUploadCustomFaceResult& result);		// �ϴ��Զ������
	BOOL SendBuddyMsg(CMsgItem * lpMsgItem);	// ���ͺ�����Ϣ
	BOOL SendGroupMsg(CMsgItem * lpMsgItem);	// ����Ⱥ��Ϣ
	BOOL SendSessMsg(CMsgItem * lpMsgItem);		// ����Ⱥ��Ա��Ϣ

public:
	CQQUser * m_lpQQUser;
	CQQProtocol * m_lpQQProtocol;

private:
	CHttpClient m_HttpClient;
	UINT m_nMsgId;
	CRITICAL_SECTION m_csItem;
	HANDLE m_hSemaphore;
	HANDLE m_hEvent;
	std::vector<CMsgItem *> m_arrItem;
	tstring m_strGFaceKey, m_strGFaceSig;
};

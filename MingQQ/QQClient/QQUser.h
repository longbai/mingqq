#pragma once

#include "../Path.h"
#include "QQProtocolData.h"
#include "MessageLogger.h"

class CQQUser
{
public:
	CQQUser(void);
	~CQQUser(void);

public:
	tstring GetUserFolder();							// ��ȡ�û��ļ��д��·��
	void SetUserFolder(LPCTSTR lpPath);					// �����û��ļ��д��·��
	
	tstring GetPersonalFolder(UINT nUserNum = 0);		// ��ȡ�����ļ��д��·��
	tstring GetChatPicFolder(UINT nUserNum = 0);		// ��ȡ����ͼƬ���·��

	tstring GetUserHeadPicFullName(UINT nUserNum = 0);	// ��ȡ�û�ͷ��ͼƬȫ·���ļ���
	tstring GetBuddyHeadPicFullName(UINT nQQNum);		// ��ȡ����ͷ��ͼƬȫ·���ļ���
	tstring GetGroupHeadPicFullName(UINT nGroupNum);	// ��ȡȺͷ��ͼƬȫ·���ļ���
	tstring GetSessHeadPicFullName(UINT nQQNum);		// ��ȡȺ��Աͷ��ͼƬȫ·���ļ���
	tstring GetChatPicFullName(LPCTSTR lpszFileName);	// ��ȡ����ͼƬȫ·���ļ���
	tstring GetMsgLogFullName(UINT nUserNum = 0);		// ��ȡ��Ϣ��¼ȫ·���ļ���

	BOOL IsNeedUpdateBuddyHeadPic(UINT nQQNum);			// �ж��Ƿ���Ҫ���º���ͷ��
	BOOL IsNeedUpdateGroupHeadPic(UINT nGroupNum);		// �ж��Ƿ���Ҫ����Ⱥͷ��
	BOOL IsNeedUpdateSessHeadPic(UINT nQQNum);			// �ж��Ƿ���Ҫ����Ⱥ��Աͷ��

	time_t GetServerTime();								// ��ȡ������ʱ��

public:
	tstring m_strQQNum;
	tstring m_strQQPwd;
	UINT m_nQQUin;
	QQ_STATUS m_nStatus;
	HWND m_hProxyWnd;
	HWND m_hCallBackWnd;
	QQ_STATUS m_nLoginStatus;
	CBuffer m_VerifyCodePic;
	tstring m_strVerifyCode;
	CVerifyCodeInfo m_VerifyCodeInfo;
	CLoginResult_1 m_LoginResult1;
	CLoginResult_2 m_LoginResult2;
	tstring m_strUserFolder;
	CBuddyInfo m_UserInfo;
	CBuddyList m_BuddyList;
	CGroupList m_GroupList;
	CRecentList m_RecentList;
	CMessageList m_MsgList;
	CMessageLogger m_MsgLogger;
};

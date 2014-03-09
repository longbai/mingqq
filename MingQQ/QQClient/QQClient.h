#pragma once

#include "QQUser.h"
#include "QQProtocol.h"
#include "LoginTask.h"
#include "LogoutTask.h"
#include "SendMsgTask.h"
#include "RecvMsgTask.h"
#include "PollTask.h"
#include "InfoTask.h"
#include "GetGroupFaceSigTask.h"
#include "ChangeStatusTask.h"

class CQQClient
{
public:
	CQQClient(void);
	~CQQClient(void);

public:
	BOOL Init();										// ��ʼ���ͻ���
	void UnInit();										// ����ʼ���ͻ���

	void SetUser(LPCTSTR lpQQNum, LPCTSTR lpQQPwd);		// ����QQ���������
	void SetLoginStatus(QQ_STATUS nStatus);				// ���õ�¼״̬
	void SetCallBackWnd(HWND hCallBackWnd);				// ���ûص����ھ��
	void SetVerifyCode(LPCTSTR lpVerifyCode);			// ������֤��

	BOOL Login();										// ��¼
	BOOL Logout();										// ע��

	void CancelLogin();									// ȡ����¼
	
	void ChangeStatus(QQ_STATUS nStatus);				// �ı�����״̬
	void UpdateBuddyList();								// ���º����б�
	void UpdateGroupList();								// ����Ⱥ�б�
	void UpdateRecentList();							// ���������ϵ���б�
	void UpdateBuddyInfo(UINT nQQUin);					// ���º�����Ϣ
	void UpdateGroupMemberInfo(UINT nGroupCode, UINT nQQUin);	// ����Ⱥ��Ա��Ϣ
	void UpdateGroupInfo(UINT nGroupCode);				// ����Ⱥ��Ϣ
	void UpdateBuddyNum(UINT nQQUin);					// ���º��Ѻ���
	void UpdateGroupMemberNum(UINT nGroupCode, UINT nQQUin);	// ����Ⱥ��Ա����
	void UpdateGroupMemberNum(UINT nGroupCode, std::vector<UINT> * arrQQUin);	// ����Ⱥ��Ա����
	void UpdateGroupNum(UINT nGroupCode);				// ����Ⱥ����
	void UpdateBuddySign(UINT nQQUin);					// ���º��Ѹ���ǩ��
	void UpdateGroupMemberSign(UINT nGroupCode, UINT nQQUin);	// ����Ⱥ��Ա����ǩ��
	void ModifyQQSign(LPCTSTR lpSign);					// �޸�QQ����ǩ��
	void UpdateBuddyHeadPic(UINT nQQUin, UINT nQQNum);			// ���º���ͷ��
	void UpdateGroupMemberHeadPic(UINT nGroupCode, UINT nQQUin, UINT nQQNum);// ����Ⱥ��Աͷ��
	void UpdateGroupHeadPic(UINT nGroupCode, UINT nGroupNum);	// ����Ⱥͷ��
	void UpdateGroupFaceSignal();						// ����Ⱥ��������
	BOOL SendBuddyMsg(UINT nToUin, time_t nTime, LPCTSTR lpMsg);				// ���ͺ�����Ϣ
	BOOL SendGroupMsg(UINT nGroupId, time_t nTime, LPCTSTR lpMsg);				// ����Ⱥ��Ϣ
	BOOL SendSessMsg(UINT nGroupId, UINT nToUin, time_t nTime, LPCTSTR lpMsg);	// ������ʱ�Ự��Ϣ

	BOOL IsOffline();									// �Ƿ�����״̬
	QQ_STATUS GetStatus();								// ��ȡ����״̬
	BOOL GetVerifyCodePic(const BYTE *& lpData, DWORD& dwSize);	// ��ȡ��֤��ͼƬ

	CBuddyInfo * GetUserInfo();							// ��ȡ�û���Ϣ
	CBuddyList * GetBuddyList();						// ��ȡ�����б�
	CGroupList * GetGroupList();						// ��ȡȺ�б�
	CRecentList * GetRecentList();						// ��ȡ�����ϵ���б�
	CMessageList * GetMessageList();					// ��ȡ��Ϣ�б�
	CMessageLogger * GetMsgLogger();					// ��ȡ��Ϣ��¼������

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

private:
	void OnUpdateBuddyList(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateGroupList(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateRecentList(UINT message, WPARAM wParam, LPARAM lParam);
	void OnBuddyMsg(UINT message, WPARAM wParam, LPARAM lParam);
	void OnGroupMsg(UINT message, WPARAM wParam, LPARAM lParam);
	void OnSessMsg(UINT message, WPARAM wParam, LPARAM lParam);
	void OnSysGroupMsg(UINT message, WPARAM wParam, LPARAM lParam);
	void OnStatusChangeMsg(UINT message, WPARAM wParam, LPARAM lParam);
	void OnKickMsg(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateBuddyNumber(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateGMemberNumber(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateGroupNumber(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateBuddySign(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateGMemberSign(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateBuddyInfo(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateGMemberInfo(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateGroupInfo(UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdateC2CMsgSig(UINT message, WPARAM wParam, LPARAM lParam);
	void OnChangeStatusResult(UINT message, WPARAM wParam, LPARAM lParam);

	void OnInternal_GetBuddyData(UINT message, WPARAM wParam, LPARAM lParam);
	void OnInternal_GetGroupData(UINT message, WPARAM wParam, LPARAM lParam);
	void OnInternal_GetGMemberData(UINT message, WPARAM wParam, LPARAM lParam);
	UINT OnInternal_GroupId2Code(UINT message, WPARAM wParam, LPARAM lParam);

	BOOL CreateProxyWnd();		// ����������
	BOOL DestroyProxyWnd();		// ���ٴ�����
	static LRESULT CALLBACK ProxyWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
	CQQUser m_QQUser;
	CQQProtocol m_QQProtocol;
	CThreadPool m_ThreadPool;
	CLoginTask m_LoginTask;
	CLogoutTask m_LogoutTask;
	CSendMsgTask m_SendMsgTask;
	CRecvMsgTask m_RecvMsgTask;
	CPollTask m_PollTask;
};

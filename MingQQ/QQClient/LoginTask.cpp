#include "StdAfx.h"
#include "LoginTask.h"

CLoginTask::CLoginTask(void)
{
	m_lpQQUser = NULL;
	m_lpQQProtocol = NULL;
	m_lpThreadPool = NULL;
	m_lpSendMsgTask = NULL;
	m_lpRecvMsgTask = NULL;
	m_lpPollTask = NULL;
	m_bStop = FALSE;
}

CLoginTask::~CLoginTask(void)
{
}

int CLoginTask::Run()
{
	CBuddyInfoResult * lpBuddyInfoResult = NULL;
	CGetSignResult * lpGetSignResult = NULL;
	CBuddyListResult * lpBuddyListResult = NULL;
	CGroupListResult * lpGroupListResult = NULL;
	CRecentListResult * lpRecentListResult = NULL;
	int nLoginResultCode = QLRC_FAILED;
	BOOL bRet;

	if (NULL == m_lpQQUser || NULL == m_lpQQProtocol 
		|| NULL == m_lpThreadPool || NULL == m_lpPollTask 
		|| NULL == m_lpSendMsgTask)
		goto Ret0;

	if (m_lpQQUser->m_strVerifyCode.empty())
	{
		bRet = m_lpQQProtocol->CheckVerifyCode(m_HttpClient,		// ����Ƿ���Ҫ������֤��
			m_lpQQUser->m_strQQNum.c_str(), WEBQQ_APP_ID, &m_lpQQUser->m_VerifyCodeInfo);
		if (!bRet || m_bStop)
			goto Ret0;

		if (m_lpQQUser->m_VerifyCodeInfo.m_nNeedVerify)				// ��Ҫ��֤��
		{
			bRet = m_lpQQProtocol->GetVerifyCodePic(m_HttpClient,	// ��ȡ��֤��ͼƬ
				WEBQQ_APP_ID, m_lpQQUser->m_strQQNum.c_str(), 
				m_lpQQUser->m_VerifyCodeInfo.m_strVCType.c_str(), 
				&m_lpQQUser->m_VerifyCodePic);
			if (!bRet || m_bStop)
				goto Ret0;
			
			nLoginResultCode = QLRC_NEED_VERIFY_CODE;
			goto Ret0;
		}
		else														// ����Ҫ��֤��
		{
			m_lpQQUser->m_strVerifyCode = m_lpQQUser->m_VerifyCodeInfo.m_strVerifyCode;
		}
	}

	bRet = m_lpQQProtocol->Login1(m_HttpClient, m_lpQQUser->m_strQQNum.c_str(),		// ��һ�ε�¼
		m_lpQQUser->m_strQQPwd.c_str(), m_lpQQUser->m_strVerifyCode.c_str(), 
		m_lpQQUser->m_VerifyCodeInfo.m_cPtUin, m_lpQQUser->m_VerifyCodeInfo.m_nPtUinLen,
		WEBQQ_APP_ID, &m_lpQQUser->m_LoginResult1);
	if (!bRet || m_bStop)
		goto Ret0;

	if (m_lpQQUser->m_LoginResult1.m_nRetCode != 0)					// ��¼ʧ��
	{
		if (m_lpQQUser->m_LoginResult1.m_nRetCode == 4)				// ��֤�����
		{
			bRet = m_lpQQProtocol->GetVerifyCodePic(m_HttpClient,	// ��ȡ��֤��ͼƬ
				WEBQQ_APP_ID, m_lpQQUser->m_strQQNum.c_str(), 
				m_lpQQUser->m_VerifyCodeInfo.m_strVCType.c_str(), 
				&m_lpQQUser->m_VerifyCodePic);
			if (!bRet || m_bStop)
				goto Ret0;
			
			nLoginResultCode = QLRC_VERIFY_CODE_ERROR;
			goto Ret0;
		}
		else if (m_lpQQUser->m_LoginResult1.m_nRetCode == 3)		// �������
		{
			nLoginResultCode = QLRC_PASSWORD_ERROR;
			goto Ret0;
		}
		else														// δ֪����
		{
			goto Ret0;
		}
	}

	bRet = m_lpQQProtocol->Login2(m_HttpClient, m_lpQQUser->m_nLoginStatus,	// �ڶ��ε�¼
		m_lpQQUser->m_LoginResult1.m_strPtWebQq.c_str(), WEBQQ_CLIENT_ID, &m_lpQQUser->m_LoginResult2);
	if (!bRet || m_lpQQUser->m_LoginResult2.m_nRetCode != 0 || m_bStop)
		goto Ret0;

	lpBuddyInfoResult = GetUserInfo();		// ��ȡ�û���Ϣ
	if (m_bStop)
		goto Ret0;

	lpGetSignResult = GetUserSign();		// ��ȡ�û�ǩ��
	if (m_bStop)
		goto Ret0;

	lpBuddyListResult = GetBuddyList();		// ��ȡ�����б�
	if (m_bStop)
		goto Ret0;

	lpGroupListResult = GetGroupList();		// ��ȡȺ�б�
	if (m_bStop)
		goto Ret0;

	lpRecentListResult = GetRecentList();	// ��ȡ�����ϵ���б�
	if (m_bStop)
		goto Ret0;

	bRet = StartPollTask();					// ������ѭ��Ϣ����
	bRet = StartGetBuddyNumberTask(lpBuddyListResult, lpGroupListResult);	// ������ȡ���Ѻ�������
	bRet = StartGetBuddySignTask(lpBuddyListResult);	// ������ȡ���Ѹ���ǩ������
	bRet = StartGetHeadPicTask(lpBuddyListResult, lpGroupListResult);		// ������ȡ����ͷ������

	nLoginResultCode = QLRC_SUCCESS;

Ret0:
	m_lpQQUser->m_strVerifyCode = _T("");

	if (QLRC_SUCCESS == nLoginResultCode)	// ��½�ɹ�
	{
		m_lpQQUser->m_nStatus = m_lpQQUser->m_LoginResult2.m_nStatus;

		::PostMessage(m_lpQQUser->m_hProxyWnd, 
			QQ_MSG_LOGIN_RESULT, NULL, nLoginResultCode);
		::PostMessage(m_lpQQUser->m_hProxyWnd, 
			QQ_MSG_UPDATE_BUDDY_INFO, NULL, (LPARAM)lpBuddyInfoResult);
		::PostMessage(m_lpQQUser->m_hProxyWnd, 
			QQ_MSG_UPDATE_BUDDY_SIGN, NULL, (LPARAM)lpGetSignResult);
		::PostMessage(m_lpQQUser->m_hProxyWnd, 
			QQ_MSG_UPDATE_BUDDY_LIST, NULL, (LPARAM)lpBuddyListResult);
		::PostMessage(m_lpQQUser->m_hProxyWnd, 
			QQ_MSG_UPDATE_GROUP_LIST, NULL, (LPARAM)lpGroupListResult);
		::PostMessage(m_lpQQUser->m_hProxyWnd, 
			QQ_MSG_UPDATE_RECENT_LIST, NULL, (LPARAM)lpRecentListResult);

		bRet = StartSendMsgTask();				// ����������Ϣ����
		bRet = StartRecvMsgTask();				// ����������Ϣ����
	}
	else									// ��½ʧ��
	{
		delete lpBuddyInfoResult;
		delete lpGetSignResult;
		delete lpBuddyListResult;
		delete lpGroupListResult;
		delete lpRecentListResult;

		if (QLRC_FAILED == nLoginResultCode)
		{
			if (m_bStop)
				nLoginResultCode = QLRC_USER_CANCEL_LOGIN;
		}
		::PostMessage(m_lpQQUser->m_hProxyWnd, QQ_MSG_LOGIN_RESULT, NULL, nLoginResultCode);
	}
	
	m_bStop = FALSE;
	return 0;
}

int CLoginTask::Stop()
{
	m_bStop = TRUE;
	m_HttpClient.SetCancalEvent();
	return 0;
}

void CLoginTask::TaskFinish()
{

}

// ��ȡ�û���Ϣ
CBuddyInfoResult * CLoginTask::GetUserInfo()
{
	CBuddyInfoResult * lpBuddyInfoResult = new CBuddyInfoResult;
	if (NULL == lpBuddyInfoResult)
		return NULL;

	BOOL bRet = m_lpQQProtocol->GetBuddyInfo(m_HttpClient, m_lpQQUser->m_nQQUin, 
		m_lpQQUser->m_LoginResult2.m_strVfWebQq.c_str(), lpBuddyInfoResult);
	if (!bRet || lpBuddyInfoResult->m_nRetCode != 0 || m_bStop)
	{
		delete lpBuddyInfoResult;
		return NULL;
	}

	return lpBuddyInfoResult;
}

// ��ȡ�û�ǩ��
CGetSignResult * CLoginTask::GetUserSign()
{
	CGetSignResult * lpGetSignResult = new CGetSignResult;
	if (NULL == lpGetSignResult)
		return NULL;

	BOOL bRet = m_lpQQProtocol->GetQQSign(m_HttpClient, m_lpQQUser->m_nQQUin, 
		m_lpQQUser->m_LoginResult2.m_strVfWebQq.c_str(), lpGetSignResult);
	if (!bRet || lpGetSignResult->m_nRetCode != 0 || m_bStop)
	{
		delete lpGetSignResult;
		return NULL;
	}

	return lpGetSignResult;
}

// ��ȡ�����б�
CBuddyListResult * CLoginTask::GetBuddyList()
{
	CBuddyListResult * lpBuddyListResult = new CBuddyListResult;
	if (NULL == lpBuddyListResult)
		return NULL;

	BOOL bRet = m_lpQQProtocol->GetBuddyList(m_HttpClient, 
		m_lpQQUser->m_nQQUin, m_lpQQUser->m_LoginResult1.m_strPtWebQq.c_str(), 
		m_lpQQUser->m_LoginResult2.m_strVfWebQq.c_str(), lpBuddyListResult);
	if (!bRet || lpBuddyListResult->m_nRetCode != 0 || m_bStop)
	{
		delete lpBuddyListResult;
		return NULL;
	}

	COnlineBuddyListResult OnlineBuddyListResult;
	bRet = m_lpQQProtocol->GetOnlineBuddyList(m_HttpClient, WEBQQ_CLIENT_ID, 
		m_lpQQUser->m_LoginResult2.m_strPSessionId.c_str(), &OnlineBuddyListResult);
	if (!bRet || OnlineBuddyListResult.m_nRetCode != 0 || m_bStop)
	{
		delete lpBuddyListResult;
		OnlineBuddyListResult.Reset();
		return NULL;
	}

	lpBuddyListResult->SetOnlineBuddyList(&OnlineBuddyListResult);
	lpBuddyListResult->SortBuddy();
	OnlineBuddyListResult.Reset();

	return lpBuddyListResult;
}

// ��ȡȺ�б�
CGroupListResult * CLoginTask::GetGroupList()
{
	CGroupListResult * lpGroupListResult = new CGroupListResult;
	if (NULL == lpGroupListResult)
		return NULL;

	BOOL bRet = m_lpQQProtocol->GetGroupList(m_HttpClient, 
		m_lpQQUser->m_LoginResult2.m_strVfWebQq.c_str(), lpGroupListResult);
	if (!bRet || lpGroupListResult->m_nRetCode != 0 || m_bStop)
	{
		delete lpGroupListResult;
		return NULL;
	}

	return lpGroupListResult;
}

// ��ȡ�����ϵ���б�
CRecentListResult * CLoginTask::GetRecentList()
{
	CRecentListResult * lpRecentListResult;
	BOOL bRet;

	lpRecentListResult = new CRecentListResult;
	if (NULL == lpRecentListResult)
		return NULL;

	bRet = m_lpQQProtocol->GetRecentList(m_HttpClient, 
		m_lpQQUser->m_LoginResult2.m_strVfWebQq.c_str(), WEBQQ_CLIENT_ID,
		m_lpQQUser->m_LoginResult2.m_strPSessionId.c_str(), lpRecentListResult);
	if (!bRet || lpRecentListResult->m_nRetCode != 0 || m_bStop)
	{
		delete lpRecentListResult;
		return NULL;
	}

	return lpRecentListResult;
}

// ����������Ϣ����
BOOL CLoginTask::StartSendMsgTask()
{
	m_lpSendMsgTask->m_lpQQUser = m_lpQQUser;
	m_lpSendMsgTask->m_lpQQProtocol = m_lpQQProtocol;
	return m_lpThreadPool->AddTask(m_lpSendMsgTask);
}

// ����������Ϣ����
BOOL CLoginTask::StartRecvMsgTask()
{
	m_lpRecvMsgTask->m_lpQQUser = m_lpQQUser;
	m_lpRecvMsgTask->m_lpQQProtocol = m_lpQQProtocol;
	m_lpRecvMsgTask->m_lpThreadPool = m_lpThreadPool;
	return m_lpThreadPool->AddTask(m_lpRecvMsgTask);
}

// ������ѯ����
BOOL CLoginTask::StartPollTask()
{
	m_lpPollTask->m_lpQQUser = m_lpQQUser;
	m_lpPollTask->m_lpQQProtocol = m_lpQQProtocol;
	m_lpPollTask->m_lpRecvMsgTask = m_lpRecvMsgTask;
	return m_lpThreadPool->AddTask(m_lpPollTask);
}

// ������ȡ����QQ��������
BOOL CLoginTask::StartGetBuddyNumberTask(CBuddyListResult * lpBuddyListResult, 
										 CGroupListResult * lpGroupListResult)
{
	CGetQQNumResult GetQQNumResult;

	if (NULL == lpBuddyListResult || NULL == lpGroupListResult)
		return FALSE;

	for (int i = 0; i < lpBuddyListResult->GetBuddyTeamCount(); i++)
	{
		CBuddyTeamInfo * lpBuddyTeamInfo = lpBuddyListResult->GetBuddyTeam(i);
		if (lpBuddyTeamInfo != NULL)
		{
			for (int j = 0; j < (int)lpBuddyTeamInfo->GetBuddyCount(); j++)
			{
				CBuddyInfo * lpBuddyInfo = lpBuddyTeamInfo->GetBuddy(j);
				if (lpBuddyInfo != NULL)
				{
					BOOL bRet = m_lpQQProtocol->GetQQNum(m_HttpClient, TRUE, lpBuddyInfo->m_nQQUin, 
						m_lpQQUser->m_LoginResult2.m_strVfWebQq.c_str(), &GetQQNumResult);
					if (bRet)
						lpBuddyInfo->SetQQNum(&GetQQNumResult);

					if (m_bStop)
						return FALSE;
				}
			}
		}	
	}

	for (int i = 0; i < lpGroupListResult->GetGroupCount(); i++)
	{
		CGroupInfo * lpGroupInfo = lpGroupListResult->GetGroup(i);
		if (lpGroupInfo != NULL)
		{
			BOOL bRet = m_lpQQProtocol->GetQQNum(m_HttpClient, FALSE, lpGroupInfo->m_nGroupCode, 
				m_lpQQUser->m_LoginResult2.m_strVfWebQq.c_str(), &GetQQNumResult);
			if (bRet)
				lpGroupInfo->SetGroupNumber(&GetQQNumResult);

			if (m_bStop)
				return FALSE;
		}
	}

	return TRUE;
}

// ������ȡ����QQǩ������
BOOL CLoginTask::StartGetBuddySignTask(CBuddyListResult * lpBuddyListResult)
{
	if (NULL == lpBuddyListResult)
		return FALSE;

	CQQSignTask * lpQQSignTask = new CQQSignTask;
	if (NULL == lpQQSignTask)
		return FALSE;

	lpQQSignTask->m_lpQQUser = m_lpQQUser;
	lpQQSignTask->m_lpQQProtocol = m_lpQQProtocol;
	lpQQSignTask->m_nType = QQSIGN_OP_TYPE_GET;

	for (int i = 0; i < lpBuddyListResult->GetBuddyTeamCount(); i++)
	{
		CBuddyTeamInfo * lpBuddyTeamInfo = lpBuddyListResult->GetBuddyTeam(i);
		if (lpBuddyTeamInfo != NULL)
		{
			for (int j = 0; j < (int)lpBuddyTeamInfo->GetBuddyCount(); j++)
			{
				CBuddyInfo * lpBuddyInfo = lpBuddyTeamInfo->GetBuddy(j);
				if (lpBuddyInfo != NULL)
					lpQQSignTask->AddGetParam(FALSE, 0, lpBuddyInfo->m_nQQUin);
			}
		}	
	}

	return m_lpThreadPool->AddTask(lpQQSignTask);
}

// ������ȡͷ��ͼƬ����
BOOL CLoginTask::StartGetHeadPicTask(CBuddyListResult * lpBuddyListResult, 
									 CGroupListResult * lpGroupListResult)
{
	if (NULL == lpBuddyListResult || NULL == lpGroupListResult)
		return FALSE;

	CGetHeadPicTask * lpGetHeadPicTask = new CGetHeadPicTask;
	if (NULL == lpGetHeadPicTask)
		return FALSE;

	lpGetHeadPicTask->m_lpQQUser = m_lpQQUser;
	lpGetHeadPicTask->m_lpQQProtocol = m_lpQQProtocol;

	if (m_lpQQUser->IsNeedUpdateBuddyHeadPic(m_lpQQUser->m_nQQUin))
		lpGetHeadPicTask->AddParam(GHPT_OP_TYPE_BUDDY_PIC, 0, 0, m_lpQQUser->m_nQQUin, m_lpQQUser->m_nQQUin);

	for (int i = 0; i < lpBuddyListResult->GetBuddyTeamCount(); i++)
	{
		CBuddyTeamInfo * lpBuddyTeamInfo = lpBuddyListResult->GetBuddyTeam(i);
		if (lpBuddyTeamInfo != NULL)
		{
			for (int j = 0; j < (int)lpBuddyTeamInfo->GetBuddyCount(); j++)
			{
				CBuddyInfo * lpBuddyInfo = lpBuddyTeamInfo->GetBuddy(j);
				if (lpBuddyInfo != NULL)
				{
					if (m_lpQQUser->IsNeedUpdateBuddyHeadPic(lpBuddyInfo->m_nQQNum))
						lpGetHeadPicTask->AddParam(GHPT_OP_TYPE_BUDDY_PIC, 0, 0, lpBuddyInfo->m_nQQUin, lpBuddyInfo->m_nQQNum);
				}
			}
		}	
	}

	for (int i = 0; i < lpGroupListResult->GetGroupCount(); i++)
	{
		CGroupInfo * lpGroupInfo = lpGroupListResult->GetGroup(i);
		if (lpGroupInfo != NULL)
		{
			if (m_lpQQUser->IsNeedUpdateGroupHeadPic(lpGroupInfo->m_nGroupNumber))
				lpGetHeadPicTask->AddParam(GHPT_OP_TYPE_GROUP_PIC, lpGroupInfo->m_nGroupCode, lpGroupInfo->m_nGroupNumber, 0, 0);
		}
	}

	return m_lpThreadPool->AddTask(lpGetHeadPicTask);
}

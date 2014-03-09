#include "StdAfx.h"
#include "QQUser.h"

CQQUser::CQQUser(void)
{
	m_hProxyWnd = NULL;
	m_hCallBackWnd = NULL;
	m_strQQNum = _T("");
	m_strQQPwd = _T("");
	m_nQQUin = 0;
	m_nLoginStatus = QQ_STATUS_ONLINE;
	m_nStatus = QQ_STATUS_OFFLINE;
	m_strVerifyCode = _T("");
	m_strUserFolder = ZYM::CPath::GetAppPath() + _T("Users\\");
}

CQQUser::~CQQUser(void)
{
}

// ��ȡ�û��ļ��д��·��
tstring CQQUser::GetUserFolder()
{
	return m_strUserFolder;
}

// �����û��ļ��д��·��
void CQQUser::SetUserFolder(LPCTSTR lpPath)
{
	if (lpPath != NULL)
		m_strUserFolder = lpPath;
}

// ��ȡ�����ļ��д��·��
tstring CQQUser::GetPersonalFolder(UINT nUserNum/* = 0*/)
{
	if (0 == nUserNum)
		nUserNum = m_nQQUin;

	TCHAR szPath[MAX_PATH] = {0};
	wsprintf(szPath, _T("%s%u\\"), m_strUserFolder.c_str(), nUserNum);
	return szPath;
}

// ��ȡ����ͼƬ���·��
tstring CQQUser::GetChatPicFolder(UINT nUserNum/* = 0*/)
{
	return GetPersonalFolder(nUserNum)+_T("ChatImage\\");
}

// ��ȡ�û�ͷ��ͼƬȫ·���ļ���
tstring CQQUser::GetUserHeadPicFullName(UINT nUserNum/* = 0*/)
{
	TCHAR szPath[MAX_PATH] = {0};
	LPCTSTR lpFmt = _T("%s%u\\HeadImage\\Buddy\\%u.png");

	if (0 == nUserNum)
		nUserNum = m_nQQUin;

	wsprintf(szPath, lpFmt, m_strUserFolder.c_str(), nUserNum, nUserNum);
	return szPath;
}

// ��ȡ����ͷ��ͼƬȫ·���ļ���
tstring CQQUser::GetBuddyHeadPicFullName(UINT nQQNum)
{
	TCHAR szPath[MAX_PATH] = {0};
	LPCTSTR lpFmt = _T("%s%u\\HeadImage\\Buddy\\%u.png");
	wsprintf(szPath, lpFmt, m_strUserFolder.c_str(), m_nQQUin, nQQNum);
	return szPath;
}

// ��ȡȺͷ��ͼƬȫ·���ļ���
tstring CQQUser::GetGroupHeadPicFullName(UINT nGroupNum)
{
	TCHAR szPath[MAX_PATH] = {0};
	LPCTSTR lpFmt = _T("%s%u\\HeadImage\\Group\\%u.png");
	wsprintf(szPath, lpFmt, m_strUserFolder.c_str(), m_nQQUin, nGroupNum);
	return szPath;
}

// ��ȡȺ��Աͷ��ͼƬȫ·���ļ���
tstring CQQUser::GetSessHeadPicFullName(UINT nQQNum)
{
	TCHAR szPath[MAX_PATH] = {0};
	LPCTSTR lpFmt = _T("%s%u\\HeadImage\\Sess\\%u.png");
	wsprintf(szPath, lpFmt, m_strUserFolder.c_str(), m_nQQUin, nQQNum);
	return szPath;
}

// ��ȡ����ͼƬȫ·���ļ���
tstring CQQUser::GetChatPicFullName(LPCTSTR lpszFileName)
{
	if (NULL == lpszFileName || NULL == *lpszFileName)
		return _T("");
	return GetChatPicFolder(m_nQQUin)+lpszFileName;
}

// ��ȡ��Ϣ��¼ȫ·���ļ���
tstring CQQUser::GetMsgLogFullName(UINT nUserNum/* = 0*/)
{
	return GetPersonalFolder(nUserNum)+_T("Msg.db");
}

// �ж��Ƿ���Ҫ���º���ͷ��
BOOL CQQUser::IsNeedUpdateBuddyHeadPic(UINT nQQNum)
{
	tstring strFullName = GetBuddyHeadPicFullName(nQQNum);

	if (!ZYM::CPath::IsFileExist(strFullName.c_str()))
		return TRUE;

	SYSTEMTIME stLastWriteTime = {0};
	GetFileLastWriteTime(strFullName.c_str(), &stLastWriteTime);

	return ((!IsToday(&stLastWriteTime)) ? TRUE : FALSE);
}

// �ж��Ƿ���Ҫ����Ⱥͷ��
BOOL CQQUser::IsNeedUpdateGroupHeadPic(UINT nGroupNum)
{
	tstring strFullName = GetGroupHeadPicFullName(nGroupNum);

	if (!ZYM::CPath::IsFileExist(strFullName.c_str()))
		return TRUE;

	SYSTEMTIME stLastWriteTime = {0};
	GetFileLastWriteTime(strFullName.c_str(), &stLastWriteTime);

	return ((!IsToday(&stLastWriteTime)) ? TRUE : FALSE);
}

// �ж��Ƿ���Ҫ����Ⱥ��Աͷ��
BOOL CQQUser::IsNeedUpdateSessHeadPic(UINT nQQNum)
{
	tstring strFullName = GetSessHeadPicFullName(nQQNum);

	if (!ZYM::CPath::IsFileExist(strFullName.c_str()))
		return TRUE;

	SYSTEMTIME stLastWriteTime = {0};
	GetFileLastWriteTime(strFullName.c_str(), &stLastWriteTime);

	return ((!IsToday(&stLastWriteTime)) ? TRUE : FALSE);
}

// ��ȡ������ʱ��
time_t CQQUser::GetServerTime()
{
	time_t nInterval = (::GetTickCount()-m_LoginResult2.m_dwTickCount)/1000;
	return m_LoginResult2.m_nServerTime+nInterval;
}
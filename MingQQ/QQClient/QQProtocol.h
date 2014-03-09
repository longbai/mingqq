#pragma once

#include <vector>
#include <time.h>
#include "HttpClient.h"
#include "Buffer.h"
#include "MD5.h"
#include "QQProtocolData.h"

class CQQProtocol
{
public:
	CQQProtocol(void);
	~CQQProtocol(void);

public:
	BOOL CheckVerifyCode(CHttpClient& HttpClient, LPCTSTR lpQQNum,	// ����Ƿ���Ҫ������֤��
		LPCTSTR lpAppId, CVerifyCodeInfo * lpVCInfo);
	BOOL GetVerifyCodePic(CHttpClient& HttpClient, LPCTSTR lpAppId,	// ��ȡ��֤��ͼƬ
		LPCTSTR lpQQNum, LPCTSTR lpVCType, CBuffer * lpVerifyCodePic);
	BOOL Login1(CHttpClient& HttpClient, LPCTSTR lpQQNum, LPCTSTR lpQQPwd,	// ��һ�ε�¼
		LPCTSTR lpVerifyCode, const CHAR * lpPtUin, int nPtUin, 
		LPCTSTR lpAppId, CLoginResult_1 * lpLoginResult1);
	BOOL Login2(CHttpClient& HttpClient, QQ_STATUS nQQStatus,		// �ڶ��ε�¼
		LPCTSTR lpPtWebQq, LPCTSTR lpClientId, CLoginResult_2 * lpLoginResult2);
	BOOL Logout(CHttpClient& HttpClient, LPCTSTR lpClientId,		// ע��
		LPCTSTR lpPSessionId, CLogoutResult * lpLogoutResult);
	BOOL GetBuddyList(CHttpClient& HttpClient, UINT nQQUin,			// ��ȡ�����б�
		LPCTSTR lpPtWebQq, LPCTSTR lpVfWebQq, CBuddyListResult * lpResult);
	BOOL GetOnlineBuddyList(CHttpClient& HttpClient, LPCTSTR lpClientId,	// ��ȡ���ߺ����б�
		LPCTSTR lpPSessionId, COnlineBuddyListResult * lpResult);
	BOOL GetGroupList(CHttpClient& HttpClient,						// ��ȡȺ�б�
		LPCTSTR lpVfWebQq, CGroupListResult * lpResult);
	BOOL GetRecentList(CHttpClient& HttpClient, LPCTSTR lpVfWebQq,	// ��ȡ�����ϵ���б�
		LPCTSTR lpClientId, LPCTSTR lpPSessionId, CRecentListResult * lpResult);
	BOOL Poll(CHttpClient& HttpClient, LPCTSTR lpClientId,			// ��ѯ��Ϣ
		LPCTSTR lpPSessionId, CBuffer * lpResult);
	BOOL GetBuddyInfo(CHttpClient& HttpClient, UINT nQQUin,			// ��ȡ������Ϣ
		LPCTSTR lpVfWebQq, CBuddyInfoResult * lpBuddyInfoResult);
	BOOL GetStrangerInfo(CHttpClient& HttpClient, UINT nQQUin,		// ��ȡİ������Ϣ
		LPCTSTR lpVfWebQq, CBuddyInfoResult * lpBuddyInfoResult);
	BOOL GetGroupInfo(CHttpClient& HttpClient, UINT nGroupCode,		// ��ȡȺ��Ϣ
		LPCTSTR lpVfWebQq, CGroupInfoResult * lpGroupInfoResult);
	BOOL GetQQNum(CHttpClient& HttpClient, BOOL bIsBuddy, UINT nQQUin,	// ��ȡ���ѡ�Ⱥ��Ա��Ⱥ����
		LPCTSTR lpVfWebQq, CGetQQNumResult * lpGetQQNumResult);
	BOOL GetQQSign(CHttpClient& HttpClient, UINT nQQUin,			// ��ȡQQ����ǩ��
		LPCTSTR lpVfWebQq, CGetSignResult * lpGetSignResult);
	BOOL SetQQSign(CHttpClient& HttpClient, LPCTSTR lpSign,			// ����QQ����ǩ��
		LPCTSTR lpVfWebQq, CSetSignResult * lpSetSignResult);
	BOOL SendBuddyMsg(CHttpClient& HttpClient, CBuddyMessage * lpBuddyMsg,	// ���ͺ�����Ϣ
		LPCTSTR lpClientId, LPCTSTR lpPSessionId, CSendBuddyMsgResult * lpSendBuddyMsgResult);
	BOOL SendGroupMsg(CHttpClient& HttpClient, CGroupMessage * lpGroupMsg,	// ����Ⱥ��Ϣ
		LPCTSTR lpClientId, LPCTSTR lpPSessionId, LPCTSTR lpGFaceKey, LPCTSTR lpGFaceSig, CSendGroupMsgResult * lpSendGroupMsgResult);
	BOOL SendSessMsg(CHttpClient& HttpClient, CSessMessage * lpSessMsg,		// ������ʱ�Ự��Ϣ
		LPCTSTR lpGroupSig, LPCTSTR lpClientId, LPCTSTR lpPSessionId, 
		CSendSessMsgResult * lpSendSessMsgResult);
	BOOL GetHeadPic(CHttpClient& HttpClient, BOOL bIsBuddy,			// ��ȡͷ��ͼƬ
		UINT nQQUin, LPCTSTR lpVfWebQq, CBuffer * lpFacePic);
	BOOL GetBuddyChatPic(CHttpClient& HttpClient, UINT nMsgId,		// ��ȡ��������ͼƬ
		LPCTSTR lpFileName, UINT nQQUin, LPCTSTR lpClientId,
		LPCTSTR lpPSessionId, CBuffer * lpBuddyPic);
	BOOL GetBuddyOffChatPic(CHttpClient& HttpClient,				// ��ȡ������������ͼƬ
		LPCTSTR lpFileName, UINT nQQUin, LPCTSTR lpClientId,
		LPCTSTR lpPSessionId, CBuffer * lpBuddyPic);
	BOOL GetGroupChatPic(CHttpClient& HttpClient, UINT nGroupId,	// ��ȡȺ����ͼƬ
		UINT nQQUin, LPCTSTR lpServer, int nPort, UINT nFileId, 
		LPCTSTR lpFileName, LPCTSTR lpVfWebQq, CBuffer * lpGroupPic);
	BOOL ChangeStatus(CHttpClient& HttpClient, QQ_STATUS nStatus,	// �ı�״̬
		LPCTSTR lpClientId, LPCTSTR lpPSessionId, CChangeStatusResult * lpChangeStatusResult);
	BOOL GetC2CMsgSignal(CHttpClient& HttpClient, UINT nGroupId,	// ��ȡ��ʱ�Ự����
		UINT nToUin, LPCTSTR lpClientId, LPCTSTR lpPSessionId, 
		CGetC2CMsgSigResult * lpGetC2CMsgSigResult);
	BOOL GetGroupFaceSignal(CHttpClient& HttpClient, LPCTSTR lpClientId,	// ��ȡȺ��������
		LPCTSTR lpPSessionId, CGetGroupFaceSigResult * lpGetGroupFaceSigResult);
	BOOL UploadCustomFace(CHttpClient& HttpClient, LPCTSTR lpFileName,
		LPCTSTR lpVfWebQq, CUploadCustomFaceResult * lpResult);		// �ϴ��Զ������
	
private:
	BOOL HttpReq(CHttpClient& HttpClient, LPCTSTR lpszUrl, 
		LPCTSTR lpszReqHeaders, const CHAR * lpPostData, DWORD dwPostDataLen, 
		DWORD& dwRespCode, std::vector<tstring>* arrRespHeader, CBuffer& RespData);
	BOOL HttpGet(CHttpClient& HttpClient, LPCTSTR lpszUrl, 
		LPCTSTR lpszReqHeaders, DWORD& dwRespCode, 
		std::vector<tstring>* arrRespHeader, CBuffer& RespData);
	BOOL HttpPost(CHttpClient& HttpClient, LPCTSTR lpszUrl, 
		LPCTSTR lpszReqHeaders, const CHAR * lpPostData, DWORD dwPostDataLen, 
		DWORD& dwRespCode, std::vector<tstring>* arrRespHeader, CBuffer& RespData);
	BOOL HttpBigPost(CHttpClient& HttpClient, LPCTSTR lpszUrl, 
		LPCTSTR lpszReqHeaders, const CHAR * lpPostData, DWORD dwPostDataLen, 
		DWORD& dwRespCode, std::vector<tstring>* arrRespHeader, CBuffer& RespData);

	std::string EncodeData(const CHAR * lpData, int nLen);
	std::string EncodeData(const WCHAR * lpData, int nLen);
	std::wstring UnicodeToHexStr(const WCHAR * lpStr, BOOL bDblSlash);

	BOOL CalcPwdHash(LPCTSTR lpQQPwd, LPCTSTR lpVerifyCode,						// �����һ�ε�¼������hash����
		const CHAR * lpPtUin, int nPtUinLen, TCHAR * lpPwdHash, int nLen);
	std::string CalcBuddyListHash(UINT nQQUin, const std::string &strPtWebQq);	// �����ȡ�����б��hash����
};
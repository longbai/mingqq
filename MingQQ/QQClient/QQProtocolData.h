#pragma once

#include <string>
#include <vector>
#include "../jsoncpp-0.5.0/json.h"		// ʹ��JsonCpp��Ҫ������ͷ�ļ�
#include "../Utils.h"
#include "../Path.h"
#include "Buffer.h"

#if defined(UNICODE) || defined(_UNICODE)
	typedef std::wstring tstring;
#else
	typedef std::string tstring;
#endif

extern const TCHAR * WEBQQ_APP_ID;
extern const TCHAR * WEBQQ_CLIENT_ID;

#define		QQ_MSG_LOGIN_RESULT				WM_USER + 100		// ��¼������Ϣ
#define		QQ_MSG_LOGOUT_RESULT			WM_USER + 101		// ע��������Ϣ
#define		QQ_MSG_UPDATE_USER_INFO			WM_USER + 102		// �����û���Ϣ
#define		QQ_MSG_UPDATE_BUDDY_LIST		WM_USER + 103		// ���º����б���Ϣ
#define		QQ_MSG_UPDATE_GROUP_LIST		WM_USER + 104		// ����Ⱥ�б���Ϣ
#define		QQ_MSG_UPDATE_RECENT_LIST		WM_USER + 105		// ���������ϵ���б���Ϣ
#define		QQ_MSG_BUDDY_MSG				WM_USER + 106		// ������Ϣ
#define		QQ_MSG_GROUP_MSG				WM_USER + 107		// Ⱥ��Ϣ
#define		QQ_MSG_SESS_MSG					WM_USER + 108		// ��ʱ�Ự��Ϣ
#define		QQ_MSG_STATUS_CHANGE_MSG		WM_USER + 109		// ����״̬�ı���Ϣ
#define		QQ_MSG_KICK_MSG					WM_USER + 110		// ����������Ϣ
#define		QQ_MSG_SYS_GROUP_MSG			WM_USER + 111		// Ⱥϵͳ��Ϣ
#define		QQ_MSG_UPDATE_BUDDY_NUMBER		WM_USER + 112		// ���º��Ѻ���
#define		QQ_MSG_UPDATE_GMEMBER_NUMBER	WM_USER + 113		// ����Ⱥ��Ա����
#define		QQ_MSG_UPDATE_GROUP_NUMBER		WM_USER + 114		// ����Ⱥ����
#define		QQ_MSG_UPDATE_BUDDY_SIGN		WM_USER + 115		// ���º��Ѹ���ǩ��
#define		QQ_MSG_UPDATE_GMEMBER_SIGN		WM_USER + 116		// ����Ⱥ��Ա����ǩ��
#define		QQ_MSG_UPDATE_BUDDY_INFO		WM_USER + 117		// ���º�����Ϣ
#define		QQ_MSG_UPDATE_GMEMBER_INFO		WM_USER + 118		// ����Ⱥ��Ա��Ϣ
#define		QQ_MSG_UPDATE_GROUP_INFO		WM_USER + 119		// ����Ⱥ��Ϣ
#define		QQ_MSG_UPDATE_C2CMSGSIG			WM_USER + 120		// ������ʱ�Ự����
#define		QQ_MSG_UPDATE_GROUPFACESIG		WM_USER + 121		// ����Ⱥ��������
#define		QQ_MSG_UPDATE_BUDDY_HEADPIC		WM_USER + 122		// ���º���ͷ��
#define		QQ_MSG_UPDATE_GMEMBER_HEADPIC	WM_USER + 123		// ����Ⱥ��Աͷ��
#define		QQ_MSG_UPDATE_GROUP_HEADPIC		WM_USER + 124		// ����Ⱥͷ��
#define		QQ_MSG_CHANGE_STATUS_RESULT		WM_USER + 125		// �ı�����״̬������Ϣ

#define		QQ_MSG_INTERNAL_GETBUDDYDATA	WM_USER + 400
#define		QQ_MSG_INTERNAL_GETGROUPDATA	WM_USER + 401
#define		QQ_MSG_INTERNAL_GETGMEMBERDATA	WM_USER + 402
#define		QQ_MSG_INTERNAL_GROUPID2CODE	WM_USER + 403

#define		QQ_MSG_FIRST					WM_USER + 100
#define		QQ_MSG_LAST						WM_USER + 500

// QQ����״̬�ַ���
#define		QQ_STATUS_STR_ONLINE			_T("online")		// ��������
#define		QQ_STATUS_STR_CALLME			_T("callme")		// Q�Ұ�
#define		QQ_STATUS_STR_AWAY				_T("away")			// �뿪
#define		QQ_STATUS_STR_BUSY				_T("busy")			// æµ
#define		QQ_STATUS_STR_SILENT			_T("silent")		// ��Ҵ���
#define		QQ_STATUS_STR_HIDDEN			_T("hidden")		// ����
#define		QQ_STATUS_STR_OFFLINE			_T("offline")		// ����

// QQ��Ϣ�����ַ���
#define		QQ_MSG_TYPE_STR_BUDDY			_T("message")		// ������Ϣ����
#define		QQ_MSG_TYPE_STR_GROUP			_T("group_message")	// Ⱥ��Ϣ����
#define		QQ_MSG_TYPE_STR_SESS			_T("sess_message")	// ��ʱ�Ự��Ϣ����
#define		QQ_MSG_TYPE_STR_STATUSCHANGE	_T("buddies_status_change")	// ����״̬�ı���Ϣ����
#define		QQ_MSG_TYPE_STR_KICK			_T("kick_message")	// ����������Ϣ����
#define		QQ_MSG_TYPE_STR_SYSGROUP		_T("sys_g_msg")		// Ⱥϵͳ��Ϣ����

enum QQ_LOGIN_RESULT_CODE	// ��¼���ش���
{
	QLRC_SUCCESS,			// ��¼�ɹ�
	QLRC_FAILED,			// ��¼ʧ��
	QLRC_PASSWORD_ERROR,	// �������
	QLRC_NEED_VERIFY_CODE,	// ��Ҫ������֤��
	QLRC_VERIFY_CODE_ERROR,	// ��֤�����
	QLRC_USER_CANCEL_LOGIN	// �û�ȡ����¼
};

enum QQ_STATUS				// QQ����״̬
{
	QQ_STATUS_ONLINE = 10,	// ��������
	QQ_STATUS_CALLME = 60,	// Q�Ұ�
	QQ_STATUS_AWAY = 30,	// �뿪
	QQ_STATUS_BUSY = 50,	// æµ
	QQ_STATUS_SILENT = 70,	// ��Ҵ���
	QQ_STATUS_HIDDEN = 80,	// ����
	QQ_STATUS_OFFLINE = 90	// ����
};

enum QQ_CLIENT_TYPE			// QQ�ͻ�������
{
	QQ_CLIENT_TYPE_PC = 1,
	QQ_CLIENT_TYPE_MOBILE = 21,
	QQ_CLIENT_TYPE_IPHONE = 24,
	QQ_CLIENT_TYPE_WEBQQ = 41,
	QQ_CLIENT_TYPE_PAD = 42
};

enum CONTENT_TYPE
{
	CONTENT_TYPE_UNKNOWN,
	CONTENT_TYPE_FONT_INFO,
	CONTENT_TYPE_TEXT,
	CONTENT_TYPE_FACE,
	CONTENT_TYPE_CUSTOM_FACE,
	CONTENT_TYPE_OFF_PIC
};

enum QQ_MSG_TYPE
{
	QQ_MSG_TYPE_UNKNOWN,
	QQ_MSG_TYPE_BUDDY,
	QQ_MSG_TYPE_GROUP,
	QQ_MSG_TYPE_SESS,
	QQ_MSG_TYPE_STATUSCHANGE,
	QQ_MSG_TYPE_KICK,
	QQ_MSG_TYPE_SYSGROUP
};

QQ_STATUS ConvertToQQStatus(LPCTSTR lpStatus);
LPCTSTR ConvertToQQStatusStr(QQ_STATUS nStatus);

tstring GetRespHeader(std::vector<tstring>* arrRespHeader,	// ��ȡHTTPͷָ���ֶ����Ƶ�ֵ
					  LPCTSTR lpszName, int nIndex = 0);

class CBuddyTeamInfo;
class CBuddyInfo;
class COnlineBuddyInfo;
class CGroupInfo;
class CRecentInfo;

class CVerifyCodeInfo
{
public:
	CVerifyCodeInfo(void);
	~CVerifyCodeInfo(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

private:
	BOOL ParsePtUin(const TCHAR * lpInPtUin, CHAR * lpOutPtUin, int& nOutPtUinLen);

public:
	int m_nNeedVerify;
	tstring m_strVerifyCode;
	tstring m_strVCType;
	CHAR m_cPtUin[1024];
	int m_nPtUinLen;
};

class CLoginResult_1
{
public:
	CLoginResult_1(void);
	~CLoginResult_1(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf, std::vector<tstring>* arrRespHeader);

public:
	int m_nRetCode;
	tstring m_strCheckSigUrl, m_strMsg, m_strNickName;
	tstring m_strPtWebQq;
	tstring m_strSKey;
};

class CLoginResult_2
{
public:
	CLoginResult_2(void);
	~CLoginResult_2(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf, std::vector<tstring>* arrRespHeader);

public:
	int m_nRetCode;
	UINT m_nUIN;
	UINT m_nCIP;
	int m_nIndex;
	int m_nPort;
	QQ_STATUS m_nStatus;
	tstring m_strVfWebQq;
	tstring m_strPSessionId;
	int m_nUser_State;
	int m_f;
	time_t m_nServerTime;
	DWORD m_dwTickCount;
};

class CLogoutResult
{
public:
	CLogoutResult(void);
	~CLogoutResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

public:
	int m_nRetCode;
	tstring m_strResult;
};

class COnlineBuddyListResult
{
public:
	COnlineBuddyListResult(void);
	~COnlineBuddyListResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

public:
	int m_nRetCode;
	std::vector<COnlineBuddyInfo *> m_arrOnlineBuddyInfo;
};

class CBuddyListResult
{
public:
	CBuddyListResult(void);
	~CBuddyListResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);
	int GetBuddyTeamCount();
	CBuddyTeamInfo * GetBuddyTeam(int nTeamIndex);
	CBuddyTeamInfo * GetBuddyTeamByInnerIndex(int nInnerTeamIndex);
	int GetBuddyCount(int nTeamIndex);
	int GetOnlineBuddyCount(int nTeamIndex);
	CBuddyInfo * GetBuddy(int nTeamIndex, int nIndex);
	CBuddyInfo * GetBuddy(UINT nQQUin);
	BOOL SetOnlineBuddyList(COnlineBuddyListResult * lpOnlineBuddyListResult);
	void SortBuddyTeam();
	void SortBuddy();

public:
	int m_nRetCode;
	std::vector<CBuddyTeamInfo *> m_arrBuddyTeamInfo;
};

class CGroupListResult
{
public:
	CGroupListResult(void);
	~CGroupListResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);
	int GetGroupCount();				// ��ȡȺ����
	CGroupInfo * GetGroup(int nIndex);	// ����������ȡȺ��Ϣ

public:
	int m_nRetCode;
	std::vector<CGroupInfo *> m_arrGroupInfo;
};

class CRecentListResult
{
public:
	CRecentListResult(void);
	~CRecentListResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

public:
	int m_nRetCode;
	std::vector<CRecentInfo *> m_arrRecentInfo;
};

class CGetQQNumResult
{
public:
	CGetQQNumResult(void);
	~CGetQQNumResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

public:
	int m_nRetCode;
	UINT m_nQQUin;			// �ڲ�QQ����
	UINT m_nQQNum;			// QQ����
};

class CGetSignResult
{
public:
	CGetSignResult(void);
	~CGetSignResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

public:
	int m_nRetCode;
	UINT m_nQQUin;			// �ڲ�QQ����
	tstring m_strSign;		// ����ǩ��
};

class CSetSignResult
{
public:
	CSetSignResult(void);
	~CSetSignResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

public:
	int m_nRetCode;
	int m_nResult;
};

class CBuddyInfoResult
{
public:
	CBuddyInfoResult(void);
	~CBuddyInfoResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

public:
	int m_nRetCode;
	UINT m_nQQUin;			// �ڲ�QQ����
	tstring m_strNickName;	// �ǳ�
	int m_nFace;			// ͷ��
	tstring m_strGender;	// �Ա�
	int m_nShengXiao;		// ��Ф
	int m_nConstel;			// ����
	int m_nBlood;			// Ѫ��
	tstring m_strBirthday;	// ����
	tstring m_strCountry;	// ����
	tstring m_strProvince;	// ʡ��
	tstring m_strCity;		// ����
	tstring m_strPhone;		// �绰
	tstring m_strMobile;	// �ֻ�
	tstring m_strEmail;		// ����
	tstring m_strOccupation;// ְҵ
	tstring m_strCollege;	// ��ҵԺУ
	tstring m_strHomepage;	// ������ҳ
	tstring m_strPersonal;	// ����˵��
	BOOL m_bAllow;
	UINT m_nRegTime;
	int m_nStat;
	int m_nVipInfo;
	int m_nClientType;		// �ͻ������ͣ�1-QQ�ͻ��� 41-WebQQ
	tstring m_strToken;
};

class CGroupInfoResult
{
public:
	CGroupInfoResult(void);
	~CGroupInfoResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);
	CBuddyInfo * GetMemberByUin(UINT nQQUin);

public:
	int m_nRetCode;
	UINT m_nGroupCode;		// Ⱥ����
	UINT m_nGroupId;		// ȺID
	tstring m_strName;		// Ⱥ����
	tstring m_strMemo;		// Ⱥ����
	tstring m_strFingerMemo;// Ⱥ���
	UINT m_nOwnerUin;		// Ⱥӵ����Uin
	UINT m_nCreateTime;		// Ⱥ����ʱ��
	int m_nFace;			// Ⱥͷ��
	int m_nLevel;			// Ⱥ�ȼ�
	int m_nClass;			// Ⱥ��������
	UINT m_nOption;
	UINT m_nFlag;
	std::vector<CBuddyInfo *> m_arrMember;	// Ⱥ��Ա
};

class CSendBuddyMsgResult
{
public:
	CSendBuddyMsgResult(void);
	~CSendBuddyMsgResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

public:
	int m_nRetCode;
	tstring m_strResult;
};

class CSendGroupMsgResult
{
public:
	CSendGroupMsgResult(void);
	~CSendGroupMsgResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

public:
	int m_nRetCode;
	tstring m_strResult;
};

class CSendSessMsgResult
{
public:
	CSendSessMsgResult(void);
	~CSendSessMsgResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

public:
	int m_nRetCode;
	tstring m_strResult;
};

class CChangeStatusResult
{
public:
	CChangeStatusResult(void);
	~CChangeStatusResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

public:
	int m_nRetCode;
	tstring m_strResult;
};

class CGetC2CMsgSigResult
{
public:
	CGetC2CMsgSigResult(void);
	~CGetC2CMsgSigResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

public:
	int m_nRetCode;
	int m_nType;
	tstring m_strValue;
	UINT m_nFlags;
	UINT m_nGroupId;
	UINT m_nQQUin;
};

class CGetGroupFaceSigResult
{
public:
	CGetGroupFaceSigResult(void);
	~CGetGroupFaceSigResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

public:
	int m_nRetCode;
	int m_nReply;
	tstring m_strGFaceKey;
	tstring m_strGFaceSig;
};

class CUploadCustomFaceResult
{
public:
	CUploadCustomFaceResult(void);
	~CUploadCustomFaceResult(void);

public:
	void Reset();
	BOOL Parse(CBuffer * lpBuf);

public:
	int m_nRetCode;
	tstring m_strRemoteFileName;
};

class CBuddyInfo			// ������Ϣ
{
public:
	CBuddyInfo(void);
	~CBuddyInfo(void);

public:
	void Reset();
	void SetQQNum(CGetQQNumResult * lpGetQQNumResult);
	void SetQQSign(CGetSignResult * lpGetSignResult);
	void SetBuddyInfo(CBuddyInfoResult * lpBuddyInfoResult);
	BOOL IsHasQQNum();
	BOOL IsHasQQSign();
	BOOL IsHasBuddyInfo();
	tstring GetDisplayGender();
	tstring GetDisplayShengXiao();
	tstring GetDisplayConstel();
	tstring GetDisplayBlood();

public:
	UINT m_nQQUin;			// �ڲ�QQ����
	UINT m_nQQNum;			// QQ����
	int m_nTeamIndex;		// ��������
	tstring m_strNickName;	// �ǳ�
	tstring m_strMarkName;	// ��ע
	BOOL m_bIsVip;			// �Ƿ�VIP��־
	int m_nVipLevel;		// VIP�ȼ�
	int m_nFace;			// ͷ��
	DWORD m_dwFlag;			// 
	QQ_STATUS m_nStatus;	// ����״̬
	int m_nClientType;		// �ͻ������ͣ�1-QQ�ͻ��� 41-WebQQ
	tstring m_strGroupCard;	// Ⱥ��Ƭ
	UINT m_nGroupFlag;
	tstring m_strSign;		// QQ����ǩ��
	tstring m_strGender;	// �Ա�
	int m_nShengXiao;		// ��Ф
	int m_nConstel;			// ����
	int m_nBlood;			// Ѫ��
	tstring m_strBirthday;	// ����
	tstring m_strCountry;	// ����
	tstring m_strProvince;	// ʡ��
	tstring m_strCity;		// ����
	tstring m_strPhone;		// �绰
	tstring m_strMobile;	// �ֻ�
	tstring m_strEmail;		// ����
	tstring m_strOccupation;// ְҵ
	tstring m_strCollege;	// ��ҵԺУ
	tstring m_strHomepage;	// ������ҳ
	tstring m_strPersonal;	// ����˵��
	BOOL m_bAllow;
	UINT m_nRegTime;
	int m_nStat;
	int m_nVipInfo;
	tstring m_strToken;
	tstring m_strGroupSig;	// Ⱥ�ڻỰ����
	BOOL m_bHasQQNum;
	BOOL m_bHasQQSign;
	BOOL m_bHasBuddyInfo;
};

class CBuddyTeamInfo		// ���ѷ�����Ϣ
{
public:
	CBuddyTeamInfo(void);
	~CBuddyTeamInfo(void);

public:
	void Reset();
	int GetBuddyCount();
	int GetOnlineBuddyCount();
	CBuddyInfo * GetBuddy(int nIndex);
	void Sort();

public:
	int m_nIndex;			// ����
	int m_nSort;			// ����˳��
	tstring m_strName;		// ��������
	std::vector<CBuddyInfo *> m_arrBuddyInfo;
};

class COnlineBuddyInfo		// ���ߺ�����Ϣ
{
public:
	COnlineBuddyInfo(void);
	~COnlineBuddyInfo(void);

public:
	void Reset();

public:
	UINT m_nUin;
	QQ_STATUS m_nStatus;	// ����״̬
	int m_nClientType;		// �ͻ�������
};

class CBuddyList	// �����б�
{
public:
	CBuddyList(void);
	~CBuddyList(void);

public:
	void Reset();
	int GetBuddyTeamCount();
	CBuddyTeamInfo * GetBuddyTeam(int nTeamIndex);
	CBuddyTeamInfo * GetBuddyTeamByIndex(int nIndex);
	int GetBuddyCount(int nTeamIndex);
	int GetOnlineBuddyCount(int nTeamIndex);
	CBuddyInfo * GetBuddy(int nTeamIndex, int nIndex);
	CBuddyInfo * GetBuddy(UINT nQQUin);
	BOOL SetOnlineBuddyList(COnlineBuddyListResult * lpOnlineBuddyListResult);
	void SortBuddyTeam();
	void SortBuddy();
	BOOL AddBuddyTeam(CBuddyTeamInfo * lpBuddyTeamInfo);

public:
	int m_nRetCode;
	std::vector<CBuddyTeamInfo *> m_arrBuddyTeamInfo;
};

class CGroupInfo			// Ⱥ��Ϣ
{
public:
	CGroupInfo(void);
	~CGroupInfo(void);

public:
	void Reset();
	int GetMemberCount();						// ��ȡȺ��Ա������
	int GetOnlineMemberCount();					// ��ȡȺ��Ա��������
	CBuddyInfo * GetMember(int nIndex);			// ����������ȡȺ��Ա��Ϣ
	CBuddyInfo * GetMemberByUin(UINT nQQUin);	// ����QQUin��ȡȺ��Ա��Ϣ
	CBuddyInfo * GetMemberByNum(UINT nQQNum);	// ����QQNum��ȡȺ��Ա��Ϣ
	void Sort();								// ��Ⱥ��Ա�б�����״̬��������
	BOOL AddMember(CBuddyInfo * lpBuddyInfo);	// ���Ⱥ��Ա
	BOOL DelAllMember();						// ɾ������Ⱥ��Ա
	void SetGroupNumber(CGetQQNumResult * lpGetQQNumResult);	// ����Ⱥ����
	BOOL SetGroupInfo(CGroupInfoResult * lpGroupInfoResult);	// ����Ⱥ��Ϣ
	BOOL IsHasGroupNumber();
	BOOL IsHasGroupInfo();

public:
	UINT m_nGroupCode;		// Ⱥ����
	UINT m_nGroupId;		// ȺID
	UINT m_nGroupNumber;	// Ⱥ����
	tstring m_strName;		// Ⱥ����
	tstring m_strMemo;		// Ⱥ����
	tstring m_strFingerMemo;// Ⱥ���
	UINT m_nOwnerUin;		// Ⱥӵ����Uin
	UINT m_nCreateTime;		// Ⱥ����ʱ��
	int m_nFace;			// Ⱥͷ��
	int m_nLevel;			// Ⱥ�ȼ�
	int m_nClass;			// Ⱥ��������
	UINT m_nOption;
	UINT m_nFlag;
	BOOL m_bHasGroupNumber;
	BOOL m_bHasGroupInfo;
	std::vector<CBuddyInfo *> m_arrMember;	// Ⱥ��Ա
};

class CGroupList	// Ⱥ�б�
{
public:
	CGroupList(void);
	~CGroupList(void);

public:
	void Reset();
	int GetGroupCount();							// ��ȡȺ����
	CGroupInfo * GetGroup(int nIndex);				// ��ȡȺ��Ϣ(��������)
	CGroupInfo * GetGroupByCode(UINT nGroupCode);	// ��ȡȺ��Ϣ(����Ⱥ����)
	CGroupInfo * GetGroupById(UINT nGroupId);		// ��ȡȺ��Ϣ(����ȺId)
	CBuddyInfo * GetGroupMemberByCode(UINT nGroupCode, UINT nQQUin);// ����Ⱥ�����Ⱥ��ԱQQUin��ȡȺ��Ա��Ϣ
	CBuddyInfo * GetGroupMemberById(UINT nGroupId, UINT nQQUin);	// ����ȺId��Ⱥ��ԱQQUin��ȡȺ��Ա��Ϣ
	BOOL AddGroup(CGroupInfo * lpGroupInfo);		// ���Ⱥ
	UINT GetGroupCodeById(UINT nGroupId);			// ��ȺId��ȡȺ����
	UINT GetGroupIdByCode(UINT nGroupCode);			// ��Ⱥ�����ȡȺId

public:
	std::vector<CGroupInfo *> m_arrGroupInfo;
};

class CRecentInfo	// �����ϵ����Ϣ
{
public:
	CRecentInfo(void);
	~CRecentInfo(void);

public:
	void Reset();

public:
	UINT m_nQQUin;
	int m_nType;
};

class CRecentList			// �����ϵ���б�
{
public:
	CRecentList(void);
	~CRecentList(void);

public:
	void Reset();
	BOOL AddRecent(CRecentInfo * lpRecentInfo);
	int GetRecentCount();
	CRecentInfo * GetRecent(int nIndex);

public:
	int m_nRetCode;
	std::vector<CRecentInfo *> m_arrRecentInfo;
};

class CFontInfo				// ������Ϣ
{
public:
	CFontInfo(void) : m_nSize(9),m_clrText(RGB(0,0,0)),m_strName(_T("����")),
		m_bBold(FALSE),m_bItalic(FALSE),m_bUnderLine(FALSE){}
	~CFontInfo(void){}

public:
	int m_nSize;			// �����С
	COLORREF m_clrText;		// ������ɫ
	tstring m_strName;		// ��������
	BOOL m_bBold;			// �Ƿ�Ӵ�
	BOOL m_bItalic;			// �Ƿ���б
	BOOL m_bUnderLine;		// �Ƿ���»���
};

class CCustomFaceInfo		// �Զ��������Ϣ
{
public:
	CCustomFaceInfo(void) : m_nFileId(0){}
	~CCustomFaceInfo(void){}

public:
	tstring m_strName;				// �����Զ������ʹ�ò���
	UINT m_nFileId;
	tstring m_strKey;
	tstring m_strServer;

	tstring m_strRemoteFileName;	// �����Զ������ʹ�ò���
};

class CContent				// ��Ϣ����
{
public:
	CContent(void) : m_nType(CONTENT_TYPE_UNKNOWN),m_nFaceId(0){}
	~CContent(void){}

public:
	CONTENT_TYPE m_nType;			// ��������
	CFontInfo m_FontInfo;			// ������Ϣ
	tstring m_strText;				// �ı���Ϣ
	int m_nFaceId;					// ϵͳ����Id
	CCustomFaceInfo m_CFaceInfo;	// �Զ��������Ϣ
};

class CBuddyMessage		// ������Ϣ
{
public:
	CBuddyMessage(void);
	~CBuddyMessage(void);

public:
	static BOOL IsType(LPCTSTR lpType);
	void Reset();
	BOOL Parse(Json::Value& JsonValue);

public:
	UINT m_nMsgId;
	UINT m_nMsgId2;
	UINT m_nFromUin;
	UINT m_nToUin;
	UINT m_nMsgType;
	UINT m_nReplyIp;
	UINT m_nTime;
	std::vector<CContent *> m_arrContent;
};

class CGroupMessage		// Ⱥ��Ϣ
{
public:
	CGroupMessage(void);
	~CGroupMessage(void);

public:
	static BOOL IsType(LPCTSTR lpType);
	void Reset();
	BOOL Parse(Json::Value& JsonValue);

public:
	UINT m_nMsgId;
	UINT m_nMsgId2;
	UINT m_nFromUin;
	UINT m_nToUin;
	UINT m_nMsgType;
	UINT m_nReplyIp;
	UINT m_nGroupCode;
	UINT m_nSendUin;
	UINT m_nSeq;
	UINT m_nInfoSeq;
	UINT m_nTime;
	std::vector<CContent *> m_arrContent;
};

class CSessMessage		// ��ʱ��Ϣ
{
public:
	CSessMessage(void);
	~CSessMessage(void);

public:
	static BOOL IsType(LPCTSTR lpType);
	void Reset();
	BOOL Parse(Json::Value& JsonValue);

public:
	UINT m_nMsgId;
	UINT m_nMsgId2;
	UINT m_nFromUin;
	UINT m_nToUin;
	UINT m_nMsgType;
	UINT m_nReplyIp;
	UINT m_nTime;
	UINT m_nGroupId;
	UINT m_nQQNum;
	UINT m_nServiceType;
	UINT m_nFlags;
	std::vector<CContent *> m_arrContent;
};

class CStatusChangeMessage	// ״̬�ı�֪ͨ��Ϣ
{
public:
	CStatusChangeMessage(void);
	~CStatusChangeMessage(void);

public:
	static BOOL IsType(LPCTSTR lpType);
	void Reset();
	BOOL Parse(Json::Value& JsonValue);

public:
	UINT m_nQQUin;
	QQ_STATUS m_nStatus;	// ����״̬
	int m_nClientType;		// �ͻ�������
};

class CKickMessage	// ��������֪ͨ��Ϣ(�����������һ�ص��¼)
{
public:
	CKickMessage(void);
	~CKickMessage(void);

public:
	static BOOL IsType(LPCTSTR lpType);
	void Reset();
	BOOL Parse(Json::Value& JsonValue);

public:
	UINT m_nMsgId;
	UINT m_nMsgId2;
	UINT m_nFromUin;
	UINT m_nToUin;
	UINT m_nMsgType;
	UINT m_nReplyIp;
	BOOL m_bShowReason;			// �Ƿ���ʾ��������ԭ��
	tstring m_strReason;		// ��������ԭ��
};

class CSysGroupMessage	// Ⱥϵͳ��Ϣ
{
public:
	CSysGroupMessage(void);
	~CSysGroupMessage(void);

public:
	static BOOL IsType(LPCTSTR lpType);
	void Reset();
	BOOL Parse(Json::Value& JsonValue);

public:
	UINT m_nMsgId;
	UINT m_nMsgId2;
	UINT m_nFromUin;
	UINT m_nToUin;
	UINT m_nMsgType;
	UINT m_nReplyIp;
	tstring m_strSubType;
	UINT m_nGroupCode;
	UINT m_nGroupNumber;
	UINT m_nAdminUin;
	tstring m_strMsg;
	UINT m_nOpType;
	UINT m_nOldMember;
	tstring m_strOldMember;
	tstring m_strAdminUin;
	tstring m_strAdminNickName;
};

class CMessageSender	// ��Ϣ������
{
public:
	CMessageSender(QQ_MSG_TYPE nType, UINT nSenderId, UINT nGroupCode);
	~CMessageSender(void);

public:
	BOOL AddMsg(void * lpMsg);						// �����Ϣ
	BOOL DelMsg(int nIndex);						// ɾ����Ϣ(��������)
	BOOL DelMsgById(UINT nMsgId);					// ɾ����Ϣ(������ϢId)
	BOOL DelAllMsg();								// ɾ��������Ϣ

	QQ_MSG_TYPE GetMsgType();						// ��ȡ��Ϣ����
	UINT GetSenderId();								// ��ȡ������Id
	int GetMsgCount();								// ��ȡ��Ϣ����
	UINT GetGroupCode();							// ��ȡȺ����

	CBuddyMessage * GetBuddyMsg(int nIndex);		// ��ȡ������Ϣ(��������)
	CGroupMessage * GetGroupMsg(int nIndex);		// ��ȡȺ��Ϣ(��������)
	CSessMessage * GetSessMsg(int nIndex);			// ��ȡ��ʱ�Ự��Ϣ(��������)
	CSysGroupMessage * GetSysGroupMsg(int nIndex);	// ��ȡȺϵͳ��Ϣ(��������)

	CBuddyMessage * GetBuddyMsgById(UINT nMsgId);	// ��ȡ������Ϣ(������ϢId)
	CGroupMessage * GetGroupMsgById(UINT nMsgId);	// ��ȡȺ��Ϣ(������ϢId)
	CSessMessage * GetSessMsgById(UINT nMsgId);		// ��ȡ��ʱ�Ự��Ϣ(������ϢId)
	CSysGroupMessage * GetSysGroupMsgById(UINT nMsgId);// ��ȡȺϵͳ��Ϣ(������ϢId)

private:
	QQ_MSG_TYPE m_nType;
	UINT m_nSenderId;
	UINT m_nGroupCode;
	std::vector<void *> m_arrMsg;
};

class CMessageList	// ��Ϣ�б�
{
public:
	CMessageList(void);
	~CMessageList(void);

public:
	BOOL AddMsg(QQ_MSG_TYPE nType, UINT nSenderId,			// �����Ϣ
		UINT nGroupCode, void * lpMsg);
	BOOL DelMsg(QQ_MSG_TYPE nType, UINT nMsgId);			// ɾ����Ϣ

	BOOL DelMsgSender(int nIndex);							// ɾ����Ϣ������(��������)
	BOOL DelMsgSender(QQ_MSG_TYPE nType, UINT nSenderId);	// ɾ����Ϣ������(������Ϣ���ͺͷ�����Id)
	BOOL DelAllMsgSender();									// ɾ��������Ϣ������

	int GetMsgSenderCount();								// ��ȡ��Ϣ����������
	CMessageSender * GetMsgSender(int nIndex);				// ��ȡ��Ϣ������(��������)
	CMessageSender * GetMsgSender(QQ_MSG_TYPE nType, UINT nSenderId);// ��ȡ��Ϣ������(������Ϣ���ͺͷ�����Id)
	CMessageSender * GetLastMsgSender();					// ��ȡ���һ��������Ϣ�����ķ�����

	CBuddyMessage * GetBuddyMsg(UINT nMsgId);				// ��ȡ������Ϣ(������ϢId)
	CGroupMessage * GetGroupMsg(UINT nMsgId);				// ��ȡȺ��Ϣ(������ϢId)
	CSessMessage * GetSessMsg(UINT nMsgId);					// ��ȡ��ʱ�Ự��Ϣ(������ϢId)
	CSysGroupMessage * GetSysGroupMsg(UINT nMsgId);			// ��ȡȺϵͳ��Ϣ(������ϢId)

	CBuddyMessage * GetBuddyMsg(UINT nQQUin, UINT nMsgId);	// ��ȡ������Ϣ(���ݷ�����Id����ϢId)
	CGroupMessage * GetGroupMsg(UINT nGroupCode, UINT nMsgId);	// ��ȡȺ��Ϣ(���ݷ�����Id����ϢId)
	CSessMessage * GetSessMsg(UINT nQQUin, UINT nMsgId);	// ��ȡ��ʱ�Ự��Ϣ(���ݷ�����Id����ϢId)
	CSysGroupMessage * GetSysGroupMsg(UINT nGroupCode, UINT nMsgId);// ��ȡȺϵͳ��Ϣ(���ݷ�����Id����ϢId)

	struct MSG_IDENTITY
	{
		QQ_MSG_TYPE nType;
		UINT nSenderId;
	};

private:
	void AddMsgIdentity(QQ_MSG_TYPE nType, UINT nSenderId);
	void DelMsgIdentity(QQ_MSG_TYPE nType, UINT nSenderId);

private:
	std::vector<CMessageSender *> m_arrMsgSender;
	std::vector<MSG_IDENTITY> m_arrMsgIdentity;
};

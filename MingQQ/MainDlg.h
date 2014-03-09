// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "CustomMsgDef.h"
#include "SkinLib/SkinLib.h"
#include "QQClient/QQClient.h"
#include "TrayIcon.h"
#include "FaceList.h"
#include "LoginAccountList.h"
#include "CascadeWinManager.h"
#include "BuddyListCtrl.h"
#include "LoginDlg.h"
#include "VerifyCodeDlg.h"
#include "MsgTipDlg.h"
#include "BuddyChatDlg.h"
#include "GroupChatDlg.h"
#include "SessChatDlg.h"
#include "BuddyInfoDlg.h"
#include "GroupInfoDlg.h"

class CGMemberInfoMapKey
{
public:
	CGMemberInfoMapKey(void):m_nGroupCode(0), m_nQQUin(0){}
	~CGMemberInfoMapKey(void){}

public:
	bool operator < (const CGMemberInfoMapKey& rhs) const
	{
		if(m_nGroupCode < rhs.m_nGroupCode) return true;
		if(m_nGroupCode > rhs.m_nGroupCode) return false;
		return  m_nQQUin < rhs.m_nQQUin;
	}

public:
	UINT m_nGroupCode;
	UINT m_nQQUin;
};

class CMainDlg : public CDialogImpl<CMainDlg>, public CUpdateUI<CMainDlg>,
		public CMessageFilter, public CIdleHandler
{
public:
	CMainDlg(void);
	~CMainDlg(void);

	enum { IDD = IDD_MAINDLG };

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP_EX(CMainDlg)
		MSG_WM_INITDIALOG(OnInitDialog)
		MSG_WM_SYSCOMMAND(OnSysCommand)
		MSG_WM_MEASUREITEM(OnMeasureItem)
		MSG_WM_DRAWITEM(OnDrawItem)
		MSG_WM_GETMINMAXINFO(OnGetMinMaxInfo)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_SIZE(OnSize)
		MSG_WM_HOTKEY(OnHotKey)
		MSG_WM_CLOSE(OnClose)
		MSG_WM_DESTROY(OnDestroy)
		COMMAND_ID_HANDLER_EX(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER_EX(ID_BTN_SIGN, OnBtn_Sign)
		COMMAND_ID_HANDLER_EX(ID_MENU_SHOW_MAIN_PANEL, OnMenu_ShowMainPanel)
		COMMAND_ID_HANDLER_EX(ID_MENU_EXIT, OnMenu_Exit)
		COMMAND_ID_HANDLER_EX(ID_BTN_SIGN, OnBtn_Sign)
		COMMAND_HANDLER_EX(ID_BTN_CANCEL, BN_CLICKED, OnBtn_Cancel)
		NOTIFY_HANDLER_EX(ID_TABCTRL_MAIN, TCN_DROPDOWN, OnTabCtrlDropDown)
		NOTIFY_HANDLER_EX(ID_TABCTRL_MAIN, TCN_SELCHANGE, OnTabCtrlSelChange)
		NOTIFY_HANDLER_EX(ID_LISTCTRL_BUDDY, NM_DBLCLK, OnBuddyListDblClk)
		NOTIFY_HANDLER_EX(ID_LISTCTRL_GROUP, NM_DBLCLK, OnGroupListDblClk)
		NOTIFY_HANDLER_EX(ID_LISTCTRL_RECENT, NM_DBLCLK, OnRecentListDblClk)
		COMMAND_HANDLER_EX(ID_PIC_HEAD, BN_CLICKED, OnPic_Clicked_Head)
		COMMAND_ID_HANDLER_EX(ID_MENU_BIGHEADPIC, OnMenu_BigHeadPic)
		COMMAND_ID_HANDLER_EX(ID_MENU_SMALLHEADPIC, OnMenu_SmallHeadPic)
		COMMAND_ID_HANDLER_EX(ID_MENU_STDHEADPIC, OnMenu_StdHeadPic)
		COMMAND_ID_HANDLER_EX(ID_MENU_SHOWBIGHEADPICINSEL, OnMenu_ShowBigHeadPicInSel)
		COMMAND_HANDLER_EX(ID_EDIT_SIGN, EN_KILLFOCUS, OnEdit_Sign_KillFocus)
		COMMAND_RANGE_HANDLER_EX(ID_MENU_IMONLINE, ID_MENU_IMOFFLINE, OnMenu_Status)

		MESSAGE_HANDLER_EX(QQ_MSG_LOGIN_RESULT, OnLoginResult)
		MESSAGE_HANDLER_EX(QQ_MSG_LOGOUT_RESULT, OnLogoutResult)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_USER_INFO, OnUpdateUserInfo)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_BUDDY_LIST, OnUpdateBuddyList)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GROUP_LIST, OnUpdateGroupList)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_RECENT_LIST, OnUpdateRecentList)
		MESSAGE_HANDLER_EX(QQ_MSG_BUDDY_MSG, OnBuddyMsg)
		MESSAGE_HANDLER_EX(QQ_MSG_GROUP_MSG, OnGroupMsg)
		MESSAGE_HANDLER_EX(QQ_MSG_SESS_MSG, OnSessMsg)
		MESSAGE_HANDLER_EX(QQ_MSG_STATUS_CHANGE_MSG, OnStatusChangeMsg)
		MESSAGE_HANDLER_EX(QQ_MSG_KICK_MSG, OnKickMsg)
		MESSAGE_HANDLER_EX(QQ_MSG_SYS_GROUP_MSG, OnSysGroupMsg)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_BUDDY_NUMBER, OnUpdateBuddyNumber)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GMEMBER_NUMBER, OnUpdateGMemberNumber)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GROUP_NUMBER, OnUpdateGroupNumber)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_BUDDY_SIGN, OnUpdateBuddySign)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GMEMBER_SIGN, OnUpdateGMemberSign)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_BUDDY_INFO, OnUpdateBuddyInfo)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GMEMBER_INFO, OnUpdateGMemberInfo)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GROUP_INFO, OnUpdateGroupInfo)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_BUDDY_HEADPIC, OnUpdateBuddyHeadPic)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GMEMBER_HEADPIC, OnUpdateGMemberHeadPic)
		MESSAGE_HANDLER_EX(QQ_MSG_UPDATE_GROUP_HEADPIC, OnUpdateGroupHeadPic)
		MESSAGE_HANDLER_EX(QQ_MSG_CHANGE_STATUS_RESULT, OnChangeStatusResult)

		MESSAGE_HANDLER_EX(WM_TRAYICON_NOTIFY, OnTrayIconNotify)
		MESSAGE_HANDLER_EX(WM_SHOW_BUDDYCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_GROUPCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_SESSCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_SYSGROUPCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_BUDDYINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_GMEMBERINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_SHOW_GROUPINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_BUDDYCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_GROUPCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_SESSCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_SYSGROUPCHATDLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_BUDDYINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_GMEMBERINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_CLOSE_GROUPINFODLG, OnShowOrCloseDlg)
		MESSAGE_HANDLER_EX(WM_DEL_MSG_SENDER, OnDelMsgSender)
		MESSAGE_HANDLER_EX(WM_CANCEL_FLASH, OnCancelFlash)

		REFLECT_NOTIFICATIONS()      //��Ϣ����֪ͨ��
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

private:
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	void OnSysCommand(UINT nID, CPoint pt);
	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	void OnGetMinMaxInfo(LPMINMAXINFO lpMMI);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnTimer(UINT_PTR nIDEvent);
	void OnSize(UINT nType, CSize size);
	void OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey);
	void OnClose();
	void OnDestroy();
	void OnAppAbout(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnTrayIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnMenu_ShowMainPanel(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnMenu_Exit(UINT uNotifyCode, int nID, CWindow wndCtl);
	void OnBtn_Sign(UINT uNotifyCode, int nID, CWindow wndCtl);
	LRESULT OnTabCtrlDropDown(LPNMHDR pnmh);
	LRESULT OnTabCtrlSelChange(LPNMHDR pnmh);
	LRESULT OnBuddyListDblClk(LPNMHDR pnmh);
	LRESULT OnGroupListDblClk(LPNMHDR pnmh);
	LRESULT OnRecentListDblClk(LPNMHDR pnmh);
	void OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl);	// ��ȡ������ť(ȡ����¼)
	void OnPic_Clicked_Head(UINT uNotifyCode, int nID, CWindow wndCtl);	// ���û�ͷ�񡱿ؼ�
	void OnMenu_BigHeadPic(UINT uNotifyCode, int nID, CWindow wndCtl);	// ����ͷ�񡱲˵�
	void OnMenu_SmallHeadPic(UINT uNotifyCode, int nID, CWindow wndCtl);// ��Сͷ�񡱲˵�
	void OnMenu_StdHeadPic(UINT uNotifyCode, int nID, CWindow wndCtl);	// ����׼ͷ�񡱲˵�
	void OnMenu_ShowBigHeadPicInSel(UINT uNotifyCode, int nID, CWindow wndCtl);	// ��ѡ��ʱ��ʾ��ͷ�񡱲˵�
	void OnEdit_Sign_KillFocus(UINT uNotifyCode, int nID, CWindow wndCtl);		// ������ǩ�����༭�ı���
	void OnMenu_Status(UINT uNotifyCode, int nID, CWindow wndCtl);		// ���û�����״̬���˵�

	void CloseDialog(int nVal);

	BOOL InitTopToolBar();		// ��ʼ��Top������
	BOOL InitBottomToolBar();	// ��ʼ��Bottom������
	BOOL InitTabCtrl();			// ��ʼ��Tab��
	BOOL InitBuddyListCtrl();	// ��ʼ�������б�ؼ�
	BOOL InitGroupListCtrl();	// ��ʼ��Ⱥ�б�ؼ�
	BOOL InitRecentListCtrl();	// ��ʼ�������ϵ���б�ؼ�

	BOOL Init();
	void UnInit();

	void StartLogin(BOOL bAutoLogin = FALSE);
	void CloseAllDlg();

	LRESULT OnLoginResult(UINT uMsg, WPARAM wParam, LPARAM lParam);			// ��¼������Ϣ
	LRESULT OnLogoutResult(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ע��������Ϣ
	LRESULT OnUpdateUserInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);		// �����û���Ϣ
	LRESULT OnUpdateBuddyList(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ���º����б���Ϣ
	LRESULT OnUpdateGroupList(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ����Ⱥ�б���Ϣ
	LRESULT OnUpdateRecentList(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ���������ϵ���б���Ϣ
	LRESULT OnBuddyMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);			// ������Ϣ
	LRESULT OnGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);			// Ⱥ��Ϣ
	LRESULT OnSessMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);				// ��ʱ�Ự��Ϣ
	LRESULT OnStatusChangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ����״̬�ı���Ϣ
	LRESULT OnKickMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);				// ����������Ϣ
	LRESULT OnSysGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);			// Ⱥϵͳ��Ϣ
	LRESULT OnUpdateBuddyNumber(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ���º��Ѻ���
	LRESULT OnUpdateGMemberNumber(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ����Ⱥ��Ա����_
	LRESULT OnUpdateGroupNumber(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ����Ⱥ����
	LRESULT OnUpdateBuddySign(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ���º��Ѹ���ǩ��
	LRESULT OnUpdateGMemberSign(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ����Ⱥ��Ա����ǩ��
	LRESULT OnUpdateBuddyInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ���º�����Ϣ
	LRESULT OnUpdateGMemberInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ����Ⱥ��Ա��Ϣ
	LRESULT OnUpdateGroupInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);		// ����Ⱥ��Ϣ
	LRESULT OnUpdateBuddyHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ���º���ͷ��ͼƬ
	LRESULT OnUpdateGMemberHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam);// ����Ⱥ��Աͷ��ͼƬ
	LRESULT OnUpdateGroupHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam);	// ����Ⱥͷ��ͼƬ
	LRESULT OnChangeStatusResult(UINT uMsg, WPARAM wParam, LPARAM lParam);	// �ı�����״̬������Ϣ

	LRESULT OnShowOrCloseDlg(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnDelMsgSender(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnCancelFlash(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void ShowBuddyChatDlg(UINT nQQUin, BOOL bShow);
	void ShowGroupChatDlg(UINT nGroupCode, BOOL bShow);
	void ShowSessChatDlg(UINT nGroupCode, UINT nQQUin, BOOL bShow);
	void ShowSysGroupChatDlg(UINT nGroupCode, BOOL bShow);
	void ShowBuddyInfoDlg(UINT nQQUin, BOOL bShow);
	void ShowGMemberInfoDlg(UINT nGroupCode, UINT nQQUin, BOOL bShow);
	void ShowGroupInfoDlg(UINT nGroupCode, BOOL bShow);

	void NotifyBuddyChatDlg(UINT nQQUin, UINT uMsg);		// ֪ͨ�������촰�ڸ���
	void NotifyGroupChatDlg(UINT nGroupCode,				// ֪ͨȺ���촰�ڸ���
		UINT uMsg, WPARAM wParam, LPARAM lParam);
	void NotifySessChatDlg(UINT nQQUin, UINT uMsg);			// ֪ͨ��ʱ�Ự���촰�ڸ���
	void NotifyBuddyInfoDlg(UINT nQQUin, UINT uMsg);		// ֪ͨ������Ϣ���ڸ���
	void NotifyGMemberInfoDlg(UINT nGroupCode, UINT nQQUin, UINT uMsg);	// ֪ͨȺ��Ա��Ϣ���ڸ���
	void NotifyGroupInfoDlg(UINT nGroupCode, UINT uMsg);	// ֪ͨȺ��Ϣ���ڸ���

	void UpdateBuddyTreeCtrl();
	void UpdateGroupTreeCtrl();
	void UpdateRecentTreeCtrl();

	void OnTrayIcon_LButtunUp();
	void OnTrayIcon_RButtunUp();
	void OnTrayIcon_MouseHover();
	void OnTrayIcon_MouseLeave();

	BOOL LoadAppIcon(QQ_STATUS nStatus);
	void DestroyAppIcon();
	BOOL LoadLoginIcon();
	void DestroyLoginIcon();
	void UpdateMsgIcon();

	CString GetHeadPicFullName(UINT nGroupCode, UINT nQQUin);
	void GetNumber(UINT nGroupCode, UINT nQQUin, UINT& nGroupNum, UINT& nQQNum);
	HICON ExtractIcon(LPCTSTR lpszFileName);

	void ShowPanel(BOOL bShow);	// ��ʾָ����庯��(bShow��TRUE��ʾ��ʾ����壬FALSE��ʾ��ʾ��¼���)
	QQ_STATUS GetStatusFromMenuID(int nMenuID);		// �Ӳ˵�ID��ȡ��Ӧ��QQ_STATUS
	void StatusMenuBtn_SetIconPic(CSkinButton& btnStatus, QQ_STATUS nStatus);	// ����ָ��״̬����״̬�˵���ť��ͼ��

private:
	CSkinDialog m_SkinDlg;
	CLoginDlg m_LoginDlg;
	CSkinMenu m_SkinMenu;
	CSkinButton m_btnMainMenu;
	CSkinButton m_btnSystemSet, m_btnMsgMgr;
	CSkinButton m_btnSafe, m_btnFind;
	CSkinButton m_btnStatus;
	CSkinEdit m_edtSign, m_edtSearch;
	CSkinPictureBox m_picHead;
	CSkinStatic m_staNickName;
	CSkinButton m_btnSign;
	CBuddyChatDlg m_BuddyChatDlg;
	CGroupChatDlg m_GroupChatDlg;
	CSkinToolBar m_tbTop, m_tbBottom;
	CSkinButton m_btnMsgBox, m_btnChangeExterior, m_btnAppMgr;
	CSkinTabCtrl m_TabCtrl;		// m_MainTabCtrl
	CBuddyListCtrl m_BuddyListCtrl;
	CBuddyListCtrl m_GroupListCtrl;
	CBuddyListCtrl m_RecentListCtrl;

	CSkinPictureBox m_picLogining;
	CSkinStatic m_staQQNum, m_staLogining;
	CSkinButton m_btnCancel;

	CMsgTipDlg m_MsgTipDlg;

	std::map<UINT, CBuddyChatDlg *> m_mapBuddyChatDlg;
	std::map<UINT, CGroupChatDlg *> m_mapGroupChatDlg;
 	std::map<UINT, CSessChatDlg *> m_mapSessChatDlg;
 	std::map<UINT, CBuddyInfoDlg *> m_mapBuddyInfoDlg;
 	std::map<CGMemberInfoMapKey, CBuddyInfoDlg *> m_mapGMemberInfoDlg;
 	std::map<UINT, CGroupInfoDlg *> m_mapGroupInfoDlg;

	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;
	ULONG_PTR m_gdiplusToken;

	CQQClient m_QQClient;
	CFaceList m_FaceList;
	CTrayIcon m_TrayIcon;

	QQ_MSG_TYPE m_nLastMsgType;
	UINT m_nLastSenderId;

	HICON m_hAppIcon;
	HICON m_hLoginIcon[6];
	int m_nCurLoginIcon;
	HICON m_hMsgIcon;
	DWORD m_dwLoginTimerId;
	DWORD m_dwMsgTimerId;

	HICON m_hDlgIcon, m_hDlgSmallIcon;
	HFONT m_fontNickName;
	CLoginAccountList m_LoginAccountList;
	LOGIN_ACCOUNT_INFO m_stAccountInfo;
	CCascadeWinManager m_CascadeWinManager;
};

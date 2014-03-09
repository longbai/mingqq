// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"

#pragma comment(lib, "winmm.lib")

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->hwnd == m_edtSign.m_hWnd)   
	{
		if (pMsg->message == WM_CHAR && pMsg->wParam == VK_RETURN) 
		{
			SetFocus();
		} 
	}

 	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
 	{
 		return FALSE;
 	}

	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

CMainDlg::CMainDlg(void)
{
	m_nLastMsgType = QQ_MSG_TYPE_BUDDY;
	m_nLastSenderId = 0;

	m_hAppIcon = NULL;
	memset(m_hLoginIcon, 0, sizeof(m_hLoginIcon));
	m_nCurLoginIcon = 0;
	m_hMsgIcon = NULL;
	m_dwLoginTimerId = NULL;
	m_dwMsgTimerId = NULL;

	m_hDlgIcon = m_hDlgSmallIcon = NULL;
	m_fontNickName = NULL;
	memset(&m_stAccountInfo, 0, sizeof(m_stAccountInfo));
}

CMainDlg::~CMainDlg(void)
{
}

BOOL CMainDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	SetWindowPos(HWND_TOPMOST, 989, 64, 300, 600, NULL);

	// set icons
	m_hDlgIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(m_hDlgIcon, TRUE);
	m_hDlgSmallIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(m_hDlgSmallIcon, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	Init();

	ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

	m_QQClient.Init();

	LoadAppIcon(m_QQClient.GetStatus());
	m_TrayIcon.AddIcon(m_hWnd, WM_TRAYICON_NOTIFY, 1, m_hAppIcon, _T("MingQQ"));

	// ����ϵͳ�����б�
	tstring strFileName = ZYM::CPath::GetAppPath() + _T("Face\\FaceConfig.xml");
	m_FaceList.LoadConfigFile(strFileName.c_str());

	// ���ص�¼�ʺ��б�
	strFileName = ZYM::CPath::GetAppPath()+_T("LoginAccountList.dat");
	m_LoginAccountList.LoadFile(strFileName.c_str());
	



// 	CImageEx img;
// 	img.LoadFromFile(_T("C:\\DefGMemberHeadPic16x16.png"));
// 	img.GrayScale();
// 	img.Save(_T("C:\\1.png"));

// 	Gdiplus::Bitmap * lpImage = new Gdiplus::Bitmap(_T("C:\\DefBuddyHeadPic.png"));
// 
// 	Gdiplus::Bitmap* pThumbnail = (Gdiplus::Bitmap*)lpImage->GetThumbnailImage(16, 16);
// 
// 	CLSID Clsid;
// 	Clsid = GetEncoderClsidByMimeType(L"image/png");
// 	pThumbnail->Save(_T("C:\\1.png"), &Clsid);
// 
// 	pThumbnail->GetHBITMAP()

	StartLogin(m_LoginAccountList.IsAutoLogin());

	// �����û�ͷ��
	UINT nQQUin = _tcstoul(m_stAccountInfo.szUser, NULL, 10);
	strFileName = m_QQClient.GetUserHeadPicFullName(nQQUin);
	if (!ZYM::CPath::IsFileExist(strFileName.c_str()))
		strFileName = ZYM::CPath::GetAppPath() + _T("Image\\DefBuddyHeadPic.png");
	m_picHead.SetBitmap(strFileName.c_str());

	return TRUE;
}

void CMainDlg::OnSysCommand(UINT nID, CPoint pt)
{
	if (nID == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
		return;
	}

	SetMsgHandled(FALSE);
}

void CMainDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	m_SkinMenu.OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CMainDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	m_SkinMenu.OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CMainDlg::OnGetMinMaxInfo(LPMINMAXINFO lpMMI)
{
	lpMMI->ptMinTrackSize.x = 300;
	lpMMI->ptMinTrackSize.y = 600;
}

void CMainDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (GetFocus() == m_edtSign.m_hWnd)
		SetFocus();
}

void CMainDlg::OnTimer(UINT_PTR nIDEvent)
{
	m_TrayIcon.OnTimer(nIDEvent);

	if (nIDEvent == m_dwMsgTimerId)		// ����Ϣ��˸����
	{
		static BOOL bSwitch = FALSE;

		if (m_dwLoginTimerId != NULL)	// δ��¼�ɹ�
			return;

		bSwitch = !bSwitch;
		if (bSwitch)
			m_TrayIcon.ModifyIcon(NULL, _T(""));
		else
			m_TrayIcon.ModifyIcon(m_hMsgIcon, _T(""));
	}
	else if (nIDEvent == m_dwLoginTimerId)	// ��¼����
	{
		m_TrayIcon.ModifyIcon(m_hLoginIcon[m_nCurLoginIcon], _T(""));
		m_nCurLoginIcon++;
		if (m_nCurLoginIcon >= 6)
			m_nCurLoginIcon = 0;
	}
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);

	CRect rcClient;
	GetClientRect(&rcClient);

	if (m_btnMainMenu.IsWindow())
		m_btnMainMenu.MoveWindow(6, rcClient.Height()-48, 40, 40);

	if (m_btnSystemSet.IsWindow())
		m_btnSystemSet.MoveWindow(46, rcClient.Height()-30, 22, 22);

	if (m_btnMsgMgr.IsWindow())
		m_btnMsgMgr.MoveWindow(72, rcClient.Height()-30, 22, 22);

	if (m_btnSafe.IsWindow())
		m_btnSafe.MoveWindow(94, rcClient.Height()-30, 62, 22);

	if (m_btnFind.IsWindow())
		m_btnFind.MoveWindow(156, rcClient.Height()-30, 62, 22);

	if (m_btnSign.IsWindow())
		m_btnSign.MoveWindow(56, 54, rcClient.Width()-102, 22);

	if (m_edtSign.IsWindow())
		m_edtSign.MoveWindow(56, 54, rcClient.Width()-102, 22);

	if (m_edtSearch.IsWindow())
		m_edtSearch.MoveWindow(3, 104, rcClient.Width()-6, 27);

	if (m_btnMsgBox.IsWindow())
		m_btnMsgBox.MoveWindow(rcClient.Width()-48, 80, 22, 20);

	if (m_btnChangeExterior.IsWindow())
		m_btnChangeExterior.MoveWindow(rcClient.Width()-26, 80, 20, 20);

	if (m_btnAppMgr.IsWindow())
		m_btnAppMgr.MoveWindow(rcClient.Width()-38, rcClient.bottom-60, 22, 22);

	if (m_tbBottom.IsWindow())
		m_tbBottom.MoveWindow(46, rcClient.bottom-60, 212, 22);

	if (m_TabCtrl.IsWindow())
	{
		int nCount = m_TabCtrl.GetItemCount();
		if (nCount > 0)
		{
			int nWidth = (rcClient.Width()-2) / nCount;
			int nRemainder = (rcClient.Width()-2) % nCount;

			for (int i = 0; i < nCount; i++)
			{
				m_TabCtrl.SetItemSize(i, nWidth, 34, nWidth-19, 19);
			}

			m_TabCtrl.SetItemSize(nCount-1, nWidth+nRemainder, 34, nWidth+nRemainder-19, 19);
		}

		m_TabCtrl.MoveWindow(1, 135, rcClient.right - 2, 34);
	}

	if (m_BuddyListCtrl.IsWindow())
		m_BuddyListCtrl.MoveWindow(1, 135+34, rcClient.Width() - 2, rcClient.Height()-236);

	if (m_GroupListCtrl.IsWindow())
		m_GroupListCtrl.MoveWindow(1, 135+34, rcClient.Width() - 2, rcClient.Height()-236);

	if (m_RecentListCtrl.IsWindow())
		m_RecentListCtrl.MoveWindow(1, 135+34, rcClient.Width() - 2, rcClient.Height()-236);

	if (m_picLogining.IsWindow())
		m_picLogining.MoveWindow((rcClient.Width() - 220) / 2, 76, 220, 150);
	
	if (m_staQQNum.IsWindow())
		m_staQQNum.MoveWindow(rcClient.left, 226, rcClient.Width(), 14);

	if (m_staLogining.IsWindow())
		m_staLogining.MoveWindow(rcClient.left, 240, rcClient.Width(), 16);

	if (m_btnCancel.IsWindow())
		m_btnCancel.MoveWindow((rcClient.Width() - 86) / 2, 304, 86, 30);
}

void CMainDlg::OnHotKey(int nHotKeyID, UINT uModifiers, UINT uVirtKey)
{
	switch (nHotKeyID)
	{
	case 1001:
		{
			OnTrayIconNotify(WM_TRAYICON_NOTIFY, NULL, WM_LBUTTONUP);
		}
		break;
	}
}

void CMainDlg::OnClose()
{
	CloseDialog(IDOK);
}

void CMainDlg::OnDestroy()
{
	if (m_staNickName.IsWindow())
		m_staNickName.DestroyWindow();

	m_btnSystemSet.DestroyWindow();
	//m_TrayIcon.RemoveIcon();
	m_SkinMenu.DestroyMenu();
	if (m_BuddyChatDlg.IsWindow())
		m_BuddyChatDlg.DestroyWindow();
	if (m_GroupChatDlg.IsWindow())
		m_GroupChatDlg.DestroyWindow();

	DestroyAppIcon();
	DestroyLoginIcon();

	if (m_hMsgIcon != NULL)
	{
		::DestroyIcon(m_hMsgIcon);
		m_hMsgIcon = NULL;
	}

	if (m_hDlgIcon != NULL)
	{
		::DestroyIcon(m_hDlgIcon);
		m_hDlgIcon = NULL;
	}

	if (m_hDlgSmallIcon != NULL)
	{
		::DestroyIcon(m_hDlgSmallIcon);
		m_hDlgSmallIcon = NULL;
	}

	if (m_fontNickName != NULL)
	{
		::DeleteObject(m_fontNickName);
		m_fontNickName = NULL;
	}

	::UnregisterHotKey(m_hWnd, 1001);	// ��ע����ȡ��Ϣ�ȼ�

	// ������������
	tstring strFileName = m_QQClient.GetPersonalFolder()+_T("MiscConfig.xml");
	SaveMiscConfig(strFileName.c_str());

	m_QQClient.UnInit();

	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);
}

void CMainDlg::OnAppAbout(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CAboutDlg dlg;
	dlg.DoModal();
}

void CMainDlg::CloseDialog(int nVal)
{
	if (m_dwLoginTimerId != NULL)
	{
		KillTimer(m_dwLoginTimerId);
		m_dwLoginTimerId = NULL;
	}
	if (m_dwMsgTimerId != NULL)
	{
		KillTimer(m_dwMsgTimerId);
		m_dwMsgTimerId = NULL;
	}

	m_TrayIcon.RemoveIcon();
	CloseAllDlg();
	m_CascadeWinManager.Clear();
	ShowWindow(SW_HIDE);

	if (m_QQClient.GetStatus() != QQ_STATUS_OFFLINE)
	{
		m_QQClient.Logout();
	}
	else
	{
		DestroyWindow();
		::PostQuitMessage(nVal);
	}
}

// ��ʼ��Top������
BOOL CMainDlg::InitTopToolBar()
{
	int nIndex = m_tbTop.AddItem(101, STBI_STYLE_BUTTON);
	m_tbTop.SetItemSize(nIndex, 35, 20);
	m_tbTop.SetItemText(nIndex, _T("10"));
	m_tbTop.SetItemToolTipText(nIndex, _T("QQ�ռ���Ϣ����\r\n  ���Ѷ�̬(10+)"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\qzone.png"));

	nIndex = m_tbTop.AddItem(102, STBI_STYLE_BUTTON);
	m_tbTop.SetItemSize(nIndex, 22, 20);
	m_tbTop.SetItemToolTipText(nIndex, _T("��Ѷ΢��"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\wblog.png"));

	nIndex = m_tbTop.AddItem(103, STBI_STYLE_BUTTON);
	m_tbTop.SetItemSize(nIndex, 35, 20);
	m_tbTop.SetItemText(nIndex, _T("99"));
	m_tbTop.SetItemToolTipText(nIndex, _T("QQ����\r\n  �ռ���(84)\r\n  Ⱥ�ʼ�(20)\r\n  Ư��ƿ(1)"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\mail.png"));

	nIndex = m_tbTop.AddItem(104, STBI_STYLE_BUTTON);
	m_tbTop.SetItemSize(nIndex, 22, 20);
	m_tbTop.SetItemToolTipText(nIndex, _T("������"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\friend.png"));

	nIndex = m_tbTop.AddItem(105, STBI_STYLE_BUTTON);
	m_tbTop.SetItemSize(nIndex, 22, 20);
	m_tbTop.SetItemToolTipText(nIndex, _T("��Ѷ����"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\paipai.png"));

	nIndex = m_tbTop.AddItem(106, STBI_STYLE_BUTTON);
	m_tbTop.SetItemSize(nIndex, 22, 20);
	m_tbTop.SetItemToolTipText(nIndex, _T("QQǮ��"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\purse.png"));

	nIndex = m_tbTop.AddItem(107, STBI_STYLE_BUTTON);
	m_tbTop.SetItemSize(nIndex, 22, 20);
	m_tbTop.SetItemToolTipText(nIndex, _T("���Ѹ�������\r\n  ��������������μ���������"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\soso.png"));

	nIndex = m_tbTop.AddItem(108, STBI_STYLE_BUTTON);
	m_tbTop.SetItemSize(nIndex, 22, 20);
	m_tbTop.SetItemToolTipText(nIndex, _T("��Ѷ���ҵ���Ѷ"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemIconPic(nIndex, _T("MainTopToolBar\\Today.png"));

	m_tbTop.SetLeftTop(2, 0);
	m_tbTop.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcTopToolBar(4, 80, 4+210, 80+20);
	m_tbTop.Create(m_hWnd, rcTopToolBar, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOLBAR_TOP);

	return TRUE;
}

// ��ʼ��Bottom������
BOOL CMainDlg::InitBottomToolBar()
{
	int nIndex = m_tbBottom.AddItem(201, STBI_STYLE_BUTTON);
	m_tbBottom.SetItemSize(nIndex, 22, 22);
	m_tbBottom.SetItemPadding(nIndex, 5);
	m_tbBottom.SetItemToolTipText(nIndex, _T("�ֻ�����"));
	m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\WirelessLive.png"));

	nIndex = m_tbBottom.AddItem(202, STBI_STYLE_BUTTON);
	m_tbBottom.SetItemSize(nIndex, 22, 22);
	m_tbBottom.SetItemPadding(nIndex, 5);
	m_tbBottom.SetItemToolTipText(nIndex, _T("QQ��Ϸ"));
	m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\QQGame.png"));

	nIndex = m_tbBottom.AddItem(203, STBI_STYLE_BUTTON);
	m_tbBottom.SetItemSize(nIndex, 22, 22);
	m_tbBottom.SetItemPadding(nIndex, 5);
	m_tbBottom.SetItemToolTipText(nIndex, _T("QQ����"));
	m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\QQPet.png"));

	nIndex = m_tbBottom.AddItem(204, STBI_STYLE_BUTTON);
	m_tbBottom.SetItemSize(nIndex, 22, 22);
	m_tbBottom.SetItemPadding(nIndex, 5);
	m_tbBottom.SetItemToolTipText(nIndex, _T("QQ����"));
	m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\QQMusic.png"));

	nIndex = m_tbBottom.AddItem(205, STBI_STYLE_BUTTON);
	m_tbBottom.SetItemSize(nIndex, 22, 22);
	m_tbBottom.SetItemPadding(nIndex, 5);
	m_tbBottom.SetItemToolTipText(nIndex, _T("��Ѷ��Ƶ"));
	m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\QQLive.png"));

	nIndex = m_tbBottom.AddItem(206, STBI_STYLE_BUTTON);
	m_tbBottom.SetItemSize(nIndex, 22, 22);
	m_tbBottom.SetItemPadding(nIndex, 5);
	m_tbBottom.SetItemToolTipText(nIndex, _T("QQ����"));
	m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\Tourism.png"));

	nIndex = m_tbBottom.AddItem(207, STBI_STYLE_BUTTON);
	m_tbBottom.SetItemSize(nIndex, 22, 22);
	m_tbBottom.SetItemPadding(nIndex, 5);
	m_tbBottom.SetItemToolTipText(nIndex, _T("QQ���Թܼ�"));
	m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\QQPCMgr.png"));

	nIndex = m_tbBottom.AddItem(208, STBI_STYLE_BUTTON);
	m_tbBottom.SetItemSize(nIndex, 22, 22);
	m_tbBottom.SetItemPadding(nIndex, 5);
	m_tbBottom.SetItemToolTipText(nIndex, _T("���Ĺ���"));
	m_tbBottom.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbBottom.SetItemIconPic(nIndex, _T("MainBottomToolBar\\PaiPai.png"));

	m_tbBottom.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcBottomToolBar(46, rcClient.bottom-60, 46+212, (rcClient.bottom-60)+22);
	m_tbBottom.Create(m_hWnd, rcBottomToolBar, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOLBAR_BOTTOM);

	return TRUE;
}

// ��ʼ��Tab��
BOOL CMainDlg::InitTabCtrl()
{
	CRect rcClient;
	GetClientRect(&rcClient);

	int nWidth = (rcClient.Width()-2) / 5;
	int nRemainder = (rcClient.Width()-2) % 5;

	int nIndex = m_TabCtrl.AddItem(301, STCI_STYLE_DROPDOWN);
	m_TabCtrl.SetItemSize(nIndex, nWidth, 34, nWidth-19, 19);
	m_TabCtrl.SetItemToolTipText(nIndex, _T("��ϵ��"));
	m_TabCtrl.SetItemIconPic(nIndex, _T("MainTabCtrl\\icon_contacts_normal.png"),
		_T("MainTabCtrl\\icon_contacts_selected.png"));

	nIndex = m_TabCtrl.AddItem(302, STCI_STYLE_DROPDOWN);
	m_TabCtrl.SetItemSize(nIndex, nWidth, 34, nWidth-19, 19);
	m_TabCtrl.SetItemToolTipText(nIndex, _T("����"));
	m_TabCtrl.SetItemIconPic(nIndex, _T("MainTabCtrl\\FriendSocial_TabBtn_Normal.png"),
		_T("MainTabCtrl\\FriendSocial_TabBtn_Focus.png"));

	nIndex = m_TabCtrl.AddItem(303, STCI_STYLE_DROPDOWN);
	m_TabCtrl.SetItemSize(nIndex, nWidth, 34, nWidth-19, 19);
	m_TabCtrl.SetItemToolTipText(nIndex, _T("Ⱥ/������"));
	m_TabCtrl.SetItemIconPic(nIndex, _T("MainTabCtrl\\icon_group_normal.png"),
		_T("MainTabCtrl\\icon_group_selected.png"));

	nIndex = m_TabCtrl.AddItem(304, STCI_STYLE_BUTTON);
	m_TabCtrl.SetItemSize(nIndex, nWidth, 34, nWidth-19, 19);
	m_TabCtrl.SetItemToolTipText(nIndex, _T("΢��"));
	m_TabCtrl.SetItemIconPic(nIndex, _T("MainTabCtrl\\WBlog_TabBtn_Normal_Msg.png"),
		_T("MainTabCtrl\\WBlog_TabBtn_Focus.png"));

	nIndex = m_TabCtrl.AddItem(305, STCI_STYLE_DROPDOWN);
	m_TabCtrl.SetItemSize(nIndex, nWidth+nRemainder, 34, nWidth+nRemainder-19, 19);
	m_TabCtrl.SetItemToolTipText(nIndex, _T("�����ϵ��"));
	m_TabCtrl.SetItemIconPic(nIndex, _T("MainTabCtrl\\icon_last_normal.png"),
		_T("MainTabCtrl\\icon_last_selected.png"));

	m_TabCtrl.SetBgPic(_T("MainTabCtrl\\main_tab_bkg.png"), CRect(5,1,5,1));
	m_TabCtrl.SetItemsBgPic(NULL, _T("MainTabCtrl\\main_tab_highlight.png"), 
		_T("MainTabCtrl\\main_tab_check.png"), CRect(5,1,5,1));
	m_TabCtrl.SetItemsArrowPic(_T("MainTabCtrl\\main_tabbtn_highlight.png"), 
		_T("MainTabCtrl\\main_tabbtn_down.png"));

	m_TabCtrl.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcTabCtrl(1, 135, rcClient.right - 1, 135+34);
	m_TabCtrl.Create(m_hWnd, rcTabCtrl, NULL, WS_CHILD | WS_VISIBLE, NULL, ID_TABCTRL_MAIN, NULL);

	m_TabCtrl.SetCurSel(0);

	return TRUE;
}

// ��ʼ�������б�ؼ�
BOOL CMainDlg::InitBuddyListCtrl()
{
	m_BuddyListCtrl.SetMargin(CRect(2,0,2,0));
	m_BuddyListCtrl.SetBgPic(_T("BuddyList\\Bg.png"));
	m_BuddyListCtrl.SetBuddyTeamHotBgPic(_T("BuddyList\\BuddyTeamHotBg.png"));
	m_BuddyListCtrl.SetBuddyItemHotBgPic(_T("BuddyList\\BuddyItemHotBg.png"), CRect(1,1,1,1));
	m_BuddyListCtrl.SetBuddyItemSelBgPic(_T("BuddyList\\BuddyItemSelBg.png"), CRect(1,1,1,1));
	m_BuddyListCtrl.SetHeadFramePic(_T("BuddyList\\Padding4Select.png"), CRect(6,6,6,6));
	m_BuddyListCtrl.SetNormalArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTexture.png"));
	m_BuddyListCtrl.SetHotArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	m_BuddyListCtrl.SetSelArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	m_BuddyListCtrl.SetNormalExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTexture.png"));
	m_BuddyListCtrl.SetHotExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_BuddyListCtrl.SetSelExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_BuddyListCtrl.SetStdGGHeadPic(_T("BuddyList\\trad_boy.png"));
	m_BuddyListCtrl.SetStdMMHeadPic(_T("BuddyList\\trad_girl.png"));
	m_BuddyListCtrl.SetStyle(BLC_BIG_ICON_STYLE);
	m_BuddyListCtrl.SetShowBigIconInSel(TRUE);
	m_BuddyListCtrl.SetBuddyTeamHeight(24);
	m_BuddyListCtrl.SetBuddyItemHeightInBigIcon(54);
	m_BuddyListCtrl.SetBuddyItemHeightInSmallIcon(28);
	m_BuddyListCtrl.SetBuddyItemHeightInStandardIcon(24);
	m_BuddyListCtrl.SetBuddyItemPadding(1);

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcListCtrl(1, 135+34, rcClient.right - 1, (135+34)+(rcClient.Height()-236));
	m_BuddyListCtrl.Create(m_hWnd, rcListCtrl, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_LISTCTRL_BUDDY);

	return TRUE;
}

// ��ʼ��Ⱥ�б�ؼ�
BOOL CMainDlg::InitGroupListCtrl()
{
	m_GroupListCtrl.SetMargin(CRect(2,0,2,0));
	m_GroupListCtrl.SetBgPic(_T("BuddyList\\Bg.png"));
	m_GroupListCtrl.SetBuddyTeamHotBgPic(_T("BuddyList\\BuddyTeamHotBg.png"));
	m_GroupListCtrl.SetBuddyItemHotBgPic(_T("BuddyList\\BuddyItemHotBg.png"), CRect(1,1,1,1));
	m_GroupListCtrl.SetBuddyItemSelBgPic(_T("BuddyList\\BuddyItemSelBg.png"), CRect(1,1,1,1));
	m_GroupListCtrl.SetHeadFramePic(_T("BuddyList\\Padding4Select.png"), CRect(6,6,6,6));
	m_GroupListCtrl.SetNormalArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTexture.png"));
	m_GroupListCtrl.SetHotArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	m_GroupListCtrl.SetSelArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	m_GroupListCtrl.SetNormalExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTexture.png"));
	m_GroupListCtrl.SetHotExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_GroupListCtrl.SetSelExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_GroupListCtrl.SetStdGGHeadPic(_T("BuddyList\\trad_boy.png"));
	m_GroupListCtrl.SetStdMMHeadPic(_T("BuddyList\\trad_girl.png"));
	m_GroupListCtrl.SetStyle(BLC_SMALL_ICON_STYLE);
	m_GroupListCtrl.SetShowBigIconInSel(TRUE);
	m_GroupListCtrl.SetBuddyTeamHeight(24);
	m_GroupListCtrl.SetBuddyItemHeightInBigIcon(54);
	m_GroupListCtrl.SetBuddyItemHeightInSmallIcon(28);
	m_GroupListCtrl.SetBuddyItemHeightInStandardIcon(24);
	m_GroupListCtrl.SetBuddyItemPadding(1);

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcListCtrl(1, 135+34, rcClient.right - 1, (135+34)+(rcClient.Height()-236));
	m_GroupListCtrl.Create(m_hWnd, rcListCtrl, NULL, WS_CHILD, NULL, ID_LISTCTRL_GROUP);

	return TRUE;
}

// ��ʼ�������ϵ���б�ؼ�
BOOL CMainDlg::InitRecentListCtrl()
{
	m_RecentListCtrl.SetMargin(CRect(2,0,2,0));
	m_RecentListCtrl.SetBgPic(_T("BuddyList\\Bg.png"));
	m_RecentListCtrl.SetBuddyTeamHotBgPic(_T("BuddyList\\BuddyTeamHotBg.png"));
	m_RecentListCtrl.SetBuddyItemHotBgPic(_T("BuddyList\\BuddyItemHotBg.png"), CRect(1,1,1,1));
	m_RecentListCtrl.SetBuddyItemSelBgPic(_T("BuddyList\\BuddyItemSelBg.png"), CRect(1,1,1,1));
	m_RecentListCtrl.SetHeadFramePic(_T("BuddyList\\Padding4Select.png"), CRect(6,6,6,6));
	//m_RecentListCtrl.AddBuddyItemSmallIcon(int nID, LPCTSTR lpszToolTipText, LPCTSTR lpszFileName);
	m_RecentListCtrl.SetNormalArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTexture.png"));
	m_RecentListCtrl.SetHotArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	m_RecentListCtrl.SetSelArrowPic(_T("BuddyList\\MainPanel_FolderNode_collapseTextureHighlight.png"));
	m_RecentListCtrl.SetNormalExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTexture.png"));
	m_RecentListCtrl.SetHotExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_RecentListCtrl.SetSelExpArrowPic(_T("BuddyList\\MainPanel_FolderNode_expandTextureHighlight.png"));
	m_RecentListCtrl.SetStdGGHeadPic(_T("BuddyList\\trad_boy.png"));
	m_RecentListCtrl.SetStdMMHeadPic(_T("BuddyList\\trad_girl.png"));
	m_RecentListCtrl.SetStyle(BLC_BIG_ICON_STYLE);
	m_RecentListCtrl.SetShowBigIconInSel(TRUE);
	m_RecentListCtrl.SetBuddyTeamHeight(24);
	m_RecentListCtrl.SetBuddyItemHeightInBigIcon(54);
	m_RecentListCtrl.SetBuddyItemHeightInSmallIcon(28);
	m_RecentListCtrl.SetBuddyItemHeightInStandardIcon(24);
	m_RecentListCtrl.SetBuddyItemPadding(1);

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcListCtrl(1, 135+34, rcClient.right - 1, (135+34)+(rcClient.Height()-236));
	m_RecentListCtrl.Create(m_hWnd, rcListCtrl, NULL, WS_CHILD, NULL, ID_LISTCTRL_RECENT);

	return TRUE;
}

BOOL CMainDlg::Init()
{
	m_SkinDlg.SetBgPic(_T("main_panel_bg.png"), CRect(2, 135, 2, 67));
	//m_SkinDlg.SetTitleBarBgPic(_T("main_panel_title.png"));
	m_SkinDlg.SetMinSysBtnPic(_T("SysBtn\\btn_mini_normal.png"), 
		_T("SysBtn\\btn_mini_highlight.png"), _T("SysBtn\\btn_mini_down.png"));
	m_SkinDlg.SetMaxSysBtnPic(_T("SysBtn\\btn_max_normal.png"), 
		_T("SysBtn\\btn_max_highlight.png"), _T("SysBtn\\btn_max_down.png"));
	m_SkinDlg.SetRestoreSysBtnPic(_T("SysBtn\\btn_restore_normal.png"), 
		_T("SysBtn\\btn_restore_highlight.png"), _T("SysBtn\\btn_restore_down.png"));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), 
		_T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetTitleText(_T("MingQQ"));

	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	CRect rcClient;
	GetClientRect(&rcClient);

	m_btnMainMenu.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnMainMenu.SetTransparent(TRUE, hDlgBgDC);
	m_btnMainMenu.SetBgPic(_T("menu_btn_normal.png"), 
		_T("menu_btn_highlight.png"), _T("menu_btn_highlight.png"), _T("menu_btn_normal.png"));
	m_btnMainMenu.SubclassWindow(GetDlgItem(ID_BTN_MAIN_MENU));
	m_btnMainMenu.MoveWindow(6, rcClient.Height()-48, 40, 40, TRUE);
	m_btnMainMenu.SetToolTipText(_T("���˵�"));

	m_btnSystemSet.SetButtonType(SKIN_ICON_BUTTON);
	m_btnSystemSet.SetTransparent(TRUE, hDlgBgDC);
	m_btnSystemSet.SetBgPic(NULL, _T("allbtn_highlight.png"), _T("allbtn_down.png"), NULL);
	m_btnSystemSet.SetIconPic(_T("SystemSet.png"));
	m_btnSystemSet.SubclassWindow(GetDlgItem(ID_BTN_SYSTEM_SET));
	m_btnSystemSet.MoveWindow(46, rcClient.Height()-30, 22, 22, TRUE);
	m_btnSystemSet.SetToolTipText(_T("��ϵͳ����"));

	m_btnMsgMgr.SetButtonType(SKIN_ICON_BUTTON);
	m_btnMsgMgr.SetTransparent(TRUE, hDlgBgDC);
	m_btnMsgMgr.SetBgPic(NULL, _T("allbtn_highlight.png"), _T("allbtn_down.png"), NULL);
	m_btnMsgMgr.SetIconPic(_T("message.png"));
	m_btnMsgMgr.SubclassWindow(GetDlgItem(ID_BTN_MSG_MGR));
	m_btnMsgMgr.MoveWindow(72, rcClient.Height()-30, 22, 22, TRUE);
	m_btnMsgMgr.SetToolTipText(_T("����Ϣ������"));

	m_btnSafe.SetButtonType(SKIN_ICON_BUTTON);
	m_btnSafe.SetTransparent(TRUE, hDlgBgDC);
	m_btnSafe.SetBgPic(NULL, _T("allbtn_highlight2.png"), _T("allbtn_down2.png"), NULL);
	m_btnSafe.SetIconPic(_T("QQSafe.png"));
	m_btnSafe.SubclassWindow(GetDlgItem(ID_BTN_SAFE));
	m_btnSafe.MoveWindow(94, rcClient.Height()-30, 62, 22, TRUE);
	m_btnSafe.SetToolTipText(_T("�򿪰�ȫ�ſ�"));

	m_btnFind.SetButtonType(SKIN_ICON_BUTTON);
	m_btnFind.SetTransparent(TRUE, hDlgBgDC);
	m_btnFind.SetBgPic(NULL, _T("allbtn_highlight2.png"), _T("allbtn_down2.png"), NULL);
	m_btnFind.SetIconPic(_T("find.png"));
	m_btnFind.SubclassWindow(GetDlgItem(ID_BTN_FIND));
	m_btnFind.MoveWindow(156, rcClient.Height()-30, 62, 22, TRUE);
	m_btnFind.SetToolTipText(_T("������ϵ��"));

	m_btnSign.SetButtonType(SKIN_ICON_BUTTON);
	m_btnSign.SetTransparent(TRUE, hDlgBgDC);
	m_btnSign.SetBgPic(NULL, _T("allbtn_highlight2.png"), _T("allbtn_down2.png"), NULL, CRect(3,0,3,0));
	m_btnSign.SetTextAlign(DT_LEFT);
	m_btnSign.SubclassWindow(GetDlgItem(ID_BTN_SIGN));
	m_btnSign.MoveWindow(56, 54, rcClient.Width()-102, 22, TRUE);
	m_btnSign.SetWindowText(_T("��һ����,ʲô��û�����¡�"));
	m_btnSign.SetToolTipText(_T("��һ����,ʲô��û�����¡�"));

	m_picHead.SetTransparent(TRUE, hDlgBgDC);
	m_picHead.SetShowCursor(TRUE);
	m_picHead.SetBgPic(_T("HeadCtrl\\Padding4Normal.png"), _T("HeadCtrl\\Padding4Hot.png"), _T("HeadCtrl\\Padding4Hot.png"));
	m_picHead.SubclassWindow(GetDlgItem(ID_PIC_HEAD));
	m_picHead.MoveWindow(6, 32, 48, 48, TRUE);
	m_picHead.SetToolTipText(_T("��������������30%�����Ƹ�������"));

	m_staNickName.SetTransparent(TRUE, hDlgBgDC);
	m_staNickName.SubclassWindow(GetDlgItem(ID_STATIC_NICKNAME));
	m_staNickName.MoveWindow(95, 35, 220, 14, FALSE);
	m_staNickName.SetWindowText(_T("�ǳ�"));

	CFontHandle font = (HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0, 0L);
	LOGFONT lf = {0};
	font.GetLogFont(&lf);
	lf.lfHeight = 15;
	lf.lfWeight = FW_BOLD;
	m_fontNickName = ::CreateFontIndirect(&lf); 
	m_staNickName.SetFont(m_fontNickName);

	m_SkinMenu.SetBgPic(_T("Menu\\menu_left_bg.png"), _T("Menu\\menu_right_bg.png"));
	m_SkinMenu.SetSelectedPic(_T("Menu\\menu_selected.png"));
	m_SkinMenu.SetSepartorPic(_T("Menu\\menu_separtor.png"));
	m_SkinMenu.SetArrowPic(_T("Menu\\menu_arrow.png"));
	m_SkinMenu.SetCheckPic(_T("Menu\\menu_check.png"));
	m_SkinMenu.SetTextColor(RGB(0, 20, 35));
	m_SkinMenu.SetSelTextColor(RGB(254, 254, 254));
	m_SkinMenu.LoadMenu(ID_MENU_MAIN_PANEL);

	DWORD dwMenuIDs[] = {ID_MENU_IMONLINE, ID_MENU_QME, ID_MENU_AWAY, 
		ID_MENU_BUSY, ID_MENU_MUTE, ID_MENU_INVISIBLE, ID_MENU_IMOFFLINE, 
		ID_MENU_LOCK, ID_MENU_GROUP_HOMEPAGE};
	CString strFileNames[] = {_T("Status\\imonline.png"), _T("Status\\Qme.png"), 
		_T("Status\\away.png"), _T("Status\\busy.png"), _T("Status\\mute.png"), 
		_T("Status\\invisible.png"), _T("Status\\imoffline.png"), _T("lock20.png"),
		_T("groupmainpage.png")};

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(0);
	for (int i = 0; i < 9; i++)
	{
		PopupMenu.SetIcon(dwMenuIDs[i], FALSE, strFileNames[i], strFileNames[i]);
	}

	PopupMenu = m_SkinMenu.GetSubMenu(2);
	for (int i = 0; i < 9; i++)
	{
		PopupMenu.SetIcon(dwMenuIDs[i], FALSE, strFileNames[i], strFileNames[i]);
	}

	PopupMenu = m_SkinMenu.GetSubMenu(3);
	PopupMenu.SetIcon(0, TRUE, _T("modehead.png"), _T("modehead.png"));
	PopupMenu = m_SkinMenu.GetSubMenu(4);
	PopupMenu.SetIcon(0, TRUE, _T("modehead.png"), _T("modehead.png"));
	PopupMenu = m_SkinMenu.GetSubMenu(5);
	PopupMenu.SetIcon(0, TRUE, _T("modehead.png"), _T("modehead.png"));
	PopupMenu = m_SkinMenu.GetSubMenu(6);
	PopupMenu.SetIcon(0, TRUE, _T("modehead.png"), _T("modehead.png"));

	PopupMenu = m_SkinMenu.GetSubMenu(0);

	m_btnStatus.SetButtonType(SKIN_MENU_BUTTON);
	m_btnStatus.SetTransparent(TRUE, hDlgBgDC);
	m_btnStatus.SetBgPic(NULL, _T("menubtn_highlight.png"), _T("menubtn_highlight.png"), NULL);
	m_btnStatus.SetArrowPic(_T("MainPanel_FolderNode_expandTexture.png"));
	m_btnStatus.SetIconPic(_T("Status\\imonline.png"));
	m_btnStatus.SubclassWindow(GetDlgItem(ID_BTN_STATUS));
	m_btnStatus.MoveWindow(54, 30, 35, 22, TRUE);
	m_btnStatus.SetMenu(PopupMenu.m_hMenu);

	m_edtSign.SetBgNormalPic(_T("SignEditBg.png"), CRect(1,1,1,1));
	m_edtSign.SubclassWindow(GetDlgItem(ID_EDIT_SIGN));
	m_edtSign.MoveWindow(56, 54, rcClient.Width()-102, 22, TRUE);
	m_edtSign.ShowWindow(SW_HIDE);

	m_edtSearch.SetBgNormalPic(_T("SearchBar\\bg.png"), CRect(3,3,3,3));
	m_edtSearch.SetIconPic(_T("SearchBar\\main_search_normal.png"));
	m_edtSearch.SetDefaultText(_T("������ϵ�ˣ���ҳ��Ϣ�����ʴ�"));
	m_edtSearch.SubclassWindow(GetDlgItem(ID_EDIT_SEARCH));
	m_edtSearch.MoveWindow(3, 104, rcClient.Width()-6, 27, TRUE);

	m_btnMsgBox.SetButtonType(SKIN_ICON_BUTTON);
	m_btnMsgBox.SetTransparent(TRUE, hDlgBgDC);
	m_btnMsgBox.SetBgPic(NULL, _T("allbtn_highlight.png"), _T("allbtn_down.png"), NULL);
	m_btnMsgBox.SetIconPic(_T("MsgBox.png"));
	m_btnMsgBox.SubclassWindow(GetDlgItem(ID_BTN_MSG_BOX));
	m_btnMsgBox.MoveWindow(rcClient.Width()-48, 80, 22, 20, TRUE);
	m_btnMsgBox.SetToolTipText(_T("����Ϣ����"));

	m_btnChangeExterior.SetButtonType(SKIN_ICON_BUTTON);
	m_btnChangeExterior.SetTransparent(TRUE, hDlgBgDC);
	m_btnChangeExterior.SetBgPic(NULL, _T("allbtn_highlight.png"), _T("allbtn_down.png"), NULL);
	m_btnChangeExterior.SetIconPic(_T("colour.png"));
	m_btnChangeExterior.SubclassWindow(GetDlgItem(ID_BTN_CHANGE_EXTERIOR));
	m_btnChangeExterior.MoveWindow(rcClient.Width()-26, 80, 20, 20, TRUE);
	m_btnChangeExterior.SetToolTipText(_T("�������"));

	m_btnAppMgr.SetButtonType(SKIN_ICON_BUTTON);
	m_btnAppMgr.SetTransparent(TRUE, hDlgBgDC);
	m_btnAppMgr.SetBgPic(NULL, _T("allbtn_highlight.png"), _T("allbtn_down.png"), NULL);
	m_btnAppMgr.SetIconPic(_T("appbox_mgr_btn.png"));
	m_btnAppMgr.SubclassWindow(GetDlgItem(ID_BTN_APP_MGR));
	m_btnAppMgr.MoveWindow(rcClient.Width()-38, rcClient.bottom-60, 22, 22, TRUE);
	m_btnAppMgr.SetToolTipText(_T("��Ӧ�ù�����"));

	m_picLogining.SetTransparent(TRUE, hDlgBgDC);
	m_picLogining.SubclassWindow(GetDlgItem(ID_PIC_LOGINING));
	m_picLogining.MoveWindow((rcClient.Width() - 220) / 2, 76, 220, 150, TRUE);

	m_staQQNum.SetTransparent(TRUE, hDlgBgDC);
	m_staQQNum.SubclassWindow(GetDlgItem(ID_STATIC_QQNUM));
	m_staQQNum.MoveWindow(rcClient.left, 226, rcClient.Width(), 14, FALSE);

	m_staLogining.SetTransparent(TRUE, hDlgBgDC);
	m_staLogining.SubclassWindow(GetDlgItem(ID_STATIC_LOGINING));
	m_staLogining.MoveWindow(rcClient.left, 240, rcClient.Width(), 16, FALSE);

	m_btnCancel.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnCancel.SetTransparent(TRUE, hDlgBgDC);
	m_btnCancel.SetBgPic(_T("Button\\login_btn_normal.png"), _T("Button\\login_btn_highlight.png"),
		_T("Button\\login_btn_down.png"), _T("Button\\login_btn_focus.png"));
	m_btnCancel.SetRound(4, 4);
	m_btnCancel.SubclassWindow(GetDlgItem(ID_BTN_CANCEL));
	m_btnCancel.MoveWindow((rcClient.Width() - 86) / 2, 304, 86, 30, TRUE);

	InitTopToolBar();		// ��ʼ��Top������
	InitBottomToolBar();	// ��ʼ��Bottom������
	InitTabCtrl();			// ��ʼ��Tab��
	InitBuddyListCtrl();	// ��ʼ�������б�ؼ�
	InitGroupListCtrl();	// ��ʼ��Ⱥ�б�ؼ�
	InitRecentListCtrl();	// ��ʼ�������ϵ���б�ؼ�

	return TRUE;
}

void CMainDlg::UnInit()
{

}

// ��ʾָ����庯��(bShow��TRUE��ʾ��ʾ����壬FALSE��ʾ��ʾ��¼���)
void CMainDlg::ShowPanel(BOOL bShow)
{
	int nShow = bShow ? SW_HIDE : SW_SHOW;
	m_picLogining.ShowWindow(nShow);
	m_staQQNum.ShowWindow(nShow);
	m_staLogining.ShowWindow(nShow);
	m_btnCancel.ShowWindow(nShow);

	nShow = bShow ? SW_SHOW : SW_HIDE;
	m_btnMainMenu.ShowWindow(nShow);
	m_btnSystemSet.ShowWindow(nShow);
	m_btnMsgMgr.ShowWindow(nShow);
	m_btnSafe.ShowWindow(nShow);
	m_btnFind.ShowWindow(nShow);
	m_btnStatus.ShowWindow(nShow);
	m_edtSearch.ShowWindow(nShow);
	m_picHead.ShowWindow(nShow);
	m_staNickName.ShowWindow(nShow);
	m_btnSign.ShowWindow(nShow);
	m_tbTop.ShowWindow(nShow);
	m_tbBottom.ShowWindow(nShow);
	m_btnMsgBox.ShowWindow(nShow);
	m_btnChangeExterior.ShowWindow(nShow);
	m_btnAppMgr.ShowWindow(nShow);;
	m_TabCtrl.ShowWindow(nShow);
	m_BuddyListCtrl.ShowWindow(nShow);

	if (bShow)	// ��ʾ�����
	{
		m_SkinDlg.SetBgPic(_T("main_panel_bg.png"), CRect(2, 135, 2, 67));
		m_picLogining.SetBitmap(NULL, TRUE);
		StatusMenuBtn_SetIconPic(m_btnStatus, m_QQClient.GetStatus());
	}
	else		// ��ʾ��¼���
	{
		m_SkinDlg.SetBgPic(_T("LoginPanel_window_windowBkg.png"), CRect(4, 65, 4, 4));

		tstring strFileName = ZYM::CPath::GetAppPath() + _T("Image\\Logining.gif");
		m_picLogining.SetBitmap(strFileName.c_str(), TRUE);

		m_staQQNum.SetWindowText(m_stAccountInfo.szUser);
		m_staLogining.SetWindowText(_T("���ڵ�¼"));
	}
}

void CMainDlg::StartLogin(BOOL bAutoLogin/* = FALSE*/)
{
	if (bAutoLogin)
	{
		BOOL bRet = m_LoginAccountList.GetLastLoginAccountInfo(&m_stAccountInfo);
		if (!bRet)
			return;
	}
	else
	{
		LoadAppIcon(m_QQClient.GetStatus());
		m_TrayIcon.AddIcon(m_hWnd, WM_TRAYICON_NOTIFY, 1, m_hAppIcon, _T("MingQQ"));

		ShowWindow(SW_HIDE);

		m_LoginDlg.m_lpQQClient = &m_QQClient;
		m_LoginDlg.m_pLoginAccountList = &m_LoginAccountList;
		if (m_LoginDlg.DoModal(m_hWnd) != IDOK)	// ��ʾ��¼�Ի���
		{
			CloseDialog(IDOK);
			return;
		}
		m_LoginDlg.GetLoginAccountInfo(&m_stAccountInfo);
	}

	ShowPanel(FALSE);		// ��ʾ��¼���
	ShowWindow(SW_SHOW);

	LoadLoginIcon();
	m_dwLoginTimerId = SetTimer(1, 200, NULL);

	m_QQClient.SetUser(m_stAccountInfo.szUser, m_stAccountInfo.szPwd);
	m_QQClient.SetLoginStatus((QQ_STATUS)m_stAccountInfo.nStatus);
	m_QQClient.SetCallBackWnd(m_hWnd);
	m_QQClient.Login();
}

LRESULT CMainDlg::OnTrayIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// wParamΪuID, lParamΪ�����Ϣ
	m_TrayIcon.OnTrayIconNotify(wParam, lParam);

	UINT uID = (UINT)wParam;
	UINT uIconMsg = (UINT)lParam;

	switch (uIconMsg)
	{
	case WM_LBUTTONUP:
		OnTrayIcon_LButtunUp();
		break;
	case WM_RBUTTONUP:
		OnTrayIcon_RButtunUp();
		break;
	case WM_MOUSEHOVER:
		OnTrayIcon_MouseHover();
		break;
	case WM_MOUSELEAVE:
		OnTrayIcon_MouseLeave();
		break;
	}
	return 0;
}

void CMainDlg::OnMenu_ShowMainPanel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (m_LoginDlg.IsWindow())
	{
		m_LoginDlg.ShowWindow(SW_SHOW);
		m_LoginDlg.SetFocus();
	}
	else if (IsWindow())
	{
		ShowWindow(SW_SHOW);
		SetFocus();
	}
}

void CMainDlg::OnMenu_Exit(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (m_LoginDlg.IsWindow())
		m_LoginDlg.PostMessage(WM_CLOSE);
	else if (IsWindow())
		PostMessage(WM_CLOSE);
}

void CMainDlg::OnBtn_Sign(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strText;
	m_btnSign.GetWindowText(strText);
	m_edtSign.SetWindowText(strText);

	m_btnSign.ShowWindow(SW_HIDE);
	m_edtSign.ShowWindow(SW_SHOW);

	m_edtSign.SetSel(0, -1);
	m_edtSign.SetFocus();
}

LRESULT CMainDlg::OnTabCtrlDropDown(LPNMHDR pnmh)
{
	CSkinMenu PopupMenu;
	CRect rcItem;
	int nCurSel, nIndex;

	nCurSel = m_TabCtrl.GetCurSel();

	switch (nCurSel)
	{
	case 0:
		nIndex = 3;
		break;

	case 1:
		nIndex = 4;
		break;

	case 2:
		nIndex = 5;
		break;

	case 4:
		nIndex = 6;
		break;

	default:
		return 0;
	}

	PopupMenu = m_SkinMenu.GetSubMenu(nIndex);
	if (PopupMenu.IsMenu())
	{
		m_TabCtrl.GetItemRectByIndex(nCurSel, rcItem);
		m_TabCtrl.ClientToScreen(&rcItem);
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, 
			rcItem.right-19, rcItem.bottom + 4, m_hWnd, &rcItem);
	}

	return 0;
}

LRESULT CMainDlg::OnBuddyListDblClk(LPNMHDR pnmh)
{
	int nTeamIndex, nIndex;
	m_BuddyListCtrl.GetCurSelIndex(nTeamIndex, nIndex);

	if (nTeamIndex != -1 && nIndex != -1)
	{
		UINT nQQUin = m_BuddyListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
		SendMessage(WM_SHOW_BUDDYCHATDLG, 0, nQQUin);
	}	
	return 0;
}

LRESULT CMainDlg::OnGroupListDblClk(LPNMHDR pnmh)
{
	int nTeamIndex, nIndex;
	m_GroupListCtrl.GetCurSelIndex(nTeamIndex, nIndex);

	if (nTeamIndex != -1 && nIndex != -1)
	{
		UINT nGroupCode = m_GroupListCtrl.GetBuddyItemID(nTeamIndex, nIndex);
		SendMessage(WM_SHOW_GROUPCHATDLG, nGroupCode, 0);
	}	
	return 0;
}

LRESULT CMainDlg::OnRecentListDblClk(LPNMHDR pnmh)
{
	int nTeamIndex, nIndex;
	m_RecentListCtrl.GetCurSelIndex(nTeamIndex, nIndex);

	if (nTeamIndex != -1 && nIndex != -1)
	{
		int nIndex2 = m_RecentListCtrl.GetBuddyItemID(nTeamIndex, nIndex);

		CRecentList * lpRecentList = m_QQClient.GetRecentList();
		if (lpRecentList != NULL)
		{
			CRecentInfo * lpRecentInfo = lpRecentList->GetRecent(nIndex2);
			if (lpRecentInfo != NULL)
			{
				if (0 == lpRecentInfo->m_nType)			// ����
				{
					SendMessage(WM_SHOW_BUDDYCHATDLG, 0, lpRecentInfo->m_nQQUin);
				}
				else if (1 == lpRecentInfo->m_nType)	// Ⱥ
				{
					CGroupList * lpGroupList = m_QQClient.GetGroupList();
					if (lpGroupList != NULL)
					{
						CGroupInfo * lpGroupInfo = lpGroupList->GetGroupById(lpRecentInfo->m_nQQUin);
						if (lpGroupInfo != NULL)
						{
							SendMessage(WM_SHOW_GROUPCHATDLG, lpGroupInfo->m_nGroupCode, 0);
						}
					}
				}
			}
		}
	}
	return 0;
}

LRESULT CMainDlg::OnTabCtrlSelChange(LPNMHDR pnmh)
{
	int nCurSel = m_TabCtrl.GetCurSel();
	switch (nCurSel)
	{
	case 0:
		m_BuddyListCtrl.ShowWindow(SW_SHOW);
		m_GroupListCtrl.ShowWindow(SW_HIDE);
		m_RecentListCtrl.ShowWindow(SW_HIDE);
		m_BuddyListCtrl.SetFocus();
		break;

	case 2:
		m_BuddyListCtrl.ShowWindow(SW_HIDE);
		m_GroupListCtrl.ShowWindow(SW_SHOW);
		m_RecentListCtrl.ShowWindow(SW_HIDE);
		m_GroupListCtrl.SetFocus();
		break;

	case 4:
		m_BuddyListCtrl.ShowWindow(SW_HIDE);
		m_GroupListCtrl.ShowWindow(SW_HIDE);
		m_RecentListCtrl.ShowWindow(SW_SHOW);
		m_RecentListCtrl.SetFocus();
		break;

	case 1:
	case 3:
		m_BuddyListCtrl.ShowWindow(SW_HIDE);
		m_GroupListCtrl.ShowWindow(SW_HIDE);
		m_RecentListCtrl.ShowWindow(SW_HIDE);
		break;
	}

	return 0;
}

// ��ȡ������ť(ȡ����¼)
void CMainDlg::OnBtn_Cancel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_QQClient.CancelLogin();
}

// ���û�ͷ�񡱿ؼ�
void CMainDlg::OnPic_Clicked_Head(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CBuddyInfo * lpBuddyInfo = m_QQClient.GetUserInfo();
	if (lpBuddyInfo != NULL)
		ShowBuddyInfoDlg(lpBuddyInfo->m_nQQUin, TRUE);
}

// ����ͷ�񡱲˵�
void CMainDlg::OnMenu_BigHeadPic(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCurSel, nIndex;
	CBuddyListCtrl * lpBuddyListCtrl = NULL;

	nCurSel = m_TabCtrl.GetCurSel();
	if (0 == nCurSel)
	{
		nIndex = 3;
		lpBuddyListCtrl = &m_BuddyListCtrl;
	}
	else if (2 == nCurSel)
	{
		nIndex = 5;
		lpBuddyListCtrl = &m_GroupListCtrl;
	}
	else if (4 == nCurSel)
	{
		nIndex = 6;
		lpBuddyListCtrl = &m_RecentListCtrl;
	}

	if (NULL == lpBuddyListCtrl || lpBuddyListCtrl->GetStyle() == BLC_BIG_ICON_STYLE)
		return;

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(nIndex);
	PopupMenu.CheckMenuRadioItem(ID_MENU_BIGHEADPIC, 
		ID_MENU_STDHEADPIC, ID_MENU_BIGHEADPIC, MF_BYCOMMAND);
	PopupMenu.CheckMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_UNCHECKED|MF_BYCOMMAND);
	PopupMenu.EnableMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_GRAYED|MF_BYCOMMAND);

	lpBuddyListCtrl->SetStyle(BLC_BIG_ICON_STYLE);
	lpBuddyListCtrl->Invalidate();
}

// ��Сͷ�񡱲˵�
void CMainDlg::OnMenu_SmallHeadPic(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCurSel, nIndex;
	UINT nCheck;
	CBuddyListCtrl * lpBuddyListCtrl = NULL;

	nCurSel = m_TabCtrl.GetCurSel();
	if (0 == nCurSel)
	{
		nIndex = 3;
		lpBuddyListCtrl = &m_BuddyListCtrl;
	}
	else if (2 == nCurSel)
	{
		nIndex = 5;
		lpBuddyListCtrl = &m_GroupListCtrl;
	}
	else if (4 == nCurSel)
	{
		nIndex = 6;
		lpBuddyListCtrl = &m_RecentListCtrl;
	}

	if (NULL == lpBuddyListCtrl || lpBuddyListCtrl->GetStyle() == BLC_SMALL_ICON_STYLE)
		return;

	nCheck = lpBuddyListCtrl->IsShowBigIconInSel() ? MF_CHECKED : MF_UNCHECKED;

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(nIndex);
	PopupMenu.CheckMenuRadioItem(ID_MENU_BIGHEADPIC, 
		ID_MENU_STDHEADPIC, ID_MENU_SMALLHEADPIC, MF_BYCOMMAND);
	PopupMenu.EnableMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_ENABLED|MF_BYCOMMAND);
	PopupMenu.CheckMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, nCheck|MF_BYCOMMAND);

	lpBuddyListCtrl->SetStyle(BLC_SMALL_ICON_STYLE);
	lpBuddyListCtrl->Invalidate();
}

// ����׼ͷ�񡱲˵�
void CMainDlg::OnMenu_StdHeadPic(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCurSel, nIndex;
	CBuddyListCtrl * lpBuddyListCtrl = NULL;

	nCurSel = m_TabCtrl.GetCurSel();
	if (0 == nCurSel)
	{
		nIndex = 3;
		lpBuddyListCtrl = &m_BuddyListCtrl;
	}
	else if (2 == nCurSel)
	{
		nIndex = 5;
		lpBuddyListCtrl = &m_GroupListCtrl;
	}
	else if (4 == nCurSel)
	{
		nIndex = 6;
		lpBuddyListCtrl = &m_RecentListCtrl;
	}

	if (NULL == lpBuddyListCtrl || lpBuddyListCtrl->GetStyle() == BLC_STANDARD_ICON_STYLE)
		return;

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(nIndex);
	PopupMenu.CheckMenuRadioItem(ID_MENU_BIGHEADPIC, 
		ID_MENU_STDHEADPIC, ID_MENU_STDHEADPIC, MF_BYCOMMAND);
	PopupMenu.CheckMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_UNCHECKED|MF_BYCOMMAND);
	PopupMenu.EnableMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_GRAYED|MF_BYCOMMAND);

	lpBuddyListCtrl->SetStyle(BLC_STANDARD_ICON_STYLE);
	lpBuddyListCtrl->Invalidate();
}

// ��ѡ��ʱ��ʾ��ͷ�񡱲˵�
void CMainDlg::OnMenu_ShowBigHeadPicInSel(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	int nCurSel, nIndex;
	CBuddyListCtrl * lpBuddyListCtrl = NULL;

	nCurSel = m_TabCtrl.GetCurSel();
	if (0 == nCurSel)
	{
		nIndex = 3;
		lpBuddyListCtrl = &m_BuddyListCtrl;
	}
	else if (2 == nCurSel)
	{
		nIndex = 5;
		lpBuddyListCtrl = &m_GroupListCtrl;
	}
	else if (4 == nCurSel)
	{
		nIndex = 6;
		lpBuddyListCtrl = &m_RecentListCtrl;
	}

	if (NULL == lpBuddyListCtrl)
		return;

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(nIndex);
	UINT nState = PopupMenu.GetMenuState(ID_MENU_SHOWBIGHEADPICINSEL, MF_BYCOMMAND);
	if (nState & MF_CHECKED)
	{
		PopupMenu.CheckMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_UNCHECKED|MF_BYCOMMAND);
		lpBuddyListCtrl->SetShowBigIconInSel(FALSE);
	}
	else
	{
		PopupMenu.CheckMenuItem(ID_MENU_SHOWBIGHEADPICINSEL, MF_CHECKED|MF_BYCOMMAND);
		lpBuddyListCtrl->SetShowBigIconInSel(TRUE);
	}
	lpBuddyListCtrl->Invalidate();
}

// ������ǩ�����༭�ı���
void CMainDlg::OnEdit_Sign_KillFocus(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CString strOldText, strNewText;
	m_btnSign.GetWindowText(strOldText);
	m_edtSign.GetWindowText(strNewText);
	if (strOldText != strNewText)
	{
		m_QQClient.ModifyQQSign(strNewText);
		m_btnSign.SetWindowText(strNewText);
	}
	m_edtSign.ShowWindow(SW_HIDE);
	m_btnSign.ShowWindow(SW_SHOW);
}

// ���û�����״̬���˵�
void CMainDlg::OnMenu_Status(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	QQ_STATUS nNewStatus = GetStatusFromMenuID(nID);
	if (nNewStatus != m_QQClient.GetStatus())	// ���ߺ����µ�¼��ʱδ����
	{
		LoadAppIcon(nNewStatus);
		if (NULL == m_dwMsgTimerId)
			m_TrayIcon.ModifyIcon(m_hAppIcon, _T("MingQQ"));
		m_QQClient.ChangeStatus(nNewStatus);

		StatusMenuBtn_SetIconPic(m_btnStatus, nNewStatus);
	}
}

// ��¼������Ϣ
LRESULT CMainDlg::OnLoginResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	QQ_LOGIN_RESULT_CODE nCode = (QQ_LOGIN_RESULT_CODE)lParam;

 	KillTimer(m_dwLoginTimerId);
 	m_dwLoginTimerId = NULL;

	QQ_STATUS nStatus = m_QQClient.GetStatus();

 	LoadAppIcon(nStatus);
 	m_TrayIcon.ModifyIcon(m_hAppIcon, _T("MingQQ"));

	switch (nCode)
	{
	case QLRC_SUCCESS:				// ��¼�ɹ�
		{
			ShowPanel(TRUE);
			ShowWindow(SW_SHOW);
			Invalidate();

			// �����¼�ʺ��б�
			LOGIN_ACCOUNT_INFO * lpAccount = m_LoginAccountList.Find(m_stAccountInfo.szUser);
			if (lpAccount != NULL)
				memcpy(lpAccount, &m_stAccountInfo, sizeof(LOGIN_ACCOUNT_INFO));
			else
				m_LoginAccountList.Add(m_stAccountInfo.szUser, m_stAccountInfo.szPwd,
					m_stAccountInfo.nStatus, m_stAccountInfo.bRememberPwd, m_stAccountInfo.bAutoLogin);
			m_LoginAccountList.SetLastLoginUser(m_stAccountInfo.szUser);
			tstring strFileName = ZYM::CPath::GetAppPath()+_T("LoginAccountList.dat");
			m_LoginAccountList.SaveFile(strFileName.c_str());

			// ������������
			strFileName = m_QQClient.GetPersonalFolder()+_T("MiscConfig.xml");
			LoadMiscConfig(strFileName.c_str());

			// ע����ȡ��Ϣ�ȼ�(Ctrl+Alt+X)
			::RegisterHotKey(m_hWnd, 1001, MOD_CONTROL|MOD_ALT, g_cHotKey);
		}
		break;

	case QLRC_FAILED:				// ��¼ʧ��
		{
			MessageBox(_T("��¼ʧ��!"), _T("��ʾ"), MB_OK);
			StartLogin();
		}
		break;

	case QLRC_PASSWORD_ERROR:		// �������
		{
			MessageBox(_T("�������!"), _T("��ʾ"), MB_OK);
			StartLogin();
		}
		break;

	case QLRC_VERIFY_CODE_ERROR:	// ��֤�����
		{
			MessageBox(_T("��֤�����!"), _T("��ʾ"), MB_OK);
		}
	case QLRC_NEED_VERIFY_CODE:		// ��Ҫ������֤��
		{
			const BYTE * lpData = NULL;
			DWORD dwSize = 0;
			BOOL bRet = m_QQClient.GetVerifyCodePic(lpData, dwSize);
			if (!bRet)
			{
				MessageBox(_T("��ȡ��֤��ͼƬʧ��!"), _T("��ʾ"), MB_OK);
				StartLogin();
				break;
			}

			CVerifyCodeDlg VerifyCodeDlg;
			VerifyCodeDlg.SetVerifyCodePic(lpData, dwSize);
			if (VerifyCodeDlg.DoModal() == IDOK)
			{
				LoadLoginIcon();
				m_dwLoginTimerId = SetTimer(1, 200, NULL);

				m_QQClient.SetVerifyCode(VerifyCodeDlg.GetVerifyCode());
				m_QQClient.Login();
			}
			else
			{
				StartLogin();
			}
		}
		break;

	case QLRC_USER_CANCEL_LOGIN:	// �û�ȡ����¼
		{
			StartLogin();
		}
		break;
	}

	return 0;
}

// ע��������Ϣ
LRESULT CMainDlg::OnLogoutResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CloseDialog(IDOK);
	return 0;
}

// �����û���Ϣ
LRESULT CMainDlg::OnUpdateUserInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CBuddyInfoResult * lpBuddyInfoResult = (CBuddyInfoResult *)lParam;
	if (NULL == lpBuddyInfoResult)
		return 0;
	//m_QQClient.OnUpdateUserInfo(lpBuddyInfoResult);
	delete lpBuddyInfoResult;

	return 0;
}

// ���º����б���Ϣ
LRESULT CMainDlg::OnUpdateBuddyList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bSuccess = (BOOL)lParam;
	if (!bSuccess)
		return 0;

	UpdateBuddyTreeCtrl();		// ���º����б�ؼ�

	return 0;
}

// ����Ⱥ�б���Ϣ
LRESULT CMainDlg::OnUpdateGroupList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bSuccess = (BOOL)lParam;
	if (!bSuccess)
		return 0;

	UpdateGroupTreeCtrl();		// ����Ⱥ�б�ؼ�

	return 0;
}

// ���������ϵ���б���Ϣ
LRESULT CMainDlg::OnUpdateRecentList(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bSuccess = (BOOL)lParam;
	if (!bSuccess)
		return 0;

	UpdateRecentTreeCtrl();		// ���������ϵ���б�ؼ�

	return 0;
}

// ������Ϣ
LRESULT CMainDlg::OnBuddyMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nQQUin = (UINT)wParam;
	UINT nMsgId = (UINT)lParam;
	if (0 == nQQUin || 0 == nMsgId)
		return 0;

	tstring strFileName = ZYM::CPath::GetAppPath() + _T("Sound\\msg.wav");	// ��������Ϣ��ʾ��
	::sndPlaySound(strFileName.c_str(), SND_ASYNC);

	std::map<UINT, CBuddyChatDlg *>::iterator iter;
	iter = m_mapBuddyChatDlg.find(nQQUin);
	if (iter != m_mapBuddyChatDlg.end())
	{
		CBuddyChatDlg * lpBuddyDlg = iter->second;
		if (lpBuddyDlg != NULL && lpBuddyDlg->IsWindow())
		{
			lpBuddyDlg->OnRecvMsg(nQQUin, nMsgId);
			return 0;
		}
	}

	int nTeamIndex, nIndex;
	m_BuddyListCtrl.GetItemIndexByID(nQQUin, nTeamIndex, nIndex);
	m_BuddyListCtrl.SetBuddyItemHeadFlashAnim(nTeamIndex, nIndex, TRUE);

	UpdateMsgIcon();

	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.AddMsgSender(QQ_MSG_TYPE_BUDDY, nQQUin);

	if (NULL == m_dwMsgTimerId)
		m_dwMsgTimerId = SetTimer(2, ::GetCaretBlinkTime(), NULL);

	return 0;
}

// Ⱥ��Ϣ
LRESULT CMainDlg::OnGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nMsgId = (UINT)lParam;
	if (0 == nGroupCode || 0 == nMsgId)
		return 0;

	std::map<UINT, CGroupChatDlg *>::iterator iter;
	iter = m_mapGroupChatDlg.find(nGroupCode);
	if (iter != m_mapGroupChatDlg.end())
	{
		CGroupChatDlg * lpGroupDlg = iter->second;
		if (lpGroupDlg != NULL && lpGroupDlg->IsWindow())
		{
			lpGroupDlg->OnRecvMsg(nGroupCode, nMsgId);
			return 0;
		}
	}

	CMessageList * lpMsgList = m_QQClient.GetMessageList();
	if (lpMsgList != NULL)
	{
		CMessageSender * lpMsgSender = lpMsgList->GetMsgSender(QQ_MSG_TYPE_GROUP, nGroupCode);
		if (lpMsgSender != NULL)
		{
			if (lpMsgSender->GetMsgCount() == 1)
			{
				tstring strFileName = ZYM::CPath::GetAppPath() + _T("Sound\\msg.wav");	// ��������Ϣ��ʾ��
				::sndPlaySound(strFileName.c_str(), SND_ASYNC);
			}
		}
	}

	int nTeamIndex, nIndex;
	m_GroupListCtrl.GetItemIndexByID(nGroupCode, nTeamIndex, nIndex);
	m_GroupListCtrl.SetBuddyItemHeadFlashAnim(nTeamIndex, nIndex, TRUE);

	UpdateMsgIcon();

	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.AddMsgSender(QQ_MSG_TYPE_GROUP, nGroupCode);

	if (NULL == m_dwMsgTimerId)
		m_dwMsgTimerId = SetTimer(2, ::GetCaretBlinkTime(), NULL);

	return 0;
}

// ��ʱ�Ự��Ϣ
LRESULT CMainDlg::OnSessMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nQQUin = (UINT)wParam;
	UINT nMsgId = (UINT)lParam;
	if (0 == nQQUin || 0 == nMsgId)
		return 0;

	tstring strFileName = ZYM::CPath::GetAppPath() + _T("Sound\\msg.wav");	// ��������Ϣ��ʾ��
	::sndPlaySound(strFileName.c_str(), SND_ASYNC);

	std::map<UINT, CSessChatDlg *>::iterator iter;
	iter = m_mapSessChatDlg.find(nQQUin);
	if (iter != m_mapSessChatDlg.end())
	{
		CSessChatDlg * lpSessChatDlg = iter->second;
		if (lpSessChatDlg != NULL && lpSessChatDlg->IsWindow())
		{
			lpSessChatDlg->OnRecvMsg(nQQUin, nMsgId);
			return 0;
		}
	}

	UpdateMsgIcon();

	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.AddMsgSender(QQ_MSG_TYPE_SESS, nQQUin);

	if (NULL == m_dwMsgTimerId)
		m_dwMsgTimerId = SetTimer(2, ::GetCaretBlinkTime(), NULL);

	return 0;
}

// ����״̬�ı���Ϣ
LRESULT CMainDlg::OnStatusChangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nQQUin = (UINT)lParam;
	if (0 == nQQUin)
		return 0;

	CBuddyList * lpBuddyList = m_QQClient.GetBuddyList();
	if (NULL == lpBuddyList)
		return 0;

	CBuddyInfo * lpBuddyInfo = lpBuddyList->GetBuddy(nQQUin);
	if (NULL == lpBuddyInfo)
		return 0;

	CBuddyTeamInfo * lpBuddyTeam = lpBuddyList->GetBuddyTeam(lpBuddyInfo->m_nTeamIndex);
	if (NULL == lpBuddyTeam)
		return 0;

	int nOnlineCnt = lpBuddyTeam->GetOnlineBuddyCount();
	BOOL bOnline = (lpBuddyInfo->m_nStatus != QQ_STATUS_OFFLINE ? TRUE : FALSE);

	if (bOnline)	// ���ź�����������
	{
		tstring strFileName = ZYM::CPath::GetAppPath() + _T("Sound\\Global.wav");
		::sndPlaySound(strFileName.c_str(), SND_ASYNC);
	}

	int nTeamIndex, nIndex;	// ��ȡ����������
	m_BuddyListCtrl.GetItemIndexByID(nQQUin, nTeamIndex, nIndex);

	// ���º��ѷ�����������
	m_BuddyListCtrl.SetBuddyTeamCurCnt(nTeamIndex, nOnlineCnt);

	// ���º���ͷ��
	tstring strFileName = m_QQClient.GetBuddyHeadPicFullName(lpBuddyInfo->m_nQQNum);
	if (!ZYM::CPath::IsFileExist(strFileName.c_str()))
		strFileName = ZYM::CPath::GetAppPath() + _T("Image\\DefBuddyHeadPic.png");
	m_BuddyListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), !bOnline);

	// ���ú�������״̬���������߶���
	m_BuddyListCtrl.SetBuddyItemOnline(nTeamIndex, nIndex, bOnline, TRUE);

	m_BuddyListCtrl.Invalidate();	// ˢ�º����б�ؼ�

	return 0;
}

// ����������Ϣ
LRESULT CMainDlg::OnKickMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LPCTSTR lpszReason = _T("�������ߣ�");
	MessageBox(lpszReason, _T("��ʾ"), MB_OK);
	PostMessage(WM_CLOSE);
	return 0;
}

// Ⱥϵͳ��Ϣ
LRESULT CMainDlg::OnSysGroupMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)lParam;
	if (0 == nGroupCode)
		return 0;

	tstring strFileName = ZYM::CPath::GetAppPath() + _T("Sound\\system.wav");	// ������ϵͳ��Ϣ��ʾ��
	::sndPlaySound(strFileName.c_str(), SND_ASYNC);

	UpdateMsgIcon();

	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.AddMsgSender(QQ_MSG_TYPE_SYSGROUP, nGroupCode);

	if (NULL == m_dwMsgTimerId)
		m_dwMsgTimerId = SetTimer(2, ::GetCaretBlinkTime(), NULL);

	return 0;
}

// ���º��Ѻ���
LRESULT CMainDlg::OnUpdateBuddyNumber(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nQQUin = (UINT)lParam;
	if (0 == nQQUin)
		return 0;

 	NotifyBuddyChatDlg(nQQUin, QQ_MSG_UPDATE_BUDDY_NUMBER);		// ֪ͨ�������촰�ڸ���
 	NotifyBuddyInfoDlg(nQQUin, QQ_MSG_UPDATE_BUDDY_NUMBER);		// ֪ͨ������Ϣ���ڸ���

	return 0;
}

// ����Ⱥ��Ա����
LRESULT CMainDlg::OnUpdateGMemberNumber(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nQQUin = (UINT)lParam;
	if (0 == nGroupCode || 0 == nQQUin)
		return 0;

 	NotifyGroupChatDlg(nGroupCode, QQ_MSG_UPDATE_GMEMBER_NUMBER, wParam, lParam);// ֪ͨȺ���촰�ڸ���
	NotifyGMemberInfoDlg(nGroupCode, nQQUin, QQ_MSG_UPDATE_GMEMBER_NUMBER);	// ֪ͨȺ��Ա��Ϣ���ڸ���
	NotifySessChatDlg(nQQUin, QQ_MSG_UPDATE_GMEMBER_NUMBER);				// ֪ͨȺ��Ա���촰�ڸ���

	return 0;
}

// ����Ⱥ����
LRESULT CMainDlg::OnUpdateGroupNumber(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)lParam;
	if (0 == nGroupCode)
		return 0;

 	NotifyGroupChatDlg(nGroupCode, QQ_MSG_UPDATE_GROUP_NUMBER, 0, 0);// ֪ͨȺ���촰�ڸ���
 	NotifyGroupInfoDlg(nGroupCode, QQ_MSG_UPDATE_GROUP_NUMBER);		// ֪ͨȺ��Ϣ���ڸ���

	return 0;
}

// ���º��Ѹ���ǩ��
LRESULT CMainDlg::OnUpdateBuddySign(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nQQUin = lParam;
	if (0 == nQQUin)
		return 0;

	CBuddyInfo * lpBuddyInfo = m_QQClient.GetUserInfo();
	if (lpBuddyInfo != NULL && lpBuddyInfo->m_nQQUin == nQQUin)	// �����û�����ǩ��
	{
		if (!lpBuddyInfo->m_strSign.empty())
		{
			m_btnSign.SetWindowText(lpBuddyInfo->m_strSign.c_str());
			m_btnSign.SetToolTipText(lpBuddyInfo->m_strSign.c_str());
		}
		else
		{
			m_btnSign.SetWindowText(_T("��һ����,ʲô��û�����¡�"));
			m_btnSign.SetToolTipText(_T("��һ����,ʲô��û�����¡�"));
		}
	}
	else	// ���º��Ѹ���ǩ��
	{
		CBuddyList * lpBuddyList = m_QQClient.GetBuddyList();		// ���º����б�ؼ��ĸ���ǩ��
		if (lpBuddyList != NULL)
		{
			CBuddyInfo * lpBuddyInfo = lpBuddyList->GetBuddy(nQQUin);
			if (lpBuddyInfo != NULL)
			{
				int nTeamIndex, nIndex;
				m_BuddyListCtrl.GetItemIndexByID(nQQUin, nTeamIndex, nIndex);
				m_BuddyListCtrl.SetBuddyItemSign(nTeamIndex, nIndex, lpBuddyInfo->m_strSign.c_str());
			}
		}

		NotifyBuddyChatDlg(nQQUin, QQ_MSG_UPDATE_BUDDY_SIGN);		// ֪ͨ�������촰�ڸ���
		NotifyBuddyInfoDlg(nQQUin, QQ_MSG_UPDATE_BUDDY_SIGN);		// ֪ͨ������Ϣ���ڸ���
	}

	return 0;
}

// ����Ⱥ��Ա����ǩ��
LRESULT CMainDlg::OnUpdateGMemberSign(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nQQUin = (UINT)lParam;
	if (0 == nGroupCode || 0 == nQQUin)
		return 0;

	NotifyGMemberInfoDlg(nGroupCode, nQQUin, QQ_MSG_UPDATE_GMEMBER_SIGN);	// ֪ͨȺ��Ա��Ϣ���ڸ���

	return 0;
}

// ���º�����Ϣ
LRESULT CMainDlg::OnUpdateBuddyInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nQQUin = lParam;
	if (0 == nQQUin)
		return 0;

 	CBuddyInfo * lpBuddyInfo = m_QQClient.GetUserInfo();
 	if (lpBuddyInfo != NULL && lpBuddyInfo->m_nQQUin == nQQUin)	// �����û��ǳ�
 		m_staNickName.SetWindowText(lpBuddyInfo->m_strNickName.c_str());
	else
		NotifyBuddyInfoDlg(nQQUin, QQ_MSG_UPDATE_BUDDY_INFO);		// ֪ͨ������Ϣ���ڸ���

	return 0;
}

// ����Ⱥ��Ա��Ϣ
LRESULT CMainDlg::OnUpdateGMemberInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nQQUin = (UINT)lParam;
	if (0 == nGroupCode || 0 == nQQUin)
		return 0;

	NotifyGMemberInfoDlg(nGroupCode, nQQUin, QQ_MSG_UPDATE_GMEMBER_INFO);	// ֪ͨȺ��Ա��Ϣ���ڸ���

	return 0;
}

// ����Ⱥ��Ϣ
LRESULT CMainDlg::OnUpdateGroupInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)lParam;
	if (0 == nGroupCode)
		return 0;

 	NotifyGroupChatDlg(nGroupCode, QQ_MSG_UPDATE_GROUP_INFO, 0, 0);	// ֪ͨȺ���촰�ڸ���
 	NotifyGroupInfoDlg(nGroupCode, QQ_MSG_UPDATE_GROUP_INFO);		// ֪ͨȺ��Ϣ���ڸ���

	return 0;
}

// HTREEITEM CMainDlg::FindTreeItemByQQUin(CTreeViewCtrl& treeCtrl, UINT nQQUin)
// {
// 	HTREEITEM hItem = treeCtrl.GetRootItem();
// 	while (hItem != NULL)
// 	{
// 		UINT nQQUin2 = (UINT)treeCtrl.GetItemData(hItem);
// 		if (nQQUin2 == nQQUin)
// 			return hItem;
// 
// 		HTREEITEM hFindItem = FindTreeItemByQQUin2(treeCtrl, hItem, nQQUin);
// 		if (hFindItem != NULL)
// 			return hFindItem;
// 
// 		hItem = treeCtrl.GetNextItem(hItem, TVGN_NEXT);
// 	}
// 	return NULL;
// }
// 
// HTREEITEM CMainDlg::FindTreeItemByQQUin2(CTreeViewCtrl& treeCtrl, HTREEITEM hItem, UINT nQQUin)
// {
// 	if (treeCtrl.ItemHasChildren(hItem))
// 	{
// 		HTREEITEM hChildItem = treeCtrl.GetChildItem(hItem);
// 
// 		while (hChildItem != NULL)
// 		{
// 			UINT nQQUin2 = (UINT)treeCtrl.GetItemData(hItem);
// 			if (nQQUin2 == nQQUin)
// 				return hItem;
// 
// 			HTREEITEM hFindItem = FindTreeItemByQQUin2(treeCtrl, hChildItem, nQQUin);
// 			if (hFindItem != NULL)
// 				return hFindItem;
// 
// 			hChildItem = treeCtrl.GetNextItem(hChildItem, TVGN_NEXT);
// 		}
// 	}
// 
// 	UINT nQQUin2 = (UINT)treeCtrl.GetItemData(hItem);
// 	return  (nQQUin2 == nQQUin) ? hItem : NULL;
// }

// ���º���ͷ��ͼƬ
LRESULT CMainDlg::OnUpdateBuddyHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nQQUin = (UINT)lParam;

	CBuddyInfo * lpBuddyInfo = m_QQClient.GetUserInfo();
	if (lpBuddyInfo != NULL && lpBuddyInfo->m_nQQUin == nQQUin)	// �����û�ͷ��
	{
		tstring strFileName = m_QQClient.GetUserHeadPicFullName(nQQUin);
		if (!ZYM::CPath::IsFileExist(strFileName.c_str()))
			strFileName = ZYM::CPath::GetAppPath() + _T("Image\\DefBuddyHeadPic.png");
		m_picHead.SetBitmap(strFileName.c_str());
		m_picHead.Invalidate();
		return 0;
	}

	// ���º����б�ͷ��
	UINT nQQNum = 0;
	BOOL bGray = FALSE;
	CBuddyList * lpBuddyList = m_QQClient.GetBuddyList();
	if (lpBuddyList != NULL)
	{
		CBuddyInfo * lpBuddyInfo = lpBuddyList->GetBuddy(nQQUin);
		if (lpBuddyInfo != NULL)
		{
			nQQNum = lpBuddyInfo->m_nQQNum;
			bGray = lpBuddyInfo->m_nStatus != QQ_STATUS_OFFLINE ? FALSE : TRUE;
		}
	}

	tstring strFileName = m_QQClient.GetBuddyHeadPicFullName(nQQNum);
	if (!ZYM::CPath::IsFileExist(strFileName.c_str()))
		strFileName = ZYM::CPath::GetAppPath() + _T("Image\\DefBuddyHeadPic.png");
	int nTeamIndex, nIndex;
	m_BuddyListCtrl.GetItemIndexByID(nQQUin, nTeamIndex, nIndex);
	m_BuddyListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), bGray);

	// ���������ϵ���б�ͷ��
	int nItemCnt = m_RecentListCtrl.GetBuddyItemCount(0);
	for (int i = 0; i < nItemCnt; i++)
	{
		int nIndex = m_RecentListCtrl.GetBuddyItemID(0, i);

		CRecentList * lpRecentList = m_QQClient.GetRecentList();
		if (lpRecentList != NULL)
		{
			CRecentInfo * lpRecentInfo = lpRecentList->GetRecent(nIndex);
			if (lpRecentInfo != NULL)
			{
				if (0 == lpRecentInfo->m_nType)			// ����
				{
					if (nQQUin == lpRecentInfo->m_nQQUin)
					{
						m_RecentListCtrl.GetItemIndexByID(nIndex, nTeamIndex, nIndex);
						m_RecentListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), bGray);
						break;
					}
				}
			}
		}
	}

	NotifyBuddyChatDlg(nQQUin, QQ_MSG_UPDATE_BUDDY_HEADPIC);		// ֪ͨ�������촰�ڸ���

	return 0;
}

// ����Ⱥ��Աͷ��ͼƬ
LRESULT CMainDlg::OnUpdateGMemberHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nQQUin = (UINT)lParam;

	NotifyGroupChatDlg(nGroupCode, QQ_MSG_UPDATE_GMEMBER_HEADPIC, wParam, lParam);	// ֪ͨȺ���촰�ڸ���
	NotifySessChatDlg(nQQUin, QQ_MSG_UPDATE_GMEMBER_HEADPIC);						// ֪ͨȺ��Ա���촰�ڸ���

	return 0;
}

// ����Ⱥͷ��ͼƬ
LRESULT CMainDlg::OnUpdateGroupHeadPic(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;

	// ����Ⱥ�б�ͷ��
	UINT nGroupNum = 0;
	CGroupList * lpGroupList = m_QQClient.GetGroupList();
	if (lpGroupList != NULL)
	{
		CGroupInfo * lpGroupInfo = lpGroupList->GetGroupByCode(nGroupCode);
		if (lpGroupInfo != NULL)
			nGroupNum = lpGroupInfo->m_nGroupNumber;
	}

	tstring strFileName = m_QQClient.GetGroupHeadPicFullName(nGroupNum);
	if (!ZYM::CPath::IsFileExist(strFileName.c_str()))
		strFileName = ZYM::CPath::GetAppPath() + _T("Image\\DefGroupHeadPic.jpg");
	int nTeamIndex, nIndex;
	m_GroupListCtrl.GetItemIndexByID(nGroupCode, nTeamIndex, nIndex);
	m_GroupListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), FALSE);

	// ���������ϵ���б�ͷ��
	int nItemCnt = m_RecentListCtrl.GetBuddyItemCount(0);
	for (int i = 0; i < nItemCnt; i++)
	{
		int nIndex = m_RecentListCtrl.GetBuddyItemID(0, i);

		CRecentList * lpRecentList = m_QQClient.GetRecentList();
		if (lpRecentList != NULL)
		{
			CRecentInfo * lpRecentInfo = lpRecentList->GetRecent(nIndex);
			if (lpRecentInfo != NULL)
			{
				if (1 == lpRecentInfo->m_nType)	// Ⱥ
				{
					CGroupList * lpGroupList = m_QQClient.GetGroupList();
					if (lpGroupList != NULL)
					{
						CGroupInfo * lpGroupInfo = lpGroupList->GetGroupById(lpRecentInfo->m_nQQUin);
						if (lpGroupInfo != NULL && nGroupCode == lpGroupInfo->m_nGroupCode)
						{
							m_RecentListCtrl.GetItemIndexByID(nIndex, nTeamIndex, nIndex);
							m_RecentListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), FALSE);
							break;
						}
					}
				}
			}
		}
	}

	NotifyGroupChatDlg(nGroupCode, QQ_MSG_UPDATE_GROUP_HEADPIC, wParam, lParam);// ֪ͨȺ���촰�ڸ���

	return 0;
}

// �ı�����״̬������Ϣ
LRESULT CMainDlg::OnChangeStatusResult(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	BOOL bSuccess = (BOOL)wParam;
	QQ_STATUS nNewStatus = (QQ_STATUS)lParam;
	if (!bSuccess)
		MessageBox(_T("�ı�����״̬ʧ�ܣ�"));
	return 0;
}

LRESULT CMainDlg::OnShowOrCloseDlg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UINT nGroupCode = (UINT)wParam;
	UINT nQQUin = (UINT)lParam;

	switch (uMsg)
	{
	case WM_SHOW_BUDDYCHATDLG:
		ShowBuddyChatDlg(nQQUin, TRUE);
		break;
	case WM_SHOW_GROUPCHATDLG:
		ShowGroupChatDlg(nGroupCode, TRUE);
		break;
	case WM_SHOW_SESSCHATDLG:
		ShowSessChatDlg(nGroupCode, nQQUin, TRUE);
		break;
	case WM_SHOW_SYSGROUPCHATDLG:
		ShowSysGroupChatDlg(nGroupCode, TRUE);
		break;
	case WM_SHOW_BUDDYINFODLG:
		ShowBuddyInfoDlg(nQQUin, TRUE);
		break;
	case WM_SHOW_GMEMBERINFODLG:
		ShowGMemberInfoDlg(nGroupCode, nQQUin, TRUE);
		break;
	case WM_SHOW_GROUPINFODLG:
		ShowGroupInfoDlg(nGroupCode, TRUE);
		break;
	case WM_CLOSE_BUDDYCHATDLG:
		ShowBuddyChatDlg(nQQUin, FALSE);
		break;
	case WM_CLOSE_GROUPCHATDLG:
		ShowGroupChatDlg(nGroupCode, FALSE);
		break;
	case WM_CLOSE_SESSCHATDLG:
		ShowSessChatDlg(nGroupCode, nQQUin, FALSE);
		break;
	case WM_CLOSE_SYSGROUPCHATDLG:
		ShowSysGroupChatDlg(nGroupCode, FALSE);
		break;
	case WM_CLOSE_BUDDYINFODLG:
		ShowBuddyInfoDlg(nQQUin, FALSE);
		break;
	case WM_CLOSE_GMEMBERINFODLG:
		ShowGMemberInfoDlg(nGroupCode, nQQUin, FALSE);
		break;
	case WM_CLOSE_GROUPINFODLG:
		ShowGroupInfoDlg(nGroupCode, FALSE);
		break;
	}
	return 0;
}

LRESULT CMainDlg::OnDelMsgSender(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	QQ_MSG_TYPE nType = (QQ_MSG_TYPE)wParam;
	UINT nSenderId = (UINT)lParam;

	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.DelMsgSender(nType, nSenderId);

	if (nType == QQ_MSG_TYPE_BUDDY)
	{
		int nTeamIndex, nIndex;
		m_BuddyListCtrl.GetItemIndexByID(nSenderId, nTeamIndex, nIndex);
		m_BuddyListCtrl.SetBuddyItemHeadFlashAnim(nTeamIndex, nIndex, FALSE);
	}
	else if (nType == QQ_MSG_TYPE_GROUP)
	{
		int nTeamIndex, nIndex;
		m_GroupListCtrl.GetItemIndexByID(nSenderId, nTeamIndex, nIndex);
		m_GroupListCtrl.SetBuddyItemHeadFlashAnim(nTeamIndex, nIndex, FALSE);
	}

	UpdateMsgIcon();

	CMessageList * lpMsgList = m_QQClient.GetMessageList();
	if (lpMsgList != NULL && lpMsgList->GetMsgSenderCount() <= 0)
	{
		if (m_MsgTipDlg.IsWindow())
			m_MsgTipDlg.DestroyWindow();
		KillTimer(m_dwMsgTimerId);
		m_dwMsgTimerId = NULL;
		m_nLastMsgType = QQ_MSG_TYPE_BUDDY;
		m_nLastSenderId = 0;
		m_TrayIcon.ModifyIcon(m_hAppIcon, _T("MingQQ"));
	}

	return 0;
}

LRESULT CMainDlg::OnCancelFlash(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	KillTimer(m_dwMsgTimerId);
	m_dwMsgTimerId = NULL;
	m_TrayIcon.ModifyIcon(m_hAppIcon, _T("MingQQ"));
	return 0;
}

void CMainDlg::ShowBuddyChatDlg(UINT nQQUin, BOOL bShow)
{
	if (nQQUin == 0)
		return;

	if (bShow)
	{
		std::map<UINT, CBuddyChatDlg *>::iterator iter;
		iter = m_mapBuddyChatDlg.find(nQQUin);
		if (iter != m_mapBuddyChatDlg.end())
		{
			CBuddyChatDlg * lpBuddyChatDlg = iter->second;
			if (lpBuddyChatDlg != NULL)
			{
				if (!lpBuddyChatDlg->IsWindow())
					lpBuddyChatDlg->Create(NULL);
				lpBuddyChatDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyChatDlg->m_hWnd);
			}
		}
		else
		{
			CBuddyChatDlg * lpBuddyChatDlg = new CBuddyChatDlg;
			if (lpBuddyChatDlg != NULL)
			{
				lpBuddyChatDlg->m_lpQQClient = &m_QQClient;
				lpBuddyChatDlg->m_lpFaceList = &m_FaceList;
				lpBuddyChatDlg->m_lpCascadeWinManager = &m_CascadeWinManager;
				lpBuddyChatDlg->m_hMainDlg = m_hWnd;
				lpBuddyChatDlg->m_nQQUin = nQQUin;
				m_mapBuddyChatDlg[nQQUin] = lpBuddyChatDlg;
				lpBuddyChatDlg->Create(NULL);
				lpBuddyChatDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyChatDlg->m_hWnd);
			}
		}
	}
	else
	{
		std::map<UINT, CBuddyChatDlg *>::iterator iter;
		iter = m_mapBuddyChatDlg.find(nQQUin);
		if (iter != m_mapBuddyChatDlg.end())
		{
			CBuddyChatDlg * lpBuddyChatDlg = iter->second;
			if (lpBuddyChatDlg != NULL)
			{
				if (lpBuddyChatDlg->IsWindow())
					lpBuddyChatDlg->DestroyWindow();
				delete lpBuddyChatDlg;
			}
			m_mapBuddyChatDlg.erase(iter);
		}
	}
}

void CMainDlg::ShowGroupChatDlg(UINT nGroupCode, BOOL bShow)
{
	if (nGroupCode == 0)
		return;

	if (bShow)
	{
		std::map<UINT, CGroupChatDlg *>::iterator iter;
		iter = m_mapGroupChatDlg.find(nGroupCode);
		if (iter != m_mapGroupChatDlg.end())
		{
			CGroupChatDlg * lpGroupChatDlg = iter->second;
			if (lpGroupChatDlg != NULL)
			{
				if (!lpGroupChatDlg->IsWindow())
					lpGroupChatDlg->Create(NULL);
				lpGroupChatDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpGroupChatDlg->m_hWnd);
			}
		}
		else
		{
			CGroupChatDlg * lpGroupChatDlg = new CGroupChatDlg;
			if (lpGroupChatDlg != NULL)
			{
				lpGroupChatDlg->m_lpQQClient = &m_QQClient;
				lpGroupChatDlg->m_lpFaceList = &m_FaceList;
				lpGroupChatDlg->m_lpCascadeWinManager = &m_CascadeWinManager;
				lpGroupChatDlg->m_hMainDlg = m_hWnd;
				lpGroupChatDlg->m_nGroupCode = nGroupCode;
				m_mapGroupChatDlg[nGroupCode] = lpGroupChatDlg;
				lpGroupChatDlg->Create(NULL);
				lpGroupChatDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpGroupChatDlg->m_hWnd);
			}
		}
	}
	else
	{
		std::map<UINT, CGroupChatDlg *>::iterator iter;
		iter = m_mapGroupChatDlg.find(nGroupCode);
		if (iter != m_mapGroupChatDlg.end())
		{
			CGroupChatDlg * lpGroupChatDlg = iter->second;
			if (lpGroupChatDlg != NULL)
			{
				if (lpGroupChatDlg->IsWindow())
					lpGroupChatDlg->DestroyWindow();
				delete lpGroupChatDlg;
			}
			m_mapGroupChatDlg.erase(iter);
		}
	}
}

void CMainDlg::ShowSessChatDlg(UINT nGroupCode, UINT nQQUin, BOOL bShow)
{
	if (nQQUin == 0)
		return;

	if (bShow)
	{
		std::map<UINT, CSessChatDlg *>::iterator iter;
		iter = m_mapSessChatDlg.find(nQQUin);
		if (iter != m_mapSessChatDlg.end())
		{
			CSessChatDlg * lpSessChatDlg = iter->second;
			if (lpSessChatDlg != NULL)
			{
				if (!lpSessChatDlg->IsWindow())
					lpSessChatDlg->Create(NULL);
				lpSessChatDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpSessChatDlg->m_hWnd);
			}
		}
		else
		{
			CSessChatDlg * lpSessChatDlg = new CSessChatDlg;
			if (lpSessChatDlg != NULL)
			{
				lpSessChatDlg->m_lpQQClient = &m_QQClient;
				lpSessChatDlg->m_lpFaceList = &m_FaceList;
				lpSessChatDlg->m_lpCascadeWinManager = &m_CascadeWinManager;
				lpSessChatDlg->m_hMainDlg = m_hWnd;
				lpSessChatDlg->m_nGroupCode = nGroupCode;
				lpSessChatDlg->m_nQQUin = nQQUin;
				m_mapSessChatDlg[nQQUin] = lpSessChatDlg;
				lpSessChatDlg->Create(NULL);
				lpSessChatDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpSessChatDlg->m_hWnd);
			}
		}
	}
	else
	{
		std::map<UINT, CSessChatDlg *>::iterator iter;
		iter = m_mapSessChatDlg.find(nQQUin);
		if (iter != m_mapSessChatDlg.end())
		{
			CSessChatDlg * lpSessChatDlg = iter->second;
			if (lpSessChatDlg != NULL)
			{
				if (lpSessChatDlg->IsWindow())
					lpSessChatDlg->DestroyWindow();
				delete lpSessChatDlg;
			}
			m_mapSessChatDlg.erase(iter);
		}
	}
}

void CMainDlg::ShowSysGroupChatDlg(UINT nGroupCode, BOOL bShow)
{
	CString strText, strGroupName, strAdminName, strAdminUin, strMsg;
	UINT nGroupNumber;

	CGroupList * lpGroupList = m_QQClient.GetGroupList();
	CMessageList * lpMsgList = m_QQClient.GetMessageList();
	if (NULL == lpGroupList || NULL == lpMsgList)
		return;

	CGroupInfo * lpGroupInfo = lpGroupList->GetGroupByCode(nGroupCode);
	CMessageSender * lpMsgSender = lpMsgList->GetMsgSender(QQ_MSG_TYPE_SYSGROUP, nGroupCode);
	if (NULL == lpMsgSender)
		return;

	if (lpGroupInfo != NULL)
		strGroupName = lpGroupInfo->m_strName.c_str();

	int nMsgCnt = lpMsgSender->GetMsgCount();
	for (int i = 0; i < nMsgCnt; i++)
	{
		CSysGroupMessage * lpSysGroupMsg = lpMsgSender->GetSysGroupMsg(i);
		if (lpSysGroupMsg != NULL)
		{
			strAdminName = lpSysGroupMsg->m_strAdminNickName.c_str();
			strAdminUin = lpSysGroupMsg->m_strAdminUin.c_str();
			strMsg = lpSysGroupMsg->m_strMsg.c_str();
			nGroupNumber = lpSysGroupMsg->m_nGroupNumber;

			if (lpSysGroupMsg->m_strSubType == _T("group_request_join_agree"))
			{
				LPCTSTR lpFmt = _T("����Ա��ͬ��������Ⱥ%s(%u)");
				strText.Format(lpFmt, strGroupName, nGroupNumber);
				MessageBox(strText, _T("��ʾ"), MB_OK);
			}
			else if (lpSysGroupMsg->m_strSubType == _T("group_request_join_deny"))
			{
				LPCTSTR lpFmt = _T("����Ա�Ѿܾ�������Ⱥ%s(%u)���ܾ����ɣ�%s");
				strText.Format(lpFmt, strGroupName, nGroupNumber, strMsg);
				MessageBox(strText, _T("��ʾ"), MB_OK);
			}
			else if (lpSysGroupMsg->m_strSubType == _T("group_leave"))
			{
				LPCTSTR lpFmt = _T("%s(%u)����Ա%s(%s)�ѽ����Ƴ�����Ⱥ��");
				strText.Format(lpFmt, strGroupName, nGroupNumber, strAdminName, strAdminUin);
				MessageBox(strText, _T("��ʾ"), MB_OK);
			}
		}
	}
	lpMsgList->DelMsgSender(QQ_MSG_TYPE_SYSGROUP, nGroupCode);
	OnDelMsgSender(NULL, QQ_MSG_TYPE_SYSGROUP, nGroupCode);
}

void CMainDlg::ShowBuddyInfoDlg(UINT nQQUin, BOOL bShow)
{
	if (nQQUin == 0)
		return;

	if (bShow)
	{
		std::map<UINT, CBuddyInfoDlg *>::iterator iter;
		iter = m_mapBuddyInfoDlg.find(nQQUin);
		if (iter != m_mapBuddyInfoDlg.end())
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (!lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
		else
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = new CBuddyInfoDlg;
			if (lpBuddyInfoDlg != NULL)
			{
				m_mapBuddyInfoDlg[nQQUin] = lpBuddyInfoDlg;
				lpBuddyInfoDlg->m_lpQQClient = &m_QQClient;
				lpBuddyInfoDlg->m_hMainDlg = m_hWnd;
				lpBuddyInfoDlg->m_nQQUin = nQQUin;
				lpBuddyInfoDlg->m_bIsGMember = FALSE;
				lpBuddyInfoDlg->m_nGroupCode = 0;
				lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
	}
	else
	{
		std::map<UINT, CBuddyInfoDlg *>::iterator iter;
		iter = m_mapBuddyInfoDlg.find(nQQUin);
		if (iter != m_mapBuddyInfoDlg.end())
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
			m_mapBuddyInfoDlg.erase(iter);
		}
	}
}

void CMainDlg::ShowGMemberInfoDlg(UINT nGroupCode, UINT nQQUin, BOOL bShow)
{
	if (0 == nGroupCode || 0 == nQQUin)
		return;

	if (bShow)
	{
		CGMemberInfoMapKey key;
		key.m_nGroupCode = nGroupCode;
		key.m_nQQUin = nQQUin;
		std::map<CGMemberInfoMapKey, CBuddyInfoDlg *>::iterator iter;
		iter = m_mapGMemberInfoDlg.find(key);
		if (iter != m_mapGMemberInfoDlg.end())
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (!lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
		else
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = new CBuddyInfoDlg;
			if (lpBuddyInfoDlg != NULL)
			{
				m_mapGMemberInfoDlg[key] = lpBuddyInfoDlg;
				lpBuddyInfoDlg->m_lpQQClient = &m_QQClient;
				lpBuddyInfoDlg->m_hMainDlg = m_hWnd;
				lpBuddyInfoDlg->m_nQQUin = nQQUin;
				lpBuddyInfoDlg->m_bIsGMember = TRUE;
				lpBuddyInfoDlg->m_nGroupCode = nGroupCode;
				lpBuddyInfoDlg->Create(NULL);
				lpBuddyInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpBuddyInfoDlg->m_hWnd);
			}
		}
	}
	else
	{
		CGMemberInfoMapKey key;
		key.m_nGroupCode = nGroupCode;
		key.m_nQQUin = nQQUin;
		std::map<CGMemberInfoMapKey, CBuddyInfoDlg *>::iterator iter;
		iter = m_mapGMemberInfoDlg.find(key);
		if (iter != m_mapGMemberInfoDlg.end())
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
			m_mapGMemberInfoDlg.erase(iter);
		}
	}
}

void CMainDlg::ShowGroupInfoDlg(UINT nGroupCode, BOOL bShow)
{
	if (0 == nGroupCode)
		return;

	if (bShow)
	{
		std::map<UINT, CGroupInfoDlg *>::iterator iter;
		iter = m_mapGroupInfoDlg.find(nGroupCode);
		if (iter != m_mapGroupInfoDlg.end())
		{
			CGroupInfoDlg * lpGroupInfoDlg = iter->second;
			if (lpGroupInfoDlg != NULL)
			{
				if (!lpGroupInfoDlg->IsWindow())
					lpGroupInfoDlg->Create(NULL);
				lpGroupInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpGroupInfoDlg->m_hWnd);
			}
		}
		else
		{
			CGroupInfoDlg * lpGroupInfoDlg = new CGroupInfoDlg;
			if (lpGroupInfoDlg != NULL)
			{
				m_mapGroupInfoDlg[nGroupCode] = lpGroupInfoDlg;
				lpGroupInfoDlg->m_lpQQClient = &m_QQClient;
				lpGroupInfoDlg->m_hMainDlg = m_hWnd;
				lpGroupInfoDlg->m_nGroupCode = nGroupCode;
				lpGroupInfoDlg->Create(NULL);
				lpGroupInfoDlg->ShowWindow(SW_SHOW);
				::SetForegroundWindow(lpGroupInfoDlg->m_hWnd);
			}
		}
	}
	else
	{
		std::map<UINT, CGroupInfoDlg *>::iterator iter;
		iter = m_mapGroupInfoDlg.find(nGroupCode);
		if (iter != m_mapGroupInfoDlg.end())
		{
			CGroupInfoDlg * lpGroupInfoDlg = iter->second;
			if (lpGroupInfoDlg != NULL)
			{
				if (lpGroupInfoDlg->IsWindow())
					lpGroupInfoDlg->DestroyWindow();
				delete lpGroupInfoDlg;
			}
			m_mapGroupInfoDlg.erase(iter);
		}
	}
}

// ֪ͨ�������촰�ڸ���
void CMainDlg::NotifyBuddyChatDlg(UINT nQQUin, UINT uMsg)
{
	std::map<UINT, CBuddyChatDlg *>::iterator iter;
	iter = m_mapBuddyChatDlg.find(nQQUin);
	if (iter != m_mapBuddyChatDlg.end())
	{
		CBuddyChatDlg * lpBuddyChatDlg = iter->second;
		if (lpBuddyChatDlg != NULL && lpBuddyChatDlg->IsWindow())
		{
			switch (uMsg)
			{
			case QQ_MSG_UPDATE_BUDDY_NUMBER:
				lpBuddyChatDlg->OnUpdateBuddyNumber();
				break;

			case QQ_MSG_UPDATE_BUDDY_SIGN:
				lpBuddyChatDlg->OnUpdateBuddySign();
				break;

			case QQ_MSG_UPDATE_BUDDY_HEADPIC:
				lpBuddyChatDlg->OnUpdateBuddyHeadPic();
				break;
			}
		}
	}
}

// ֪ͨȺ���촰�ڸ���
void CMainDlg::NotifyGroupChatDlg(UINT nGroupCode, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	std::map<UINT, CGroupChatDlg *>::iterator iter;
	iter = m_mapGroupChatDlg.find(nGroupCode);
	if (iter != m_mapGroupChatDlg.end())
	{
		CGroupChatDlg * lpGroupChatDlg = iter->second;
		if (lpGroupChatDlg != NULL && lpGroupChatDlg->IsWindow())
		{
			switch (uMsg)
			{
			case QQ_MSG_UPDATE_GROUP_INFO:
				lpGroupChatDlg->OnUpdateGroupInfo();
				break;

			case QQ_MSG_UPDATE_GROUP_NUMBER:
				lpGroupChatDlg->OnUpdateGroupNumber();
				break;

			case QQ_MSG_UPDATE_GROUP_HEADPIC:
				lpGroupChatDlg->OnUpdateGroupHeadPic();
				break;

			case QQ_MSG_UPDATE_GMEMBER_NUMBER:
				lpGroupChatDlg->OnUpdateGMemberNumber(wParam, lParam);
				break;

			case QQ_MSG_UPDATE_GMEMBER_HEADPIC:
				lpGroupChatDlg->OnUpdateGMemberHeadPic(wParam, lParam);
				break;
			}
		}
	}
}

// ֪ͨ��ʱ�Ự���촰�ڸ���
void CMainDlg::NotifySessChatDlg(UINT nQQUin, UINT uMsg)
{
	std::map<UINT, CSessChatDlg *>::iterator iter;
	iter = m_mapSessChatDlg.find(nQQUin);
	if (iter != m_mapSessChatDlg.end())
	{
		CSessChatDlg * lpSessChatDlg = iter->second;
		if (lpSessChatDlg != NULL && lpSessChatDlg->IsWindow())
		{
 			switch (uMsg)
 			{
			case QQ_MSG_UPDATE_GMEMBER_NUMBER:
				lpSessChatDlg->OnUpdateGMemberNumber();
				break;
 			case QQ_MSG_UPDATE_GMEMBER_HEADPIC:
 				lpSessChatDlg->OnUpdateGMemberHeadPic();
 				break;
 			}
		}
	}
}

// ֪ͨ������Ϣ���ڸ���
void CMainDlg::NotifyBuddyInfoDlg(UINT nQQUin, UINT uMsg)
{
	std::map<UINT, CBuddyInfoDlg *>::iterator iter;
	iter = m_mapBuddyInfoDlg.find(nQQUin);
	if (iter != m_mapBuddyInfoDlg.end())
	{
		CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
		if (lpBuddyInfoDlg != NULL && lpBuddyInfoDlg->IsWindow())
		{
			switch (uMsg)
			{
			case QQ_MSG_UPDATE_BUDDY_INFO:
				lpBuddyInfoDlg->OnUpdateBuddyInfo();
				break;

			case QQ_MSG_UPDATE_BUDDY_NUMBER:
				lpBuddyInfoDlg->OnUpdateBuddyNumber();
				break;

			case QQ_MSG_UPDATE_BUDDY_SIGN:
				lpBuddyInfoDlg->OnUpdateBuddySign();
				break;
			}
		}
	}
}

// ֪ͨȺ��Ա��Ϣ���ڸ���
void CMainDlg::NotifyGMemberInfoDlg(UINT nGroupCode, UINT nQQUin, UINT uMsg)
{
	CGMemberInfoMapKey key;
	key.m_nGroupCode = nGroupCode;
	key.m_nQQUin = nQQUin;
	std::map<CGMemberInfoMapKey, CBuddyInfoDlg *>::iterator iter;
	iter = m_mapGMemberInfoDlg.find(key);
	if (iter != m_mapGMemberInfoDlg.end())
	{
		CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
		if (lpBuddyInfoDlg != NULL && lpBuddyInfoDlg->IsWindow())
		{
			switch (uMsg)
			{
			case QQ_MSG_UPDATE_GMEMBER_INFO:
				lpBuddyInfoDlg->OnUpdateBuddyInfo();
				break;

			case QQ_MSG_UPDATE_GMEMBER_NUMBER:
				lpBuddyInfoDlg->OnUpdateBuddyNumber();
				break;

			case QQ_MSG_UPDATE_GMEMBER_SIGN:
				lpBuddyInfoDlg->OnUpdateBuddySign();
				break;
			}
		}
	}
}

// ֪ͨȺ��Ϣ���ڸ���
void CMainDlg::NotifyGroupInfoDlg(UINT nGroupCode, UINT uMsg)
{
	std::map<UINT, CGroupInfoDlg *>::iterator iter;
	iter = m_mapGroupInfoDlg.find(nGroupCode);
	if (iter != m_mapGroupInfoDlg.end())
	{
		CGroupInfoDlg * lpGroupInfoDlg = iter->second;
		if (lpGroupInfoDlg != NULL && lpGroupInfoDlg->IsWindow())
		{
			switch (uMsg)
			{
			case QQ_MSG_UPDATE_GROUP_INFO:
				lpGroupInfoDlg->OnUpdateGroupInfo();
				break;

			case QQ_MSG_UPDATE_GROUP_NUMBER:
				lpGroupInfoDlg->OnUpdateGroupNumber();
				break;
			}
		}
	}
}

void CMainDlg::UpdateBuddyTreeCtrl()
{
	CBuddyList * lpBuddyList = m_QQClient.GetBuddyList();
	if (NULL == lpBuddyList)
		return;

	m_BuddyListCtrl.DelAllItems();

	int nBuddyTeamCount = lpBuddyList->GetBuddyTeamCount();
	for (int i = 0; i < nBuddyTeamCount; i++)
	{
		int nBuddyCount = lpBuddyList->GetBuddyCount(i);
		int nOnlineBuddyCount = lpBuddyList->GetOnlineBuddyCount(i);
		int nTeamIndex = 0;

		CBuddyTeamInfo * lpBuddyTeamInfo = lpBuddyList->GetBuddyTeam(i);
		if (lpBuddyTeamInfo != NULL)
		{
			nTeamIndex = m_BuddyListCtrl.AddBuddyTeam(i);
			m_BuddyListCtrl.SetBuddyTeamName(nTeamIndex, lpBuddyTeamInfo->m_strName.c_str());
			m_BuddyListCtrl.SetBuddyTeamMaxCnt(nTeamIndex, nBuddyCount);
			m_BuddyListCtrl.SetBuddyTeamCurCnt(nTeamIndex, nOnlineBuddyCount);
			m_BuddyListCtrl.SetBuddyTeamExpand(nTeamIndex, FALSE);
		}

		for (int j = 0; j < nBuddyCount; j++)
		{
			CBuddyInfo * lpBuddyInfo = lpBuddyList->GetBuddy(i, j);
			if (lpBuddyInfo != NULL)
			{
				CString strQQNum;
				strQQNum.Format(_T("%u"), lpBuddyInfo->m_nQQNum);

				BOOL bGender = (lpBuddyInfo->m_strGender == _T("male")) ? TRUE : FALSE;
				BOOL bGray = lpBuddyInfo->m_nStatus != QQ_STATUS_OFFLINE ? FALSE : TRUE;

				tstring strFileName = m_QQClient.GetBuddyHeadPicFullName(lpBuddyInfo->m_nQQNum);
				if (!ZYM::CPath::IsFileExist(strFileName.c_str()))
					strFileName = ZYM::CPath::GetAppPath() + _T("Image\\DefBuddyHeadPic.png");

				int nIndex = m_BuddyListCtrl.AddBuddyItem(nTeamIndex, lpBuddyInfo->m_nQQUin);
				m_BuddyListCtrl.SetBuddyItemQQNum(nTeamIndex, nIndex, strQQNum);
				m_BuddyListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpBuddyInfo->m_strNickName.c_str());
				m_BuddyListCtrl.SetBuddyItemMarkName(nTeamIndex, nIndex, lpBuddyInfo->m_strMarkName.c_str());
				m_BuddyListCtrl.SetBuddyItemGender(nTeamIndex, nIndex, bGender);
				m_BuddyListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), bGray);
				m_BuddyListCtrl.SetBuddyItemOnline(nTeamIndex, nIndex, !bGray, FALSE);
			}
		}
	}

	if (m_BuddyListCtrl.IsWindowVisible())
		m_BuddyListCtrl.Invalidate();
}

void CMainDlg::UpdateGroupTreeCtrl()
{
	CGroupList * lpGroupList = m_QQClient.GetGroupList();
	if (NULL == lpGroupList)
		return;

	m_GroupListCtrl.DelAllItems();

	int nGroupCount = lpGroupList->GetGroupCount();

	int nTeamIndex = m_GroupListCtrl.AddBuddyTeam(0);
	m_GroupListCtrl.SetBuddyTeamName(nTeamIndex, _T("�ҵ�QQȺ"));
	m_GroupListCtrl.SetBuddyTeamMaxCnt(nTeamIndex, nGroupCount);
	m_GroupListCtrl.SetBuddyTeamCurCnt(nTeamIndex, nGroupCount);
	m_GroupListCtrl.SetBuddyTeamExpand(nTeamIndex, FALSE);

	for (int i = 0; i < nGroupCount; i++)
	{
		CGroupInfo * lpGroupInfo = lpGroupList->GetGroup(i);
		if (lpGroupInfo != NULL)
		{
			tstring strFileName = m_QQClient.GetGroupHeadPicFullName(lpGroupInfo->m_nGroupNumber);
			if (!ZYM::CPath::IsFileExist(strFileName.c_str()))
				strFileName = ZYM::CPath::GetAppPath() + _T("Image\\DefGroupHeadPic.jpg");

			int nIndex = m_GroupListCtrl.AddBuddyItem(nTeamIndex, lpGroupInfo->m_nGroupCode);
			m_GroupListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpGroupInfo->m_strName.c_str());
			m_GroupListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), FALSE);
		}
	}

	if (m_GroupListCtrl.IsWindowVisible())
		m_GroupListCtrl.Invalidate();
}

void CMainDlg::UpdateRecentTreeCtrl()
{
	CRecentList * lpRecentList = m_QQClient.GetRecentList();
	if (NULL == lpRecentList)
		return;

	m_RecentListCtrl.DelAllItems();

	int nTeamIndex = m_RecentListCtrl.AddBuddyTeam(-1);
	m_RecentListCtrl.SetBuddyTeamName(nTeamIndex, _T("�����ϵ��"));
	m_RecentListCtrl.SetBuddyTeamExpand(nTeamIndex, FALSE);

	int nCount = 0;

	int nRecentCount = lpRecentList->GetRecentCount();
	for (int i = 0; i < nRecentCount; i++)
	{
		CRecentInfo * lpRecentInfo = lpRecentList->GetRecent(i);
		if (lpRecentInfo != NULL)
		{
			if (0 == lpRecentInfo->m_nType)			// ����
			{
				CBuddyList * lpBuddyList = m_QQClient.GetBuddyList();
				if (lpBuddyList != NULL)
				{
					CBuddyInfo * lpBuddyInfo = lpBuddyList->GetBuddy(lpRecentInfo->m_nQQUin);
					if (lpBuddyInfo != NULL)
					{
						CString strQQNum;
						strQQNum.Format(_T("%u"), lpBuddyInfo->m_nQQNum);

						BOOL bGender = (lpBuddyInfo->m_strGender == _T("male")) ? TRUE : FALSE;
						BOOL bGray = lpBuddyInfo->m_nStatus != QQ_STATUS_OFFLINE ? FALSE : TRUE;

						tstring strFileName = m_QQClient.GetBuddyHeadPicFullName(lpBuddyInfo->m_nQQNum);
						if (!ZYM::CPath::IsFileExist(strFileName.c_str()))
							strFileName = ZYM::CPath::GetAppPath() + _T("Image\\DefBuddyHeadPic.png");

						int nIndex = m_RecentListCtrl.AddBuddyItem(nTeamIndex, i);
						m_RecentListCtrl.SetBuddyItemQQNum(nTeamIndex, nIndex, strQQNum);
						m_RecentListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpBuddyInfo->m_strNickName.c_str());
						m_RecentListCtrl.SetBuddyItemMarkName(nTeamIndex, nIndex, lpBuddyInfo->m_strMarkName.c_str());
						m_RecentListCtrl.SetBuddyItemGender(nTeamIndex, nIndex, bGender);
						m_RecentListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), bGray);

						nCount++;
					}
				}
			}
			else if (1 == lpRecentInfo->m_nType)	// Ⱥ
			{
				CGroupList * lpGroupList = m_QQClient.GetGroupList();
				if (lpGroupList != NULL)
				{
					CGroupInfo * lpGroupInfo = lpGroupList->GetGroupById(lpRecentInfo->m_nQQUin);
					if (lpGroupInfo != NULL)
					{
						tstring strFileName = m_QQClient.GetGroupHeadPicFullName(lpGroupInfo->m_nGroupNumber);
						if (!ZYM::CPath::IsFileExist(strFileName.c_str()))
							strFileName = _T("Image\\DefGroupHeadPic.jpg");

						int nIndex = m_RecentListCtrl.AddBuddyItem(nTeamIndex, i);
						m_RecentListCtrl.SetBuddyItemNickName(nTeamIndex, nIndex, lpGroupInfo->m_strName.c_str());
						m_RecentListCtrl.SetBuddyItemHeadPic(nTeamIndex, nIndex, strFileName.c_str(), FALSE);

						nCount++;
					}
				}
			}
			else if (2 == lpRecentInfo->m_nType)	// ������������
			{
			}
		}
	}

	m_RecentListCtrl.SetBuddyTeamMaxCnt(nTeamIndex, nCount);
	m_RecentListCtrl.SetBuddyTeamCurCnt(nTeamIndex, nCount);

	if (m_RecentListCtrl.IsWindowVisible())
		m_RecentListCtrl.Invalidate();
}

void CMainDlg::OnTrayIcon_LButtunUp()
{
	if (m_dwMsgTimerId != NULL)
	{
		CMessageList * lpMsgList = m_QQClient.GetMessageList();
		if (lpMsgList != NULL && lpMsgList->GetMsgSenderCount() > 0)
		{
			CMessageSender * lpMsgSender = lpMsgList->GetLastMsgSender();
			if (lpMsgSender != NULL)
			{
				QQ_MSG_TYPE nType = lpMsgSender->GetMsgType();
				UINT nSenderId = lpMsgSender->GetSenderId();
				UINT nGroupCode = lpMsgSender->GetGroupCode();

				switch (nType)
				{
				case QQ_MSG_TYPE_BUDDY:
					ShowBuddyChatDlg(nSenderId, TRUE);
					break;
				case QQ_MSG_TYPE_GROUP:
					ShowGroupChatDlg(nSenderId, TRUE);
					break;
				case QQ_MSG_TYPE_SESS:
					ShowSessChatDlg(nGroupCode, nSenderId, TRUE);
					break;
				case QQ_MSG_TYPE_SYSGROUP:
					ShowSysGroupChatDlg(nGroupCode, TRUE);
					break;
				}
			}
		}
	}
	else
	{
		if (m_LoginDlg.IsWindow())
		{
			m_LoginDlg.ShowWindow(SW_SHOW);
			::SetForegroundWindow(m_LoginDlg.m_hWnd);
		}
		else if (IsWindow())
		{
			ShowWindow(SW_SHOW);
			::SetForegroundWindow(m_hWnd);
		}
	}
}

void CMainDlg::OnTrayIcon_RButtunUp()
{
	if (m_MsgTipDlg.IsWindow())
		m_MsgTipDlg.DestroyWindow();

	int nPos = (m_QQClient.GetStatus() != QQ_STATUS_OFFLINE) ? 2 : 1;

	CPoint pt;
	GetCursorPos(&pt);

	::SetForegroundWindow(m_hWnd);

	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(nPos);
	PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, m_hWnd);

	// BUGFIX: See "PRB: Menus for Notification Icons Don't Work Correctly"
	::PostMessage(m_hWnd, WM_NULL, 0, 0);
}

void CMainDlg::OnTrayIcon_MouseHover()
{
	if (m_dwMsgTimerId != NULL)
	{
		CMessageList * lpMsgList = m_QQClient.GetMessageList();
		if (lpMsgList != NULL && lpMsgList->GetMsgSenderCount() > 0)
		{
			CRect rcTrayIcon;
			m_TrayIcon.GetTrayIconRect(&rcTrayIcon);

			m_MsgTipDlg.m_lpQQClient = &m_QQClient;
			m_MsgTipDlg.m_hMainDlg = m_hWnd;
			m_MsgTipDlg.m_rcTrayIcon = rcTrayIcon;
			if (!m_MsgTipDlg.IsWindow())
				m_MsgTipDlg.Create(m_hWnd);
			m_MsgTipDlg.ShowWindow(SW_SHOWNOACTIVATE);
		}
	}
	//::OutputDebugString(_T("WM_MOUSEHOVER\n"));
}

void CMainDlg::OnTrayIcon_MouseLeave()
{
	if (m_MsgTipDlg.IsWindow())
	{
		CRect rcWindow;
		m_MsgTipDlg.GetWindowRect(&rcWindow);

		POINT pt = {0};
		::GetCursorPos(&pt);

		if (!::PtInRect(&rcWindow, pt))
			m_MsgTipDlg.DestroyWindow();
		else
			m_MsgTipDlg.StartTrackMouseLeave();
	}
	//::OutputDebugString(_T("WM_MOUSELEAVE\n"));
}

BOOL CMainDlg::LoadAppIcon(QQ_STATUS nStatus)
{
	DestroyAppIcon();

	tstring strFileName;
	switch (nStatus)
	{
	case QQ_STATUS_ONLINE:
		strFileName = ZYM::CPath::GetAppPath() + _T("Image\\imonline.ico");
		break;
	case QQ_STATUS_CALLME:
		strFileName = ZYM::CPath::GetAppPath() + _T("Image\\Qme.ico");
		break;
	case QQ_STATUS_AWAY:
		strFileName = ZYM::CPath::GetAppPath() + _T("Image\\away.ico");
		break;
	case QQ_STATUS_BUSY:
		strFileName = ZYM::CPath::GetAppPath() + _T("Image\\busy.ico");
		break;
	case QQ_STATUS_SILENT:
		strFileName = ZYM::CPath::GetAppPath() + _T("Image\\mute.ico");
		break;
	case QQ_STATUS_HIDDEN:
		strFileName = ZYM::CPath::GetAppPath() + _T("Image\\invisible.ico");
		break;
	case QQ_STATUS_OFFLINE:
		strFileName = ZYM::CPath::GetAppPath() + _T("Image\\offline.ico");
		break;
	}
	m_hAppIcon = AtlLoadIconImage(strFileName.c_str(), LR_DEFAULTCOLOR|LR_LOADFROMFILE, 
		::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	return m_hAppIcon != NULL ? TRUE : FALSE;
}

void CMainDlg::DestroyAppIcon()
{
	if (m_hAppIcon != NULL)
	{
		::DestroyIcon(m_hAppIcon);
		m_hAppIcon = NULL;
	}
}

BOOL CMainDlg::LoadLoginIcon()
{
	DestroyLoginIcon();

	CString strFileName;
	for (int i = 0; i < 6; i++)
	{
		strFileName.Format(_T("%sImage\\Loading_%d.ico"), ZYM::CPath::GetAppPath().c_str(), i+1);
		m_hLoginIcon[i] = AtlLoadIconImage((LPCTSTR)strFileName, LR_DEFAULTCOLOR|LR_LOADFROMFILE, 
			::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	}

	return TRUE;
}

void CMainDlg::DestroyLoginIcon()
{
	for (int i = 0; i < 6; i++)
	{
		if (m_hLoginIcon[i] != NULL)
		{
			::DestroyIcon(m_hLoginIcon[i]);
			m_hLoginIcon[i] = NULL;
		}
	}
}

void CMainDlg::UpdateMsgIcon()
{
	CMessageList * lpMsgList = m_QQClient.GetMessageList();
	if (NULL == lpMsgList)
		return;

	CMessageSender * lpMsgSender = lpMsgList->GetLastMsgSender();
	if (NULL == lpMsgSender)
		return;
	
	QQ_MSG_TYPE nMsgType = lpMsgSender->GetMsgType();
	UINT nSenderId = lpMsgSender->GetSenderId();
	UINT nGroupCode = lpMsgSender->GetGroupCode();

	if (m_nLastMsgType != nMsgType || m_nLastSenderId != nSenderId)
	{
		m_nLastMsgType = nMsgType;
		m_nLastSenderId = nSenderId;

		if (m_hMsgIcon != NULL)
		{
			::DestroyIcon(m_hMsgIcon);
			m_hMsgIcon = NULL;
		}

		CString strHeadPicFileName;

		if (QQ_MSG_TYPE_BUDDY == nMsgType)
			strHeadPicFileName = GetHeadPicFullName(0, nSenderId);
		else if (QQ_MSG_TYPE_GROUP == nMsgType)
			strHeadPicFileName = GetHeadPicFullName(nSenderId, 0);
		else if (QQ_MSG_TYPE_SESS == nMsgType)
			strHeadPicFileName = GetHeadPicFullName(nGroupCode, nSenderId);
// 		else if (QQ_MSG_TYPE_SYSGROUP == nMsgType)
// 			strHeadPicFileName = ZYM::CPath::GetAppPath() + _T("Image\\DefSysHeadPic.ico");

		m_hMsgIcon = ExtractIcon(strHeadPicFileName);
		if (NULL == m_hMsgIcon)
		{
			if (QQ_MSG_TYPE_BUDDY == nMsgType || QQ_MSG_TYPE_SESS == nMsgType)
				m_hMsgIcon = AtlLoadIconImage(IDI_BUDDY, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
			else if (QQ_MSG_TYPE_GROUP == nMsgType)
				m_hMsgIcon = AtlLoadIconImage(IDI_GROUPCHATDLG_16, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
			else if (QQ_MSG_TYPE_SYSGROUP == nMsgType)
				m_hMsgIcon = AtlLoadIconImage(IDI_SYS, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
		}
	}
}

CString CMainDlg::GetHeadPicFullName(UINT nGroupCode, UINT nQQUin)
{
	UINT nGroupNum, nQQNum;
	GetNumber(nGroupCode, nQQUin, nGroupNum, nQQNum);

	if (nGroupCode != 0 && nQQUin != 0)	// Ⱥ��Ա
		return m_QQClient.GetSessHeadPicFullName(nQQNum).c_str();
	else if (nGroupCode != 0)			// Ⱥ
		return m_QQClient.GetGroupHeadPicFullName(nGroupNum).c_str();
	else								// ����
		return m_QQClient.GetBuddyHeadPicFullName(nQQNum).c_str();
}

void CMainDlg::GetNumber(UINT nGroupCode, UINT nQQUin, UINT& nGroupNum, UINT& nQQNum)
{
	nGroupNum = nQQNum = 0;

	if (nGroupCode != 0 && nQQUin != 0)
	{
		CGroupList * lpGroupList = m_QQClient.GetGroupList();
		if (lpGroupList != NULL)
		{
			CBuddyInfo * lpBuddyInfo = lpGroupList->GetGroupMemberByCode(nGroupCode, nQQUin);
			if (lpBuddyInfo != NULL)
				nQQNum = lpBuddyInfo->m_nQQNum;
		}
	}
	else if (nGroupCode != 0)
	{
		CGroupList * lpGroupList = m_QQClient.GetGroupList();
		if (lpGroupList != NULL)
		{
			CGroupInfo * lpGroupInfo = lpGroupList->GetGroupByCode(nGroupCode);
			if (lpGroupInfo != NULL)
				nGroupNum = lpGroupInfo->m_nGroupNumber;
		}
	}
	else if (nQQUin != 0)
	{
		CBuddyList * lpBuddyList = m_QQClient.GetBuddyList();
		if (lpBuddyList != NULL)
		{
			CBuddyInfo * lpBuddyInfo = lpBuddyList->GetBuddy(nQQUin);
			if (lpBuddyInfo != NULL)
				nQQNum = lpBuddyInfo->m_nQQNum;
		}
	}
}

HICON CMainDlg::ExtractIcon(LPCTSTR lpszFileName)
{
	if (NULL == lpszFileName || NULL == *lpszFileName)
		return NULL;

	int cx = 16, cy = 16;
	HBITMAP hBmp = NULL;

	Gdiplus::Bitmap imgHead(lpszFileName);
	if (imgHead.GetLastStatus() != Gdiplus::Ok)
		return NULL;

	if (imgHead.GetWidth() != cx || imgHead.GetHeight() != cy)
	{
		Gdiplus::Bitmap* pThumbnail = (Gdiplus::Bitmap*)imgHead.GetThumbnailImage(cx, cy, NULL, NULL);
		if (pThumbnail != NULL)
		{
			pThumbnail->GetHBITMAP(Gdiplus::Color(255,255,255), &hBmp);
			delete pThumbnail;
		}
	}
	else
	{
		imgHead.GetHBITMAP(Gdiplus::Color(255,255,255), &hBmp);
	}

	if (NULL == hBmp)
		return NULL;

	HICON hIcon = NULL;

	CImageList imgList;
	BOOL bRet = imgList.Create(cx, cy, TRUE | ILC_COLOR32, 1, 1);
	if (bRet)
	{
		imgList.SetBkColor(RGB(255,255,255));
		imgList.Add(hBmp);
		hIcon = imgList.ExtractIcon(0);
	}
	::DeleteObject(hBmp);
	imgList.Destroy();

	return hIcon;
}

void CMainDlg::CloseAllDlg()
{
	{
		std::map<UINT, CBuddyChatDlg *>::iterator iter;
		for (iter = m_mapBuddyChatDlg.begin(); iter != m_mapBuddyChatDlg.end(); iter++)
		{
			CBuddyChatDlg * lpBuddyChatDlg = iter->second;
			if (lpBuddyChatDlg != NULL)
			{
				if (lpBuddyChatDlg->IsWindow())
					lpBuddyChatDlg->DestroyWindow();
				delete lpBuddyChatDlg;
			}
		}
		m_mapBuddyChatDlg.clear();
	}

	{
		std::map<UINT, CGroupChatDlg *>::iterator iter;
		for (iter = m_mapGroupChatDlg.begin(); iter != m_mapGroupChatDlg.end(); iter++)
		{
			CGroupChatDlg * lpGroupChatDlg = iter->second;
			if (lpGroupChatDlg != NULL)
			{
				if (lpGroupChatDlg->IsWindow())
					lpGroupChatDlg->DestroyWindow();
				delete lpGroupChatDlg;
			}
		}
		m_mapGroupChatDlg.clear();
	}

	{
		std::map<UINT, CSessChatDlg *>::iterator iter;
		for (iter = m_mapSessChatDlg.begin(); iter != m_mapSessChatDlg.end(); iter++)
		{
			CSessChatDlg * lpSessChatDlg = iter->second;
			if (lpSessChatDlg != NULL)
			{
				if (lpSessChatDlg->IsWindow())
					lpSessChatDlg->DestroyWindow();
				delete lpSessChatDlg;
			}
		}
		m_mapSessChatDlg.clear();
	}

	{
		std::map<UINT, CBuddyInfoDlg *>::iterator iter;
		for (iter = m_mapBuddyInfoDlg.begin(); iter != m_mapBuddyInfoDlg.end(); iter++)
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
		}
		m_mapBuddyInfoDlg.clear();
	}

	{
		std::map<CGMemberInfoMapKey, CBuddyInfoDlg *>::iterator iter;
		for (iter = m_mapGMemberInfoDlg.begin(); iter != m_mapGMemberInfoDlg.end(); iter++)
		{
			CBuddyInfoDlg * lpBuddyInfoDlg = iter->second;
			if (lpBuddyInfoDlg != NULL)
			{
				if (lpBuddyInfoDlg->IsWindow())
					lpBuddyInfoDlg->DestroyWindow();
				delete lpBuddyInfoDlg;
			}
		}
		m_mapGMemberInfoDlg.clear();
	}

	{
		std::map<UINT, CGroupInfoDlg *>::iterator iter;
		for (iter = m_mapGroupInfoDlg.begin(); iter != m_mapGroupInfoDlg.end(); iter++)
		{
			CGroupInfoDlg * lpGroupInfoDlg = iter->second;
			if (lpGroupInfoDlg != NULL)
			{
				if (lpGroupInfoDlg->IsWindow())
					lpGroupInfoDlg->DestroyWindow();
				delete lpGroupInfoDlg;
			}
		}
		m_mapGroupInfoDlg.clear();
	}
}

// �Ӳ˵�ID��ȡ��Ӧ��QQ_STATUS
QQ_STATUS CMainDlg::GetStatusFromMenuID(int nMenuID)
{
	switch (nMenuID)
	{
	case ID_MENU_IMONLINE:
		return QQ_STATUS_ONLINE;
	case ID_MENU_QME:
		return QQ_STATUS_CALLME;
	case ID_MENU_AWAY:
		return QQ_STATUS_AWAY;
	case ID_MENU_BUSY:
		return QQ_STATUS_BUSY;
	case ID_MENU_MUTE:
		return QQ_STATUS_SILENT;
	case ID_MENU_INVISIBLE:
		return QQ_STATUS_HIDDEN;
	case ID_MENU_IMOFFLINE:
		return QQ_STATUS_OFFLINE;
	default:
		return QQ_STATUS_OFFLINE;
	}
}

// ����ָ��״̬����״̬�˵���ť��ͼ��
void CMainDlg::StatusMenuBtn_SetIconPic(CSkinButton& btnStatus, QQ_STATUS nStatus)
{
	LPCTSTR lpszFileName;

	switch (nStatus)
	{
	case QQ_STATUS_ONLINE:
		lpszFileName = _T("Status\\imonline.png");
		break;
	case QQ_STATUS_CALLME:
		lpszFileName = _T("Status\\Qme.png");
		break;
	case QQ_STATUS_AWAY:
		lpszFileName = _T("Status\\away.png");
		break;
	case QQ_STATUS_BUSY:
		lpszFileName = _T("Status\\busy.png");
		break;
	case QQ_STATUS_SILENT:
		lpszFileName = _T("Status\\mute.png");
		break;
	case QQ_STATUS_HIDDEN:
		lpszFileName = _T("Status\\invisible.png");
		break;
	case QQ_STATUS_OFFLINE:
		lpszFileName = _T("Status\\imoffline.png");
		break;
	default:
		return;
	}

	btnStatus.SetIconPic(lpszFileName);
	btnStatus.Invalidate();
}
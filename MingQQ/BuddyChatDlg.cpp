#include "StdAfx.h"
#include "BuddyChatDlg.h"

CBuddyChatDlg::CBuddyChatDlg(void)
{
	m_lpQQClient = NULL;
	m_lpFaceList = NULL;
	m_lpCascadeWinManager = NULL;
	m_hMainDlg = NULL;
	m_nQQUin = NULL;

	m_hDlgIcon = m_hDlgSmallIcon = NULL;
	m_fontBuddyNameLink = NULL;
	m_hRBtnDownWnd = NULL;
	memset(&m_ptRBtnDown, 0, sizeof(m_ptRBtnDown));
	m_pLastImageOle = NULL;
	m_cxPicBarDlg = 122;
	m_cyPicBarDlg = 24;

	m_nQQNumber = 0;
	m_nUserNumber = 0;
	m_strBuddyName = _T("�����ǳ�");

	m_dwThreadId = 0;
}

CBuddyChatDlg::~CBuddyChatDlg(void)
{

}

BOOL CBuddyChatDlg::PreTranslateMessage(MSG* pMsg)
{
	if (::GetForegroundWindow() == m_hWnd && !m_Accelerator.IsNull() && 
		m_Accelerator.TranslateAccelerator(m_hWnd, pMsg))
		return TRUE;

	if (pMsg->hwnd == m_richRecv.m_hWnd || pMsg->hwnd == m_richSend.m_hWnd)
	{
		if (pMsg->message == WM_MOUSEMOVE)			// ����/�����ı��������ƶ���Ϣ
		{
			if (OnRichEdit_MouseMove(pMsg))
				return TRUE;
		}
		else if (pMsg->message == WM_LBUTTONDBLCLK) // ����/�����ı�������˫����Ϣ
		{
			if (OnRichEdit_LBtnDblClk(pMsg))
				return TRUE;
		}
		else if (pMsg->message == WM_RBUTTONDOWN)	// ����/�����ı��������Ҽ�������Ϣ
		{
			if (OnRichEdit_RBtnDown(pMsg))
				return TRUE;
		}
	}

	return CWindow::IsDialogMessage(pMsg);
}

void CBuddyChatDlg::OnRecvMsg(UINT nQQUin, UINT nMsgId)
{
	if (NULL == m_lpQQClient || m_nQQUin != nQQUin)
		return;

	if (::GetForegroundWindow() != m_hWnd)
		FlashWindowEx(m_hWnd, 3);

	if (nMsgId == 0)
	{
		CMessageList * lpMsgList = m_lpQQClient->GetMessageList();
		if (lpMsgList != NULL)
		{
			CMessageSender * lpMsgSender = lpMsgList->GetMsgSender(QQ_MSG_TYPE_BUDDY, nQQUin);
			if (lpMsgSender != NULL)
			{
				int nMsgCnt = lpMsgSender->GetMsgCount();
				for (int i = 0; i < nMsgCnt; i++)
				{
					CBuddyMessage * lpBuddyMsg = lpMsgSender->GetBuddyMsg(i);
					if (lpBuddyMsg != NULL)
					{
						AddMsgToRecvEdit(lpBuddyMsg);
					}
				}
				lpMsgList->DelMsgSender(QQ_MSG_TYPE_BUDDY, nQQUin);
				::PostMessage(m_hMainDlg, WM_DEL_MSG_SENDER, QQ_MSG_TYPE_BUDDY, nQQUin);
			}
		}
	}
	else
	{
		CMessageList * lpMsgList = m_lpQQClient->GetMessageList();
		if (lpMsgList != NULL)
		{
			CMessageSender * lpMsgSender = lpMsgList->GetMsgSender(QQ_MSG_TYPE_BUDDY, nQQUin);
			if (lpMsgSender != NULL)
			{
				CBuddyMessage * lpBuddyMsg = lpMsgSender->GetBuddyMsgById(nMsgId);
				if (lpBuddyMsg != NULL)
				{
					AddMsgToRecvEdit(lpBuddyMsg);
					lpMsgSender->DelMsgById(nMsgId);
				}

				if (lpMsgSender->GetMsgCount() <= 0)
					lpMsgList->DelMsgSender(QQ_MSG_TYPE_BUDDY, nQQUin);
			}
		}
	}
}

// ���º��Ѻ���֪ͨ
void CBuddyChatDlg::OnUpdateBuddyNumber()
{
	UpdateData();
	UpdateBuddyNameCtrl();
}

// ���º���ǩ��֪ͨ
void CBuddyChatDlg::OnUpdateBuddySign()
{
	UpdateData();
	UpdateBuddySignCtrl();
}

// ���º���ͷ��֪ͨ
void CBuddyChatDlg::OnUpdateBuddyHeadPic()
{
	tstring strFileName;
	if (m_lpQQClient != NULL && m_nQQNumber != 0)
		strFileName = m_lpQQClient->GetBuddyHeadPicFullName(m_nQQNumber);
	if (!ZYM::CPath::IsFileExist(strFileName.c_str()))
		strFileName = ZYM::CPath::GetAppPath() + _T("Image\\DefBuddyHeadPic.png");
	m_picHead.SetBitmap(strFileName.c_str());
	m_picHead.Invalidate();
}

BOOL CBuddyChatDlg::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	m_lpCascadeWinManager->Add(m_hWnd, 540, 510);

	// set icons
	m_hDlgIcon = AtlLoadIconImage(IDI_BUDDYCHATDLG_32, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(m_hDlgIcon, TRUE);
	m_hDlgSmallIcon = AtlLoadIconImage(IDI_BUDDYCHATDLG_16, LR_DEFAULTCOLOR, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON));
	SetIcon(m_hDlgSmallIcon, FALSE);

	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);

	UpdateData();

	CBuddyInfo * lpBuddyInfo = GetBuddyInfoPtr();
	if (lpBuddyInfo != NULL)
	{
		if (!lpBuddyInfo->IsHasQQNum())		// ���º��Ѻ���
		{
			m_lpQQClient->UpdateBuddyNum(m_nQQUin);
		}
		else								// ���º���ͷ��
		{
			if (m_lpQQClient->IsNeedUpdateBuddyHeadPic(m_nQQNumber))
				m_lpQQClient->UpdateBuddyHeadPic(m_nQQUin, m_nQQNumber);
		}

		if (!lpBuddyInfo->IsHasQQSign())	// ���¸���ǩ��
			m_lpQQClient->UpdateBuddySign(m_nQQUin);
	}

	Init();		// ��ʼ��

	PostMessage(WM_SETDLGINITFOCUS, 0, 0);		// ���öԻ����ʼ����
	SetTimer(1001, 300, NULL);
	
	return TRUE;
}

BOOL CBuddyChatDlg::OnCopyData(CWindow wnd, PCOPYDATASTRUCT pCopyDataStruct)
{
	if (NULL == pCopyDataStruct)
		return FALSE;

	switch (pCopyDataStruct->dwData)
	{
	case IPC_CODE_MSGLOG_PASTE:			// ��Ϣ��¼�������ճ����Ϣ
		{
			if (pCopyDataStruct->lpData != NULL && pCopyDataStruct->cbData > 0)
				AddMsgToSendEdit((LPCTSTR)pCopyDataStruct->lpData);
		}
		break;

	case IPC_CODE_MSGLOG_EXIT:			// ��Ϣ��¼��������˳���Ϣ
		{
			m_tbMid.SetItemCheckState(13, FALSE);
			m_tbMid.Invalidate();
		}
		break;
	}

	return TRUE;
}

void CBuddyChatDlg::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	m_SkinMenu.OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}

void CBuddyChatDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	m_SkinMenu.OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CBuddyChatDlg::OnGetMinMaxInfo(LPMINMAXINFO lpMMI)
{
	lpMMI->ptMinTrackSize.x = 540;
	lpMMI->ptMinTrackSize.y = 510;
}

void CBuddyChatDlg::OnMove(CPoint ptPos)
{
	SetMsgHandled(FALSE);

	m_lpCascadeWinManager->SetPos(m_hWnd, ptPos.x, ptPos.y);
}

void CBuddyChatDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);

	CRect rcClient;
	GetClientRect(&rcClient);

	if (m_staBuddySign.IsWindow())
		m_staBuddySign.MoveWindow(44, 22, rcClient.Width()-50, 14);

	if (m_picAD_1.IsWindow())
		m_picAD_1.MoveWindow(rcClient.right-144, rcClient.top+43, 142, 42);

	if (m_picAD_2.IsWindow())
		m_picAD_2.MoveWindow(rcClient.right-143, rcClient.top+85, 140, 224);

	if (m_picAD_3.IsWindow())
		m_picAD_3.MoveWindow(rcClient.right-143, rcClient.bottom-145, 140, 141);

	if (m_btnClose.IsWindow())
		m_btnClose.MoveWindow(rcClient.right-310, rcClient.bottom-5-22, 69, 22);

	if (m_btnSend.IsWindow())
		m_btnSend.MoveWindow(rcClient.right-236, rcClient.bottom-5-22, 64, 22);

	if (m_btnArrow.IsWindow())
		m_btnArrow.MoveWindow(rcClient.right-172, rcClient.bottom-5-22, 19, 22);

	//if (m_tbTop.IsWindow())
	//	m_tbTop.MoveWindow(3, 45, 386, 44);
		
	if (m_tbMid.IsWindow())
		m_tbMid.MoveWindow(1, rcClient.bottom-155, rcClient.Width()-1-152, 27);

	if (m_richRecv.IsWindow())
	{
		if (!::IsWindowVisible(m_FontSelDlg.m_hWnd))
			m_richRecv.MoveWindow(6, rcClient.top+44+44, rcClient.Width()-6-152, rcClient.Height()-243);
		else
			m_richRecv.MoveWindow(6, rcClient.top+44+44, rcClient.Width()-6-152, rcClient.Height()-243-32);
	}

	if (m_richSend.IsWindow())
		m_richSend.MoveWindow(6, rcClient.bottom-128, rcClient.Width()-6-152, 95);

	if (::IsWindowVisible(m_FontSelDlg.m_hWnd))
		m_FontSelDlg.MoveWindow(1, rcClient.bottom-187, rcClient.Width()-1-152, 32);
}

void CBuddyChatDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (1001 == nIDEvent)
	{
		OnRecvMsg(m_nQQUin, NULL);	// ��ʾ��Ϣ
		KillTimer(nIDEvent);
		SetTimer(1002, 300, NULL);
	}
	else if (nIDEvent == 1002)
	{
		if (!m_FontSelDlg.IsWindow())
			m_FontSelDlg.Create(m_hWnd);
		KillTimer(nIDEvent);
	}
}

void CBuddyChatDlg::OnClose()
{
	DestroyWindow();
}

void CBuddyChatDlg::OnDestroy()
{
	SetMsgHandled(FALSE);

	CloseMsgLogBrowser();

	m_lpCascadeWinManager->Del(m_hWnd);

	UnInit();	// ����ʼ���ؼ�

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

	if (m_fontBuddyNameLink != NULL)
	{
		BOOL bRet = ::DeleteObject(m_fontBuddyNameLink);
		m_fontBuddyNameLink = NULL;
	}

	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
}

// ���������ơ������ӿؼ�
void CBuddyChatDlg::OnLnk_BuddyName(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	::PostMessage(m_hMainDlg, WM_SHOW_BUDDYINFODLG, NULL, m_nQQUin);
}

// ������ѡ�񹤾�������ť
void CBuddyChatDlg::OnBtn_Font(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (BN_PUSHED == uNotifyCode)
	{
		if (!m_FontSelDlg.IsWindow())
			m_FontSelDlg.Create(m_hWnd);

		CRect rcClient;
		GetClientRect(&rcClient);
		m_richRecv.MoveWindow(6, rcClient.top+44+44, rcClient.Width()-6-152, rcClient.Height()-243-32);
		m_FontSelDlg.MoveWindow(1, rcClient.bottom-187, rcClient.Width()-1-152, 32);
		m_FontSelDlg.ShowWindow(SW_SHOW);
	}
	else if (BN_UNPUSHED == uNotifyCode)
	{
		m_FontSelDlg.ShowWindow(SW_HIDE);
		CRect rcClient;
		GetClientRect(&rcClient);
		m_richRecv.MoveWindow(6, rcClient.top+44+44, rcClient.Width()-6-152, rcClient.Height()-243);
	}
}

// �����顱��ť
void CBuddyChatDlg::OnBtn_Face(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (BN_PUSHED == uNotifyCode)
	{
		m_FaceSelDlg.SetFaceList(m_lpFaceList);
		if (!m_FaceSelDlg.IsWindow())
		{
			m_FaceSelDlg.Create(m_hWnd);

			CRect rcBtn;
			m_tbMid.GetItemRectByIndex(1, rcBtn);
			m_tbMid.ClientToScreen(&rcBtn);

			int cx = 432;
			int cy = 236;
			int x = rcBtn.left - cx / 2;
			int y = rcBtn.top - cy;

			m_FaceSelDlg.SetWindowPos(NULL, x, y, cx, cy, NULL);
			m_FaceSelDlg.ShowWindow(SW_SHOW);
		}
	}
	else if (BN_UNPUSHED == uNotifyCode)
	{

	}
}

// ������ͼƬ����ť
void CBuddyChatDlg::OnBtn_Image(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	BOOL bOpenFileDialog = TRUE;
	LPCTSTR lpszDefExt = NULL;
	LPCTSTR lpszFileName = NULL;
	DWORD dwFlags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR|OFN_EXTENSIONDIFFERENT;
	LPCTSTR lpszFilter = _T("ͼ���ļ�(*.bmp;*.jpg;*.jpeg;*.gif;*.png)\0*.bmp;*.jpg;*.jpeg;*.gif;*.png\0\0");;
	HWND hWndParent = m_hWnd;

	CFileDialog fileDlg(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, hWndParent);
	fileDlg.m_ofn.lpstrTitle = _T("��ͼƬ");
	if (fileDlg.DoModal() == IDOK)
	{
		_RichEdit_InsertFace(m_richSend.m_hWnd, fileDlg.m_ofn.lpstrFile, -1, -1);
		m_richSend.SetFocus();
	}
}

// ����Ϣ��¼����ť
void CBuddyChatDlg::OnBtn_MsgLog(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (uNotifyCode == BN_CLICKED)
	{
		BOOL bChecked = m_tbMid.GetItemCheckState(13);
		if (!bChecked)
		{
			CloseMsgLogBrowser();
			return;
		}

		OpenMsgLogBrowser();
	}
}

// ��������Ϊ����ť
void CBuddyChatDlg::OnBtn_SaveAs(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	OnMenu_SaveAs(uNotifyCode, nID, wndCtl);
}

// ���رա���ť
void CBuddyChatDlg::OnBtn_Close(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	PostMessage(WM_CLOSE);
}

// �����͡���ť
void CBuddyChatDlg::OnBtn_Send(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	if (NULL == m_lpQQClient)
		return;

	int nCustomPicCnt = RichEdit_GetCustomPicCount(m_richSend.m_hWnd);
	if (nCustomPicCnt > 1)
	{
		MessageBox(_T("ÿ����Ϣ������1��ͼƬ������ͼƬ��������͡�"), _T("��ʾ"));
		return;
	}

	tstring strText;
	RichEdit_GetText(m_richSend.m_hWnd, strText);

	if (strText.size() <= 0)
		return;

	time_t nTime = m_lpQQClient->GetServerTime();

	AddMsgToRecvEdit(nTime, strText.c_str());

	CFontInfo fontInfo = m_FontSelDlg.GetFontInfo();

	TCHAR szColor[32] = {0};
	RGBToHexStr(fontInfo.m_clrText, szColor, sizeof(szColor)/sizeof(TCHAR));

	TCHAR szFontInfo[1024] = {0};
	LPCTSTR lpFontFmt = _T("/o[\"%s,%d,%s,%d,%d,%d\"]");
	wsprintf(szFontInfo, lpFontFmt, fontInfo.m_strName.c_str(), fontInfo.m_nSize, 
		szColor, fontInfo.m_bBold, fontInfo.m_bItalic, fontInfo.m_bUnderLine);

	strText += szFontInfo;

	m_lpQQClient->SendBuddyMsg(m_nQQUin, nTime, strText.c_str());

	m_richSend.SetWindowText(_T(""));
	m_richSend.SetFocus();
}

// ����ͷ����ť
void CBuddyChatDlg::OnBtn_Arrow(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	CSkinMenu PopupMenu = m_SkinMenu.GetSubMenu(8);
	if (PopupMenu.IsMenu())
	{
		CRect rc;
		m_btnArrow.GetClientRect(&rc);
		m_btnArrow.ClientToScreen(&rc);
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, 
			rc.left, rc.bottom + 4, m_hWnd, &rc);
	}
}

LRESULT CBuddyChatDlg::OnToolbarDropDown(LPNMHDR pnmh)
{
	NMTOOLBAR * pnmtb = (NMTOOLBAR *)pnmh;
	CSkinMenu PopupMenu;
	int nIndex = -1;
	CRect rc(pnmtb->rcButton);

	switch (pnmtb->iItem)
	{
	case 101:
		nIndex = 0;
		m_tbTop.ClientToScreen(&rc);
		break;

	case 102:
		nIndex = 1;
		m_tbTop.ClientToScreen(&rc);
		break;

	case 103:
		nIndex = 2;
		m_tbTop.ClientToScreen(&rc);
		break;

	case 106:
		nIndex = 3;
		m_tbTop.ClientToScreen(&rc);
		break;

	case 107:
		nIndex = 4;
		m_tbTop.ClientToScreen(&rc);
		break;

	case 211:
		nIndex = 5;
		m_tbMid.ClientToScreen(&rc);
		break;

	case 212:
		nIndex = 6;
		m_tbMid.ClientToScreen(&rc);
		break;

	case 214:
		nIndex = 7;
		m_tbMid.ClientToScreen(&rc);
		break;

	default:
		return 0;
	}

	PopupMenu = m_SkinMenu.GetSubMenu(nIndex);
	if (PopupMenu.IsMenu())
	{
		PopupMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_VERTICAL, 
			rc.left, rc.bottom + 4, m_hWnd, &rc);
	}

	return 0;
}

// ����������Ϣ
LRESULT CBuddyChatDlg::OnUpdateFontInfo(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	CFontInfo fontInfo = m_FontSelDlg.GetFontInfo();
	RichEdit_SetDefFont(m_richSend.m_hWnd, fontInfo.m_strName.c_str(),
		fontInfo.m_nSize, fontInfo.m_clrText, fontInfo.m_bBold,
		fontInfo.m_bItalic, fontInfo.m_bUnderLine, FALSE);
	return 0;
}

// �����顱�ؼ�ѡȡ��Ϣ
LRESULT CBuddyChatDlg::OnFaceCtrlSel(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nFaceId = m_FaceSelDlg.GetSelFaceId();
	int nFaceIndex = m_FaceSelDlg.GetSelFaceIndex();
	CString strFileName = m_FaceSelDlg.GetSelFaceFileName();
	if (!strFileName.IsEmpty())
	{
		_RichEdit_InsertFace(m_richSend.m_hWnd, strFileName, nFaceId, nFaceIndex);
		m_richSend.SetFocus();
	}

	m_tbMid.SetItemCheckState(1, FALSE);
	m_tbMid.Invalidate();

	return 0;
}

// ���öԻ����ʼ����
LRESULT CBuddyChatDlg::OnSetDlgInitFocus(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	m_richSend.SetFocus();
	return 0;
}

//	��������Ϣ�����ı������ӵ����Ϣ
LRESULT CBuddyChatDlg::OnRichEdit_Recv_Link(LPNMHDR pnmh)
{
	CString strUrl;

	if (pnmh->code == EN_LINK)
	{
		ENLINK *pLink = (ENLINK *)pnmh;
		if (pLink->msg == WM_LBUTTONUP)
		{
			m_richRecv.SetSel(pLink->chrg);
			m_richRecv.GetSelText(strUrl);

			if (strUrl.Left(7).MakeLower() == _T("http://"))
			{
				::ShellExecute(NULL, _T("open"), strUrl, NULL, NULL, SW_SHOWNORMAL);
			}
		}
	}
	return 0;
}

LRESULT CBuddyChatDlg::OnRichEdit_Send_Paste(LPNMHDR pnmh)
{
	NMRICHEDITOLECALLBACK * lpOleNotify = (NMRICHEDITOLECALLBACK *)pnmh;
	if (lpOleNotify != NULL && lpOleNotify->hdr.code == EN_PASTE
		&& lpOleNotify->hdr.hwndFrom == m_richSend.m_hWnd)
	{
		AddMsgToSendEdit(lpOleNotify->lpszText);
	}
	return 0;
}

// �����С��˵�
void CBuddyChatDlg::OnMenu_Cut(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richSend.Cut();
}

// �����ơ��˵�
void CBuddyChatDlg::OnMenu_Copy(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	HWND hWnd = GetFocus();
	if (hWnd == m_richSend.m_hWnd)
	{
		m_richSend.Copy();
	}
	else if (hWnd == m_richRecv.m_hWnd)
	{
		m_richRecv.Copy();
	}
}

// ��ճ�����˵�
void CBuddyChatDlg::OnMenu_Paste(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richSend.PasteSpecial(CF_TEXT);
}

// ��ȫ��ѡ�񡱲˵�
void CBuddyChatDlg::OnMenu_SelAll(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	HWND hWnd = GetFocus();
	if (hWnd == m_richSend.m_hWnd)
	{
		m_richSend.SetSel(0, -1);
	}
	else if (hWnd == m_richRecv.m_hWnd)
	{
		m_richRecv.SetSel(0, -1);
	}
}

// ���������˵�
void CBuddyChatDlg::OnMenu_Clear(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	m_richRecv.SetWindowText(_T(""));
}

// ����ʾ�������˵�
void CBuddyChatDlg::OnMenu_ZoomRatio(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	switch (nID)
	{
	case ID_MENU_ZOOMRATIO_400:
		::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 16, 4);
		break;
	case ID_MENU_ZOOMRATIO_200:
		::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 8, 4);
		break;
	case ID_MENU_ZOOMRATIO_150:
		::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 6, 4);
		break;
	case ID_MENU_ZOOMRATIO_125:
		::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 5, 4);
		break;
	case ID_MENU_ZOOMRATIO_100:
		::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 0, 0);
		break;
	case ID_MENU_ZOOMRATIO_75:
		::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 3, 4);
		break;
	case ID_MENU_ZOOMRATIO_50:
		::SendMessage(m_richRecv.m_hWnd, EM_SETZOOM, 1, 2);
		break;
	default:
		return;
	}

	CSkinMenu menuPopup = m_SkinMenu.GetSubMenu(10);
	for (int i = ID_MENU_ZOOMRATIO_400; i <= ID_MENU_ZOOMRATIO_50; i++)
	{
		if (i != nID)
			menuPopup.CheckMenuItem(i, MF_BYCOMMAND|MF_UNCHECKED);
		else
			menuPopup.CheckMenuItem(i, MF_BYCOMMAND|MF_CHECKED);
	}	
}

// �����Ϊ���˵�
void CBuddyChatDlg::OnMenu_SaveAs(UINT uNotifyCode, int nID, CWindow wndCtl)
{
	IImageOle * pImageOle = NULL;
	BOOL bRet = RichEdit_GetImageOle(m_hRBtnDownWnd, m_ptRBtnDown, &pImageOle);
	if (!bRet || NULL == pImageOle)
		return;

	CString strFileName;

	BSTR bstrFileName = NULL;
	HRESULT hr = pImageOle->GetFileName(&bstrFileName);
	if (SUCCEEDED(hr))
		strFileName = bstrFileName;
	if (bstrFileName != NULL)
		::SysFreeString(bstrFileName);

	TCHAR cFileName[MAX_PATH] = {0};
	BOOL bOpenFileDialog = FALSE;
	LPCTSTR lpszDefExt;
	LPCTSTR lpszFileName = _T("δ����");
	DWORD dwFlags = OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT|OFN_NOCHANGEDIR|OFN_EXTENSIONDIFFERENT;
	LPCTSTR lpszFilter;
	HWND hWndParent = m_hWnd;

	GUID guid = {0};
	hr = pImageOle->GetRawFormat(&guid);

	if (InlineIsEqualGUID(guid, Gdiplus::ImageFormatJPEG))
	{
		lpszDefExt = _T(".jpg");
		lpszFilter = _T("ͼ���ļ�(*.jpg)\0*.jpg\0ͼ���ļ�(*.bmp)\0*.bmp\0\0");
	}
	else if (InlineIsEqualGUID(guid, Gdiplus::ImageFormatPNG))
	{
		lpszDefExt = _T(".png");
		lpszFilter = _T("ͼ���ļ�(*.png)\0*.png\0\0");
	}
	else if (InlineIsEqualGUID(guid, Gdiplus::ImageFormatGIF))
	{
		lpszDefExt = _T(".gif");
		lpszFilter = _T("ͼ���ļ�(*.gif)\0*.gif\0ͼ���ļ�(*.jpg)\0*.jpg\0ͼ���ļ�(*.bmp)\0*.bmp\0\0");
	}
	else
	{
		lpszDefExt = _T(".jpg");
		lpszFilter = _T("ͼ���ļ�(*.jpg)\0*.jpg\0ͼ���ļ�(*.bmp)\0*.bmp\0\0");
	}

	CFileDialog fileDlg(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, hWndParent);
	fileDlg.m_ofn.lpstrTitle = _T("����ͼƬ");
	if (fileDlg.DoModal() == IDOK)
	{
		CString strSavePath = fileDlg.m_ofn.lpstrFile;
		CString strExtName = (_T(".") + ZYM::CPath::GetExtension(strSavePath)).c_str();
		GUID guid2 = GetFileTypeGuidByExtension(strExtName);

		if (InlineIsEqualGUID(guid, guid2))
		{
			CopyFile(strFileName, strSavePath, FALSE);
		}
		else
		{
			BSTR bstrSavePath = ::SysAllocString(strSavePath);
			if (bstrSavePath != NULL)
			{
				pImageOle->SaveAsFile(bstrSavePath);
				::SysFreeString(bstrSavePath);
			}
		}
	}

	if (pImageOle != NULL)
		pImageOle->Release();
}

// ����/�����ı��������ƶ���Ϣ
BOOL CBuddyChatDlg::OnRichEdit_MouseMove(MSG * pMsg)
{
	IImageOle * pNewImageOle = NULL;
	RECT rc = {0};

	POINT pt = {LOWORD(pMsg->lParam), HIWORD(pMsg->lParam)};
	IImageOle * pImageOle = NULL;
	BOOL bRet = RichEdit_GetImageOle(pMsg->hwnd, pt, &pImageOle);
	if (bRet && pImageOle != NULL)
	{
		pNewImageOle = pImageOle;
		pImageOle->GetObjectRect(&rc);
	}
	if (pImageOle != NULL)
		pImageOle->Release();

	if (m_pLastImageOle != pNewImageOle)
	{
		m_pLastImageOle = pNewImageOle;
		if (m_pLastImageOle != NULL)
		{
			m_hRBtnDownWnd = pMsg->hwnd;
			m_ptRBtnDown = pt;

			if (!m_PicBarDlg.IsWindow())
				m_PicBarDlg.Create(m_hWnd);

			RECT rc2 = {0};
			::GetClientRect(pMsg->hwnd, &rc2);
			POINT pt = {rc.right, rc.bottom-m_cyPicBarDlg};
			if (pt.x < rc2.left)
				pt.x = rc2.left;
			if (pt.x > rc2.right)
				pt.x = rc2.right;
			if (pt.y > rc2.bottom-m_cyPicBarDlg)
				pt.y = rc2.bottom-m_cyPicBarDlg;
			::ClientToScreen(pMsg->hwnd, &pt);

			::SetWindowPos(m_PicBarDlg.m_hWnd, NULL, pt.x, pt.y, m_cxPicBarDlg, m_cyPicBarDlg, SWP_NOACTIVATE|SWP_SHOWWINDOW);
		}
		else
		{
			::ShowWindow(m_PicBarDlg.m_hWnd, SW_HIDE);
		}
	}
	return FALSE;
}

// ����/�����ı�������˫����Ϣ
BOOL CBuddyChatDlg::OnRichEdit_LBtnDblClk(MSG * pMsg)
{
	POINT pt = {GET_X_LPARAM(pMsg->lParam), GET_Y_LPARAM(pMsg->lParam)};

	IImageOle * pImageOle = NULL;
	BOOL bRet = RichEdit_GetImageOle(pMsg->hwnd, pt, &pImageOle);
	if (bRet && pImageOle != NULL)					// ˫������ͼƬ
	{
		LONG nFaceId = -1, nFaceIndex = -1;
		pImageOle->GetFaceId(&nFaceId);
		pImageOle->GetFaceIndex(&nFaceIndex);
		if (-1 == nFaceId && -1 == nFaceIndex)		// ��ϵͳ����
		{
			BSTR bstrFileName = NULL;				// ��ȡͼƬ�ļ���
			HRESULT hr = pImageOle->GetFileName(&bstrFileName);
			if (SUCCEEDED(hr))						// ����ͼƬ����������ͼƬ
			{
				CString strExeName = ZYM::CPath::GetAppPath().c_str();
				strExeName += _T("ImageView.exe");

				CString strArg = _T("\"");
				strArg += bstrFileName;
				strArg += _T("\"");

				if (ZYM::CPath::IsFileExist(strExeName))
				{
					HWND hWnd = ::FindWindow(_T("DUI_WINDOW"), _T("ImageView"));
					if (::IsWindow(hWnd))
						::SendMessage(hWnd, WM_CLOSE, 0, 0);
					::ShellExecute(NULL, NULL, strExeName, strArg, NULL, SW_SHOWNORMAL);
				}
				else
					::ShellExecute(NULL, _T("open"), bstrFileName, NULL, NULL, SW_SHOWNORMAL);
			}
			if (bstrFileName != NULL)
				::SysFreeString(bstrFileName);
		}
	}
	if (pImageOle != NULL)
		pImageOle->Release();

	return bRet;
}

// ����/�����ı��������Ҽ�������Ϣ
BOOL CBuddyChatDlg::OnRichEdit_RBtnDown(MSG * pMsg)
{
	if (pMsg->hwnd == m_richSend.m_hWnd)
	{
		m_hRBtnDownWnd = pMsg->hwnd;
		m_ptRBtnDown.x = GET_X_LPARAM(pMsg->lParam);
		m_ptRBtnDown.y = GET_Y_LPARAM(pMsg->lParam);

		CSkinMenu menuPopup = m_SkinMenu.GetSubMenu(9);

		UINT nSel = ((m_richSend.GetSelectionType() != SEL_EMPTY) ? 0 : MF_GRAYED);
		menuPopup.EnableMenuItem(ID_MENU_CUT, MF_BYCOMMAND|nSel);
		menuPopup.EnableMenuItem(ID_MENU_COPY, MF_BYCOMMAND|nSel);

		UINT nPaste = (m_richSend.CanPaste() ? 0 : MF_GRAYED) ;
		menuPopup.EnableMenuItem(ID_MENU_PASTE, MF_BYCOMMAND|nPaste);

		IImageOle * pImageOle = NULL;
		BOOL bRet = RichEdit_GetImageOle(pMsg->hwnd, m_ptRBtnDown, &pImageOle);
		UINT nSaveAs = ((bRet && pImageOle != NULL) ? 0 : MF_GRAYED) ;
		menuPopup.EnableMenuItem(ID_MENU_SAVEAS, MF_BYCOMMAND|nSaveAs);
		if (pImageOle != NULL)
			pImageOle->Release();

		menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			pMsg->pt.x, pMsg->pt.y, m_hWnd, NULL);
	}
	else if (pMsg->hwnd == m_richRecv.m_hWnd)
	{
		m_hRBtnDownWnd = pMsg->hwnd;
		m_ptRBtnDown.x = GET_X_LPARAM(pMsg->lParam);
		m_ptRBtnDown.y = GET_Y_LPARAM(pMsg->lParam);

		CSkinMenu menuPopup = m_SkinMenu.GetSubMenu(10);

		UINT nSel = ((m_richRecv.GetSelectionType() != SEL_EMPTY) ? 0 : MF_GRAYED);
		menuPopup.EnableMenuItem(ID_MENU_COPY, MF_BYCOMMAND|nSel);
		//menuPopup.EnableMenuItem(ID_MENU_CLEAR, MF_BYCOMMAND|nSel);

		IImageOle * pImageOle = NULL;
		BOOL bRet = RichEdit_GetImageOle(pMsg->hwnd, m_ptRBtnDown, &pImageOle);
		UINT nSaveAs = ((bRet && pImageOle != NULL) ? 0 : MF_GRAYED) ;
		menuPopup.EnableMenuItem(ID_MENU_SAVEAS, MF_BYCOMMAND|nSaveAs);
		if (pImageOle != NULL)
			pImageOle->Release();

		menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			pMsg->pt.x, pMsg->pt.y, m_hWnd, NULL);
	}
	return FALSE;
}

// ��ȡ������Ϣָ��
CBuddyInfo * CBuddyChatDlg::GetBuddyInfoPtr()
{
	if (m_lpQQClient != NULL)
	{
		CBuddyList * lpBuddyList = m_lpQQClient->GetBuddyList();
		if (lpBuddyList != NULL)
			return lpBuddyList->GetBuddy(m_nQQUin);
	}
	return NULL;
}

// ��ȡ�û���Ϣָ��
CBuddyInfo * CBuddyChatDlg::GetUserInfoPtr()
{
	if (m_lpQQClient != NULL)
		return m_lpQQClient->GetUserInfo();
	else
		return NULL;
}

// ��������
void CBuddyChatDlg::UpdateData()
{
	CBuddyInfo * lpBuddyInfo = GetBuddyInfoPtr();
	if (lpBuddyInfo != NULL)
	{
		m_nQQNumber = lpBuddyInfo->m_nQQNum;
		if (!lpBuddyInfo->m_strMarkName.empty())
			m_strBuddyName = lpBuddyInfo->m_strMarkName.c_str();
		else
			m_strBuddyName = lpBuddyInfo->m_strNickName.c_str();
		if (!lpBuddyInfo->m_strSign.empty())
			m_strBuddySign = lpBuddyInfo->m_strSign.c_str();
		else
			m_strBuddySign = _T("��һ����,ʲô��û�����¡�");
	}

	CBuddyInfo * lpUserInfo = GetUserInfoPtr();
	if (lpUserInfo != NULL)
	{
		m_nUserNumber = lpUserInfo->m_nQQNum;
		m_strUserName = lpUserInfo->m_strNickName.c_str();
	}
}

// ���¶Ի��������
void CBuddyChatDlg::UpdateDlgTitle()
{
	SetWindowText(m_strBuddyName);
}

// ���º������ƿؼ�
void CBuddyChatDlg::UpdateBuddyNameCtrl()
{
	CString strText;
	if (m_nQQNumber != 0)
		strText.Format(_T("%s(%u)"), m_strBuddyName, m_nQQNumber);
	else
		strText.Format(_T("%s"), m_strBuddyName);
	m_lnkBuddyName.SetLabel(strText);
}

// ���º���ǩ���ؼ�
void CBuddyChatDlg::UpdateBuddySignCtrl()
{
	m_staBuddySign.SetWindowText(m_strBuddySign);
}

// ��ʼ��Top������
BOOL CBuddyChatDlg::InitTopToolBar()
{
	int nIndex = m_tbTop.AddItem(101, STBI_STYLE_DROPDOWN);
	m_tbTop.SetItemSize(nIndex, 52, 40, 42, 10);
	m_tbTop.SetItemPadding(nIndex, 1);
	m_tbTop.SetItemToolTipText(nIndex, _T("��ʼ��Ƶ�Ự"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemLeftBgPic(nIndex, _T("aio_toolbar_leftnormal.png"), 
		_T("aio_toolbar_leftdown.png"), CRect(0,0,0,0));
	m_tbTop.SetItemRightBgPic(nIndex, _T("aio_toolbar_rightnormal.png"), 
		_T("aio_toolbar_rightdown.png"), CRect(0,0,0,0));
	m_tbTop.SetItemArrowPic(nIndex, _T("aio_littletoolbar_arrow.png"));
	m_tbTop.SetItemIconPic(nIndex, _T("BuddyTopToolBar\\video.png"));

	nIndex = m_tbTop.AddItem(102, STBI_STYLE_DROPDOWN);
	m_tbTop.SetItemSize(nIndex, 52, 40, 42, 10);
	m_tbTop.SetItemPadding(nIndex, 1);
	m_tbTop.SetItemToolTipText(nIndex, _T("��ʼ�����Ự"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemLeftBgPic(nIndex, _T("aio_toolbar_leftnormal.png"), 
		_T("aio_toolbar_leftdown.png"), CRect(0,0,0,0));
	m_tbTop.SetItemRightBgPic(nIndex, _T("aio_toolbar_rightnormal.png"), 
		_T("aio_toolbar_rightdown.png"), CRect(0,0,0,0));
	m_tbTop.SetItemArrowPic(nIndex, _T("aio_littletoolbar_arrow.png"));
	m_tbTop.SetItemIconPic(nIndex, _T("BuddyTopToolBar\\audio.png"));

	nIndex = m_tbTop.AddItem(103, STBI_STYLE_DROPDOWN);
	m_tbTop.SetItemSize(nIndex, 52, 40, 42, 10);
	m_tbTop.SetItemPadding(nIndex, 1);
	m_tbTop.SetItemToolTipText(nIndex, _T("�����ļ�"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemLeftBgPic(nIndex, _T("aio_toolbar_leftnormal.png"), 
		_T("aio_toolbar_leftdown.png"), CRect(0,0,0,0));
	m_tbTop.SetItemRightBgPic(nIndex, _T("aio_toolbar_rightnormal.png"), 
		_T("aio_toolbar_rightdown.png"), CRect(0,0,0,0));
	m_tbTop.SetItemArrowPic(nIndex, _T("aio_littletoolbar_arrow.png"));
	m_tbTop.SetItemIconPic(nIndex, _T("BuddyTopToolBar\\sendfile.png"));

	nIndex = m_tbTop.AddItem(104, STBI_STYLE_BUTTON);
	m_tbTop.SetItemSize(nIndex, 36, 40);
	m_tbTop.SetItemPadding(nIndex, 2);
	m_tbTop.SetItemToolTipText(nIndex, _T("���Ͷ���"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemIconPic(nIndex, _T("BuddyTopToolBar\\sendsms.png"));

	nIndex = m_tbTop.AddItem(105, STBI_STYLE_BUTTON);
	m_tbTop.SetItemSize(nIndex, 36, 40);
	m_tbTop.SetItemPadding(nIndex, 2);
	m_tbTop.SetItemToolTipText(nIndex, _T("����������"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemIconPic(nIndex, _T("BuddyTopToolBar\\create_disc_group.png"));

	nIndex = m_tbTop.AddItem(106, STBI_STYLE_WHOLEDROPDOWN);
	m_tbTop.SetItemSize(nIndex, 44, 40, 34, 10);
	m_tbTop.SetItemPadding(nIndex, 2);
	m_tbTop.SetItemToolTipText(nIndex, _T("�ٱ�"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemArrowPic(nIndex, _T("aio_littletoolbar_arrow.png"));
	m_tbTop.SetItemIconPic(nIndex, _T("BuddyTopToolBar\\report.png"));

	nIndex = m_tbTop.AddItem(107, STBI_STYLE_WHOLEDROPDOWN);
	m_tbTop.SetItemSize(nIndex, 44, 40, 34, 10);
	m_tbTop.SetItemPadding(nIndex, 2);
	m_tbTop.SetItemToolTipText(nIndex, _T("Ӧ��"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemArrowPic(nIndex, _T("aio_littletoolbar_arrow.png"));
	m_tbTop.SetItemIconPic(nIndex, _T("BuddyTopToolBar\\app.png"));

	nIndex = m_tbTop.AddItem(108, STBI_STYLE_BUTTON);
	m_tbTop.SetItemSize(nIndex, 36, 40);
	m_tbTop.SetItemPadding(nIndex, 2);
	m_tbTop.SetItemToolTipText(nIndex, _T("Զ��Э��"));
	m_tbTop.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbTop.SetItemIconPic(nIndex, _T("BuddyTopToolBar\\remote_assistance.png"));

	m_tbTop.SetLeftTop(2, 2);
	m_tbTop.SetTransparent(TRUE, m_SkinDlg.GetBgDC());

	CRect rcTopToolBar(3, 45, 3+386, 45+44);
	m_tbTop.Create(m_hWnd, rcTopToolBar, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOLBAR_TOP);

	return TRUE;
}

// ��ʼ��Middle������
BOOL CBuddyChatDlg::InitMidToolBar()
{
	int nIndex = m_tbMid.AddItem(201, STBI_STYLE_BUTTON|STBI_STYLE_CHECK);
	m_tbMid.SetItemSize(nIndex, 24, 20);
	m_tbMid.SetItemPadding(nIndex, 1);
	m_tbMid.SetItemToolTipText(nIndex, _T("����ѡ�񹤾���"));
	m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_font.png"));

	nIndex = m_tbMid.AddItem(202, STBI_STYLE_BUTTON|STBI_STYLE_CHECK);
	m_tbMid.SetItemSize(nIndex, 24, 20);
	m_tbMid.SetItemPadding(nIndex, 1);
	m_tbMid.SetItemToolTipText(nIndex, _T("ѡ�����"));
	m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_face.png"));

	nIndex = m_tbMid.AddItem(203, STBI_STYLE_BUTTON);
	m_tbMid.SetItemSize(nIndex, 24, 20);
	m_tbMid.SetItemPadding(nIndex, 1);
	m_tbMid.SetItemToolTipText(nIndex, _T("��Աħ������/��������/Ϳѻ����/������"));
	m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_richface.png"));

	nIndex = m_tbMid.AddItem(204, STBI_STYLE_BUTTON);
	m_tbMid.SetItemSize(nIndex, 24, 20);
	m_tbMid.SetItemPadding(nIndex, 1);
	m_tbMid.SetItemToolTipText(nIndex, _T("����ѷ��ʹ��ڶ���"));
	m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_twitter.png"));

	nIndex = m_tbMid.AddItem(205, STBI_STYLE_BUTTON);
	m_tbMid.SetItemSize(nIndex, 24, 20);
	m_tbMid.SetItemPadding(nIndex, 1);
	m_tbMid.SetItemToolTipText(nIndex, _T("ѡ��һ�±���"));
	m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_flirtationface.png"));

	nIndex = m_tbMid.AddItem(206, STBI_STYLE_BUTTON|STBI_STYLE_CHECK);
	m_tbMid.SetItemSize(nIndex, 24, 20);
	m_tbMid.SetItemPadding(nIndex, 2);
	m_tbMid.SetItemToolTipText(nIndex, _T("�๦�ܸ�������"));
	m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\QQIme.png"));

	nIndex = m_tbMid.AddItem(207, STBI_STYLE_SEPARTOR);
	m_tbMid.SetItemSize(nIndex, 2, 20);
	m_tbMid.SetItemPadding(nIndex, 1);
	m_tbMid.SetItemSepartorPic(nIndex, _T("aio_qzonecutline_normal.png"));

	nIndex = m_tbMid.AddItem(208, STBI_STYLE_BUTTON);
	m_tbMid.SetItemSize(nIndex, 24, 20);
	m_tbMid.SetItemPadding(nIndex, 1);
	m_tbMid.SetItemToolTipText(nIndex, _T("����ͼƬ"));
	m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_sendpic.png"));

	nIndex = m_tbMid.AddItem(209, STBI_STYLE_BUTTON);
	m_tbMid.SetItemSize(nIndex, 24, 20);
	m_tbMid.SetItemPadding(nIndex, 1);
	m_tbMid.SetItemToolTipText(nIndex, _T("���"));
	m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\musicsharebtn20.png"));

	nIndex = m_tbMid.AddItem(210, STBI_STYLE_BUTTON);
	m_tbMid.SetItemSize(nIndex, 24, 20);
	m_tbMid.SetItemPadding(nIndex, 1);
	m_tbMid.SetItemToolTipText(nIndex, _T("�������ͷ�����ͷݾ�ϲ"));
	m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\present.png"));

	nIndex = m_tbMid.AddItem(211, STBI_STYLE_DROPDOWN);
	m_tbMid.SetItemSize(nIndex, 31, 20, 23, 8);
	m_tbMid.SetItemPadding(nIndex, 1);
	m_tbMid.SetItemToolTipText(nIndex, _T("��Ļ��ͼ Ctrl + Alt + A"));
	m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbMid.SetItemLeftBgPic(nIndex, _T("aio_toolbar_leftnormal.png"), 
		_T("aio_toolbar_leftdown.png"), CRect(1,0,0,0));
	m_tbMid.SetItemRightBgPic(nIndex, _T("aio_toolbar_rightnormal.png"), 
		_T("aio_toolbar_rightdown.png"), CRect(0,0,1,0));
	m_tbMid.SetItemArrowPic(nIndex, _T("aio_littletoolbar_arrow.png"));
	m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_cut.png"));

	nIndex = m_tbMid.AddItem(212, STBI_STYLE_DROPDOWN);
	m_tbMid.SetItemSize(nIndex, 31, 20, 23, 8);
	m_tbMid.SetItemPadding(nIndex, 2);
	m_tbMid.SetItemToolTipText(nIndex, _T("��������"));
	m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbMid.SetItemLeftBgPic(nIndex, _T("aio_toolbar_leftnormal.png"), 
		_T("aio_toolbar_leftdown.png"), CRect(1,0,0,0));
	m_tbMid.SetItemRightBgPic(nIndex, _T("aio_toolbar_rightnormal.png"), 
		_T("aio_toolbar_rightdown.png"), CRect(0,0,1,0));
	m_tbMid.SetItemArrowPic(nIndex, _T("aio_littletoolbar_arrow.png"));
	m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\SoSo.png"));

	nIndex = m_tbMid.AddItem(213, STBI_STYLE_SEPARTOR);
	m_tbMid.SetItemSize(nIndex, 2, 20);
	m_tbMid.SetItemPadding(nIndex, 1);
	m_tbMid.SetItemSepartorPic(nIndex, _T("aio_qzonecutline_normal.png"));

	nIndex = m_tbMid.AddItem(214, STBI_STYLE_DROPDOWN|STBI_STYLE_CHECK);
	m_tbMid.SetItemSize(nIndex, 82, 20, 74, 8);
	m_tbMid.SetItemPadding(nIndex, 2);
	m_tbMid.SetItemText(nIndex, _T("��Ϣ��¼"));
	m_tbMid.SetItemToolTipText(nIndex, _T("��ʾ��Ϣ��¼"));
	m_tbMid.SetItemBgPic(nIndex, NULL, _T("aio_toolbar_highligh.png"), 
		_T("aio_toolbar_down.png"), CRect(3,3,3,3));
	m_tbMid.SetItemLeftBgPic(nIndex, _T("aio_toolbar_leftnormal.png"), 
		_T("aio_toolbar_leftdown.png"), CRect(1,0,0,0));
	m_tbMid.SetItemRightBgPic(nIndex, _T("aio_toolbar_rightnormal.png"), 
		_T("aio_toolbar_rightdown.png"), CRect(0,0,1,0));
	m_tbMid.SetItemArrowPic(nIndex, _T("aio_littletoolbar_arrow.png"));
	m_tbMid.SetItemIconPic(nIndex, _T("MidToolBar\\aio_quickbar_register.png"));

	m_tbMid.SetLeftTop(2, 4);
	m_tbMid.SetBgPic(_T("MidToolBar\\bg.png"), CRect(0,0,0,0));

	CRect rcClient;
	GetClientRect(&rcClient);

	CRect rcMidToolBar(1, rcClient.bottom-155, 1+(rcClient.Width()-1-152), (rcClient.bottom-155)+27);
	m_tbMid.Create(m_hWnd, rcMidToolBar, NULL, WS_CHILD|WS_VISIBLE, NULL, ID_TOOLBAR_MID);

	return TRUE;
}

// ��ʼ��IRichEditOleCallback�ӿ�
BOOL CBuddyChatDlg::InitRichEditOleCallback()
{
	IRichEditOleCallback2* pRichEditOleCallback2 = NULL;
	HRESULT hr = ::CoCreateInstance(CLSID_ImageOle, NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IRichEditOleCallback2), (void**)&pRichEditOleCallback2);
	if (SUCCEEDED(hr))
	{
		pRichEditOleCallback2->SetNotifyHwnd(m_hWnd);
		pRichEditOleCallback2->SetRichEditHwnd(m_richRecv.m_hWnd);
		m_richRecv.SetOleCallback(pRichEditOleCallback2);
		pRichEditOleCallback2->Release();
	}

	pRichEditOleCallback2 = NULL;
	hr = ::CoCreateInstance(CLSID_ImageOle, NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IRichEditOleCallback2), (void**)&pRichEditOleCallback2);
	if (SUCCEEDED(hr))
	{
		pRichEditOleCallback2->SetNotifyHwnd(m_hWnd);
		pRichEditOleCallback2->SetRichEditHwnd(m_richSend.m_hWnd);
		m_richSend.SetOleCallback(pRichEditOleCallback2);
		pRichEditOleCallback2->Release();
	}

	return SUCCEEDED(hr);
}

// ��ʼ���ؼ�
BOOL CBuddyChatDlg::Init()
{
	m_SkinDlg.SubclassWindow(m_hWnd);
	m_SkinDlg.SetBgPic(_T("BuddyChatDlgBg.png"), CRect(40, 86, 152, 155));
	m_SkinDlg.SetMinSysBtnPic(_T("SysBtn\\btn_mini_normal.png"), 
		_T("SysBtn\\btn_mini_highlight.png"), _T("SysBtn\\btn_mini_down.png"));
	m_SkinDlg.SetMaxSysBtnPic(_T("SysBtn\\btn_max_normal.png"), 
		_T("SysBtn\\btn_max_highlight.png"), _T("SysBtn\\btn_max_down.png"));
	m_SkinDlg.SetRestoreSysBtnPic(_T("SysBtn\\btn_restore_normal.png"), 
		_T("SysBtn\\btn_restore_highlight.png"), _T("SysBtn\\btn_restore_down.png"));
	m_SkinDlg.SetCloseSysBtnPic(_T("SysBtn\\btn_close_normal.png"), 
		_T("SysBtn\\btn_close_highlight.png"), _T("SysBtn\\btn_close_down.png"));

	HDC hDlgBgDC = m_SkinDlg.GetBgDC();

	CRect rcClient;
	GetClientRect(&rcClient);

	m_picHead.SubclassWindow(GetDlgItem(ID_PIC_HEAD));
	m_picHead.MoveWindow(8, 6, 32, 32, FALSE);
	m_picHead.SetTransparent(TRUE, hDlgBgDC);
	m_picHead.SetBgPic(_T("aio_head_normal.png"), _T("aio_head_normal.png"), _T("aio_head_normal.png"));

	tstring strFileName;
	if (m_lpQQClient != NULL && m_nQQNumber != 0)
		strFileName = m_lpQQClient->GetBuddyHeadPicFullName(m_nQQNumber);
	if (!ZYM::CPath::IsFileExist(strFileName.c_str()))
		strFileName = ZYM::CPath::GetAppPath() + _T("Image\\DefBuddyHeadPic.png");
	m_picHead.SetBitmap(strFileName.c_str());

	m_lnkBuddyName.SubclassWindow(GetDlgItem(ID_LINK_BUDDYNAME));
	m_lnkBuddyName.MoveWindow(44, 8, 60, 14, FALSE);
	m_lnkBuddyName.SetTransparent(TRUE, hDlgBgDC);
	m_lnkBuddyName.SetLinkColor(RGB(0,0,0));
	m_lnkBuddyName.SetHoverLinkColor(RGB(0,0,0));
	m_lnkBuddyName.SetVisitedLinkColor(RGB(0,0,0));

	CFontHandle font = (HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0, 0L);
	LOGFONT lf = {0};
	font.GetLogFont(&lf);
	lf.lfHeight = 13;
	lf.lfWeight = FW_BOLD;
	m_fontBuddyNameLink = ::CreateFontIndirect(&lf); 
	m_lnkBuddyName.SetNormalFont(m_fontBuddyNameLink);

	m_staBuddySign.SubclassWindow(GetDlgItem(ID_STATIC_BUDDYSIGN));
	m_staBuddySign.MoveWindow(44, 22, rcClient.Width()-50, 14, FALSE);
	m_staBuddySign.SetTransparent(TRUE, hDlgBgDC);

	m_picAD_1.SubclassWindow(GetDlgItem(ID_STATIC_AD_1));
	m_picAD_1.MoveWindow(rcClient.right-144, rcClient.top+43, 142, 42, FALSE);
	strFileName = ZYM::CPath::GetAppPath()+_T("Image\\AD_1.png");
	m_picAD_1.SetBitmap(strFileName.c_str());
	m_picAD_1.SetDrawMode(DRAW_MODE_STRETCH);

	m_picAD_2.SubclassWindow(GetDlgItem(ID_STATIC_AD_2));
	m_picAD_2.MoveWindow(rcClient.right-143, rcClient.top+85, 140, 224, FALSE);
	strFileName = ZYM::CPath::GetAppPath()+_T("Image\\AD_2.png");
	m_picAD_2.SetBitmap(strFileName.c_str());
	m_picAD_2.SetDrawMode(DRAW_MODE_STRETCH);

	m_picAD_3.SubclassWindow(GetDlgItem(ID_STATIC_AD_3));
	m_picAD_3.MoveWindow(rcClient.right-143, rcClient.bottom-145, 140, 141, FALSE);
	strFileName = ZYM::CPath::GetAppPath()+_T("Image\\AD_3.png");
	m_picAD_3.SetBitmap(strFileName.c_str());
	m_picAD_3.SetDrawMode(DRAW_MODE_STRETCH);

	m_btnClose.SubclassWindow(GetDlgItem(ID_BTN_CLOSE));
	m_btnClose.MoveWindow(rcClient.right-310, rcClient.bottom-5-22, 69, 22, FALSE);
	m_btnClose.SetTransparent(TRUE, hDlgBgDC);
	m_btnClose.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnClose.SetBgPic(_T("Button\\btn_normal.png"), _T("Button\\btn_highlight.png"),
		_T("Button\\btn_down.png"), _T("Button\\btn_focus.png"));

	m_btnSend.SubclassWindow(GetDlgItem(ID_BTN_SEND));
	m_btnSend.MoveWindow(rcClient.right-236, rcClient.bottom-5-22, 64, 22, FALSE);
	m_btnSend.SetTransparent(TRUE, hDlgBgDC);
	m_btnSend.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnSend.SetBgPic(_T("Button\\btnleft_normal.png"), _T("Button\\btnleft_highlight.png"),
		_T("Button\\btnleft_down.png"), _T("Button\\btnleft_focus.png"));

	m_btnArrow.SubclassWindow(GetDlgItem(ID_BTN_ARROW));
	m_btnArrow.MoveWindow(rcClient.right-172, rcClient.bottom-5-22, 19, 22, FALSE);
	m_btnArrow.SetTransparent(TRUE, hDlgBgDC);
	m_btnArrow.SetButtonType(SKIN_PUSH_BUTTON);
	m_btnArrow.SetBgPic(_T("Button\\btnright_normal.png"), _T("Button\\btnright_highlight.png"),
		_T("Button\\btnright_down.png"), _T("Button\\btnright_fouce.png"));

	m_SkinMenu.LoadMenu(ID_MENU_BUDDYCHAT);
	m_SkinMenu.SetBgPic(_T("Menu\\menu_left_bg.png"), _T("Menu\\menu_right_bg.png"));
	m_SkinMenu.SetSelectedPic(_T("Menu\\menu_selected.png"));
	m_SkinMenu.SetSepartorPic(_T("Menu\\menu_separtor.png"));
	m_SkinMenu.SetArrowPic(_T("Menu\\menu_arrow.png"));
	m_SkinMenu.SetCheckPic(_T("Menu\\menu_check.png"));
	m_SkinMenu.SetTextColor(RGB(0, 20, 35));
	m_SkinMenu.SetSelTextColor(RGB(254, 254, 254));

	InitTopToolBar();				// ��ʼ��Top������
	InitMidToolBar();				// ��ʼ��Middle������
	m_PicBarDlg.Create(m_hWnd);		// ����ͼƬ����������

	CRect rcRecv = CRect(CPoint(6, rcClient.top+44+44), CSize(rcClient.Width()-6-152, rcClient.Height()-243));
	DWORD dwStyle = WS_CHILD|WS_VISIBLE|WS_TABSTOP|ES_MULTILINE|ES_AUTOVSCROLL|WS_VSCROLL|ES_WANTRETURN;
	m_richRecv.Create(m_hWnd, rcRecv, NULL, dwStyle, WS_EX_TRANSPARENT, ID_RICHEDIT_RECV);
	m_richRecv.SetTransparent(TRUE, hDlgBgDC);

	CRect rcSend = CRect(CPoint(6, rcClient.bottom-128), CSize(rcClient.Width()-6-152, 95));
	m_richSend.Create(m_hWnd, rcSend, NULL, dwStyle, WS_EX_TRANSPARENT, ID_RICHEDIT_SEND);
	m_richSend.SetTransparent(TRUE, hDlgBgDC);
	//m_richSend.SetBackgroundColor(RGB(252, 253, 254));

	// ������Ϣ���ı���ؼ�
	CFontInfo fontInfo = m_FontSelDlg.GetPublicFontInfo();
	RichEdit_SetDefFont(m_richSend.m_hWnd, fontInfo.m_strName.c_str(),
		fontInfo.m_nSize, fontInfo.m_clrText, fontInfo.m_bBold,
		fontInfo.m_bItalic, fontInfo.m_bUnderLine, FALSE);

	// ������Ϣ���ı���ؼ�
	DWORD dwMask = m_richRecv.GetEventMask();
	dwMask = dwMask | ENM_LINK  | ENM_MOUSEEVENTS | ENM_SCROLLEVENTS | ENM_KEYEVENTS;
	m_richRecv.SetEventMask(dwMask);

	m_richRecv.SetAutoURLDetect();
	m_richRecv.SetReadOnly();

	UpdateDlgTitle();
	UpdateBuddyNameCtrl();
	UpdateBuddySignCtrl();

	m_Accelerator.LoadAccelerators(ID_ACCE_CHATDLG);

	InitRichEditOleCallback();	// ��ʼ��IRichEditOleCallback�ӿ�

	return TRUE;
}

// ����ʼ���ؼ�
BOOL CBuddyChatDlg::UnInit()
{
	if (m_PicBarDlg.IsWindow())
		m_PicBarDlg.DestroyWindow();

	if (m_picHead.IsWindow())
		m_picHead.DestroyWindow();

	if (m_lnkBuddyName.IsWindow())
		m_lnkBuddyName.DestroyWindow();

	if (m_staBuddySign.IsWindow())
		m_staBuddySign.DestroyWindow();

	if (m_picAD_1.IsWindow())
		m_picAD_1.DestroyWindow();

	if (m_picAD_2.IsWindow())
		m_picAD_2.DestroyWindow();

	if (m_picAD_3.IsWindow())
		m_picAD_3.DestroyWindow();

	if (m_btnClose.IsWindow())
		m_btnClose.DestroyWindow();

	if (m_btnSend.IsWindow())
		m_btnSend.DestroyWindow();

	if (m_btnArrow.IsWindow())
		m_btnArrow.DestroyWindow();

	m_SkinMenu.DestroyMenu();

	if (m_tbTop.IsWindow())
		m_tbTop.DestroyWindow();

	if (m_tbMid.IsWindow())
		m_tbMid.DestroyWindow();

	if (m_FontSelDlg.IsWindow())
		m_FontSelDlg.DestroyWindow();

	if (m_FaceSelDlg.IsWindow())
		m_FaceSelDlg.DestroyWindow();

	if (m_richRecv.IsWindow())
		m_richRecv.DestroyWindow();

	if (m_richSend.IsWindow())
		m_richSend.DestroyWindow();

	m_Accelerator.DestroyObject();
	//	m_menuRichEdit.DestroyMenu();

	return TRUE;
}

void CBuddyChatDlg::_RichEdit_ReplaceSel(HWND hWnd, LPCTSTR lpszNewText)
{
	if (hWnd == m_richRecv.m_hWnd)
	{
		CFontInfo fontInfo = m_FontSelDlg.GetFontInfo();
		RichEdit_ReplaceSel(hWnd, lpszNewText, 
			fontInfo.m_strName.c_str(), fontInfo.m_nSize, 
			fontInfo.m_clrText, fontInfo.m_bBold, fontInfo.m_bItalic, 
			fontInfo.m_bUnderLine, FALSE, 300);
	}
	else
	{
		RichEdit_ReplaceSel(hWnd, lpszNewText);
	}
}

BOOL CBuddyChatDlg::_RichEdit_InsertFace(HWND hWnd, LPCTSTR lpszFileName, int nFaceId, int nFaceIndex)
{
	ITextServices * pTextServices;
	ITextHost * pTextHost;
	BOOL bRet;

	if (hWnd == m_richRecv.m_hWnd)
	{
		pTextServices = m_richRecv.GetTextServices();
		pTextHost = m_richRecv.GetTextHost();

		long lStartChar = 0, lEndChar = 0;
		RichEdit_GetSel(hWnd, lStartChar, lEndChar);
		bRet = RichEdit_InsertFace(pTextServices, pTextHost, 
			lpszFileName, nFaceId, nFaceIndex, RGB(255,255,255), TRUE, 40);
		if (bRet)
		{
			lEndChar = lStartChar + 1;
			RichEdit_SetSel(hWnd, lStartChar, lEndChar);
			RichEdit_SetStartIndent(hWnd, 300);
			RichEdit_SetSel(hWnd, lEndChar, lEndChar);
		}
	}
	else
	{
		pTextServices = m_richSend.GetTextServices();
		pTextHost = m_richSend.GetTextHost();

		bRet = RichEdit_InsertFace(pTextServices, pTextHost, 
			lpszFileName, nFaceId, nFaceIndex, RGB(255,255,255), TRUE, 40);
	}

	if (pTextServices != NULL)
		pTextServices->Release();
	if (pTextHost != NULL)
		pTextHost->Release();

	return bRet;
}

BOOL CBuddyChatDlg::HandleSysFaceId(HWND hRichEditWnd, LPCTSTR& p, CString& strText)
{
	int nFaceId = GetBetweenInt(p+2, _T("[\""), _T("\"]"), -1);
	CFaceInfo * lpFaceInfo = m_lpFaceList->GetFaceInfoById(nFaceId);
	if (lpFaceInfo != NULL)
	{
		if (!strText.IsEmpty())
		{
			_RichEdit_ReplaceSel(hRichEditWnd, strText); 
			strText = _T("");
		}

		_RichEdit_InsertFace(hRichEditWnd, lpFaceInfo->m_strFileName.c_str(), 
			lpFaceInfo->m_nId, lpFaceInfo->m_nIndex);

		p = _tcsstr(p+2, _T("\"]"));
		p++;
		return TRUE;
	}
	return FALSE;
}

BOOL CBuddyChatDlg::HandleSysFaceIndex(HWND hRichEditWnd, LPCTSTR& p, CString& strText)
{
	int nFaceIndex = GetBetweenInt(p+2, _T("[\""), _T("\"]"), -1);
	CFaceInfo * lpFaceInfo = m_lpFaceList->GetFaceInfoByIndex(nFaceIndex);
	if (lpFaceInfo != NULL)
	{
		if (!strText.IsEmpty())
		{
			_RichEdit_ReplaceSel(hRichEditWnd, strText); 
			strText = _T("");
		}

		_RichEdit_InsertFace(hRichEditWnd, lpFaceInfo->m_strFileName.c_str(), 
			lpFaceInfo->m_nId, lpFaceInfo->m_nIndex);

		p = _tcsstr(p+2, _T("\"]"));
		p++;
		return TRUE;
	}
	return FALSE;
}

BOOL CBuddyChatDlg::HandleCustomPic(HWND hRichEditWnd, LPCTSTR& p, CString& strText)
{
	CString strFileName = GetBetweenString(p+2, _T("[\""), _T("\"]")).c_str();
	if (!strFileName.IsEmpty())
	{
		if (!strText.IsEmpty())
		{
			_RichEdit_ReplaceSel(hRichEditWnd, strText); 
			strText = _T("");
		}

		if (::PathIsRelative(strFileName))
			strFileName = m_lpQQClient->GetChatPicFullName(strFileName).c_str();

		_RichEdit_InsertFace(hRichEditWnd, strFileName, -1, -1);

		p = _tcsstr(p+2, _T("\"]"));
		p++;
		return TRUE;
	}
	return FALSE;
}

// "/f["ϵͳ����id"]/s["ϵͳ����index"]/c["�Զ���ͼƬ·��"]"
void CBuddyChatDlg::AddMsg(HWND hRichEditWnd, LPCTSTR lpText)
{
	if (NULL == lpText || NULL == *lpText)
		return;

	CString strText;

	for (LPCTSTR p = lpText; *p != _T('\0'); p++)
	{
		if (*p == _T('/'))
		{
			if (*(p+1) == _T('/'))
			{
				strText += *p;
				p++;
				continue;
			}
			else if (*(p+1) == _T('f'))
			{
				if (HandleSysFaceId(hRichEditWnd, p, strText))
					continue;
			}
			else if (*(p+1) == _T('s'))
			{
				if (HandleSysFaceIndex(hRichEditWnd, p, strText))
					continue;
			}
			else if (*(p+1) == _T('c'))
			{
				if (HandleCustomPic(hRichEditWnd, p, strText))
					continue;
			}
		}
		strText += *p;
	}

	if (!strText.IsEmpty())
		_RichEdit_ReplaceSel(hRichEditWnd, strText);
}

void CBuddyChatDlg::AddMsgToSendEdit(LPCTSTR lpText)
{
	AddMsg(m_richSend.m_hWnd, lpText);
	m_richSend.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

void CBuddyChatDlg::AddMsgToRecvEdit(time_t nTime, LPCTSTR lpText)
{
	if (NULL == lpText || NULL == *lpText)
		return;

	TCHAR cTime[32] = {0};
	FormatTime(nTime, _T("%H:%M:%S"), cTime, sizeof(cTime)/sizeof(TCHAR));

	CString strText;
	strText.Format(_T("%s  %s\r\n"), m_strUserName, cTime);

	RichEdit_SetSel(m_richRecv.m_hWnd, -1, -1);
	RichEdit_ReplaceSel(m_richRecv.m_hWnd, strText, 
		_T("����"), 10, RGB(0,128,64), FALSE, FALSE, FALSE, FALSE, 0);
	strText = _T("");
 
	AddMsg(m_richRecv.m_hWnd, lpText);

	RichEdit_ReplaceSel(m_richRecv.m_hWnd, _T("\r\n"));
	RichEdit_SetStartIndent(m_richRecv.m_hWnd, 0);
	m_richRecv.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

void CBuddyChatDlg::AddMsgToRecvEdit(CBuddyMessage * lpBuddyMsg)
{
	if (NULL == lpBuddyMsg || NULL == m_lpQQClient)
		return;
	
	TCHAR cTime[32] = {0};
	if (IsToday(lpBuddyMsg->m_nTime))
		FormatTime(lpBuddyMsg->m_nTime, _T("%H:%M:%S"), cTime, sizeof(cTime)/sizeof(TCHAR));
	else
		FormatTime(lpBuddyMsg->m_nTime, _T("%Y-%m-%d %H:%M:%S"), cTime, sizeof(cTime)/sizeof(TCHAR));

	CString strText;
	strText.Format(_T("%s  %s\r\n"), m_strBuddyName, cTime);

	RichEdit_SetSel(m_richRecv.m_hWnd, -1, -1);
	RichEdit_ReplaceSel(m_richRecv.m_hWnd, strText, 
		_T("����"), 10, RGB(0, 0, 255), FALSE, FALSE, FALSE, FALSE, 0);

	CFontInfo fontInfo;
	for (int i = 0; i < (int)lpBuddyMsg->m_arrContent.size(); i++)
	{
		CContent * lpContent = lpBuddyMsg->m_arrContent[i];
		if (lpContent != NULL)
		{
			switch (lpContent->m_nType)
			{
			case CONTENT_TYPE_FONT_INFO:
				{
					fontInfo.m_strName = lpContent->m_FontInfo.m_strName;
					fontInfo.m_nSize = lpContent->m_FontInfo.m_nSize;
					fontInfo.m_clrText = lpContent->m_FontInfo.m_clrText;
					fontInfo.m_bBold = lpContent->m_FontInfo.m_bBold;
					fontInfo.m_bItalic = lpContent->m_FontInfo.m_bItalic;
					fontInfo.m_bUnderLine = lpContent->m_FontInfo.m_bUnderLine;
				}
				break;

			case CONTENT_TYPE_TEXT:
				{
					RichEdit_ReplaceSel(m_richRecv.m_hWnd, lpContent->m_strText.c_str(), 
						fontInfo.m_strName.c_str(), fontInfo.m_nSize, 
						fontInfo.m_clrText, fontInfo.m_bBold, fontInfo.m_bItalic, 
						fontInfo.m_bUnderLine, FALSE, 300);
				}
				break;

			case CONTENT_TYPE_FACE:
				{
					if (m_lpFaceList != NULL)
					{
						CFaceInfo * lpFaceInfo = m_lpFaceList->GetFaceInfoById(lpContent->m_nFaceId);
						if (lpFaceInfo != NULL)
						{
							_RichEdit_InsertFace(m_richRecv.m_hWnd, 
								lpFaceInfo->m_strFileName.c_str(), lpFaceInfo->m_nId, lpFaceInfo->m_nIndex);
						}
					}
				}
				break;

			case CONTENT_TYPE_CUSTOM_FACE:
			case CONTENT_TYPE_OFF_PIC:
				{
					tstring strFileName = m_lpQQClient->GetChatPicFullName(lpContent->m_CFaceInfo.m_strName.c_str());
					if (!ZYM::CPath::IsFileExist(strFileName.c_str()))
						strFileName = ZYM::CPath::GetAppPath() + _T("Image\\DownloadFailed.gif");
					_RichEdit_InsertFace(m_richRecv.m_hWnd, strFileName.c_str(), -1, -1);
				}
				break;
			}
		}
	}

	RichEdit_ReplaceSel(m_richRecv.m_hWnd, _T("\r\n"));
	RichEdit_SetStartIndent(m_richRecv.m_hWnd, 0);
	m_richRecv.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

// ����Ϣ��¼�������
void CBuddyChatDlg::OpenMsgLogBrowser()
{
	CString strExeName = ZYM::CPath::GetAppPath().c_str();
	strExeName += _T("MsgLogBrowser.exe");

	if (!ZYM::CPath::IsFileExist(strExeName))
		return;

	CString strMsgFile = m_lpQQClient->GetMsgLogFullName().c_str();
	strMsgFile.Replace(_T("\\"), _T("/"));

	CString strChatPicDir = m_lpQQClient->GetChatPicFolder().c_str();
	strChatPicDir.Replace(_T("\\"), _T("/"));

	CString strCmdLine;
	strCmdLine.Format(_T("\"%s\" %u %u %u %u %u \"%s\" \"%s\""), 
		strExeName, m_hWnd, m_nQQNumber, 0, 0, m_nUserNumber,  strMsgFile, strChatPicDir);
	LPTSTR lpszCmdLine = _tcsdup(strCmdLine.GetBuffer());
	if (NULL == lpszCmdLine)
		return;

	STARTUPINFO si = {0};
	PROCESS_INFORMATION pi = {0};

	si.cb = sizeof(si);

	BOOL bRet = ::CreateProcess(NULL, lpszCmdLine, NULL,
		NULL, FALSE, 0, NULL, NULL, &si, &pi);
	free(lpszCmdLine);
	if (!bRet)
		return;

	m_dwThreadId = pi.dwThreadId;

	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
}

// �ر���Ϣ��¼�������
void CBuddyChatDlg::CloseMsgLogBrowser()
{
	if (m_dwThreadId != NULL)
	{
		::PostThreadMessage(m_dwThreadId, WM_CLOSE_MSGLOGDLG, 0, 0);
		m_dwThreadId = NULL;
	}
}
#include "StdAfx.h"
#include "SkinStatic.h"

CSkinStatic::CSkinStatic(void)
{
	m_lpBgImgN = NULL;
	m_bTransparent = FALSE;
	m_hBgDC = NULL;
}

CSkinStatic::~CSkinStatic(void)
{
}

BOOL CSkinStatic::SetNormalBgPic(LPCTSTR lpszFileName)
{
	CSkinManager::GetInstance()->ReleaseImage(m_lpBgImgN);
	m_lpBgImgN = CSkinManager::GetInstance()->GetImage(lpszFileName);
	return (m_lpBgImgN != NULL) ? TRUE : FALSE;
}

void CSkinStatic::SetTransparent(BOOL bTransparent, HDC hBgDC)
{
	m_bTransparent = bTransparent;
	m_hBgDC = hBgDC;
}

BOOL CSkinStatic::SubclassWindow(HWND hWnd)
{
	return __super::SubclassWindow(hWnd);
}

int CSkinStatic::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	return 0;
}

BOOL CSkinStatic::OnEraseBkgnd(CDCHandle dc)
{
	return TRUE;
}

void CSkinStatic::OnPaint(CDCHandle dc)
{
	CPaintDC PaintDC(m_hWnd);

	CRect rcClient;
	GetClientRect(&rcClient);

	CMemoryDC MemDC(PaintDC.m_hDC, rcClient);
	Draw(MemDC.m_hDC);
}

int CSkinStatic::OnSetText(LPCTSTR lpstrText)
{
	int nRet = DefWindowProc();

	HDC hDC = ::GetDC(m_hWnd);
	Draw(hDC);
	::ReleaseDC(m_hWnd, hDC);

	return nRet;
}

void CSkinStatic::OnDestroy()
{
	CSkinManager::GetInstance()->ReleaseImage(m_lpBgImgN);
}

void CSkinStatic::DrawParentWndBg(HDC hDC)
{
	HWND hParentWnd = ::GetParent(m_hWnd);

	CRect rcWindow;
	GetWindowRect(&rcWindow);
	::ScreenToClient(hParentWnd, (LPPOINT)&rcWindow); 
	::ScreenToClient(hParentWnd, ((LPPOINT)&rcWindow)+1);

	::BitBlt(hDC, 0, 0, rcWindow.Width(), rcWindow.Height(), m_hBgDC, rcWindow.left, rcWindow.top, SRCCOPY);
}

void CSkinStatic::Draw(HDC hDC)
{
	CDCHandle dc(hDC);

	CRect rcClient;
	GetClientRect(&rcClient);

	if (m_bTransparent)
		DrawParentWndBg(dc);

	if (m_lpBgImgN != NULL && !m_lpBgImgN->IsNull())
		m_lpBgImgN->Draw2(dc, rcClient);

	CString strText;
	GetWindowText(strText);

	if (strText.GetLength() > 0)
	{
		dc.SetBkMode(TRANSPARENT);
		HFONT hFontOld = dc.SelectFont((HFONT)::SendMessage(m_hWnd, WM_GETFONT, 0, 0L));

		DWORD dwStyle = GetStyle();
		UINT uFormat = DT_LEFT;
		if (dwStyle & SS_CENTER)
			uFormat = DT_CENTER;
		else if (dwStyle & SS_RIGHT)
			uFormat = DT_RIGHT;
		uFormat |= (dwStyle & SS_LEFTNOWORDWRAP) ? (DT_SINGLELINE|DT_VCENTER) : DT_WORDBREAK;

		dc.DrawText(strText, -1, &rcClient, uFormat);

		dc.SelectFont(hFontOld);
	}
}
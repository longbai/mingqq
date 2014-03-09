#pragma once

#include "SkinManager.h"

#define TCN_DROPDOWN				(NM_FIRST-5)

#define STCI_STYLE_BUTTON			0x0000
#define STCI_STYLE_DROPDOWN			0x0008

class CSkinTabCtrlItem
{
public:
	CSkinTabCtrlItem(void);
	~CSkinTabCtrlItem(void);

public:
	int m_nID;
	DWORD m_dwStyle;
	int m_nWidth, m_nHeight;
	int m_nLeftWidth, m_nRightWidth;
	int m_nPadding;
	CString m_strText;
	CString m_strToolTipText;
	CImageEx * m_lpBgImgN, * m_lpBgImgH, * m_lpBgImgD;
	CImageEx * m_lpArrowImgH, * m_lpArrowImgD;
	CImageEx * m_lpIconImg, * m_lpSelIconImg;
};

class CSkinTabCtrl : public CWindowImpl<CSkinTabCtrl, CWindow>
{
public:
	CSkinTabCtrl(void);
	~CSkinTabCtrl(void);

	DECLARE_WND_CLASS(_T("__SkinTabCtrl__"))

	BEGIN_MSG_MAP_EX(CSkinTabCtrl)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()

public:
	void SetLeftTop(int nLeft, int nTop);
	void SetAutoSize(BOOL bAutoSize);
	void SetTransparent(BOOL bTransparent, HDC hBgDC);
	BOOL SetBgPic(LPCTSTR lpszFileName, const CRect& rcNinePart);
	BOOL SetItemsBgPic(LPCTSTR lpNormal, LPCTSTR lpHighlight, 
		LPCTSTR lpDown, const CRect& rcNinePart);
	BOOL SetItemsArrowPic(LPCTSTR lpHighlight, LPCTSTR lpDown);

	int GetItemCount();
	int GetCurSel();
	void SetCurSel(int nCurSel);

	int AddItem(int nID, DWORD dwStyle);
	void SetItemID(int nIndex, int nID);
	void SetItemStyle(int nIndex, int dwStyle);
	void SetItemSize(int nIndex, int nWidth, int nHeight, 
		int nLeftWidth = 0, int nRightWidth = 0);
	void SetItemPadding(int nIndex, int nPadding);
	void SetItemText(int nIndex, LPCTSTR lpszText);
	void SetItemToolTipText(int nIndex, LPCTSTR lpszText);
	BOOL SetItemBgPic(int nIndex, LPCTSTR lpNormal, 
		LPCTSTR lpHighlight, LPCTSTR lpDown, const CRect& rcNinePart);
	BOOL SetItemArrowPic(int nIndex, LPCTSTR lpHighlight, LPCTSTR lpDown);
	BOOL SetItemIconPic(int nIndex, LPCTSTR lpIcon, LPCTSTR lpSelIcon);

	BOOL GetItemRectByIndex(int nIndex, CRect& rect);
	BOOL GetItemRectByID(int nID, CRect& rect);

private:
	BOOL OnEraseBkgnd(CDCHandle dc);
	void OnPaint(CDCHandle dc);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseLeave();
	LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnDestroy();

	BOOL StartTrackMouseLeave();
	void CalcCenterRect(CRect& rcDest, int cx, int cy, CRect& rcCenter);
	CSkinTabCtrlItem * GetItemByIndex(int nIndex);
	CSkinTabCtrlItem * GetItemByID(int nID);
	int HitTest(POINT pt);

	void DrawParentWndBg(HDC hDC);
	void DrawItem(HDC hDC, int nIndex);

private:
	CToolTipCtrl m_ToolTipCtrl;
	std::vector<CSkinTabCtrlItem *> m_arrItems;
	CImageEx * m_lpBgImg;
	CImageEx * m_lpItemBgImgN, * m_lpItemBgImgH, * m_lpItemBgImgD;
	CImageEx * m_lpArrowImgH, * m_lpArrowImgD;
	int m_nSelIndex, m_nHoverIndex;
	BOOL m_bPressArrow;
	BOOL m_bMouseTracking;
	int m_nLeft, m_nTop;
	BOOL m_bAutoSize;
	BOOL m_bTransparent;
	HDC m_hBgDC;
};

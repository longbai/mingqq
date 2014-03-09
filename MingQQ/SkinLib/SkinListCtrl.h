#pragma once

#include <vector>
#include "SkinManager.h"
#include "SkinScrollBar.h"

class CLVColumn
{
public:
	CLVColumn(void);
	~CLVColumn(void);

public:
	CString m_strText;
	CImageEx * m_lpImage;
	DWORD m_dwFmt;
	int m_nWidth;
};

class CLVItem
{
public:
	CLVItem(void);
	~CLVItem(void);

public:
	CString m_strText;
	Gdiplus::Bitmap * m_lpImage;
	DWORD m_dwFmt;
	LPARAM m_lParam;
};

class CLVRow
{
public:
	CLVRow(int nCols);
	~CLVRow(void);

public:
	BOOL AddItem();
	BOOL InsertItem(int nCol);
	BOOL DeleteItem(int nCol);

	BOOL SetItem(int nCol, LPCTSTR lpszText, LPCTSTR lpszImage, 
		int cx, int cy, BOOL bGray, DWORD dwFmt, LPARAM lParam);

	BOOL SetItemText(int nCol,  LPCTSTR lpszText);
	BOOL SetItemImage(int nCol,  LPCTSTR lpszImage, int cx, int cy, BOOL bGray = FALSE);
	BOOL SetItemFormat(int nCol,  DWORD dwFmt);
	BOOL SetItemData(int nCol,  LPARAM lParam);

	CString GetItemText(int nCol);
	Gdiplus::Bitmap * GetItemImage(int nCol);
	DWORD GetItemFormat(int nCol);
	LPARAM GetItemData(int nCol);

	int GetItemCount();
	BOOL DeleteAllItems();

private:
	CLVItem * GetItem(int nCol);

private:
	std::vector<CLVItem *> m_arrItems;
};

class CSkinListCtrl : public CWindowImpl<CSkinListCtrl, CWindow>
{
public:
	CSkinListCtrl(void);
	~CSkinListCtrl(void);

	BEGIN_MSG_MAP_EX(CSkinListCtrl)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_PAINT(OnPaint)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		MSG_WM_LBUTTONDOWN(OnLButtonDown)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_MOUSELEAVE(OnMouseLeave)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_HSCROLL(OnHScroll)
		MSG_WM_VSCROLL(OnVScroll)
		MSG_WM_MOUSEWHEEL(OnMouseWheel)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_SIZE(OnSize)
		MSG_WM_GETDLGCODE(OnGetDlgCode)
		//MESSAGE_RANGE_HANDLER_EX(WM_MOUSEFIRST, WM_MOUSELAST, OnMouseMessage)
		MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()

public:
	BOOL SetBgPic(LPCTSTR lpszFileName);				// �����б���ͼƬ
	BOOL SetHeadSeparatedPic(LPCTSTR lpszFileName);		// ���ñ�ͷ�ָ���ͼƬ

	BOOL SetHeadNormalPic(LPCTSTR lpszFileName);		// ���ñ�ͷ����ͼƬ(��ͨ״̬)
	BOOL SetHeadHotPic(LPCTSTR lpszFileName);			// ���ñ�ͷ����ͼƬ(����״̬)
	BOOL SetHeadPushedPic(LPCTSTR lpszFileName);		// ���ñ�ͷ����ͼƬ(����״̬)

	BOOL SetOddItemBgPic(LPCTSTR lpszFileName);			// �������б���ͼƬ(��ͨ״̬)
	BOOL SetEvenItemBgPic(LPCTSTR lpszFileName);		// ����˫�б���ͼƬ(��ͨ״̬)
	BOOL SetItemHotBgPic(LPCTSTR lpszFileName);			// �����б���ͼƬ(����״̬)
	BOOL SetItemSelBgPic(LPCTSTR lpszFileName);			// �����б���ͼƬ(ѡ��״̬)

	void SetTransparent(BOOL bTransparent, HDC hBgDC);	// �����б����Ƿ�͸��
	void SetHeadVisible(BOOL bVisible);					// ���ñ�ͷ�Ƿ�ɼ�
	BOOL IsHeadVisible();								// �жϱ�ͷ�Ƿ�ɼ�
	void SetHeadHeight(int nHeight);					// ���ñ�ͷ�߶�
	void SetItemHeight(int nHeight);					// �����и߶�
	void SetItemImageSize(int cx, int cy);				// ������ͼ���С

	int AddColumn(LPCTSTR lpszText,						// �����
		LPCTSTR lpszImage, DWORD dwFmt, int nWidth);
	int InsertColumn(int nCol, LPCTSTR lpszText,		// ������
		LPCTSTR lpszImage, DWORD dwFmt, int nWidth);
	BOOL DeleteColumn(int nCol);						// ɾ����

	int AddItem(LPCTSTR lpszText,						// �����
		LPCTSTR lpszImage, BOOL bGray, DWORD dwFmt, LPARAM lParam);
	int InsertItem(int nItem, LPCTSTR lpszText,			// ������
		LPCTSTR lpszImage, BOOL bGray, DWORD dwFmt, LPARAM lParam);
	BOOL DeleteItem(int nItem);							// ɾ����

	int GetColumnCount();								// ��ȡ������
	BOOL DeleteAllColumns();							// ɾ��������
	int GetItemCount();									// ��ȡ������
	BOOL DeleteAllItems();								// ɾ��������

	BOOL SetColumnText(int nCol, LPCTSTR lpszText);		// �������ı�
	BOOL SetColumnImage(int nCol, LPCTSTR lpszImage);	// ������ͼ��
	BOOL SetColumnFormat(int nCol, DWORD dwFmt);		// �����и�ʽ
	BOOL SetColumnWidth(int nCol, int nWidth);			// �����п��

	CString GetColumnText(int nCol);					// ��ȡ���ı�
	DWORD GetColumnFormat(int nCol);					// ��ȡ�и�ʽ
	int GetColumnWidth(int nCol);						// ��ȡ�п��

	BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText);	// �������ı�
	BOOL SetItemImage(int nItem, int nSubItem,						// ������ͼ��
		LPCTSTR lpszImage, BOOL bGray = FALSE);
	BOOL SetItemFormat(int nItem, int nSubItem, DWORD dwFmt);		// �����и�ʽ
	BOOL SetItemData(int nItem, int nSubItem, LPARAM lParam);		// ����������

	CString GetItemText(int nItem, int nSubItem);		// ��ȡ���ı�
	DWORD GetItemFormat(int nItem, int nSubItem);		// ��ȡ�и�ʽ
	LPARAM GetItemData(int nItem, int nSubItem);		// ��ȡ������

	int GetCurSelItemIndex();							// ��ȡ��ǰѡ��������
	void SetItemTextColor(COLORREF clr);				// �������ı���ɫ(��ͨ״̬)
	void SetSelItemTextColor(COLORREF clr);				// ���ı���ɫ(ѡ��״̬)

private:
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	BOOL OnEraseBkgnd(CDCHandle dc);
	void OnPaint(CDCHandle dc);
	void OnLButtonDblClk(UINT nFlags, CPoint point);
	void OnLButtonDown(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	void OnRButtonDown(UINT nFlags, CPoint point);
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnMouseLeave();
	void OnTimer(UINT_PTR nIDEvent);
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar pScrollBar);
	void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar pScrollBar);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnSize(UINT nType, CSize size);
	UINT OnGetDlgCode(LPMSG lpMsg);
	//LRESULT OnMouseMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnDestroy();

	CLVColumn * GetColumn(int nCol);
	CLVRow * GetItem(int nItem);

	void DrawParentWndBg(HDC hDC);
	void DrawHeadPart(HDC hDC);
	void DrawItemPart(HDC hDC);

	BOOL GetHeadPartRect(CRect& rect);
	BOOL GetItemPartRect(CRect& rect);
	BOOL GetItemRect(int nIndex, CRect& rect);
	int HeadHitTest(POINT pt);
	int ItemHitTest(POINT pt);
	BOOL StartTrackMouseLeave();
	void AdjustScrollBarSize();
	void CheckScrollBarStatus();
	void Scroll(int cx, int cy);
	void EnsureVisible(int nIndex);
	int GetColumnsWidth();

	BOOL Init();
	void UnInit();

private:
	//CToolTipCtrl m_ToolTipCtrl;
	CSkinScrollBar m_HScrollBar;
	CSkinScrollBar m_VScrollBar;

	std::vector<CLVColumn *> m_arrCols;
	std::vector<CLVRow *> m_arrRows;

	CImageEx * m_lpBgImg;					// �б���ͼƬ
	CImageEx * m_lpHeadSeparatedImg;		// ��ͷ�ָ���ͼƬ
	CImageEx * m_lpHeadImgN;				// ��ͷ����ͼƬ(��ͨ״̬)
	CImageEx * m_lpHeadImgH;				// ��ͷ����ͼƬ(����״̬)
	CImageEx * m_lpHeadImgP;				// ��ͷ����ͼƬ(����״̬)
	CImageEx * m_lpOddItemBgImgN;			// ���б���ͼƬ(��ͨ״̬)
	CImageEx * m_lpEvenItemBgImgN;			// ˫�б���ͼƬ(��ͨ״̬)
	CImageEx * m_lpItemBgImgH;				// �б���ͼƬ(����״̬)
	CImageEx * m_lpItemBgImgS;				// �б���ͼƬ(ѡ��״̬)

	BOOL m_bTransparent;					// �б����Ƿ�͸����־
	HDC m_hBgDC;							// �����ڱ���DC
	BOOL m_bMouseTracking;					// �����ٱ�־
	BOOL m_bHeadVisible;					// ��ͷ�Ƿ�ɼ���־
	int m_nHeadHeight;						// ��ͷ�߶�
	int m_nItemHeight;						// �и߶�
	CSize m_szItemImage;					// ��ͼ���С

	int m_nPressHeadIndex;					// ��갴�±�ͷ������
	int m_nHoverHeadIndex;					// �����ͣ��ͷ������

	int m_nPressItemIndex;					// ��갴����������
	int m_nHoverItemIndex;					// �����ͣ��������
	int m_nSelItemIndex;					// ���ѡ����������

	int m_nLeft, m_nTop;					// ��ͼԭ������

	COLORREF m_clrItemText;					// ���ı���ɫ(��ͨ״̬)
	COLORREF m_clrSelItemText;				// ���ı���ɫ(ѡ��״̬)
};

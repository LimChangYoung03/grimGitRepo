#pragma once
#include "CAssignment.h"
#include "CircleInfo.h"
// CCanvasCtrl

class CCanvasCtrl : public CWnd
{
	DECLARE_DYNAMIC(CCanvasCtrl)

public:
	CCanvasCtrl();
	virtual ~CCanvasCtrl();
	BOOL Initialize(int width, int height);
	void PutPixel(int x, int y, COLORREF color);
	void Clear(COLORREF color, bool bCanvasOnly = false);
	void DrawTextString(const CString& str, int x, int y, COLORREF color);
	void Refresh(bool bErase = false);
	int GetWidth() const { return m_nWidth; }
	int GetHeight() const { return m_nHeight; }
	void SetDrawParameters(int radius, int borderThickness) { m_nCurrentRadius = radius; m_nCurrentBorderThickness = borderThickness; }
	void DrawFilledCircle(int cx, int cy, int r, COLORREF color);
	void DrawCircle(int cx, int cy, int r, COLORREF color = RGB(0, 0, 0));
	void SetPoints(CircleInfo* pCircles, int count) { m_Assignment.SetPoints(pCircles, count); }
	void RandomDrawCircle(COLORREF color) { m_Assignment.RandomDrawCircle(this, color); }
	int GetCurrentCircleCount() const { return m_Assignment.GetCurrentCircleCount(); }
	void SetRandomGroupOffset(int width, int height);
	void RandomMove();
	int GetCircumCircleRadius() const { return m_Assignment.GetCircumeCircleRadius(); }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg
		LRESULT OnInitCanvas(WPARAM, LPARAM);
	void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

private:
	CImage m_Image;
	int m_nWidth;
	int m_nHeight;
	int m_nCurrentRadius;
	int m_nCurrentBorderThickness;
	CAssignment m_Assignment;

	CBitmap m_Bitmap;
	CDC     m_MemDC;
	BYTE* m_pBits;
	bool m_bDragging;
	CPoint m_LastMousePos;
	int m_nSelectedIndex;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};



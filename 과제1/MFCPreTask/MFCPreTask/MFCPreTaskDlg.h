
// MFCPreTaskDlg.h: 헤더 파일
//

#pragma once
#include "CCanvasCtrl.h"
#include <atomic>

#define WM_UPDATE_CIRCLES (WM_APP + 100)
// CMFCPreTaskDlg 대화 상자
class CMFCPreTaskDlg : public CDialogEx
{
	// 생성입니다.
public:
	CMFCPreTaskDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCPRETASK_DIALOG };
#endif
public:
	int GetRadiusFromEdit();
	int GetBorderThicknessFromEdit();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


	// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CCanvasCtrl m_Canvas;
public:
	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnInit();
	afx_msg LRESULT OnUpdateCircles(WPARAM wParam, LPARAM lParam);

public:
	std::atomic<bool> m_bStopThread{ false };
	CWinThread* m_pThread = nullptr;
	CRect m_CanvasArea;
	afx_msg void OnBnClickedBtnRandom();
	int m_nDelay;
	int m_nRepeatCount;
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedBtnRandomOffset();
	// //마우스 포인트 Dot의 반지름
	int m_nDotRadius;
	// //외접원의 가장자리 두께
	int m_nBorder;
};
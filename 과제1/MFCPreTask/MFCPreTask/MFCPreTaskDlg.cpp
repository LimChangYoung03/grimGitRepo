
// MFCPreTaskDlg.cpp: 구현 파일
//
/*
* Pre Task Condition
* 1. 3개의 point를 입력창치를 통해 입력받는다.
* 2. point를 입력받을 때마다 해당 포인트를 중심으로하는 원을 그린다.
* 3. point입력시, 그려지는 원의반지름은 사용자가 ui를 통해 입력한 값을 사용한다.
* 3. 3번째 point를 입력받으면 세 원의 중심point를 모두 지나는 정원 하나를 그린다.
* 4. 4번째부터는 원이 그려지지 않는다.
* 5. 3개의 원의 중심을 지나가는 정원의 내부는 채워지지 않아야 하며 가장자리 두께는 사용자가 입력한 값을 사용한다.
* 6. 클릭 지점 3개중 한개를 마우스로 선택하여 드래그 하면 드래그 상태가 끝날때까지 정원이 이동한다.
* 7. 초기화 버튼을 누르면 그려졌던 모든 내용들을 삭제하고 초기 상태로 돌아간다.
* 8. 정원이 그려진 상태에서 랜덤 이도 버튼을 누르면 3개의 클릭지점 모두를 랜덤한 위치로 이동시킵니다.이때, 정원또한 다시 그려집니다.
* 9. 랜덤한 위치로 이동 및 정원그리기는 초당 2회 총 10회 자동 반복한다.
* 10. 각 클릭 시점의 중심 좌표가 화면에 텍스트로 표시되어야 한다.
* 
* => 외부 입력값:마우스 포인트 좌표, 원의 반지름, 가장자리 두께,
* 
*/
#include "pch.h"
#include "framework.h"
#include "MFCPreTask.h"
#include "MFCPreTaskDlg.h"
#include "afxdialogex.h"
#include "resource.h"
#include "CircleInfo.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
	
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCPreTaskDlg 대화 상자



CMFCPreTaskDlg::CMFCPreTaskDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCPRETASK_DIALOG, pParent)
	, m_nDelay(0)
	, m_nRepeatCount(0)
	, m_nDotRadius(0)
	, m_nBorder(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

int CMFCPreTaskDlg::GetRadiusFromEdit()
{
	BOOL bTranslated = FALSE;
	int radius = GetDlgItemInt(IDC_EDIT_RADIUS, &bTranslated, FALSE);
	if (!bTranslated || radius<=0)
	{
		AfxMessageBox(_T("반지름 입력이 올바르지 않습니다. 반지름의 크기느 0보다 큰 숫자를 입력해 주세요."));
		return 0;
	}
	return radius;
	
}

int CMFCPreTaskDlg::GetBorderThicknessFromEdit()
{
	BOOL bTranslated = FALSE;
	int borderThickness = GetDlgItemInt(IDC_EDIT_BORDER, &bTranslated, FALSE);
	if (!bTranslated || borderThickness <= 0)
	{
		AfxMessageBox(_T("가장자리 두께 입력이 올바르지 않습니다. 가장자리 두께는 0보다 큰 숫자를 입력해 주세요."));
		return 0;
	}
	return borderThickness;
}


void CMFCPreTaskDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DELAY, m_nDelay);
	DDX_Text(pDX, IDC_EDIT_REPEAT, m_nRepeatCount);
	DDX_Text(pDX, IDC_EDIT_RADIUS, m_nDotRadius);
	DDX_Text(pDX, IDC_EDIT_BORDER, m_nBorder);
}

BEGIN_MESSAGE_MAP(CMFCPreTaskDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_APPLY, &CMFCPreTaskDlg::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_INIT, &CMFCPreTaskDlg::OnBnClickedBtnInit)
	ON_MESSAGE(WM_UPDATE_CIRCLES, &CMFCPreTaskDlg::OnUpdateCircles)

	ON_BN_CLICKED(IDC_BTN_RANDOM, &CMFCPreTaskDlg::OnBnClickedBtnRandom)
	ON_BN_CLICKED(IDC_BTN_STOP, &CMFCPreTaskDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_Btn_RANDOM_OFFSET, &CMFCPreTaskDlg::OnBnClickedBtnRandomOffset)
END_MESSAGE_MAP()


// CMFCPreTaskDlg 메시지 처리기

BOOL CMFCPreTaskDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_Canvas.SubclassDlgItem(IDC_CANVAS, this);

	CRect rect;
	m_Canvas.GetClientRect(&rect);
	m_CanvasArea = rect;
	if (m_Canvas.Initialize(rect.Width(), rect.Height()))
	{
		m_Canvas.Clear(RGB(255, 255, 255));
	}

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMFCPreTaskDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 애플리케이션의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMFCPreTaskDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMFCPreTaskDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMFCPreTaskDlg::OnBnClickedBtnApply()
{
	int radius = GetRadiusFromEdit();
	int borderThickness = GetBorderThicknessFromEdit();
	m_Canvas.SetDrawParameters(radius, borderThickness);
}

void CMFCPreTaskDlg::OnBnClickedBtnInit()
{
	m_Canvas.Clear(RGB(255, 255, 255));

 }
LRESULT CMFCPreTaskDlg::OnUpdateCircles(WPARAM wParam, LPARAM lParam)
{
	CircleInfo* pData = (CircleInfo*)lParam;
	int count = (int)wParam;
	m_Canvas.SetPoints(pData,count);

	delete pData;

	m_Canvas.Clear(RGB(255, 255, 255),true);
	m_Canvas.RandomDrawCircle(RGB(0, 0, 0));
	m_Canvas.Invalidate();

	return 0;
	return LRESULT();
}
UINT RandomCircleThread(LPVOID pParam)
{
	CMFCPreTaskDlg* pDlg = (CMFCPreTaskDlg*)pParam;
	
	int rectWidth = pDlg->m_CanvasArea.Width();
	int rectHeight = pDlg->m_CanvasArea.Height();
	int delay = pDlg->m_nDelay;
	int repeatCount = pDlg->m_nRepeatCount;
	const int minDelay = 100; // 최소 100ms
	for (int i = 0; i < repeatCount; i++)
	{
		if(pDlg->m_bStopThread)
			break;
		int threadStopCheckInterval = delay/ minDelay; // 100ms마다 중지 여부 체크
		for (int t = 0; t < threadStopCheckInterval; t++)  // 2초를 100ms 단위로 나눔
		{
			if (pDlg->m_bStopThread)
				break;

			Sleep(minDelay);
		}
		CircleInfo* pData = new CircleInfo[MAX_POINT];
		// 랜덤 점 3개 생성

		for (int j = 0; j < MAX_POINT; j++)
		{
			if (pDlg->m_bStopThread)
				break;
			int r = pDlg->GetRadiusFromEdit();
			pData[j].centerX = r + rand() % (rectWidth - 2 * r);
			pData[j].centerY = r + rand() % (rectHeight - 2 * r);
			pData[j].radius = pDlg->GetRadiusFromEdit();
		}
		if (pDlg->m_bStopThread)
			break;
		// 외접원 계산도 여기서 가능

		// UI Thread에 전달
		pDlg->PostMessage(WM_UPDATE_CIRCLES, 3, (LPARAM)pData);
	}
	pDlg->m_pThread = nullptr;
	return 0;
}

void CMFCPreTaskDlg::OnBnClickedBtnRandom()
{
	UpdateData();
	if (m_nBorder <= 0 || m_nDotRadius <= 0)
	{
		AfxMessageBox(_T("반지름과 가장자리 두께는 0보다 큰 숫자를 입력해 주세요."));
		return;
	}
	if (m_nRepeatCount <= 0 || m_nDelay <= 0)
	{
		AfxMessageBox(_T("반복 횟수와 딜레이는 0보다 큰 숫자를 입력해 주세요."));
		return;
	}
	if(m_pThread != nullptr)
		return;

	m_bStopThread = false;
	AfxBeginThread(RandomCircleThread, this);
 }

void CMFCPreTaskDlg::OnBnClickedBtnStop()
{
	m_bStopThread = true;
}

void CMFCPreTaskDlg::OnBnClickedBtnRandomOffset()
{
	if (m_Canvas.GetCurrentCircleCount() < MAX_POINT)
	{
		AfxMessageBox(_T("3개의 원이 모두 그려진 상태에서만 랜덤 오프셋이 적용됩니다."));
		return;
	}
	int r = m_Canvas.GetCircumCircleRadius();
	int width = m_CanvasArea.Width();
	int heigh = m_CanvasArea.Height();

	m_Canvas.SetRandomGroupOffset(width, heigh);
	m_Canvas.RandomMove();


}

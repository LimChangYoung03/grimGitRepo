// CCanvasCtrl.cpp: 구현 파일
//

#include "pch.h"
#include "CCanvasCtrl.h"




// CCanvasCtrl

IMPLEMENT_DYNAMIC(CCanvasCtrl, CWnd)

CCanvasCtrl::CCanvasCtrl()
	: m_nWidth(0), m_nHeight(0)
{

}

CCanvasCtrl::~CCanvasCtrl()
{
	if (!m_Image.IsNull())
		m_Image.Destroy();
}

BOOL CCanvasCtrl::Initialize(int width, int height)
{
	if (width <= 0 || height <= 0)
		return FALSE;

	m_nWidth = width;
	m_nHeight = height;

	// 기존 리소스 정리
	if (m_MemDC.GetSafeHdc())
		m_MemDC.DeleteDC();

	if (m_Bitmap.GetSafeHandle())
		m_Bitmap.DeleteObject();

	BITMAPINFO bmi = {};
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;   // 🔥 Top-down
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	void* pBits = nullptr;

	HDC hdc = ::GetDC(NULL);

	HBITMAP hBmp = ::CreateDIBSection(
		hdc,
		&bmi,
		DIB_RGB_COLORS,
		&pBits,
		NULL,
		0
	);

	::ReleaseDC(NULL, hdc);

	if (!hBmp || !pBits)
		return FALSE;

	m_Bitmap.Attach(hBmp);

	m_MemDC.CreateCompatibleDC(NULL);
	m_MemDC.SelectObject(&m_Bitmap);

	m_pBits = (BYTE*)pBits;

	return TRUE;

}

void CCanvasCtrl::PutPixel(int x, int y, COLORREF color)
{
	if (!m_pBits)
		return;

	if (x < 0 || x >= m_nWidth ||
		y < 0 || y >= m_nHeight)
		return;

	BYTE* pPixel = m_pBits + (y * m_nWidth + x) * 4;

	pPixel[0] = GetBValue(color);
	pPixel[1] = GetGValue(color);
	pPixel[2] = GetRValue(color);
	pPixel[3] = 0;
}

void CCanvasCtrl::Clear(COLORREF color, bool bCanvasOnly)
{
	
	if (!m_pBits)
		return;

	for (int y = 0; y < m_nHeight; y++)
	{
		BYTE* pRow = m_pBits + y * m_nWidth * 4;

		for (int x = 0; x < m_nWidth; x++)
		{
			pRow[x * 4 + 0] = GetBValue(color);
			pRow[x * 4 + 1] = GetGValue(color);
			pRow[x * 4 + 2] = GetRValue(color);
			pRow[x * 4 + 3] = 0;
		}
	}
	if(!bCanvasOnly)
		m_Assignment.Initialize();

	Refresh();

}

void CCanvasCtrl::DrawTextString(const CString& str, int x, int y, COLORREF color)
{
	if (!m_MemDC.GetSafeHdc())
		return;

	m_MemDC.SetTextColor(color);
	m_MemDC.SetBkMode(TRANSPARENT);

	m_MemDC.TextOut(x, y, str);
}

void CCanvasCtrl::Refresh(bool bErase)
{
	Invalidate(bErase);
}


BEGIN_MESSAGE_MAP(CCanvasCtrl, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_MESSAGE(WM_APP + 1, &CCanvasCtrl::OnInitCanvas)   // 🔥 여기 추가

END_MESSAGE_MAP()



// CCanvasCtrl 메시지 처리기

LRESULT CCanvasCtrl::OnInitCanvas(WPARAM, LPARAM)
{
	CRect rect;
	GetClientRect(&rect);

	if (rect.Width() > 0 && rect.Height() > 0)
	{
		Initialize(rect.Width(), rect.Height());
		Clear(RGB(0, 0, 0));
		Invalidate();
	}

	return 0;
}

void CCanvasCtrl::OnPaint()
{
	CPaintDC dc(this);

	if (m_pBits)
	{
		dc.BitBlt(
			0, 0,
			m_nWidth,
			m_nHeight,
			&m_MemDC,
			0, 0,
			SRCCOPY
		);
	}
}

BOOL CCanvasCtrl::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	return true;
}

void CCanvasCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	if(m_nCurrentRadius <= 0)
	{
		AfxMessageBox(_T("반지름이 설정이  옳바르지 않습니다. 반지름은 0보다 큰 값을 입력하고 적용 버튼을 눌러 주세요."));
		return;
	}
	if (m_Assignment.GetCurrentCircleCount() >= MAX_POINT)
	{
		for (int i = 0; i < m_Assignment.GetCurrentCircleCount(); i++)
		{
			if (m_Assignment.IsPointInsideCircle(point.x, point.y, i))	
			{
			
				m_bDragging = true;
				m_nSelectedIndex = i;
				m_LastMousePos = point;
				break;
			}
		}
		return;
	}
	SetCapture();

	m_Assignment.AddCircle(point.x, point.y, m_nCurrentRadius);
	m_Assignment.DrawAll(this, RGB(0, 0, 0));
	
	Refresh();
	CWnd::OnLButtonDown(nFlags, point);
}

void CCanvasCtrl::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	m_bDragging = false;
	ReleaseCapture();
	CWnd::OnLButtonUp(nFlags, point);
}

void CCanvasCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_Assignment.GetCurrentCircleCount() <  MAX_POINT)
		return;
	if (nFlags & MK_LBUTTON)
	{
		CString str;
		str.Format(_T("Mouse Move: %d, %d"), point.x, point.y);

		if (m_bDragging)
		{
			int dx = point.x - m_LastMousePos.x;
			int dy = point.y - m_LastMousePos.y;
			m_Assignment.Move(dx, dy);
			Clear(RGB(255, 255, 255), true); // 캔버스는 지우고 할당 정보는 유지
			m_Assignment.DrawAll(this, RGB(0, 0, 0));
			Refresh();
			m_LastMousePos = point;
		}
	/*	DrawTextString(str, 0, 0, RGB(0, 0, 0));*/
	}
	CWnd::OnMouseMove(nFlags, point);
}

void CCanvasCtrl::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CCanvasCtrl::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CWnd::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		CRect rect;
		GetClientRect(&rect);

		if (rect.Width() > 0 && rect.Height() > 0)
		{
			Initialize(rect.Width(), rect.Height());
			Clear(RGB(0, 0, 0));
		}
	}
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
void CCanvasCtrl::DrawFilledCircle(int cx, int cy, int r, COLORREF color)
{
		
	int rSquared = r * r;

	for (int y = -r; y <= r; y++)
	{
		for (int x = -r; x <= r; x++)
		{
			if (x * x + y * y <= rSquared)
			{
				int pointX = cx + x;
				int pointY = cy + y;
				if (pointX >= 0 && pointX < m_nWidth && pointY >= 0 && pointY < m_nHeight)
					PutPixel(cx + x, cy + y, color);
			}
		}
	}
}

void CCanvasCtrl::DrawCircle(int cx, int cy, int r, COLORREF color)
{
	if (!m_pBits)
        return;
	int border = m_nCurrentBorderThickness;
    if (r <= 0 || border <= 0)
        return;

    if (border > r)
        border = r;

	// ---- 클리핑 범위 계산 ----//Canvas 밖으로 원이 나가지 않도록 클리핑 범위를 계산
    int minX = max(0, cx - r);
    int maxX = min(m_nWidth  - 1, cx + r);

    int minY = max(0, cy - r);
    int maxY = min(m_nHeight - 1, cy + r);

    int outerR2 = r * r;
    int innerR  = r - border;
    int innerR2 = innerR * innerR;

    for (int y = minY; y <= maxY; y++)
    {
        int dy = y - cx;   
        dy = y - cy;       

        for (int x = minX; x <= maxX; x++)
        {
            int dx = x - cx;

            int dist2 = dx * dx + dy * dy;

            if (dist2 <= outerR2 && dist2 >= innerR2)
            {
                PutPixel(x, y, color);
            }
        }
    }
}
/// <summary>
/// canvas 내에서 원 그룹의 위치를 랜덤하게 이동시키기 위한 offset을 계산하는 함수입니다.
/// canvas의 크기와 3점들의 중심 좌표를 이용하여, 
/// 원 그룹이 canvas 밖으로 나가지 않도록 offset의 범위를 계산한 후, 그 범위 내에서 랜덤한 offset 값을 설정합니다.
/// </summary>
/// <param name="width"></param>
/// <param name="height"></param>
void CCanvasCtrl::SetRandomGroupOffset(int width, int height)
{

	int circleCount = m_Assignment.GetCurrentCircleCount();
	if (circleCount == 0)
		return;

	int minX = INT_MAX, maxX = INT_MIN;
	int minY = INT_MAX, maxY = INT_MIN;

	for (int i = 0; i < circleCount; i++)
	{
		minX = min(minX, m_Assignment.GetCircleInfo(i).centerX);
		maxX = max(maxX, m_Assignment.GetCircleInfo(i).centerX);
		minY = min(minY, m_Assignment.GetCircleInfo(i).centerY);
		maxY = max(maxY, m_Assignment.GetCircleInfo(i).centerY);
	}

	int offsetX_min = -minX;
	int offsetX_max = width - maxX;

	int offsetY_min = -minY;
	int offsetY_max = height - maxY;

	if (offsetX_max >= offsetX_min)
		m_Assignment.m_nGroupOffset_X = offsetX_min + rand() % (offsetX_max - offsetX_min + 1);

	if (offsetY_max >= offsetY_min)
		m_Assignment.m_nGroupOffset_Y = offsetY_min + rand() % (offsetY_max - offsetY_min + 1);

}

void CCanvasCtrl::RandomMove()
{
	Clear(RGB(255, 255, 255), true); // 캔버스는 지우고 할당 정보는 유지
	m_Assignment.DrawAll(this, RGB(0, 0, 0));
	Refresh();
}


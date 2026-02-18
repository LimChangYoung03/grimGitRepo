#include "pch.h"
#include "CAssignment.h"
#include "CCanvasCtrl.h"

CAssignment::CAssignment() : m_nCount(0)
{
}

CAssignment::~CAssignment()
{

}

bool CAssignment::Initialize()
{
	for (int i = 0; i < MAX_POINT; i++)
	{
		m_Circles[i].centerX = 0;
		m_Circles[i].centerY = 0;
		m_Circles[i].radius = 0;
	}
	m_nCount = 0;
	m_nGroupOffset_X = 0;
	m_nGroupOffset_Y = 0;
	m_bCircumCircleCreated = false;
	m_CircumCircle.centerX = 0;
	m_CircumCircle.centerY = 0;
	m_CircumCircle.radius = 0;

	return true;
}

void CAssignment::AddCircle(int centerX, int centerY, int radius)
{
	if (m_nCount >= MAX_POINT)
		return;

	m_Circles[m_nCount].centerX = centerX;
	m_Circles[m_nCount].centerY = centerY;
	m_Circles[m_nCount].radius = radius;
	m_nCount++;

}

void CAssignment::DrawAll(CCanvasCtrl* pCanvas, COLORREF color)
{
	TRACE("DrawAll pCanvas = %p\n", pCanvas);

	if(!pCanvas)
		return;
	if(m_nCount < MAX_POINT)
		m_nGroupOffset_X = m_nGroupOffset_Y = 0;
		
	int center_x, center_y, radius;
	if (m_nCount == MAX_POINT)
	{
		if (m_bCircumCircleCreated)
		{
			center_x = m_CircumCircle.centerX;
			center_y = m_CircumCircle.centerY;
			radius = m_CircumCircle.radius;
		}
		else
		{
			bool rst = CreateCircumcircle(center_x, center_y, radius);
			m_bCircumCircleCreated = rst;

		}
		if(m_bCircumCircleCreated);
		{
			center_x += m_nGroupOffset_X;
			center_y += m_nGroupOffset_Y;
			pCanvas->DrawCircle(center_x, center_y, radius, RGB(0, 0, 255));
			CString str;
			str.Format(_T("Center_%d, %d"), center_x, center_y);

			pCanvas->DrawTextString(str, center_x, center_y, RGB(255, 0, 0));
		}
	}
	for (int i = 0; i < m_nCount; i++)
	{
		int centerX = m_Circles[i].centerX + m_nGroupOffset_X;
		int centerY = m_Circles[i].centerY + m_nGroupOffset_Y;
		pCanvas->DrawFilledCircle(centerX, centerY, m_Circles[i].radius, color);
		CString str;
		str.Format(_T("Center_%d, %d"), centerX, centerY);

		pCanvas->DrawTextString(str, centerX,centerY, RGB(0, 255, 0));
	}
	
	
	

	//pCanvas->Refresh();
		
}

int CAssignment::GetCurrentCircleCount() const
{
	return m_nCount;
}

bool CAssignment::CreateCircumcircle(int& center_x, int& center_y, int& radius)
{
	if(m_nCount < MAX_POINT)
		return false;

	int x1 = m_Circles[0].centerX;
	int y1 = m_Circles[0].centerY;
	int x2 = m_Circles[1].centerX;
	int y2 = m_Circles[1].centerY;
	int x3 = m_Circles[2].centerX;
	int y3 = m_Circles[2].centerY;

	double A = x2 - x1;
	double B = y2 - y1;
	double C = x3 - x1;
	double D = y3 - y1;

	double G = 2 * (A * (y3 - y2) - B * (x3 - x2));
	if (fabs(G) < 1e-6)//|G| < 0.000001
	{
		center_x = center_y = radius = 0;
		
		return false;
	}
	else
	{
		double E = A * (x1 + x2) + B * (y1 + y2);
		double F = C * (x1 + x3) + D * (y1 + y3);

		double cx = (D * E - B * F) / G;
		double cy = (A * F - C * E) / G;

		double dx = cx - x1;
		double dy = cy - y1;

		double r = sqrt(dx * dx + dy * dy);

		center_x = (int)cx;
		center_y = (int)cy;
		radius = (int)r;

		m_CircumCircle.centerX = center_x;
		m_CircumCircle.centerY = center_y;
		m_CircumCircle.radius = radius;
		return true;
	}
}

void CAssignment::Move(int dx, int dy)
{
	m_nGroupOffset_X += dx;
	m_nGroupOffset_Y += dy;
}

bool CAssignment::IsPointInsideCircle(int px, int py ,int index)
{
	CircleInfo c = m_Circles[index];
	int offsetX = m_nGroupOffset_X;
	int offsetY = m_nGroupOffset_Y;
	int cx = c.centerX + offsetX;
	int cy = c.centerY + offsetY;
	int dx = px - cx;
	int dy = py - cy;
	int r = c.radius;

	return dx * dx + dy * dy <= r * r;
}

void CAssignment::SetPoints(CircleInfo* pCircles, int count)
{
	if (!pCircles || count <= 0)
		return;

	m_nCount = min(count, MAX_POINT);

	for (int i = 0; i < m_nCount; i++)
	{
		m_Circles[i] = pCircles[i];
	}
}

void CAssignment::RandomDrawCircle(CCanvasCtrl* pCanvas, COLORREF color)
{
	int center_x, center_y, radius;
	bool rst = CreateCircumcircle(center_x, center_y, radius);
	m_bCircumCircleCreated = rst;

	if (rst)
	{
		pCanvas->DrawFilledCircle(center_x, center_y, radius, color);
		pCanvas->Refresh();
	}
	
}

CircleInfo CAssignment::GetCircleInfo(int index) const
{
	if (index < 0 || index >= m_nCount)
		return CircleInfo();

	return m_Circles[index];
}





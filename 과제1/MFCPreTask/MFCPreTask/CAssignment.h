#pragma once
#include "CircleInfo.h"
class CCanvasCtrl;

class CAssignment
{
private:
	
public:
	
	

	CAssignment();
	~CAssignment();
	bool Initialize();
	void AddCircle(int centerX, int centerY, int radius);
	void DrawAll(CCanvasCtrl* pCanvas, COLORREF color);
	int GetCurrentCircleCount() const;
	bool CreateCircumcircle(int &center_x, int &center_y, int &radius);
	void Move(int dx, int dy);
	bool IsPointInsideCircle(int px, int py, int idx);
	void SetPoints(CircleInfo* pCircles, int count);
	void RandomDrawCircle(CCanvasCtrl* pCanvas, COLORREF color);
	int GetCircumeCircleRadius() const { return m_CircumCircle.radius; }
	int m_nGroupOffset_X;
	int m_nGroupOffset_Y;
	CircleInfo GetCircleInfo(int index) const;
private: 




	CircleInfo m_Circles[MAX_POINT];
	CircleInfo m_CircumCircle;
	bool m_bCircumCircleCreated;

	int m_nCount;

	

};


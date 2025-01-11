// gPrjDlg.cpp : 구현 파일

#include "stdafx.h"
#include "gPrj.h"
#include "gPrjDlg.h"
#include "afxdialogex.h"
#include <cmath>
#include <string>
#include <iostream>
#include <cstdlib> 
#include <ctime>
using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#define CENTER_RAD 5
#endif

class CAboutDlg : public CDialogEx {
public:
	CAboutDlg();
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif
protected:
	virtual void DoDataExchange(CDataExchange* pDX);
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {}
void CAboutDlg::DoDataExchange(CDataExchange* pDX) { CDialogEx::DoDataExchange(pDX); }
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

CgPrjDlg::CgPrjDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_GPRJ_DIALOG, pParent), cnt(0), pointSize(4), thickness(2), draggedIndex(-1), isDragging(false), dragOffset(0, 0) {
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CgPrjDlg::DoDataExchange(CDataExchange* pDX) {
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CgPrjDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN() 
	ON_WM_MOUSEMOVE() 
	ON_WM_LBUTTONUP() 
	ON_BN_CLICKED(BTN_RESET, &CgPrjDlg::OnBnClickedReset)
	ON_EN_CHANGE(IDC_RADIUS, &CgPrjDlg::OnEnChangeRadius)
	ON_EN_CHANGE(IDC_THICKNESS, &CgPrjDlg::OnEnChangeThickness)
	ON_BN_CLICKED(IDC_BTN_RANDOM, &CgPrjDlg::OnBnClickedBtnRandom)
END_MESSAGE_MAP()

void CgPrjDlg::callFunc(int n)
{
	std::cout << "callFunc called with: " << n << std::endl;
}

BOOL CgPrjDlg::OnInitDialog() {
	CDialogEx::OnInitDialog();
	SetIcon(m_hIcon, TRUE);  
	SetIcon(m_hIcon, FALSE);  
	return TRUE;
}

void CgPrjDlg::OnPaint() {
	CPaintDC dc(this); 

	if (cnt < 3) {
		CPen blackPen(PS_SOLID, 2, RGB(0, 0, 0));
		CBrush blackBrush(RGB(0, 0, 0));

		CPen* pOldPen = dc.SelectObject(&blackPen);
		CBrush* pOldBrush = dc.SelectObject(&blackBrush);

		for (int i = 0; i < cnt; i++) {
			dc.Ellipse(points[i].x - pointSize, points[i].y - pointSize,
				points[i].x + pointSize, points[i].y + pointSize);
		}

		dc.SelectObject(pOldPen);
		dc.SelectObject(pOldBrush);
	}

	if (cnt == 3) {

		POINT center;
		CalculateCircle(points[0], points[1], points[2], center, radius);

		CPen circlePen(PS_SOLID, thickness, RGB(0, 0, 0));
		CPen* pOldPen = dc.SelectObject(&circlePen);

		dc.Ellipse(center.x - (int)radius, center.y - (int)radius,
			center.x + (int)radius, center.y + (int)radius);

		CPen centerPen(PS_SOLID, 2, RGB(0, 0, 0));
		dc.SelectObject(&centerPen);
		dc.Ellipse(center.x - CENTER_RAD, center.y - CENTER_RAD,
			center.x + CENTER_RAD, center.y + CENTER_RAD);
		
		CString str;
		str.Format(_T("Center: (%ld, %ld)"), center.x, center.y);
		dc.SetTextColor(RGB(0, 0, 0)); 
		dc.TextOut(center.x + 10, center.y, str); 

		for (int i = 0; i < cnt; i++) {
			CPen blackPen(PS_SOLID, 2, RGB(0, 0, 0)); 
			CBrush blackBrush(RGB(0, 0, 0)); 

			CPen* pOldPen = dc.SelectObject(&blackPen);
			CBrush* pOldBrush = dc.SelectObject(&blackBrush);

			for (int i = 0; i < cnt; i++) {
				dc.Ellipse(points[i].x - pointSize, points[i].y - pointSize,
					points[i].x + pointSize, points[i].y + pointSize);
			}

			dc.SelectObject(pOldPen);
			dc.SelectObject(pOldBrush);
		}
	}
}

void CgPrjDlg::OnLButtonDown(UINT nFlags, CPoint point) {
	if (cnt < 3) {

		points[cnt].x = point.x;
		points[cnt].y = point.y;
		cnt++;
		Invalidate();
	}
	else {
		for (int i = 0; i < cnt; i++) {
			int dist = static_cast<int>(sqrt(pow(points[i].x - point.x, 2) + pow(points[i].y - point.y, 2)));
			if (dist <= 6) { 
				draggedIndex = i;
				dragOffset.x = point.x - points[i].x;
				dragOffset.y = point.y - points[i].y;
				isDragging = true; 
				break;
			}
		}
	}

	CDialogEx::OnLButtonDown(nFlags, point);
}

void CgPrjDlg::CalculateCircle(POINT p1, POINT p2, POINT p3, POINT& center, double& radius) {
	double offset = pow(p2.x, 2) + pow(p2.y, 2);
	double bc = (pow(p1.x, 2) + pow(p1.y, 2) - offset) / 2.0;
	double cd = (offset - pow(p3.x, 2) - pow(p3.y, 2)) / 2.0;
	double det = (p1.x - p2.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p2.y);
	if (fabs(det) < 1e-10) {
		return; 
	}
	det = 1 / det;
	double cx = (bc * (p2.y - p3.y) - cd * (p1.y - p2.y)) * det;
	double cy = ((p1.x - p2.x) * cd - (p2.x - p3.x) * bc) * det;
	center.x = static_cast<LONG>(cx);
	center.y = static_cast<LONG>(cy);
	radius = sqrt(pow(p2.x - cx, 2) + pow(p2.y - cy, 2));
}

void CgPrjDlg::OnDestroy() {
	CDialogEx::OnDestroy();
}

void CgPrjDlg::OnSysCommand(UINT nID, LPARAM lParam) {
	if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else {
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CgPrjDlg::OnMouseMove(UINT nFlags, CPoint point) {
	if (isDragging) {
		points[draggedIndex].x = point.x - dragOffset.x;
		points[draggedIndex].y = point.y - dragOffset.y;
		Invalidate(); 
	}
	CDialogEx::OnMouseMove(nFlags, point);
}

void CgPrjDlg::OnLButtonUp(UINT nFlags, CPoint point) {
	isDragging = false; 
	Invalidate();
	CDialogEx::OnLButtonUp(nFlags, point);
}

void CgPrjDlg::OnBnClickedReset()
{
	cnt = 0; 
	memset(points, 0, sizeof(points));
	Invalidate();
}

void CgPrjDlg::OnEnChangeRadius()
{
	CString strRadius;
	GetDlgItem(IDC_RADIUS)->GetWindowText(strRadius);
	pointSize = _ttoi(strRadius); 
	Invalidate();
}

void CgPrjDlg::OnEnChangeThickness()
{
	CString strThickness;
	GetDlgItem(IDC_THICKNESS)->GetWindowText(strThickness); 
	thickness = _ttoi(strThickness); 
	Invalidate();  
}

void CgPrjDlg::OnBnClickedBtnRandom()
{
	CRect clientRect;
	GetClientRect(&clientRect);
	int width = clientRect.Width();
	int height = clientRect.Height();

	for (int i = 0; i < 3; i++) {
		points[i].x = rand() % width; 
		points[i].y = rand() % height;
	}

	cnt = 3;
	Invalidate();
}

HCURSOR CgPrjDlg::OnQueryDragIcon() {
	return static_cast<HCURSOR>(m_hIcon);
}
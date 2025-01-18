#pragma once
#include "DlgImage.h"
#include <cmath>
#include <cstdlib>
#include <ctime>
#define WM_USER_RANDOM_MOVE (WM_USER + 2)
#define MAXCNT 3

class CgPrjDlg : public CDialogEx
{
public:
    CgPrjDlg(CWnd* pParent = NULL);
    virtual BOOL OnInitDialog();    

#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_GPRJ_DIALOG };
#endif

protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    DECLARE_MESSAGE_MAP()

private:
    void CalculateCircle(POINT p1, POINT p2, POINT p3, POINT& center, double& radius);
    void StartDrawingThread();
    static UINT MoveAndDrawThread(LPVOID pParam);
    void DrawPoint(HDC hdc, int cx, int cy, int r);
    void DrawCircleOutline(HDC hdc, int cx, int cy, int r, int thickness);

    afx_msg void OnBnClickedReset();
    afx_msg void OnBnClickedBtnRandom();
    afx_msg void OnBnClickedStartMoveAndDraw();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnEnChangeRadius();
    afx_msg void OnEnChangeThickness();
    afx_msg void OnPaint();
    LRESULT OnRandomMove(WPARAM wParam, LPARAM lParam);

    POINT points[MAXCNT];
    int cnt;
    int pointsize;
    double radius;
    int thickness;
    bool isDragging;
    int draggedIndex;
    CPoint dragOffset;
    HICON m_hIcon;
};

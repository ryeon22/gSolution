#pragma once
#include "DlgImage.h"

class CgPrjDlg : public CDialogEx
{
public:
	CgPrjDlg(CWnd* pParent = NULL);	
	
	void callFunc(int n);
	afx_msg void OnBnClickedReset();
	afx_msg void OnEnChangeRadius();
	afx_msg void OnEnChangeThickness();
	void OnMouseMove(UINT nFlags, CPoint point);
	void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedBtnRandom();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GPRJ_DIALOG };
#endif

protected:

	CDlgImage* m_pDlgImage;
	CDlgImage* m_pDlgImgResult;
	POINT points[3];
	int cnt;
	int pointSize;
	double radius;
	int thickness;
	virtual void DoDataExchange(CDataExchange* pDX);
	HICON m_hIcon;
	int draggedIndex; 
	bool isDragging;  
	CPoint dragOffset; 

	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();

	void CalculateCircle(POINT p1, POINT p2, POINT p3, POINT& center, double& radius);

	DECLARE_MESSAGE_MAP()	
};

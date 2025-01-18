#include "stdafx.h"
#include "gPrj.h"
#include "gPrjDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

BEGIN_MESSAGE_MAP(CgPrjDlg, CDialogEx)
    ON_BN_CLICKED(BTN_RESET, &CgPrjDlg::OnBnClickedReset)
    ON_BN_CLICKED(IDC_BTN_RANDOM, &CgPrjDlg::OnBnClickedBtnRandom)
    ON_BN_CLICKED(IDC_StartMoveAndDraw, &CgPrjDlg::OnBnClickedStartMoveAndDraw)
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
    ON_WM_LBUTTONUP()
    ON_EN_CHANGE(IDC_RADIUS, &CgPrjDlg::OnEnChangeRadius)
    ON_EN_CHANGE(IDC_THICKNESS, &CgPrjDlg::OnEnChangeThickness)
    ON_MESSAGE(WM_USER_RANDOM_MOVE, &CgPrjDlg::OnRandomMove)
    ON_WM_PAINT()
END_MESSAGE_MAP()

CgPrjDlg::CgPrjDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_GPRJ_DIALOG, pParent), cnt(0), pointsize(5), radius(0),
    thickness(2), isDragging(false), draggedIndex(-1), m_hIcon(NULL) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CgPrjDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
}

BOOL CgPrjDlg::OnInitDialog() {
    CDialogEx::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);
    return TRUE;
}

/*
* @brief ȭ�鿡 �׸��� 
*/
void CgPrjDlg::OnPaint() {
    PAINTSTRUCT ps;
    HDC hdc = ::BeginPaint(m_hWnd, &ps); 

    // Ŭ���� �� �׸���
    for (int i = 0; i < cnt; ++i) {
        DrawPoint(hdc, points[i].x, points[i].y, pointsize); 
    }

    // ���� �׸���
    if (cnt == MAXCNT) {
        POINT center;
        CalculateCircle(points[0], points[1], points[2], center, radius);

        DrawCircleOutline(hdc, center.x, center.y, static_cast<int>(radius), thickness);
        DrawPoint(hdc, center.x, center.y, 4); 

        CString str;
        str.Format(_T("(%d, %d)"), center.x, center.y);
        ::TextOut(hdc, center.x + 10, center.y - 10, str, str.GetLength());
    }

    ::EndPaint(m_hWnd, &ps); 
}

/*
* @brief �ʱ�ȭ ��ư Ŭ�� �� ȣ��
*/
void CgPrjDlg::OnBnClickedReset() {
    cnt = 0;
    memset(points, 0, sizeof(points));
    Invalidate();
}

/*
* @brief ���� �̵� 1ȸ ��ư Ŭ�� �� ȣ��
*/
void CgPrjDlg::OnBnClickedBtnRandom() {
    CRect clientRect;
    GetClientRect(&clientRect);
    for (int i = 0; i < MAXCNT; ++i) {
        points[i].x = rand() % clientRect.Width();
        points[i].y = rand() % clientRect.Height();
    }
    cnt = MAXCNT;
    Invalidate();
}

/*
* @brief ���� �̵� 10ȸ ��ư Ŭ�� �� ȣ��
*/
void CgPrjDlg::OnBnClickedStartMoveAndDraw() {
    StartDrawingThread();
}

/*
* @brief ���� �̵� �� �׸��� ������ �Լ� 
* @param pParam ������ �Լ��� �Ű����� (CgPrjDlg ������)
* @return ������ ���� ���� 
*/
UINT CgPrjDlg::MoveAndDrawThread(LPVOID pParam) {
    CgPrjDlg* pDlg = reinterpret_cast<CgPrjDlg*>(pParam);
    if (!pDlg) return 1;

    for (int i = 0; i < 10; ++i) {
        pDlg->PostMessage(WM_USER_RANDOM_MOVE, 0, 0);
        Sleep(500);
    }
    return 0;
}

/*
* @brief ���� �̵� �� �׸��� ������ ���� 
*/
void CgPrjDlg::StartDrawingThread() {
    AfxBeginThread(MoveAndDrawThread, this);
}

/*
* @brief ���� �̵� �޽��� ó�� 
* @return �޽��� ó�� ��� 
*/
LRESULT CgPrjDlg::OnRandomMove(WPARAM wParam, LPARAM lParam) {
    OnBnClickedBtnRandom();
    return 0;
}

/*
* @brief ���콺 ���� ��ư Ŭ�� �� ȣ��
* @param nFlags ���� �÷���
* @param point Ŭ���� ��ǥ
*/
void CgPrjDlg::OnLButtonDown(UINT nFlags, CPoint point) {
    if (cnt < MAXCNT) {
        points[cnt++] = point;
        Invalidate();
    } else {
        for (int i = 0; i < MAXCNT; ++i) {
            int dx = points[i].x - point.x;
            int dy = points[i].y - point.y;
            if (dx * dx + dy * dy <= 100) {
                isDragging = true;
                draggedIndex = i;
                dragOffset = { dx, dy };
                break;
            }
        }
    }
}

/*
* @brief ���콺 �̵� �� ȣ��
* @param nFlags ���� �÷���
* @param point ���� ���콺 ��ǥ 
*/
void CgPrjDlg::OnMouseMove(UINT nFlags, CPoint point) {
    if (isDragging && draggedIndex >= 0) {
        points[draggedIndex] = { point.x - dragOffset.x, point.y - dragOffset.y };
        Invalidate();
    }
}

/*
* @brief ���콺 ���� ��ư ���� �� ȣ��
* @param nFlags ���� �÷���
* @param point ���� ���콺 ��ǥ
*/
void CgPrjDlg::OnLButtonUp(UINT nFlags, CPoint point) {
    isDragging = false;
    draggedIndex = -1;
}

/*
* @brief ������ �Է� ���� �� ȣ��
*/
void CgPrjDlg::OnEnChangeRadius() {
    CString str;
    GetDlgItem(IDC_RADIUS)->GetWindowText(str);
    pointsize = _ttoi(str);
    Invalidate();
}

/*
* @brief �����ڸ� �β� �Է� ���� �� ȣ��
*/
void CgPrjDlg::OnEnChangeThickness() {
    CString str;
    GetDlgItem(IDC_THICKNESS)->GetWindowText(str);
    thickness = _ttoi(str);
    Invalidate();
}

/*
* @brief �� ���� ���� ���� �߽ɰ� �������� ����ϴ� �Լ�
* @param p1, p2, p3 ù��°~����° ��
* @param center, radius ���� ���� �߽� ��ǥ, ������
*/
void CgPrjDlg::CalculateCircle(POINT p1, POINT p2, POINT p3, POINT& center, double& radius) {
    double dx1 = p2.x - p1.x, dy1 = p2.y - p1.y;
    double mid1 = (dx1 * (p1.x + p2.x) + dy1 * (p1.y + p2.y)) / 2;
    double dx2 = p3.x - p1.x, dy2 = p3.y - p1.y;
    double mid2 = (dx2 * (p1.x + p3.x) + dy2 * (p1.y + p3.y)) / 2;
    double det = dx1 * dy2 - dx2 * dy1;

    if (fabs(det) < 1e-6) return;

    center.x = static_cast<LONG>((dy2 * mid1 - dy1 * mid2) / det);
    center.y = static_cast<LONG>((dx1 * mid2 - dx2 * mid1) / det);
    radius = sqrt(pow(center.x - p1.x, 2) + pow(center.y - p1.y, 2));
}

/*
* @brief ���� �׸��� �Լ�
* @param hdc ����̽� ���ؽ�Ʈ
* @param cx, cy, r �߽� x,y ��ǥ , ������
*/
void CgPrjDlg::DrawPoint(HDC hdc, int cx, int cy, int r) {
    for (int y = -r; y <= r; ++y) {
        for (int x = -r; x <= r; ++x) {
            if (x * x + y * y <= r * r) {
                ::SetPixel(hdc, cx + x, cy + y, RGB(0,0,0)); 
            }
        }
    }
}

/*
* @brief ���� �ܰ��� �׸��� �Լ�
* @param hdc ����̽� ���ؽ�Ʈ
* @param cx, cy, r �߽� x,y ��ǥ , ������
* @param thickness �� �β�
*/
void CgPrjDlg::DrawCircleOutline(HDC hdc, int cx, int cy, int r, int thickness) {
    for (int y = -r - thickness; y <= r + thickness; ++y) {
        for (int x = -r - thickness; x <= r + thickness; ++x) {
            int distanceSquared = x * x + y * y;
            int outerRadiusSquared = r * r;
            int innerRadiusSquared = (r - thickness) * (r - thickness);

            if (distanceSquared <= outerRadiusSquared && distanceSquared >= innerRadiusSquared) {
                ::SetPixel(hdc, cx + x, cy + y, RGB(0, 0, 0));
            }
        }
    }
}


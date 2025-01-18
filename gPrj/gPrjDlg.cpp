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
* @brief 화면에 그리기 
*/
void CgPrjDlg::OnPaint() {
    PAINTSTRUCT ps;
    HDC hdc = ::BeginPaint(m_hWnd, &ps); 

    // 클릭된 점 그리기
    for (int i = 0; i < cnt; ++i) {
        DrawPoint(hdc, points[i].x, points[i].y, pointsize); 
    }

    // 정원 그리기
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
* @brief 초기화 버튼 클릭 시 호출
*/
void CgPrjDlg::OnBnClickedReset() {
    cnt = 0;
    memset(points, 0, sizeof(points));
    Invalidate();
}

/*
* @brief 랜덤 이동 1회 버튼 클릭 시 호출
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
* @brief 랜덤 이동 10회 버튼 클릭 시 호출
*/
void CgPrjDlg::OnBnClickedStartMoveAndDraw() {
    StartDrawingThread();
}

/*
* @brief 랜덤 이동 및 그리기 스레드 함수 
* @param pParam 스레드 함수에 매개변수 (CgPrjDlg 포인터)
* @return 스레드 실행 상태 
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
* @brief 랜덤 이동 및 그리기 스레드 시작 
*/
void CgPrjDlg::StartDrawingThread() {
    AfxBeginThread(MoveAndDrawThread, this);
}

/*
* @brief 랜덤 이동 메시지 처리 
* @return 메시지 처리 결과 
*/
LRESULT CgPrjDlg::OnRandomMove(WPARAM wParam, LPARAM lParam) {
    OnBnClickedBtnRandom();
    return 0;
}

/*
* @brief 마우스 왼쪽 버튼 클릭 시 호출
* @param nFlags 상태 플래그
* @param point 클릭된 좌표
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
* @brief 마우스 이동 시 호출
* @param nFlags 상태 플래그
* @param point 현재 마우스 좌표 
*/
void CgPrjDlg::OnMouseMove(UINT nFlags, CPoint point) {
    if (isDragging && draggedIndex >= 0) {
        points[draggedIndex] = { point.x - dragOffset.x, point.y - dragOffset.y };
        Invalidate();
    }
}

/*
* @brief 마우스 왼쪽 버튼 해제 시 호출
* @param nFlags 상태 플래그
* @param point 현재 마우스 좌표
*/
void CgPrjDlg::OnLButtonUp(UINT nFlags, CPoint point) {
    isDragging = false;
    draggedIndex = -1;
}

/*
* @brief 반지름 입력 변경 시 호출
*/
void CgPrjDlg::OnEnChangeRadius() {
    CString str;
    GetDlgItem(IDC_RADIUS)->GetWindowText(str);
    pointsize = _ttoi(str);
    Invalidate();
}

/*
* @brief 가장자리 두께 입력 변경 시 호출
*/
void CgPrjDlg::OnEnChangeThickness() {
    CString str;
    GetDlgItem(IDC_THICKNESS)->GetWindowText(str);
    thickness = _ttoi(str);
    Invalidate();
}

/*
* @brief 세 점을 통해 원의 중심과 반지름을 계산하는 함수
* @param p1, p2, p3 첫번째~세번째 점
* @param center, radius 계산된 원의 중심 좌표, 반지름
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
* @brief 점을 그리는 함수
* @param hdc 디바이스 컨텍스트
* @param cx, cy, r 중심 x,y 좌표 , 반지름
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
* @brief 원의 외곽선 그리는 함수
* @param hdc 디바이스 컨텍스트
* @param cx, cy, r 중심 x,y 좌표 , 반지름
* @param thickness 선 두께
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


#include "framework.h"
#include "resource1.h"
#include "toolbar.h"
#include "resource.h"
#include <commctrl.h>


ToolBar::ToolBar(void) {}

void ToolBar::OnCreate(HWND hWnd, HINSTANCE hInst)
{
    TBBUTTON tbb[3]; //��� Toolbar � ������ ��������
    ZeroMemory(tbb, sizeof(tbb));
    for (int i = 0; i <= 2; i++) {
        tbb[i].iBitmap = i;
        tbb[i].fsState = TBSTATE_ENABLED;
        tbb[i].fsStyle = TBSTYLE_BUTTON; //��� �������� - ������
    }

    tbb[0].idCommand = ID_TOOL_OPEN; //��� ID ���� � ���������� WM_COMMAND
    tbb[1].idCommand = ID_TOOL_SAVE;
    tbb[2].idCommand = ID_TOOL_SAVEAS;

    int tbbSize = 3; // size of tbb
    int buttonPixels = 24; // pixels in one BITMAP button

    SendMessage(hWndToolBar, TB_ADDBUTTONS, tbbSize, (LPARAM)&tbb);


    hWndToolBar = CreateToolbarEx(hWnd, //���������� ����
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | CCS_TOP | TBSTYLE_TOOLTIPS,
        IDC_TOOLBAR, //ID ���������� ���� Toolbar
        tbbSize,
        hInst,
        IDB_BITMAP1,
        tbb,            //����� ����� ������
        tbbSize,        //������� ������
        buttonPixels, buttonPixels, buttonPixels, buttonPixels, //������������ �� ������
        sizeof(TBBUTTON));
}

void ToolBar::OnSize(HWND hWnd)
{
    RECT rc, rw;
    if (hWndToolBar)
    {
        GetClientRect(hWnd, &rc);        //��� ������ ��������� ����
        GetWindowRect(hWndToolBar, &rw); //��� ������� ����� ������ Toolbar
        MoveWindow(hWndToolBar, 0, 0, rc.right - rc.left, rw.bottom - rw.top, FALSE);
    }
}

void ToolBar::OnTool(HWND hWnd, LPARAM lParam)
{
    if (oldlParam)
    {
        SendMessage(hWndToolBar, TB_PRESSBUTTON, oldlParam, 0); // �������� ����� ������
    }
    SendMessage(hWndToolBar, TB_PRESSBUTTON, lParam, 1);        // �������� ���� ������
    oldlParam = lParam;
}

void ToolBar::OnNotify(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
    LPNMHDR pnmh = (LPNMHDR)lParam;
    if (pnmh->code == TTN_NEEDTEXT)
    {
        LPTOOLTIPTEXT lpttt = (LPTOOLTIPTEXT)lParam;
        switch (lpttt->hdr.idFrom)
        {

        case ID_TOOL_OPEN:
            lstrcpy(lpttt->szText, L"³������...");
            break;

        case ID_TOOL_SAVE:
            lstrcpy(lpttt->szText, L"��������");
            break;

        case ID_TOOL_SAVEAS:
            lstrcpy(lpttt->szText, L"�������� ��...");
            break;

        default: lstrcpy(lpttt->szText, L"���� �������");
        }
    }
}

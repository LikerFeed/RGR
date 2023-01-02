#include "framework.h"
#include "resource1.h"
#include "toolbar.h"
#include "resource.h"
#include <commctrl.h>


ToolBar::ToolBar(void) {}

void ToolBar::OnCreate(HWND hWnd, HINSTANCE hInst)
{
    TBBUTTON tbb[3]; //для Toolbar з трьома кнопками
    ZeroMemory(tbb, sizeof(tbb));
    tbb[0].iBitmap = 0;
    tbb[0].fsState = TBSTATE_ENABLED;
    tbb[0].fsStyle = TBSTYLE_BUTTON; //тип елементу - кнопка
    tbb[0].idCommand = ID_TOOL_OPEN; //цей ID буде у повідомленні WM_COMMAND

    tbb[1].iBitmap = 1;
    tbb[1].fsState = TBSTATE_ENABLED;
    tbb[1].fsStyle = TBSTYLE_BUTTON;
    tbb[1].idCommand = ID_TOOL_SAVE;

    tbb[2].iBitmap = 2;
    tbb[2].fsState = TBSTATE_ENABLED;
    tbb[2].fsStyle = TBSTYLE_BUTTON;
    tbb[2].idCommand = ID_TOOL_SAVEAS;

    SendMessage(hWndToolBar, TB_ADDBUTTONS, 3, (LPARAM)&tbb);

    hWndToolBar = CreateToolbarEx(hWnd, //батьківське вікно
        WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | CCS_TOP | TBSTYLE_TOOLTIPS,
        IDC_TOOLBAR, //ID дочірнього вікна Toolbar
        3,
        hInst,
        IDB_BITMAP1,
        tbb,            //масив опису кнопок
        3,              //кількість кнопок
        24, 24, 24, 24, //розташування та розміри
        sizeof(TBBUTTON));
}

void ToolBar::OnSize(HWND hWnd)
{
    RECT rc, rw;
    if (hWndToolBar)
    {
        GetClientRect(hWnd, &rc);        //нові розміри головного вікна
        GetWindowRect(hWndToolBar, &rw); //нам потрібно знати висоту Toolbar
        MoveWindow(hWndToolBar, 0, 0, rc.right - rc.left, rw.bottom - rw.top, FALSE);
    }
}

void ToolBar::OnTool(HWND hWnd, LPARAM lParam)
{
    if (oldlParam)
    {
        SendMessage(hWndToolBar, TB_PRESSBUTTON, oldlParam, 0); // відпустіть стару кнопку
    }
    SendMessage(hWndToolBar, TB_PRESSBUTTON, lParam, 1);        // натисніть нову кнопку
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
            lstrcpy(lpttt->szText, L"Відкрити...");
            break;

        case ID_TOOL_SAVE:
            lstrcpy(lpttt->szText, L"Зберегти");
            break;

        case ID_TOOL_SAVEAS:
            lstrcpy(lpttt->szText, L"Зберегти як...");
            break;

        default: lstrcpy(lpttt->szText, L"Щось невідоме");
        }
    }
}

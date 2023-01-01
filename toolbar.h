#pragma once
#include "framework.h"
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

class ToolBar
{
private:
	HWND hWndToolBar;
	LPARAM oldlParam = NULL;
public:
	ToolBar();
	void OnCreate(HWND, HINSTANCE);
	void OnSize(HWND);
	void OnTool(HWND, LPARAM);
	void OnNotify(HWND, WPARAM, LPARAM);
};

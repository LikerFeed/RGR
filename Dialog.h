#pragma once
#include "framework.h"
#include "commdlg.h"


class Dialog
{
public:
	static LPWSTR ProcessOpenDlg(HWND);
	static LPWSTR ProcessSaveAsDlg(HWND);

};

#include "Dialog.h"


LPWSTR Dialog::ProcessOpenDlg(HWND hWnd)
{
	LPWSTR fileName = new WCHAR[256];

	OPENFILENAME ofn;
	fileName[0] = 0;
	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = L"TSV-files	(*.tsv)\0*.tsv\0\0";
	ofn.lpstrDefExt = L"tsv";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = 256;

	if (GetOpenFileName(&ofn)) return fileName;
	else return NULL;
}

LPWSTR Dialog::ProcessSaveAsDlg(HWND hWnd)
{
	LPWSTR fileName = new WCHAR[256];

	OPENFILENAME ofn;
	fileName[0] = 0;
	memset(&ofn, 0, sizeof(OPENFILENAME));

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = L"TSV-files	(*.tsv)\0*.tsv\0\0";
	ofn.lpstrDefExt = L"tsv";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = 256;

	if (GetSaveFileName(&ofn)) return fileName;
	else return NULL;
}

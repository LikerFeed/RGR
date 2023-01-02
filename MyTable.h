#pragma once
#include "framework.h"
#include <commctrl.h>
#pragma comment (lib, "comctl32.lib")
#include <string>
#include <vector>
#include "Sort.h"

using namespace std;

class MyTableClass
{
private:
	HWND hWndList;
	Sort order;
	int selectedCol;
	vector<wstring> columns;
	vector<vector<wstring>> rows;
	RECT rcl;
private:
	void SetFont(HWND);
	void AddColumn(int, wstring, int);
	void AddRow(int, int, vector<wstring>);
	void FillTable(LPWSTR);
	void Clear();
	void HandleSort(LPARAM);
	vector<wstring> GetEntitiesStrings();
public:
	MyTableClass();
	MyTableClass(HWND);
	~MyTableClass();
	HWND GetHWND();
	void OnColumnClick(LPARAM);
	void OnSize(HWND);
	void OnFileOpen(HWND);
	void OnFileSave();
	void OnFileSaveAs(HWND);
	wstring GetCell(int, int);
	void SetText(int, int, wstring);
	int CompareListItemsAsc(LPARAM, LPARAM);
	int CompareListItemsDesc(LPARAM, LPARAM);
};

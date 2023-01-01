#pragma once
#include "MyTable.h"
#include "framework.h"
#include "TextChange.h"

typedef unsigned int uint;

class CellEditor
{
	HWND hEditable;
	MyTableClass* table;
	uint uCurrentItem;
	uint uCurrentSubItem;
	vector<TextChange> changes;
public:
	CellEditor();
	~CellEditor();
	CellEditor(MyTableClass*);
	LRESULT EditBoxWndProc(HWND, UINT, WPARAM, LPARAM);
	void CreateEditBox(HWND, NMLISTVIEW*);
	void OnDestroyWindow();
	void OnUndo();
};

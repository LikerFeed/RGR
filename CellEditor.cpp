#include "CellEditor.h"
#include "RGR.h"

static CellEditor* thisPtr = NULL;
static LRESULT WINAPI CallEditBoxWndProc(HWND, UINT, WPARAM, LPARAM);

CellEditor::CellEditor()
{
	this->table = nullptr;
	this->hEditable = NULL;
	this->uCurrentItem = 0;
	this->uCurrentSubItem = 0;
	thisPtr = this;
}

CellEditor::CellEditor(MyTableClass* tv)
{
	this->table = tv;
	this->hEditable = NULL;
	this->uCurrentItem = 0;
	this->uCurrentSubItem = 0;
	thisPtr = this;
}

CellEditor::~CellEditor()
{
}

void CellEditor::CreateEditBox(HWND hWnd, NMLISTVIEW* _lParam)
{
	const auto hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	RECT r;
	WNDPROC wpOld;
	const int bufferSize = 256; 
	// Створює поле редагування всередині комірки, яку потрібно змінити
	// Отримати координати верхнього лівого та нижнього правого кутів комірки
	ListView_GetSubItemRect(table->GetHWND(), _lParam->iItem, _lParam->iSubItem, LVIR_LABEL, &r);


	int width = r.right - r.left;
	int height = r.bottom - r.top;

	this->hEditable = CreateWindowEx(0, WC_EDIT,
		table->GetCell(_lParam->iItem, _lParam->iSubItem).c_str(),
		WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
		r.left, r.top,
		width,
		height,
		table->GetHWND(),
		NULL, hInst, 0);

	SendMessage(this->hEditable, EM_LIMITTEXT, bufferSize, 0); // Він приймає не більше bufferSize
	SendMessage(this->hEditable, EM_SETSEL, 0, bufferSize);	   // Вибор тексту
	SetFocus(this->hEditable);								   // Новий фокус

	// Замініть віконну процедуру, нова процедура - SubClass_ListView_WndProc
	wpOld = (WNDPROC)SetWindowLongPtr(this->hEditable, GWLP_WNDPROC, (LONG_PTR)CallEditBoxWndProc);

	// Збереження
	SetProp(this->hEditable, L"WP_OLD", (HANDLE)wpOld);
	SetProp(this->hEditable, L"ITEM", (HANDLE)_lParam->iItem);
	SetProp(this->hEditable, L"SUBITEM", (HANDLE)_lParam->iSubItem);
}


LRESULT CellEditor::EditBoxWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (LOWORD(wParam) == VK_RETURN)
		{
			LV_ITEM LvItem; // Коли натиснуто клавішу ENTER, зберегаєтья вставлений у піделемент текст
			WCHAR text[257] = L"";

			int iRow = (int)GetProp(thisPtr->hEditable, L"ITEM");
			int iCol = (int)GetProp(thisPtr->hEditable, L"SUBITEM");
			auto oldText = this->table->GetCell(iRow, iCol);

			TextChange change =
			{
				iCol,
				iRow,
				oldText
			};

			changes.push_back(change);

			LvItem.iItem = iRow;								   // Пункт змінено
			LvItem.iSubItem = iCol;								   // Підпункт змінено
			LvItem.pszText = text;								   // Де зберігати новий текст
			GetWindowText(thisPtr->hEditable, text, sizeof(text)); // Отримати новий текст і встановити його в підпункті
			SendMessage(thisPtr->table->GetHWND(), LVM_SETITEMTEXT, (WPARAM)GetProp(hEditable, L"ITEM"), (LPARAM)&LvItem);
			DestroyWindow(hEditable);							   // Знищеня поля редагування
		}
		else if (LOWORD(wParam) == VK_ESCAPE)
			DestroyWindow(thisPtr->hEditable);
		break;
	case WM_DESTROY:
		RemoveProp(thisPtr->hEditable, L"WP_OLD");
		RemoveProp(thisPtr->hEditable, L"ITEM");
		RemoveProp(thisPtr->hEditable, L"SUBITEM");

		SetWindowLong(thisPtr->hEditable, GWLP_WNDPROC, (LONG)GetProp(thisPtr->hEditable, L"WP_OLD"));
		hEditable = NULL;
		break;
	default:
		return CallWindowProc((WNDPROC)GetProp(hEditable, L"WP_OLD"),
			hwnd, uMsg, wParam, lParam);
	}
	return 0;
}

static LRESULT WINAPI CallEditBoxWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (thisPtr) return thisPtr->EditBoxWndProc(hwnd, uMsg, wParam, lParam);
}

void CellEditor::OnDestroyWindow()
{
	if (this->hEditable) DestroyWindow(this->hEditable);
}

void CellEditor::OnUndo()
{
	if (changes.size() > 0) 
	{
		auto change = changes[changes.size() - 1];
		table->SetText(change.iCol, change.iRow, change.text);
		changes.pop_back();
	}
	else return;
}

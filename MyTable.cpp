#include "MyTable.h"
#include "RGR.h"
#include "DataAndFiles.h"
#include "Dialog.h"


static MyTableClass* thisPtr = nullptr;
static int CALLBACK CallBackSortAsc(LPARAM, LPARAM, LPARAM);
static int CALLBACK CallBackSortDesc(LPARAM, LPARAM, LPARAM);

int numberSortingAsc(void* a, void* b) {
	int result = 0;
	if (a > b) result = 1;
	else if (a < b) result = -1;
	return result;
}

int numberSortingDesc(void* a, void* b) {
	int result = 0;
	if (a < b) result = 1;
	else if (a > b) result = -1;
	return result;
}

MyTableClass::MyTableClass()
{
	this->rcl = { };
	this->hWndList = NULL;
	this->selectedCol = 0;
	this->order = Sort::Unsorted;
	thisPtr = this;
}

int x = 0, y = 37;

MyTableClass::MyTableClass(HWND hWndParent)
{
	MyTableClass();
	const auto hInst = (HINSTANCE)GetWindowLongPtr(hWndParent, GWLP_HINSTANCE);
	GetClientRect(hWndParent, &rcl);
	int listWidth = rcl.right - rcl.left;
	int listHeight = rcl.bottom - rcl.top;

	this->hWndList = CreateWindow(WC_LISTVIEW, L"",
		WS_VISIBLE | WS_BORDER | WS_CHILD | LVS_REPORT,
		x, y, listWidth, listHeight,
		hWndParent, (HMENU)IDC_LISTVIEW, hInst, 0);
	ListView_SetExtendedListViewStyle(this->hWndList, LVS_EX_FULLROWSELECT);
	this->selectedCol = 0;
	this->order = Sort::Unsorted;
	thisPtr = this;
	this->SetFont(hWndParent);
}

MyTableClass::~MyTableClass()
{
}

HWND MyTableClass::GetHWND()
{
	return this->hWndList;
}

void MyTableClass::SetFont(HWND hWnd)
{
	int fontSize = 20;
	LOGFONT logFont;
	HFONT hFont;
	memset(&logFont, 0, sizeof(LOGFONT));
	logFont.lfHeight = fontSize;
	wcscpy_s(logFont.lfFaceName, L"Consolas");
	hFont = CreateFontIndirect(&logFont);

	PostMessage(this->hWndList, WM_SETFONT, (WPARAM)hFont, TRUE);
}

void MyTableClass::AddColumn(int columnIndex, wstring value, int width)
{
	const int maxLen = 128;

	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt = LVCFMT_LEFT;
	lvc.cx = width;
	lvc.iSubItem = columnIndex;
	lvc.pszText = const_cast<LPWSTR>(value.c_str());
	lvc.cchTextMax = maxLen;

	ListView_InsertColumn(this->hWndList, columnIndex, &lvc);
}

void MyTableClass::AddRow(int colsCount, int rowIndex, vector<wstring> row)
{
	const int maxLen = 128;

	for (int columnIndex = 0; columnIndex < colsCount; ++columnIndex)
	{
		wstring value = row[columnIndex];
		LVITEM lvi;

		lvi.mask = LVIF_TEXT;
		lvi.iItem = rowIndex;
		lvi.iSubItem = columnIndex;
		lvi.pszText = const_cast<LPWSTR>(value.c_str());
		lvi.cchTextMax = maxLen;

		if (columnIndex > 0) ListView_SetItem(hWndList, &lvi);
		else ListView_InsertItem(hWndList, &lvi);
	}
}

void MyTableClass::SetText(int iCol, int iRow, wstring text)
{
	ListView_SetItemText(this->hWndList, iRow,
		iCol, const_cast<LPWSTR>(text.c_str()));
}

wstring MyTableClass::GetCell(int row, int col)
{
	const int maxLength = 256;
	WCHAR buffer[maxLength] = L"";

	LVITEM lvi;
	lvi.mask = LVIF_TEXT;
	lvi.iItem = row;
	lvi.pszText = buffer;
	lvi.iSubItem = col;
	lvi.cchTextMax = maxLength;
	ListView_GetItem(this->hWndList, &lvi);

	return wstring(buffer);
}

void MyTableClass::Clear()
{
	ListView_DeleteAllItems(this->hWndList);
	HWND hWndHeader = (HWND)::SendMessage(this->hWndList, LVM_GETHEADER, 0, 0);
	int columnCount = (int)::SendMessage(hWndHeader, HDM_GETITEMCOUNT, 0, 0L);
	for (int iCol = columnCount - 1; iCol >= 0; --iCol)
		ListView_DeleteColumn(this->hWndList, iCol);
}

void MyTableClass::FillTable(LPWSTR fileInput)
{
	columns.clear();
	rows.clear();
	this->Clear();
	auto entities = DataAndFiles::ReadFile(fileInput);

	this->columns = entities[0];
	entities.erase(entities.begin());
	this->rows = entities;

	for (int index = 0; index < this->columns.size(); ++index) // вставити стовпці
	{
		wstring textStr = this->columns[index];
		int width = (rcl.right - rcl.left) * 0.1;
		this->AddColumn(index, textStr, width);
	}

	for (int rowIndex = 0; rowIndex < this->rows.size(); ++rowIndex) // вставка рядків
	{
		auto row = this->rows[rowIndex];
		this->AddRow(this->columns.size(), rowIndex, row);
	}
}

void MyTableClass::OnColumnClick(LPARAM lParam)
{
	auto pLVInfo = (LPNMLISTVIEW)lParam;
	const int nSortColumn = pLVInfo->iSubItem;

	if (this->selectedCol != nSortColumn) this->order = Sort::Ascending;
	this->selectedCol = nSortColumn;

	this->HandleSort(lParam); // сортування списку
}

void MyTableClass::OnSize(HWND hWnd)
{
	RECT rc;
	if (this->hWndList)
	{
		GetClientRect(hWnd, &rc);
		MoveWindow(this->hWndList, 0, y, rc.right - rc.left, rc.bottom - rc.top - y, FALSE);
	}
}

void MyTableClass::OnFileOpen(HWND hWnd)
{
	auto fileName = Dialog::ProcessOpenDlg(hWnd);
	if (fileName) this->FillTable(fileName);
}

void MyTableClass::OnFileSave()
{
	auto lines = this->GetEntitiesStrings();
	DataAndFiles::SaveFile(nullptr, lines);
}

void MyTableClass::OnFileSaveAs(HWND hWnd)
{
	auto fileName = Dialog::ProcessSaveAsDlg(hWnd);
	auto lines = this->GetEntitiesStrings();
	if (fileName) DataAndFiles::SaveFile(fileName, lines);
}

void MyTableClass::HandleSort(LPARAM lParam)
{
	auto pLVInfo = (LPNMLISTVIEW)lParam;
	auto lParamSort = 1 + pLVInfo->iSubItem;

	switch (this->order)
	{
	case Sort::Unsorted:
		FillTable(DataAndFiles::fileName);
		this->order = Sort::Ascending;
		break;
	case Sort::Ascending:
		ListView_SortItemsEx(pLVInfo->hdr.hwndFrom, CallBackSortAsc, lParamSort);
		this->order = Sort::Descending;
		break;
	case Sort::Descending:
		ListView_SortItemsEx(pLVInfo->hdr.hwndFrom, CallBackSortDesc, lParamSort);
		this->order = Sort::Unsorted;
		break;
	default:
		string message = "Unknown sorting state: " + to_string((int)this->order);
		throw new exception(message.c_str());
		break;
	}
}

vector<wstring> MyTableClass::GetEntitiesStrings()
{
	HWND hWndHeader = (HWND)::SendMessage(this->hWndList, LVM_GETHEADER, 0, 0);
	int columnsCount = (int)::SendMessage(hWndHeader, HDM_GETITEMCOUNT, 0, 0L);
	int rowsCount = ListView_GetItemCount(this->hWndList);
	wstring delim = L"\t";
	wstring eol = L"\n";

	vector<wstring> strEntities;
	wstring strCols;
	for (int iCol = 0; iCol < columnsCount; ++iCol)
	{
		if (iCol == columns.size() - 1)
		{
			strCols = strCols + columns[iCol] + eol;
			break;
		}
		strCols = strCols + columns[iCol] + delim;
	}
	strEntities.push_back(strCols);

	for (int iRow = 0; iRow < rowsCount; ++iRow)
	{
		wstring outStr;
		for (int iCol = 0; iCol < columnsCount; ++iCol)
		{
			if (iCol == columns.size() - 1)
			{
				outStr = outStr + this->GetCell(iRow, iCol) + eol;
				break;
			}
			outStr = outStr + this->GetCell(iRow, iCol) + delim;
		}
		strEntities.push_back(outStr);
	}
	return strEntities;
}

int MyTableClass::CompareListItemsAsc(LPARAM lParam1, LPARAM lParam2)
{
	wstring strItem1 = this->GetCell((int)lParam1, this->selectedCol);
	wstring strItem2 = this->GetCell((int)lParam2, this->selectedCol);

	double numItem1, numItem2;
	bool isNumberSorting = DataAndFiles::DoubleTryParse(strItem1, &numItem1)
		&& DataAndFiles::DoubleTryParse(strItem2, &numItem2);
	if (isNumberSorting)
	{
		return numberSortingAsc(&numItem1, &numItem2);
	}
	else
	{
		return numberSortingAsc(&strItem1, &strItem2);
	}
}

int MyTableClass::CompareListItemsDesc(LPARAM lParam1, LPARAM lParam2)
{
	wstring strItem1 = this->GetCell((int)lParam1, this->selectedCol);
	wstring strItem2 = this->GetCell((int)lParam2, this->selectedCol);

	double numItem1, numItem2;
	bool isNumberSorting = DataAndFiles::DoubleTryParse(strItem1, &numItem1)
		&& DataAndFiles::DoubleTryParse(strItem2, &numItem2);

	if (isNumberSorting)
	{
		return numberSortingDesc(&numItem1, &numItem2);
	}
	else
	{
		return numberSortingDesc(&strItem1, &strItem2);
	}
}

static int CALLBACK CallBackSortAsc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if (thisPtr) return thisPtr->CompareListItemsAsc(lParam1, lParam2);
}

static int CALLBACK CallBackSortDesc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	if (thisPtr) return thisPtr->CompareListItemsDesc(lParam1, lParam2);
}

#include "DataAndFiles.h"
#include <fstream>


LPWSTR DataAndFiles::fileName = NULL;


vector<vector<wstring>> DataAndFiles::ReadFile(LPWSTR fileInput)
{
	if (fileInput != DataAndFiles::fileName) delete DataAndFiles::fileName;
	DataAndFiles::fileName = fileInput;

	vector<vector<wstring>> entities = { };
	wstring str;
	wifstream file(fileInput);
	while (getline(file, str))
	{
		auto attibutes = Split(str, L"\t");
		entities.push_back(attibutes);
	}

	return entities;
}

void DataAndFiles::SaveFile(LPWSTR fileSave, vector<wstring> lines)
{
	if (!fileSave) fileSave = DataAndFiles::fileName;
	else DataAndFiles::fileName = fileSave;

	wofstream outfile;
	outfile.open(fileSave, ios_base::out);

	for (int i = 0; i < lines.size(); ++i)
		outfile << lines[i];
}

vector<wstring> DataAndFiles::Split(wstring str, wstring delim)
{
	vector<wstring> splittedValues;
	size_t pos = 0;
	wstring token;
	while (pos != wstring::npos)
	{
		pos = str.find(delim);
		token = str.substr(0, pos);
		splittedValues.push_back(token);
		str.erase(0, pos + delim.length());
	}

	return splittedValues;
}

bool DataAndFiles::DoubleTryParse(wstring str, double* out)
{
	try
	{
		*out = stod(str);
		return true;
	}
	catch (const std::exception&)
	{
		return false;
	}
}

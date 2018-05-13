#include "stdafx.h"
#include "util.h"

std::string wstringToString(const std::wstring& wstr)
{
	LPCWSTR pwszSrc = wstr.c_str();
	int nLen = WideCharToMultiByte(CP_UTF8, 0, pwszSrc, -1, NULL, 0, NULL, NULL);
	if (nLen == 0)
		return std::string("");

	char* pszDst = new char[nLen];
	if (!pszDst)
		return std::string("");

	WideCharToMultiByte(CP_UTF8, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	std::string str(pszDst);
	delete[] pszDst;
	pszDst = NULL;

	return str;
}

std::wstring stringToWstring(const std::string& str)
{
	LPCSTR pszSrc = str.c_str();
	int nLen = MultiByteToWideChar(CP_UTF8, 0, pszSrc, -1, NULL, 0);
	if (nLen == 0)
		return std::wstring(L"");

	wchar_t* pwszDst = new wchar_t[nLen];
	if (!pwszDst)
		return std::wstring(L"");

	MultiByteToWideChar(CP_UTF8, 0, pszSrc, -1, pwszDst, nLen);
	std::wstring wstr(pwszDst);
	delete[] pwszDst;
	pwszDst = NULL;

	return wstr;
}

void TcharToChar(const TCHAR * tchar, char * _char)
{
	int iLength;
	iLength = WideCharToMultiByte(CP_UTF8, 0, tchar, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_UTF8, 0, tchar, -1, _char, iLength, NULL, NULL);
}

void CharToTchar(const char * _char, TCHAR * tchar)
{
	int iLength;
	iLength = MultiByteToWideChar(CP_UTF8, 0, _char, strlen(_char) + 1, NULL, 0);
	MultiByteToWideChar(CP_UTF8, 0, _char, strlen(_char) + 1, tchar, iLength);
}


void SplitString(const std::string& str, std::vector<std::string>& vector, const std::string& character)
{
	std::string::size_type pos1, pos2;
	pos2 = str.find(character);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		vector.push_back(str.substr(pos1, pos2 - pos1));

		pos1 = pos2 + character.size();
		pos2 = str.find(character, pos1);
	}
	if (pos1 != str.length())
		vector.push_back(str.substr(pos1));
}

void SplitWString(const std::wstring& str, std::vector<std::wstring>& vector, const std::wstring& character)
{
	std::wstring::size_type pos1, pos2;
	pos2 = str.find(character);
	pos1 = 0;
	while (std::wstring::npos != pos2)
	{
		vector.push_back(str.substr(pos1, pos2 - pos1));

		pos1 = pos2 + character.size();
		pos2 = str.find(character, pos1);
	}
	if (pos1 != str.length())
		vector.push_back(str.substr(pos1));
}


int CalStringWidth(const std::string & str)
{
	std::wstring wstr = STW_U8(str);

	HDC hDC = ::GetDC(NULL);
	HFONT hFont = CreateFont(27, 0, 0, 0, FW_DONTCARE, 0, 0, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, NULL);
	SelectObject(hDC, hFont);
	LPCTSTR  string = wstr.c_str();
	SIZE size = { 0 };
	GetTextExtentPoint32(hDC, string, _tcslen(string), &size);
	RECT rect = { 0 };
	DeleteDC(hDC);
	return size.cx;
}

int CalWstringWidth(const std::wstring & wstr)
{
	HDC hDC = ::GetDC(NULL);
	HFONT hFont = CreateFont(27, 0, 0, 0, FW_DONTCARE, 0, 0, 0,
		ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, NULL);
	SelectObject(hDC, hFont);
	LPCTSTR  string = wstr.c_str();
	SIZE size = { 0 };
	GetTextExtentPoint32(hDC, string, _tcslen(string), &size);
	RECT rect = { 0 };
	//::DrawText(hDC, string, _tcslen(string), &rect, DT_CALCRECT | DT_NOPREFIX | DT_SINGLELINE);
	DeleteDC(hDC);
	//int str_width = std::abs(rect.right - rect.left);
	return size.cx;
}
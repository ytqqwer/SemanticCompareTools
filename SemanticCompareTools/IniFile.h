#pragma once

#include <string>

class IniFile
{
public:
	IniFile(LPCTSTR szFileName);
	~IniFile();

public:
	// Attributes     
	void SetFileName(LPCTSTR szFileName);

	void SetFilePath(LPCTSTR szPath);

	std::wstring GetFilePath();

public:
	// Operations  
	//DWORD GetProfileSectionNames(CStringArray& strArray); // 返回section数量  

	//LPCTSTR ---> LP Const Tchar Str
	BOOL SetProfileString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPCTSTR lpszKeyValue);
	DWORD GetProfileString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPTSTR szKeyValue);
	
	BOOL SetProfileStringWithPath(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPCTSTR lpszKeyValue);
	DWORD GetProfileStringWithPath(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPTSTR szKeyValue);

	//BOOL SetProfileInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, int nKeyValue);
	//int GetProfileInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName);

	BOOL DeleteSection(LPCTSTR lpszSectionName);
	BOOL DeleteKey(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName);

public:
	static IniFile* getInstance();
private:
	static IniFile iniFile;

private:
	std::wstring  m_szFileName; //config.ini, 如果该文件不存在，则exe第一次试图Write时将创建该文件  
	std::wstring  m_szPath;

	UINT m_unMaxSection; // 最多支持的section数(256)  
	UINT m_unSectionNameMaxSize; // section名称长度，这里设为32(Null-terminated)  
};

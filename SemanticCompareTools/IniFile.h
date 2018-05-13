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
	//DWORD GetProfileSectionNames(CStringArray& strArray); // ����section����  

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
	std::wstring  m_szFileName; //config.ini, ������ļ������ڣ���exe��һ����ͼWriteʱ���������ļ�  
	std::wstring  m_szPath;

	UINT m_unMaxSection; // ���֧�ֵ�section��(256)  
	UINT m_unSectionNameMaxSize; // section���Ƴ��ȣ�������Ϊ32(Null-terminated)  
};

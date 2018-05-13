#include "stdafx.h"
#include "IniFile.h"

IniFile IniFile::iniFile(L"config.ini");

IniFile::IniFile(LPCTSTR szFileName)
{
	m_unMaxSection = 512;
	m_unSectionNameMaxSize = 33; // 32位UID串  

	SetFileName(szFileName);
}

IniFile::~IniFile()
{
}

void IniFile::SetFileName(LPCTSTR szFileName)
{
	// 变换为相对路径  
	std::wstring fileName = szFileName;
	fileName = L".//" + fileName;

	m_szFileName = fileName;
}

void IniFile::SetFilePath(LPCTSTR szPath)
{
	m_szPath = szPath;
}

std::wstring IniFile::GetFilePath()
{
	return m_szPath;
}

//DWORD IniFile::GetProfileSectionNames(CStringArray &strArray)
//{
//	int nAllSectionNamesMaxSize = m_unMaxSection * m_unSectionNameMaxSize + 1;
//	char *pszSectionNames = new char[nAllSectionNamesMaxSize];
//	DWORD dwCopied = 0;
//	dwCopied = ::GetPrivateProfileSectionNames(pszSectionNames, nAllSectionNamesMaxSize, m_szFileName);
//
//	strArray.RemoveAll();
//
//	char *pSection = pszSectionNames;
//	do
//	{
//		CString szSection(pSection);
//		if (szSection.GetLength() < 1)
//		{
//			delete[] pszSectionNames;
//			return dwCopied;
//		}
//		strArray.Add(szSection);
//
//		pSection = pSection + szSection.GetLength() + 1; // next section name  
//	} while (pSection && pSection<pszSectionNames + nAllSectionNamesMaxSize);
//
//	delete[] pszSectionNames;
//	return dwCopied;
//}

BOOL IniFile::SetProfileString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPCTSTR lpszKeyValue)
{
	return ::WritePrivateProfileString(lpszSectionName, lpszKeyName, lpszKeyValue, m_szFileName.c_str());
}

DWORD IniFile::GetProfileString(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPTSTR szKeyValue)
{
	DWORD dwCopied = 0;
	dwCopied = ::GetPrivateProfileString(lpszSectionName, lpszKeyName, L"",
		szKeyValue, MAX_PATH, m_szFileName.c_str());
	
	return dwCopied;
}

BOOL IniFile::SetProfileStringWithPath(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPCTSTR lpszKeyValue)
{
	auto path = m_szPath + L"\\";
	std::wstring fullPath = path + lpszSectionName;

	return ::WritePrivateProfileString(fullPath.c_str(), lpszKeyName, lpszKeyValue, m_szFileName.c_str());
}

DWORD IniFile::GetProfileStringWithPath(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, LPTSTR szKeyValue)
{
	auto path = m_szPath + L"\\";
	std::wstring fullPath = path + lpszSectionName;

	DWORD dwCopied = 0;
	dwCopied = ::GetPrivateProfileString(fullPath.c_str(), lpszKeyName, L"",
		szKeyValue, MAX_PATH, m_szFileName.c_str());

	return dwCopied;
}

//BOOL IniFile::SetProfileInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName, int nKeyValue)
//{
//	CString szKeyValue;
//	szKeyValue.Format("%d", nKeyValue);
//
//	return ::WritePrivateProfileString(lpszSectionName, lpszKeyName, szKeyValue, m_szFileName.c_str());
//}

//int IniFile::GetProfileInt(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName)
//{
//	int nKeyValue = ::GetPrivateProfileInt(lpszSectionName, lpszKeyName, 0, m_szFileName.c_str());
//
//	return nKeyValue;
//}


BOOL IniFile::DeleteSection(LPCTSTR lpszSectionName)
{
	return ::WritePrivateProfileSection(lpszSectionName, NULL, m_szFileName.c_str());
}

BOOL IniFile::DeleteKey(LPCTSTR lpszSectionName, LPCTSTR lpszKeyName)
{
	return ::WritePrivateProfileString(lpszSectionName, lpszKeyName, NULL, m_szFileName.c_str());
}

IniFile * IniFile::getInstance()
{
	return &iniFile;
}

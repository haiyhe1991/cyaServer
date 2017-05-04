// win_regedit.cpp

#if defined(WIN32)

#include "win_regedit.h"

#if defined(VS_IS_VC6)
#pragma comment(linker, "/ignore:4089")
#endif

BOOL IsExistRegKey(HKEY hKey, LPCTSTR subkey)
{
	if (NULL == subkey || 0 == subkey[0])
		return false;

	HKEY hSubKey = NULL;
	if (ERROR_SUCCESS == RegOpenKeyEx(hKey, subkey, 0, KEY_QUERY_VALUE, &hSubKey))
	{
		RegCloseKey(hSubKey);
		return true;
	}
	return false;
}

BOOL IsExistRegValue(HKEY hKey, LPCTSTR subkey, LPCTSTR valueName)
{
	if (NULL == subkey || 0 == subkey[0])
		return false;

	HKEY hSubKey = NULL;
	if (ERROR_SUCCESS != RegOpenKeyEx(hKey, subkey, 0, KEY_QUERY_VALUE, &hSubKey))
		return false;

	BYTE buf[1024] = { 0 };
	DWORD bufsize = sizeof(buf);
	LONG lReted = RegQueryValueEx(hSubKey, valueName, NULL, NULL, buf, &bufsize);
	RegCloseKey(hSubKey);
	return lReted == ERROR_SUCCESS;
}

BOOL CreateRegSubKey(HKEY hKey, LPCTSTR subkey)
{
	if (NULL == subkey || 0 == subkey[0])
		return false;

	HKEY hSubKey = NULL;
	DWORD dwDisposition;
	LONG lRet = RegCreateKeyEx(hKey, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hSubKey, &dwDisposition);
	if (ERROR_SUCCESS != lRet)
		return false;
	RegCloseKey(hSubKey);
	return dwDisposition == REG_CREATED_NEW_KEY;//dwDisposition == REG_OPENED_EXISTING_KEY表示此子键已经存在
}

BOOL AddRegKeyValue(HKEY hKey, LPCTSTR subkey, LPCTSTR valueName, LPCTSTR value)
{
	if (NULL == subkey || 0 == subkey[0] || NULL == value || 0 == value[0])
		return false;

	HKEY hSubKey;
	if (ERROR_SUCCESS != RegOpenKeyEx(hKey, subkey, 0, KEY_ALL_ACCESS, &hSubKey))
		return false;

	if (ERROR_SUCCESS != RegSetValueEx(hSubKey, valueName, 0, REG_SZ, (const BYTE*)value, (DWORD)(sizeof(TCHAR) * _tcslen(value))))
	{
		RegCloseKey(hSubKey);
		return false;
	}

	RegCloseKey(hSubKey);
	return true;
}

BOOL GetRegKeyValue(HKEY hKey, LPCTSTR subkey, LPCTSTR valueName, TCHAR* value, DWORD value_type /*= REG_SZ*/)
{
	if (NULL == subkey || 0 == subkey[0])
		return false;

	HKEY hSubKey;
	if (ERROR_SUCCESS != RegOpenKeyEx(hKey, subkey, 0, KEY_READ, &hSubKey))
		return false;

	DWORD dwLen = 1024;
	if (ERROR_SUCCESS != RegQueryValueEx(hSubKey, valueName, 0, &value_type, (LPBYTE)value, &dwLen))
	{
		RegCloseKey(hSubKey);
		return false;
	}

	RegCloseKey(hSubKey);
	return true;
}

BOOL DelRegSubKey(HKEY hKey, LPCTSTR subkey)
{
	if (NULL == subkey || 0 == subkey[0])
		return false;
	return ERROR_SUCCESS == RegDeleteKey(hKey, subkey);
}

BOOL DelRegKeyValue(HKEY hKey, LPCTSTR subkey, LPCTSTR valueName)
{
	if (NULL == subkey || 0 == subkey[0] || NULL == valueName || 0 == valueName[0])
		return false;

	HKEY hSubKey;
	if (ERROR_SUCCESS != RegOpenKeyEx(hKey, subkey, 0, KEY_ALL_ACCESS, &hSubKey))
		return false;

	LONG lRet = RegDeleteValue(hSubKey, valueName);
	RegCloseKey(hSubKey);
	return ERROR_SUCCESS == lRet;
}

TCHAR* GUIDToStr(GUID id, TCHAR sId[40])
{
	_stprintf(sId, _T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
		id.Data1, id.Data2, id.Data3,
		id.Data4[0], id.Data4[1],
		id.Data4[2], id.Data4[3], id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);
	return sId;
}

#endif
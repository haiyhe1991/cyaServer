// win_regedit.h
// Windows注册表操作

#ifndef __CYA_WIN_REGEDIT_H__
#define __CYA_WIN_REGEDIT_H__

#if defined(WIN32)

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#pragma comment(lib, "Advapi32.lib")
#include "cyaTypes.h"

/*
HKEY:
HKEY_CLASSES_ROOT
HKEY_CURRENT_CONFIG
HKEY_CURRENT_USER
HKEY_LOCAL_MACHINE
HKEY_USERS
Windows NT/2000/XP: HKEY_PERFORMANCE_DATA
Windows 95/98/Me: HKEY_DYN_DATA
*/

//查询注册表子键是否存在
//hKey，同RegOpenKeyEx()；subkey，子键名
BOOL IsExistRegKey(HKEY hKey, LPCTSTR subkey);

//查询注册表值是否存在
//valueName为NULL或""时查询该子键的默认值
BOOL IsExistRegValue(HKEY hKey, LPCTSTR subkey, LPCTSTR valueName);

//建立一个注册表子键
BOOL CreateRegSubKey(HKEY hKey, LPCTSTR subkey);

//在注册表子键上添加一个值
//valueName如果为NULL或""则添加默认值
BOOL AddRegKeyValue(HKEY hKey, LPCTSTR subkey, LPCTSTR valueName, LPCTSTR value);

//得到注册表子键的一个值
//valueName如果为NULL或""则得到默认值
//『需要保证缓冲区value足够大』
BOOL GetRegKeyValue(HKEY hKey, LPCTSTR subkey, LPCTSTR valueName, TCHAR* value, DWORD value_type = REG_SZ);

//删除一个子键
BOOL DelRegSubKey(HKEY hKey, LPCTSTR subkey);

//删除一个键值
BOOL DelRegKeyValue(HKEY hKey, LPCTSTR subkey, LPCTSTR valueName);

//生成字符串形式的GUID，返回sId
TCHAR* GUIDToStr(GUID id, TCHAR sId[40]);

#endif	//WIN32

#endif	//_SGS_ROSELIB_WIN_REGEDIT_H_

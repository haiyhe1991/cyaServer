// win_regedit.h
// Windowsע������

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

//��ѯע����Ӽ��Ƿ����
//hKey��ͬRegOpenKeyEx()��subkey���Ӽ���
BOOL IsExistRegKey(HKEY hKey, LPCTSTR subkey);

//��ѯע���ֵ�Ƿ����
//valueNameΪNULL��""ʱ��ѯ���Ӽ���Ĭ��ֵ
BOOL IsExistRegValue(HKEY hKey, LPCTSTR subkey, LPCTSTR valueName);

//����һ��ע����Ӽ�
BOOL CreateRegSubKey(HKEY hKey, LPCTSTR subkey);

//��ע����Ӽ������һ��ֵ
//valueName���ΪNULL��""�����Ĭ��ֵ
BOOL AddRegKeyValue(HKEY hKey, LPCTSTR subkey, LPCTSTR valueName, LPCTSTR value);

//�õ�ע����Ӽ���һ��ֵ
//valueName���ΪNULL��""��õ�Ĭ��ֵ
//����Ҫ��֤������value�㹻��
BOOL GetRegKeyValue(HKEY hKey, LPCTSTR subkey, LPCTSTR valueName, TCHAR* value, DWORD value_type = REG_SZ);

//ɾ��һ���Ӽ�
BOOL DelRegSubKey(HKEY hKey, LPCTSTR subkey);

//ɾ��һ����ֵ
BOOL DelRegKeyValue(HKEY hKey, LPCTSTR subkey, LPCTSTR valueName);

//�����ַ�����ʽ��GUID������sId
TCHAR* GUIDToStr(GUID id, TCHAR sId[40]);

#endif	//WIN32

#endif	//_SGS_ROSELIB_WIN_REGEDIT_H_

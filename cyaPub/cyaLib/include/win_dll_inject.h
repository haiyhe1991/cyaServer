// win_dll_inject.h
// 动态库注入

#ifndef __WIN_DLL_INJECT_H__
#define __WIN_DLL_INJECT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if defined(WIN32)

#include "cyaTypes.h"

// 指定进程id注入
BOOL InjectDLL(DWORD dwProcessId, LPCTSTR pszLibFile);
BOOL EjectDLL(DWORD dwProcessId, LPCTSTR pszLibFile);

// 根据进程名注入
// pszProcessExeName，进程名字『不区分大小写』
// pszLibFile，动态库名
// isFullProcessName，true，pszProcessExeName为进程全名，false，pszProcessExeName为进程相对名
// onlyFirstProcess，是否只注入第一个匹配的进程
BOOL InjectDLL(LPCTSTR pszProcessExeName, LPCTSTR pszLibFile, BOOL isFullProcessName, BOOL onlyFirstProcess);
BOOL EjectDLL(LPCTSTR pszProcessExeName, LPCTSTR pszLibFile, BOOL isFullProcessName, BOOL onlyFirstProcess);

// 带注入动态库功能的创建进程函数
// dwPriorityClass，进程优先级
BOOL CreateProcessWithDLL(LPCTSTR cmdLine, LPCTSTR injectDLL /*= NULL*/, DWORD dwPriorityClass = NORMAL_PRIORITY_CLASS);

#endif	// WIN32

#endif
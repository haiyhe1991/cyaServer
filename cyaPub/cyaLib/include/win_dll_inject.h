// win_dll_inject.h
// ��̬��ע��

#ifndef __WIN_DLL_INJECT_H__
#define __WIN_DLL_INJECT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if defined(WIN32)

#include "cyaTypes.h"

// ָ������idע��
BOOL InjectDLL(DWORD dwProcessId, LPCTSTR pszLibFile);
BOOL EjectDLL(DWORD dwProcessId, LPCTSTR pszLibFile);

// ���ݽ�����ע��
// pszProcessExeName���������֡������ִ�Сд��
// pszLibFile����̬����
// isFullProcessName��true��pszProcessExeNameΪ����ȫ����false��pszProcessExeNameΪ���������
// onlyFirstProcess���Ƿ�ֻע���һ��ƥ��Ľ���
BOOL InjectDLL(LPCTSTR pszProcessExeName, LPCTSTR pszLibFile, BOOL isFullProcessName, BOOL onlyFirstProcess);
BOOL EjectDLL(LPCTSTR pszProcessExeName, LPCTSTR pszLibFile, BOOL isFullProcessName, BOOL onlyFirstProcess);

// ��ע�붯̬�⹦�ܵĴ������̺���
// dwPriorityClass���������ȼ�
BOOL CreateProcessWithDLL(LPCTSTR cmdLine, LPCTSTR injectDLL /*= NULL*/, DWORD dwPriorityClass = NORMAL_PRIORITY_CLASS);

#endif	// WIN32

#endif
#ifndef __CYA_LOG_H__
#define __CYA_LOG_H__

#if defined(_MSC_VER) && (_MSC_VER > 1000)
	#pragma once
#endif

#include <stdarg.h>
#include "cyaTypes.h"

/*
����˵����
��WIN32���̡�
1�������CYALOG_DECL�����ĺ���/����뵽����һ�����ʹ�ã���ʹ��dll��������Ҫ��ʹ�ù����ﶨ��ȫ�ֺ�CYALOG_DECL��
2�������CYALOG_DECL�����ĺ���/��ŵ�ĳ��dll���浼��ʹ�ã�����dll����������Ҫ����ȫ�ֺ�CYALOG_EXPORTS��ʹ��dll�Ĺ��̲���Ҫ�����κκꡣ
��Linux���̡�
�������������Ҫ�����κ�ȫ�ֺ�

����C��������ʹ��CYALOG_API��CYALOG_DECL���Σ�
����C++�����ʹ��CYALOG_DECL����
*/
#if defined(CYALOG_DECL)
	#undef CYALOG_DECL
	#define CYALOG_DECL
#endif

#ifndef CYALOG_DECL
	#ifdef WIN32
		#ifdef CYALOG_EXPORTS
			#define CYALOG_DECL __declspec(dllexport)
		#else
			#define CYALOG_DECL __declspec(dllimport)
		#endif
	#else
		#if defined(__GNUC__) && __GNUC__ >= 4
			#define CYALOG_DECL __attribute__((visibility("default")))
		#else
			#define CYALOG_DECL
		#endif
	#endif
#endif

#ifdef __cplusplus
	#define CYALOG_API extern "C" CYALOG_DECL
#else
	#define CYALOG_API CYALOG_DECL
#endif

// ��־����
#define LOG_INFO				0
#define LOG_WARNING				1
#define LOG_ERROR				2
#define LOG_DEBUG				3

#define LOG_FILE_LINE_INFO		31	// ��ӡԴ�ļ������к�

// ��־���������
#define LOG_ALL_MASK				0xFFFFFFFF
#define LOG_NONE_MASK				0
#define LOG_INFO_MASK				(1 << LOG_INFO)
#define LOG_WARNING_MASK			(1 << LOG_WARNING)
#define LOG_ERROR_MASK				(1 << LOG_ERROR)
#define LOG_DEBUG_MASK				(1 << LOG_DEBUG)


// ������־��Ŀ¼��Ĭ��Ϊ��־ģ��(�����Ƕ�̬�⣬EXE, OCX...)��Ŀ¼�����dirΪ�ձ�ʾ�ָ�Ĭ��ֵ
// ע�⣺��Ҫ�κ�������־��������ǰ���ô˲���
CYALOG_API BOOL CyaLogSetDir(LPCTSTR dir);
CYALOG_API LPCTSTR CyaLogGetDir(); // ʹ��CyaLogGetDir()�ķ���ֵ�ڼ䣬�����ٵ���CyaLogSetDir()�����򷵻ص�ָ�������Ч

// ����ÿn��flushһ���ļ���Ĭ��32�У�����1��
CYALOG_API void CyaLogSetFlushLines(int n);
CYALOG_API int CyaLogGetFlushLines();

// ���õ�����־�ļ���С(�ֽ�)��Ĭ��4M
CYALOG_API void CyaLogSetFileSizeLimit(int bytes);
CYALOG_API int CyaLogGetFileSizeLimit();

// �Ƿ��ӡ����Ļ���أ�Ĭ��true
CYALOG_API void CyaLogSetOutScreen(BOOL onoff);
CYALOG_API BOOL CyaLogIsOutScreen();

// �Ƿ�����д������ļ���Ĭ��true
CYALOG_API void CyaLogSetToFile(BOOL onoff);
CYALOG_API BOOL CyaLogIsToFile();


// ������־�ļ������ڣ�Ĭ��Ϊ7�죬0��ʾ���������������μ�void CyaLogSetReserve(int MBytes)��
CYALOG_API void CyaLogSetLife2(int secs);
CYALOG_API int CyaLogGetLife2();
CYALOG_API void CyaLogSetLife(int days);
CYALOG_API int CyaLogGetLife();

// ������־���⣬logType = LOG_INFO��LOG_WARNING...
CYALOG_API LPCTSTR CyaLogGetEventTitle(int logType);
CYALOG_API void CyaLogSetEventTitle(int logType, LPCTSTR title);

// �õ�����ʹ�õ���־�ļ�����ʧ�ܷ���""
CYALOG_API LPCTSTR CyaLogGetFileName();

//��ӡ��־
CYALOG_API void CyaLogEventX(int logType, LPCTSTR fmt, va_list va);

//  flush ��־�ļ����Ѵ�ӡ��������־����д������
CYALOG_API void CyaLogFlushFile();

// ������־Ŀ¼�ı����ռ䣬�ﵽ�����ռ�󣬽���ɾ����ɵ���־�ļ�(0��ʾ��ֹ�ù���)
// ��λ����(MB)
// �������ͣ� 0 -- Ӳ��  1 -- �ڴ�
// Ĭ��Ϊ1024M����Сֵ64M
CYALOG_API void CyaLogSetReserve(int MBytes, int mediumType = 0);
CYALOG_API int CyaLogGetReserve(int* mediumType = NULL);

// ������־�ļ���������. 0��ʾ��ֹ�ù���
CYALOG_API void CyaLogSetMaxFileNum(int maxFileNum);
CYALOG_API int CyaLogGetMaxFileNum();

// �Ƿ��������������windowsƽ̨��Ч����DEBUG��Ĭ�ϴ򿪣�RELEASE��Ĭ�Ϲر�
CYALOG_API void CyaLogOutputToDebuger(BOOL en);
CYALOG_API BOOL CyaLogIsOutputDebuger();


#define IMPL_LogEvent_(logType)							\
	inline void LogEvent_##logType(const TCHAR* fmt, ...)		\
	{															\
		va_list va;												\
		va_start(va, fmt);										\
		::CyaLogEventX(logType, fmt, va);						\
		va_end(va);												\
	}

IMPL_LogEvent_(LOG_INFO)
IMPL_LogEvent_(LOG_WARNING)
IMPL_LogEvent_(LOG_ERROR)
IMPL_LogEvent_(LOG_DEBUG)

#undef IMPL_LogEvent_

#undef LogEvent
#undef LogPrint
#undef LogInfo
#undef LogWarning
#undef LogError
#undef LogDebug

#define LogEvent(logType, x__)		\
	do	{	LogEvent_##logType x__;		} while(false)

#define LogPrint(logType, x__)		LogEvent(logType, x__)
#define LogInfo(x__)				LogEvent(LOG_INFO, x__)
#define LogWarning(x__)				LogEvent(LOG_WARNING, x__)
#define LogError(x__)				LogEvent(LOG_ERROR, x__)

#define LogDebug(x__)				LogEvent(LOG_DEBUG, x__)
#endif
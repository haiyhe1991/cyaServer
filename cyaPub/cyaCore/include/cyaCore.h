#ifndef __CYA_CORE_H__
#define __CYA_CORE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTaskPool.h"
#include "cyaCoreExports.h"

//�����̳߳ز���
CYACORE_API void CyaCore_SetTaskPoolArgs(int initThreads /*= 1*/,
										int holdThreads /*= 32*/,
										int maxThreads /*= 1024*/,
										int recycleInterval /*= 1000*60*/,
										int startupTimersAtOnce /*= 4*/);

//��ȡȫ���̳߳ض���
CYACORE_API ITTaskPool* CyaCore_FetchTaskPool();

//��ȡȫ�ֶ�ʱ��
CYACORE_API ITTimerPool* CyaCore_FetchTimerPool();

// ����ȫ���̳߳ض���һ����������������ã�
CYACORE_API void CyaCore_DestroyTaskPool();

//�����ڴ�ز���
CYACORE_API void CyaCore_SetSmallMemPoolArg(INT_PTR kBytes);

//�����ڴ�
CYACORE_API void* CyaCore_Alloc(INT_PTR si);

//�ͷ��ڴ�
CYACORE_API void CyaCore_Free(void* p);

//�������
#define RoseCore_New(className) \
	::new(CyaCore_Alloc(sizeof(className))) className()
#define RoseCore_NewEx(className, constructParam) \
	::new(CyaCore_Alloc(sizeof(className))) className constructParam

//��������
template<typename classObj>
inline void CyaCore_Delete(classObj* obj)
{
	if (obj == NULL)	return;	obj->~classObj();	CyaCore_Free(obj);
}


#endif
#ifndef __CYA_CORE_H__
#define __CYA_CORE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTaskPool.h"
#include "cyaCoreExports.h"

//设置线程池参数
CYACORE_API void CyaCore_SetTaskPoolArgs(int initThreads /*= 1*/,
										int holdThreads /*= 32*/,
										int maxThreads /*= 1024*/,
										int recycleInterval /*= 1000*60*/,
										int startupTimersAtOnce /*= 4*/);

//获取全局线程池对象
CYACORE_API ITTaskPool* CyaCore_FetchTaskPool();

//获取全局定时器
CYACORE_API ITTimerPool* CyaCore_FetchTimerPool();

// 销毁全局线程池对象（一般可在主函数最后调用）
CYACORE_API void CyaCore_DestroyTaskPool();

//设置内存池参数
CYACORE_API void CyaCore_SetSmallMemPoolArg(INT_PTR kBytes);

//分配内存
CYACORE_API void* CyaCore_Alloc(INT_PTR si);

//释放内存
CYACORE_API void CyaCore_Free(void* p);

//构造对象
#define RoseCore_New(className) \
	::new(CyaCore_Alloc(sizeof(className))) className()
#define RoseCore_NewEx(className, constructParam) \
	::new(CyaCore_Alloc(sizeof(className))) className constructParam

//析构对象
template<typename classObj>
inline void CyaCore_Delete(classObj* obj)
{
	if (obj == NULL)	return;	obj->~classObj();	CyaCore_Free(obj);
}


#endif
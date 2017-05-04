#ifndef __CYA_MEM_POOL_H__
#define __CYA_MEM_POOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "ssmempool.h"
//小内存池接口
struct ITSmallMemPool
{
	virtual void DeleteThis() = 0;
	/// 获取在内存池中申请的最大size，大于此值的申请将直接调用系统malloc()
	virtual INT_PTR GetMaxAllocInPoolSize() const = 0;
	/// 申请内存，失败返回NULL
	virtual void* Alloc(INT_PTR si) = 0;
	/// 释放内存
	virtual void Free(void* p) = 0;
	///释放所有空闲block
	virtual void FreeAllIdleBlocks() = 0;
	///获取当前pool中分配内存数量(字节)
	virtual INT64 GetTotalSizeInPool() = 0;
	///获取app从当前pool中取走的字节数
	virtual INT64 GetAllocedBytes() = 0;

	virtual ~ITSmallMemPool(){}
};

#define XSMP_THRESHOLD_K_BYTES		64
#define XSMP_REVISED_BYTES			sizeof(INT_PTR)

///@thresholdKBytes-内存池中分配的最大(KB)字节,范围[1, 64]，须是2的整数次幂
///@revisedBytes-内存池中保留使用,比如用来存放分配内存的大小
ITSmallMemPool* MakeTSmallMemPool(INT_PTR thresholdKBytes = XSMP_THRESHOLD_K_BYTES, INT_PTR revisedBytes = XSMP_REVISED_BYTES);

class TSmallMemPoolObj
{
private:
	TSmallMemPoolObj(const TSmallMemPoolObj&);
	TSmallMemPoolObj& operator= (const TSmallMemPoolObj&);

public:
	ITSmallMemPool* /*const*/ m_poolObj;

public:
	explicit TSmallMemPoolObj(INT_PTR thresholdKBytes = XSMP_THRESHOLD_K_BYTES, INT_PTR revisedBytes = XSMP_REVISED_BYTES)
		: m_poolObj(MakeTSmallMemPool(thresholdKBytes, revisedBytes))
	{}
	~TSmallMemPoolObj()
	{
		m_poolObj->DeleteThis();
	}
	ITSmallMemPool* operator->()
	{
		return m_poolObj;
	}
};

#endif
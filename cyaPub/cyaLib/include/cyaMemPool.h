#ifndef __CYA_MEM_POOL_H__
#define __CYA_MEM_POOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "ssmempool.h"
//С�ڴ�ؽӿ�
struct ITSmallMemPool
{
	virtual void DeleteThis() = 0;
	/// ��ȡ���ڴ������������size�����ڴ�ֵ�����뽫ֱ�ӵ���ϵͳmalloc()
	virtual INT_PTR GetMaxAllocInPoolSize() const = 0;
	/// �����ڴ棬ʧ�ܷ���NULL
	virtual void* Alloc(INT_PTR si) = 0;
	/// �ͷ��ڴ�
	virtual void Free(void* p) = 0;
	///�ͷ����п���block
	virtual void FreeAllIdleBlocks() = 0;
	///��ȡ��ǰpool�з����ڴ�����(�ֽ�)
	virtual INT64 GetTotalSizeInPool() = 0;
	///��ȡapp�ӵ�ǰpool��ȡ�ߵ��ֽ���
	virtual INT64 GetAllocedBytes() = 0;

	virtual ~ITSmallMemPool(){}
};

#define XSMP_THRESHOLD_K_BYTES		64
#define XSMP_REVISED_BYTES			sizeof(INT_PTR)

///@thresholdKBytes-�ڴ���з�������(KB)�ֽ�,��Χ[1, 64]������2����������
///@revisedBytes-�ڴ���б���ʹ��,����������ŷ����ڴ�Ĵ�С
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
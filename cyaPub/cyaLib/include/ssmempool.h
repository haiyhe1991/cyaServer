#ifndef __SSMEM_POOL_H__
#define __SSMEM_POOL_H__


#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"
#include "cyaLocker.h"

//�ڴ��
struct SameSizeMemoryBlock
{
	INT_PTR m_size;			//��ɷ����ڴ��ܴ�С
	INT_PTR m_unitCount;	//�鵥Ԫ����
	INT_PTR m_freeCount;	//���ڿ����ڴ浥Ԫ��Ŀ
	INT_PTR m_firstFreeUnitIndex;		//��һ�����õ��ڴ浥Ԫλ������
	SameSizeMemoryBlock* m_next;		//ָ����һ���ڴ��
	char m_data[sizeof(INT_PTR)];		//�ڴ浥Ԫָ��

	void Init(INT_PTR unitCount, INT_PTR unitSize)
	{
		m_size = unitCount * unitSize;
		m_unitCount = unitCount;
		m_freeCount = unitCount - 1;
		m_firstFreeUnitIndex = 1;
		m_next = NULL;

		char* pData = m_data;
		for (INT_PTR i = 1; i < unitCount; ++i)	//��ʼ��1��unitCount-1ָ��
		{
			*(INT_PTR*)pData = i;
			pData += unitSize;
		}
	}

	void UnInit()
	{
		m_size = 0;
		m_unitCount = 0;
		m_freeCount = 0;
		m_firstFreeUnitIndex = -1;
		m_next = NULL;
	}

	SameSizeMemoryBlock()
	{
	}

	~SameSizeMemoryBlock()
	{
	}
};

//�ڴ��
class SameSizeMemoryPool
{
private:
	INT_PTR m_unitSize;			//���ڵ�Ԫ�Ĵ�С
	INT_PTR m_unitCount;		//���ڵ�Ԫ����
	INT_PTR m_increaseUnitCount;		//׷���¿�ʱ,׷�ӵ�Ԫ����
	INT_PTR m_holdCountInPool;	//��pool�п���ڸ�ֵʱ,Free��ʱ���п��п��ʱ���ͷŸÿ�
	INT_PTR m_blockCountInPool;	//pool�п������
	BOOL	m_isThdSecurity;	//�Ƿ�֤�̰߳�ȫ
	INT_PTR m_maxBlockCountInPool;	//��pool������block��
	INT_PTR m_allocedUnitCount;		//�Ѿ�����unit����
	CXTLocker m_locker;
	SameSizeMemoryBlock* m_firstBlock;		//ָ���һ���ڴ��

public:
	//initBlockCount:��ʼ�������
	//unitSize:���ڵ�Ԫ��С
	//unitCount:��һ���ڴ��Ŀɷ��䵥Ԫ��Ŀ
	//increaseUnitCount:�ڶ����ڴ��֮��Ŀ��ڵ�Ԫ��Ŀ
	//maxHoldCount:��pool�п���ڸ�ֵʱ,Free��ʱ���п��п��ʱ���ͷŸÿ�(0��Ч)
	//isThdSecurity:�Ƿ���Ҫ��֤�̰߳�ȫ
	//mallocBlock:����ʱ�Ƿ��ʼ�����п�
	SameSizeMemoryPool(INT_PTR unitSize, INT_PTR unitCount = 512, INT_PTR increaseUnitCount = 256,
		INT_PTR maxHoldCount = 64, INT_PTR maxBlockCount = 1024, BOOL isThdSecurity = true);
	virtual ~SameSizeMemoryPool();

	void* Alloc();		//�����ڴ�
	void Free(void* p);	//�����ڴ�
	void Destroy();		//����
	BOOL IsInMemPool(void* p);	//��p�Ƿ���pool��
	void FreeAllIdleBlocks();	//�ͷ����п��е�block

	INT_PTR GetBlockCountInPool() const;	//��ȡpool�п������
	INT_PTR GetHoldCountInPool() const;		//��ȡpool��ά�ֿ������
	INT_PTR GetBlockUnitSize() const;		//��ȡ���е�Ԫ�Ĵ�С
	INT_PTR GetMaxBlockCountInPool() const;	//��pool����������ɵĿ�����
	INT_PTR GetAllocedItemCount() const;	//��app��pool��һ��ȡ�ߵĵ�Ԫ��
	INT64 GetTotalSizeInPool();				//��ȡpool��ǰ��ϵͳ�з�����ڴ��ֽ���
	INT64 GetAllocedBytes();		//��app��pool��һ��ȡ�ߵ��ڴ��ֽ���
};

template <typename TConstructor>
class SameSizeMemoryPool_Construct : public SameSizeMemoryPool
{
	typedef TConstructor _ConstructorClass;
public:
	SameSizeMemoryPool_Construct(INT_PTR unitSize, INT_PTR unitCount /*= 512*/,
		INT_PTR increaseUnitCount /*= 256*/, INT_PTR maxHoldCount /*= 64*/,
		INT_PTR maxBlockCount /*= 1024*/, BOOL isThdSecurity /*= true*/, const _ConstructorClass& constructor)
		: SameSizeMemoryPool(unitSize, unitCount, increaseUnitCount, maxHoldCount, maxBlockCount, isThdSecurity)
		, m_constructor(constructor)
	{

	}

	void* Alloc()
	{
		void* p = SameSizeMemoryPool::Alloc();
		if (p == NULL)
			return NULL;
		m_constructor((BYTE*)p, true);
		return p;
	}

	void Free(void* p)
	{
		if (NULL == p)
			return;
		m_constructor((BYTE*)p, false);
		SameSizeMemoryPool::Free(p);
	}

private:
	_ConstructorClass const m_constructor;
};

template <typename classT>
class SameSizeMemoryPool_NewDelete : public SameSizeMemoryPool
{
	typedef classT _ClassObj;
public:
	SameSizeMemoryPool_NewDelete(INT_PTR unitSize, INT_PTR unitCount = 512,
		INT_PTR increaseUnitCount = 256, INT_PTR maxHoldCount = 64,
		INT_PTR maxBlockCount = 1024, BOOL isThdSecurity = true)
		: SameSizeMemoryPool(unitSize, unitCount, increaseUnitCount, maxHoldCount, maxBlockCount, isThdSecurity)
	{

	}

	_ClassObj* NewObj()
	{
		void* p = this->Alloc();
		if (p == NULL)
			return NULL;
		return new(p)_ClassObj();
	}

	void DeleteObj(_ClassObj* p)
	{
		if (NULL == p)
			return;
		p->~_ClassObj();
		this->Free(p);
	}

};

template <typename classT>
class CMemPoolObj : public SameSizeMemoryPool
{
	typedef classT _ClassObj;
public:
	CMemPoolObj(INT_PTR unitSize, INT_PTR unitCount = 512,
		INT_PTR increaseUnitCount = 256, INT_PTR maxHoldCount = 64,
		INT_PTR maxBlockCount = 1024, BOOL isThdSecurity = true)
		: SameSizeMemoryPool(unitSize, unitCount, increaseUnitCount, maxHoldCount, maxBlockCount, isThdSecurity)
	{

	}

	_ClassObj* NewObj()
	{
		void* p = this->Alloc();
		if (p == NULL)
			return NULL;
		return new(p)_ClassObj();
	}

	void DeleteObj(_ClassObj* p)
	{
		if (NULL == p)
			return;
		p->~_ClassObj();
		this->Free(p);
	}

	// 	_ClassObj* Alloc()
	// 		{	return (_ClassObj*)this->Alloc();	}
	// 
	// 	void Free(_ClassObj* p)
	// 		{	this->Free(p);	}
};

#endif
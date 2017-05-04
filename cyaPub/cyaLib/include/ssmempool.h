#ifndef __SSMEM_POOL_H__
#define __SSMEM_POOL_H__


#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"
#include "cyaLocker.h"

//内存块
struct SameSizeMemoryBlock
{
	INT_PTR m_size;			//块可分配内存总大小
	INT_PTR m_unitCount;	//块单元个数
	INT_PTR m_freeCount;	//块内空闲内存单元数目
	INT_PTR m_firstFreeUnitIndex;		//第一个可用的内存单元位置索引
	SameSizeMemoryBlock* m_next;		//指向下一个内存块
	char m_data[sizeof(INT_PTR)];		//内存单元指针

	void Init(INT_PTR unitCount, INT_PTR unitSize)
	{
		m_size = unitCount * unitSize;
		m_unitCount = unitCount;
		m_freeCount = unitCount - 1;
		m_firstFreeUnitIndex = 1;
		m_next = NULL;

		char* pData = m_data;
		for (INT_PTR i = 1; i < unitCount; ++i)	//初始化1到unitCount-1指向
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

//内存池
class SameSizeMemoryPool
{
private:
	INT_PTR m_unitSize;			//块内单元的大小
	INT_PTR m_unitCount;		//块内单元个数
	INT_PTR m_increaseUnitCount;		//追加新块时,追加单元个数
	INT_PTR m_holdCountInPool;	//当pool中块大于该值时,Free的时候有空闲块的时候释放该块
	INT_PTR m_blockCountInPool;	//pool中块的总数
	BOOL	m_isThdSecurity;	//是否保证线程安全
	INT_PTR m_maxBlockCountInPool;	//该pool中最大的block数
	INT_PTR m_allocedUnitCount;		//已经分配unit个数
	CXTLocker m_locker;
	SameSizeMemoryBlock* m_firstBlock;		//指向第一个内存块

public:
	//initBlockCount:初始化块个数
	//unitSize:块内单元大小
	//unitCount:第一个内存块的可分配单元数目
	//increaseUnitCount:第二个内存块之后的块内单元数目
	//maxHoldCount:当pool中块大于该值时,Free的时候有空闲块的时候释放该块(0无效)
	//isThdSecurity:是否需要保证线程安全
	//mallocBlock:构造时是否初始化所有块
	SameSizeMemoryPool(INT_PTR unitSize, INT_PTR unitCount = 512, INT_PTR increaseUnitCount = 256,
		INT_PTR maxHoldCount = 64, INT_PTR maxBlockCount = 1024, BOOL isThdSecurity = true);
	virtual ~SameSizeMemoryPool();

	void* Alloc();		//分配内存
	void Free(void* p);	//回收内存
	void Destroy();		//销毁
	BOOL IsInMemPool(void* p);	//该p是否在pool中
	void FreeAllIdleBlocks();	//释放所有空闲的block

	INT_PTR GetBlockCountInPool() const;	//获取pool中块的数量
	INT_PTR GetHoldCountInPool() const;		//获取pool中维持块的数量
	INT_PTR GetBlockUnitSize() const;		//获取块中单元的大小
	INT_PTR GetMaxBlockCountInPool() const;	//获pool中最大能容纳的块数量
	INT_PTR GetAllocedItemCount() const;	//获app从pool中一共取走的单元数
	INT64 GetTotalSizeInPool();				//获取pool当前从系统中分配的内存字节数
	INT64 GetAllocedBytes();		//获app从pool中一共取走的内存字节数
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
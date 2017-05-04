#include "cyaMemPool.h"
#include "cyaMaxMin.h"

static void CalcUnitCountPerBlock(INT_PTR unitSize, INT_PTR maxAllocInPool,
								INT_PTR& initUnitCountPerBlock, INT_PTR& increaseUnitCountPerBlock)
{
	initUnitCountPerBlock = max(512, maxAllocInPool / unitSize * 2);
	increaseUnitCountPerBlock = max(256, initUnitCountPerBlock / 2);
}

class TSmallMemPool : public ITSmallMemPool
{
private:
	TSmallMemPool(const TSmallMemPool&);
	TSmallMemPool& operator = (const TSmallMemPool&);

public:
	explicit TSmallMemPool(INT_PTR thresholdKBytes = XSMP_THRESHOLD_K_BYTES, INT_PTR revisedBytes = XSMP_REVISED_BYTES);
	virtual ~TSmallMemPool();

	virtual void	DeleteThis();
	virtual INT_PTR GetMaxAllocInPoolSize() const;
	virtual void*	Alloc(INT_PTR si);
	virtual void	Free(void* p);
	virtual void	FreeAllIdleBlocks();
	virtual INT64	GetTotalSizeInPool();
	virtual INT64	GetAllocedBytes();

private:
	INT_PTR m_maxAllocInPoolSize;	//>m_macAllocInPoolSize此大小采用malloc策略
	INT_PTR m_maxSameSizePoolCount;	//在SmallMemPool中SameSizeMemoryPool的个数
	SameSizeMemoryPool* m_sameSizeMemPool;
	BOOL m_isValid;
	long m_freeCount;
};

TSmallMemPool::TSmallMemPool(INT_PTR thresholdKBytes /*= XSMP_THRESHOLD_K_BYTES*/, INT_PTR revisedBytes /*= XSMP_REVISED_BYTES*/)
							: m_maxAllocInPoolSize(0)
							, m_maxSameSizePoolCount(0)
							, m_sameSizeMemPool(NULL)
							, m_isValid(false)
							, m_freeCount(0)
{
	if (thresholdKBytes < 1 || thresholdKBytes > 4096 || Align2Power(thresholdKBytes) != thresholdKBytes)
	{
#if !defined(OS_IS_ANDROID)
		ASSERT(false);
#endif
		return;
	}
	if (revisedBytes < 0 || revisedBytes > 512 || 0 != revisedBytes % sizeof(INT_PTR))
	{
#if !defined(OS_IS_ANDROID)
		ASSERT(false);
#endif
		return;
	}
	INT_PTR const BITS = sizeof(thresholdKBytes) * 8;
	INT_PTR x;
	for (x = 0; x<BITS; x++)
	{
		if (thresholdKBytes != (thresholdKBytes << x >> x))
		{
			INT_PTR n = 1 << (BITS - x - 1);
			if (n != thresholdKBytes)
			{
#if !defined(OS_IS_ANDROID)
				ASSERT(false);
#endif
				return;
			}
			thresholdKBytes = n;
			break;
		}
	}
	m_maxAllocInPoolSize = revisedBytes + 1024 * thresholdKBytes;
	m_maxSameSizePoolCount = BITS - x + 7;
	m_sameSizeMemPool = (SameSizeMemoryPool*)::malloc(sizeof(SameSizeMemoryPool) * m_maxSameSizePoolCount);
	if (NULL == m_sameSizeMemPool)
		return;

	// 每个小内存池的元素大小依次为：
	// sizeof(INT_PTR)+revisedBytes+8、sizeof(INT_PTR)+revisedBytes+16、sizeof(INT_PTR)+revisedBytes+32、...
	// 设计原则为大量的小内存要求的尺寸与上面的size尽量符合，以减少内存浪费
	for (INT_PTR i = 0; i<m_maxSameSizePoolCount; i++)
	{
		INT_PTR unitSize = sizeof(INT_PTR) + (revisedBytes + (1 << (3 + i)));
		INT_PTR initCount = 0;
		INT_PTR increaseCount = 0;
		CalcUnitCountPerBlock(unitSize, 1024 * thresholdKBytes, initCount, increaseCount);
		::new(m_sameSizeMemPool + i) SameSizeMemoryPool(unitSize, initCount, increaseCount,
			/*increaseCount*/0, GET_INT_MAX_VALUE(INT_PTR) / m_maxAllocInPoolSize, true);
	}
	m_isValid = true;
}

TSmallMemPool::~TSmallMemPool()
{
	if (m_sameSizeMemPool == NULL)
		return;

	for (INT_PTR i = 0; i < m_maxSameSizePoolCount; ++i)
		m_sameSizeMemPool[i].~SameSizeMemoryPool();

	free(m_sameSizeMemPool);
	m_sameSizeMemPool = NULL;
}

void* TSmallMemPool::Alloc(INT_PTR si)
{
	if (!m_isValid)
		return NULL;
	if (si <= 0)
		return NULL;

	char* p = NULL;
	if (si > m_maxAllocInPoolSize)
	{
		p = (char*)malloc(si + sizeof(INT_PTR));
		if (NULL == p)
			return NULL;
		*(INT_PTR*)p = si;
		p += sizeof(INT_PTR);
	}
	else
	{
		for (INT_PTR i = 0; i < m_maxSameSizePoolCount; ++i)
		{
			INT_PTR unitSize = m_sameSizeMemPool[i].GetBlockUnitSize() - sizeof(INT_PTR);
			if (unitSize >= si)
			{
				p = (char*)m_sameSizeMemPool[i].Alloc();
				if (p != NULL)
				{
					*(INT_PTR*)p = si;
					p += sizeof(INT_PTR);
				}
				break;
			}
		}
	}
	return p;
}

void TSmallMemPool::Free(void* p)
{
	if (NULL == p)
		return;
	if (!m_isValid)
		return;
	char* p1 = (char*)p - sizeof(INT_PTR);
	INT_PTR si = *(INT_PTR*)p1;
	if (si > m_maxAllocInPoolSize)
		free(p1);
	else
	{
		for (INT_PTR i = 0; i < m_maxSameSizePoolCount; ++i)
		{
			INT_PTR unitSize = m_sameSizeMemPool[i].GetBlockUnitSize() - sizeof(INT_PTR);
			if (unitSize >= si)
			{
				m_sameSizeMemPool[i].Free(p1);
				break;
			}
		}

		// 约每20万次普通释放，尝试向系统释放一次
		if (0 == (InterlockedIncrement(&m_freeCount) & 0x0002FFFF))
		{
			INT64 pool_alloced = GetAllocedBytes();
			INT64 total_sys = 0, freed_sys = 0;
			freed_sys = OSGetSysFreeMem(&total_sys);
			// 剩余内存<=总内存的1/2，或内存池分配内存>=总内存的1/8
			if (freed_sys * 2 <= total_sys ||/*&&*/ pool_alloced >= total_sys / 8)
				FreeAllIdleBlocks();
		}
	}
}

void TSmallMemPool::FreeAllIdleBlocks()
{
	if (!m_isValid)
		return;
	for (INT_PTR i = 0; i < m_maxSameSizePoolCount; ++i)
		m_sameSizeMemPool[i].FreeAllIdleBlocks();
}

INT64 TSmallMemPool::GetTotalSizeInPool()
{
	if (!m_isValid)
		return 0;
	INT64 total = 0;
	for (INT_PTR i = 0; i < m_maxSameSizePoolCount; ++i)
		total += m_sameSizeMemPool[i].GetTotalSizeInPool();
	return total;
}

INT64 TSmallMemPool::GetAllocedBytes()
{
	if (!m_isValid)
		return 0;
	INT64 total = 0;
	for (INT_PTR i = 0; i < m_maxSameSizePoolCount; ++i)
		total += m_sameSizeMemPool[i].GetAllocedBytes();
	return total;
}

void TSmallMemPool::DeleteThis()
{
	delete this;
}

INT_PTR TSmallMemPool::GetMaxAllocInPoolSize() const
{
	return m_maxAllocInPoolSize;
}

ITSmallMemPool* MakeTSmallMemPool(INT_PTR thresholdKBytes /*= SMP_THRESHOLD_KBYTES_EX*/, INT_PTR revisedBytes /*= SMP_REVISED_BYTES_EX*/)
{
	return new TSmallMemPool(thresholdKBytes, revisedBytes);
}
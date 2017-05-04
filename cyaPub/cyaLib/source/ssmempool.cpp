#include "ssmempool.h"
#include "cyaMaxMin.h"

#define UNIT_ALIGN_SIZE 4

SameSizeMemoryPool::SameSizeMemoryPool(INT_PTR unitSize, INT_PTR unitCount /*= 512*/, INT_PTR increaseUnitCount /*= 256*/,
	INT_PTR maxHoldCount /*= 64*/, INT_PTR maxBlockCount /*= 1024*/,
	BOOL isThdSecurity /*= false*/)
	: m_firstBlock(NULL)
	, m_unitSize(max(sizeof(INT_PTR), ALIGN_TO(unitSize, UNIT_ALIGN_SIZE)))
	, m_unitCount(unitCount)
	, m_isThdSecurity(isThdSecurity)
	, m_increaseUnitCount(increaseUnitCount)
	, m_holdCountInPool(maxHoldCount)
	, m_blockCountInPool(0)
	, m_maxBlockCountInPool(maxBlockCount)
	, m_allocedUnitCount(0)
{

}

void* SameSizeMemoryPool::Alloc()
{
	void* p = NULL;
	if (m_isThdSecurity)
		m_locker.Lock();

	// 	if(m_blockCountInPool > m_maxBlockCountInPool)
	// 		return NULL;

	if (m_firstBlock == NULL)	//û�п����
	{
		SameSizeMemoryBlock* pFirst = (SameSizeMemoryBlock*)malloc(sizeof(SameSizeMemoryBlock) +
			m_unitCount * m_unitSize -
			sizeof(INT_PTR));
		if (pFirst == NULL)
			goto __end__;
		pFirst->Init(m_unitCount, m_unitSize);

		m_firstBlock = pFirst;
		p = (void*)pFirst->m_data;
		++m_blockCountInPool;
	}
	else
	{
		SameSizeMemoryBlock* pTmpBlock = m_firstBlock;
		while (pTmpBlock && pTmpBlock->m_freeCount == 0) //pTmpBlockû�ߵ�����ҵ�ǰblockû�п��е�Ԫ
		{
			pTmpBlock = pTmpBlock->m_next;	//������
			if (pTmpBlock)
				ASSERT(pTmpBlock->m_freeCount >= 0);
		}

		if (pTmpBlock)	//�п��е�Ԫ��block
		{
			ASSERT(pTmpBlock->m_firstFreeUnitIndex >= 0);
			char* pFree = pTmpBlock->m_data + (pTmpBlock->m_firstFreeUnitIndex * m_unitSize);
			pTmpBlock->m_firstFreeUnitIndex = *(INT_PTR*)pFree;
			if (--pTmpBlock->m_freeCount <= 0)	//���е�Ԫ����Ϊ0,��ÿ������е�Ԫ�������ȥ��
				pTmpBlock->m_firstFreeUnitIndex = -1;
			p = (void*)pFree;
		}
		else	//�޿��е�Ԫ��block�����·���һ����block
		{
			if (m_increaseUnitCount <= 0)
				goto __end__;

			pTmpBlock = (SameSizeMemoryBlock*)malloc(sizeof(SameSizeMemoryBlock) +
				m_increaseUnitCount * m_unitSize
				- sizeof(INT_PTR));
			if (pTmpBlock == NULL)
				goto __end__;
			pTmpBlock->Init(m_increaseUnitCount, m_unitSize);

			ASSERT(pTmpBlock->m_next == NULL);
			pTmpBlock->m_next = m_firstBlock;	//���·����block�ŵ���ǰ��
			m_firstBlock = pTmpBlock;
			p = (void*)pTmpBlock->m_data;
			++m_blockCountInPool;
		}
	}
__end__:
	if (NULL != p)
		++m_allocedUnitCount;
	if (m_isThdSecurity)
		m_locker.Unlock();
	return p;
}

void SameSizeMemoryPool::Free(void* p)
{
	BOOL r = false;
	if (m_isThdSecurity)
		m_locker.Lock();

	SameSizeMemoryBlock* tmp_block = m_firstBlock;
	SameSizeMemoryBlock* tmp_preBlock = m_firstBlock;

	if (m_firstBlock == NULL)
		goto __end__;

	while (p < tmp_block->m_data ||
		p >(tmp_block->m_data + tmp_block->m_size)) //p���ڵ�ǰblock��
	{
		tmp_preBlock = tmp_block;	//ǰһ��block
		tmp_block = tmp_block->m_next;
		if (NULL == tmp_block)
			goto __end__;
	}

	//���е�Ԫ����
	++tmp_block->m_freeCount;
	*(INT_PTR*)p = tmp_block->m_firstFreeUnitIndex;
	tmp_block->m_firstFreeUnitIndex = ((char*)p - tmp_block->m_data) / m_unitSize;

	//�ÿ������е�Ԫ�����ڿ���״̬
	if (m_holdCountInPool > 0 && tmp_block->m_freeCount == tmp_block->m_unitCount && m_blockCountInPool > m_holdCountInPool)
	{
		--m_blockCountInPool;
		tmp_preBlock->m_next = tmp_block->m_next;		//���ոÿ�,�ı�����ָ��
		if (tmp_preBlock == m_firstBlock)
			m_firstBlock = NULL;

		free(tmp_block);
	}
	r = true;

__end__:
	if (r)
		--m_allocedUnitCount;
	if (m_isThdSecurity)
		m_locker.Unlock();
}

SameSizeMemoryPool::~SameSizeMemoryPool(void)
{
	Destroy();
}

void SameSizeMemoryPool::Destroy()
{
	if (m_isThdSecurity)
		m_locker.Lock();
	SameSizeMemoryBlock* p = m_firstBlock;
	while (NULL != p)
	{
		SameSizeMemoryBlock* tmp = p;
		p = p->m_next;
		free(tmp);
	}

	m_firstBlock = NULL;
	if (m_isThdSecurity)
		m_locker.Unlock();
}

BOOL SameSizeMemoryPool::IsInMemPool(void* p)
{
	BOOL ret = true;
	if (NULL == p)
		return false;

	if (m_isThdSecurity)
		m_locker.Lock();

	SameSizeMemoryBlock* tmp_block = m_firstBlock;
	SameSizeMemoryBlock* tmp_preBlock = m_firstBlock;

	if (m_firstBlock == NULL)
	{
		ret = false;
		goto __end__;
	}

	while (p < tmp_block->m_data ||
		p >(tmp_block->m_data + tmp_block->m_size)) //p���ڵ�ǰblock��
	{
		tmp_preBlock = tmp_block;	//ǰһ��block
		tmp_block = tmp_block->m_next;
		if (NULL == tmp_block)
		{
			ret = false;
			goto __end__;
		}
	}

__end__:
	if (m_isThdSecurity)
		m_locker.Unlock();
	return ret;
}

void SameSizeMemoryPool::FreeAllIdleBlocks()
{
	if (m_isThdSecurity)
		m_locker.Lock();

	SameSizeMemoryBlock* pTmpblock = m_firstBlock;
	SameSizeMemoryBlock* pPrevblock = pTmpblock;
	while (pTmpblock != NULL)
	{
		if (pTmpblock->m_freeCount == pTmpblock->m_unitCount)
		{
			if (m_firstBlock == pTmpblock)	//��ͷ���
			{
				m_firstBlock = pTmpblock->m_next;
				SameSizeMemoryBlock* p = pTmpblock;
				pTmpblock = pTmpblock->m_next;
				p->UnInit();
				free(p);
			}
			else
			{
				SameSizeMemoryBlock* p = pTmpblock;
				pPrevblock->m_next = pTmpblock->m_next;
				pTmpblock = pTmpblock->m_next;
				p->UnInit();
				free(p);
			}
			--m_blockCountInPool;
		}
		else
		{
			pPrevblock = pTmpblock;
			pTmpblock = pTmpblock->m_next;
		}
	}
	// 	SameSizeMemoryBlock* tmp_block = m_firstBlock;
	// 	SameSizeMemoryBlock* tmp_prevBlock = m_firstBlock;
	// 	while(tmp_block != NULL /*&& m_blockCountInPool > m_holdCountInPool*/)
	// 	{
	// 		if(tmp_block->m_freeCount == tmp_block->m_unitCount)
	// 		{
	// 			if(tmp_block == m_firstBlock)
	// 			{
	// 				//m_firstBlock = NULL;
	// 				m_firstBlock = tmp_block->m_next;
	// 				//ASSERT(tmp_block->m_next == NULL);
	// 			}
	// 			else
	// 				m_firstBlock->m_next = tmp_block->m_next;
	// 
	// 			SameSizeMemoryBlock* p = tmp_block;
	// 			tmp_prevBlock->m_next = tmp_block->m_next;
	// 			tmp_block = tmp_block->m_next;	//�ı�����ָ��
	// 
	// 			p->UnInit();
	// 			free(p);
	// 			--m_blockCountInPool;
	// 		}
	// 		else
	// 		{
	// 			tmp_prevBlock = tmp_block;
	// 			tmp_block = tmp_block->m_next;
	// 			//ASSERT((DWORD)tmp_block != 0xfeeefeee);
	// 		}
	// 	}

	if (m_isThdSecurity)
		m_locker.Unlock();
}

INT_PTR SameSizeMemoryPool::GetBlockCountInPool() const
{
	return m_blockCountInPool;
}

INT_PTR SameSizeMemoryPool::GetHoldCountInPool() const
{
	return m_holdCountInPool;
}

INT_PTR SameSizeMemoryPool::GetBlockUnitSize() const
{
	return m_unitSize;
}

INT_PTR SameSizeMemoryPool::GetMaxBlockCountInPool() const
{
	return m_maxBlockCountInPool;
}

INT_PTR SameSizeMemoryPool::GetAllocedItemCount() const
{
	return m_allocedUnitCount;
}

INT64 SameSizeMemoryPool::GetTotalSizeInPool()
{
	return (INT64)m_blockCountInPool * m_unitSize;
}

INT64 SameSizeMemoryPool::GetAllocedBytes()
{
	return (INT64)m_allocedUnitCount * m_unitSize;
}
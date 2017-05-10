#ifndef __NO_LOCK_MEMPOOL_H__
#define __NO_LOCK_MEMPOOL_H__

/********************************************************************
说    明:
无锁内存池
适用于一个线程申请，另外一个线程释放的使用场合。
如果有多个线程同时申请或者多个线程同时释放，则不能使用本类。

*********************************************************************/

#include "cyaBase.h"
#include "NoLockList.h"

class CNoLockMempool
{
public:
	class CNoLockMemPoolBlock
	{
	public:
		CNoLockMemPoolBlock()
		{
			Next = NULL;
			pMemPool = NULL;
		}
	public:
		void release(/*int nQueueNum = 0*/)
		{
			ASSERT(pMemPool != NULL);
			if (pMemPool != NULL)
			{
				pMemPool->Free(static_cast<void*>(this)/*, nQueueNum*/);
			}
		}

		int copy(BYTE* pValue, UINT32 nLength, int nOffset = 0)
		{
			if ((int)nLength > (pMemPool->GetChunkSize() - nOffset))
			{
				//DEBUG((LM_WARNING, "CNoLockMemPoolBlock copy failed, data length is too bigger. the length = %d, ChunkSize = %d\n", nLength, pMemPool->GetChunkSize()));
				return -1;
			}
			memcpy(pData + nOffset, pValue, nLength);
			return 0;
		}

		BYTE* getdata()
		{
			return pData;
		}

	public:
		// 对应CListNode的class CListNode * Next;
		CNoLockMemPoolBlock * Next;
		// 数据对应的内存池
		CNoLockMempool * pMemPool;
		// 数据内容
		BYTE pData[0];
	};

public:
	CNoLockMempool(void)
	{
		m_nIncrementCount = 0;
		m_nInitCount = 0;
		m_nChunkSize = 0;
		m_nMaxIncTimes = 0;
		m_nTotalCount = 0;
		m_nMaxTotalCount = 0;
	}

	~CNoLockMempool(void)
	{
		void * pBuf = m_NoLockList.GetLastNode();
		while (pBuf != NULL)
		{
			free(pBuf);
			pBuf = m_NoLockList.GetLastNode();
		}
	}

	int Init(int nChunkSize, int nChunkCount, int nIncrementCount = 0, int nMaxIncTimes = 0/*, int nMaxQueueNum = 1*/)
	{
		void * pBuf = m_NoLockList.GetLastNode();
		while (pBuf != NULL)
		{
			free(pBuf);
			pBuf = m_NoLockList.GetLastNode();
		}

		m_nIncrementCount = 0;
		m_nInitCount = 0;
		m_nChunkSize = 0;
		m_nMaxIncTimes = 0;
		m_nTotalCount = 0;
		m_nMaxTotalCount = 0;

		if (nChunkSize < 1 || nChunkCount <= 1 || nIncrementCount < 0)
		{
			return -1;
		}

		m_nChunkSize = nChunkSize;
		m_nInitCount = nChunkCount;

		m_nIncrementCount = nIncrementCount;
		m_nMaxIncTimes = nMaxIncTimes;
		if ((0 == m_nIncrementCount) || (0 == m_nMaxIncTimes))
		{
			m_nMaxTotalCount = 0;
		}
		else
		{
			m_nMaxTotalCount = m_nMaxIncTimes * m_nIncrementCount + m_nInitCount;
		}

		return 0;
	}

	CNoLockMemPoolBlock* Malloc()
	{
		void * pBuf = m_NoLockList.GetNode();
		if (pBuf)
		{
			return (CNoLockMemPoolBlock*)pBuf;
		}
		else
		{
			if ((0 != m_nMaxTotalCount) && (m_nTotalCount > m_nMaxTotalCount))
			{
				return NULL;
			}

			pBuf = malloc(m_nChunkSize + sizeof(CNoLockMemPoolBlock));
			if (pBuf == NULL)
			{
				return NULL;
			}
			m_nTotalCount++;

			CNoLockMemPoolBlock *mb = new (pBuf)CNoLockMemPoolBlock();
			mb->pMemPool = this;
			return mb;
		}
	}

	/*CNoLockMemPoolBlock * MallocMsgBlock()
	{
	CNoLockMemPoolBlock * mb = NULL;
	void * addr = Malloc();
	if (addr != NULL)
	{
	mb = new (addr) CNoLockMemPoolBlock();
	mb->pMemPool = this;
	}
	return mb;
	}*/

	void Free(void * p/*, int nQueueNum = 0*/)
	{
		m_NoLockList.PutNode(p);
	}

	void FreeMsgBlock(CNoLockMemPoolBlock * mb/*, int nQueueNum = 0*/)
	{
		Free(static_cast<void*>(mb)/*, nQueueNum*/);
	}

	inline int GetFreeCount()
	{
		return m_NoLockList.GetNodeCount();
	}

	inline int GetMaxTotalCount()
	{
		return m_nMaxTotalCount;
	}

	inline int GetTotalCount()
	{
		return m_nTotalCount;
	}

	inline int GetInitCount()
	{
		return m_nInitCount;
	}

	inline int GetIncrementCount()
	{
		return m_nIncrementCount;
	}

	inline int GetChunkSize()
	{
		return m_nChunkSize;
	}

private:
	// 初始申请的大小
	int m_nInitCount;
	// 当初始申请内存池用完后，再次申请时增加的大小
	int m_nIncrementCount;
	// 每个数据块的大小
	int m_nChunkSize;
	// 最大增加多少次
	int m_nMaxIncTimes;
	// 总节点数目
	int m_nTotalCount;
	// 本队列允许的最大节点数目
	int m_nMaxTotalCount;

	CNoLockList m_NoLockList;
};

typedef CNoLockMempool::CNoLockMemPoolBlock CNoLockMsgBlock;

#endif
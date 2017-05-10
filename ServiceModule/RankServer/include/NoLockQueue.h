#ifndef __NO_LOCK_QUEUE_H__
#define __NO_LOCK_QUEUE_H__

/********************************************************************
说    明:
无锁队列的实现头文件
只适用于一个线程写入、一个线程读取的场合，其他场合请不要使用。默认队列中最少维护一个节点。
如果想把队列中的节点都处理完毕，请插入空节点，就是putq(NULL)插入一个空节点即可。

队列中的节点类型为CNoLockMsgBlock
队列类型为：CNoLockQueue
在队列Init方法时，会同时初始化队列所使用的所有内存。
*********************************************************************/

#include "NoLockMempool.h"
#include "cyaRefCount.h"

class CNoLockQueue : public BaseRefCount0
{
public:
	enum enMsgSize
	{
		MSG_SIZE_64 = 0,
		MSG_SIZE_256,
		MSG_SIZE_1024,
		MSG_TYPE_MAX,
	};

	class CNoLockQueueNode
	{
	public:
		CNoLockQueueNode()
		{
			next = NULL;
			mb = NULL;
		}
		class CNoLockQueueNode * next;
		void * mb;
	};

	CNoLockQueue()
	{
		memset(m_MsgPool, 0, sizeof(m_MsgPool));
		m_QueuePool = NULL;
		m_pHead = NULL;
		m_pRear = NULL;
	}

	~CNoLockQueue()
	{
		for (int i = 0; i < MSG_TYPE_MAX; i++)
		{
			if (m_MsgPool[i] != NULL)
			{
				delete m_MsgPool[i];
				m_MsgPool[i] = NULL;
			}
		}

		if (m_QueuePool == NULL)
		{
			delete m_QueuePool;
			m_QueuePool = NULL;
		}
	}

	/********************************************************************
	函 数 名：Init
	功    能：初始化队列和列所使用的内存池
	参数说明：
	(1) int nInitCount 表示队列初始的节点数目
	(2) int nMsgBlockSize 表示队列中每个节点的数据域的长度,就是CNoLockMsgBlock.pData指向的内存的长度
	,如果为0表示未知NoLockMsgBlock.pData指向的内存的大小, 此时只可以使用putq2和getq2
	(3) int nIncrementCount 当最大节点数使用完毕时，允许在增加的节点的数目
	(4) int nMaxIncTimes 标识允许增大的节点的总次数。每次增加nIncrementCount个节点
	返 回 值：
	无
	调用说明：
	nMsgBlockSize如果为大于0，则本方法会初始化数据的内存池;如果nMsgBlockSize==0,则本方法只会初始化队列的内存池
	第三个参数默认为0,第三个有值时，可能导致内存池无限增长，在流量冲击时，可能导致最终申请的内存大小出现不可能
	历史纪录:
	<作者>  <日期>      <版本> <内容>
	*********************************************************************/
	void Init(/*int nMsgBlockSize, */int nInitCount = 1, int nIncrementCount = 0, int nMaxIncTimes = 0)
	{
		//if (nMsgBlockSize > 0)
		{
			m_MsgPool[MSG_SIZE_64] = new CNoLockMempool();
			m_MsgPool[MSG_SIZE_64]->Init(64 + sizeof(CNoLockMsgBlock), nInitCount + 1, nIncrementCount, nMaxIncTimes);
			m_MsgPool[MSG_SIZE_256] = new CNoLockMempool();
			m_MsgPool[MSG_SIZE_256]->Init(256 + sizeof(CNoLockMsgBlock), nInitCount + 1, nIncrementCount, nMaxIncTimes);
			m_MsgPool[MSG_SIZE_1024] = new CNoLockMempool();
			m_MsgPool[MSG_SIZE_1024]->Init(1024 + sizeof(CNoLockMsgBlock), nInitCount + 1, nIncrementCount, nMaxIncTimes);
		}

		m_QueuePool = new CNoLockMempool();
		m_QueuePool->Init(sizeof(CNoLockQueueNode), nInitCount + 1, nIncrementCount, nMaxIncTimes);
		AddFirstNullNode();
	}

	CNoLockMsgBlock * CreateMsgBlock(UINT32 datalen)
	{
		int index = 0;
		if (datalen <= 64)
		{
			index = MSG_SIZE_64;
		}
		else if (datalen <= 256)
		{
			index = MSG_SIZE_256;
		}
		else if (datalen <= 1024)
		{
			index = MSG_SIZE_1024;
		}
		else
		{
			return NULL;
		}

		if (m_MsgPool[index] == NULL)
		{
			return NULL;
		}

		return m_MsgPool[index]->Malloc();
	}

	int putq(CNoLockMsgBlock * mb)
	{
		if (m_QueuePool == NULL)
		{
			return -1;
		}

		void* addr = (void *)m_QueuePool->Malloc();
		if (addr == NULL)
		{
			return -1;
		}

		CNoLockQueueNode * pNode = new (addr)CNoLockQueueNode();
		pNode->mb = mb;
		pNode->next = NULL;
		m_pRear->next = pNode;
		m_pRear = pNode;
		AddRef();

		return 0;
	}

	int getq(CNoLockMsgBlock * &mb)
	{
		if (GetRefCount() > 1)
		{
			ReleaseRef();
			CNoLockQueueNode * pNode = m_pHead;
			m_pHead = m_pHead->next;
			mb = (CNoLockMsgBlock*)pNode->mb;
			m_QueuePool->Free((void*)pNode);
			return 0;
		}
		else
		{
			return -1;
		}
	}

	int getlast(CNoLockMsgBlock * &mb)
	{
		if (GetRefCount() < 1)
		{
			return -1;
		}

		//ReleaseRef();
		CNoLockQueueNode * pNode = m_pHead;
		m_pHead = m_pHead->next;
		mb = (CNoLockMsgBlock*)pNode->mb;
		m_QueuePool->Free((void*)pNode);

		return 0;
	}

	int putq2(void * p)
	{
		void * addr = m_QueuePool->Malloc();
		if (addr == NULL)
		{
			return -1;
		}

		CNoLockQueueNode * pNode = new (addr)CNoLockQueueNode();
		pNode->mb = p;
		pNode->next = NULL;
		m_pRear->next = pNode;
		m_pRear = pNode;
		AddRef();

		return 0;
	}

	int getq2(void * &p)
	{
		if (GetRefCount() > 1)
		{
			ReleaseRef();
			CNoLockQueueNode * pNode = m_pHead;
			m_pHead = m_pHead->next;
			p = pNode->mb;
			m_QueuePool->Free((void*)pNode);
			return 0;
		}
		else
		{
			return -1;
		}
	}

	int GetFreeCount()
	{
		int nFree = m_QueuePool->GetFreeCount();
		return nFree;
	}

	int GetTotalCount()
	{
		return m_QueuePool->GetTotalCount() + m_QueuePool->GetInitCount();
	}

	int GetWatermark()
	{
		return m_QueuePool->GetTotalCount();
	}

private:
	void AddFirstNullNode()
	{
		void * addr = m_QueuePool->Malloc();
		if (addr == NULL)
		{
			return;
		}
		CNoLockQueueNode * pNode = new (addr)CNoLockQueueNode();
		pNode->mb = NULL;
		pNode->next = NULL;
		m_pHead = pNode;
		m_pRear = pNode;
		AddRef();
	}

private:
	// 本队列中每个节点所使用的内存池
	CNoLockMempool * m_MsgPool[MSG_TYPE_MAX];
	// 本队列本身节点所使用的内存池
	CNoLockMempool * m_QueuePool;

	CNoLockQueueNode * m_pHead;
	CNoLockQueueNode * m_pRear;
};


#endif
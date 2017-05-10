#ifndef __NO_LOCK_QUEUE_H__
#define __NO_LOCK_QUEUE_H__

/********************************************************************
˵    ��:
�������е�ʵ��ͷ�ļ�
ֻ������һ���߳�д�롢һ���̶߳�ȡ�ĳ��ϣ����������벻Ҫʹ�á�Ĭ�϶���������ά��һ���ڵ㡣
�����Ѷ����еĽڵ㶼������ϣ������սڵ㣬����putq(NULL)����һ���սڵ㼴�ɡ�

�����еĽڵ�����ΪCNoLockMsgBlock
��������Ϊ��CNoLockQueue
�ڶ���Init����ʱ����ͬʱ��ʼ��������ʹ�õ������ڴ档
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
	�� �� ����Init
	��    �ܣ���ʼ�����к�����ʹ�õ��ڴ��
	����˵����
	(1) int nInitCount ��ʾ���г�ʼ�Ľڵ���Ŀ
	(2) int nMsgBlockSize ��ʾ������ÿ���ڵ��������ĳ���,����CNoLockMsgBlock.pDataָ����ڴ�ĳ���
	,���Ϊ0��ʾδ֪NoLockMsgBlock.pDataָ����ڴ�Ĵ�С, ��ʱֻ����ʹ��putq2��getq2
	(3) int nIncrementCount �����ڵ���ʹ�����ʱ�����������ӵĽڵ����Ŀ
	(4) int nMaxIncTimes ��ʶ��������Ľڵ���ܴ�����ÿ������nIncrementCount���ڵ�
	�� �� ֵ��
	��
	����˵����
	nMsgBlockSize���Ϊ����0���򱾷������ʼ�����ݵ��ڴ��;���nMsgBlockSize==0,�򱾷���ֻ���ʼ�����е��ڴ��
	����������Ĭ��Ϊ0,��������ֵʱ�����ܵ����ڴ���������������������ʱ�����ܵ�������������ڴ��С���ֲ�����
	��ʷ��¼:
	<����>  <����>      <�汾> <����>
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
	// ��������ÿ���ڵ���ʹ�õ��ڴ��
	CNoLockMempool * m_MsgPool[MSG_TYPE_MAX];
	// �����б���ڵ���ʹ�õ��ڴ��
	CNoLockMempool * m_QueuePool;

	CNoLockQueueNode * m_pHead;
	CNoLockQueueNode * m_pRear;
};


#endif
#ifndef __NO_LOCK_LIST_H__
#define __NO_LOCK_LIST_H__

#include "cyaBase.h"

class CNoLockList
{
public:
	// 内存池中空闲队列的节点指针
	class CListNode
	{
	public:
		CListNode()
		{
			Next = NULL;
		}
		class CListNode * Next;
	};


	CNoLockList(void)
	{
		m_pHead = NULL;
		m_pRear = NULL;

		m_uGetCount = 0;
		m_uPutCount = 0;

	}
	~CNoLockList(void)
	{

	}

	// 获取一个空闲节点
	void * GetNode()
	{
		if (NULL == m_pHead)
		{
			return NULL;
		}

		if (m_pHead == m_pRear)
		{
			return NULL;
		}

		m_uGetCount++;
		CListNode *pTemp = m_pHead;
		m_pHead = m_pHead->Next;

		return pTemp;
	}

	// 仅在析构的时候使用,可以获取无锁链表中的最后一个内存块
	void * GetLastNode()
	{
		if (NULL == m_pHead)
		{
			return NULL;
		}

		if (m_pHead == m_pRear)
		{
			m_uGetCount++;
			CListNode *pTemp = m_pHead;

			m_pHead = NULL;
			m_pRear = NULL;

			m_uGetCount = 0;
			m_uPutCount = 0;

			return pTemp;
		}

		m_uGetCount++;
		CListNode *pTemp = m_pHead;
		m_pHead = m_pHead->Next;

		return pTemp;
	}

	// 压入一个空闲节点
	void PutNode(void * pNode)
	{
		m_uPutCount++;
		CListNode * pNewNode = (CListNode*)pNode;
		if (m_pRear == NULL)
		{
			m_pRear = pNewNode;
			m_pHead = pNewNode;
		}
		else
		{
			m_pRear->Next = pNewNode;
			m_pRear = pNewNode;
		}
		pNewNode->Next = NULL;
	}

	void SetName(char chName[])
	{
		strcpy(m_chName, chName);
	}

	UINT64 GetNodeCount()
	{
		return m_uPutCount - m_uGetCount;
	}

	void DebugCount()
	{

	}

private:
	// 指向空闲链表首节点的指针,取出的节点为首节点
	CListNode * m_pHead;
	// 指向空闲链表尾节点的指针,新加入的节点在尾节点
	CListNode * m_pRear;

	UINT64 m_uGetCount;
	UINT64 m_uPutCount;

	char m_chName[30];
};

#endif
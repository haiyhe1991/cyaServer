#ifndef __NO_LOCK_LIST_H__
#define __NO_LOCK_LIST_H__

#include "cyaBase.h"

class CNoLockList
{
public:
	// �ڴ���п��ж��еĽڵ�ָ��
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

	// ��ȡһ�����нڵ�
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

	// ����������ʱ��ʹ��,���Ի�ȡ���������е����һ���ڴ��
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

	// ѹ��һ�����нڵ�
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
	// ָ����������׽ڵ��ָ��,ȡ���Ľڵ�Ϊ�׽ڵ�
	CListNode * m_pHead;
	// ָ���������β�ڵ��ָ��,�¼���Ľڵ���β�ڵ�
	CListNode * m_pRear;

	UINT64 m_uGetCount;
	UINT64 m_uPutCount;

	char m_chName[30];
};

#endif
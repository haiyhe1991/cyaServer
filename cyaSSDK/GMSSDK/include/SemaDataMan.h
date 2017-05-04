#ifndef __SEMA_DATA_MAN_H__
#define __SEMA_DATA_MAN_H__

#include "cyaSync.h"
#include "cyaTcp.h"
#include "cyaRefCount.h"

class CSemaDataMan : public BaseRefCount1
{
public:
	CSemaDataMan()
		:m_data(NULL),
		m_dataLen(0)

	{
		OSSemaInit(&m_sema);
	}

	~CSemaDataMan()
	{
		OSSemaDestroy(&m_sema);
		if (m_data != NULL)
		{
			CyaTcp_Free(m_data);
			m_data = NULL;
		}
	}

	virtual void DeleteObj()
	{
		CyaTcp_Free(this);
	}


	void* DelData()
	{
		return m_data;
	}

	UINT32 GetDataLen()
	{
		return m_dataLen;
	}

	BOOL Wait(DWORD timeout = INFINITE)
	{
		return OSSemaWait(&m_sema, timeout);
	}


	void Post()
	{
		OSSemaPost(&m_sema);
	}


	void InputData(void* data, int dataLen, BOOL isOver)
	{
		if (data == NULL || dataLen == 0)
			return;

		if (m_data == NULL)
		{
			m_data = data;
			m_dataLen = dataLen;
		}
		else
		{
			void* p = m_data;
			UINT32 pLen = m_dataLen;

			int nSize = pLen + dataLen;
			m_data = CyaTcp_Alloc(nSize + 1);
			ASSERT(m_data != NULL);
			ASSERT(p != NULL);
			memcpy(m_data, p, pLen);
			memcpy((BYTE*)m_data + pLen, data, dataLen);

			CyaTcp_Free(p);
			p = NULL;
			pLen = 0;
		}
		if (isOver)
			Post();
	}



private:
	void* m_data;
	UINT32 m_dataLen;
	OSSema m_sema;



};


#endif
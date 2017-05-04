#include "DBSResult.h"
#include "cyaTcp.h"
#include <memory.h>

DBSResult::DBSResult(const void* rstRes, int rstBytes)
			: m_offset(0)
			, m_result((SDBSResultRes*)rstRes)
			, m_totalBytes(rstBytes)
{
	m_result->ntoh();
}

DBSResult::~DBSResult()
{
	if (m_result)
	{
		CyaTcp_Free((void*)m_result);
		m_result = NULL;
	}
	m_offset = 0;
	m_totalBytes = 0;
}

UINT32 DBSResult::NumRows()
{
	if (m_result->fields <= 0)
		return 0;
	return m_result->rows;
}

UINT32 DBSResult::AffectedRows()
{
	return m_result->rows;
}

UINT16 DBSResult::NumFields()
{
	return m_result->fields;
}

DBS_FIELD DBSResult::FetchField(DBS_FIELDLENGTH* pFieldLen)
{
	ASSERT(m_result->fields <= DEFAULT_FIELDS_NUM);
	if (m_totalBytes <= sizeof(SDBSResultRes) - sizeof(char))
		return NULL;
	if (m_result->fields <= 0 || m_result->fields > DEFAULT_FIELDS_NUM)
		return NULL;

#if 0
	char* p = (char*)m_result->data;
	int nBytes = ntohs(*(UINT16*)p);
	p += sizeof(UINT16);

	int idx = 0;
	m_fields[idx] = p;
	int offset = strlen(m_fields[idx]);
	char* pSep = NULL;
	do
	{
		pSep = (char*)memchr(p + offset, '\0', nBytes - offset);
		m_fields[++idx] = pSep + 1;
		offset += strlen(m_fields[idx]) + 1;
	} while (pSep != NULL && nBytes - offset > 0);
	return m_fields;
#else
	char* p = (char*)m_result->data;
	for (UINT16 i = 0; i < m_result->fields; ++i)
	{
		UINT16 nBytes = ntohs(*(UINT16*)p);   //hhy改 原来是int
		ASSERT(nBytes > 0);
		m_fieldsLength[i] = nBytes - 1;
		p += sizeof(UINT16);
		m_fields[i] = p;
		p += nBytes;
	}
	if (pFieldLen)
		*pFieldLen = m_fieldsLength;
	return m_fields;
#endif
}

DBS_ROW DBSResult::FetchRow(DBS_ROWLENGTH* pRowLen)
{
	if (m_result->fields <= 0 || m_result->rows <= 0 || m_result->fields > DEFAULT_FIELDS_NUM)
		return NULL;

	int n = sizeof(SDBSResultRes) - sizeof(char);
	if (m_offset >= m_totalBytes - n)
		return NULL;

	ASSERT(m_result->fields <= DEFAULT_FIELDS_NUM);

#if 0
	char* p = (char*)m_result->data;
	if (m_offset == 0)
	{
		UINT16 nFieldsBytes = ntohs(*(UINT16*)p);
		m_offset += sizeof(UINT16);
		m_offset += nFieldsBytes;
	}
	p += m_offset;

	int oneRowBytes = ntohs(*(UINT16*)p);
	ASSERT(oneRowBytes > 0);
	m_offset += sizeof(UINT16);
	p += sizeof(UINT16);

	int idx = 0;
	m_row[idx] = p;
	int offset = strlen(m_row[idx]);
	char* pSep = NULL;
	do
	{
		pSep = (char*)memchr(p + offset, '\0', oneRowBytes - offset);
		m_row[++idx] = pSep + 1;
		offset += strlen(m_row[idx]) + 1;
	} while (pSep != NULL && oneRowBytes - offset > 0);

	m_offset += oneRowBytes;
	return m_row;
#else
	char* p = (char*)m_result->data;
	if (m_offset == 0)	//跳过字段名
	{
		for (UINT16 i = 0; i < m_result->fields; ++i)
		{
			UINT16 nBytes = ntohs(*(UINT16*)p); //hhy改 原来是int
			p += sizeof(UINT16) + nBytes;
			m_offset += sizeof(UINT16) + nBytes;
		}
	}
	else
		p += m_offset;

	for (UINT16 i = 0; i < m_result->fields && m_totalBytes - n - m_offset > 0; ++i)
	{
		UINT16 nBytes = ntohs(*(UINT16*)p);	//hhy改 原来是int
		m_rowLength[i] = nBytes - 1;
		p += sizeof(UINT16);
		m_row[i] = p;	//(nBytes - 1) <= 0 ? NULL : p;
		p += nBytes;
		m_offset += sizeof(UINT16) + nBytes;
	}
	if (pRowLen)
		*pRowLen = m_rowLength;
	return m_row;
#endif
}

#ifndef __DBS_RESULT_H__
#define __DBS_RESULT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "DBSSDK.h"

#define DEFAULT_FIELDS_NUM 64
class DBSResult
{
private:
	DBSResult(const DBSResult&);
	DBSResult& operator = (const DBSResult&);

public:
	DBSResult(const void* rstRes, int rstBytes);
	~DBSResult();

	UINT32 NumRows();
	UINT32 AffectedRows();
	UINT16 NumFields();
	DBS_FIELD FetchField(DBS_FIELDLENGTH* pFieldLen = NULL);
	DBS_ROW   FetchRow(DBS_ROWLENGTH* pRowLen = NULL);

private:
	SDBSResultRes* m_result;
	unsigned int m_fieldsLength[DEFAULT_FIELDS_NUM];
	unsigned int m_rowLength[DEFAULT_FIELDS_NUM];
	char* m_fields[DEFAULT_FIELDS_NUM];
	char* m_row[DEFAULT_FIELDS_NUM];
	int   m_offset;
	int	  m_totalBytes;
};

#endif
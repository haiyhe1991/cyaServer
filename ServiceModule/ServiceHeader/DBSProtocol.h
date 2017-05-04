#ifndef __DBS_PROTOCOL_H__
#define __DBS_PROTOCOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"
#include "ServiceErrorCode.h"

static const DWORD MAX_PACKET = 4 * 1024;		/// ����4K�����ݷְ�����

#ifndef WIN32
	#define DBS_PRINTF snprintf
#else
	#define DBS_PRINTF _snprintf_s
#endif /// WIN32

/************************************************************************
DBS������ݽṹ
*************************************************************************/
#pragma pack(push, 1)

typedef enum e_dbs_msg_type
{
	DBS_MSG_QUERY = 0,
	DBS_MSG_UPDATE = 1,
	DBS_MSG_INSERT = 2,
	DBS_MSG_DELETE = 3,
	DBS_MSG_HEART = 4,
	DBS_MSG_LOAD = 5,
	DBS_MSG_ULOAD = 6,
	DBS_MSG_GET = 7,
	DBS_MSG_SET = 8,
	DBS_MSG_APPEND = 9,
	DBS_MSG_REMOVE = 10
} DBS_MSG_TYPE;

//ͷ�ļ�;
struct SDBSProtocolHead
{
	UINT16	pktLen : 15;
	UINT16	pktOver : 1;
	UINT16	id;
	UINT32	seq;

	void ntoh()
	{
		UINT16 v = *(UINT16*)this;
		*(UINT16*)this = ntohs(v);//�����ֽ�shortת��Ϊ�����ֽ�short����Ϊ�����ֽ��ǴӸߵ��͵����У�
		id = ntohs(id);
		seq = ntohl(seq);
	}
	void hton()
	{
		UINT16 v = *(UINT16*)this;
		*(UINT16*)this = htons(v);//�����ֽ�shortת��Ϊ�����ֽ�short��
		id = htons(id);
		seq = htonl(seq);
	}
};

//��Ӧ����;
struct SDBSResultRes
{
	UINT16 retCode;		//������
	UINT32 rows;		//��¼����
	UINT16 fields;		//�ֶθ���
	char   data[1];		//����

	void ntoh()
	{
		retCode = ntohs(retCode);
		rows = ntohl(rows);
		fields = ntohs(fields);
	}

	void hton()
	{
		retCode = htons(retCode);
		rows = htonl(rows);
		fields = htons(fields);
	}
};

//��֤Key;
struct SDBSKey
{
	char* pKey;
	UINT16 len;
};

//ֵ;
struct SDBSValue
{
	char* pValue;
	UINT16 len;
};
//keyֵ;
struct SDBSKeyValue
{
	SDBSKey key;
	SDBSValue val;
};

#pragma pack(pop)

#endif
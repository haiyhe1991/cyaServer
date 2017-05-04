#ifndef __DBS_PROTOCOL_H__
#define __DBS_PROTOCOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"
#include "ServiceErrorCode.h"

static const DWORD MAX_PACKET = 4 * 1024;		/// 超过4K的数据分包发送

#ifndef WIN32
	#define DBS_PRINTF snprintf
#else
	#define DBS_PRINTF _snprintf_s
#endif /// WIN32

/************************************************************************
DBS相关数据结构
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

//头文件;
struct SDBSProtocolHead
{
	UINT16	pktLen : 15;
	UINT16	pktOver : 1;
	UINT16	id;
	UINT32	seq;

	void ntoh()
	{
		UINT16 v = *(UINT16*)this;
		*(UINT16*)this = ntohs(v);//网络字节short转化为主机字节short，因为主机字节是从高到低的排列，
		id = ntohs(id);
		seq = ntohl(seq);
	}
	void hton()
	{
		UINT16 v = *(UINT16*)this;
		*(UINT16*)this = htons(v);//主机字节short转化为网络字节short。
		id = htons(id);
		seq = htonl(seq);
	}
};

//响应数据;
struct SDBSResultRes
{
	UINT16 retCode;		//返回码
	UINT32 rows;		//记录行数
	UINT16 fields;		//字段个数
	char   data[1];		//数据

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

//验证Key;
struct SDBSKey
{
	char* pKey;
	UINT16 len;
};

//值;
struct SDBSValue
{
	char* pValue;
	UINT16 len;
};
//key值;
struct SDBSKeyValue
{
	SDBSKey key;
	SDBSValue val;
};

#pragma pack(pop)

#endif
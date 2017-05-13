#ifndef __SERVICE_ROOM_H__
#define __SERVICE_ROOM_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "ServiceCommon.h"

#pragma pack(push, 1)

struct SRandInstInfo
{
	UINT16 transferId;	//当前传送阵编号
	UINT16 instId;		//副本id
	void hton()
	{
		transferId = htons(transferId);
		instId = htons(instId);
	}

	void ntoh()
	{
		transferId = ntohs(transferId);
		instId = ntohs(instId);
	}
};

//随机副本打开,MLS_RAND_INST_OPEN_NOTIFY
struct SRandInstOpenRes
{
	BYTE num;
	SRandInstInfo data[1];
};

//随机副本打开,MLS_RAND_INST_CLOSE_NOTIFY
struct SRandInstCloseRes
{
	SRandInstInfo closeInst;
};

struct SSceneLoginDaemon
{
	UINT32 id;

	void hton()
	{
		id = htonl(id);
	}

	void ntoh()
	{
		id = ntohl(id);
	}
};

#pragma pack(pop)


#endif
#ifndef __GWS_PROTOCOL_H__
#define __GWS_PROTOCOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"
#include "ServiceProtocol.h"
#include "ServiceErrorCode.h"

/**************************************************************************
GWS相关数据结构
***************************************************************************/
#pragma pack(push, 1)

enum EServerEvent	//事件
{
	GWS_CONNECTED,	//服务器连接成功
	GWS_BROKEN		//连接断线
};

enum EServerType
{
	LINK_SERVER_1 = 1,
	LINK_SERVER_2 = 2,
	LINK_SERVER_3 = 3,
	LINK_SERVER_4 = 4,
	MAIN_LOGIC_SERVER = 5,
	ROOM_SERVER_1 = 6,
	ROOM_SERVER_2 = 7,
	CHAT_SERVER = 8,
	OTHER_LOGIC_SERVER = 9,
	MAN_INRERFACE_SERVER = 10,
	CENTER_GW_SERVER = 11,
	DB_SERVER = 12
};

struct SGWSProtocolHead
{
	UINT16 pduLen : 12;	//负载数据长度(不包含头长度)
	UINT16 pktType : 3;	//包类型
	UINT16 encrypt : 1;	//是否加密

	UINT16 sourceId : 6;	//发送源id(服务器编号)
	UINT16 targetId : 6;	//发送目标id(服务器编号)
	UINT16 extField : 4;	//扩展保留

	UINT32 userId;		//用户id
	UINT32 linkerSessId;	//连接上网关服务器上的会话id
	UINT32 pktSeq;		//包序号

	void hton()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;

		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = htons(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);

		*(UINT32*)(pThis + offset) = htonl(*(UINT32*)(pThis + offset));
		offset += sizeof(UINT32);
		*(UINT32*)(pThis + offset) = htonl(*(UINT32*)(pThis + offset));
		offset += sizeof(UINT32);
		*(UINT32*)(pThis + offset) = htonl(*(UINT32*)(pThis + offset));
	}

	void ntoh()
	{
		int offset = 0;
		BYTE* pThis = (BYTE*)this;

		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);
		*(UINT16*)(pThis + offset) = ntohs(*(UINT16*)(pThis + offset));
		offset += sizeof(UINT16);

		*(UINT32*)(pThis + offset) = ntohl(*(UINT32*)(pThis + offset));
		offset += sizeof(UINT32);
		*(UINT32*)(pThis + offset) = ntohl(*(UINT32*)(pThis + offset));
		offset += sizeof(UINT32);
		*(UINT32*)(pThis + offset) = ntohl(*(UINT32*)(pThis + offset));
	}
};

struct SGWSConfirmReq
{
	UINT16 cmdCode;			//指令码
	BYTE serverType;		//服务器类型
	BYTE confirmCode[33];	//验证码

	void hton()
	{
		cmdCode = htons(cmdCode);
	}

	void ntoh()
	{
		cmdCode = ntohs(cmdCode);
	}
};

struct SGWSConfirmRes
{
	UINT16 cmdCode;			//指令码
	UINT16 cmdRet;			//响应结果
	BYTE   confirmRet;		//返回结果：0x00 == 成功， 有服务器编号;0x01 == 验证码错误，无服务器编号
	UINT16 serverCode;		//服务器编号

	void hton()
	{
		cmdCode = htons(cmdCode);
		cmdRet = htons(cmdRet);
		serverCode = htons(serverCode);
	}

	void ntoh()
	{
		cmdCode = ntohs(cmdCode);
		cmdRet = ntohs(cmdRet);
		serverCode = ntohs(serverCode);
	}
};

struct SServerCommonReply	//具体服务器返回数据格式
{
	UINT16 cmdCode;		//命令码
	UINT16 retCode;		//返回值
	char   data[1];		//负载数据

	void hton()
	{
		cmdCode = htons(cmdCode);
		retCode = htons(retCode);
	}

	void ntoh()
	{
		cmdCode = ntohs(cmdCode);
		retCode = ntohs(retCode);
	}
};

#pragma pack(pop)

#define GWS_COMMON_REPLY_LEN (sizeof(SServerCommonReply) - sizeof(char))

//是否是连接服务器
#define IS_LINKER_SERVER(id)			((id) >= 1 && (id) <= 4)

//是否主逻辑服务器
#define IS_MAIN_LOGIC_SERVER(id)		((id) == 5)

//是否房间服务器
#define IS_ROOM_SERVER(id)				((id) == 6 || (id) == 7)

//是否聊天服务器
#define IS_CHAT_SERVER(id)				((id) == 8)

//是否其他逻辑服务器
#define IS_OTHER_LOGIC_SERVER(id)		((id) == 9)

//是否管理接口服务器
#define IS_MAN_INRERFACE_SERVER(id)		((id) == 10)

//是否中心网关服务器
#define IS_CENTER_GW_SERVER(id)			((id) == 11)

//是否数据服务器
#define IS_DB_SERVER(id)				((id) == 12)



//是否合法服务器
#define IS_VALID_SERVER(id)				((id) >= 1 && (id) <= 12)

inline const char* GetServerTypeStr(int id)
{
	if (IS_LINKER_SERVER(id))
		return "LinkServer";
	else if (IS_MAIN_LOGIC_SERVER(id))
		return "MLogicServer";
	else if (IS_ROOM_SERVER(id))
		return "RoomServer";
	else if (IS_CHAT_SERVER(id))
		return "ChatServer";
	else if (IS_OTHER_LOGIC_SERVER(id))
		return "SLogicServer";
	else if (IS_MAN_INRERFACE_SERVER(id))
		return "ExtManServer";
	else if (IS_DB_SERVER(id))
		return "DBServer";
	else
		return "UnknownServer";

}

#endif

#ifndef __SERVICE_PROTOCOL_H__
#define __SERVICE_PROTOCOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if !defined(SERVICE_UNREAL_BUILD)
	#include "cyaTypes.h"
#endif	//SGS_UNREAL_BUILD

#include "ServiceCmdCode.h"
#include "ServiceLinker.h"
#include "ServiceLicence.h"
#include "ServiceChat.h"
#include "ServiceMLogic.h"
#include "ServiceSLogic.h"
#include "ServiceExtMan.h"

#define DEFAULT_TOKEN_KEY	"CYA_DEFAULT_TOKEN_KEY_1234567890"	//默认密钥
#define DEFAULT_KEY_SIZE	32		//密钥长度
#define DEFAULT_TOKEN_SIZE  4		//令牌长度
#define	MAX_BUF_LEN			4*1024

#pragma pack(push, 1)

/*********************************************************************
公共数据结构
**********************************************************************/
enum ESGSPacketType
{
	DATA_PACKET = 0,				//数据包
	DATA_LINKER_GROUP_PACKET,		//linker群发包
	DATA_RELAY_FAILED_PACKET,		//转发数据失败
	DATA_HEARTBEAT_PACKET,			//心跳包
	DATA_PING_PACKET,				//ping (外部协议适用)
	DATA_CONFIRM_PACKET,			//连接验证包(内部通信协议)
	DATA_TOKEN_KEY_PACKET,			//令牌包及密钥
	DATA_CTRL_PACKET,				//控制包(内部使用,外部禁止使用)
};

struct SGSProtocolHead	//公共协议头
{
	UINT16 pduLen : 12;	//数据长度
	UINT16 pktType : 3;	//包类型
	UINT16 isCrypt : 1;	//是否加密

	void hton()
	{
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)pThis = htons(*(UINT16*)pThis);
	}

	void ntoh()
	{
		BYTE* pThis = (BYTE*)this;
		*(UINT16*)pThis = ntohs(*(UINT16*)pThis);
	}
};

struct SGSReqPayload	//请求数据
{
	UINT16 rawDataBytes;	//原始数据(未加密)长度
	UINT32 token;			//令牌
	UINT16 cmdCode;			//命令码
	BYTE data[1];			//数据区

	void hton()
	{
		rawDataBytes = htons(rawDataBytes);
		token = htonl(token);
		cmdCode = htons(cmdCode);
	}

	void ntoh()
	{
		rawDataBytes = ntohs(rawDataBytes);
		token = ntohl(token);
		cmdCode = ntohs(cmdCode);
	}
};

struct SGSResPayload	//响应数据
{
	UINT16 rawDataBytes;	//原始数据(未加密)长度
	UINT32 token;			//令牌
	UINT16 cmdCode;			//命令码
	UINT16 retCode;			//返回码
	BYTE   data[1];			//数据区

	void hton()
	{
		rawDataBytes = htons(rawDataBytes);
		token = htonl(token);
		cmdCode = htons(cmdCode);
		retCode = htons(retCode);
	}

	void ntoh()
	{
		rawDataBytes = ntohs(rawDataBytes);
		token = ntohl(token);
		cmdCode = ntohs(cmdCode);
		retCode = ntohs(retCode);
	}
};

#pragma pack(pop)

#define SGS_PROTO_HEAD_LEN	(sizeof(SGSProtocolHead))				//SGS协议头长
#define SGS_REQ_HEAD_LEN	(sizeof(SGSReqPayload) - sizeof(BYTE))	//SGS请求头长
#define SGS_RES_HEAD_LEN	(sizeof(SGSResPayload) - sizeof(BYTE))	//SGS响应头长
#define SGS_BASIC_REQ_LEN	(SGS_PROTO_HEAD_LEN + SGS_REQ_HEAD_LEN)	//SGS基本请求数据长度
#define SGS_BASIC_RES_LEN	(SGS_PROTO_HEAD_LEN + SGS_RES_HEAD_LEN)	//SGS基本响应数据长度
#define MAX_RES_USER_BYTES	(2048 - SGS_PROTO_HEAD_LEN - SGS_RES_HEAD_LEN - 15)	//客户端最大接收数据长度
#define MAX_RES_INNER_USER_BYTES	(4096 - SGS_PROTO_HEAD_LEN - SGS_RES_HEAD_LEN - 15)	//客户端最大接收数据长度

#define LINK_CHAT_SERVER(cmdCode)	((0x6B6C) <= (cmdCode) && (0x7530) >= (cmdCode))
#define LINK_MLOGIC_SERVER(cmdCode)	((0x2710) <= (cmdCode) && (0x4E20) >= (cmdCode))

#endif
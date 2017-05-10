#ifndef __PUB_FUNCTION_H__
#define __PUB_FUNCTION_H__

#include "cyaTypes.h"

/// 构造协议控制头 
void CreateProtocolPacket(void* pktBuf, bool encrypt, UINT16 pktType, UINT16 payloadLen);
/// 构造没有加密的消息回复数据
void CreateUnEncryptResPayLoad(BYTE* token, void* pktBuf, const void* pktData, UINT16 pktLen, UINT16 cmdCode, UINT16 retCode);
/// 构造加密的消息回复数据
void CreateEncryptResPayLoad(BYTE* token, void* pktBuf, const void* pktData, UINT16 pktLen, UINT16 cmdCode, UINT16 retCode, const BYTE* enKey);

/// 构造没有加密的消息请求数据
void CreateUnEncryptReqPayLoad(UINT32 token, void* pktBuf, const void* pktData, UINT16 pktLen, UINT16 cmdCode);
/// 构造加密的消息请求数据
void CreateEncryptReqPayLoad(UINT32 token, void* pktBuf, const void* pktData, UINT16 pktLen, UINT16 cmdCode, const BYTE* enKey);

/// 获取字节对齐后的数据长度
UINT16 GetAlignedLen(bool encrypt, UINT16 baseLen, UINT16 baseAligned = 16);
/// 比较两个Token是否相等
bool EqualToken(UINT32 dwToken, const BYTE* srcToken, UINT16 srcLen);

/// 构造linker登录验证服务器
void CreateLinkLoginLsReq(void* reqBuf, UINT16 partitionId, UINT32 lsIp, UINT16 lsPort, BYTE linkID, UINT32 linkIp, UINT16 linkPort, const char* token);
/// 构造向验证服务器上报LinkServer连接数
void CreateLinkSessinsReq(void* reqBuf, UINT32 sessions, UINT32 tokens = 0);
/// 构造向验证服务器上报角色变动信息
void CreateRoleChanged(void* reqBuf, UINT32 userId,/* UINT32 roleId,*/ BYTE model);
/// 构造向验证服务器回复生成的Token
void CreateTokenRes(void* reqBuf, UINT32 userId, const char* userToken);


#define RES_HEAD_SIZE		sizeof(SGSResPayload)			/// 带数据的返回头大小
#define REQ_HEAD_SIZE		sizeof(SGSReqPayload)			/// 不带数据的请求头大小

#define LOGINLS_HSIZE		sizeof(SLinkerLoginReq)			/// 登陆验证服务器的头大小
#define ROLECHG_HSIZE		sizeof(SRoleOperReq)			/// 角色创建或删除头大小
#define CONNECT_HSIZE		sizeof(SLinkerReportConnections)/// 连接数报告头大小

#define COMMON_HSIZE		sizeof(SServerCommonReply)		/// 聊天服务器通用数据头大小
#define COM_RAW_SIZE		(COMMON_HSIZE - sizeof(BYTE))

#define CHATMSG_HSIZE		sizeof(SChatMessageRecv)		/// 聊天内容数据头大小
#define CHATMSG_RAW_SIZE	(CHATMSG_HSIZE - sizeof(BYTE))

#endif
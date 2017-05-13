#ifndef __SERVICE_ROOM_DAEMON_H__
#define __SERVICE_ROOM_DAEMON_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/// 错误码定义
#define DAEMON_OK			0		/// OK
#define DAEMON_ERROR		-1		/// 未知错误

///
#define DAEMON_ID_LEN		64
#define PRO_HEAD_SIZE		sizeof(SGSProtocolHead)			/// 控制协议头大小
#define RES_HEAD_SIZE		sizeof(SGSResPayload)			/// 带数据的返回头大小
#define REQ_HEAD_SIZE		sizeof(SGSReqPayload)			/// 不带数据的请求头大小

#define RES_RAW_SIZE		(RES_HEAD_SIZE - sizeof(BYTE))	/// 不带数据的返回头大小
#define REQ_RAW_SIZE		(REQ_HEAD_SIZE - sizeof(BYTE))	/// 不带数据的请求头大小

typedef enum
{
	RoomConnected = 0,
	RoomDisconnected = 1
} ERoomConnectStatus;

typedef struct
{
	unsigned char num;
} RoomUserNum;

#endif ///_SGS_ROOMDAEMON_DEFINED_H_
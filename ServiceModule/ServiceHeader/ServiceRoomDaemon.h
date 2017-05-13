#ifndef __SERVICE_ROOM_DAEMON_H__
#define __SERVICE_ROOM_DAEMON_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/// �����붨��
#define DAEMON_OK			0		/// OK
#define DAEMON_ERROR		-1		/// δ֪����

///
#define DAEMON_ID_LEN		64
#define PRO_HEAD_SIZE		sizeof(SGSProtocolHead)			/// ����Э��ͷ��С
#define RES_HEAD_SIZE		sizeof(SGSResPayload)			/// �����ݵķ���ͷ��С
#define REQ_HEAD_SIZE		sizeof(SGSReqPayload)			/// �������ݵ�����ͷ��С

#define RES_RAW_SIZE		(RES_HEAD_SIZE - sizeof(BYTE))	/// �������ݵķ���ͷ��С
#define REQ_RAW_SIZE		(REQ_HEAD_SIZE - sizeof(BYTE))	/// �������ݵ�����ͷ��С

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
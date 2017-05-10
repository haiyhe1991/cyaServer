#ifndef __PUB_FUNCTION_H__
#define __PUB_FUNCTION_H__

#include "cyaTypes.h"

/// ����Э�����ͷ 
void CreateProtocolPacket(void* pktBuf, bool encrypt, UINT16 pktType, UINT16 payloadLen);
/// ����û�м��ܵ���Ϣ�ظ�����
void CreateUnEncryptResPayLoad(BYTE* token, void* pktBuf, const void* pktData, UINT16 pktLen, UINT16 cmdCode, UINT16 retCode);
/// ������ܵ���Ϣ�ظ�����
void CreateEncryptResPayLoad(BYTE* token, void* pktBuf, const void* pktData, UINT16 pktLen, UINT16 cmdCode, UINT16 retCode, const BYTE* enKey);

/// ����û�м��ܵ���Ϣ��������
void CreateUnEncryptReqPayLoad(UINT32 token, void* pktBuf, const void* pktData, UINT16 pktLen, UINT16 cmdCode);
/// ������ܵ���Ϣ��������
void CreateEncryptReqPayLoad(UINT32 token, void* pktBuf, const void* pktData, UINT16 pktLen, UINT16 cmdCode, const BYTE* enKey);

/// ��ȡ�ֽڶ��������ݳ���
UINT16 GetAlignedLen(bool encrypt, UINT16 baseLen, UINT16 baseAligned = 16);
/// �Ƚ�����Token�Ƿ����
bool EqualToken(UINT32 dwToken, const BYTE* srcToken, UINT16 srcLen);

/// ����linker��¼��֤������
void CreateLinkLoginLsReq(void* reqBuf, UINT16 partitionId, UINT32 lsIp, UINT16 lsPort, BYTE linkID, UINT32 linkIp, UINT16 linkPort, const char* token);
/// ��������֤�������ϱ�LinkServer������
void CreateLinkSessinsReq(void* reqBuf, UINT32 sessions, UINT32 tokens = 0);
/// ��������֤�������ϱ���ɫ�䶯��Ϣ
void CreateRoleChanged(void* reqBuf, UINT32 userId,/* UINT32 roleId,*/ BYTE model);
/// ��������֤�������ظ����ɵ�Token
void CreateTokenRes(void* reqBuf, UINT32 userId, const char* userToken);


#define RES_HEAD_SIZE		sizeof(SGSResPayload)			/// �����ݵķ���ͷ��С
#define REQ_HEAD_SIZE		sizeof(SGSReqPayload)			/// �������ݵ�����ͷ��С

#define LOGINLS_HSIZE		sizeof(SLinkerLoginReq)			/// ��½��֤��������ͷ��С
#define ROLECHG_HSIZE		sizeof(SRoleOperReq)			/// ��ɫ������ɾ��ͷ��С
#define CONNECT_HSIZE		sizeof(SLinkerReportConnections)/// ����������ͷ��С

#define COMMON_HSIZE		sizeof(SServerCommonReply)		/// ���������ͨ������ͷ��С
#define COM_RAW_SIZE		(COMMON_HSIZE - sizeof(BYTE))

#define CHATMSG_HSIZE		sizeof(SChatMessageRecv)		/// ������������ͷ��С
#define CHATMSG_RAW_SIZE	(CHATMSG_HSIZE - sizeof(BYTE))

#endif
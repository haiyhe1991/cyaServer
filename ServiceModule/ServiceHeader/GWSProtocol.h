#ifndef __GWS_PROTOCOL_H__
#define __GWS_PROTOCOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"
#include "ServiceProtocol.h"
#include "ServiceErrorCode.h"

/**************************************************************************
GWS������ݽṹ
***************************************************************************/
#pragma pack(push, 1)

enum EServerEvent	//�¼�
{
	GWS_CONNECTED,	//���������ӳɹ�
	GWS_BROKEN		//���Ӷ���
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
	UINT16 pduLen : 12;	//�������ݳ���(������ͷ����)
	UINT16 pktType : 3;	//������
	UINT16 encrypt : 1;	//�Ƿ����

	UINT16 sourceId : 6;	//����Դid(���������)
	UINT16 targetId : 6;	//����Ŀ��id(���������)
	UINT16 extField : 4;	//��չ����

	UINT32 userId;		//�û�id
	UINT32 linkerSessId;	//���������ط������ϵĻỰid
	UINT32 pktSeq;		//�����

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
	UINT16 cmdCode;			//ָ����
	BYTE serverType;		//����������
	BYTE confirmCode[33];	//��֤��

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
	UINT16 cmdCode;			//ָ����
	UINT16 cmdRet;			//��Ӧ���
	BYTE   confirmRet;		//���ؽ����0x00 == �ɹ��� �з��������;0x01 == ��֤������޷��������
	UINT16 serverCode;		//���������

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

struct SServerCommonReply	//����������������ݸ�ʽ
{
	UINT16 cmdCode;		//������
	UINT16 retCode;		//����ֵ
	char   data[1];		//��������

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

//�Ƿ������ӷ�����
#define IS_LINKER_SERVER(id)			((id) >= 1 && (id) <= 4)

//�Ƿ����߼�������
#define IS_MAIN_LOGIC_SERVER(id)		((id) == 5)

//�Ƿ񷿼������
#define IS_ROOM_SERVER(id)				((id) == 6 || (id) == 7)

//�Ƿ����������
#define IS_CHAT_SERVER(id)				((id) == 8)

//�Ƿ������߼�������
#define IS_OTHER_LOGIC_SERVER(id)		((id) == 9)

//�Ƿ����ӿڷ�����
#define IS_MAN_INRERFACE_SERVER(id)		((id) == 10)

//�Ƿ��������ط�����
#define IS_CENTER_GW_SERVER(id)			((id) == 11)

//�Ƿ����ݷ�����
#define IS_DB_SERVER(id)				((id) == 12)



//�Ƿ�Ϸ�������
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

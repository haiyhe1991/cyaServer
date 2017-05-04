#ifndef __SERVICE_EXT_MAN_H__
#define __SERVICE_EXT_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "ServiceCommon.h"
#include "ServiceManageLogic.h"

#pragma pack(push, 1)

//���������, MANS_USER_LOGIN_CONFIRM
struct SExtManLoginReq
{
	char account[33];
	char pwd[33];
};

struct SExtManLoginRes
{
	UINT32 loginToken;	//��֤��
	char   key[33];		//����key
	void hton()
		{	loginToken = htonl(loginToken);	}
	void ntoh()
		{	loginToken = ntohl(loginToken);	}
};

//��ѯ����˻�����, LCS_QUERY_ACCOUNT_PARTITION
struct SExtManQueryAccountPartitionReq
{
	UINT16 from;	//��ʼλ��
	BYTE   num;		//����
	void hton()
		{	from = htons(from);		}
	void ntoh()
		{	from = ntohs(from);		}
};

struct SAccountPartitionInfo
{
	UINT16 id;
	char   name[33];
	void hton()
		{	id = htons(id);		}
	void ntoh()
		{	id = ntohs(id);		}
};

struct SExtManQueryAccountPartitionRes
{
	UINT16 total;	//�û�������
	BYTE   retNum;	//��ǰ��������
	SAccountPartitionInfo userPartitions[1];
	void hton()
	{
		total = htons(total);
		for(BYTE i = 0; i < retNum; ++i)
			userPartitions[i].hton();
	}
	void ntoh()
	{
		total = ntohs(total);
		for(BYTE i = 0; i < retNum; ++i)
			userPartitions[i].ntoh();
	}
};

//��ѯ����˻��û�����, LICENCE_QUERY_USERS
struct SExtManQueryPlayerAccountReq
{
	UINT16 userPartId;	//�û�����id
	UINT32 from;	//��ʼλ��
	UINT16 num;		//��ѯ����
	void hton()
	{
		userPartId = htonl(userPartId);
		from = htonl(from);
		num = htons(num);
	}
	void ntoh()
	{
		userPartId = ntohl(userPartId);
		from = ntohl(from);
		num = ntohs(num);
	}
};

struct SAccountnfo
{
	UINT32 userId;
	char username[33];
	char ltts[20];
	char regtts[20];
	UINT32 visitNum;

	void hton()
	{
		userId = htonl(userId);
		visitNum = htonl(visitNum);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		visitNum = ntohl(visitNum);
	}
};

struct SExtManQueryPlayerAccountRes
{
	UINT32 total;		//����
	UINT16 retNum;		//��������
	SAccountnfo users[1];	//�û�����
	void hton()
	{
		total = htonl(total);
		retNum = htons(retNum);
		for(UINT16 i = 0; i < retNum; ++i)
			users[i].hton();
	}
	void ntoh()
	{
		total = ntohl(total);
		retNum = ntohs(retNum);
		for(UINT16 i = 0; i < retNum; ++i)
			users[i].ntoh();
	}
};

//��ѯ��Ϸ����, LCS_QUERY_GAME_PARTITION
struct SExtManQueryGamePartitionReq
{
	UINT16 from;
	BYTE   num;

	void hton()
		{	from = htons(from);	}
	void ntoh()
		{	from = ntohs(from);	}
};

struct SGamePartitionInfo
{
	UINT16 partId;	//�������
	char   name[33];	//��������
	UINT32 onlinePlayer;	//������������
	BYTE   status;	//������ǰ״̬
	void hton()
	{
		partId = htons(partId);
		onlinePlayer = htonl(onlinePlayer);
	}
	void ntoh()
	{
		partId = ntohs(partId);
		onlinePlayer = ntohl(onlinePlayer);
	}
};

struct SExtManQueryGamePartitionRes
{
	UINT16 total;
	BYTE   retNum;
	SGamePartitionInfo gameParts[1];
	void hton()
	{
		total = htons(total);
		for(BYTE i = 0; i < retNum; ++i)
			gameParts[i].hton();
	}
	void ntoh()
	{
		total = ntohs(total);
		for(BYTE i = 0; i < retNum; ++i)
			gameParts[i].ntoh();
	}
};

//���ӷ���, LCS_ADD_PARTITION
struct SExtAddPartitionReq
{
	char name[33];	//����
	char lic[33];	//��֤��
	BYTE isRecommend;	//�Ƿ��Ƽ�����
};

struct SExtAddPartitionRes
{
	UINT16 newPartId;	//��id
	void hton()
		{	newPartId = htons(newPartId);	}
	void ntoh()
		{	newPartId = ntohs(newPartId);	}
};

//ɾ������, LCS_DEL_PARTITION
struct SExtDelPartitionReq
{
	UINT16 partitionId;	//����id
	void hton()
		{	partitionId = htons(partitionId);	}
	void ntoh()
		{	partitionId = ntohs(partitionId);	}
};

//�޸ķ�������, LCS_MODIFY_PARTITION
struct SExtModPartitionReq	//����
{
	UINT16 partitionId;	//����id
	char newName[33];	//������
	void hton()
		{	partitionId = htons(partitionId);	}
	void ntoh()
		{	partitionId = ntohs(partitionId);	}
};

///�����Ƽ�����, LCS_SET_RECOMMEND_PARTITION
struct SExtSetRecommendPartitionReq //����
{
	UINT16 partitionId;	//����id
	void hton()
		{	partitionId = htons(partitionId);	}
	void ntoh()
		{	partitionId = ntohs(partitionId);	}
};

//ֹͣ/��������������, LCS_START_LINKER_SERVE/LCS_STOP_LINKER_SERVE
struct SExtCtrlPartitionReq		//����
{
	UINT16 partitionId;	//����id
	void hton()
		{	partitionId = htons(partitionId);	}
	void ntoh()
		{	partitionId = ntohs(partitionId);	}
};

//��ѯ����б�, LGS_QUERY_PLAYER_LIST
struct SExtQueryPlayerListReq
{
	UINT32 from;
	UINT16 num;
	void hton()
	{
		from = htonl(from);
		num = htons(num);
	}
	void ntoh()
	{
		from = ntohl(from);
		num = ntohs(num);
	}
};

struct SPlayerListInfo
{
	UINT32 userId;			//�����˺�id
	UINT32 roleId;			//��ɫid
	char   nick[33];		//�ǳ�
	BYTE   level;			//�ȼ�
	SBigNumber exp;			//����
	SBigNumber gold;		//���
	UINT32 rpcoin;			//����
	char   createtts[20];	//����ʱ��
	char   lvtts[20];		//������ʱ��
	UINT32 vistNum;			//���ʴ���
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		rpcoin = htonl(rpcoin);
		vistNum = htonl(vistNum);
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		rpcoin = ntohl(rpcoin);
		vistNum = ntohl(vistNum);
	}
};

struct SExtQueryPlayerListRes
{
	UINT32 total;		//����
	UINT16 retNum;		//��ǰ��������
	SPlayerListInfo data[1];	//����
	void hton()
	{
		for(UINT16 i = 0; i < retNum; ++i)
			data[i].hton();

		total = htonl(total);
		retNum = htons(retNum);
	}
	void ntoh()
	{
		total = ntohl(total);
		retNum = ntohs(retNum);

		for(UINT16 i = 0; i < retNum; ++i)
			data[i].ntoh();
	}
};

//�����û��ʼ�
struct SExtSendEmailReq
{
	UINT32 userId;
	UINT32 roleId;
	char szCaption[33];	//����
	char szBody[128];		//����
	SBigNumber gold;	//���
	UINT32 rpcoin;	//����
	BYTE   attachmentsNum;	//��������
	SPropertyAttr data[1];
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
		rpcoin = htonl(rpcoin);
		for(BYTE i = 0; i < attachmentsNum; ++i)
			data[i].hton();
	}
	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
		rpcoin = ntohl(rpcoin);
		for(BYTE i = 0; i < attachmentsNum; ++i)
			data[i].ntoh();
	}
};

#pragma pack(pop)

#endif	//_SGSExtMan_h__
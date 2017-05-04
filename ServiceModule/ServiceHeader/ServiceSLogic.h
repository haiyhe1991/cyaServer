#ifndef __SERVICE_S_LOGIC__
#define __SERVICE_S_LOGIC__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if !defined(SERVICE_UNREAL_BUILD)
	#include "cyaTypes.h"
#endif	//SERVICE_UNREAL_BUILD

#pragma pack(push, 1)

//��ѯ�ȼ����а�, SLS_QUERY_LEVEL_RANK
struct SQueryLevelRankReq
{
	BYTE from;	//��ʼλ��;
	BYTE num;	//����;
};

struct SLevelRankInfo	//�ȼ���������
{
	UINT16 rank;			//����
	UINT32 roleId;			//��ɫID
	char   roleNick[33];    //��ɫ�ǳ�
	BYTE   title;			//��ɫְҵ�ƺ�
	UINT16 roleType;		//��ɫ����
	BYTE   roleSex;			//��ɫ�Ա�
	UINT32 cashcount;		//��ֵ�ܽ��
	UINT32 guildId;			//����ID
	char   guildNick[33];   //��������
	BYTE   level;		 	//�ȼ�
	UINT16 trend;    		//��������
	void hton()
	{
		rank = htons(rank);
		roleId = htonl(roleId);
		roleType = htons(roleType);
		cashcount = htonl(cashcount);
		guildId = htonl(guildId);
		trend = htons(trend);
	}
	void ntoh()
	{
		rank = ntohs(rank);
		roleId = ntohl(roleId);
		roleType = ntohs(roleType);
		cashcount = ntohl(cashcount);
		guildId = ntohl(guildId);
		trend = ntohs(trend);
	}
};

struct SQueryLevelRankRes	//��ѯ�ȼ����а񷵻ذ�
{
	BYTE total;		//������Ϣ����;
	BYTE from;		//��ʼλ��;
	BYTE retNum;	//������Ϣ����;
	SLevelRankInfo lvRankInfos[1];	//array<�ȼ���������>    ���ص���Ϣ;
};

// ��ѯս�����а�
struct SQueryCPRankReq	//��ѯս�����а�����
{
	BYTE from;	//��ʼλ��;
	BYTE num;	//����;
};

struct SCPRankInfo	//ս������������
{
	UINT16 rank;			//����
	UINT32 roleId;			//��ɫID
	char   roleNick[33];	//��ɫ�ǳ�
	BYTE   title;			//��ɫְҵ�ƺ�
	BYTE   roleSex;			//��ɫ�Ա�
	UINT16 roleType;		//��ɫ����
	UINT32 cashcount;		//��ֵ�ܽ��
	UINT32 guildId;			//����ID
	char   guildNick[33];   //��������
	UINT32 cp;				//ս����
	UINT16 trend;			//��������
	void hton()
	{
		rank = htons(rank);
		roleId = htonl(roleId);
		roleType = htons(roleType);
		cashcount = htonl(cashcount);
		guildId = htonl(guildId);
		trend = htons(trend);
		cp = htonl(cp);
	}
	void ntoh()
	{
		rank = ntohs(rank);
		roleId = ntohl(roleId);
		roleType = ntohs(roleType);
		cashcount = ntohl(cashcount);
		guildId = ntohl(guildId);
		trend = ntohs(trend);
		cp = ntohl(cp);
	}
};

struct SQueryCPRankRes	//��ѯս�����а񷵻ذ�
{
	BYTE total;		//������Ϣ����;
	BYTE from;		//��ʼλ��;
	BYTE retNum;	//������Ϣ����;
	SCPRankInfo cpRankInfos[1];	//array<ս������������>  ���ص���Ϣ;
};

//�޸ĳɾ�����
struct SModAchievementReq//�ɾ������޸�֪ͨ
{
	UINT16 id;		//�ɾ�ID;
	UINT32 data[4];	//�ɾ��������[4];
	void hton()
	{
		id = htons(id);
		for(int i = 0; i < 4; ++i)
			data[i] = htonl(data[i]);
	}
	void ntoh()
	{
		id = ntohs(id);
		for(int i = 0; i < 4; ++i)
			data[i] = ntohl(data[i]);
	}
};

#pragma pack(pop)

#endif	//_SGSSLogic_h__
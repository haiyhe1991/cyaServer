#ifndef __SERVICE_LICENCE_H__
#define __SERVICE_LICENCE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if !defined(SERVICE_UNREAL_BUILD)
	#include "cyaTypes.h"
#endif	//SERVICE_UNREAL_BUILD

/***********************************************************************************
							��֤������������ݽṹ
***********************************************************************************/

#pragma pack(push, 1)

//�û���½��֤����, LICENCE_USER_LOGIN
struct SUserTokenLoginReq	//����(ģʽ0)
{
	BYTE mode;			//��֤ģʽ0
	char token[33];		//����
	char loginPlat[17];	//��½ƽ̨
	char devid[65];		//�豸��
	char model[17];		//����
	char os[17];	//����ϵͳ
	char ver[17];		//��ǰ���汾
};

struct SUserPswLoginReq	//����(ģʽ1)
{
	BYTE mode;			//��֤ģʽ1
	char username[33];	//�˺�
	char password[33];	//����
	char loginPlat[17];	//��½ƽ̨
	char devid[65];		//�豸��
	char model[17];		//����
	char os[17];		//����ϵͳ
	char ver[17];		//��ǰ���汾
};

struct SUserLoginRes	//��Ӧ
{
	UINT32 userId;		//�û�id
	char username[33];	//�˺���

	void hton()
		{	userId = htonl(userId);	}

	void ntoh()
		{	userId = ntohl(userId);	}
};

//ע���û�, LICENCE_USER_REGIST
struct SUserRegistReq
{
	char username[33];	//�˺�
	char password[33];	//����
	char devid[65];		//�豸��
	char loginPlat[17];	//��½ƽ̨
	char model[17];		//����
	char os[17];		//����ϵͳ
	char ver[17];		//��ǰ���汾
};

//�û���ѯ����, LICENCE_USER_QUERY_PARTITION
struct SUserQueryPartitionReq	//����
{
	UINT16 from;	//��ʼ�ڼ�������
	UINT16 num;		//��ѯ����

	void hton()
	{	
		from = htons(from);
		num = htons(num);	
	}

	void ntoh()
	{	
		from = ntohs(from);
		num = ntohs(num);	
	}
};

struct SPartitionInfo	//������Ϣ
{
	UINT16 partitionId;		//����id
	char   name[33];		//��������
	BYTE   isRecommend;		//�Ƿ��Ƽ�����
	BYTE   status;			//����״̬

	void hton()
		{	partitionId = htons(partitionId);	}

	void ntoh()
		{	partitionId = ntohs(partitionId);	}
};

struct SUserQueryPartitionRes	//��Ӧ
{
	UINT16 totals;			//��������
	UINT16 num;		//��������
	SPartitionInfo data[1];	//���ط�����Ϣ����

	void hton()
	{	
		for(UINT16 i = 0; i < num; ++i)
			data[i].hton();

		num = htons(num);
		totals = htons(totals);	
	}

	void ntoh()
	{	
		totals = ntohs(totals);
		num = ntohs(num);
		for(UINT16 i = 0; i < num; ++i)
			data[i].ntoh();
	}
};

//�û��������, LICENCE_USER_ENTER_PARTITION
struct SUserEnterPartitionReq	//����
{
	UINT16 partitionId;	//����id
	UINT32 userId;		//�˺�id

	void hton()
	{
		partitionId = htons(partitionId);
		userId = htonl(userId);
	}

	void ntoh()
	{	
		partitionId = ntohs(partitionId);
		userId = ntohl(userId);
	}
};

struct SUserEnterPartitionRes	//��Ӧ
{
	UINT16 partitionId;	//����id
	UINT32 userId;		//�˺�id
	char token[33];		//token
	UINT32 linkerIp;	//����link������IP��ַ
	UINT16 linkerPort;	//����link�������˿�

	void hton()
	{
		partitionId = htons(partitionId);
		userId = htonl(userId);
		linkerIp = htonl(linkerIp);
		linkerPort = htons(linkerPort);
	}

	void ntoh()
	{
		partitionId = ntohs(partitionId);
		userId = ntohl(userId);
		linkerIp = ntohl(linkerIp);
		linkerPort = ntohs(linkerPort);
	}
};

//�û���ѯ��½��Ϣ, LICENCE_QUERY_PARTITION_ROLES
struct SUserQueryLoginReq	//����
{
	UINT32 userId;
	void hton()
		{	userId = htonl(userId);	}

	void ntoh()
		{	userId = ntohl(userId);	}
};

struct SUserQueryLoginRes
{
	UINT16 lastLoginPartId;		//���һ�ε�½����id
	char   name[33];			///���һ�ε�½��������
	char   lastLoginDate[20];	//���һ�ε�½��Ȥʱ��
	void hton()
		{	lastLoginPartId = htons(lastLoginPartId);	}

	void ntoh()
		{	lastLoginPartId = ntohs(lastLoginPartId);	}
};

//��ѯ�ѽ���ְҵ
struct SQueryUnlockJobsReq
{
	UINT32 userId;
	void hton()
		{	userId = htonl(userId);	}

	void ntoh()
		{	userId = ntohl(userId);	}
};

struct SQueryUnlockJobsRes
{
	UINT16 num;		//��������
	UINT16 data[1];	//����ְҵ����

	void hton()
	{
		for(UINT16 i = 0; i < num; ++i)
			data[i] = htons(data[i]);
		num = htons(num);
	}

	void ntoh()
	{
		num = ntohs(num);
		for(UINT16 i = 0; i < num; ++i)
			data[i] = ntohs(data[i]);
	}
};

#pragma pack(pop)

#endif	//_SGSLicence_h__
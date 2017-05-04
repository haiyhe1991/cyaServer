#ifndef __LCS_PROTOCOL_H__
#define __LCS_PROTOCOL_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"

#define LCS_SQL_BUF_SIZE 1024

#pragma pack(push, 1)

//linker��Ϣ
struct SLinkerInfo
{
	BYTE linkerId;	//linker id
	UINT32 dwIp;	//linker������ip
	UINT16 port;	//linker�������˿�
	UINT32 conns;	//linker��������ǰ������
	BYTE  status;	//linker��������ǰ״̬
};

//linker֪ͨ��֤�������û���֤ͨ��, LICENCE_LINKER_USER_CHECK
struct SLinkerNotifyUserCheck
{
	UINT32 userId;	//�˺�id
	void hton()
		{	userId = htonl(userId);	}
	void ntoh()
		{	userId = ntohl(userId);	}
};

//��֤������֪ͨlinker�û�����, LICENCE_NOTIFY_USER_OFFLINE
struct SLinkerNotifyUserOffline
{
	UINT32 userId;	//�˺�id
	BYTE   linkerId;	//linker id
	void hton()
		{	userId = htonl(userId);	}
	void ntoh()
		{	userId = ntohl(userId);	}
};

/// Linker��¼��֤������, LICENCE_LINKER_LOGIN
struct SLinkerLoginReq
{
	UINT16 partitionId;	//����id
	BYTE linkerId;	//link���������
	UINT32 linkerIp;	//link������IP
	UINT16 linkerPort;	//link�������˿�
	char   linkerToken[33];	//��֤��
	UINT32 subareIp;	//��������������IP
	UINT16 subarePort;	//��������������˿�

	void hton()
	{
		partitionId = htons(partitionId);
		linkerIp = htonl(linkerIp);
		linkerPort = htons(linkerPort);
		subareIp = htonl(subareIp);
		subarePort = htons(subarePort);
	}

	void ntoh()
	{
		partitionId = ntohs(partitionId);
		linkerIp = ntohl(linkerIp);
		linkerPort = ntohs(linkerPort);
		subareIp = ntohl(subareIp);
		subarePort = ntohs(subarePort);
	}
};

//�ϱ�linker��ǰ������, LICENCE_LINKER_REPORT_CONNECTIONS
struct SLinkerReportConnections	
{
	UINT32 connections;	//link��������ǰ������
	UINT32 tokens;		//token��

	void hton()
	{
		connections = htonl(connections);
		tokens = htonl(tokens);
	}

	void ntoh()
	{
		connections = ntohl(connections);
		tokens = ntohl(tokens);
	}
};

//��ȡ�ϱ�linker������ʱ����(��λ:��), LICENCE_GET_REPORT_TIMER
struct SLinkerGetReportTimerRes
{
	UINT32 timerSec;	//��
	void hton()
		{	timerSec = htonl(timerSec);		}

	void ntoh()
		{	timerSec = ntohl(timerSec);		}
};


//��ѯ����״̬, LICENCE_LINKER_QUERY_PARTITION_STATUS
#define PARTITION_STATUS_OK		0	//����
#define PARTITION_STATUS_MAINTAIN	1	//ά��
struct SPartitionStatusQueryRes
{
	BYTE status;	//0-����,1-ά��
};

//��linker�������л�ȡ�û�token����, LICENCE_LINKER_GET_TOKEN
struct SLinkerGetUserTokenReq
{
	UINT32 userId;		//�˺�id
	char username[33];	//�˺���

	void hton()
		{	userId = htonl(userId);	}

	void ntoh()
		{	userId = ntohl(userId);	}
};

struct SLinkerGetUserTokenRes
{
	UINT32 userId;		//�˺�id
	char token[33];		//token

	void hton()
		{	userId = htonl(userId);	}

	void ntoh()
		{	userId = ntohl(userId);	}
};

//��ѯ�ѽ���ְҵ
struct SLinkerQueryUnlockJobsReq
{
	UINT32 userId;
	UINT32 roleId;
	BYTE type;
	void hton()
	{
		userId = htonl(userId);
		roleId = htonl(roleId);
	}

	void ntoh()
	{
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

struct SLinkerQueryUnlockJobsRes
{
	UINT32 userId;
	UINT32 roleId;
	UINT16 num;		//��������
	UINT16 data[1];	//����ְҵ����

	void hton()
	{	
		for(UINT16 i = 0; i < num; ++i)
			data[i]= htons(data[i]);
		num = htons(num);
		userId = htonl(userId);
		roleId = htonl(roleId);	
	}

	void ntoh()
	{
		num = ntohs(num);
		for(UINT16 i = 0; i < num; ++i)
			data[i]= ntohs(data[i]);
		userId = ntohl(userId);
		roleId = ntohl(roleId);
	}
};

#define REPORT_ROLE_CREATE	0 /// ��ɫ����
#define REPORT_ROLE_DELETE	1 /// ��ɫɾ��
/// LinkS����֤�������㱨��ɫ��������ɾ��
/// �ͻ��������ɫ��������ɾ��
/// ������ɫ�������Ľ�ɫ��������ɾ��, LICENCE_LINKER_ROLE_CHANGE
struct SRoleOperReq	
{
	UINT32 userId;		/// �˻�ID
	//UINT32 roleId;		/// ��ɫID
	BYTE model;			/// ģʽ 0:����,1:ɾ��

	void hton()
	{	
		userId = htonl(userId);
		//roleId = htonl(roleId);
	}

	void ntoh()
	{	
		userId = ntohl(userId);
		//roleId = ntohl(roleId);
	}
};

#pragma pack(pop)

#endif	//_LCSProtocol_h__
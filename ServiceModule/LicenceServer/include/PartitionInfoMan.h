#ifndef __PARTITION_INFO_MAN_H__
#define __PARTITION_INFO_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
����������
*/

#include <map>
#include "cyaTypes.h"
#include "cyaLocker.h"
#include "cyaRefCount.h"
#include "cyaCoreTimer.h"
#include "ServiceExtMan.h"
#include "ServiceLicence.h"

class LicenceSession;
class PartitionInfoMan
{
private:
	class PartitionSite : public BaseRefCount1	//������Ϣ
	{
		struct LinkerInfo	//���߷���link��������Ϣ
		{
			BYTE status;		//linker״̬(0-����,1-ά��)
			UINT32 linkerIp;	//link������IP
			UINT16 linkerPort;	//link�������˿�
			UINT32 connections;	//linker��������ǰ������
			UINT32 tokens;		//token��
			LicenceSession* session; //��½����֤��������linker����

			LinkerInfo()
			{
				status = 0;
				linkerIp = 0;
				linkerPort = 0;
				connections = 0;
				tokens = 0;
				session = NULL;
			}
		};

	public:
		PartitionSite();
		~PartitionSite();

		int LinkerLogin(BYTE linkerId, LicenceSession* session, UINT32 linkerIp, UINT16 linkerPort);
		int LinkerLogout(BYTE linkerId);
		int UpdateLinkerConnections(BYTE linkerId, UINT32 connections, UINT32 tokens);

		int StartAllLinkerServe();
		int StopAllLinkerServe();

		int GetMinPayloadLinker(BYTE& linkerId);
		int EnterLinkerGetToken(BYTE linkerId, UINT32 userId, const char* username);
		int GetLinkerAddr(BYTE linkerId, UINT32& linkerIp, UINT16& linkerPort);
		int NotifyUserOffline(BYTE exceptLinkerId, UINT32 userId);
		int QueryUnlockJobs(UINT32 userId);

		void CheckHeartbeat();
		void DelAllLinkers();

		BYTE GetStatus() const
		{
			return m_status;
		}

		const char* GetName() const
		{
			return m_name.c_str();
		}

		BOOL IsRecommend() const
		{
			return m_isRecommend;
		}

		void SetName(const char* name)
		{
			m_name = name;
		}

		void SetStatus(BYTE status)
		{
			m_status = status;
		}

		void SetRecommendFlag(BOOL isRecommend)
		{
			m_isRecommend = isRecommend;
		}

		UINT32 GetRegistAccountNum() const
		{
			return m_accountNum;
		}

		void SetRegistAccountNum(UINT32 num)
		{
			m_accountNum = num;
		}

		BOOL HasLinkerLogin()
		{
			return m_linkersMap.empty() ? false : true;
		}

	private:
		std::string m_name;			//��������
		BYTE   m_status;			//����״̬(0-����,1-ά��)
		BOOL   m_isRecommend;		//�Ƿ��Ƽ�
		UINT32 m_accountNum;		//�����˺�����
		CXTLocker m_linkersMapLocker;
		std::map<BYTE/*����linker id*/, LinkerInfo> m_linkersMap;	//����linker��
	};

public:
	PartitionInfoMan();
	~PartitionInfoMan();

	//���������Ϣ
	BOOL LoadPartitionInfo();
	//ж�ط�����Ϣ
	void UnLoadPartitionInfo();
	//linker��½
	int LinkerLogin(UINT16 partitionId, BYTE linkerId, LicenceSession* session, UINT32 linkerIp, UINT16 linkerPort);
	//linker�˳�
	int LinkerLogout(UINT16 partitionId, BYTE linkerId);
	//��ȡ����״̬
	int GetPartitionStatus(UINT16 partitionId, BYTE& status);
	//�Ƿ��Ƽ�����
	BOOL IsRecommendPartition(UINT16 partitionId);
	//����linker������
	int UpdateLinkerConnections(UINT16 partitionId, BYTE linkerId, UINT32 connections, UINT32 tokens);
	//��������linker
	int StartPartitionLinkerServe(UINT16 partitionId);
	//ֹͣ����linker
	int StopPartitionLinkerServe(UINT16 partitionId);
	//��ȡ�����ڸ�����С��linker
	int GetMinPayloadLinker(UINT16 partitionId, BYTE& linkerId);
	//��linker��ȡtoken
	int UserEnterLinkerGetToken(UINT16 partitionId, BYTE linkerId, UINT32 userId, const char* username);
	//��ȡ����linker��ַ��Ϣ
	int GetPartitionLinkerAddr(UINT16 partitionId, BYTE linkerId, UINT32& linkerIp, UINT16& linkerPort);
	//��ȡ������Ϣ��
	int GetPartitionInfoTable(UINT16 from, UINT16 num, SUserQueryPartitionRes* pQueryRes);
	//֪ͨlinker���û�����
	int NotifyUserFromLinkerOffline(UINT16 partitionId, BYTE exceptLinkerId, UINT32 userId);
	//��ȡ�Ƽ�����id
	UINT16 GetRecommendPartitionId();
	//��ȡ��Сע���û�����
	void GetMinAccountPartition(UINT16& partId, UINT32& accountNum);
	//��ѯ�˺��ѽ���ְҵ
	int QueryUnlockJobs(UINT16 partId, UINT32 userId);
	//���·���ע���û�����
	void UpdatePartitionUsers(UINT16 partId);

public:
	//��ѯ��Ϸ����
	int QueryGamePartition(UINT16 from, BYTE num, SExtManQueryGamePartitionRes* pRes);
	//���ӷ���
	int AddPartition(const char* name, const char* lic, BYTE isRecommend, UINT16& newPartId);
	//�޸ķ�������
	int ModifyPartitionName(UINT16 partitionId, const char* newName);
	//�����Ƽ�����
	int SetRecommendPartition(UINT16 partitionId);
	//ɾ������
	int DelPartition(UINT16 partitionId);
	//��ȡ��������
	int GetPartitionName(UINT16 partitionId, char* pszName);

private:
	PartitionSite* GetPartitionSite(UINT16 partitionId);
	static void LinkerHeartTimer(void* param, TTimerID id);
	void OnHeartCheck();

private:
	CyaCoreTimer m_linkerHeartTimer;
	CXTLocker m_partitionInfoMapLocker;
	std::map<UINT16/*����id*/, PartitionSite*> m_partitionInfoMap;	//������Ϣ��
};

BOOL InitPartitionInfoMan();
PartitionInfoMan* GetPartitionInfoMan();
void DestroyPartitionInfoMan();

#endif
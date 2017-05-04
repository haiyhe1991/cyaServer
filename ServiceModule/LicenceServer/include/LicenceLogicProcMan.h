#ifndef __LICENCE_LOGIC_PROC_MAN_H__
#define __LICENCE_LOGIC_PROC_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��֤�������߼�����
*/

#include <map>
#include "cyaTypes.h"

class LicenceSession;
class LicenceLogicProcMan
{
public:
	LicenceLogicProcMan();
	~LicenceLogicProcMan();

public:
	void LinkerLogout(UINT32 partitionId, BYTE linkerId);
	void UserLogout(UINT32 userId);
	int  ProcessLogic(LicenceSession* licSession, UINT16 cmdCode, const void* payload, int nPayloadLen, BYTE pktType);

private:
	typedef int (LicenceLogicProcMan::*fnLgProcessHandler)(LicenceSession* licSession, const void* payload, int nPayloadLen);
	std::map<UINT16, fnLgProcessHandler> m_lgProcHandler;

private:
	//link��������¼
	int OnLinkerLogin(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//�û���½
	int OnUserLogin(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//��ѯ����״̬
	int OnQueryPartitionStatus(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//�ϱ�linker��ǰ������
	int OnReportLinkerConnections(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//��ȡ�ϱ�linker������ʱ����
	int OnGetLinkerReportTimer(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//��link��������ȡtoken��Ӧ��
	int OnLinkGetTokenResPkt(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//�û��������
	int OnUserEnterPartition(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//�û���ѯ������Ϣ
	int OnUserQueryPartitionInfo(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//֪ͨ����linker�ϵ��û�����
	int OnNotifyLinkerUserOffline(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//ӵ��������ɾ����ɫ
	int OnCreateOrDelRole(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//��ѯ�û���½��Ϣ
	int OnQueryUserLoginInfo(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//ע���û�
	int OnRegistUser(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//��ѯ�ѽ���ְҵ
	int OnQueryUnlockJobs(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//��link��������ȡ����ְҵ��Ӧ��
	int OnLinkerQueryUnlockJobsResPkt(LicenceSession* licSession, const void* payload, int nPayloadLen);
};

void InitLicenceLogicProcMan();
LicenceLogicProcMan* GetLicenceLogicProcMan();
void DestroyLicenceLogicProcMan();

#endif
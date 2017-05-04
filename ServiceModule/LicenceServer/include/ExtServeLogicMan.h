#ifndef __EXT_SERVE_LOGIC_MAN_H__
#define __EXT_SERVE_LOGIC_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
����������߼�����
*/

#include <map>
#include "cyaTypes.h"

class LicenceSession;
class ExtServeLogicMan
{
public:
	ExtServeLogicMan();
	~ExtServeLogicMan();

public:
	int ProcessLogic(LicenceSession* licSession, UINT16 cmdCode, const void* payload, int nPayloadLen, BYTE pktType);

private:
	//��ѯ�û���
	int OnQueryUserPartition(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//��ѯ����˻�
	int OnQueryPlayerAccount(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//��ѯ��Ϸ����
	int OnQueryGamePartition(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//���ӷ���
	int OnAddPartition(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//ɾ������
	int OnDelPartition(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//�޸ķ�������
	int OnModPartitionName(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//�����Ƽ�����
	int OnSetRecommandPartition(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//����������������(DATA_PACKET)
	int OnStartPartitionServe(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//ֹͣ������������(DATA_PACKET)
	int OnStopPartitionServe(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//����link������Ӧ��(DATA_CTRL_PACKET)
	int OnStartLinkServeResPkt(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//ֹͣlink������Ӧ��(DATA_CTRL_PACKET)
	int OnStopLinkServeResPkt(LicenceSession* licSession, const void* payload, int nPayloadLen);

private:
	typedef int (ExtServeLogicMan::*fnExtManProcessHandler)(LicenceSession* licSession, const void* payload, int nPayloadLen);
	std::map<UINT16, fnExtManProcessHandler> m_handlerMap;
};

void InitExtServeLogicMan();
ExtServeLogicMan* GetExtServeLogicMan();
void DestroyExtServeLogicMan();

#endif
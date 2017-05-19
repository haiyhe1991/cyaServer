#ifndef __PERSONAL_RAND_INST_MAN_H__
#define __PERSONAL_RAND_INST_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
���������������
*/

#include <map>
#include "cyaCoreTimer.h"
#include "cyaThread.h"
#include "ServiceMLogic.h"

class PersonalRandInstMan
{
	PersonalRandInstMan(const PersonalRandInstMan&);
	PersonalRandInstMan& operator = (const PersonalRandInstMan&);

public:
	PersonalRandInstMan();
	~PersonalRandInstMan();

	void Start();
	void Stop();

	int EnterRandInstId(UINT16 instId);
	void LeaveRandInstId(UINT16 instId);
	int GetTransferEnterNum(UINT16 transferId, UINT16& enterNum, UINT32& leftSec);
	int GetOpenedTransfers(SGetOpenPersonalRandInstRes* pOpenRes);

private:
	static void RandTransferTimer(void* param, TTimerID id);
	void OnRandTransfer();

	static int THWorker(void* param);
	int OnWorker();

private:
	int InputOpenTransfer(UINT16 transferId, UINT16 instId);
	void OnRandInstClose(UINT16 transferId, UINT16 instId);
	BOOL IsInOpenDurationTime(BYTE startHour, BYTE startMin, BYTE endHour, BYTE endMin);

private:
	struct SOpenRandInstStatus
	{
		UINT16 transferId;	//��ǰ��������
		UINT16 enterNum;	//��ǰ��������
		LTMSEL startMSel;	//����ʱ��
	};

	UINT16 m_maxEnterNum;			//������������������
	UINT32 m_openRandInstInterval;	//�����������ʱ����
	UINT32 m_lastTimeSec;			//��������ʱ��

	BOOL m_exit;
	OSThread m_thHandle;
	CyaCoreTimer m_timer;
	CXTLocker m_openTransferMapLocker;
	std::map<UINT16/*����id*/, SOpenRandInstStatus> m_openTransferMap;
};

void InitPersonalRandInstMan();
void DestroyPersonalRandInstMan();
PersonalRandInstMan* GetPersonalRandInstMan();
void StartPersonalRandInst();
void StopPersonalRandInst();

#endif
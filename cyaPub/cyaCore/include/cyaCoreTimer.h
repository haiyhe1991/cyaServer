#ifndef __CYA_CORE_TIMER_H__
#define __CYA_CORE_TIMER_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaLocker.h"
#include "cyaCoreExports.h"

struct STimerInfos
{
	TTimerID id;
	BOOL asyncStopping; // �Ƿ������첽�˳���
	fnTimerCallback timerCall;
	void* param;
	DWORD periodMs;
	LTMSEL startTime;
};

class CYACORE_DECL CyaCoreTimer
{
public:
	// ����ʱ����timer�������������stop״̬
	// ����˵����ͬITimerPool::Add()
	CyaCoreTimer(fnTimerCallback timerCall, void* param, DWORD periodMs, LTMSEL startTime = MIN_UTC_MSEL);

	CyaCoreTimer();
	~CyaCoreTimer();

	BOOL IsStarted() const;
	STimerInfos GetTimerInfos() const;

	// ʹ���²�������timer
	// ����˵����ͬITimerPool::Add()
	BOOL Start(fnTimerCallback timerCall, void* param, DWORD periodMs, LTMSEL startTime = MIN_UTC_MSEL);
	// ʹ����ǰ�Ĳ�������timer
	BOOL Start();

	void Stop(BOOL wait = true);
	BOOL SetPeriod(DWORD periodMs);
	BOOL Enforce(); // ��������һ��timer

private:
	CyaCoreTimer(const CyaCoreTimer&);
	CyaCoreTimer& operator= (const CyaCoreTimer&);

	mutable CXTLocker actionLock_; // ��������ֻ��Start()��Stop()����
	mutable STimerInfos timerInfos_;
};

#endif
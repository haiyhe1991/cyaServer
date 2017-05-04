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
	BOOL asyncStopping; // 是否正在异步退出中
	fnTimerCallback timerCall;
	void* param;
	DWORD periodMs;
	LTMSEL startTime;
};

class CYACORE_DECL CyaCoreTimer
{
public:
	// 构造时填入timer参数，构造后处于stop状态
	// 参数说明类同ITimerPool::Add()
	CyaCoreTimer(fnTimerCallback timerCall, void* param, DWORD periodMs, LTMSEL startTime = MIN_UTC_MSEL);

	CyaCoreTimer();
	~CyaCoreTimer();

	BOOL IsStarted() const;
	STimerInfos GetTimerInfos() const;

	// 使用新参数启动timer
	// 参数说明类同ITimerPool::Add()
	BOOL Start(fnTimerCallback timerCall, void* param, DWORD periodMs, LTMSEL startTime = MIN_UTC_MSEL);
	// 使用以前的参数启动timer
	BOOL Start();

	void Stop(BOOL wait = true);
	BOOL SetPeriod(DWORD periodMs);
	BOOL Enforce(); // 立即运行一次timer

private:
	CyaCoreTimer(const CyaCoreTimer&);
	CyaCoreTimer& operator= (const CyaCoreTimer&);

	mutable CXTLocker actionLock_; // 动作锁，只锁Start()和Stop()操作
	mutable STimerInfos timerInfos_;
};

#endif
#ifndef __CONSUME_PRODUCE_MAN_H__
#define __CONSUME_PRODUCE_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
玩家消费产出记录
*/

#include <queue>
#include "cyaLocker.h"
#include "cyaThread.h"

class ConsumeProduceMan
{
	ConsumeProduceMan(const ConsumeProduceMan&);
	ConsumeProduceMan& operator = (const ConsumeProduceMan&);

	struct SConsumeProduceLogItem
	{
		BYTE moneyType;		//消费/产出货币类型(0-金币， 1-代币)
		UINT32 roleId;
		UINT32 num;		//该次消费数量
		UINT64 remain;	//当前余额
		LTMSEL consumeDate;
		std::string nick;
		std::string consumeDesc;
	};

public:
	ConsumeProduceMan();
	~ConsumeProduceMan();

	void Start();
	void Stop();

public:
	//插入消费记录
	void InputConsumeRecord(BYTE moneyType, UINT32 roleId, const char* nick, UINT32 num, const char* consumeAction, UINT64 remain);
	//插入产出记录
	void InputProduceRecord(BYTE moneyType, UINT32 roleId, const char* nick, UINT32 num, const char* consumeAction, UINT64 remain);

private:
	static int THWorker(void* param);
	BOOL OnWriteConsumeLog();
	BOOL OnWriteProduceLog();

private:
	BOOL m_exit;
	OSThread m_thHandle;

	CXTLocker m_consumeLogQLocker;
	std::queue<SConsumeProduceLogItem> m_consumeLogQ;
	CXTLocker m_produceLogQLocker;
	std::queue<SConsumeProduceLogItem> m_produceLogQ;
};

void InitConsumeProduceMan();
ConsumeProduceMan* GetConsumeProduceMan();
void DestroyConsumeProduceMan();

#endif
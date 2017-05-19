#ifndef __PLAYER_ACTION_MAN_H__
#define __PLAYER_ACTION_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
玩家行为记录
*/

#include <queue>
#include "cyaThread.h"
#include "cyaLocker.h"

class PlayerActionMan
{
	PlayerActionMan(const PlayerActionMan&);
	PlayerActionMan& operator = (const PlayerActionMan&);

public:
	PlayerActionMan();
	~PlayerActionMan();

	struct SEnterInstItem
	{
		UINT32 roleId;
		std::string strNick;
		BYTE job;
		BYTE level;
		UINT16 instId;
		UINT16 result;
		LTMSEL tts;

		SEnterInstItem()
		{
			roleId = 0;
			strNick = "";
			job = 0;
			level = 0;
			instId = 0;
			result = 0;
			tts = 0;
		}
	};

	struct SFinishInstItem
	{
		UINT32 roleId;
		UINT16 instId;
		UINT16 result;
		LTMSEL enter_tts;
		LTMSEL fin_tts;

		SFinishInstItem()
		{
			roleId = 0;
			instId = 0;
			result = 0;
			enter_tts = 0;
			fin_tts = 0;
		}
	};


	void Start();
	void Stop();

public:
	void InputEnterInstLog(const SEnterInstItem& enterItem);
	void InputFinishInstLog(const SFinishInstItem& finItem);

private:
	static int THWorker(void* param);
	BOOL OnRecordEnterInst();
	BOOL OnRecordFinishInst();

private:
	BOOL m_exit;
	OSThread m_thHandle;

	CXTLocker m_enterInstQLocker;
	std::queue<SEnterInstItem> m_enterInstQ;

	CXTLocker m_finInstQLocker;
	std::queue<SFinishInstItem> m_finInstQ;
};

void InitPlayerActionMan();
PlayerActionMan* GetPlayerActionMan();
void DestroyPlayerActionMan();

#endif
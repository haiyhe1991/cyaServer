#ifndef __ACTIVITY_STAGE_CFG_H__
#define __ACTIVITY_STAGE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include "cyaBase.h"

class ActivityStageCfg
{
public:
	enum Type
	{
		Level = 1,
		PassInst = 2
	};

	struct Condition
	{
		int type;
		int num;
	};

public:
	ActivityStageCfg();
	~ActivityStageCfg();

public:
	//载入配置
	BOOL Load(const char* pszFile);

	//获取开启条件
	bool GetActivityStageOpenCondition(UINT16 instId, Condition &ret);

private:
	std::map<UINT16, Condition> m_Condition;
};

#endif
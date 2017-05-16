#ifndef __ACTIVITY_CFG_H__
#define __ACTIVITY_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�����
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class ActivityCfg
{
public:
	ActivityCfg();
	~ActivityCfg();

#pragma pack(push, 1)
	struct SActivityItem
	{
		LTMSEL startMSel;
		LTMSEL endMSel;
		std::vector<SDropPropCfgItem> rewardVec;
	};
#pragma pack(pop)

public:
	//���������ļ�
	BOOL Load(const char* filename);
	//��ȡ���Ϣ
	int GetActivityItem(UINT32 activityId, SActivityItem& activityItem);

private:
	std::string m_filename;
};

#endif
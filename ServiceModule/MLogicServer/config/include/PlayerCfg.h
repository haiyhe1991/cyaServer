#ifndef __PLAYER_CFG_H__
#define __PLAYER_CFG_H__

/*
玩家配置
*/

#include "CfgCom.h"

class PlayerCfg
{
public:
	PlayerCfg();
	~PlayerCfg();

public:
	//载入配置
	BOOL Load(const char* filename);

	// 生成周免角色
	BYTE GeneraWeekFreeActorType();

private:
	std::map<BYTE, UINT32> m_playerInfo;
};


#endif
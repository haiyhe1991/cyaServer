#ifndef __PLAYER_CFG_H__
#define __PLAYER_CFG_H__

/*
�������
*/

#include "CfgCom.h"

class PlayerCfg
{
public:
	PlayerCfg();
	~PlayerCfg();

public:
	//��������
	BOOL Load(const char* filename);

	// ���������ɫ
	BYTE GeneraWeekFreeActorType();

private:
	std::map<BYTE, UINT32> m_playerInfo;
};


#endif
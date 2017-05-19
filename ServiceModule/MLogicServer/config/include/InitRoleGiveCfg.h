#ifndef __INIT_ROLE_GIVE_CFG_H__
#define __INIT_ROLE_GIVE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��ɫ��ʼ����
*/

#include <vector>
#include "CfgCom.h"

class InitRoleGiveCfg
{
public:
	InitRoleGiveCfg();
	~InitRoleGiveCfg();

#pragma pack(push, 1)
	struct SMoneyItem
	{
		UINT32 gold;
		UINT32 rpcoin;
		SMoneyItem()
		{
			gold = 0;
			rpcoin = 0;
		}
	};
	struct SOtherItem
	{
		UINT32 exp;
		UINT32 vit;
		SOtherItem()
		{
			exp = 0;
			vit = 0;
		}
	};
	// 	struct SEquipItem
	// 	{
	// 		UINT16 handId;		//����
	// 		UINT16 shoesId;		//Ь��
	// 		UINT16 necklaceId;	//����
	// 		UINT16 ringId;		//����
	// 		UINT16 weaponId;	//����
	// 		UINT16 armorId;		//����
	// 		SEquipItem()
	// 		{
	// 			handId = 0;
	// 			shoesId = 0;
	// 			necklaceId = 0;
	// 			ringId = 0;
	// 			weaponId = 0;
	// 			armorId = 0;
	// 		}
	// 	};
	struct SInitRoleGiveItem
	{
		SMoneyItem money;
		SOtherItem other;
		//SEquipItem wearEquip;
		std::vector<UINT16> wearEquipVec;
		std::vector<SDropPropCfgItem> propVec;
		SInitRoleGiveItem()
		{
			wearEquipVec.clear();
			propVec.clear();
		}
	};
#pragma pack(pop)

public:
	//���������ļ�
	BOOL Load(const char* filename);
	//��ȡ��ɫ��ʼ����
	BOOL GetRoleInitGiveItem(SInitRoleGiveItem& giveItem);

private:
	std::string m_filename;
};

#endif
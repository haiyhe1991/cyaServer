#ifndef __EQUIP_COMPOSE_CFG_H__
#define __EQUIP_COMPOSE_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include <vector>
#include "CfgCom.h"

class EquipComposeCfg
{
public:
	EquipComposeCfg();
	~EquipComposeCfg();

#pragma pack(push, 1)
	union UComposeCondition
	{
		struct
		{
			BYTE minLv;
			BYTE maxLv;
		} lv;

		struct
		{
			LTMSEL start;
			LTMSEL end;
		} tm;

		struct
		{
			UINT16 magicId;
		} magic;

		struct
		{
			BYTE num;
			SDropPropCfgItem* pHasProp;
		} prop;
	};
	struct SComposeConditionItem
	{
		BYTE type;
		UComposeCondition item;
	};
	struct SEquipComposeItem
	{
		std::vector<SDropPropCfgItem> compsoePropVec;			//�ϳɵõ���Ʒ
		std::vector<SDropPropCfgItem> basicConsumePropVec;		//����������Ʒ
		std::vector<SDropPropCfgItem> additionalConsumePropVec;	//����������Ʒ
		std::vector<SComposeConditionItem> conditionVec;		//����
		BYTE chance;		//�ϳɸ���
		UINT32 consumeGold;	//���Ľ��
	};
#pragma pack(pop)

public:
	//��������
	BOOL Load(const char* pszFile);
	//��ȡװ���ϳ�����
	int GetEquipComposeItem(UINT16 composeId, SEquipComposeItem*& pComposeItem);

private:
	std::map<UINT16, SEquipComposeItem> m_equipComposeMap;
};

#endif
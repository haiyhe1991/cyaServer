#ifndef __EQUIP_CFG_H__
#define __EQUIP_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
װ������
*/

#include <map>
#include <set>
#include <vector>
#include "CfgCom.h"

class EquipCfg
{
public:
	EquipCfg();
	~EquipCfg();

#pragma pack(push, 1)
	//������Ʒ��Ϣ
	// 	struct SGiveProps
	// 	{
	// 		UINT16 propId;
	// 		BYTE   propType;
	// 		BYTE   num;
	// 		SGiveProps()
	// 		{
	// 			propId = 0;
	// 			propType = 0;
	// 			num = 0;
	// 		}
	// 	};
	//����װ����Ϣ
	// 	struct SGiveWearEquip
	// 	{
	// 		UINT16 handId;		//����
	// 		UINT16 shoesId;		//Ь��
	// 		UINT16 necklaceId;	//����
	// 		UINT16 ringId;		//����
	// 		UINT16 weaponId;	//����
	// 		UINT16 armorId;		//����
	// 
	// 		SGiveWearEquip()
	// 		{
	// 			handId = 0;
	// 			shoesId = 0;
	// 			necklaceId = 0;
	// 			ringId = 0;
	// 			weaponId = 0;
	// 			armorId = 0;
	// 		}
	// 	};
	//������Ϣ
	// 	struct SGiveInfo
	// 	{
	// 		UINT64 gold;							//���ͽ��
	// 		UINT32 rpcoin;							//���ʹ���
	// 		SGiveWearEquip giveWearEquip;			//����װ��
	// 		std::vector<SGiveProps> giveProps;	//������Ʒ
	// 		SGiveInfo()
	// 		{
	// 			gold = 0;
	// 			rpcoin = 0;
	// 			giveProps.clear();
	// 		}
	// 	};
	//װ���ӵ�
	struct SEquipAddPoint
	{
		BYTE   type;	//�ӵ�����
		UINT32 val;		//�ӵ�ֵ
		BYTE   master;	//��/������

		SEquipAddPoint()
		{
			type = 0;
			val = 0;
			master = 0;
		}
	};
	//װ������
	struct SEquitAttr
	{
		std::string name;				//����
		BYTE quality;		//Ʒ��
		BYTE pos;			//װ��λ��
		BYTE limitLevel;	//�������Ƶȼ�(��ʱ��Ч,�����ж�)
		UINT16 suitId;		//��װid
		UINT64 sellPrice;	//�ۼ�(���)
		std::vector<SEquipAddPoint> equipPointVec;	//װ�����Լӵ�
		SEquitAttr()
		{
			name = "";
			quality = 0;
			pos = 0;
			limitLevel = 0;
			suitId = 0;
			sellPrice = 0;
			equipPointVec.clear();
		}
	};
#pragma pack(pop)

public:
	//����ְҵװ������
	BOOL Load(const char* pszFile);
	//��ȡװ����Ϣ
	int GetEquipPropAttr(UINT16 equipId, SPropertyAttr& propAttr, BYTE& maxStack);
	//��ȡװ���û����ۼ۸�
	int GetEquipUserSellPrice(UINT16 equipId, UINT64& price);
	//��ȡװ�����ѵ���
	BYTE GetEquipMaxStack(UINT16 equipId);
	//��ȡװ������
	void GetEquipName(UINT16 equipId, std::string& name);
	//��ȡװ�����Ե�
	int GetEquipAttrPoint(UINT16 equipId, std::vector<SEquipAddPoint>*& addPoint);

private:
	std::map<UINT16/*װ��id*/, SEquitAttr> m_equipMap;			//װ����
	std::map<UINT16/*��װid*/, std::set<UINT16>/*װ��id*/ > m_suitEquipMap;	//��װ
};
#endif
#ifndef __GEM_CFG_H__
#define __GEM_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
��ʯ����
*/

#include <map>
#include "CfgCom.h"
#include "ServiceMLogic.h"

class GemCfg
{
public:
	GemCfg();
	~GemCfg();

#pragma pack(push, 1)
	struct SGemAttrCfg
	{
		BYTE type;	//��ʯ����
		BYTE level;	//�ȼ�
		UINT64 playerSellGold;	//��ҳ��۽�Ҽ�
		//UINT32 shopSellToken;	//�̳ǳ��۴���
		//UINT64 playerBuyBackGold;	//��һع���Ҽ�
		UINT32 attrVal;				//��ʯ����ֵ
		BYTE   maxStack;			//���ѵ���
		SGemAttrCfg()
		{
			type = 0;
			level = 0;
			playerSellGold = 0;
			//shopSellToken = 0;
			//playerBuyBackGold = 0;
			attrVal = 0;
			maxStack = 0;
		}
	};
	struct SEquipGemHoleLimitCfg
	{
		BYTE equipLv;	//װ��ǿ���ȼ�
		BYTE gemHoles;	//��ʯ���׺�
		SEquipGemHoleLimitCfg()
		{
			equipLv = 0;
			gemHoles = 0;
		}
	};
	struct SGemConsumeCfg
	{
		UINT64 loadGold;	//��Ƕ����
		UINT64 unloadGold;	//ȡ������
		UINT64 composeGold;	//�ϳ�����
		UINT64 decomposeGold;	//�ֽ�����
		SGemConsumeCfg()
		{
			loadGold = 0;
			unloadGold = 0;
			composeGold = 0;
			decomposeGold = 0;
		}
	};
	struct SEquipGemAttr
	{
		BYTE gemType;		//��ʯ����
		BYTE equipLimitlv;	//װ��ǿ���ȼ�����
		SEquipGemAttr()
		{
			gemType = 0;
			equipLimitlv = 0;
		}
	};
#pragma pack(pop)

public:
	//���뱦ʯ�������
	BOOL Load(const char* szGemFile, const char* szGemCostFile, const char* szGemHoleFile);
	//����װ������,��ʯ�׻�ȡ��Ӧ��ʯ����,װ���ȼ�����
	int GetEquipInlaidGemAttr(BYTE equipType, BYTE holeSeq, SEquipGemAttr& attr);
	//���ݵȼ���ȡ��ʯ����
	int GetGemSpendByLevel(BYTE gemLv, SGemConsumeCfg& spend);
	//��ȡ��ʯ��������
	int GetGemCfgAttr(UINT16 gemPropId, SGemAttrCfg& gemAttr);
	//��ȡ��ʯ��������
	int GetGemPropAttr(UINT16 gemPropId, SPropertyAttr& propAttr, BYTE& maxStack);
	//���ݱ�ʯ����,�ȼ���ȡ��ʯid,���ѵ�����
	int GetGemIdMaxStackByTypeLevel(BYTE gemType, BYTE gemLv, UINT16& gemId, BYTE& maxStack);
	//���ݱ�ʯid��ȡ���ѵ���
	BYTE GetGemMaxStack(UINT16 gemId);
	//��ȡ��ʯ�û����ۼ۸�(����)
	int GetGemUserSellPrice(UINT16 gemId, UINT64& price);
	//���ݱ�ʯid��ȡ��ʯ����
	int GetGemSpendById(UINT16 gemId, SGemConsumeCfg& spend);
	//��ȡ��ʯ�ϳ����ı�ʯ����
	UINT16 GetComposeGemConsumeGems() const;

private:
	//���뱦ʯ����
	BOOL LoadGemCfg(const char* filename);
	//����װ����ʯ������������
	BOOL LoadGemHoleLimitCfg(const char* filename);
	//���뱦ʯ��������
	BOOL LoadGemConsumeCfg(const char* filename);
	//����װ����ʯ����
	BOOL LoadEquipGemCfg(const char* filename);

private:
	std::map<UINT16/*��ʯid*/, SGemAttrCfg> m_gemAttrMap;	//��ʯ����
	std::map<BYTE/*װ��ǿ���ȼ�*/, BYTE/*������*/> m_equipLimitHolesMap;	//װ����������
	std::map<BYTE/*��ʯ�ȼ�*/, SGemConsumeCfg> m_gemConsumeMap;	//��ʯ����
	std::map<BYTE/*װ������*/, std::vector<SEquipGemAttr>/*����Ƕ��ʯ����*/ > m_equipGemHolesMap;
};


#endif
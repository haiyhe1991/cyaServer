#ifndef __VIP_CFG_H__
#define __VIP_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include <vector>
#include "CfgCom.h"

class VIPCfg
{
public:
	VIPCfg();
	~VIPCfg();

#pragma pack(push, 1)
	struct SVIPGiveProp
	{
		BYTE type;
		UINT16 propId;
		UINT32 num;

		SVIPGiveProp()
		{
			type = 0;
			propId = 0;
			num = 0;
		}
	};
	struct SVIPAttrCfg
	{
		UINT32 cashcount;	//�ﵽ��ֵ�ܶ�
		UINT32 maxVit;		//�������
		UINT16 buyVitNum;	//������������
		UINT16 goldExchangeNum;	//��Ҷһ�����
		UINT16 buyKOFnum;			//����ģʽ�������
		UINT16 fightNum;			//1v1�����������
		BYTE   deadReviveNum;	//�����������
		UINT16 instSweepNum;	//����ɨ������
		UINT16 resourceInstChalNum;	//��Դ������ս����
		BYTE   mallBuyDiscount;	//�̳ǹ�������ۿ�
		BYTE   signReward;		//ǩ����������
		std::vector<SVIPGiveProp> givePropVec;	//������Ʒ

		SVIPAttrCfg()
		{
			buyKOFnum = 0;
			cashcount = 0;
			maxVit = 0;
			buyVitNum = 0;
			goldExchangeNum = 0;
			deadReviveNum = 0;
			instSweepNum = 0;
			resourceInstChalNum = 0;
			mallBuyDiscount = 0;
			signReward = 0;
			givePropVec.clear();
		}
	};
#pragma pack(pop)

public:
	//��������
	BOOL Load(const char* filename);
	//����VIP�ȼ�
	BYTE CalcVIPLevel(UINT32 cashcount);
	//��ȡVIP������Ʒ
	void GetVIPGiveProps(BYTE vipLv, std::vector<SVIPGiveProp>*& pGivePropVec);
	//��ȡVIP�������
	void GetVIPMaxVit(BYTE vipLv, UINT32& maxVit);
	//��ȡVIP������������
	void GetVIPBuyVitTimes(BYTE vipLv, UINT16& buyVitTimes);
	//��ȡVIP��Ҷһ�����
	void GetVIPGoldExchangeTimes(BYTE vipLv, UINT16& exchangeTimes);
	//��ȡVIP�����������
	void GetVIPDeadReviveTimes(BYTE vipLv, BYTE& reviveTimes);
	//��ȡVIP����ɨ������
	void GetVIPInstSweepTimes(BYTE vipLv, UINT16& sweepTimes);
	//��ȡVIP��Դ������ս����
	void GetVIPResourceInstChalNum(BYTE vipLv, UINT16& chalTimes);
	//��ȡVIP�̳ǹ�������ۿ�
	void GetVIPMallBuyDiscount(BYTE vipLv, BYTE& discount);
	//��ȡVIPǩ����������
	void GetVIPSignReward(BYTE vipLv, BYTE& signReward);
	//��ȡVIP���򾺼�ģʽ����
	void GetVIPBuyKOFTimes(BYTE vipLv, UINT16& buyKOFTimes);
	//��ȡ1v1��������
	void GetOneVsOneEnterTimes(BYTE vipLv, UINT16& enterTimes);

private:
	std::map<BYTE/*VIP�ȼ�*/, SVIPAttrCfg> m_vipMap;
};

#endif
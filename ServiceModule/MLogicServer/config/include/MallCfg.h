#ifndef __MALL_CFG_H__
#define __MALL_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�̳�����
*/

#include <map>
#include <set>
#include "CfgCom.h"

class MallCfg
{
public:
	MallCfg();
	~MallCfg();

#pragma pack(push, 1)
	struct SMallGoodsAttr
	{
		BYTE  type;				//��Ʒ����
		UINT16 propId;			//��Ʒid
		double discount;		//�ۿ�
		UINT32 priceToken;		//�ۼ�(����)
		LTMSEL sellDateLimit;	//����ʱ������
		UINT16 useDays;			//ʹ������
		UINT16 stackNum;		//���ι�������
		BYTE   vipLvLimit;		//VIP�ȼ�����(VIP�ȼ�>=vipLvLimit���ܹ������Ʒ)
		BYTE   discountVIPLvLimit;	//�ۿ�VIP�ȼ�����(VIP�ȼ�>=discountVIPLvLimit���ܴ�discount��)

		SMallGoodsAttr()
		{
			type = 0;				//��Ʒ����
			propId = 0;				//��Ʒid
			discount = 0.0;			//�ۿ�
			priceToken = 0;			//�ۼ�(����)
			sellDateLimit = 0;		//����ʱ������
			useDays = 0;			//ʹ������
			stackNum = 0;			//���ι����������
			vipLvLimit = 0;			//VIP�ȼ�����
			discountVIPLvLimit = 0;	//�ۿ�VIP�ȼ�����
		}
	};
#pragma pack(pop)

public:
	//�����̳�����
	BOOL LoadMall(const char* filename);
	//��ȡ�̳���Ʒ����
	int GetMallBuyGoodsAttr(UINT32 buyId, SMallGoodsAttr*& pGoodsAttr);
	int GetMallBuyGoodsAttrByPropId(UINT16 propId, SMallGoodsAttr *&pGoodsAttr);

private:
	std::map<UINT16, UINT32> m_goodsIndex;
	std::map<UINT32, SMallGoodsAttr> m_goodsMap;
};

#endif
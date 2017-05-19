#ifndef __LV_RANKING_CFG_H__
#define __LV_RANKING_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�ȼ����н���� add by hxw 20150929
*/

#include <vector>
#include "CfgCom.h"
#include "ServiceErrorCode.h"

class LvRankingCfg
{
public:
	LvRankingCfg(void);
	virtual ~LvRankingCfg(void);


	struct SLvRanking
	{
		BOOL IsOpen;
		LTMSEL StartTime;
		int  EndTime;
		std::vector<SDropPropCfgItem> propVec;
		SLvRanking()
		{
			IsOpen = false;
			EndTime = 0;
		}
	};

	struct SLvRankingCV
	{
		std::vector<SDropPropCfgItem> rewardvs;
		int		  cvalue;
		SLvRankingCV(){
			cvalue = 0;
		}
	};

public:
	//���������ļ�
	BOOL Load(const char* filename);
	//��ȡ�ȼ����н���
	BOOL GetLvRanking(const BYTE rank, const int Lv, std::vector<SDropPropCfgItem>*& items);
	//�Ƿ��ǻʱ�䣬����TRUE��ʾ�������FLASE��ʾ�޷��
	BOOL IsValidityTime();
	//�Ƿ���
	BOOL IsOpen() const { return m_IsOpen; }

	void SetOpen(const BOOL bOpen) { m_IsOpen = bOpen; }

private:
	std::string m_filename;
	BOOL m_IsOpen;
	//�Ƿ���Чʱ����
	BOOL m_IsValidity;
	LTMSEL m_StartTime;
	int  m_EndTime;  //��λСʱ
	std::map<BYTE/*����*/, SLvRankingCV> m_LvRewardsMap;
};

#endif
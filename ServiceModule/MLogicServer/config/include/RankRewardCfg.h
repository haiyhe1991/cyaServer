#ifndef __RANK_REWARD_CFG_H__
#define __RANK_REWARD_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

/*
�ȼ����н���� add by hxw 20150929
*/

#include <vector>
#include "CfgCom.h"
#include "ServiceErrorCode.h"

//���еȼ� ����
const int __Level_Rank_Type = 1;
//���и�����ս��������
const int __InstanceNum_Rank_Type = 2;
//ǿ�߽�������
const int __Stronger_Reward_Type = 3;
//����ʱ�佱��
const int __Online_Reward_Type = 4;

const char C_REWARD_INFO[4][32] = { "�弶���˽���", "ˢͼ���˽���", "ǿ�߽���", "���߽���" };

class CRankReward
{
public:
	struct SRkRewardRes
	{
		std::vector<SDropPropCfgItem> rewardvs;
		UINT32	  Value;

	};

public:
	CRankReward() :m_IsValidity(false), m_IsOpen(false){}
	virtual ~CRankReward(){}

	void SetFile(const char* fname){ m_fileName = fname; }

	virtual BOOL Load(const char* fname);

	INT GetRewardNum() const {
		return m_LvRewardsMap.size();
	}

	//��ȡ�ȼ����н���
	BOOL GetReward(const UINT16 rankId, const int iValue, CRankReward::SRkRewardRes*& items);
	//�Ƿ��ǻʱ�䣬����TRUE��ʾ�������FLASE��ʾ�޷��
	BOOL bValiTime();
	//�Ƿ���
	BOOL bOpen() const { return m_IsOpen; }
	//���ÿ���״̬
	void SetOpen(const BOOL bOpen) { m_IsOpen = bOpen; }

	BOOL IsCanGet(const UINT16 id, const UINT32 value) const;

	int GetValue(UINT16 id);

private:
	std::string m_fileName;
	BOOL m_IsOpen;
	//�Ƿ���Чʱ����
	BOOL m_IsValidity;
	std::map<UINT32, SRkRewardRes> m_LvRewardsMap;
	//���ʼʱ��
	LTMSEL m_StartTime;
	//�����ʱ��
	LTMSEL  m_EndTime;
};

class RankRewardCfg
{
public:
	RankRewardCfg(void);
	virtual ~RankRewardCfg(void);
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

public:
	//���������ļ�
	BOOL Load(std::vector<std::string> vnames);
	//��ȡ�ȼ����н���
	/*
	type > 0
	rankid ����ID
	*/
	//
	//��ȡ���н����������
	INT GetRewardPlayNum(const BYTE type) const;

	BOOL GetRankReward(const BYTE type, const UINT16 rankid, CRankReward::SRkRewardRes*& items, const int iValue);
	//�Ƿ��ǻʱ�䣬����TRUE��ʾ�������FLASE��ʾ�޷��
	BOOL IsValidityTime(const BYTE type);
	//�Ƿ���
	BOOL IsOpen(const BYTE type) const;

	void SetOpen(const BYTE type, const BOOL bOpen);

	BOOL IsCanGet(const BYTE type, const UINT16 id, const UINT32 value) const;

	//��ȡ��־��Ϣ
	int GetLoginfoName(std::string& logsz, BYTE type, UINT16 reward = 0);


private:
	/*1 LV����   2 ˢͼ    3 cp     4 time*/
	CRankReward m_rankReward[RANK_REWARD_TYPE_NUM];
};


#endif
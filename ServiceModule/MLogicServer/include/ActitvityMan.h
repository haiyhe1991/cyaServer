#ifndef __ACTIVITY_MAN_H__
#define __ACTIVITY_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�����
*/

#include <vector>
#include "cyaTypes.h"
#include "ServiceMLogic.h"
#include "RouletteCfg.h"
#include "GameCfgFileMan.h"

class ActitvityMan
{
public:
	struct SDBSign
	{
		BYTE day;

		// ��������
		bool operator< (const SDBSign &that) const
		{
			return day < that.day;
		}
	};

public:
	ActitvityMan();
	~ActitvityMan();

	//��ѯ�Ѿ���ȡ�Ļ
	int QueryReceivedActivity(UINT32 roleId, SQueryReceivedActivityRes* pActivityRes);
	//�Ƿ���ȡ���Ļ
	BOOL IsReceivedActivity(UINT32 roleId, UINT32 activityId);
	//ˢ�²μӻ
	void RefreshReceivedActivity(UINT32 roleId, UINT32 activityId, const char* pszDesc = "");

	//�һ���У��
	int VerifyCashCode(const char* pszCode, UINT32 roleId, UINT16& rewardId);

	//��ȡ���̽�Ʒ
	int GetRouletteReward(UINT32 roleId, SGetRoulettePropRes* pRoulettePropRes);
	int GetRouletteReward(UINT32 roleId, UINT16& extractNum, std::vector<RouletteCfg::SRoulettePropCfgItem>& itemVec);
	//���½�ɫ����ʣ�ཱƷ
	int UpdateRoleRouletteReward(UINT32 roleId, UINT16 extractNum, const std::vector<RouletteCfg::SRoulettePropCfgItem>& itemVec);

	//��ȡ���̴�
	int FetchRouletteBigReward(UINT32 roleId, UINT16& extractNum, SRoulettePropItem& bigReward);
	//�������̴�
	int UpdateRouletteBigReward(UINT32 roleId, const SRoulettePropItem& bigReward);
	//�������̽�Ʒ
	void ClearRouletteReward();

	/* zpy ��ѯǩ�� */
	int QueryRoleDailySign(UINT32 roleId, SQueryRoleDailySignRes *pQueryRoleSign);
	/* zpy ��ɫǩ�� */
	int OnRoleDailySign(INT32 userID, UINT32 roleId, SRoleDailySignRes *pRoleSign, SReceiveRewardRefresh *pRefresh);
	/* zpy ����ÿ��ǩ�� */
	void ClearRoleDailySign(/*UINT32 roleId delete by hxw 20150929*/);

private:
	/* zpy ��ѯ��ɫǩ����Ϣ */
	int QueryRoleDailySignFromDB(UINT32 roleId, std::vector<SDBSign> &out_days);
	/* zpy д���ɫǩ����Ϣ */
	int WriteRoleDailySignToDB(INT32 roleId, std::vector<SDBSign> &days);
};

void InitActitvityMan();
ActitvityMan* GetActitvityMan();
void DestroyActitvityMan();

#endif
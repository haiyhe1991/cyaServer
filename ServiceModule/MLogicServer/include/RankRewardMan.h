#ifndef __RANK_REWARD_MAN_H__
#define __RANK_REWARD_MAN_H__

#include "ActitvityMan.h"
#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "ServiceProtocol.h"
#include "cyaMaxMin.h"
#include "RolesInfoMan.h"

//add file by hxw 20151006
//���а���������

const int  I_YD_LEN = 8;
const char C_START_TIME[] = "000000";
const char C_END_TIME[] = "235959";


class  RankRewardMan
{
public:
	struct SRanks{
		//��ɫID
		UINT32 roleID;
		//����ID�����ڶ�λ���ݿ��е�λ��
		UINT32 dataID;
		//��������
		UINT16 rankID;
		//����ʱ����
		UINT32 iValue;
		//�Ƿ���ȡ����
		BOOL   bGet;
		//��ɫ����
		BYTE   actorType;
		//��ɫ�ǳ�
		char	nick[33];
	};
public:
	RankRewardMan(){ m_lastRefTime = 0; };
	virtual ~RankRewardMan(){};
	//add by hxw 20151028 ��ѯ�ȼ����а���Ϣ
	int QueryLvRankInfo(SQueryInsnumRanksInfo* ranks);
	//end

	//add by hxw 20150929 
	// ��ѯ�Ƿ���ȡ�ȼ���������
	int QueryLvRank(UINT32 roleId, SQueryRankRewardGet* ranks);
	// ��ѯ�Ƿ���ȡ������ͼ��������
	int QueryInsnumRank(UINT32 roleId, SQueryRankRewardGet* ranks);
	//��ѯ������ͼ���е����а�
	int QueryInsnumRankInfo(SQueryInsnumRanksInfo* ranks);

	//��ȡ�ȼ����н���
	int GetLvRankReward(UINT32 userId, UINT32 roleId, SGetRankRewardRes *pRankAward, SReceiveRewardRefresh *pRefresh);
	//��ȡ������ս��������
	int GetInsnumRankReward(UINT32 userId, UINT32 roleId, SGetRankRewardRes *pRankAward, SReceiveRewardRefresh *pRefresh);

	//��ȡǿ�߽�����ȡ��Ϣ
	int QueryStrongerInfoForDB(UINT32 roleID, SRoleInfos* role);

	int QueryStrongerInfo(UINT32 userId, UINT32 roleId, SQueryStrongerInfos* infos);
	//��ȡǿ�߽���
	int GetStrongerReward(UINT32 userId, UINT32 roleId, UINT16 rewardID, SGetRankRewardRes *pRankAward, SReceiveRewardRefresh *pRefresh);

	//��ȡ���߽�����Ϣfrom DB
	int QueryOnlineInfoForDB(UINT32 roleID, SRoleInfos* role, const char* tts);
	//��ȡ���߽�����Ϣ
	int QueryOnlineInfo(UINT32 userId, UINT32 roleID, SQueryOnlineInfos* infos);
	//�������߽���ʱ�� time
	int UpdateOnlineTime(UINT32 id, LTMSEL time);
	//��ȡ���߽�����ȡ
	int GetOnlineReward(UINT32 userId, UINT32 roleId, UINT16 rewardID, SGetRankRewardRes *pRankAward, SReceiveRewardRefresh *pRefresh);

	//ÿ��0���ʱ������������ҵ����߽�������
	int RefreshOnlineOnZero();

	//����ˢ����������
	int RefreshRanks();

	//������ȡ��������
	int RestRefreshRanks();

#define TEST_UP_RANK_INSNUM 0
#if TEST_UP_RANK_INSNUM
	void UpInst(){ UpdateRankInsnum(); }
#endif

private:
	//ÿ�콫���ÿ�������ս�����������������У����Ҵ浽���б��У�ÿ��12��ִ�У�һ��ִ��һ��
	int UpdateRankInsnum();

	int GetLvRanks();
	//int GetCpRanks();
	int GetInstnumRanks();
	//int GetPoRanks();


	//�޸�ranklevel�е���ȡ״̬
	int UpDBLvRanksGet(UINT32 id);
	//�޸�rankinstnum�е���ȡ״̬
	int UpDBInsnumRanksGet(UINT32 id);
	//��cp_reward�в�����ȡ��Ϣ
	int UpDBCPRewardGet(UINT32 roleId, UINT16 rewardid, UINT32 cp = 0);
	//��online�в���������ȡ����
	int InsertOnline(UINT32 roleID, const char* tts = NULL);
	//����������ȡ����
	int UpDBOnline(UINT32 id, LTMSEL otime, const std::vector<UINT16>* online);

private:
	//�ȼ�����������
	//CXTLocker m_vLocker[RANK_REWARD_TYPE_NUM];
	CXTLocker m_lvRankLK;
	CXTLocker m_lvInsnumLK;
	LTMSEL m_lastRefTime;
	//char m_sTime[33];
	//char m_eTime[33];
	std::vector<SRanks> m_vRanks[RANK_REWARD_TYPE_NUM];


};

void InitRankRewardMan();
RankRewardMan* GetRankRewardMan();
void DestroyRankRewardMan();

#endif
#ifndef __RANK_REWARD_MAN_H__
#define __RANK_REWARD_MAN_H__

#include "ActitvityMan.h"
#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "ServiceProtocol.h"
#include "cyaMaxMin.h"
#include "RolesInfoMan.h"

//add file by hxw 20151006
//排行榜奖励管理类

const int  I_YD_LEN = 8;
const char C_START_TIME[] = "000000";
const char C_END_TIME[] = "235959";


class  RankRewardMan
{
public:
	struct SRanks{
		//角色ID
		UINT32 roleID;
		//数据ID，用于定位数据库中的位置
		UINT32 dataID;
		//排行名次
		UINT16 rankID;
		//排行时数据
		UINT32 iValue;
		//是否领取奖励
		BOOL   bGet;
		//角色类型
		BYTE   actorType;
		//角色昵称
		char	nick[33];
	};
public:
	RankRewardMan(){ m_lastRefTime = 0; };
	virtual ~RankRewardMan(){};
	//add by hxw 20151028 查询等级排行榜信息
	int QueryLvRankInfo(SQueryInsnumRanksInfo* ranks);
	//end

	//add by hxw 20150929 
	// 查询是否领取等级排名奖励
	int QueryLvRank(UINT32 roleId, SQueryRankRewardGet* ranks);
	// 查询是否领取副本推图排名奖励
	int QueryInsnumRank(UINT32 roleId, SQueryRankRewardGet* ranks);
	//查询副本推图排行的排行榜
	int QueryInsnumRankInfo(SQueryInsnumRanksInfo* ranks);

	//获取等级排行奖励
	int GetLvRankReward(UINT32 userId, UINT32 roleId, SGetRankRewardRes *pRankAward, SReceiveRewardRefresh *pRefresh);
	//获取副本挑战次数奖励
	int GetInsnumRankReward(UINT32 userId, UINT32 roleId, SGetRankRewardRes *pRankAward, SReceiveRewardRefresh *pRefresh);

	//获取强者奖励领取信息
	int QueryStrongerInfoForDB(UINT32 roleID, SRoleInfos* role);

	int QueryStrongerInfo(UINT32 userId, UINT32 roleId, SQueryStrongerInfos* infos);
	//获取强者奖励
	int GetStrongerReward(UINT32 userId, UINT32 roleId, UINT16 rewardID, SGetRankRewardRes *pRankAward, SReceiveRewardRefresh *pRefresh);

	//获取在线奖励信息from DB
	int QueryOnlineInfoForDB(UINT32 roleID, SRoleInfos* role, const char* tts);
	//获取在线奖励信息
	int QueryOnlineInfo(UINT32 userId, UINT32 roleID, SQueryOnlineInfos* infos);
	//更新在线奖励时间 time
	int UpdateOnlineTime(UINT32 id, LTMSEL time);
	//获取在线奖励领取
	int GetOnlineReward(UINT32 userId, UINT32 roleId, UINT16 rewardID, SGetRankRewardRes *pRankAward, SReceiveRewardRefresh *pRefresh);

	//每天0点的时候，整理在线玩家的在线奖励数据
	int RefreshOnlineOnZero();

	//定点刷新排行数据
	int RefreshRanks();

	//启动获取排行数据
	int RestRefreshRanks();

#define TEST_UP_RANK_INSNUM 0
#if TEST_UP_RANK_INSNUM
	void UpInst(){ UpdateRankInsnum(); }
#endif

private:
	//每天将玩家每天完成挑战副本次数的数据排行，并且存到排行表中，每天12点执行，一天执行一次
	int UpdateRankInsnum();

	int GetLvRanks();
	//int GetCpRanks();
	int GetInstnumRanks();
	//int GetPoRanks();


	//修改ranklevel中的领取状态
	int UpDBLvRanksGet(UINT32 id);
	//修改rankinstnum中的领取状态
	int UpDBInsnumRanksGet(UINT32 id);
	//在cp_reward中插入领取信息
	int UpDBCPRewardGet(UINT32 roleId, UINT16 rewardid, UINT32 cp = 0);
	//在online中插入在线领取数据
	int InsertOnline(UINT32 roleID, const char* tts = NULL);
	//更新在线领取数据
	int UpDBOnline(UINT32 id, LTMSEL otime, const std::vector<UINT16>* online);

private:
	//等级排行数据锁
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
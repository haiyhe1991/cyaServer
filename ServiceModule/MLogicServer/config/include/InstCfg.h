#ifndef __INST_CFG_H__
#define __INST_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
副本配置
*/

#include <set>
#include <map>
#include <vector>
#include "CfgCom.h"

class MonsterCfg;
class GameDropCfg;
class InstCfg
{
public:
	InstCfg(GameDropCfg* pDropCfg, MonsterCfg* pMonsterCfg);
	~InstCfg();

#pragma pack(push, 1)
	//刷怪属性
	struct SBrushMonster
	{
		std::map<BYTE/*怪等级*/, UINT16/*数量*/> lvMonsterMap;
		SBrushMonster()
		{
			lvMonsterMap.clear();
		}
	};

	//战斗区域掉落
	struct SBattleAreaDrop
	{
		UINT32 dropId;	//掉落id
		BYTE   dropNum;	//掉落数量
		UINT32 gold;	//金币
		BYTE   hasCream;	//是否有精英怪
		BYTE   hasBoss;		//是否有boss怪
		BYTE   hasMonster;	//是否有小怪
		std::map<UINT16/*怪id*/, SBrushMonster>	brushMonster;	//怪

		SBattleAreaDrop()
		{
			dropId = 0;
			dropNum = 0;
			gold = 0;
			hasBoss = 0;
			hasCream = 0;
			hasMonster = 0;
			brushMonster.clear();
		}
	};
	//宝箱
	struct SStorageAreaBox
	{
		UINT32 dropId;	//掉落id
		BYTE   dropNum;	//掉落数量
		UINT32 gold;	//金币

		SStorageAreaBox()
		{
			dropId = 0;
			dropNum = 0;
			gold = 0;
		}
	};
	//破碎物品
	struct SBrokenPropDrop
	{
		UINT32 dropId;	//掉落id
		BYTE   dropNum;	//掉落数量
		UINT32 gold;	//金币

		SBrokenPropDrop()
		{
			dropId = 0;
			dropNum = 0;
			gold = 0;
		}
	};

	//副本
	struct SInstAttrCfg
	{
		UINT32 spendVit;		//消耗体力
		BYTE   limitRoleLevel;	//限制角色等级(角色等级小于该值则不能进入该副本)
		UINT16 reviveId;		//复活id
		UINT16 frontInstId;		//前置关卡id
		UINT16 chapterId;		//所属章节
		UINT16 enterNum;		//进入次数
		BYTE   type;			//副本类型
		UINT32 sweepExp;		//扫荡经验
		float degree;			//副本难度系数
		std::string name;		//副本名字
		SBrokenPropDrop brokenDrops;	//破碎物掉落
		std::map<BYTE/*战斗区域id*/, SBattleAreaDrop> battleArea;	//战斗区域掉落
		std::map<BYTE/*宝箱id*/, SStorageAreaBox> boxDrops;			//宝箱

		SInstAttrCfg()
		{
			spendVit = 0;
			limitRoleLevel = 0;
			degree = 0.0f;
			frontInstId = 0;
			chapterId = 0;
			type = 0;
			sweepExp = 0;
			reviveId = 0;
			battleArea.clear();
			boxDrops.clear();
		}
	};
	//副本掉落宝箱
	struct SInstDropBox
	{
		std::map<BYTE/*宝箱id*/, std::vector<SDropPropCfgItem> > boxMap;
		SInstDropBox()
		{
			boxMap.clear();
		}
	};
	//副本出怪
	struct SInstBrushMonster
	{
		std::map<UINT16/*怪id*/, SBrushMonster> monsterMap;
		SInstBrushMonster()
		{
			monsterMap.clear();
		}
	};
	//副本掉落物品
	struct SInstDropProp
	{
		std::vector<SDropPropCfgItem> props;
		SInstDropProp()
		{
			props.clear();
		}
	};
#pragma pack(pop)

public:
	//载入副本配置
	BOOL LoadInst(const char* filename);
	//载入随机副本
	BOOL LoadRandInst(const char* filename);
	//获取副本需要消耗的体力点
	int GetInstSpendVit(UINT16 instId, UINT32& spendVit);
	//获取副本等级限制
	int GetInstLvLimit(UINT16 instId, BYTE& lvLimit);
	//获取副本配置
	int GetInstConfig(UINT16 instId, SInstAttrCfg *&config);
	//获取副本基本信息
	int GetInstBasicInfo(UINT16 instId, BYTE&instType, BYTE& lvLimit, UINT32& spendVit);
	//生成随机副本id
	UINT16 GenerateRandInstId();
	//获取副本名称
	int GetInstName(UINT16 instId, std::string& instName);
	//生成宝箱掉落
	int GenerateDropBox(UINT16 instId, SInstDropBox& dropBox);
	//生成物品掉落
	int GenerateAreaMonsterDrops(UINT16 instId, BYTE areaId, SInstDropProp& dropProps, BYTE& monsterDrops, BYTE& creamDrops, BYTE& bossDrops);
	//获取战斗区域怪
	int GetBattleAreaMonster(UINT16 instId, BYTE areaId, SInstBrushMonster& brushMonster);
	//生成破碎物掉落
	int GenerateBrokenDrops(UINT16 instId, SInstDropProp& brokenDrops);
	//设置参数值
	void SetParamValue(UINT32 goldBranchThrehold, BYTE goldBranchMaxPercent, BYTE dropLv1ToBossPercent, BYTE dropLv1ToCreamPercent);
	//获取副本扫荡经验
	UINT64 GetInstSweepExp(UINT16 instId);
	//生成扫荡掉落
	int GenerateInstSweepDrops(UINT16 instId, SInstDropProp& drops);
	//获取副本所在章节
	int GetInstOwnerChapter(UINT16 instId, UINT16& chapterId);
	//获取副本类型
	BYTE GetInstType(UINT16 instId);
	//获取进入次数
	UINT16 GetInstEnterNum(UINT16 instId);
	//通过类型获取副本id
	bool GetInstByInstType(BYTE type, std::set<UINT16> *&ret);
	/* zpy 2015.12.22新增 展开掉落组 */
	void SpreadDropGroups(UINT32 dropId, BYTE dropNum, std::vector<SDropPropCfgItem> *ret);

private:
	void GoldBranch(UINT32 gold, std::vector<SDropPropCfgItem>& vec);

private:
	UINT32 m_goldBranchThrehold;	//金币分堆阀值
	BYTE   m_goldBranchMaxPercent;	//金币分堆最大百分比
	BYTE m_dropLv1ToBossPercent;	//普通物品分到boss百分比
	BYTE m_dropLv1ToCreamPercent;	//普通物品分到精英百分比

	MonsterCfg* m_pMonsterCfg;		//怪配置
	GameDropCfg* m_gameDropCfg;		//公共掉落
	std::map<UINT16/*关卡id*/, SInstAttrCfg> m_instCfg;		//关卡配置表
	std::map<UINT16/*关卡id*/, SInstAttrCfg> m_randInstCfg;	//随机关卡配置表
	std::map<BYTE, std::set<UINT16> > m_instByType;
};

class ExConfig
{
public:
	ExConfig(int id, const char* info = NULL)
	{
		printf("instId=%d,info=%s is error!", id, info);
		eid = id;
	}
	~ExConfig(){}

	int eid;
};


#endif
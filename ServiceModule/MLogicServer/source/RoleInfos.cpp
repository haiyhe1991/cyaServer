#include <math.h>
#include <limits.h>
#include "RoleInfos.h"
#include "ServiceErrorCode.h"
#include "GameCfgFileMan.h"
#include "EmailCleanMan.h"
#include "cyaMaxMin.h"
#include "ServiceProtocol.h"
#include "cyaLog.h"
#include "DailyTaskMan.h"
#include "ConsumeProduceMan.h"
#include "AchievementMan.h"
#include "ConfigFileMan.h"
#include "CashDBMan.h"
#include "RolesInfoMan.h"

static RoleInfosObjAllocator* sg_roleObjAllocator = NULL;
void InitRoleInfosObjAllocator()
{
	ASSERT(sg_roleObjAllocator == NULL);
	sg_roleObjAllocator = new RoleInfosObjAllocator();
	ASSERT(sg_roleObjAllocator != NULL);
}

RoleInfosObjAllocator* GetRoleInfosObjAllocator()
{
	return sg_roleObjAllocator;
}

void DestroyRoleInfosObjAllocator()
{
	RoleInfosObjAllocator* roleAlloctor = sg_roleObjAllocator;
	sg_roleObjAllocator = NULL;
	if (NULL != roleAlloctor)
		delete roleAlloctor;
}

RoleInfosObjAllocator::RoleInfosObjAllocator()
#if USE_MEM_POOL
: m_roleAttrPool(sizeof(SRoleInfos), 64 * 1024, 1024)
#endif
{

}

RoleInfosObjAllocator::~RoleInfosObjAllocator()
{

}

SRoleInfos* RoleInfosObjAllocator::NewRoleInfosObj()
{
#if USE_MEM_POOL
	return m_roleAttrPool.NewObj();
#else
	return new SRoleInfos();
#endif
}

void RoleInfosObjAllocator::DeleteRoleInfosObj(SRoleInfos* pInfo)
{
#if USE_MEM_POOL
	m_roleAttrPool.DeleteObj(pInfo);
#else
	delete pInfo;
#endif
}

void SRoleInfos::AutoRecoveryVit(UINT32 maxAutoVit, LTMSEL nowMsel)
{
	if (vit < maxAutoVit)
	{
		LTMSEL lastEnterMSel = LocalStrTimeToMSel(lastEnterTime);
		if (lastEnterMSel > nowMsel)	//系统改时间了 ???
			return;

		UINT16 nInterval = GetGameCfgFileMan()->GetVitRecoveryInterval();
		UINT16 nRecVit = GetGameCfgFileMan()->GetVitRecoveryVal();

		LTMSEL sec = (nowMsel - lastEnterMSel) / 1000;
		UINT32 newVit = SGSU32Add(vit, (sec / nInterval) * nRecVit);
		vit = newVit > maxAutoVit ? maxAutoVit : newVit;
	}
}

void SRoleInfos::InitBackpack(const BYTE* pData, int nDataLen)
{
	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SPropertyAttr));
	nCount = min(nCount, *(BYTE*)pData);
	SPropertyAttr* pProperty = (SPropertyAttr*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
	{
		packs.push_back(pProperty[i]);
		if (pProperty[i].id > uid)
			uid = pProperty[i].id;
	}
	++uid;
}

void SRoleInfos::InitBuffer(const BYTE* pData, int nDataLen)
{
	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SBufferInfo));
	nCount = min(nCount, *(BYTE*)pData);
	SBufferInfo* pBuffer = (SBufferInfo*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
		buffers.push_back(pBuffer[i]);
}

void SRoleInfos::InitAcceptedTasks(const BYTE* pData, int nDataLen, LTMSEL nowMsel)
{
	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SAcceptTaskInfo));
	nCount = min(nCount, *(BYTE*)pData);
	SAcceptTaskInfo* pTask = (SAcceptTaskInfo*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
	{
		TaskCfg::STaskAttrCfg* pTaskInfo = NULL;
		int retCode = GetGameCfgFileMan()->GetTaskInfo(pTask[i].taskId, pTaskInfo);
		if (retCode != MLS_OK)
			continue;

		if (!pTask[i].isFinished)
		{
			BYTE targetType = pTaskInfo->targetType;
			switch (targetType)
			{
			case ESGS_TASK_TARGET_ESCORT:			//护送
				pTask[i].target[2] = 0;	//血量
				break;

			case ESGS_TASK_TARGET_PASS_NO_HURT:	//无伤通关
				pTask[i].target[1] = 0;	//血量
				break;

			case ESGS_TASK_TARGET_LIMIT_TIME:		//限时通关
				pTask[i].target[1] = 0;	//时间
				break;

			case ESGS_TASK_TARGET_KILL_MONSTER_LIMIT_TIME:	//限时杀怪
			case ESGS_TASK_TARGET_DESTROY_SCENE_LIMIT_TIME:	//限时破坏场景物（复数）
				pTask[i].target[2] = 0;	//时间
				pTask[i].target[3] = 0;	//数量
				break;

			case ESGS_TASK_TARGET_FETCH_PROP_LIMIT_TIME:	//限时获取道具（复数）
				pTask[i].target[3] = 0;	//时间
				pTask[i].target[4] = 0;	//数量
				break;

			case ESGS_TASK_TARGET_BLOCK_TIMES:		//格挡多少次
			case ESGS_TASK_TARGET_COMBO_NUM_REACH:	//达到足够多的连击数
			case ESGS_TASK_TARGET_KEEP_BLOOD:		//血量保持
				pTask[i].target[1] = 0;	//数量
				break;

			default:
				break;
			}
		}

		BYTE type = pTaskInfo->type;
		if (type != ESGS_SYS_DAILY_TASK)		//非日常任务
			acceptTasks.push_back(pTask[i]);
		else
		{
			SYSTEMTIME lastSysTm, nowSysTm;
			LTMSEL lastEnterMSel = LocalStrTimeToMSel(lastEnterTime);
			ToTimeEx(lastEnterMSel, &lastSysTm, true);
			ToTimeEx(nowMsel, &nowSysTm, true);
			if (lastSysTm.wDay == nowSysTm.wDay && lastSysTm.wMonth == nowSysTm.wMonth && lastSysTm.wYear == nowSysTm.wYear)
				acceptTasks.push_back(pTask[i]);
		}
	}
}

void SRoleInfos::InitSkills(const BYTE* pData, int nDataLen)
{
	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SSkillMoveInfo));
	nCount = min(nCount, *(BYTE*)pData);
	SSkillMoveInfo* pSkill = (SSkillMoveInfo*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
		studySkills.push_back(pSkill[i]);
}

void SRoleInfos::InitWearEquipment(const BYTE* pData, int nDataLen)
{
	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SPropertyAttr));
	nCount = min(nCount, *(BYTE*)pData);
	SPropertyAttr* pWearEquipment = (SPropertyAttr*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
	{
		wearEquipment.push_back(pWearEquipment[i]);
		if (pWearEquipment[i].id > uid)
			uid = pWearEquipment[i].id;
	}
	++uid;
}

void SRoleInfos::InitHasDress(const BYTE* pData, int nDataLen)
{
	int nCount = (nDataLen - sizeof(UINT16)) / sizeof(UINT16);
	nCount = min(MAX_HAS_DRESS_NUM, min(nCount, *(UINT16*)pData));
	UINT16* pDressIds = (UINT16*)(pData + sizeof(UINT16));
	for (int i = 0; i < nCount; ++i)
		dress.push_back(pDressIds[i]);
}

void SRoleInfos::InitHasEquipFragment(const BYTE* pData, int nDataLen)
{
	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SEquipFragmentAttr));
	nCount = min(nCount, *(BYTE*)pData);
	SEquipFragmentAttr* pFragmentAttr = (SEquipFragmentAttr*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
		fragments.push_back(pFragmentAttr[i]);
}

void SRoleInfos::InitEquipPos(const BYTE* pData, int nDataLen)
{
	equipPos.clear();
	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SEquipPosAttr));
	nCount = min(nCount, *(BYTE*)pData);
	ASSERT(nCount == 6);
	SEquipPosAttr* pAttr = (SEquipPosAttr*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
		equipPos.push_back(pAttr[i]);
}

int SRoleInfos::GetBackpackPropAttrById(UINT32 id, SPropertyAttr& propAttr)
{
	BYTE si = (BYTE)packs.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (packs[i].id == id)
		{
			memcpy(&propAttr, &packs[i], sizeof(SPropertyAttr));
			return MLS_OK;
		}
	}
	return MLS_PROPERTY_NOT_EXIST;
}

BOOL SRoleInfos::IsExistProperty(UINT16 propId, UINT16 leastNum)
{
	UINT16 count = 0;
	BYTE si = (BYTE)packs.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (packs[i].propId != propId)
			continue;

		count = SGSU16Add(count, packs[i].num);
		if (count >= leastNum)
			return true;
	}

	return false;
}

// zpy 背包和已穿戴装备装备中是否存在某物品
BOOL SRoleInfos::IsExistInbackpackAdnWearEquipment(UINT16 propId, UINT16 leastNum)
{
	UINT16 count = 0;
	{
		BYTE size = (BYTE)packs.size();
		for (BYTE i = 0; i < size; ++i)
		{
			if (packs[i].propId != propId)
				continue;
			count = SGSU16Add(count, packs[i].num);
			if (count >= leastNum)
				return true;
		}
	}

	{
		BYTE size = (BYTE)wearEquipment.size();
		for (BYTE i = 0; i < size; ++i)
		{
			if (wearEquipment[i].propId != propId)
				continue;

			count = SGSU16Add(count, wearEquipment[i].num);
			if (count >= leastNum)
				return true;
		}
	}

	return false;
}

// zpy 初始化天梯信息
void SRoleInfos::InitLadderInfo(UINT16 win_in_row, UINT16 refresh_count, UINT16 eliminate_count, UINT16 challenge_count, LTMSEL last_finish_time, UINT32 matching_rank[LADDER_MATCHTING_MAX_NUM])
{
	this->ladder_win_in_row = win_in_row;
	this->ladder_refresh_count = refresh_count;
	this->ladder_eliminate_count = eliminate_count;
	this->ladder_challenge_count = challenge_count;
	this->ladder_last_finish_time = last_finish_time;
	for (int i = 0; i < LADDER_MATCHTING_MAX_NUM; ++i)
	{
		this->ladder_matching_rank[i] = matching_rank[i];
	}
}

// zpy 增加角色经验
SBigNumber SRoleInfos::AddRoleExp(UINT64 add_exp)
{
	// 角色等级未达到上限才增加经验
	if (level < GetGameCfgFileMan()->GetMaxRoleLevel())
	{
		SBigNumberAdd(exp, add_exp);
	}
	return exp;
}

SBigNumber SRoleInfos::AddRoleExp(SBigNumber add_exp)
{
	// 角色等级未达到上限才增加经验
	if (level < GetGameCfgFileMan()->GetMaxRoleLevel())
	{
		SBigNumberAdd(exp, add_exp);
	}
	return exp;
}

UINT16 SRoleInfos::NumberOfProperty(UINT16 propId)
{
	UINT16 count = 0;
	BYTE si = (BYTE)packs.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (packs[i].propId != propId)
			continue;

		count = SGSU16Add(count, packs[i].num);
	}

	return count;
}

UINT16 SRoleInfos::NumberOfProperty(UINT16 propId, UINT16 leastNum)
{
	UINT16 count = 0;
	BYTE si = (BYTE)packs.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (packs[i].propId != propId)
			continue;

		count = SGSU16Add(count, packs[i].num);
		if (count >= leastNum)
			return leastNum;
	}

	return count;
}

int SRoleInfos::GetBackpackProps(SPropertyAttr* pProps, BYTE maxNum, BYTE from/* = 0*/, BYTE* totals /*= NULL*/)
{
	int idx = 0;
	BYTE si = (BYTE)packs.size();

	if (maxNum > 0)
	{
		for (BYTE i = from; i < si; ++i)
		{
			memcpy(&pProps[idx], &packs[i], sizeof(SPropertyAttr));
			pProps[idx].hton();
			if (++idx >= maxNum)
				break;
		}
	}

	if (totals != NULL)
		*totals = si;
	return idx;
}

int SRoleInfos::DropBackpackProp(UINT32 id, BYTE num)
{
	for (std::vector<SPropertyAttr>::iterator it = packs.begin(); it != packs.end(); ++it)
	{
		if (it->id == id)
		{
			if (it->num > num)
				it->num -= num;
			else
				packs.erase(it);
			break;
		}
	}
	return MLS_OK;
}

int SRoleInfos::SellProperty(UINT32 id, BYTE num, SPropertyAttr& propAttr, SSellPropertyRes* pSellRes)
{
	if (num == 0)
	{
		pSellRes->gold = gold;
		return MLS_OK;
	}

	//查找背包中是否有该物品
	std::vector<SPropertyAttr>::iterator it = packs.begin();
	for (; it != packs.end(); ++it)
	{
		if (it->id == id)
		{
			/*
			* 2016.1.8 zpy 修改
			* pyg版本是将道具全部出售，并未使用客户端期望的出售数量
			*/
			if (it->num < num)
			{
				return MLS_PROPERTY_NOT_ENOUGH;
			}
			else
			{
				memcpy(&propAttr, &(*it), sizeof(SPropertyAttr));
				break;
			}
		}
	}

	//获取物品出售单价
	UINT64 price = 0;	//单价
	int retCode = GetGameCfgFileMan()->GetPropertyUserSellPrice(propAttr.type, propAttr.propId, price);
	if (retCode != MLS_OK)
		return retCode;

	if (price == 0)	//物品不允许出售
		return MLS_PROP_NOT_ALLOW_SELL;

	//增加出售所得金币
	UINT64 sellGold = price * num;
	ProduceGold(sellGold, "出售物品");
	//SBigNumberAdd(gold, sellGold);

	//从背包扣除所卖物品
	pSellRes->num = 1;
	it->num = SGSU8Sub(it->num, num);
	memcpy(&pSellRes->data[0], &(*it), sizeof(SPropertyAttr));
	if (it->num == 0)
		packs.erase(it);

	propAttr.level = 0;
	propAttr.star = 0;
	propAttr.num = num;

	return MLS_OK;
}

int SRoleInfos::BuyBackProperty(SPropertyAttr& buyProp, BYTE num, SPropBuyBackRes* pBuyBackRes)
{
	if (num == 0)
	{
		pBuyBackRes->gold = gold;
		return MLS_OK;
	}

	//获取回购价格
	UINT64 price = 0;
	int retCode = GetGameCfgFileMan()->GetPropertyUserBuyBackPrice(buyProp.type, buyProp.propId, price);
	if (retCode != MLS_OK)
		return retCode;

	UINT64 spendPrice = price * buyProp.num;
	if (BigNumberToU64(gold) < spendPrice)
		return MLS_GOLD_NOT_ENOUGH;		//金币不足

	//获取物品堆叠数量
	BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(buyProp.type, buyProp.propId);

	//购买数量
	BYTE buyNum = min(buyProp.num, num);

	//背包是否能放下
	if (!CanPutIntoBackpack(buyProp.propId, buyNum, maxStack))
		return MLS_BACKPACK_IS_FULL;

	//扣除金币
	//SBigNumberSub(gold, spendPrice);
	ConsumeGold(spendPrice, "回购物品");
	pBuyBackRes->gold = gold;

	//放入背包
	if (buyProp.type == ESGS_PROP_EQUIP)	//装备
	{
		PutBackpackSingleNumProperty(buyProp, maxStack);
		pBuyBackRes->propNum = 1;
		memcpy(&pBuyBackRes->props[0], &buyProp, sizeof(SPropertyAttr));
		pBuyBackRes->props[0].hton();
	}
	else
	{
		SPropertyAttr propAttr;
		retCode = GetGameCfgFileMan()->GetPropertyAttr(buyProp.type, buyProp.propId, propAttr, maxStack);
		if (retCode != MLS_OK)
			return retCode;

		BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SPropBuyBackRes, gold, props)) / sizeof(SPropertyAttr));
		PutBackpackProperty(buyProp.type, buyProp.propId, buyNum, maxStack, pBuyBackRes->props, maxNum, pBuyBackRes->propNum);
		// 		BYTE retNum = 0;
		// 		BYTE modifyPropNum = 0;
		// 		SConsumeProperty consumeItem[MAX_PUT_ITEM_RET_NUM];
		// 		PutBackpackProperty(buyProp.type, buyProp.propId, buyProp.num, maxStack, consumeItem, (BYTE)MAX_PUT_ITEM_RET_NUM, retNum);
		// 		BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SPropBuyBackRes, gold, props)) / sizeof(SPropertyAttr));
		// 		for(BYTE j = 0; j < retNum; ++j)
		// 		{
		// 			if(modifyPropNum >= maxNum)
		// 				break;
		// 
		// 			memcpy(&pBuyBackRes->props[modifyPropNum], &propAttr, sizeof(SPropertyAttr));
		// 			pBuyBackRes->props[modifyPropNum].type = buyProp.type;
		// 			pBuyBackRes->props[modifyPropNum].id = consumeItem[j].id;
		// 			pBuyBackRes->props[modifyPropNum].num = consumeItem[j].num;
		// 			pBuyBackRes->props[modifyPropNum].hton();
		// 			++modifyPropNum;
		// 		}
		// 
		// 		pBuyBackRes->propNum = modifyPropNum;
	}
	return MLS_OK;
}

int SRoleInfos::DropBackpackPropEx(UINT16 propId, UINT16 num)
{
	if (num <= 0)
		return MLS_OK;

	UINT16 deductNum = num;
	for (std::vector<SPropertyAttr>::iterator it = packs.begin(); it != packs.end();)
	{
		if (it->propId != propId)
		{
			++it;
			continue;
		}

		SPropertyAttr& prop = *it;
		BYTE n = SGSU8Sub(prop.num, (BYTE)deductNum);
		deductNum = SGSU16Sub(deductNum, prop.num);
		if (n == 0)
			it = packs.erase(it);
		else
		{
			prop.num = n;
			++it;
		}

		if (deductNum == 0)
			break;
	}

	return MLS_OK;
}

int SRoleInfos::PutBackpackProperty(BYTE propType, UINT16 propId, UINT16 num, BYTE maxStack, SConsumeProperty* pPutItem, BYTE maxItemNum, BYTE& retNum)
{
	ASSERT(maxStack > 0);
	if (num <= 0)
		return MLS_OK;

	/* zpy 成就系统新增 */
	GemCfg::SGemAttrCfg gem_attr;
	if (GetGameCfgFileMan()->GetGemCfgAttr(propId, gem_attr) != MLS_PROPERTY_NOT_EXIST)
	{
		GetAchievementMan()->OnGotGem(roleId, gem_attr.level);
	}

	BYTE idx = 0;
	UINT16 putNum = 0;
	BYTE si = (BYTE)packs.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (packs[i].propId != propId)
			continue;

		BYTE n = SGSU8Sub(maxStack, packs[i].num);	//该格子还能放多少
		if (n > 0)
		{
			UINT16 notPutNum = SGSU16Sub(num, putNum);	//还剩多少未放
			BYTE inPackNum = min(notPutNum, n);			//该次放入格子数量
			packs[i].num = SGSU8Add(packs[i].num, inPackNum/*n*/);
			putNum = SGSU16Add(putNum, inPackNum/*n*/);
			if (idx < maxItemNum)
			{
				pPutItem[idx].id = packs[i].id;
				pPutItem[idx].num = packs[i].num;
				++idx;
			}
		}

		if (putNum >= num)
			break;
	}

	if (putNum >= num)
	{
		retNum = idx;
		return MLS_OK;
	}

	UINT16 nFreeNum = SGSU16Sub(num, putNum);	//剩余未放数量
	BYTE nFreeSpace = SGSU8Sub(packSize, si);	//背包剩余格子数量
	BYTE nStack = 0;

	//获取物品属性
	SPropertyAttr propAttr;
	memset(&propAttr, 0, sizeof(SPropertyAttr));
	int retCode = GetGameCfgFileMan()->GetPropertyAttr(propType, propId, propAttr, nStack);
	if (retCode != MLS_OK)
		return retCode;
	propAttr.type = propType;

	while (nFreeNum > 0 && nFreeSpace > 0 /*&& idx < maxItemNum*/)
	{
		propAttr.num = min(maxStack, nFreeNum);
		nFreeNum = SGSU16Sub(nFreeNum, propAttr.num);
		propAttr.id = uid++;	//wearEquipment.size() + packs.size() + 1;
		packs.push_back(propAttr);
		nFreeSpace--;

		if (idx >= maxItemNum)
			continue;

		pPutItem[idx].id = propAttr.id;
		pPutItem[idx].num = propAttr.num;
		++idx;
	}
	retNum = idx;
	return MLS_OK;
}

int SRoleInfos::PutBackpackProperty(BYTE propType, UINT16 propId, UINT16 num, BYTE maxStack, SPropertyAttr* pBackpackMod, BYTE maxModNum, BYTE& retNum)
{
	ASSERT(maxStack > 0);
	if (num <= 0)
		return MLS_OK;

	/* zpy 成就系统新增 */
	GemCfg::SGemAttrCfg gem_attr;
	if (GetGameCfgFileMan()->GetGemCfgAttr(propId, gem_attr) != MLS_PROPERTY_NOT_EXIST)
	{
		GetAchievementMan()->OnGotGem(roleId, gem_attr.level);
	}

	BYTE idx = 0;
	UINT16 putNum = 0;
	BYTE si = (BYTE)packs.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (packs[i].propId != propId)
			continue;

		BYTE n = SGSU8Sub(maxStack, packs[i].num);	//该格子还能放多少
		if (n > 0)
		{
			UINT16 notPutNum = SGSU16Sub(num, putNum);	//还剩多少未放
			BYTE inPackNum = min(notPutNum, n);			//该次放入格子数量
			packs[i].num = SGSU8Add(packs[i].num, inPackNum/*n*/);
			putNum = SGSU16Add(putNum, inPackNum/*n*/);
			if (idx < maxModNum)
			{
				memcpy(&pBackpackMod[idx], &packs[i], sizeof(SPropertyAttr));
				++idx;
			}
		}

		if (putNum >= num)
			break;
	}

	if (putNum >= num)
	{
		retNum = idx;
		return MLS_OK;
	}

	UINT16 nFreeNum = SGSU16Sub(num, putNum);	//剩余未放数量
	BYTE nFreeSpace = SGSU8Sub(packSize, si);	//背包剩余格子数量
	BYTE nStack = 0;

	//获取物品属性
	SPropertyAttr propAttr;
	memset(&propAttr, 0, sizeof(SPropertyAttr));
	int retCode = GetGameCfgFileMan()->GetPropertyAttr(propType, propId, propAttr, nStack);
	if (retCode != MLS_OK)
		return retCode;
	propAttr.type = propType;

	while (nFreeNum > 0 && nFreeSpace > 0 /*&& idx < maxItemNum*/)
	{
		propAttr.num = min(maxStack, nFreeNum);
		nFreeNum = SGSU16Sub(nFreeNum, propAttr.num);
		propAttr.id = uid++;	//wearEquipment.size() + packs.size() + 1;
		packs.push_back(propAttr);
		nFreeSpace--;

		if (idx >= maxModNum)
			continue;

		memcpy(&pBackpackMod[idx], &propAttr, sizeof(SPropertyAttr));
		++idx;
	}
	retNum = idx;
	return MLS_OK;
}

int SRoleInfos::DropBackpackPropEx(UINT16 propId, UINT16 num, SConsumeProperty* pConsumeItem, BYTE maxItemNum, BYTE& retNum)
{
	BYTE idx = 0;
	UINT16 deductNum = num;
	if (deductNum <= 0)
		return MLS_OK;

	for (std::vector<SPropertyAttr>::iterator it = packs.begin(); it != packs.end();)
	{
		if (it->propId != propId)
		{
			++it;
			continue;
		}

		SPropertyAttr& prop = *it;
		BYTE n = SGSU8Sub(prop.num, (BYTE)deductNum);
		deductNum = SGSU16Sub(deductNum, prop.num);
		if (n == 0)
		{
			if (idx < maxItemNum && pConsumeItem != NULL)
			{
				pConsumeItem[idx].id = prop.id;
				pConsumeItem[idx].num = 0;
				++idx;
			}

			it = packs.erase(it);
		}
		else
		{
			prop.num = n;
			if (idx < maxItemNum && pConsumeItem != NULL)
			{
				pConsumeItem[idx].id = prop.id;
				pConsumeItem[idx].num = n;
				++idx;
			}
			++it;
		}

		if (deductNum == 0)
			break;
	}

	retNum = idx;
	return MLS_OK;
}

int SRoleInfos::DropBackpackPropEx(UINT16 propId, UINT16 num, SPropertyAttr* pBackpackMod, BYTE maxModNum, BYTE& retNum)
{
	BYTE idx = 0;
	UINT16 deductNum = num;
	if (deductNum <= 0)
		return MLS_OK;

	for (std::vector<SPropertyAttr>::iterator it = packs.begin(); it != packs.end();)
	{
		if (it->propId != propId)
		{
			++it;
			continue;
		}

		SPropertyAttr& prop = *it;
		BYTE n = SGSU8Sub(prop.num, (BYTE)deductNum);
		deductNum = SGSU16Sub(deductNum, prop.num);
		if (n == 0)
		{
			if (idx < maxModNum && pBackpackMod != NULL)
			{
				memcpy(&pBackpackMod[idx], &prop, sizeof(SPropertyAttr));
				pBackpackMod[idx].num = 0;
				++idx;
			}

			it = packs.erase(it);
		}
		else
		{
			prop.num = n;
			if (idx < maxModNum && pBackpackMod != NULL)
			{
				memcpy(&pBackpackMod[idx], &prop, sizeof(SPropertyAttr));
				pBackpackMod[idx].num = n;
				++idx;
			}
			++it;
		}

		if (deductNum == 0)
			break;
	}

	retNum = idx;
	return MLS_OK;
}

BOOL SRoleInfos::IsHasMagic(UINT16 magicId)
{
	BYTE n = (BYTE)magics.size();
	for (BYTE i = 0; i < n; ++i)
	{
		if (magics[i] == magicId)
			return true;
	}

	return false;
}

BOOL SRoleInfos::IsHasDress(UINT16 dressId)
{
	int n = (int)dress.size();
	for (int i = 0; i < n; ++i)
	{
		if (dress[i] == dressId)
			return true;
	}

	return false;
}

UINT16 SRoleInfos::GetFragmentNum(UINT16 fragId)
{
	int n = (int)fragments.size();
	for (int i = 0; i < n; ++i)
	{
		if (fragments[i].fragmentId == fragId)
			return fragments[i].num;
	}

	return 0;
}

void SRoleInfos::DropFragment(UINT16 fragId, UINT16 num)
{
	std::vector<SEquipFragmentAttr>::iterator it = fragments.begin();
	for (; it != fragments.end(); ++it)
	{
		if (it->fragmentId != fragId)
			continue;

		if (it->num <= num)
			fragments.erase(it);
		else
			it->num = SGSU16Sub(it->num, num);
		break;
	}
}

void SRoleInfos::DropMagic(UINT16 magicId)
{
	std::vector<UINT16>::iterator it = magics.begin();
	for (; it != magics.end(); ++it)
	{
		if (*it == dressId)
		{
			magics.erase(it);
			break;
		}
	}
}

void SRoleInfos::DropDress(UINT16 dressId)
{
	std::vector<UINT16>::iterator it = dress.begin();
	for (; it != dress.end(); ++it)
	{
		if (*it == dressId)
		{
			dress.erase(it);
			break;
		}
	}
}

BOOL SRoleInfos::CanPutIntoBackpack(UINT16 propId, UINT16 num, BYTE maxStack)
{
	UINT16 putNum = 0;
	BOOL canPut = false;
	BYTE si = (BYTE)packs.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (packs[i].propId != propId)	//物品id相等
			continue;

		BYTE inNum = SGSU8Sub(maxStack, packs[i].num);
		putNum = SGSU16Add(putNum, inNum);
		if (putNum >= num)
		{
			canPut = true;
			break;
		}
	}

	if (canPut)
		return canPut;

	BYTE nFreeSpace = SGSU8Sub(packSize, si);
	if (nFreeSpace <= 0)	//大于背包最大数量
		return false;

	UINT16 nFreeNum = SGSU16Sub(num, putNum);
	BYTE n = nFreeNum / maxStack;

	if (n > nFreeSpace)
		canPut = false;
	else if (n < nFreeSpace)
		canPut = true;
	else
		canPut = nFreeNum % maxStack > 0 ? false : true;

	return canPut;
}

void SRoleInfos::InitNewRoleGiveItems(UINT32 userID)
{
	InitRoleGiveCfg::SInitRoleGiveItem giveItem;
	if (!GetGameCfgFileMan()->GetRoleInitGiveItem(giveItem))
		return;



	//测试时赠送 add by hxw 20151210
	BOOL bGive = FALSE;
	if (!GetConfigFileMan()->IsRelease())
	{
		//第一个角色赠送
		if (0 == GetRolesInfoMan()->QueryUserRolesNum(userID))
			bGive = TRUE;
	}
	// add by hxw 20151210 && bGive
	if (giveItem.money.gold > 0 && bGive)
		U64ToBigNumber(giveItem.money.gold, gold);

	if (giveItem.money.rpcoin > 0 && bGive)
	{
		//add by hxw 20151120
		if (GetConfigFileMan()->GetCashSaveType() == CASH_SAVE_TYPE_TO_CASHDB)
		{
			ASSERT(userID != 0);
			GetCashDBMan()->AddCash(userID, giveItem.money.rpcoin);
		}
		else
		{
			cash = giveItem.money.rpcoin;
		}
		//end		
	}

	if (giveItem.other.exp>0 && bGive)
	{
		//U64ToBigNumber(giveItem.other.exp,exp);
		AddRoleExp(giveItem.other.exp);
		BYTE lv = GetGameCfgFileMan()->GetRoleLevelByExp(occuId, giveItem.other.exp);
		if (lv > level)
		{
			level = lv;
			GetGameCfgFileMan()->GetRoleAllInitSkills(occuId, level, lv, studySkills);
		}
	}
	BYTE maxStack = 0;
	BYTE si = 0;
	if (bGive)
	{
		si = (BYTE)giveItem.propVec.size();
		for (BYTE i = 0; i < si&& bGive; ++i)
		{
			BYTE propType = giveItem.propVec[i].type;
			UINT32 num = giveItem.propVec[i].num;
			UINT16 propId = giveItem.propVec[i].propId;

			if (propType == ESGS_PROP_FRAGMENT)		//装备碎片
				AddEquipFragment(propId, num);
			else if (propType == ESGS_PROP_CHAIN_SOUL_FRAGMENT)	//炼魂碎片
				AddChainSoulFragment(propId, num);
			else if (propType == ESGS_PROP_DRESS)	//时装
				dress.push_back(propId);
			else if (propType == ESGS_PROP_MAGIC)	//魔导器
				magics.push_back(propId);
			else									//背包物品
			{
				SPropertyAttr propAttr;
				memset(&propAttr, 0, sizeof(SPropertyAttr));
				int retCode = GetGameCfgFileMan()->GetPropertyAttr(propType, propId, propAttr, maxStack);
				if (retCode != MLS_OK)
					continue;

				while (num > 0)
				{
					propAttr.num = min(maxStack, (BYTE)num);
					propAttr.id = uid++;
					packs.push_back(propAttr);
					num = SGSU8Sub(num, propAttr.num);
				}
			}
		}
	}

	//穿戴装备
	si = (BYTE)giveItem.wearEquipVec.size();
	for (BYTE i = 0; i < si; ++i)
	{
		SPropertyAttr propAttr;
		memset(&propAttr, 0, sizeof(SPropertyAttr));
		int retCode = GetGameCfgFileMan()->GetEquipPropAttr(giveItem.wearEquipVec[i], propAttr, maxStack);
		if (retCode == MLS_OK)
		{
			propAttr.num = 1;
			propAttr.type = ESGS_PROP_EQUIP;
			propAttr.id = uid++;
			wearEquipment.push_back(propAttr);
		}
	}
}

void SRoleInfos::InitNewRoleEmail(UINT32 userId, UINT32 roleId)
{
	EmailCleanMan::SSyncEmailSendContent email;
	email.receiverRoleId = roleId;
	email.receiverUserId = userId;
	email.pGBSenderNick = "系统";	//发送者昵称

	std::vector<EmailCfg::SNewPlayerEmailItem> giveItem;
	int retCode = GetGameCfgFileMan()->GetNewPlayerEmailGiveProps(giveItem);
	if (retCode != MLS_OK)
		return;

	int n = (int)giveItem.size();
	for (int i = 0; i < n; ++i)
	{
		email.type = ESGS_EMAIL_NEW_ACTOR_REWARD;
		email.gold = giveItem[i].gold;
		email.rpcoin = giveItem[i].rpcoin;
		email.exp = giveItem[i].exp;
		email.vit = giveItem[i].vit;
		email.pGBBody = giveItem[i].strBody.c_str();
		email.pGBCaption = giveItem[i].strTitle.c_str();

		BYTE maxStack = 0;
		std::vector<SPropertyAttr> propVec;
		std::vector<SDropPropCfgItem>& giveProps = giveItem[i].props;
		BYTE si = (BYTE)giveProps.size();
		for (BYTE j = 0; j < si; ++j)
		{
			BYTE propType = giveProps[j].type;
			UINT32 num = giveProps[j].num;
			UINT16 propId = giveProps[j].propId;

			SPropertyAttr propAttr;
			memset(&propAttr, 0, sizeof(SPropertyAttr));
			if (propType == ESGS_PROP_EQUIP || propType == ESGS_PROP_GEM || propType == ESGS_PROP_MATERIAL)
			{
				retCode = GetGameCfgFileMan()->GetPropertyAttr(propType, propId, propAttr, maxStack);
				if (retCode != MLS_OK)
					continue;
				propAttr.num = min(maxStack, num);
			}
			else
			{
				propAttr.propId = propId;
				propAttr.num = num;
				propAttr.type = propType;
			}
			propVec.push_back(propAttr);
		}
		email.attachmentsNum = (BYTE)propVec.size();
		email.pAttachments = propVec.data();
		GetEmailCleanMan()->AsyncSendEmail(email);
	}
}

int SRoleInfos::GetWearEquipmentProps(SPropertyAttr* pProps, BYTE maxNum, BYTE from/* = 0*/, BYTE* totals /*= NULL*/)
{
	int idx = 0;
	BYTE si = (BYTE)wearEquipment.size();
	for (BYTE i = from; i < si; ++i)
	{
		memcpy(&pProps[idx], &wearEquipment[i], sizeof(SPropertyAttr));
		pProps[idx].hton();
		if (++idx >= maxNum)
			break;
	}

	if (totals != NULL)
		*totals = si;
	return idx;
}

int SRoleInfos::GetWearEquipmentProp(UINT32 id, SPropertyAttr& propAttr)
{
	int retCode = MLS_PROPERTY_NOT_EXIST;
	BYTE si = (BYTE)wearEquipment.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (wearEquipment[i].id == id)
		{
			memcpy(&propAttr, &wearEquipment[i], sizeof(SPropertyAttr));
			retCode = MLS_OK;
			break;
		}
	}
	return retCode;
}

int SRoleInfos::EquipmentCompose(UINT16 composeId, SEquipComposeRes* pComposeRes)
{
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SEquipComposeRes, gold, data)) / sizeof(SPropertyAttr));
	EquipComposeCfg::SEquipComposeItem* pComposeItem = NULL;
	int retCode = GetGameCfgFileMan()->GetEquipComposeItem(composeId, pComposeItem);
	if (retCode != MLS_OK)
		return retCode;

	//金币是否足够
	if (BigNumberToU64(gold) < pComposeItem->consumeGold)
		return MLS_GOLD_NOT_ENOUGH;

	//是否满足条件
	std::vector<EquipComposeCfg::SComposeConditionItem>& conditionVec = pComposeItem->conditionVec;
	int n = (int)conditionVec.size();
	for (int i = 0; i < n; ++i)
	{
		BYTE tp = conditionVec[i].type;
		switch (tp)
		{
		case ESGS_EQUIP_COMPOSE_ROLE_LEVEL:
			retCode = (level >= conditionVec[i].item.lv.minLv && level <= conditionVec[i].item.lv.maxLv) ? MLS_OK : MLS_ROLE_LV_OUT_RANGE;
			break;
		case ESGS_EQUIP_COMPOSE_DATE_TIME:
		{
											 LTMSEL nowMSel = GetMsel();
											 retCode = (nowMSel >= conditionVec[i].item.tm.start && nowMSel <= conditionVec[i].item.tm.end) ? MLS_OK : MLS_DATETIME_OUT_RANGE;
		}
			break;
		case ESGS_EQUIP_COMPOSE_MAGIC:
			retCode = IsHasMagic(conditionVec[i].item.magic.magicId) ? MLS_OK : MLS_NOT_HAS_MAGIC;
			break;
		case ESGS_EQUIP_COMPOSE_PROP:
		{
										for (BYTE j = 0; j < conditionVec[i].item.prop.num; ++j)
										{
											if (conditionVec[i].item.prop.pHasProp[j].type == ESGS_PROP_FRAGMENT)
											{
												UINT16 num = GetFragmentNum(conditionVec[i].item.prop.pHasProp[j].propId);
												if (num < conditionVec[i].item.prop.pHasProp[j].num)
												{
													retCode = MLS_MATERIAL_NOT_ENOUGH;
													break;
												}
											}
											else
											{
												if (!IsExistInbackpackAdnWearEquipment(conditionVec[i].item.prop.pHasProp[j].propId, conditionVec[i].item.prop.pHasProp[j].num))
												{
													retCode = MLS_MATERIAL_NOT_ENOUGH;
													break;
												}
											}
										}
		}
			break;
		default:
			break;
		}
	}

	if (retCode != MLS_OK)
		return retCode;

	//背包是否满
	std::vector<SDropPropCfgItem>& compsoePropVec = pComposeItem->compsoePropVec;
	n = (int)compsoePropVec.size();
	for (int i = 0; i < n; ++i)
	{
		BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(compsoePropVec[i].type, compsoePropVec[i].propId);
		if (!CanPutIntoBackpack(compsoePropVec[i].propId, compsoePropVec[i].num, maxStack))
		{
			retCode = MLS_BACKPACK_IS_FULL;
			break;
		}
	}
	if (retCode != MLS_OK)
		return retCode;

	//所需材料是否足够
	std::vector<SDropPropCfgItem>& basicConsumePropVec = pComposeItem->basicConsumePropVec;
	{
		//基本材料必定为装备，并且只会有一个
		if (basicConsumePropVec.size() != 1 && basicConsumePropVec.front().type != ESGS_PROP_EQUIP)
		{
			return MLS_NOT_ALLOW_COMPOSE;
		}

		//背包和已穿戴装备中是否有此装备
		if (!IsExistInbackpackAdnWearEquipment(basicConsumePropVec.front().propId, basicConsumePropVec.front().num))
		{
			return MLS_MATERIAL_NOT_ENOUGH;
		}

		//额外材料必定为装备碎片
		std::vector<SDropPropCfgItem>& additionalConsumePropVec = pComposeItem->additionalConsumePropVec;
		n = (int)additionalConsumePropVec.size();
		for (int i = 0; i < n; ++i)
		{
			if (additionalConsumePropVec[i].type == ESGS_PROP_FRAGMENT)
			{
				if (GetFragmentNum(additionalConsumePropVec[i].propId) < additionalConsumePropVec[i].num)
				{
					return MLS_MATERIAL_NOT_ENOUGH;
				}
			}
			else
			{
				return MLS_NOT_ALLOW_COMPOSE;
			}
		}
	}

	BYTE retNum = 0;
	BYTE packModNum = 0;
	bool fromBackpack = NumberOfProperty(basicConsumePropVec.front().propId) > 0;

	// 给予装备
	if (fromBackpack)
	{
		//合成装备放进背包，必定只有一个
		BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(compsoePropVec.front().type, compsoePropVec.front().propId);
		PutBackpackProperty(compsoePropVec.front().type, compsoePropVec.front().propId, compsoePropVec.front().num, maxStack,
			pComposeRes->data + packModNum, SGSU8Sub(maxNum, packModNum), retNum);
		packModNum = SGSU8Add(packModNum, retNum);
	}
	else
	{
		//合成装备替换原来的装备
		BYTE maxStack = 0;
		SPropertyAttr propAttr;
		memset(&propAttr, 0, sizeof(SPropertyAttr));
		int retCode = GetGameCfgFileMan()->GetEquipPropAttr(compsoePropVec.front().propId, propAttr, maxStack);
		if (retCode == MLS_OK)
		{
			propAttr.id = uid++;
			propAttr.num = compsoePropVec.front().num;
			ReplaceEquipment(basicConsumePropVec.front().propId, propAttr);
			pComposeRes->equip = propAttr;
		}
		else
		{
			return MLS_COMPOSE_FAILED;
		}
	}

	//扣除消耗
	{
		//扣除基本材料，必定是装备，必定只有一个	
		if (fromBackpack)
		{
			//从背包中扣除
			DropBackpackPropEx(basicConsumePropVec.front().propId, basicConsumePropVec.front().num,
				pComposeRes->data + packModNum, SGSU8Sub(maxNum, packModNum), retNum);
			packModNum = SGSU8Add(packModNum, retNum);
		}

		//扣除额外材料消耗
		std::vector<SDropPropCfgItem>& additionalConsumePropVec = pComposeItem->additionalConsumePropVec;
		n = (int)additionalConsumePropVec.size();
		for (int i = 0; i < n; ++i)
		{
			DropFragment(additionalConsumePropVec[i].propId, additionalConsumePropVec[i].num);
		}

		//扣除金币
		ConsumeGold(pComposeItem->consumeGold, "合成消耗");
	}

	pComposeRes->gold = gold;
	pComposeRes->num = min(maxNum, packModNum);
	pComposeRes->replace = fromBackpack == false;

	return retCode;

	/* zpy 2015.10.9注释
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SEquipComposeRes, gold, data)) / sizeof(SPropertyAttr));
	EquipComposeCfg::SEquipComposeItem* pComposeItem = NULL;
	int retCode = GetGameCfgFileMan()->GetEquipComposeItem(composeId, pComposeItem);
	if(retCode != MLS_OK)
	return retCode;

	//金币是否足够
	if(BigNumberToU64(gold) < pComposeItem->consumeGold)
	return MLS_GOLD_NOT_ENOUGH;

	//是否满足条件
	std::vector<EquipComposeCfg::SComposeConditionItem>& conditionVec = pComposeItem->conditionVec;
	int n = (int)conditionVec.size();
	for(int i = 0; i < n; ++i)
	{
	BYTE tp = conditionVec[i].type;
	switch(tp)
	{
	case ESGS_EQUIP_COMPOSE_ROLE_LEVEL:
	retCode = (level >= conditionVec[i].item.lv.minLv && level <= conditionVec[i].item.lv.maxLv) ? MLS_OK : MLS_ROLE_LV_OUT_RANGE;
	break;
	case ESGS_EQUIP_COMPOSE_DATE_TIME:
	{
	LTMSEL nowMSel = GetMsel();
	retCode = (nowMSel >= conditionVec[i].item.tm.start && nowMSel <= conditionVec[i].item.tm.end) ? MLS_OK : MLS_DATETIME_OUT_RANGE;
	}
	break;
	case ESGS_EQUIP_COMPOSE_MAGIC:
	retCode = IsHasMagic(conditionVec[i].item.magic.magicId) ? MLS_OK : MLS_NOT_HAS_MAGIC;
	break;
	case ESGS_EQUIP_COMPOSE_PROP:
	{
	for(BYTE j = 0; j < conditionVec[i].item.prop.num; ++j)
	{
	if(conditionVec[i].item.prop.pHasProp[j].type == ESGS_PROP_FRAGMENT)
	{
	UINT16 num = GetFragmentNum(conditionVec[i].item.prop.pHasProp[j].propId);
	if(num < conditionVec[i].item.prop.pHasProp[j].num)
	{
	retCode = MLS_MATERIAL_NOT_ENOUGH;
	break;
	}
	}
	else
	{
	if(!IsExistProperty(conditionVec[i].item.prop.pHasProp[j].propId, conditionVec[i].item.prop.pHasProp[j].num))
	{
	retCode = MLS_MATERIAL_NOT_ENOUGH;
	break;
	}
	}
	}
	}
	break;
	default:
	break;
	}
	}

	if(retCode != MLS_OK)
	return retCode;

	//背包是否满
	std::vector<SDropPropCfgItem>& compsoePropVec = pComposeItem->compsoePropVec;
	n = (int)compsoePropVec.size();
	for(int i = 0; i < n; ++i)
	{
	BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(compsoePropVec[i].type, compsoePropVec[i].propId);
	if(!CanPutIntoBackpack(compsoePropVec[i].propId, compsoePropVec[i].num, maxStack))
	{
	retCode = MLS_BACKPACK_IS_FULL;
	break;
	}
	}
	if(retCode != MLS_OK)
	return retCode;

	//所需材料是否足够
	std::vector<SDropPropCfgItem>& basicConsumePropVec = pComposeItem->basicConsumePropVec;
	n = (int)basicConsumePropVec.size();
	for(int i = 0; i < n; ++i)
	{
	BYTE propType = basicConsumePropVec[i].type;
	if(propType == ESGS_PROP_EQUIP || propType == ESGS_PROP_GEM || propType == ESGS_PROP_MATERIAL)
	{
	if(!IsExistProperty(basicConsumePropVec[i].propId, basicConsumePropVec[i].num))
	{
	retCode = MLS_MATERIAL_NOT_ENOUGH;
	break;
	}
	}
	else if(propType == ESGS_PROP_FRAGMENT)
	{
	if(GetFragmentNum(basicConsumePropVec[i].propId) < basicConsumePropVec[i].num)
	{
	retCode = MLS_MATERIAL_NOT_ENOUGH;
	break;
	}
	}
	else if(propType == ESGS_PROP_MAGIC)
	{
	if(!IsHasMagic(basicConsumePropVec[i].propId))
	{
	retCode = MLS_MATERIAL_NOT_ENOUGH;
	break;
	}
	}
	else if(propType == ESGS_PROP_DRESS)
	{
	if(!IsHasDress(basicConsumePropVec[i].propId))
	{
	retCode = MLS_MATERIAL_NOT_ENOUGH;
	break;
	}
	}
	}
	if(retCode != MLS_OK)
	return retCode;

	std::vector<SDropPropCfgItem>& additionalConsumePropVec = pComposeItem->additionalConsumePropVec;
	n = (int)additionalConsumePropVec.size();
	for(int i = 0; i < n; ++i)
	{
	BYTE propType = additionalConsumePropVec[i].type;
	if(propType == ESGS_PROP_EQUIP || propType == ESGS_PROP_GEM || propType == ESGS_PROP_MATERIAL)
	{
	if(!IsExistProperty(additionalConsumePropVec[i].propId, additionalConsumePropVec[i].num))
	{
	retCode = MLS_MATERIAL_NOT_ENOUGH;
	break;
	}
	}
	else if(propType == ESGS_PROP_FRAGMENT)
	{
	if(GetFragmentNum(additionalConsumePropVec[i].propId) < additionalConsumePropVec[i].num)
	{
	retCode = MLS_MATERIAL_NOT_ENOUGH;
	break;
	}
	}
	else if(propType == ESGS_PROP_MAGIC)
	{
	if(!IsHasMagic(additionalConsumePropVec[i].propId))
	{
	retCode = MLS_MATERIAL_NOT_ENOUGH;
	break;
	}
	}
	else if(propType == ESGS_PROP_DRESS)
	{
	if(!IsHasDress(additionalConsumePropVec[i].propId))
	{
	retCode = MLS_MATERIAL_NOT_ENOUGH;
	break;
	}
	}
	}
	if(retCode != MLS_OK)
	return retCode;

	//扣除消耗
	BYTE retNum = 0;
	BYTE packModNum = 0;
	n = (int)basicConsumePropVec.size();
	for(int i = 0; i < n; ++i)
	{
	BYTE propType = basicConsumePropVec[i].type;
	if(propType == ESGS_PROP_EQUIP || propType == ESGS_PROP_GEM || propType == ESGS_PROP_MATERIAL)
	{
	DropBackpackPropEx(basicConsumePropVec[i].propId, basicConsumePropVec[i].num,
	pComposeRes->data + packModNum, SGSU8Sub(maxNum, packModNum), retNum);
	packModNum = SGSU8Add(packModNum, retNum);
	}
	else if(propType == ESGS_PROP_FRAGMENT)
	DropFragment(basicConsumePropVec[i].propId, basicConsumePropVec[i].num);
	else if(propType == ESGS_PROP_MAGIC)
	DropMagic(basicConsumePropVec[i].propId);
	else if(propType == ESGS_PROP_DRESS)
	DropDress(basicConsumePropVec[i].propId);
	}

	n = (int)additionalConsumePropVec.size();
	for(int i = 0; i < n; ++i)
	{
	BYTE propType = basicConsumePropVec[i].type;
	if(propType == ESGS_PROP_EQUIP || propType == ESGS_PROP_GEM || propType == ESGS_PROP_MATERIAL)
	{
	DropBackpackPropEx(additionalConsumePropVec[i].propId, additionalConsumePropVec[i].num,
	pComposeRes->data + packModNum, SGSU8Sub(maxNum, packModNum), retNum);
	packModNum = SGSU8Add(packModNum, retNum);
	}
	else if(propType == ESGS_PROP_FRAGMENT)
	DropFragment(additionalConsumePropVec[i].propId, additionalConsumePropVec[i].num);
	else if(propType == ESGS_PROP_MAGIC)
	DropMagic(additionalConsumePropVec[i].propId);
	else if(propType == ESGS_PROP_DRESS)
	DropDress(additionalConsumePropVec[i].propId);
	}

	//合成装备放进背包
	n = (int)compsoePropVec.size();
	for(int i = 0; i < n; ++i)
	{
	BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(compsoePropVec[i].type, compsoePropVec[i].propId);
	PutBackpackProperty(compsoePropVec[i].type, compsoePropVec[i].propId, compsoePropVec[i].num, maxStack,
	pComposeRes->data + packModNum, SGSU8Sub(maxNum, packModNum), retNum);
	packModNum = SGSU8Add(packModNum, retNum);
	}
	pComposeRes->num = min(maxNum, packModNum);

	//扣除金币
	//SBigNumberSub(gold, pComposeItem->consumeGold);
	ConsumeGold(pComposeItem->consumeGold, "合成消耗");
	pComposeRes->gold = gold;

	return retCode;
	*/
}

int SRoleInfos::LoadEquipment(UINT32 id, BYTE pos)
{
#if 1
	BYTE si = (BYTE)wearEquipment.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (wearEquipment[i].pos == pos)
			return MLS_ALREADY_WEAR_EQUIP;
	}

	std::vector<SPropertyAttr>::iterator it = packs.begin();
	for (; it != packs.end(); ++it)
	{
		if (it->id == id)
			break;
	}

	if (it == packs.end())
		return MLS_PROPERTY_NOT_EXIST;

	if (it->type != ESGS_PROP_EQUIP || it->pos != pos)
		return MLS_WEAR_POS_NOT_MATCH;

	//by hxw 20150901
	if (this->level<it->level)
		return MLS_WEAR_LV_LIMIT;
	//end

	//放进已穿戴设备
	wearEquipment.push_back(*it);

	//移除背包
	packs.erase(it);

	//重新计算战力
	CalcRoleCombatPower();
	return MLS_OK;
#else
	std::vector<SPropertyAttr>::iterator it = packs.begin();
	for (; it != packs.end(); ++it)
	{
		if (it->id == id)
			break;
	}

	if (it == packs.end())
		return MLS_PROPERTY_NOT_EXIST;

	if (it->type != ESGS_PROP_EQUIP || it->pos != pos)
		return MLS_WEAR_POS_NOT_MATCH;

	//若该部位已经穿戴则将先前的装备覆盖掉
	std::vector<SPropertyAttr>::iterator it2 = wearEquipment.begin();
	for (; it2 != wearEquipment.end(); ++it2)
	{
		if (it2->type == ESGS_PROP_EQUIP && it2->pos == pos)
		{
			wearEquipment.erase(it2);
			break;
		}
	}

	if (it2 != wearEquipment.end())
		packs.push_back(*it2);	//把先前穿戴的装备放进背包(装备不叠加,直接放进去)

	//放进已穿戴设备
	wearEquipment.push_back(*it);

	//从背包中移除穿戴装备(装备不叠加,直接移除)
	packs.erase(it);

	//重新计算战力
	CalcRoleCombatPower();
	return MLS_OK;
#endif
}

int SRoleInfos::UnLoadEquipment(UINT32 id)
{
	if (packs.size() >= packSize)
		return MLS_BACKPACK_IS_FULL;

	std::vector<SPropertyAttr>::iterator it = wearEquipment.begin();
	for (; it != wearEquipment.end(); ++it)
	{
		if (it->id == id && it->type == ESGS_PROP_EQUIP)
		{
			packs.push_back(*it);		//放进背包
			wearEquipment.erase(it);	//移除已穿戴装备
			break;
		}
	}

	//重新计算战力
	CalcRoleCombatPower();
	return MLS_OK;
}

//zpy 替换装备
int SRoleInfos::ReplaceEquipment(UINT32 id, const SPropertyAttr& propAttr)
{
	bool find = false;
	std::vector<SPropertyAttr>::iterator it = wearEquipment.begin();
	for (; it != wearEquipment.end(); ++it)
	{
		if (it->propId == id && it->type == ESGS_PROP_EQUIP)
		{
			*it = propAttr;
			find = true;
			break;
		}
	}

	ASSERT(find);

	//重新计算战力
	CalcRoleCombatPower();
	return MLS_OK;
}

int SRoleInfos::EquipPosInlaidGem(BYTE pos, UINT32 gemId, BYTE holeSeq)
{
	if (holeSeq > ESGS_GEM_HOLE_3)
		return MLS_GEM_HOLE_INVALID;

	GemCfg::SEquipGemAttr equipInlaidattr;
	int retCode = GetGameCfgFileMan()->GetEquipInlaidGemAttr(pos, holeSeq, equipInlaidattr);
	if (retCode != MLS_OK)
		return retCode;

	BYTE strengthenLv = 0;
	int idx = -1;
	BYTE n = (BYTE)equipPos.size();
	for (BYTE i = 0; i < n; ++i)
	{
		if (equipPos[i].pos == pos)
		{
			idx = i;
			strengthenLv = equipPos[i].lv;
			break;
		}
	}

	if (idx < 0)
		return MLS_UNKNOWN_EQUIP_POS;

	//是否低于强化等级限制
	if (strengthenLv < equipInlaidattr.equipLimitlv)
		return MLS_GEM_HOLE_NOT_OPEN;

	//该孔是否已镶嵌过宝石

	if (equipPos[idx].gem[holeSeq] > 0)	//该孔镶嵌过
		return MLS_GEM_HOLE_IS_INLAID;

	//背包中查找宝石
	SPropertyAttr* pGemAttr = NULL;
	std::vector<SPropertyAttr>::iterator it = packs.begin();
	for (; it != packs.end(); ++it)
	{
		if (it->id == gemId && it->type == ESGS_PROP_GEM)
		{
			pGemAttr = &(*it);
			break;
		}
	}

	if (pGemAttr == NULL)
		return MLS_PROPERTY_NOT_EXIST;

	//宝石片不能镶嵌
	if (pGemAttr->level == 0)
		return MLS_GEM_SLICE_NO_INLAID;

	//宝石类型和装备需要镶嵌的类型不一致
	if (pGemAttr->grade != equipInlaidattr.gemType)
		return MLS_GEM_AND_HOLE_NOT_MATCH;

	//获取宝石镶嵌消耗金币(根据宝石等级)
	GemCfg::SGemConsumeCfg spend;
	retCode = GetGameCfgFileMan()->GetGemSpendByLevel(pGemAttr->level, spend);
	if (retCode != MLS_OK)
		return retCode;

	if (BigNumberToU64(gold) < spend.loadGold)
		return MLS_GOLD_NOT_ENOUGH;

	equipPos[idx].gem[holeSeq] = pGemAttr->propId;	//镶嵌宝石id

	//扣除金币
	if (spend.loadGold > 0)
		ConsumeGold(spend.loadGold, "宝石镶嵌");
	//SBigNumberSub(gold, spend.loadGold);

	//从背包中移除宝石
	pGemAttr->num = SGSU8Sub(pGemAttr->num, 1);
	if (pGemAttr->num == 0)
		packs.erase(it);

	//重新计算战力
	CalcRoleCombatPower();

	return MLS_OK;
}

int SRoleInfos::EquipPosRemoveGem(BYTE pos, BYTE holeSeq, SEquipPosRemoveGemRes* pRemoveRes)
{
	if (holeSeq > ESGS_GEM_HOLE_3)
		return MLS_GEM_HOLE_INVALID;

	//查找是否镶嵌过
	int idx = -1;
	BYTE n = (BYTE)equipPos.size();
	for (BYTE i = 0; i < n; ++i)
	{
		if (equipPos[i].pos == pos)
		{
			idx = i;
			break;
		}
	}
	if (idx < 0)
		return MLS_UNKNOWN_EQUIP_POS;

	UINT16 gemPropId = equipPos[idx].gem[holeSeq];
	if (gemPropId == 0)
		return MLS_GEM_NOT_INLAID;

	//获取取出宝石消耗
	GemCfg::SGemConsumeCfg spend;
	int retCode = GetGameCfgFileMan()->GetGemSpendById(gemPropId, spend);
	if (retCode != MLS_OK)
		return retCode;

	if (BigNumberToU64(gold) < spend.unloadGold)
		return MLS_GOLD_NOT_ENOUGH;

	//宝石能否放进背包
	BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(ESGS_PROP_GEM, gemPropId);
	if (!CanPutIntoBackpack(gemPropId, 1, maxStack))
		return MLS_BACKPACK_IS_FULL;

	//配置中查找该宝石id属性
	GemCfg::SGemAttrCfg gemAttr;
	retCode = GetGameCfgFileMan()->GetGemCfgAttr(gemPropId, gemAttr);
	if (retCode != MLS_OK)
		return retCode;

	//扣除金币
	if (spend.unloadGold > 0)
		ConsumeGold(spend.unloadGold, "取出宝石镶嵌");
	//SBigNumberSub(gold, spend.unloadGold);

	//清除镶嵌宝石
	equipPos[idx].gem[holeSeq] = 0;

	//将1颗宝石放进背包
	pRemoveRes->gemPropAttr.propId = gemPropId;
	pRemoveRes->gemPropAttr.level = gemAttr.level;
	pRemoveRes->gemPropAttr.grade = gemAttr.type;
	pRemoveRes->gemPropAttr.type = ESGS_PROP_GEM;
	pRemoveRes->gemPropAttr.num = 1;
	PutBackpackSingleNumProperty(pRemoveRes->gemPropAttr, maxStack);

	//重新计算战力
	CalcRoleCombatPower();

	return MLS_OK;
}

int SRoleInfos::GemCompose(BYTE num, const SConsumeProperty* pMaterial, SGemComposeRes* pComposeRes)
{
	//背包查找消耗宝石是否足够
	int cosumeGemLv = -1;	//消耗宝石等级
	int cosumeGemType = -1;	//消耗宝石类型
	int retCode = MLS_MATERIAL_NOT_ENOUGH;
	UINT16 consumeGems = 0;
	BYTE si = (BYTE)packs.size();
	for (BYTE i = 0; i < num; ++i)
	{
		BYTE packGemLv = 0;	//消耗宝石等级
		BYTE packGemType = 0;	//消耗宝石类型
		retCode = MLS_MATERIAL_NOT_ENOUGH;
		consumeGems = SGSU16Add(consumeGems, pMaterial[i].num);
		for (BYTE j = 0; j < si; ++j)
		{
			if (packs[j].id == pMaterial[i].id && packs[j].type == ESGS_PROP_GEM && packs[j].num >= pMaterial[i].num)
			{
				retCode = MLS_OK;
				packGemLv = packs[j].level;
				packGemType = packs[j].grade;
				break;
			}
		}

		if (retCode != MLS_OK)
			break;

		if (cosumeGemLv < 0)
			cosumeGemLv = packGemLv;
		else
		{
			if (cosumeGemLv != packGemLv)
			{
				retCode = MLS_GEM_MATERIAL_NOT_MATCH;	//需要消耗宝石等级不一样
				break;
			}
		}

		if (cosumeGemType < 0)
			cosumeGemType = packGemType;
		else
		{
			if (cosumeGemType != packGemType)
			{
				retCode = MLS_GEM_MATERIAL_NOT_MATCH;	//需要消耗宝石类型不一样
				break;
			}
		}
	}

	if (retCode != MLS_OK)
		return retCode;

	//消耗宝石不足
	if (consumeGems < GetGameCfgFileMan()->GetComposeGemConsumeGems())
		return MLS_MATERIAL_NOT_ENOUGH;

	//获取宝石合成消耗
	GemCfg::SGemConsumeCfg spend;
	retCode = GetGameCfgFileMan()->GetGemSpendByLevel(cosumeGemLv + 1, spend);
	if (retCode != MLS_OK)
		return retCode;

	//金币不足
	if (BigNumberToU64(gold) < spend.composeGold)
		return MLS_GOLD_NOT_ENOUGH;

	//获取合成后宝石属性
	UINT16 newGemId = 0;
	BYTE   maxStack = 0;
	retCode = GetGameCfgFileMan()->GetGemIdMaxStackByTypeLevel(cosumeGemType, cosumeGemLv + 1, newGemId, maxStack);
	if (retCode != MLS_OK)
		return retCode;

	if (!CanPutIntoBackpack(newGemId, 1, maxStack))
		return MLS_BACKPACK_IS_FULL;

	//合成1颗新宝石放回背包
	pComposeRes->composeGemAttr.type = ESGS_PROP_GEM;
	pComposeRes->composeGemAttr.propId = newGemId;
	pComposeRes->composeGemAttr.level = cosumeGemLv + 1;
	pComposeRes->composeGemAttr.grade = cosumeGemType;
	pComposeRes->composeGemAttr.num = 1;
	PutBackpackSingleNumProperty(pComposeRes->composeGemAttr, maxStack);

	//扣除消耗宝石
	for (BYTE i = 0; i < num; ++i)
		DropBackpackProp(pMaterial[i].id, pMaterial[i].num);

	//扣除金币
	if (spend.composeGold > 0)
		ConsumeGold(spend.composeGold, "宝石合成");
	//SBigNumberSub(gold, spend.composeGold);
	return MLS_OK;
}

int SRoleInfos::SkillUpgrade(UINT16 skillId, SSkillUpgradeRes* pUpgradeRes)
{
	int idx = -1;
	int si = (int)studySkills.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (studySkills[i].id == skillId)
		{
			idx = i;
			break;
		}
	}

	if (idx < 0)
		return MLS_SKILL_ID_NOT_EXIST;

	BYTE newSkillLv = SGSU8Add(studySkills[idx].level, 1);
	if (studySkills[idx].level == newSkillLv)
		return MLS_OVER_MAX_SKILL_LEVEL;

	if (level < newSkillLv)
		return MLS_LOWER_SKILL_LIMIT_LEVEL;	//低于角色等级

	SkillCfg::SSkillAttrCfg cost;
	int retCode = GetGameCfgFileMan()->GetUpgradeAnySkillAttr(occuId, skillId, newSkillLv, cost);
	if (retCode != MLS_OK)
		return retCode;

	//cost.GoldCost = cost.GoldCost * (newSkillLv - 1) * newSkillLv * cost.GoldCostCoefficient;
	cost.GoldCost = cost.GoldCost * pow(newSkillLv, cost.GoldCostCoefficient);
	if (BigNumberToU64(gold) < cost.GoldCost)
		return MLS_GOLD_NOT_ENOUGH;	//金币不足

	if (cost.GoldCost > 0)
		ConsumeGold(cost.GoldCost, "技能升级");
	//SBigNumberSub(gold, cost.GoldCost);	//扣金币

	studySkills[idx].level = newSkillLv;

	pUpgradeRes->gold = gold;
	pUpgradeRes->newSkill.id = skillId;
	pUpgradeRes->newSkill.level = newSkillLv;
	pUpgradeRes->newSkill.key = studySkills[idx].key;

	// 重新计算战力
	CalcRoleCombatPower();

	return MLS_OK;
}

int SRoleInfos::GetStudySkills(SSkillMoveInfo* pSkills, BYTE maxNum)
{
	int idx = min(maxNum, (BYTE)studySkills.size());
	for (int i = 0; i < idx; ++i)
	{
		memcpy(&pSkills[i], &studySkills[i], sizeof(SSkillMoveInfo));
		pSkills[i].hton();
	}
	return idx;
}

UINT32 SRoleInfos::CalcRoleCombatPower()
{
	SRoleAttrPoint attrPoint;

	//人物基础属性
	GetRoleBasicAttrPoint(attrPoint);

	//穿戴装备战力
	GetRoleWearEquipAttrPoint(attrPoint);

	//称号战力
	GetRoleWearTitleAttrPoint(attrPoint);

	//穿戴时装战力
	GetRoleWearFashionAttrPoint(attrPoint);

	//炼魂系统
	GetRoleChainSoulAttrPoint(attrPoint);

	//公式转换
	cp = ConvertAttrToPower(attrPoint);

	/* zpy 成就系统新增 */
	GetAchievementMan()->OnUpdateCombatPower(roleId, cp);

	return cp;
}

void SRoleInfos::GetRoleBasicAttrPoint(SRoleAttrPoint& attrPoint)
{
	//基础hp/mp
	attrPoint.u32HP += hp;
	attrPoint.u32MP += mp;

	//基础攻击+基础防御
	attrPoint.attack += attack;
	attrPoint.def += def;
}

void CalcEquipStarAttrPoint(BYTE starLv, SRoleAttrPoint& equipAttr)
{
	if (starLv <= 0)
		return;

	RiseStarCfg::SRiseStarCfg cfg;
	if (MLS_OK == GetGameCfgFileMan()->GetRiseStarCfg(starLv, cfg))
	{
		if (equipAttr.u32HP > 0)
		{
			equipAttr.u32HP *= cfg.extraEffect;
		}

		if (equipAttr.u32MP > 0)
		{
			equipAttr.u32MP *= cfg.extraEffect;
		}

		if (equipAttr.attack > 0)
		{
			equipAttr.attack *= cfg.extraEffect;
		}

		if (equipAttr.def > 0)
		{
			equipAttr.def *= cfg.extraEffect;
		}

		if (equipAttr.crit)
		{
			equipAttr.crit *= cfg.extraEffect;
		}

		if (equipAttr.tenacity)
		{
			equipAttr.tenacity *= cfg.extraEffect;
		}
	}
}

// zpy 新增
void CalcEquipStrengthenAttrPoint(BYTE strengthenLv, BYTE type, UINT32 val, BYTE master, const StrengthenCfg::SEquipAttrInc& attrInc, SRoleAttrPoint& equipAttr)
{
	switch (type)
	{
	case ESGS_EQUIP_ATTACH_HP:			//hp
		equipAttr.u32HP += val;
		break;
	case ESGS_EQUIP_ATTACH_MP:			//mp
		equipAttr.u32MP += val;
		break;
	case ESGS_EQUIP_ATTACH_ATTACK:		//攻击
		equipAttr.attack += val;
		break;
	case ESGS_EQUIP_ATTACH_DEF:			//防御
		equipAttr.def += val;
		break;
	case ESGS_EQUIP_ATTACH_CRIT:		//暴击
		equipAttr.crit += val;
		break;
	case ESGS_EQUIP_ATTACH_TENACITY:	//韧性
		equipAttr.tenacity += val;
		break;
	default:
		break;
	}

	// 装备主属性：基础一属性+装备强化等级*一属性增量
	// 装备二属性+装备强化等级*二属性增量
	float factor = 1.0f;
	StrengthenAttriCfg::SStrengthenAttr attr;
	factor = master == 1 ? attrInc.masterInc / 100.0f : attrInc.depuInc / 100.0f;

	// zpy 2016.1.9修改 策划郑一变更需求
	/*for (int i = 1; i <= strengthenLv; ++i)
	{*/
	if (GetGameCfgFileMan()->GetStrengthenAttri(strengthenLv, attr))
	{
		switch (type)
		{
		case ESGS_EQUIP_ATTACH_HP:			//hp
			equipAttr.u32HP += factor * attr.hp;
			break;
		case ESGS_EQUIP_ATTACH_MP:			//mp
			equipAttr.u32MP += factor * attr.mp;
			break;
		case ESGS_EQUIP_ATTACH_ATTACK:		//攻击
			equipAttr.attack += factor * attr.attack;
			break;
		case ESGS_EQUIP_ATTACH_DEF:			//防御
			equipAttr.def += factor * attr.def;
			break;
		case ESGS_EQUIP_ATTACH_CRIT:		//暴击
			equipAttr.crit += factor * attr.crit;
			break;
		case ESGS_EQUIP_ATTACH_TENACITY:	//韧性
			equipAttr.tenacity += factor * attr.strong;
			break;
		default:
			break;
		}
	}
	//}
}

void SRoleInfos::GetRoleWearEquipAttrPoint(SRoleAttrPoint& attrPoint)
{
	//std::map<UINT16/*套装id*/, BYTE/*数量*/> suitMap;

	//穿戴装备
	BYTE nNum = (BYTE)wearEquipment.size();
	for (BYTE i = 0; i < nNum; ++i)
	{
		std::vector<EquipCfg::SEquipAddPoint>* pAttrVec = NULL;
		int retCode = GetGameCfgFileMan()->GetEquipAttrPoint(wearEquipment[i].propId, pAttrVec);
		if (retCode != MLS_OK)
			continue;

		int posIdx = -1;
		BYTE posNum = (BYTE)equipPos.size();
		for (BYTE x = 0; x < posNum; ++x)
		{
			if (equipPos[x].pos == wearEquipment[i].pos)
			{
				posIdx = x;
				break;
			}
		}

		if (posIdx < 0)
			continue;

		//装备基本加点
		std::vector<EquipCfg::SEquipAddPoint>& attrVec = *pAttrVec;

		//装备强化加点
		StrengthenCfg::SEquipAttrInc attrInc;
		retCode = GetGameCfgFileMan()->GetEquipAttrIncreament(wearEquipment[i].pos, wearEquipment[i].grade, attrInc);
		if (retCode != MLS_OK)
		{
			attrInc.depuInc = 0;
			attrInc.masterInc = 0;
			//continue;
		}

		//装备强化属性
		SRoleAttrPoint equipAttr;
		int nSize = (int)attrVec.size();
		for (int j = 0; j < nSize; ++j)
		{
			CalcEquipStrengthenAttrPoint(equipPos[posIdx].lv, attrVec[j].type, attrVec[j].val, attrVec[j].master, attrInc, equipAttr);
		}

		//星级属性
		if (equipPos[posIdx].star > 0)
		{
			CalcEquipStarAttrPoint(equipPos[posIdx].star, equipAttr);
		}
		attrPoint.u32HP += equipAttr.u32HP;
		attrPoint.u32MP += equipAttr.u32MP;
		attrPoint.crit += equipAttr.crit;
		attrPoint.tenacity += equipAttr.tenacity;
		attrPoint.attack += equipAttr.attack;
		attrPoint.def += equipAttr.def;

		//装备镶嵌宝石属性
		for (BYTE k = 0; k < MAX_GEM_HOLE_NUM; ++k)
		{
			UINT16 gemId = equipPos[posIdx].gem[k];
			if (gemId == 0)
				continue;

			GemCfg::SGemAttrCfg gemAttr;
			retCode = GetGameCfgFileMan()->GetGemCfgAttr(gemId, gemAttr);
			if (retCode != MLS_OK)
				continue;

			CalcEquipGemAttrPoint(gemAttr.type, gemAttr.attrVal, attrPoint);
		}

		//套装属性
		//UINT16 suitId = wearEquipment[i].suitId;
		//if(suitId == 0)
		//	continue;

		//std::map<UINT16/*套装id*/, BYTE/*数量*/>::iterator it = suitMap.find(suitId);
		//if(it == suitMap.end())
		//	suitMap.insert(std::make_pair(suitId, 1));
		//else
		//	it->second = SGSU8Add(it->second, 1);
	}

	//套装属性
	//for(std::map<UINT16/*套装id*/, BYTE/*数量*/>::iterator it = suitMap.begin(); it != suitMap.end(); ++it)
	//{
	//	if(it->second <= 1)
	//		continue;

	//	std::vector<SuitCfg::SuitAttr> suitAttrVec;
	//	int retCode = GetGameCfgFileMan()->GetSuitAttr(it->first, it->second, suitAttrVec);
	//	if(retCode != MLS_OK)
	//		continue;

	//	int nSize = (int)suitAttrVec.size();
	//	for(int j = 0; j < nSize; ++j)
	//		CalcEquipAttachAttrPoint(suitAttrVec[j].type, suitAttrVec[j].val, attrPoint);
	//}
}

void SRoleInfos::GetRoleWearTitleAttrPoint(SRoleAttrPoint& attrPoint)
{
	if (titleId == 0)
		return;

	std::vector<TitleCfg::STitleAttr> titleAttrVec;
	int retCode = GetGameCfgFileMan()->GetRoleTitleAttr(occuId, titleId, titleAttrVec);
	if (retCode != MLS_OK)
		return;

	int si = (int)titleAttrVec.size();
	for (int i = 0; i < si; ++i)
		CalcEquipAttachAttrPoint(titleAttrVec[i].type, titleAttrVec[i].val, attrPoint);
}

void SRoleInfos::GetRoleWearFashionAttrPoint(SRoleAttrPoint& attrPoint)
{
	if (dressId == 0)
		return;

	std::vector<FashionCfg::SFashionAttr>* pFashionAttrVec = NULL;
	int retCode = GetGameCfgFileMan()->GetFashionAttr(dressId, pFashionAttrVec);
	if (retCode != MLS_OK)
		return;
	std::vector<FashionCfg::SFashionAttr>& fashionAttrVec = *pFashionAttrVec;

	int si = (int)fashionAttrVec.size();
	for (int i = 0; i < si; ++i)
		CalcEquipAttachAttrPoint(fashionAttrVec[i].type, fashionAttrVec[i].val, attrPoint);
}

/* zpy 获取炼魂属性点 */
void SRoleInfos::GetRoleChainSoulAttrPoint(SRoleAttrPoint& attrPoint)
{
	ASSERT(chainSoulPos.size() == MAX_CHAIN_SOUL_HOLE_NUM);

	// 叠加各部件属性加成
	BYTE min_level = UCHAR_MAX;
	for (size_t i = 0; i < MAX_CHAIN_SOUL_HOLE_NUM; ++i)
	{
		GreedTheLiveCfg::ChainSoulPosConfig *pConfig = NULL;
		if (GetGameCfgFileMan()->GetChainSoulPosConfig(chainSoulPos[i].pos, chainSoulPos[i].level, pConfig))
		{
			for (size_t j = 0; j < pConfig->attr_plus.size(); ++j)
			{
				CalcEquipAttachAttrPoint(pConfig->attr_plus[j].type, pConfig->attr_plus[j].attrVal, attrPoint);
			}
		}

		if (chainSoulPos[i].level < min_level)
		{
			min_level = chainSoulPos[i].level;
		}
	}

	//计算炼魂核心属性加成
	GreedTheLiveCfg::ChainSoulCoreConfig *pConfig = NULL;
	BYTE core_level = GetGameCfgFileMan()->GetCoreLevelByConditionLevel(min_level);
	if (GetGameCfgFileMan()->GetChainSoulCoreConfig(core_level, pConfig))
	{
		for (size_t i = 0; i < pConfig->attr_plus.size(); ++i)
		{
			CalcEquipAttachAttrPoint(pConfig->attr_plus[i].type, pConfig->attr_plus[i].attrVal, attrPoint);
		}
	}
}

UINT32 SRoleInfos::ConvertAttrToPower(const SRoleAttrPoint& attrPoint)
{
	//基本属性战力= HP*HPBalance+ATK*ATKBalance+DEF*DEFBalance+MP*MPBalance
	GameCfgFileMan::SCPBalanceAttr cpBalanceAttr;
	GetGameCfgFileMan()->GetCPBalanceAttr(cpBalanceAttr);
	UINT32 attr_cp = attrPoint.u32HP * cpBalanceAttr.HPBalance;
	attr_cp += attrPoint.attack * cpBalanceAttr.ATKBalance;
	attr_cp += attrPoint.def * cpBalanceAttr.DEFBalance;
	attr_cp += attrPoint.u32MP * cpBalanceAttr.MPBalance;

	//暴击战力=ATK*ATKBalance/100*CritHurt/100* CriticalRating/10000*CRIT/(CRIT+SGBalance/100)；
	UINT32 crit_cp = attrPoint.attack * cpBalanceAttr.ATKBalance * cpBalanceAttr.CritHurt * cpBalanceAttr.CriticalRating * attrPoint.crit / (attrPoint.crit + cpBalanceAttr.SGBalance);

	//韧性战力=ATK*ATKBalance/100*CritHurt/100* CriticalRating/10000*(1-CTBalance/100/(CTBalance/100+STRONG)) 
	UINT32 tenacity_cp = attrPoint.attack * cpBalanceAttr.ATKBalance * cpBalanceAttr.CritHurt * cpBalanceAttr.CriticalRating *
		(1 - cpBalanceAttr.CTBalance / (cpBalanceAttr.CTBalance + attrPoint.tenacity));

	/**
	* 技能战力
	* 2015.12.18 策划郑一新增
	* 技能战力=((ATK*ATKBalance/100*(Damage/100+DamageUp/100*(Lv-1))+(DamageCount+DamageCountUp*(Lv-1))*ATKBalance/100)/CD/SkillTime)*SKBalance/100
	*/
	UINT32 skill_cp = 0;
	for (size_t i = 0; i < studySkills.size(); ++i)
	{
		SkillCfg::SSkillAttrCfg *skill = NULL;
		if (GetGameCfgFileMan()->GetSkillAttr(occuId, studySkills[i].id, skill))
		{
			skill_cp += ((attrPoint.attack * cpBalanceAttr.ATKBalance * (skill->Damage + skill->DamageUp * (studySkills[i].level - 1))
				+ (skill->DamageCount + skill->DamageCountUp * (studySkills[i].level - 1)) * cpBalanceAttr.ATKBalance)
				/ (float)skill->CDTime / skill->SkillTime) * cpBalanceAttr.SKBalance;
		}
	}

	return attr_cp + crit_cp + tenacity_cp + skill_cp;
}

int SRoleInfos::GetBackpackPropIdx(UINT32 id)
{
	int idx = -1;
	BYTE si = (BYTE)packs.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (packs[i].id == id)
		{
			idx = i;
			break;
		}
	}
	return idx;
}

int SRoleInfos::GetWearEquipIdx(UINT32 id)
{
	int idx = -1;
	BYTE si = (BYTE)wearEquipment.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (wearEquipment[i].id == id)
		{
			idx = i;
			break;
		}
	}
	return idx;
}

int SRoleInfos::PutBackpackSingleNumProperty(SPropertyAttr& prop, BYTE maxStack)
{
	ASSERT(prop.num == 1 && prop.num <= maxStack);
	int retCode = MLS_BACKPACK_IS_FULL;
	//add by hxw 20160108 背包换格子保证不存在多个没满的散装物品
	int firstPos = -1;
	//end

	/* zpy 成就系统新增 */
	GemCfg::SGemAttrCfg gem_attr;
	if (GetGameCfgFileMan()->GetGemCfgAttr(prop.propId, gem_attr) != MLS_PROPERTY_NOT_EXIST)
	{
		GetAchievementMan()->OnGotGem(roleId, gem_attr.level);
	}

	BYTE si = (BYTE)packs.size();
	if (prop.type != ESGS_PROP_EQUIP)	//装备不存在叠加存放
	{
		for (BYTE i = 0; i < si; ++i)
		{
			if (packs[i].propId != prop.propId)	//物品id相等
				continue;
			if (firstPos<0)
				firstPos = i;

			if (SGSU8Sub(maxStack, packs[i].num) < prop.num)
				continue;

			BYTE nums = SGSU8Add(packs[i].num, prop.num);
			packs[i].num = nums;
			prop.num = nums;
			prop.id = packs[i].id;
			retCode = MLS_OK;
			break;
		}
	}

	if (retCode == MLS_OK)
		return retCode;

	if (si >= packSize)	//大于背包最大数量
		return retCode;
	prop.id = uid++;
	packs.push_back(prop);	//放入新的格子
	if (firstPos >= 0)
	{
		std::swap(packs[firstPos], packs.back());
	}

	return MLS_OK;
}

int SRoleInfos::AcceptTask(UINT16 taskId)
{
	BYTE si = (BYTE)acceptTasks.size();
	if (si >= GetGameCfgFileMan()->GetMaxAcceptTasks())
		return MLS_OVER_MAX_TASK_NUM;

	TaskCfg::STaskAttrCfg* pTaskInfo = NULL;
	int retCode = GetGameCfgFileMan()->GetTaskInfo(taskId, pTaskInfo);
	if (retCode != MLS_OK)
		return retCode;

	TaskCfg::STaskLimitCondition& taskLimit = pTaskInfo->limit;
	if (level < taskLimit.limitLv)
		return MLS_LOWER_TASK_LIMIT_LEVEL;

	for (BYTE i = 0; i < si; ++i)
	{
		if (acceptTasks[i].taskId == taskId)
			return MLS_EXIST_TASK_QUEUE;
	}

	SAcceptTaskInfo task;
	memset(&task, 0, sizeof(SAcceptTaskInfo));

	BYTE targetType = pTaskInfo->targetType;
	std::vector<UINT32>& target = pTaskInfo->targetVec;
	int n = min(MAX_TASK_TARGET_NUM, (int)target.size());
	if (n <= 0)
		return MLS_NOT_EXIST_TASK;

	switch (targetType)
	{
	case ESGS_TASK_TARGET_PASS_STAGE:			//通关
	case ESGS_TASK_TARGET_FIND_NPC:				//找到NPC
	case ESGS_TASK_TARGET_PASS_NO_DIE:			//不死通关
	case ESGS_TASK_TARGET_LIMIT_TIME:			//限时通关
	case ESGS_TASK_TARGET_KEEP_BLOOD:			//血量保持
	case ESGS_TASK_TARGET_PASS_NO_HURT:			//无伤通关
	case ESGS_TASK_TARGET_COMBO_NUM_REACH:		//达到足够多的连击数
	case ESGS_TASK_TARGET_PASS_STAGE_NUM:		//通关任意副本n次
	case ESGS_TASK_TARGET_PASS_RAND_STAGE_NUM:	//通关随机副本n次
		task.target[0] = target[0];	//副本id
		break;

	case ESGS_TASK_TARGET_KILL_MONSTER:			//杀怪
		task.target[0] = target[0];	//副本id
		task.target[1] = target[1]; //怪id
		break;

	case ESGS_TASK_TARGET_DESTROY_SCENE:	//破坏场景物
		task.target[0] = target[0];	//副本id
		task.target[1] = target[1];	//场景物类型
		break;

	case ESGS_TASK_TARGET_FETCH_PROP:		//获取道具
		task.target[0] = target[0];	//副本id
		task.target[1] = target[1];	//道具类型
		task.target[2] = target[2];	//道具id
		break;

	case ESGS_TASK_TARGET_ESCORT:			//护送
		task.target[0] = target[0];	//副本id
		task.target[1] = target[1]; //NPC id
		break;

	case ESGS_TASK_TARGET_NO_USE_SPECIAL_SKILL:	//不使用特殊技能通关
	case ESGS_TASK_TARGET_PASS_NO_BLOOD:		//不使用血瓶通关
		task.target[0] = target[0];	//副本id
		task.target[1] = target[1];	//禁止使用ID
		break;

	case ESGS_TASK_TARGET_FETCH_PROP_LIMIT_TIME:	//限时获取道具（复数）
		task.target[0] = target[0];		//副本id
		task.target[1] = target[1];		//道具类型
		task.target[2] = target[2];		//道具id
		break;

	case ESGS_TASK_TARGET_DESTROY_SCENE_LIMIT_TIME:	//限时破坏场景物（复数）
		task.target[0] = target[0];		//副本id
		task.target[1] = target[1];		//场景物类型
		break;

	case ESGS_TASK_TARGET_KILL_MONSTER_LIMIT_TIME:	//限时杀怪
		task.target[0] = target[0];		//副本id
		task.target[1] = target[1];		//怪id
		break;

	case ESGS_TASK_TARGET_ROLE_LV_REACH:	//角色等级达到
		task.target[0] = level;
		task.isFinished = level >= target[0] ? 1 : 0;
		break;

	case ESGS_TASK_TARGET_LOGIN_RECEIVE_VIT:	//登陆领取体力
		task.target[0] = target[0];
		break;

	case ESGS_TASK_TARGET_OPEN_CHEST_BOX_NUM:	//手动开启宝箱
		task.target[0] = target[0];				//道具类型
		task.target[1] = target[1];				//道具id
		break;

	default:
		break;
	}

	if (retCode != MLS_OK)
		return retCode;

	task.taskId = taskId;
	acceptTasks.push_back(task);
	return MLS_OK;
}

int SRoleInfos::DropTask(UINT16 taskId)
{
	for (std::vector<SAcceptTaskInfo>::iterator it = acceptTasks.begin(); it != acceptTasks.end(); ++it)
	{
		if (it->taskId == taskId)
		{
			acceptTasks.erase(it);
			break;
		}
	}
	return MLS_OK;
}

int SRoleInfos::JudgeTaskFinish(UINT16 taskId, SFinishTaskFailNotify* pFinTaskNotify)
{
	BOOL bExist = false;
	std::vector<SAcceptTaskInfo>::iterator it = acceptTasks.begin();
	for (it = acceptTasks.begin(); it != acceptTasks.end(); ++it)
	{
		if (it->taskId == taskId)
		{
			bExist = true;
			break;
		}
	}

	if (!bExist)
		return MLS_NOT_EXIST_TASK;

	//完成任务目标
	TaskCfg::STaskAttrCfg* pTaskInfo = NULL;
	int retCode = GetGameCfgFileMan()->GetTaskInfo(taskId, pTaskInfo);
	if (retCode != MLS_OK)
		return retCode;

	std::vector<UINT32>& taskTargetVec = pTaskInfo->targetVec;
	BYTE targetType = pTaskInfo->targetType;
	BYTE sysTaskType = pTaskInfo->type;

	int taskTargetParam = (int)taskTargetVec.size();
	SAcceptTaskInfo& finTask = *it;

	switch (targetType)
	{
	case ESGS_TASK_TARGET_PASS_STAGE:			//通关
		retCode = MLS_OK;
		break;

	case ESGS_TASK_TARGET_KILL_MONSTER:			//杀怪
		ASSERT(taskTargetParam >= 3);
		retCode = (finTask.target[1] == taskTargetVec[1] && finTask.target[2] >= taskTargetVec[2]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		break;

	case ESGS_TASK_TARGET_FETCH_PROP:			//获取道具
		ASSERT(taskTargetParam >= 4);
		retCode = (finTask.target[1] == taskTargetVec[1] && finTask.target[2] == taskTargetVec[2] && finTask.target[3] >= taskTargetVec[3]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		break;

	case ESGS_TASK_TARGET_DESTROY_SCENE:		//破坏场景物
		ASSERT(taskTargetParam >= 3);
		retCode = (finTask.target[1] == taskTargetVec[1] && finTask.target[2] >= taskTargetVec[2]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		break;

	case ESGS_TASK_TARGET_LIMIT_TIME:			//限时通关
		ASSERT(taskTargetParam >= 2);
		retCode = (finTask.target[1] != 0 && finTask.target[1] <= taskTargetVec[1]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		if (retCode != MLS_OK)
			finTask.target[1] = 0;
		break;

	case ESGS_TASK_TARGET_ESCORT:				//护送
		ASSERT(taskTargetParam >= 3);
		retCode = (finTask.target[2] >= taskTargetVec[2]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		if (retCode != MLS_OK)
			finTask.target[2] = 0;
		break;

	case ESGS_TASK_TARGET_FIND_NPC:				//找到NPC id
		ASSERT(taskTargetParam >= 2);
		retCode = (finTask.target[1] == taskTargetVec[1]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		if (retCode != MLS_OK)
			finTask.target[1] = 0;
		break;

	case ESGS_TASK_TARGET_PASS_NO_HURT:			//无伤通关(HP)
	case ESGS_TASK_TARGET_KEEP_BLOOD:			//血量保持(HP)
		ASSERT(taskTargetParam >= 2);
		retCode = (finTask.target[1] >= taskTargetVec[1]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		if (retCode != MLS_OK)
			finTask.target[1] = 0;
		break;

	case ESGS_TASK_TARGET_BLOCK_TIMES:			//格挡多少次
	case ESGS_TASK_TARGET_COMBO_NUM_REACH:		//达到足够多的连击数
		ASSERT(taskTargetParam >= 2);
		retCode = (finTask.target[1] >= taskTargetVec[1]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		if (retCode != MLS_OK)
			finTask.target[1] = 0;
		break;

	case ESGS_TASK_TARGET_ROLE_LV_REACH:		//角色等级达到
		retCode = (level >= taskTargetVec[0]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		break;

	case ESGS_TASK_TARGET_FETCH_PROP_LIMIT_TIME:	//限时获取道具（复数） 
		ASSERT(taskTargetParam >= 5);
		retCode = (finTask.target[3] <= taskTargetVec[3] && finTask.target[4] >= taskTargetVec[4]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		if (retCode != MLS_OK)
		{
			finTask.target[3] = 0;
			finTask.target[4] = 0;
		}
		break;

	case ESGS_TASK_TARGET_KILL_MONSTER_LIMIT_TIME:	//限时杀怪
	case ESGS_TASK_TARGET_DESTROY_SCENE_LIMIT_TIME:	//限时破坏场景物（复数）
		ASSERT(taskTargetParam >= 4);
		retCode = (finTask.target[2] <= taskTargetVec[2] && finTask.target[3] >= taskTargetVec[3]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		if (retCode != MLS_OK)
		{
			finTask.target[2] = 0;
			finTask.target[3] = 0;
		}
		break;

	case ESGS_TASK_TARGET_COMPOSE:				//合成
	case ESGS_TASK_TARGET_CASH:					//兑换
	case ESGS_TASK_TARGET_STRENGTHEN:			//强化
	case ESGS_TASK_TARGET_RISE_STAR:			//升星
		ASSERT(taskTargetParam >= 3);
		retCode = (finTask.target[2] >= taskTargetVec[2]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		break;
	case ESGS_TASK_TARGET_PASS_STAGE_NUM:			//通关任意普通副本次数
	case ESGS_TASK_TARGET_PASS_RAND_STAGE_NUM:		//通关随机普通副本次数
		retCode = (finTask.target[1] >= taskTargetVec[1]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		break;
	case ESGS_TASK_TARGET_OPEN_CHEST_BOX_NUM:		//手动开启宝箱
		retCode = (finTask.target[2] >= taskTargetVec[2]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		break;
	case ESGS_TASK_TARGET_PASS_NO_BLOOD:		//不使用血瓶通关
	case ESGS_TASK_TARGET_PASS_NO_DIE:			//不死通关
	case ESGS_TASK_TARGET_NO_USE_SPECIAL_SKILL:	//不使用特殊技能通关
	case ESGS_TASK_TARGET_KILL_ALL_MONSTER:		//杀死所有怪
	case ESGS_TASK_TARGET_DESTROY_ALL_SCENE:	//破坏所有场景物
	case ESGS_TASK_TARGET_LOGIN_RECEIVE_VIT:	//登陆领取体力
		retCode = MLS_OK;
		break;

	default:
		retCode = MLS_FINISH_TASK_FAILED;
		break;
	}

	it->isFinished = retCode == MLS_OK ? 1 : 0;
	if (!it->isFinished)
	{
		pFinTaskNotify->taskId = taskId;
		memcpy(pFinTaskNotify->target, it->target, MAX_TASK_TARGET_NUM * sizeof(UINT32));

		if (sysTaskType == ESGS_SYS_RAND_TASK)
			acceptTasks.erase(it);
	}
	return retCode;
}

int SRoleInfos::ReceiveTaskReward(UINT32 userId, UINT32 roleId, UINT16 taskId, SReceiveTaskRewardRes* pTaskRewardRes)
{
	BOOL isFinished = false;
	std::vector<SAcceptTaskInfo>::iterator it = acceptTasks.begin();
	for (; it != acceptTasks.end(); ++it)
	{
		if (it->taskId == taskId)
		{
			isFinished = !!it->isFinished;
			break;
		}
	}

	if (it == acceptTasks.end())
		return MLS_NOT_EXIST_TASK;

	if (!isFinished)
		return MLS_FINISH_TASK_FAILED;

	//移除任务
	acceptTasks.erase(it);

	std::vector<SPropertyAttr> emailAttachmentsVec;
	SPropertyAttr* pModifyProps = pTaskRewardRes->modifyProps;
	BYTE maxProps = (BYTE)((MAX_RES_USER_BYTES - member_offset(SReceiveTaskRewardRes, taskId, modifyProps)) / sizeof(SPropertyAttr));

	//获取任务完成奖励
	TaskCfg::STaskReward taskReward;
	int retCode = GetGameCfgFileMan()->GetTaskReard(taskId, level, taskReward);
	if (MLS_OK != retCode)
		return retCode;
	std::vector<SDropPropCfgItem>& rewardVec = taskReward.props;

	UINT32 token = 0;
	UINT64 u64Gold = taskReward.gold;
	UINT64 u64Exp = taskReward.exp;

	//奖励物品
	BYTE n = (BYTE)rewardVec.size();
	std::set<UINT16> addFragments;
	std::set<UINT16> addSoulFragments;
	for (BYTE i = 0; i < n; ++i)
	{
		SDropPropCfgItem & item = rewardVec[i];
		BYTE propType = item.type;
		switch (propType)
		{
		case ESGS_PROP_GOLD:
			u64Gold = SGSU64Add(u64Gold, item.num);
			break;
		case ESGS_PROP_TOKEN:
			token = SGSU32Add(token, item.num);
			break;
		case ESGS_PROP_FRAGMENT:
			AddEquipFragment(item.propId, item.num);
			addFragments.insert(item.propId);
			break;
		case ESGS_PROP_CHAIN_SOUL_FRAGMENT:
			AddChainSoulFragment(item.propId, item.num);
			addSoulFragments.insert(item.propId);
			break;
		case ESGS_PROP_EXP:
			u64Exp = SGSU64Add(u64Exp, item.num);
			break;
		case ESGS_PROP_EQUIP:
		case ESGS_PROP_GEM:
		case ESGS_PROP_MATERIAL:
		{
								   BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(propType, item.propId);
								   if (!CanPutIntoBackpack(item.propId, item.num, maxStack))
								   {
									   //背包放不下, 发邮件附件
									   SPropertyAttr attachmentProp;
									   memset(&attachmentProp, 0, sizeof(SPropertyAttr));
									   int retCode = GetGameCfgFileMan()->GetPropertyAttr(propType, item.propId, attachmentProp, maxStack);
									   if (retCode == MLS_OK)
									   {
										   attachmentProp.num = (BYTE)item.num;
										   emailAttachmentsVec.push_back(attachmentProp);
									   }
								   }
								   else
								   {
									   BYTE retNum = 0;
									   PutBackpackProperty(propType, item.propId, item.num, maxStack,
										   pModifyProps + pTaskRewardRes->modifyNum,
										   SGSU8Sub(maxProps, pTaskRewardRes->modifyNum), retNum);

									   pTaskRewardRes->modifyNum = SGSU8Add(pTaskRewardRes->modifyNum, retNum);
								   }
		}
			break;
		default:
			break;
		}
	}

	/* zpy 2015.12.1新增，客戶端要求將碎片信息放入背包 */
	for (size_t i = 0; i < fragments.size(); ++i)
	{
		std::set<UINT16>::iterator itr = addFragments.find(fragments[i].fragmentId);
		if (itr != addFragments.end())
		{
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].id = 0;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].propId = fragments[i].fragmentId;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].type = ESGS_PROP_FRAGMENT;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].suitId = 0;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].level = 0;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].grade = 0;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].star = 0;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].num = fragments[i].num;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].pos = 0;
			pTaskRewardRes->modifyNum = SGSU8Add(pTaskRewardRes->modifyNum, 1);
		}
	}
	for (size_t i = 0; i < chainSoulFragments.size(); ++i)
	{
		std::set<UINT16>::iterator itr = addSoulFragments.find(chainSoulFragments[i].fragmentId);
		if (itr != addSoulFragments.end())
		{
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].id = 0;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].propId = chainSoulFragments[i].fragmentId;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].type = ESGS_PROP_CHAIN_SOUL_FRAGMENT;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].suitId = 0;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].level = 0;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].grade = 0;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].star = 0;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].num = chainSoulFragments[i].num;
			pTaskRewardRes->modifyProps[pTaskRewardRes->modifyNum].pos = 0;
			pTaskRewardRes->modifyNum = SGSU8Add(pTaskRewardRes->modifyNum, 1);
		}
	}

	if (token > 0)
		ProduceToken(token, "任务奖励");

	if (u64Gold > 0)
		ProduceGold(u64Gold, "任务奖励");

	if (u64Exp > 0)
	{
		//增加经验
		AddRoleExp(u64Exp);

		//判断是否升级(经验增加才会升级)
		BYTE newLv = 0;
		GetGameCfgFileMan()->JudgeRoleUpgrade(occuId, level, BigNumberToU64(exp), newLv);
		if (newLv > level)	//需要升级
		{
			//开放新技能
			GetGameCfgFileMan()->GetRoleAllInitSkills(occuId, level, newLv, studySkills);
			//新等级
			level = newLv;
			//新等级角色hp/mp
			SRoleAttrPoint attr;
			if (MLS_OK == GetGameCfgFileMan()->GetRoleCfgBasicAttr(occuId, level, attr))
			{
				hp = attr.u32HP;
				mp = attr.u32MP;
				attack = attr.attack;
				def = attr.def;
			}
			//升级了
			pTaskRewardRes->isUpgrade = 1;

			//重新计算战力
			CalcRoleCombatPower();
		}
	}

	//是否发送邮件
	BYTE attachmentsNum = (BYTE)emailAttachmentsVec.size();
	if (attachmentsNum > 0)
	{
		EmailCleanMan::SSyncEmailSendContent email;
		email.type = ESGS_EMAIL_TASK_REWARD;
		email.receiverRoleId = roleId;
		email.receiverUserId = userId;
		email.attachmentsNum = attachmentsNum;
		email.pAttachments = emailAttachmentsVec.data();
		email.pGBSenderNick = "系统";		//发送者昵称
		email.pGBCaption = "背包满, 任务奖励";	//标题
		email.pGBBody = "任务奖励物品";		//正文
		GetEmailCleanMan()->AsyncSendEmail(email);
	}

	//响应数据
	pTaskRewardRes->taskId = taskId;
	pTaskRewardRes->exp = exp;		//经验(当前拥有+本次所得)
	pTaskRewardRes->gold = gold; 	//新拥有总的游戏币(当前拥有+本次所得)
	pTaskRewardRes->rpcoin = cash;  //新拥有总的代币(当前拥有+本次所得)
	for (BYTE i = 0; i < pTaskRewardRes->modifyNum; ++i)
		pTaskRewardRes->modifyProps[i].hton();

	return MLS_OK;
}

int SRoleInfos::GetDailyTasks(UINT32 roleId, SGetDailyTaskRes* pDailyTasks, BYTE maxNum)
{
	TaskCfg::SDailyTasks roleDailyTasks;
	int retCode = GetGameCfgFileMan()->GetDailyTasks(level, roleDailyTasks);
	if (retCode != MLS_OK)
		return retCode;

	BYTE idx = 0;
	size_t si = roleDailyTasks.dailyTasks.size();
	for (size_t i = 0; i < si; ++i)
	{
		UINT16 dailyTaskId = roleDailyTasks.dailyTasks[i];

		//是否在已接任务列表中
		if (IsExistAcceptedTasks(dailyTaskId))
			continue;

		//是否已完成
		if (GetDailyTaskMan()->IsFinishedDailyTask(roleId, dailyTaskId))
			continue;

		pDailyTasks->dailyTasks[idx] = htons(dailyTaskId);
		if (++idx >= maxNum)
			break;
	}
	pDailyTasks->num = idx;
	return MLS_OK;
}

BOOL SRoleInfos::IsExistAcceptedTasks(UINT16 taskId)
{
	BYTE si = (BYTE)acceptTasks.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (acceptTasks[i].taskId == taskId)
			return true;
	}
	return false;
}

int SRoleInfos::UpdateTaskTarget(UINT16 taskId, const UINT32* pTarget, int n/* = MAX_TASK_TARGET_NUM*/)
{
	SAcceptTaskInfo* pAcceptTask = NULL;
	BYTE si = (BYTE)acceptTasks.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (acceptTasks[i].taskId == taskId)
		{
			pAcceptTask = &acceptTasks[i];
			break;
		}
	}

	if (pAcceptTask == NULL)
		return MLS_NOT_EXIST_TASK;

	TaskCfg::STaskAttrCfg* pTaskInfo = NULL;
	int retCode = GetGameCfgFileMan()->GetTaskInfo(pAcceptTask->taskId, pTaskInfo);
	if (retCode != MLS_OK)
		return retCode;

	BYTE targetType = pTaskInfo->targetType;
	if (targetType == 0)
		return MLS_NOT_EXIST_TASK;

	switch (targetType)
	{
	case ESGS_TASK_TARGET_KILL_MONSTER:			//杀怪
		if ((pAcceptTask->target[0] == pTarget[0]) && (pAcceptTask->target[1] == pTarget[1] || pAcceptTask->target[1] == 0))	//副本id && 怪id
			pAcceptTask->target[2] = SGSU32Add(pAcceptTask->target[2], pTarget[2]);
		break;

	case ESGS_TASK_TARGET_DESTROY_SCENE:		//破坏场景物
		if ((pAcceptTask->target[0] == pTarget[0]) && (pAcceptTask->target[1] == pTarget[1] || pAcceptTask->target[1] == 0))	//副本id && 场景物类型
			pAcceptTask->target[2] = SGSU32Add(pAcceptTask->target[2], pTarget[2]);	//数量
		break;

	case ESGS_TASK_TARGET_FETCH_PROP:			//获取道具
		if ((pAcceptTask->target[0] == pTarget[0]) &&
			(pAcceptTask->target[1] == 0 ||
			(pAcceptTask->target[1] == pTarget[1] && pAcceptTask->target[2] == 0) ||
			(pAcceptTask->target[1] == pTarget[1] && pAcceptTask->target[2] == pTarget[2])))	//副本id && 道具类型 && 道具id
			pAcceptTask->target[3] = SGSU32Add(pAcceptTask->target[3], pTarget[3]);	//数量
		break;
	case ESGS_TASK_TARGET_LIMIT_TIME:			//限时通关
		if (pAcceptTask->target[0] == pTarget[0])	//副本id
			pAcceptTask->target[1] = pTarget[1];	//时间
		break;

	case ESGS_TASK_TARGET_ESCORT:				//护送
		if (pAcceptTask->target[1] == pTarget[1])	//NPC
			pAcceptTask->target[2] = pTarget[2];	//生命(HP)
		break;

	case ESGS_TASK_TARGET_PASS_NO_HURT:			//无伤通关(HP)
	case ESGS_TASK_TARGET_BLOCK_TIMES:			//格挡多少次
	case ESGS_TASK_TARGET_COMBO_NUM_REACH:		//达到足够多的连击数
	case ESGS_TASK_TARGET_KEEP_BLOOD:			//血量保持(HP)
	case ESGS_TASK_TARGET_FIND_NPC:				//找到NPC
		if (pAcceptTask->target[0] == pTarget[0])	//副本id
			pAcceptTask->target[1] = pTarget[1];
		break;

	case ESGS_TASK_TARGET_FETCH_PROP_LIMIT_TIME:	//限时获取道具（复数）
		if ((pAcceptTask->target[0] == pTarget[0]) &&
			(pAcceptTask->target[1] == 0 ||
			(pAcceptTask->target[1] == pTarget[1] && pAcceptTask->target[2] == 0) ||
			(pAcceptTask->target[1] == pTarget[1] && pAcceptTask->target[2] == pTarget[2])))	//副本id && 道具类型 && 道具id
		{
			pAcceptTask->target[3] = pTarget[3];	//时间
			pAcceptTask->target[4] = SGSU32Add(pAcceptTask->target[4], pTarget[4]);	//数量
		}
		break;

	case ESGS_TASK_TARGET_KILL_MONSTER_LIMIT_TIME:	//限时杀怪
	case ESGS_TASK_TARGET_DESTROY_SCENE_LIMIT_TIME:	//限时破坏场景物（复数）
		if ((pAcceptTask->target[0] == pTarget[0]) && (pAcceptTask->target[1] == pTarget[1] || pAcceptTask->target[1] == 0))	//副本id && 怪id/场景物类型
		{
			pAcceptTask->target[2] = pTarget[2];	//时间
			pAcceptTask->target[3] = SGSU32Add(pAcceptTask->target[3], pTarget[3]);	//数量
		}
		break;

	case ESGS_TASK_TARGET_CASH:					//兑换
	case ESGS_TASK_TARGET_STRENGTHEN:			//强化
	case ESGS_TASK_TARGET_RISE_STAR:			//升星
	case ESGS_TASK_TARGET_COMPOSE:				//合成
		if (pAcceptTask->target[0] == pTarget[0] && pAcceptTask->target[1] == pTarget[1])	//道具类型 && 道具id
			pAcceptTask->target[2] = SGSU32Add(pAcceptTask->target[2], pTarget[2]);	//数量
		break;
	case ESGS_TASK_TARGET_ROLE_LV_REACH:		//角色等级达到
		pAcceptTask->target[0] = min(level, pTarget[0]);
		break;

	case ESGS_TASK_TARGET_PASS_STAGE_NUM:		//通关任意普通副本次数
	case ESGS_TASK_TARGET_PASS_RAND_STAGE_NUM:	//通关随机普通副本次数
		pAcceptTask->target[1] = SGSU32Add(pAcceptTask->target[1], 1);
		break;

	case ESGS_TASK_TARGET_OPEN_CHEST_BOX_NUM:		//手动开启宝箱
		pAcceptTask->target[2] = SGSU32Add(pAcceptTask->target[2], 1);
		break;

	case ESGS_TASK_TARGET_PASS_STAGE:			//通关
	case ESGS_TASK_TARGET_PASS_NO_BLOOD:		//不使用血瓶通关
	case ESGS_TASK_TARGET_NO_USE_SPECIAL_SKILL:	//不使用特殊技能通关
	case ESGS_TASK_TARGET_PASS_NO_DIE:			//不死通关
	case ESGS_TASK_TARGET_KILL_ALL_MONSTER:		//杀死所有怪
	case ESGS_TASK_TARGET_DESTROY_ALL_SCENE:	//破坏所有场景物
	case ESGS_TASK_TARGET_LOGIN_RECEIVE_VIT:	//登陆领取体力
		break;

	default:
		break;
	}
	return MLS_OK;
}

int SRoleInfos::ReceiveEmailAttachments(BYTE emailType, const SPropertyAttr* pAttachmentsProp, BYTE attachmentsNum, SReceiveEmailAttachmentsRes* pEmailAttachmentsRes, BOOL& isUpgrade)
{
	for (BYTE i = 0; i < attachmentsNum; ++i)
	{
		BYTE propType = pAttachmentsProp[i].type;
		if (propType == ESGS_PROP_FRAGMENT || propType == ESGS_PROP_DRESS || propType == ESGS_PROP_MAGIC || propType == ESGS_PROP_CHAIN_SOUL_FRAGMENT)
			continue;

		BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(propType, pAttachmentsProp[i].propId);
		if (!CanPutIntoBackpack(pAttachmentsProp[i].propId, pAttachmentsProp[i].num, maxStack))
			return MLS_BACKPACK_IS_FULL;
	}

	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SReceiveEmailAttachmentsRes, emailId, props)) / sizeof(SPropertyAttr));
	SPropertyAttr* pBackpackModify = pEmailAttachmentsRes->props;
	pEmailAttachmentsRes->propsNum = 0;
	for (BYTE i = 0; i < attachmentsNum; ++i)
	{
		BYTE retNum = 0;
		BYTE propType = pAttachmentsProp[i].type;
		UINT16 propId = pAttachmentsProp[i].propId;
		BYTE propNum = pAttachmentsProp[i].num;

		if (propType == ESGS_PROP_FRAGMENT)
		{
			AddEquipFragment(propId, propNum);
			continue;
		}
		else if (propType == ESGS_PROP_CHAIN_SOUL_FRAGMENT)
		{
			AddChainSoulFragment(propId, propNum);
			continue;
		}
		else if (propType == ESGS_PROP_DRESS)
		{
			dress.push_back(propId);
			continue;
		}
		else if (propType == ESGS_PROP_MAGIC)
		{
			magics.push_back(propId);
			continue;
		}

		BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(propType, propId);
		PutBackpackProperty(propType, propId, propNum, maxStack, pBackpackModify + pEmailAttachmentsRes->propsNum, SGSU8Sub(maxNum, pEmailAttachmentsRes->propsNum), retNum);
		pEmailAttachmentsRes->propsNum = SGSU8Add(pEmailAttachmentsRes->propsNum, retNum);
	}

	for (BYTE i = 0; i < pEmailAttachmentsRes->propsNum; ++i)
		pEmailAttachmentsRes->props[i].hton();

	//加金币
	UINT64 goldNum = BigNumberToU64(pEmailAttachmentsRes->gold);
	if (goldNum > 0)
		ProduceGold(goldNum, GetGameCfgFileMan()->GetEmailTypeDesc(emailType));
	//SBigNumberAdd(gold, pEmailAttachmentsRes->gold);
	pEmailAttachmentsRes->gold = gold;

	//加代币
	if (pEmailAttachmentsRes->rpcoin > 0)
		ProduceToken(pEmailAttachmentsRes->rpcoin, GetGameCfgFileMan()->GetEmailTypeDesc(emailType));
	//cash = SGSU32Add(cash, pEmailAttachmentsRes->rpcoin);
	pEmailAttachmentsRes->rpcoin = cash;

	//加经验
	isUpgrade = false;
	if (BigNumberToU64(pEmailAttachmentsRes->exp) > 0)
	{
		BYTE newLevel = 0;
		AddRoleExp(pEmailAttachmentsRes->exp);

		//判断是否升级
		GetGameCfgFileMan()->JudgeRoleUpgrade(occuId, level, BigNumberToU64(exp), newLevel);
		if (newLevel > level)
		{
			//开放新技能
			GetGameCfgFileMan()->GetRoleAllInitSkills(occuId, level, newLevel, studySkills);

			//新等级角色hp/mp
			SRoleAttrPoint attr;
			if (MLS_OK == GetGameCfgFileMan()->GetRoleCfgBasicAttr(occuId, newLevel, attr))
			{
				hp = attr.u32HP;
				mp = attr.u32MP;
				attack = attr.attack;
				def = attr.def;
			}

			//新等级
			level = newLevel;
			isUpgrade = true;
		}
	}
	pEmailAttachmentsRes->exp = exp;

	//加体力
	if (pEmailAttachmentsRes->vit > 0)
	{
		UINT32 nVit = SGSU32Add(vit, pEmailAttachmentsRes->vit);
		vit = min(GetGameCfgFileMan()->GetMaxAutoRecoveryVit(), nVit);
	}
	pEmailAttachmentsRes->vit = vit;

	return MLS_OK;
}

int SRoleInfos::MallBuyGoods(UINT32 buyId, UINT16 num, SMallBuyGoodsRes* pBuyGoodsRes, UINT32& spendToken)
{
	MallCfg::SMallGoodsAttr* pGoodsAttr = NULL;
	int retCode = GetGameCfgFileMan()->GetMallBuyGoodsAttr(buyId, pGoodsAttr);
	if (retCode != MLS_OK)
		return retCode;
	MallCfg::SMallGoodsAttr& goodsAttr = *pGoodsAttr;

	//是否过期
	if (goodsAttr.sellDateLimit > 0 && GetMsel() > goodsAttr.sellDateLimit)
		return MLS_DRESS_OUT_DATE;

	//是否达到VIP等级限制
	BYTE vipLv = GetGameCfgFileMan()->CalcVIPLevel(cashcount);
	if (vipLv < goodsAttr.vipLvLimit)
		return MLS_VIP_LV_NOT_ENOUGH;

	//普通折扣
	if (IS_DISCOUNT_EQUAL_ZERO(goodsAttr.discount))
		spendToken = goodsAttr.priceToken * num;
	else
		spendToken = goodsAttr.priceToken * goodsAttr.discount * num / 10;

	//VIP折扣
	BYTE vipDiscount = 100;
	if (vipLv > 0)
	{
		GetGameCfgFileMan()->GetVIPMallBuyDiscount(vipLv, vipDiscount);
		spendToken = spendToken * vipDiscount / 100;
	}

	//代币是否足够
	if (cash < spendToken)
		return MLS_CASH_NOT_ENOUGH;

	switch (goodsAttr.type)
	{
	case ESGS_PROP_DRESS:				//时装
	{
											retCode = GetGameCfgFileMan()->IsJobFashionMatch(occuId, goodsAttr.propId);
											if (retCode != MLS_OK)
												return retCode;

											UINT16 sum = num * goodsAttr.stackNum;
											if (dress.size() + sum > MAX_HAS_DRESS_NUM)
												return MLS_OVER_MAX_DRESS_LIMIT;

											for (UINT16 i = 0; i < sum; ++i)
												dress.push_back(goodsAttr.propId);
											break;
	}
	case ESGS_PROP_FRAGMENT:	// 装备碎片
	{
									UINT16 sum = num * goodsAttr.stackNum;
									AddEquipFragment(goodsAttr.propId, sum);
									break;
	}
	case ESGS_PROP_CHAIN_SOUL_FRAGMENT:	//炼魂碎片
	{
											UINT16 sum = num * goodsAttr.stackNum;
											AddChainSoulFragment(goodsAttr.propId, sum);
											break;
	}
	case ESGS_PROP_EQUIP:				//装备
	case ESGS_PROP_GEM:					//宝石
	case ESGS_PROP_MATERIAL:			//材料
	{
											UINT16 sum = num * goodsAttr.stackNum;
											BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(goodsAttr.type, goodsAttr.propId);
											if (!CanPutIntoBackpack(goodsAttr.propId, sum, maxStack))
												return MLS_BACKPACK_IS_FULL;

											BYTE maxProps = (BYTE)((MAX_RES_USER_BYTES - member_offset(SMallBuyGoodsRes, rpcoin, props)) / sizeof(SPropertyAttr));
											PutBackpackProperty(goodsAttr.type, goodsAttr.propId, sum, maxStack, pBuyGoodsRes->props, maxProps, pBuyGoodsRes->backpackNum);
											for (BYTE i = 0; i < pBuyGoodsRes->backpackNum; ++i)
												pBuyGoodsRes->props[i].hton();
											break;
	}
	}

	//扣除代币
	//cash = SGSU32Sub(cash, spendToken);
	ConsumeToken(spendToken, "商城购买物品");
	pBuyGoodsRes->rpcoin = cash;

	return MLS_OK;
}

UINT16 SRoleInfos::AddEquipFragment(UINT16 fragmentId, UINT16 num)
{
	BYTE si = (BYTE)fragments.size();
	for (BYTE i = 0; i < si; ++i)
	{
		if (fragments[i].fragmentId == fragmentId)
		{
			fragments[i].num = SGSU16Add(fragments[i].num, num);
			return fragments[i].num;
		}
	}

	SEquipFragmentAttr fragmentAttr;
	fragmentAttr.fragmentId = fragmentId;
	fragmentAttr.num = num;
	fragments.push_back(fragmentAttr);
	return num;
}

int SRoleInfos::RoleRevive(UINT16 reviveId, SReviveRoleRes* pReviveRes)
{
	//累加复活次数
	BYTE n = SGSU8Add(reviveTimes, 1);

	//计算VIP增加复活次数
	BYTE vipAddTimes = 0;
	BYTE vipLv = GetGameCfgFileMan()->CalcVIPLevel(cashcount);
	if (vipLv > 0)
		GetGameCfgFileMan()->GetVIPDeadReviveTimes(vipLv, vipAddTimes);

	//获取所需复活币
	UINT32 reviveCoin = 0;
	int retCode = GetGameCfgFileMan()->GetReviveConsume(reviveId, n, vipAddTimes, reviveCoin);
	if (retCode != MLS_OK)
		return retCode;

	//不消耗复活币
	if (reviveCoin == 0)
	{
		pReviveRes->token = cash;
		pReviveRes->num = 0;
		reviveTimes = n;
		return MLS_OK;
	}

	//需要消耗复活币
	UINT32 spendToken = 0;
	UINT32 reviveCoinExchage = 0;
	GetGameCfgFileMan()->GetMaterialFuncValue(ESGS_MATERIAL_REVIVE_COIN, reviveCoinExchage);
	UINT16 allReviveCoinNum = NumberOfProperty(ESGS_MATERIAL_REVIVE_COIN, reviveCoin);
	if (allReviveCoinNum == 0)	//一个复活币都没有
	{
		spendToken = reviveCoinExchage * reviveCoin;
		if (cash < spendToken)
			return MLS_CASH_NOT_ENOUGH;
	}
	else
	{
		if (allReviveCoinNum < reviveCoin)	//复活币不足则需检查代币是否足够
		{
			spendToken = reviveCoinExchage * (reviveCoin - allReviveCoinNum);
			if (cash < spendToken)
				return MLS_CASH_NOT_ENOUGH;
		}

		//扣除复活币
		BYTE maxItemNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SReviveRoleRes, token, data)) / sizeof(SConsumeProperty));
		DropBackpackPropEx(ESGS_MATERIAL_REVIVE_COIN, allReviveCoinNum < reviveCoin ? allReviveCoinNum : reviveCoin, pReviveRes->data, maxItemNum, pReviveRes->num);
	}

	//扣除代币
	if (spendToken > 0)
		ConsumeToken(spendToken, "复活消耗");
	//cash = SGSU32Sub(cash, spendToken);

	reviveTimes = n;
	pReviveRes->token = cash;
	return retCode;
}

int SRoleInfos::GoldExchange(SGoldExchangeRes* pExchangeRes)
{
	//获取vip增加次数
	UINT16 vipAddTimes = 0;
	BYTE vipLv = GetGameCfgFileMan()->CalcVIPLevel(cashcount);
	if (vipLv > 0)
		GetGameCfgFileMan()->GetVIPGoldExchangeTimes(vipLv, vipAddTimes);

	UINT64 getGold = 0;
	UINT32 spendToken = 0;
	int retCode = GetGameCfgFileMan()->GetGoldExchange(exchangedTimes, vipAddTimes, getGold, spendToken);
	if (retCode != MLS_OK)
		return retCode;

	if (cash < spendToken)
		return MLS_CASH_NOT_ENOUGH;

	//扣除代币
	//cash = SGSU32Sub(cash, spendToken);
	ConsumeToken(spendToken, "金币兑换");
	//增加金币
	//SBigNumberAdd(gold, getGold);
	ProduceGold(getGold, "金币兑换");
	//增加兑换次数
	exchangedTimes = SGSU16Add(exchangedTimes, 1);

	//返回数据
	pExchangeRes->exchangedTimes = exchangedTimes;
	pExchangeRes->gold = gold;
	pExchangeRes->rpcoin = cash;

	return retCode;
}

/* zpy 体力兑换 */
int SRoleInfos::VitExchange(SVitExchangeRes* pExchangeRes)
{
	//获取vip增加次数
	UINT16 vipAddTimes = 0;
	BYTE vipLv = GetGameCfgFileMan()->CalcVIPLevel(cashcount);
	if (vipLv > 0)
		GetGameCfgFileMan()->GetVIPVitExchangeTimes(vipLv, vipAddTimes);

	UINT64 getVit = 0;
	UINT32 spendToken = 0;
	int retCode = GetGameCfgFileMan()->GetVitExchange(vitExchangedTimes, vipAddTimes, getVit, spendToken);
	if (retCode != MLS_OK)
		return retCode;

	if (cash < spendToken)
		return MLS_CASH_NOT_ENOUGH;

	//扣除代币
	ConsumeToken(spendToken, "体力兑换");

	//增加体力
	vit = SGSU32Add(vit, getVit);

	//增加兑换次数
	vitExchangedTimes = SGSU16Add(vitExchangedTimes, 1);

	//返回数据
	pExchangeRes->exchangedTimes = vitExchangedTimes;
	pExchangeRes->vit = vit;
	pExchangeRes->rpcoin = cash;

	return retCode;
}

/* zpy 领取每日签到奖励 */
int SRoleInfos::ReceiveDailySignReward(BYTE days, bool continuous, SRoleDailySignRes *pRoleSign, std::vector<SPropertyAttr>& emailAttachmentsVec, SReceiveRewardRefresh *pRefresh)
{
	BYTE VIP = GetGameCfgFileMan()->CalcVIPLevel(cashcount);
	const DailySignCfg::SDailySignItem *pDailySignItem = NULL;
	if (GetGameCfgFileMan()->GetDailySignConfig(days, pDailySignItem))
	{
		BYTE maxProps = (BYTE)((MAX_RES_USER_BYTES - member_offset(SRoleDailySignRes, rpcoin, data)) / sizeof(SPropertyAttr));
		if (pDailySignItem->continuous == false || continuous)
		{
			for (size_t i = 0; i < pDailySignItem->award.size(); ++i)
			{
				UINT32 num = pDailySignItem->award[i].num;
				UINT16 propId = pDailySignItem->award[i].id;
				BYTE propType = pDailySignItem->award[i].type;

				// 双倍奖励
				if (pDailySignItem->double_vip_level != 0 && VIP >= pDailySignItem->double_vip_level)
				{
					num = num * 2;
				}

				switch (propType)
				{
				case ESGS_PROP_GOLD:					// 金币
					ProduceGold(num, "每日签到奖励");
					pRefresh->gold = true;
					break;
				case ESGS_PROP_TOKEN:					// 代币
					ProduceToken(num, "每日签到奖励");
					pRefresh->rpcoin = true;
					break;
				case ESGS_PROP_VIT:						// 体力
					vit = SGSU32Add(vit, num);
					pRefresh->vit = true;
					break;
				case ESGS_PROP_EXP:						// 经验
					AddRoleExp(num);
					pRefresh->exp = true;
					break;
				case ESGS_PROP_FRAGMENT:				// 装备碎片
					AddEquipFragment(propId, num);
					pRefresh->fragment = true;
					break;
				case ESGS_PROP_CHAIN_SOUL_FRAGMENT:		// 炼魂碎片
					AddChainSoulFragment(propId, num);
					pRefresh->chainSoulFragment = true;
					break;
				case ESGS_PROP_EQUIP:					// 装备
				case ESGS_PROP_GEM:						// 宝石
				case ESGS_PROP_MATERIAL:				// 材料
				{
															BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(propType, propId);
															if (!CanPutIntoBackpack(propId, num, maxStack))
															{
																//背包放不下, 发邮件附件
																SPropertyAttr attachmentProp;
																memset(&attachmentProp, 0, sizeof(SPropertyAttr));
																int retCode = GetGameCfgFileMan()->GetPropertyAttr(propType, propId, attachmentProp, maxStack);
																if (retCode == MLS_OK)
																{
																	attachmentProp.num = num;
																	emailAttachmentsVec.push_back(attachmentProp);
																}
															}
															else
															{
																BYTE retNum = 0;
																PutBackpackProperty(propType, propId, num, maxStack, pRoleSign->data + pRoleSign->num, SGSU8Sub(maxProps, pRoleSign->num), retNum);
																pRoleSign->num = SGSU8Add(pRoleSign->num, retNum);
																pRefresh->prop = true;
															}
				}
					break;
				case ESGS_PROP_MAGIC:					//魔导器
					magics.push_back(propId);
					pRefresh->magics = true;
					break;
				case ESGS_PROP_DRESS:					//时装
					dress.push_back(propId);
					pRefresh->dress = true;
					break;
				default:
					break;
				}
			}
		}
	}

	pRoleSign->vit = vit;
	pRoleSign->rpcoin = cash;
	pRoleSign->exp.hi = exp.hi;
	pRoleSign->exp.lo = exp.lo;
	pRoleSign->gold.hi = gold.hi;
	pRoleSign->gold.lo = gold.lo;

	// 获取经验后是否升级
	if (pRefresh->exp)
	{
		BYTE newLv = 0;
		GetGameCfgFileMan()->JudgeRoleUpgrade(occuId, level, BigNumberToU64(exp), newLv);
		if (newLv > level)
		{
			GetGameCfgFileMan()->GetRoleAllInitSkills(occuId, level, newLv, studySkills);
			level = newLv;
			pRefresh->upgrade = true;
		}
	}

	return MLS_OK;
}

//add by hxw 20151022
BOOL SRoleInfos::IsGetStrongerReward(UINT16 rewardid)
{
	int len = strongerVec.size();

	for (int i = 0; i<len; i++)
	{
		if (rewardid == strongerVec[i])
			return true;
	}
	return false;
}
BOOL SRoleInfos::IsGetOnlineReward(UINT16 rewardid)
{
	int len = onlines.size();

	for (int i = 0; i<len; i++)
	{
		if (rewardid == onlines[i])
			return true;
	}
	return false;
}

int SRoleInfos::GetOnlineInfo(SOnlineInfo* info, std::vector<UINT16>& vonline)
{
	vonline.clear();
	info->id = onlineVec.id;
	info->itmes = onlineVec.itmes;
	info->lasttime = onlineVec.lasttime;
	int len = onlines.size();
	for (int i = 0; i<len; i++)
	{
		vonline.push_back(onlines[i]);
	}
	return MLS_OK;
}

int SRoleInfos::GetStrongerInfo(std::vector<UINT16>& infov)
{
	infov.clear();
	for (int i = 0; i<strongerVec.size(); i++)
	{
		infov.push_back(strongerVec[i]);
	}
	return MLS_OK;
}

int SRoleInfos::InputStrongerReward(UINT16 rewardid)
{
	for (int i = 0; i<strongerVec.size(); i++)
	{
		if (rewardid == strongerVec[i])
		{
			return MLS_OK;
		}
	}
	strongerVec.push_back(rewardid);
	return MLS_OK;
}
int SRoleInfos::InputOnlineReward(UINT16 rewardid)
{
	for (int i = 0; i<onlines.size(); i++)
	{
		if (rewardid == onlines[i])
		{
			return MLS_OK;
		}
	}
	onlines.push_back(rewardid);
	return MLS_OK;
}
int SRoleInfos::SetOnlineInfo(UINT32 id, UINT32 time, LTMSEL lasttime)
{
	if (id != 0)
	{
		onlineVec.id = id;
		onlineVec.itmes = time;
		if (lasttime == 0)
			onlineVec.lasttime = LocalStrTimeToMSel(lastEnterTime);
		else
			onlineVec.lasttime = lasttime;
	}
	else
	{
		if (lasttime>0)
		{
			onlineVec.itmes += (lasttime - onlineVec.lasttime) / 1000;
			onlineVec.lasttime = lasttime;

		}
	}

	return MLS_OK;
}


//add by hxw 20151015 Rank奖励通用领取
int SRoleInfos::ReceiveRankReward(BYTE type, UINT16 rewardid, SGetRankRewardRes *pRoleSign, \
	std::vector<SPropertyAttr>& emailAttachmentsVec, SReceiveRewardRefresh *pRefresh)
{
	pRoleSign->num = 0;
	pRoleSign->vit = 0;
	pRoleSign->rpcoin = 0;
	pRoleSign->exp.hi = 0;
	pRoleSign->exp.lo = 0;
	pRoleSign->gold.hi = 0;
	pRoleSign->gold.lo = 0;

	CRankReward::SRkRewardRes* items = NULL;

	GetGameCfgFileMan()->GetRankReward(type, rewardid, items);

	if (items == NULL)
		return MLS_GET_RANK_REWARD_CONFIG_FAIL;


	std::vector<SDropPropCfgItem>::iterator it = items->rewardvs.begin();


	for (; it != items->rewardvs.end(); it++)
	{
		BYTE maxProps = (BYTE)((MAX_RES_USER_BYTES - member_offset(SGetRankRewardRes, rpcoin, data)) / sizeof(SPropertyAttr));

		BYTE propType = it->type;
		switch (propType)
		{
		case ESGS_PROP_GOLD:
			ProduceGold(it->num, "奖励");
			pRefresh->gold = true;
			break;
		case ESGS_PROP_TOKEN:
			ProduceToken(it->num, "奖励");
			pRefresh->rpcoin = true;
			break;
		case ESGS_PROP_EXP:						// 经验
			AddRoleExp(it->num);
			pRefresh->exp = true;
			break;
		case ESGS_PROP_FRAGMENT:
			AddEquipFragment(it->propId, it->num);
			pRefresh->fragment = true;
			break;
		case ESGS_PROP_CHAIN_SOUL_FRAGMENT:
			AddChainSoulFragment(it->propId, it->num);
			pRefresh->chainSoulFragment = true;
			break;
		case ESGS_PROP_EQUIP:
		case ESGS_PROP_GEM:
		case ESGS_PROP_MATERIAL:
		{
								   BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(it->type, it->propId);
								   if (!CanPutIntoBackpack(it->propId, it->num, maxStack))
								   {
									   //背包放不下, 发邮件附件
									   SPropertyAttr attachmentProp;
									   memset(&attachmentProp, 0, sizeof(SPropertyAttr));
									   int retCode = GetGameCfgFileMan()->GetPropertyAttr(propType, it->propId, attachmentProp, maxStack);
									   if (retCode == MLS_OK)
									   {
										   attachmentProp.num = it->num;
										   emailAttachmentsVec.push_back(attachmentProp);
									   }
								   }
								   else
								   {
									   BYTE retNum = 0;
									   PutBackpackProperty(propType, it->propId, it->num, maxStack, pRoleSign->data + pRoleSign->num, SGSU8Sub(maxProps, pRoleSign->num), retNum);
									   pRoleSign->num = SGSU8Add(pRoleSign->num, retNum);
									   pRefresh->prop = true;
								   }
		}
			break;
		default:
			break;
		}
	}

	// 获取经验后是否升级
	if (pRefresh->exp)
	{
		BYTE newLv = 0;
		GetGameCfgFileMan()->JudgeRoleUpgrade(occuId, level, BigNumberToU64(exp), newLv);
		if (newLv > level)
		{
			GetGameCfgFileMan()->GetRoleAllInitSkills(occuId, level, newLv, studySkills);
			level = newLv;
			pRefresh->upgrade = true;
		}
	}
	pRoleSign->vit = vit;
	pRoleSign->rpcoin = cash;
	pRoleSign->exp.hi = exp.hi;
	pRoleSign->exp.lo = exp.lo;
	pRoleSign->gold.hi = gold.hi;
	pRoleSign->gold.lo = gold.lo;
	return MLS_OK;
}
//end 20151015

int SRoleInfos::ReceiveVIPReward(BYTE vipLv, SReceiveVIPRewardRes* pVIPRewardRes, std::vector<SPropertyAttr>& emailAttachmentsVec, SReceiveRewardRefresh* pRefresh)
{
	//计算VIP等级
	BYTE VIP = GetGameCfgFileMan()->CalcVIPLevel(cashcount);
	if (vipLv > VIP)
		return MLS_VIP_LV_NOT_ENOUGH;

	//获取VIP奖励
	std::vector<VIPCfg::SVIPGiveProp>* pVIPRewardVec = NULL;
	GetGameCfgFileMan()->GetVIPGiveProps(vipLv, pVIPRewardVec);
	if (pVIPRewardVec == NULL)
		return MLS_VIP_LV_NOT_ENOUGH;

	//发放VIP奖励
	std::vector<VIPCfg::SVIPGiveProp>& vipRewardVec = *pVIPRewardVec;
	BYTE maxProps = (BYTE)((MAX_RES_USER_BYTES - member_offset(SReceiveVIPRewardRes, rpcoin, data)) / sizeof(SPropertyAttr));
	BYTE si = (BYTE)vipRewardVec.size();
	for (BYTE i = 0; i < si; ++i)
	{
		UINT32 num = vipRewardVec[i].num;
		UINT16 propId = vipRewardVec[i].propId;
		BYTE propType = vipRewardVec[i].type;
		switch (propType)
		{
		case ESGS_PROP_GOLD:
			//SBigNumberAdd(gold, num);
			ProduceGold(num, "VIP奖励");
			pRefresh->gold = true;
			break;
		case ESGS_PROP_TOKEN:
			//cash = SGSU32Add(cash, num);
			ProduceToken(num, "VIP奖励");
			pRefresh->rpcoin = true;
			break;
		case ESGS_PROP_FRAGMENT:
			AddEquipFragment(propId, num);
			pRefresh->fragment = true;
			break;
		case ESGS_PROP_CHAIN_SOUL_FRAGMENT:
			AddChainSoulFragment(propId, num);
			pRefresh->chainSoulFragment = true;
			break;
		case ESGS_PROP_EQUIP:
		case ESGS_PROP_GEM:
		case ESGS_PROP_MATERIAL:
		{
								   BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(propType, propId);
								   if (!CanPutIntoBackpack(propId, num, maxStack))
								   {
									   //背包放不下, 发邮件附件
									   SPropertyAttr attachmentProp;
									   memset(&attachmentProp, 0, sizeof(SPropertyAttr));
									   int retCode = GetGameCfgFileMan()->GetPropertyAttr(propType, propId, attachmentProp, maxStack);
									   if (retCode == MLS_OK)
									   {
										   attachmentProp.num = num;
										   emailAttachmentsVec.push_back(attachmentProp);
									   }
								   }
								   else
								   {
									   BYTE retNum = 0;
									   PutBackpackProperty(propType, propId, num, maxStack, pVIPRewardRes->data + pVIPRewardRes->num, SGSU8Sub(maxProps, pVIPRewardRes->num), retNum);
									   pVIPRewardRes->num = SGSU8Add(pVIPRewardRes->num, retNum);
									   pRefresh->prop = true;
								   }
		}
			break;
		default:
			break;
		}
	}
	return MLS_OK;
}

/* zpy 成就系统新增 */
int SRoleInfos::ReceiveAchievementReward(UINT32 achievement_id, SReceiveRewardRes *pAchievementReward, std::vector<SPropertyAttr>& emailAttachmentsVec, SReceiveRewardRefresh *pRefresh)
{
	AchievementCfg::SAchievementReward reward;
	if (GetGameCfgFileMan()->GetAchievementReward(achievement_id, reward))
	{
		BYTE maxProps = (BYTE)((MAX_RES_USER_BYTES - member_offset(SReceiveRewardRes, gold, data)) / sizeof(SPropertyAttr));
		for (size_t i = 0; i < reward.reward.size(); ++i)
		{
			UINT32 num = reward.reward[i].num;
			UINT16 propId = reward.reward[i].id;
			BYTE propType = reward.reward[i].type;

			switch (propType)
			{
			case ESGS_PROP_GOLD:					// 金币
				ProduceGold(num, "成就系统奖励");
				pRefresh->gold = true;
				break;
			case ESGS_PROP_TOKEN:					// 代币
				ProduceToken(num, "成就系统奖励");
				pRefresh->rpcoin = true;
				break;
			case ESGS_PROP_VIT:						// 体力
				vit = SGSU32Add(vit, num);
				pRefresh->vit = true;
				break;
			case ESGS_PROP_EXP:						// 经验
				AddRoleExp(num);
				pRefresh->exp = true;
				break;
			case ESGS_PROP_FRAGMENT:				// 装备碎片
				AddEquipFragment(propId, num);
				pRefresh->fragment = true;
				break;
			case ESGS_PROP_CHAIN_SOUL_FRAGMENT:		// 炼魂碎片
				AddChainSoulFragment(propId, num);
				pRefresh->chainSoulFragment = true;
				break;
			case ESGS_PROP_EQUIP:					// 装备
			case ESGS_PROP_GEM:						// 宝石
			case ESGS_PROP_MATERIAL:				// 材料
			{
														BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(propType, propId);
														if (!CanPutIntoBackpack(propId, num, maxStack))
														{
															//背包放不下, 发邮件附件
															SPropertyAttr attachmentProp;
															memset(&attachmentProp, 0, sizeof(SPropertyAttr));
															int retCode = GetGameCfgFileMan()->GetPropertyAttr(propType, propId, attachmentProp, maxStack);
															if (retCode == MLS_OK)
															{
																attachmentProp.num = num;
																emailAttachmentsVec.push_back(attachmentProp);
															}
														}
														else
														{
															BYTE retNum = 0;
															PutBackpackProperty(propType, propId, num, maxStack, pAchievementReward->data + pAchievementReward->num, SGSU8Sub(maxProps, pAchievementReward->num), retNum);
															pAchievementReward->num = SGSU8Add(pAchievementReward->num, retNum);
															pRefresh->prop = true;
														}
			}
				break;
			case ESGS_PROP_MAGIC:					//魔导器
				magics.push_back(propId);
				pRefresh->magics = true;
				break;
			case ESGS_PROP_DRESS:					//时装
				dress.push_back(propId);
				pRefresh->dress = true;
				break;
			default:
				break;
			}
		}

		// 获取经验后是否升级
		if (pRefresh->exp)
		{
			BYTE newLv = 0;
			GetGameCfgFileMan()->JudgeRoleUpgrade(occuId, level, BigNumberToU64(exp), newLv);
			if (newLv > level)
			{
				GetGameCfgFileMan()->GetRoleAllInitSkills(occuId, level, newLv, studySkills);
				level = newLv;
				pRefresh->upgrade = true;
			}
		}

		pAchievementReward->rpcoin = cash;
		pAchievementReward->gold.hi = gold.hi;
		pAchievementReward->gold.lo = gold.lo;
		pAchievementReward->exp.hi = exp.hi;
		pAchievementReward->exp.lo = exp.lo;
		pAchievementReward->vit = vit;
	}
	return MLS_OK;
}

/* zpy 限时活动奖励 */
int SRoleInfos::ReceiveLimitedTimeActivityReward(const std::vector<SPropertyItem> &rewardVec, SFinishInstRes* pFinishInstRes, std::vector<SPropertyAttr> emailAttachmentsVec, SReceiveRewardRefresh *pRefresh)
{
	BYTE maxProps = (BYTE)((MAX_RES_USER_BYTES - member_offset(SFinishInstRes, rpcoin, data)) / sizeof(SPropertyAttr));
	for (size_t i = 0; i < rewardVec.size(); ++i)
	{
		UINT32 num = rewardVec[i].num;
		UINT16 propId = rewardVec[i].propId;
		BYTE propType = rewardVec[i].type;

		switch (propType)
		{
		case ESGS_PROP_GOLD:					// 金币
			ProduceGold(num, "限时活动奖励");
			pRefresh->gold = true;
			break;
		case ESGS_PROP_TOKEN:					// 代币
			ProduceToken(num, "限时活动奖励");
			pRefresh->rpcoin = true;
			break;
		case ESGS_PROP_VIT:						// 体力
			vit = SGSU32Add(vit, num);
			pRefresh->vit = true;
			break;
		case ESGS_PROP_EXP:						// 经验
			AddRoleExp(num);
			pRefresh->exp = true;
			break;
		case ESGS_PROP_FRAGMENT:				// 装备碎片
			AddEquipFragment(propId, num);
			pRefresh->fragment = true;
			break;
		case ESGS_PROP_CHAIN_SOUL_FRAGMENT:		// 炼魂碎片
			AddChainSoulFragment(propId, num);
			pRefresh->chainSoulFragment = true;
			break;
		case ESGS_PROP_EQUIP:					// 装备
		case ESGS_PROP_GEM:						// 宝石
		case ESGS_PROP_MATERIAL:				// 材料
		{
													BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(propType, propId);
													if (!CanPutIntoBackpack(propId, num, maxStack))
													{
														//背包放不下, 发邮件附件
														SPropertyAttr attachmentProp;
														memset(&attachmentProp, 0, sizeof(SPropertyAttr));
														int retCode = GetGameCfgFileMan()->GetPropertyAttr(propType, propId, attachmentProp, maxStack);
														if (retCode == MLS_OK)
														{
															attachmentProp.num = num;
															emailAttachmentsVec.push_back(attachmentProp);
														}
													}
													else
													{
														BYTE retNum = 0;
														PutBackpackProperty(propType, propId, num, maxStack, pFinishInstRes->data + pFinishInstRes->num, SGSU8Sub(maxProps, pFinishInstRes->num), retNum);
														pFinishInstRes->num = SGSU8Add(pFinishInstRes->num, retNum);
														pRefresh->prop = true;
													}
		}
			break;
		case ESGS_PROP_MAGIC:					//魔导器
			magics.push_back(propId);
			pRefresh->magics = true;
			break;
		case ESGS_PROP_DRESS:					//时装
			dress.push_back(propId);
			pRefresh->dress = true;
			break;
		default:
			break;
		}
	}

	// 获取经验后是否升级
	if (pRefresh->exp)
	{
		BYTE newLv = 0;
		GetGameCfgFileMan()->JudgeRoleUpgrade(occuId, level, BigNumberToU64(exp), newLv);
		if (newLv > level)
		{
			GetGameCfgFileMan()->GetRoleAllInitSkills(occuId, level, newLv, studySkills);
			level = newLv;
			pRefresh->upgrade = true;
		}
	}

	pFinishInstRes->rpcoin = cash;
	pFinishInstRes->gold.hi = gold.hi;
	pFinishInstRes->gold.lo = gold.lo;
	pFinishInstRes->exp.hi = exp.hi;
	pFinishInstRes->exp.lo = exp.lo;
	pFinishInstRes->vit = vit;

	return MLS_OK;
}

int SRoleInfos::EquipPosStrengthen(BYTE pos, SEquipPosStrengthenRes* pStrengthenRes)
{
	int idx = -1;
	BYTE newStrengthenLv = 1;
	BYTE n = (BYTE)equipPos.size();
	for (BYTE i = 0; i < n; ++i)
	{
		if (equipPos[i].pos == pos)
		{
			idx = i;
			newStrengthenLv = SGSU8Add(equipPos[i].lv, 1);
			break;
		}
	}

	if (idx < 0)
		return MLS_UNKNOWN_EQUIP_POS;

	if (newStrengthenLv > level)		//超过角色等级
		return MLS_MAX_STRENGTHEN_LEVEL;

	GameCfgFileMan::SEquipStrengthenSpend spend;
	int retCode = GetGameCfgFileMan()->GetEquipStrengthenSpend(pos, newStrengthenLv, spend);
	if (retCode != MLS_OK)
		return retCode;

	UINT64 curGold = BigNumberToU64(gold);
	if (curGold < spend.gold)
		return MLS_GOLD_NOT_ENOUGH;		//金币不足

	if (!IsExistProperty(ESGS_MATERIAL_STRENGTHEN_STONE, spend.stones))
		return MLS_MATERIAL_NOT_ENOUGH;	//消耗材料数量不足

	//扣金币
	if (spend.gold > 0)
		ConsumeGold(spend.gold, "装备位强化");
	//SBigNumberSub(gold, spend.gold);

	//扣消耗材料
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SEquipPosStrengthenRes, newStrengthenLv, consumeStones)) / sizeof(SConsumeProperty));
	DropBackpackPropEx(ESGS_MATERIAL_STRENGTHEN_STONE, spend.stones, pStrengthenRes->consumeStones, maxNum, pStrengthenRes->num);

	equipPos[idx].lv = newStrengthenLv;		//新的强化等级
	pStrengthenRes->gold = gold;
	pStrengthenRes->newStrengthenLv = newStrengthenLv;

	//重新计算战力
	CalcRoleCombatPower();

	return MLS_OK;
}

int SRoleInfos::EquipPosRiseStar(BYTE pos, const SRiseStarAttachParam& param, SEquipPosRiseStarRes* pRiseStarRes)
{
	int idx = -1;
	BYTE riseStar = 1;
	BYTE n = (BYTE)equipPos.size();
	for (BYTE i = 0; i < n; ++i)
	{
		if (equipPos[i].pos == pos)
		{
			idx = i;
			riseStar = SGSU8Add(equipPos[i].star, 1);
			break;
		}
	}

	if (idx < 0)
		return MLS_UNKNOWN_EQUIP_POS;

	//是否超过最大星级
	if (riseStar > GetGameCfgFileMan()->GetEquipMaxStarLevel())
		return MLS_MAX_STAR_LEVEL;

	//升星配置
	RiseStarCfg::SRiseStarCfg cfg;
	int retCode = GetGameCfgFileMan()->GetRiseStarCfg(riseStar, cfg);
	if (retCode != MLS_OK)
		return retCode;
	if (level < cfg.limitLevel)
		return MLS_STAR_LIMIT_LEVEL;

	//获取升星花费
	RiseStarCfg::SRiseStarSpend spend;
	retCode = GetGameCfgFileMan()->GetRiseStarSpend(riseStar, spend);
	if (retCode != MLS_OK)
		return retCode;

	UINT64 curGold = BigNumberToU64(gold);
	if (curGold < spend.gold)
		return MLS_GOLD_NOT_ENOUGH;		//金币不足

	if (!IsExistProperty(ESGS_MATERIAL_VEINS_STAR_STONE, spend.stones))
		return MLS_MATERIAL_NOT_ENOUGH;	//消耗材料(纹星石)数量不足

	//光芒石(降低破损率)
	BYTE decDamagedChance = 0;
	if (param.decDamagedChance > 0)
	{
		if (!IsExistProperty(ESGS_MATERIAL_SHINE_STONE, param.decDamagedChance))
			return MLS_MATERIAL_NOT_ENOUGH;	//消耗材料(光芒石)数量不足

		UINT32 nVal = 0;
		GetGameCfgFileMan()->GetMaterialFuncValue(ESGS_MATERIAL_SHINE_STONE, nVal);
		BYTE basicDecDamagedVal = nVal;

		decDamagedChance = basicDecDamagedVal;
		for (BYTE i = 1; i < param.decDamagedChance; ++i)
		{
			BYTE n = (basicDecDamagedVal + 1) >> i;
			//n = max(1, n);
			decDamagedChance = SGSU8Add(decDamagedChance, n);
			if (n == 1)
				break;
		}
	}

	//辉煌石(增加成功率)
	BYTE addSuccedChance = 0;
	if (param.addSuccedChance > 0)
	{
		if (!IsExistProperty(ESGS_MATERIAL_BRILLIANT_STONE, param.addSuccedChance))
			return MLS_MATERIAL_NOT_ENOUGH;	//消耗材料(辉煌石)数量不足

		UINT32 nVal = 0;
		GetGameCfgFileMan()->GetMaterialFuncValue(ESGS_MATERIAL_BRILLIANT_STONE, nVal);
		BYTE basicAddSuccedVal = nVal;

		addSuccedChance = basicAddSuccedVal;
		for (BYTE i = 1; i < param.addSuccedChance; ++i)
		{
			BYTE n = (basicAddSuccedVal + 1) >> i;
			//n = max(1, n);
			addSuccedChance = SGSU8Add(addSuccedChance, n);
			if (n == 1)
				break;
		}
	}

	//升星
	BYTE maxSuccessChance = GetGameCfgFileMan()->GetRiseStarMaxSuccessChance();
	BYTE succedChance = min(maxSuccessChance, SGSU8Add(cfg.succedChance, (BYTE)addSuccedChance));
	if (rand() % 100 < succedChance)	//成功
	{
		equipPos[idx].star = riseStar;
	}
	else
	{
		BYTE minDamagedChance = GetGameCfgFileMan()->GetRiseStarMinDamageChance();
		BYTE damagedChance = max(minDamagedChance, SGSU8Sub(cfg.damagedChance, (BYTE)decDamagedChance));
		if (rand() % 100 < damagedChance)	//破损降级
		{
			equipPos[idx].star = equipPos[idx].star > 0 ? SGSU8Sub(equipPos[idx].star, 1) : 0;
		}
	}
	pRiseStarRes->newStarLv = equipPos[idx].star;

	//扣金币
	if (spend.gold > 0)
		ConsumeGold(spend.gold, "装备位升星");
	//SBigNumberSub(gold, spend.gold);
	pRiseStarRes->gold = gold;

	//扣纹星石
	BYTE retNum = 0;
	pRiseStarRes->num = 0;
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SEquipPosRiseStarRes, newStarLv, consumeStones)) / sizeof(SConsumeProperty));
	DropBackpackPropEx(ESGS_MATERIAL_VEINS_STAR_STONE, spend.stones, pRiseStarRes->consumeStones, maxNum, retNum);
	pRiseStarRes->num = retNum;

	//扣光芒石(降低破损率)
	if (param.decDamagedChance > 0/* && maxNum > pRiseStarRes->num*/)
	{
		retNum = 0;
		BYTE n = SGSU8Sub(maxNum, pRiseStarRes->num);
		SConsumeProperty* p = pRiseStarRes->consumeStones + pRiseStarRes->num;
		DropBackpackPropEx(ESGS_MATERIAL_SHINE_STONE, param.decDamagedChance, p, n, retNum);
		pRiseStarRes->num += retNum;
	}

	//扣辉煌石(增加成功率)
	if (param.addSuccedChance > 0/* && maxNum > pRiseStarRes->num*/)
	{
		retNum = 0;
		BYTE n = SGSU8Sub(maxNum, pRiseStarRes->num);
		SConsumeProperty* p = pRiseStarRes->consumeStones + pRiseStarRes->num;
		DropBackpackPropEx(ESGS_MATERIAL_BRILLIANT_STONE, param.addSuccedChance, p, n, retNum);
		pRiseStarRes->num += retNum;
	}

	return MLS_OK;
}

void SRoleInfos::ProduceGold(UINT64 goldNum, const char* pszAction, BOOL recordLog/* = true*/)
{
	SBigNumberAdd(gold, goldNum);
	if (recordLog)
		GetConsumeProduceMan()->InputProduceRecord(ESGS_GOLD, roleId, nick, goldNum, pszAction, BigNumberToU64(gold));
}

void SRoleInfos::ConsumeGold(UINT64 goldNum, const char* pszAction, BOOL recordLog/* = true*/)
{
	SBigNumberSub(gold, goldNum);
	if (recordLog)
		GetConsumeProduceMan()->InputConsumeRecord(ESGS_GOLD, roleId, nick, goldNum, pszAction, BigNumberToU64(gold));
}

void SRoleInfos::ProduceToken(UINT32 tokenNum, const char* pszAction, BOOL recordLog/* = true*/)
{
	cash = SGSU32Add(cash, tokenNum);
	if (recordLog)
		GetConsumeProduceMan()->InputProduceRecord(ESGS_TOKEN, roleId, nick, tokenNum, pszAction, cash);
}

void SRoleInfos::ConsumeToken(UINT32 tokenNum, const char* pszAction, BOOL recordLog/* = true*/)
{
	cash = SGSU32Sub(cash, tokenNum);
	if (recordLog)
		GetConsumeProduceMan()->InputConsumeRecord(ESGS_TOKEN, roleId, nick, tokenNum, pszAction, cash);
}

// zpy 获取炼魂碎片数量
UINT16 SRoleInfos::GetChainSoulFragmentNum(UINT16 fragId)
{
	for (size_t i = 0; i < chainSoulFragments.size(); ++i)
	{
		if (chainSoulFragments[i].fragmentId == fragId)
		{
			return chainSoulFragments[i].num;
		}
	}
	return 0;
}

// zpy 扣除炼魂碎片
void SRoleInfos::DropChainSoulFragment(UINT16 fragId, UINT16 num)
{
	for (std::vector<SChainSoulFragmentAttr>::iterator itr = chainSoulFragments.begin(); itr != chainSoulFragments.end(); ++itr)
	{
		if (itr->fragmentId == fragId)
		{
			if (itr->num <= num)
			{
				chainSoulFragments.erase(itr);
			}
			else
			{
				itr->num = SGSU16Sub(itr->num, num);
			}
			break;
		}
	}
}

// zpy 增加炼魂碎片
UINT16 SRoleInfos::AddChainSoulFragment(UINT16 fragmentId, UINT16 num)
{
	for (size_t i = 0; i < chainSoulFragments.size(); ++i)
	{
		if (chainSoulFragments[i].fragmentId == fragmentId)
		{
			chainSoulFragments[i].num = SGSU16Add(chainSoulFragments[i].num, num);
			return chainSoulFragments[i].num;
		}
	}

	SChainSoulFragmentAttr fragmentAttr;
	fragmentAttr.fragmentId = fragmentId;
	fragmentAttr.num = num;
	chainSoulFragments.push_back(fragmentAttr);
	return num;
}

// zpy 初始化已拥有炼魂碎片
void SRoleInfos::InitHasChainSoulFragment(const BYTE* pData, int nDataLen)
{
	chainSoulFragments.clear();
	if (nDataLen <= 0) return;

	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SChainSoulFragmentAttr));
	nCount = min(nCount, *(BYTE*)pData);
	SChainSoulFragmentAttr* pFragmentAttr = (SChainSoulFragmentAttr*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
		chainSoulFragments.push_back(pFragmentAttr[i]);
}

// zpy 初始化炼魂部件
void SRoleInfos::InitChainSoulPos(const BYTE* pData, int nDataLen)
{
	chainSoulPos.clear();
	if (nDataLen <= 0) return;

	BYTE nCount = (BYTE)((nDataLen - sizeof(BYTE)) / sizeof(SChainSoulPosAttr));
	nCount = min(nCount, *(BYTE*)pData);
	ASSERT(nCount == MAX_CHAIN_SOUL_HOLE_NUM);
	SChainSoulPosAttr* pAttr = (SChainSoulPosAttr*)(pData + sizeof(BYTE));
	for (BYTE i = 0; i < nCount; ++i)
		chainSoulPos.push_back(pAttr[i]);
}
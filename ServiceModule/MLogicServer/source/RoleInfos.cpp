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
		if (lastEnterMSel > nowMsel)	//ϵͳ��ʱ���� ???
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
			case ESGS_TASK_TARGET_ESCORT:			//����
				pTask[i].target[2] = 0;	//Ѫ��
				break;

			case ESGS_TASK_TARGET_PASS_NO_HURT:	//����ͨ��
				pTask[i].target[1] = 0;	//Ѫ��
				break;

			case ESGS_TASK_TARGET_LIMIT_TIME:		//��ʱͨ��
				pTask[i].target[1] = 0;	//ʱ��
				break;

			case ESGS_TASK_TARGET_KILL_MONSTER_LIMIT_TIME:	//��ʱɱ��
			case ESGS_TASK_TARGET_DESTROY_SCENE_LIMIT_TIME:	//��ʱ�ƻ������������
				pTask[i].target[2] = 0;	//ʱ��
				pTask[i].target[3] = 0;	//����
				break;

			case ESGS_TASK_TARGET_FETCH_PROP_LIMIT_TIME:	//��ʱ��ȡ���ߣ�������
				pTask[i].target[3] = 0;	//ʱ��
				pTask[i].target[4] = 0;	//����
				break;

			case ESGS_TASK_TARGET_BLOCK_TIMES:		//�񵲶��ٴ�
			case ESGS_TASK_TARGET_COMBO_NUM_REACH:	//�ﵽ�㹻���������
			case ESGS_TASK_TARGET_KEEP_BLOOD:		//Ѫ������
				pTask[i].target[1] = 0;	//����
				break;

			default:
				break;
			}
		}

		BYTE type = pTaskInfo->type;
		if (type != ESGS_SYS_DAILY_TASK)		//���ճ�����
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

// zpy �������Ѵ���װ��װ�����Ƿ����ĳ��Ʒ
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

// zpy ��ʼ��������Ϣ
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

// zpy ���ӽ�ɫ����
SBigNumber SRoleInfos::AddRoleExp(UINT64 add_exp)
{
	// ��ɫ�ȼ�δ�ﵽ���޲����Ӿ���
	if (level < GetGameCfgFileMan()->GetMaxRoleLevel())
	{
		SBigNumberAdd(exp, add_exp);
	}
	return exp;
}

SBigNumber SRoleInfos::AddRoleExp(SBigNumber add_exp)
{
	// ��ɫ�ȼ�δ�ﵽ���޲����Ӿ���
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

	//���ұ������Ƿ��и���Ʒ
	std::vector<SPropertyAttr>::iterator it = packs.begin();
	for (; it != packs.end(); ++it)
	{
		if (it->id == id)
		{
			/*
			* 2016.1.8 zpy �޸�
			* pyg�汾�ǽ�����ȫ�����ۣ���δʹ�ÿͻ��������ĳ�������
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

	//��ȡ��Ʒ���۵���
	UINT64 price = 0;	//����
	int retCode = GetGameCfgFileMan()->GetPropertyUserSellPrice(propAttr.type, propAttr.propId, price);
	if (retCode != MLS_OK)
		return retCode;

	if (price == 0)	//��Ʒ���������
		return MLS_PROP_NOT_ALLOW_SELL;

	//���ӳ������ý��
	UINT64 sellGold = price * num;
	ProduceGold(sellGold, "������Ʒ");
	//SBigNumberAdd(gold, sellGold);

	//�ӱ����۳�������Ʒ
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

	//��ȡ�ع��۸�
	UINT64 price = 0;
	int retCode = GetGameCfgFileMan()->GetPropertyUserBuyBackPrice(buyProp.type, buyProp.propId, price);
	if (retCode != MLS_OK)
		return retCode;

	UINT64 spendPrice = price * buyProp.num;
	if (BigNumberToU64(gold) < spendPrice)
		return MLS_GOLD_NOT_ENOUGH;		//��Ҳ���

	//��ȡ��Ʒ�ѵ�����
	BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(buyProp.type, buyProp.propId);

	//��������
	BYTE buyNum = min(buyProp.num, num);

	//�����Ƿ��ܷ���
	if (!CanPutIntoBackpack(buyProp.propId, buyNum, maxStack))
		return MLS_BACKPACK_IS_FULL;

	//�۳����
	//SBigNumberSub(gold, spendPrice);
	ConsumeGold(spendPrice, "�ع���Ʒ");
	pBuyBackRes->gold = gold;

	//���뱳��
	if (buyProp.type == ESGS_PROP_EQUIP)	//װ��
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

	/* zpy �ɾ�ϵͳ���� */
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

		BYTE n = SGSU8Sub(maxStack, packs[i].num);	//�ø��ӻ��ܷŶ���
		if (n > 0)
		{
			UINT16 notPutNum = SGSU16Sub(num, putNum);	//��ʣ����δ��
			BYTE inPackNum = min(notPutNum, n);			//�ôη����������
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

	UINT16 nFreeNum = SGSU16Sub(num, putNum);	//ʣ��δ������
	BYTE nFreeSpace = SGSU8Sub(packSize, si);	//����ʣ���������
	BYTE nStack = 0;

	//��ȡ��Ʒ����
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

	/* zpy �ɾ�ϵͳ���� */
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

		BYTE n = SGSU8Sub(maxStack, packs[i].num);	//�ø��ӻ��ܷŶ���
		if (n > 0)
		{
			UINT16 notPutNum = SGSU16Sub(num, putNum);	//��ʣ����δ��
			BYTE inPackNum = min(notPutNum, n);			//�ôη����������
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

	UINT16 nFreeNum = SGSU16Sub(num, putNum);	//ʣ��δ������
	BYTE nFreeSpace = SGSU8Sub(packSize, si);	//����ʣ���������
	BYTE nStack = 0;

	//��ȡ��Ʒ����
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
		if (packs[i].propId != propId)	//��Ʒid���
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
	if (nFreeSpace <= 0)	//���ڱ����������
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



	//����ʱ���� add by hxw 20151210
	BOOL bGive = FALSE;
	if (!GetConfigFileMan()->IsRelease())
	{
		//��һ����ɫ����
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

			if (propType == ESGS_PROP_FRAGMENT)		//װ����Ƭ
				AddEquipFragment(propId, num);
			else if (propType == ESGS_PROP_CHAIN_SOUL_FRAGMENT)	//������Ƭ
				AddChainSoulFragment(propId, num);
			else if (propType == ESGS_PROP_DRESS)	//ʱװ
				dress.push_back(propId);
			else if (propType == ESGS_PROP_MAGIC)	//ħ����
				magics.push_back(propId);
			else									//������Ʒ
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

	//����װ��
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
	email.pGBSenderNick = "ϵͳ";	//�������ǳ�

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

	//����Ƿ��㹻
	if (BigNumberToU64(gold) < pComposeItem->consumeGold)
		return MLS_GOLD_NOT_ENOUGH;

	//�Ƿ���������
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

	//�����Ƿ���
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

	//��������Ƿ��㹻
	std::vector<SDropPropCfgItem>& basicConsumePropVec = pComposeItem->basicConsumePropVec;
	{
		//�������ϱض�Ϊװ��������ֻ����һ��
		if (basicConsumePropVec.size() != 1 && basicConsumePropVec.front().type != ESGS_PROP_EQUIP)
		{
			return MLS_NOT_ALLOW_COMPOSE;
		}

		//�������Ѵ���װ�����Ƿ��д�װ��
		if (!IsExistInbackpackAdnWearEquipment(basicConsumePropVec.front().propId, basicConsumePropVec.front().num))
		{
			return MLS_MATERIAL_NOT_ENOUGH;
		}

		//������ϱض�Ϊװ����Ƭ
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

	// ����װ��
	if (fromBackpack)
	{
		//�ϳ�װ���Ž��������ض�ֻ��һ��
		BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(compsoePropVec.front().type, compsoePropVec.front().propId);
		PutBackpackProperty(compsoePropVec.front().type, compsoePropVec.front().propId, compsoePropVec.front().num, maxStack,
			pComposeRes->data + packModNum, SGSU8Sub(maxNum, packModNum), retNum);
		packModNum = SGSU8Add(packModNum, retNum);
	}
	else
	{
		//�ϳ�װ���滻ԭ����װ��
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

	//�۳�����
	{
		//�۳��������ϣ��ض���װ�����ض�ֻ��һ��	
		if (fromBackpack)
		{
			//�ӱ����п۳�
			DropBackpackPropEx(basicConsumePropVec.front().propId, basicConsumePropVec.front().num,
				pComposeRes->data + packModNum, SGSU8Sub(maxNum, packModNum), retNum);
			packModNum = SGSU8Add(packModNum, retNum);
		}

		//�۳������������
		std::vector<SDropPropCfgItem>& additionalConsumePropVec = pComposeItem->additionalConsumePropVec;
		n = (int)additionalConsumePropVec.size();
		for (int i = 0; i < n; ++i)
		{
			DropFragment(additionalConsumePropVec[i].propId, additionalConsumePropVec[i].num);
		}

		//�۳����
		ConsumeGold(pComposeItem->consumeGold, "�ϳ�����");
	}

	pComposeRes->gold = gold;
	pComposeRes->num = min(maxNum, packModNum);
	pComposeRes->replace = fromBackpack == false;

	return retCode;

	/* zpy 2015.10.9ע��
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SEquipComposeRes, gold, data)) / sizeof(SPropertyAttr));
	EquipComposeCfg::SEquipComposeItem* pComposeItem = NULL;
	int retCode = GetGameCfgFileMan()->GetEquipComposeItem(composeId, pComposeItem);
	if(retCode != MLS_OK)
	return retCode;

	//����Ƿ��㹻
	if(BigNumberToU64(gold) < pComposeItem->consumeGold)
	return MLS_GOLD_NOT_ENOUGH;

	//�Ƿ���������
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

	//�����Ƿ���
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

	//��������Ƿ��㹻
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

	//�۳�����
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

	//�ϳ�װ���Ž�����
	n = (int)compsoePropVec.size();
	for(int i = 0; i < n; ++i)
	{
	BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(compsoePropVec[i].type, compsoePropVec[i].propId);
	PutBackpackProperty(compsoePropVec[i].type, compsoePropVec[i].propId, compsoePropVec[i].num, maxStack,
	pComposeRes->data + packModNum, SGSU8Sub(maxNum, packModNum), retNum);
	packModNum = SGSU8Add(packModNum, retNum);
	}
	pComposeRes->num = min(maxNum, packModNum);

	//�۳����
	//SBigNumberSub(gold, pComposeItem->consumeGold);
	ConsumeGold(pComposeItem->consumeGold, "�ϳ�����");
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

	//�Ž��Ѵ����豸
	wearEquipment.push_back(*it);

	//�Ƴ�����
	packs.erase(it);

	//���¼���ս��
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

	//���ò�λ�Ѿ���������ǰ��װ�����ǵ�
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
		packs.push_back(*it2);	//����ǰ������װ���Ž�����(װ��������,ֱ�ӷŽ�ȥ)

	//�Ž��Ѵ����豸
	wearEquipment.push_back(*it);

	//�ӱ������Ƴ�����װ��(װ��������,ֱ���Ƴ�)
	packs.erase(it);

	//���¼���ս��
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
			packs.push_back(*it);		//�Ž�����
			wearEquipment.erase(it);	//�Ƴ��Ѵ���װ��
			break;
		}
	}

	//���¼���ս��
	CalcRoleCombatPower();
	return MLS_OK;
}

//zpy �滻װ��
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

	//���¼���ս��
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

	//�Ƿ����ǿ���ȼ�����
	if (strengthenLv < equipInlaidattr.equipLimitlv)
		return MLS_GEM_HOLE_NOT_OPEN;

	//�ÿ��Ƿ�����Ƕ����ʯ

	if (equipPos[idx].gem[holeSeq] > 0)	//�ÿ���Ƕ��
		return MLS_GEM_HOLE_IS_INLAID;

	//�����в��ұ�ʯ
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

	//��ʯƬ������Ƕ
	if (pGemAttr->level == 0)
		return MLS_GEM_SLICE_NO_INLAID;

	//��ʯ���ͺ�װ����Ҫ��Ƕ�����Ͳ�һ��
	if (pGemAttr->grade != equipInlaidattr.gemType)
		return MLS_GEM_AND_HOLE_NOT_MATCH;

	//��ȡ��ʯ��Ƕ���Ľ��(���ݱ�ʯ�ȼ�)
	GemCfg::SGemConsumeCfg spend;
	retCode = GetGameCfgFileMan()->GetGemSpendByLevel(pGemAttr->level, spend);
	if (retCode != MLS_OK)
		return retCode;

	if (BigNumberToU64(gold) < spend.loadGold)
		return MLS_GOLD_NOT_ENOUGH;

	equipPos[idx].gem[holeSeq] = pGemAttr->propId;	//��Ƕ��ʯid

	//�۳����
	if (spend.loadGold > 0)
		ConsumeGold(spend.loadGold, "��ʯ��Ƕ");
	//SBigNumberSub(gold, spend.loadGold);

	//�ӱ������Ƴ���ʯ
	pGemAttr->num = SGSU8Sub(pGemAttr->num, 1);
	if (pGemAttr->num == 0)
		packs.erase(it);

	//���¼���ս��
	CalcRoleCombatPower();

	return MLS_OK;
}

int SRoleInfos::EquipPosRemoveGem(BYTE pos, BYTE holeSeq, SEquipPosRemoveGemRes* pRemoveRes)
{
	if (holeSeq > ESGS_GEM_HOLE_3)
		return MLS_GEM_HOLE_INVALID;

	//�����Ƿ���Ƕ��
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

	//��ȡȡ����ʯ����
	GemCfg::SGemConsumeCfg spend;
	int retCode = GetGameCfgFileMan()->GetGemSpendById(gemPropId, spend);
	if (retCode != MLS_OK)
		return retCode;

	if (BigNumberToU64(gold) < spend.unloadGold)
		return MLS_GOLD_NOT_ENOUGH;

	//��ʯ�ܷ�Ž�����
	BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(ESGS_PROP_GEM, gemPropId);
	if (!CanPutIntoBackpack(gemPropId, 1, maxStack))
		return MLS_BACKPACK_IS_FULL;

	//�����в��Ҹñ�ʯid����
	GemCfg::SGemAttrCfg gemAttr;
	retCode = GetGameCfgFileMan()->GetGemCfgAttr(gemPropId, gemAttr);
	if (retCode != MLS_OK)
		return retCode;

	//�۳����
	if (spend.unloadGold > 0)
		ConsumeGold(spend.unloadGold, "ȡ����ʯ��Ƕ");
	//SBigNumberSub(gold, spend.unloadGold);

	//�����Ƕ��ʯ
	equipPos[idx].gem[holeSeq] = 0;

	//��1�ű�ʯ�Ž�����
	pRemoveRes->gemPropAttr.propId = gemPropId;
	pRemoveRes->gemPropAttr.level = gemAttr.level;
	pRemoveRes->gemPropAttr.grade = gemAttr.type;
	pRemoveRes->gemPropAttr.type = ESGS_PROP_GEM;
	pRemoveRes->gemPropAttr.num = 1;
	PutBackpackSingleNumProperty(pRemoveRes->gemPropAttr, maxStack);

	//���¼���ս��
	CalcRoleCombatPower();

	return MLS_OK;
}

int SRoleInfos::GemCompose(BYTE num, const SConsumeProperty* pMaterial, SGemComposeRes* pComposeRes)
{
	//�����������ı�ʯ�Ƿ��㹻
	int cosumeGemLv = -1;	//���ı�ʯ�ȼ�
	int cosumeGemType = -1;	//���ı�ʯ����
	int retCode = MLS_MATERIAL_NOT_ENOUGH;
	UINT16 consumeGems = 0;
	BYTE si = (BYTE)packs.size();
	for (BYTE i = 0; i < num; ++i)
	{
		BYTE packGemLv = 0;	//���ı�ʯ�ȼ�
		BYTE packGemType = 0;	//���ı�ʯ����
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
				retCode = MLS_GEM_MATERIAL_NOT_MATCH;	//��Ҫ���ı�ʯ�ȼ���һ��
				break;
			}
		}

		if (cosumeGemType < 0)
			cosumeGemType = packGemType;
		else
		{
			if (cosumeGemType != packGemType)
			{
				retCode = MLS_GEM_MATERIAL_NOT_MATCH;	//��Ҫ���ı�ʯ���Ͳ�һ��
				break;
			}
		}
	}

	if (retCode != MLS_OK)
		return retCode;

	//���ı�ʯ����
	if (consumeGems < GetGameCfgFileMan()->GetComposeGemConsumeGems())
		return MLS_MATERIAL_NOT_ENOUGH;

	//��ȡ��ʯ�ϳ�����
	GemCfg::SGemConsumeCfg spend;
	retCode = GetGameCfgFileMan()->GetGemSpendByLevel(cosumeGemLv + 1, spend);
	if (retCode != MLS_OK)
		return retCode;

	//��Ҳ���
	if (BigNumberToU64(gold) < spend.composeGold)
		return MLS_GOLD_NOT_ENOUGH;

	//��ȡ�ϳɺ�ʯ����
	UINT16 newGemId = 0;
	BYTE   maxStack = 0;
	retCode = GetGameCfgFileMan()->GetGemIdMaxStackByTypeLevel(cosumeGemType, cosumeGemLv + 1, newGemId, maxStack);
	if (retCode != MLS_OK)
		return retCode;

	if (!CanPutIntoBackpack(newGemId, 1, maxStack))
		return MLS_BACKPACK_IS_FULL;

	//�ϳ�1���±�ʯ�Żر���
	pComposeRes->composeGemAttr.type = ESGS_PROP_GEM;
	pComposeRes->composeGemAttr.propId = newGemId;
	pComposeRes->composeGemAttr.level = cosumeGemLv + 1;
	pComposeRes->composeGemAttr.grade = cosumeGemType;
	pComposeRes->composeGemAttr.num = 1;
	PutBackpackSingleNumProperty(pComposeRes->composeGemAttr, maxStack);

	//�۳����ı�ʯ
	for (BYTE i = 0; i < num; ++i)
		DropBackpackProp(pMaterial[i].id, pMaterial[i].num);

	//�۳����
	if (spend.composeGold > 0)
		ConsumeGold(spend.composeGold, "��ʯ�ϳ�");
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
		return MLS_LOWER_SKILL_LIMIT_LEVEL;	//���ڽ�ɫ�ȼ�

	SkillCfg::SSkillAttrCfg cost;
	int retCode = GetGameCfgFileMan()->GetUpgradeAnySkillAttr(occuId, skillId, newSkillLv, cost);
	if (retCode != MLS_OK)
		return retCode;

	//cost.GoldCost = cost.GoldCost * (newSkillLv - 1) * newSkillLv * cost.GoldCostCoefficient;
	cost.GoldCost = cost.GoldCost * pow(newSkillLv, cost.GoldCostCoefficient);
	if (BigNumberToU64(gold) < cost.GoldCost)
		return MLS_GOLD_NOT_ENOUGH;	//��Ҳ���

	if (cost.GoldCost > 0)
		ConsumeGold(cost.GoldCost, "��������");
	//SBigNumberSub(gold, cost.GoldCost);	//�۽��

	studySkills[idx].level = newSkillLv;

	pUpgradeRes->gold = gold;
	pUpgradeRes->newSkill.id = skillId;
	pUpgradeRes->newSkill.level = newSkillLv;
	pUpgradeRes->newSkill.key = studySkills[idx].key;

	// ���¼���ս��
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

	//�����������
	GetRoleBasicAttrPoint(attrPoint);

	//����װ��ս��
	GetRoleWearEquipAttrPoint(attrPoint);

	//�ƺ�ս��
	GetRoleWearTitleAttrPoint(attrPoint);

	//����ʱװս��
	GetRoleWearFashionAttrPoint(attrPoint);

	//����ϵͳ
	GetRoleChainSoulAttrPoint(attrPoint);

	//��ʽת��
	cp = ConvertAttrToPower(attrPoint);

	/* zpy �ɾ�ϵͳ���� */
	GetAchievementMan()->OnUpdateCombatPower(roleId, cp);

	return cp;
}

void SRoleInfos::GetRoleBasicAttrPoint(SRoleAttrPoint& attrPoint)
{
	//����hp/mp
	attrPoint.u32HP += hp;
	attrPoint.u32MP += mp;

	//��������+��������
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

// zpy ����
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
	case ESGS_EQUIP_ATTACH_ATTACK:		//����
		equipAttr.attack += val;
		break;
	case ESGS_EQUIP_ATTACH_DEF:			//����
		equipAttr.def += val;
		break;
	case ESGS_EQUIP_ATTACH_CRIT:		//����
		equipAttr.crit += val;
		break;
	case ESGS_EQUIP_ATTACH_TENACITY:	//����
		equipAttr.tenacity += val;
		break;
	default:
		break;
	}

	// װ�������ԣ�����һ����+װ��ǿ���ȼ�*һ��������
	// װ��������+װ��ǿ���ȼ�*����������
	float factor = 1.0f;
	StrengthenAttriCfg::SStrengthenAttr attr;
	factor = master == 1 ? attrInc.masterInc / 100.0f : attrInc.depuInc / 100.0f;

	// zpy 2016.1.9�޸� �߻�֣һ�������
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
		case ESGS_EQUIP_ATTACH_ATTACK:		//����
			equipAttr.attack += factor * attr.attack;
			break;
		case ESGS_EQUIP_ATTACH_DEF:			//����
			equipAttr.def += factor * attr.def;
			break;
		case ESGS_EQUIP_ATTACH_CRIT:		//����
			equipAttr.crit += factor * attr.crit;
			break;
		case ESGS_EQUIP_ATTACH_TENACITY:	//����
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
	//std::map<UINT16/*��װid*/, BYTE/*����*/> suitMap;

	//����װ��
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

		//װ�������ӵ�
		std::vector<EquipCfg::SEquipAddPoint>& attrVec = *pAttrVec;

		//װ��ǿ���ӵ�
		StrengthenCfg::SEquipAttrInc attrInc;
		retCode = GetGameCfgFileMan()->GetEquipAttrIncreament(wearEquipment[i].pos, wearEquipment[i].grade, attrInc);
		if (retCode != MLS_OK)
		{
			attrInc.depuInc = 0;
			attrInc.masterInc = 0;
			//continue;
		}

		//װ��ǿ������
		SRoleAttrPoint equipAttr;
		int nSize = (int)attrVec.size();
		for (int j = 0; j < nSize; ++j)
		{
			CalcEquipStrengthenAttrPoint(equipPos[posIdx].lv, attrVec[j].type, attrVec[j].val, attrVec[j].master, attrInc, equipAttr);
		}

		//�Ǽ�����
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

		//װ����Ƕ��ʯ����
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

		//��װ����
		//UINT16 suitId = wearEquipment[i].suitId;
		//if(suitId == 0)
		//	continue;

		//std::map<UINT16/*��װid*/, BYTE/*����*/>::iterator it = suitMap.find(suitId);
		//if(it == suitMap.end())
		//	suitMap.insert(std::make_pair(suitId, 1));
		//else
		//	it->second = SGSU8Add(it->second, 1);
	}

	//��װ����
	//for(std::map<UINT16/*��װid*/, BYTE/*����*/>::iterator it = suitMap.begin(); it != suitMap.end(); ++it)
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

/* zpy ��ȡ�������Ե� */
void SRoleInfos::GetRoleChainSoulAttrPoint(SRoleAttrPoint& attrPoint)
{
	ASSERT(chainSoulPos.size() == MAX_CHAIN_SOUL_HOLE_NUM);

	// ���Ӹ��������Լӳ�
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

	//��������������Լӳ�
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
	//��������ս��= HP*HPBalance+ATK*ATKBalance+DEF*DEFBalance+MP*MPBalance
	GameCfgFileMan::SCPBalanceAttr cpBalanceAttr;
	GetGameCfgFileMan()->GetCPBalanceAttr(cpBalanceAttr);
	UINT32 attr_cp = attrPoint.u32HP * cpBalanceAttr.HPBalance;
	attr_cp += attrPoint.attack * cpBalanceAttr.ATKBalance;
	attr_cp += attrPoint.def * cpBalanceAttr.DEFBalance;
	attr_cp += attrPoint.u32MP * cpBalanceAttr.MPBalance;

	//����ս��=ATK*ATKBalance/100*CritHurt/100* CriticalRating/10000*CRIT/(CRIT+SGBalance/100)��
	UINT32 crit_cp = attrPoint.attack * cpBalanceAttr.ATKBalance * cpBalanceAttr.CritHurt * cpBalanceAttr.CriticalRating * attrPoint.crit / (attrPoint.crit + cpBalanceAttr.SGBalance);

	//����ս��=ATK*ATKBalance/100*CritHurt/100* CriticalRating/10000*(1-CTBalance/100/(CTBalance/100+STRONG)) 
	UINT32 tenacity_cp = attrPoint.attack * cpBalanceAttr.ATKBalance * cpBalanceAttr.CritHurt * cpBalanceAttr.CriticalRating *
		(1 - cpBalanceAttr.CTBalance / (cpBalanceAttr.CTBalance + attrPoint.tenacity));

	/**
	* ����ս��
	* 2015.12.18 �߻�֣һ����
	* ����ս��=((ATK*ATKBalance/100*(Damage/100+DamageUp/100*(Lv-1))+(DamageCount+DamageCountUp*(Lv-1))*ATKBalance/100)/CD/SkillTime)*SKBalance/100
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
	//add by hxw 20160108 ���������ӱ�֤�����ڶ��û����ɢװ��Ʒ
	int firstPos = -1;
	//end

	/* zpy �ɾ�ϵͳ���� */
	GemCfg::SGemAttrCfg gem_attr;
	if (GetGameCfgFileMan()->GetGemCfgAttr(prop.propId, gem_attr) != MLS_PROPERTY_NOT_EXIST)
	{
		GetAchievementMan()->OnGotGem(roleId, gem_attr.level);
	}

	BYTE si = (BYTE)packs.size();
	if (prop.type != ESGS_PROP_EQUIP)	//װ�������ڵ��Ӵ��
	{
		for (BYTE i = 0; i < si; ++i)
		{
			if (packs[i].propId != prop.propId)	//��Ʒid���
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

	if (si >= packSize)	//���ڱ����������
		return retCode;
	prop.id = uid++;
	packs.push_back(prop);	//�����µĸ���
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
	case ESGS_TASK_TARGET_PASS_STAGE:			//ͨ��
	case ESGS_TASK_TARGET_FIND_NPC:				//�ҵ�NPC
	case ESGS_TASK_TARGET_PASS_NO_DIE:			//����ͨ��
	case ESGS_TASK_TARGET_LIMIT_TIME:			//��ʱͨ��
	case ESGS_TASK_TARGET_KEEP_BLOOD:			//Ѫ������
	case ESGS_TASK_TARGET_PASS_NO_HURT:			//����ͨ��
	case ESGS_TASK_TARGET_COMBO_NUM_REACH:		//�ﵽ�㹻���������
	case ESGS_TASK_TARGET_PASS_STAGE_NUM:		//ͨ�����⸱��n��
	case ESGS_TASK_TARGET_PASS_RAND_STAGE_NUM:	//ͨ���������n��
		task.target[0] = target[0];	//����id
		break;

	case ESGS_TASK_TARGET_KILL_MONSTER:			//ɱ��
		task.target[0] = target[0];	//����id
		task.target[1] = target[1]; //��id
		break;

	case ESGS_TASK_TARGET_DESTROY_SCENE:	//�ƻ�������
		task.target[0] = target[0];	//����id
		task.target[1] = target[1];	//����������
		break;

	case ESGS_TASK_TARGET_FETCH_PROP:		//��ȡ����
		task.target[0] = target[0];	//����id
		task.target[1] = target[1];	//��������
		task.target[2] = target[2];	//����id
		break;

	case ESGS_TASK_TARGET_ESCORT:			//����
		task.target[0] = target[0];	//����id
		task.target[1] = target[1]; //NPC id
		break;

	case ESGS_TASK_TARGET_NO_USE_SPECIAL_SKILL:	//��ʹ�����⼼��ͨ��
	case ESGS_TASK_TARGET_PASS_NO_BLOOD:		//��ʹ��Ѫƿͨ��
		task.target[0] = target[0];	//����id
		task.target[1] = target[1];	//��ֹʹ��ID
		break;

	case ESGS_TASK_TARGET_FETCH_PROP_LIMIT_TIME:	//��ʱ��ȡ���ߣ�������
		task.target[0] = target[0];		//����id
		task.target[1] = target[1];		//��������
		task.target[2] = target[2];		//����id
		break;

	case ESGS_TASK_TARGET_DESTROY_SCENE_LIMIT_TIME:	//��ʱ�ƻ������������
		task.target[0] = target[0];		//����id
		task.target[1] = target[1];		//����������
		break;

	case ESGS_TASK_TARGET_KILL_MONSTER_LIMIT_TIME:	//��ʱɱ��
		task.target[0] = target[0];		//����id
		task.target[1] = target[1];		//��id
		break;

	case ESGS_TASK_TARGET_ROLE_LV_REACH:	//��ɫ�ȼ��ﵽ
		task.target[0] = level;
		task.isFinished = level >= target[0] ? 1 : 0;
		break;

	case ESGS_TASK_TARGET_LOGIN_RECEIVE_VIT:	//��½��ȡ����
		task.target[0] = target[0];
		break;

	case ESGS_TASK_TARGET_OPEN_CHEST_BOX_NUM:	//�ֶ���������
		task.target[0] = target[0];				//��������
		task.target[1] = target[1];				//����id
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

	//�������Ŀ��
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
	case ESGS_TASK_TARGET_PASS_STAGE:			//ͨ��
		retCode = MLS_OK;
		break;

	case ESGS_TASK_TARGET_KILL_MONSTER:			//ɱ��
		ASSERT(taskTargetParam >= 3);
		retCode = (finTask.target[1] == taskTargetVec[1] && finTask.target[2] >= taskTargetVec[2]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		break;

	case ESGS_TASK_TARGET_FETCH_PROP:			//��ȡ����
		ASSERT(taskTargetParam >= 4);
		retCode = (finTask.target[1] == taskTargetVec[1] && finTask.target[2] == taskTargetVec[2] && finTask.target[3] >= taskTargetVec[3]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		break;

	case ESGS_TASK_TARGET_DESTROY_SCENE:		//�ƻ�������
		ASSERT(taskTargetParam >= 3);
		retCode = (finTask.target[1] == taskTargetVec[1] && finTask.target[2] >= taskTargetVec[2]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		break;

	case ESGS_TASK_TARGET_LIMIT_TIME:			//��ʱͨ��
		ASSERT(taskTargetParam >= 2);
		retCode = (finTask.target[1] != 0 && finTask.target[1] <= taskTargetVec[1]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		if (retCode != MLS_OK)
			finTask.target[1] = 0;
		break;

	case ESGS_TASK_TARGET_ESCORT:				//����
		ASSERT(taskTargetParam >= 3);
		retCode = (finTask.target[2] >= taskTargetVec[2]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		if (retCode != MLS_OK)
			finTask.target[2] = 0;
		break;

	case ESGS_TASK_TARGET_FIND_NPC:				//�ҵ�NPC id
		ASSERT(taskTargetParam >= 2);
		retCode = (finTask.target[1] == taskTargetVec[1]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		if (retCode != MLS_OK)
			finTask.target[1] = 0;
		break;

	case ESGS_TASK_TARGET_PASS_NO_HURT:			//����ͨ��(HP)
	case ESGS_TASK_TARGET_KEEP_BLOOD:			//Ѫ������(HP)
		ASSERT(taskTargetParam >= 2);
		retCode = (finTask.target[1] >= taskTargetVec[1]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		if (retCode != MLS_OK)
			finTask.target[1] = 0;
		break;

	case ESGS_TASK_TARGET_BLOCK_TIMES:			//�񵲶��ٴ�
	case ESGS_TASK_TARGET_COMBO_NUM_REACH:		//�ﵽ�㹻���������
		ASSERT(taskTargetParam >= 2);
		retCode = (finTask.target[1] >= taskTargetVec[1]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		if (retCode != MLS_OK)
			finTask.target[1] = 0;
		break;

	case ESGS_TASK_TARGET_ROLE_LV_REACH:		//��ɫ�ȼ��ﵽ
		retCode = (level >= taskTargetVec[0]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		break;

	case ESGS_TASK_TARGET_FETCH_PROP_LIMIT_TIME:	//��ʱ��ȡ���ߣ������� 
		ASSERT(taskTargetParam >= 5);
		retCode = (finTask.target[3] <= taskTargetVec[3] && finTask.target[4] >= taskTargetVec[4]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		if (retCode != MLS_OK)
		{
			finTask.target[3] = 0;
			finTask.target[4] = 0;
		}
		break;

	case ESGS_TASK_TARGET_KILL_MONSTER_LIMIT_TIME:	//��ʱɱ��
	case ESGS_TASK_TARGET_DESTROY_SCENE_LIMIT_TIME:	//��ʱ�ƻ������������
		ASSERT(taskTargetParam >= 4);
		retCode = (finTask.target[2] <= taskTargetVec[2] && finTask.target[3] >= taskTargetVec[3]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		if (retCode != MLS_OK)
		{
			finTask.target[2] = 0;
			finTask.target[3] = 0;
		}
		break;

	case ESGS_TASK_TARGET_COMPOSE:				//�ϳ�
	case ESGS_TASK_TARGET_CASH:					//�һ�
	case ESGS_TASK_TARGET_STRENGTHEN:			//ǿ��
	case ESGS_TASK_TARGET_RISE_STAR:			//����
		ASSERT(taskTargetParam >= 3);
		retCode = (finTask.target[2] >= taskTargetVec[2]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		break;
	case ESGS_TASK_TARGET_PASS_STAGE_NUM:			//ͨ��������ͨ��������
	case ESGS_TASK_TARGET_PASS_RAND_STAGE_NUM:		//ͨ�������ͨ��������
		retCode = (finTask.target[1] >= taskTargetVec[1]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		break;
	case ESGS_TASK_TARGET_OPEN_CHEST_BOX_NUM:		//�ֶ���������
		retCode = (finTask.target[2] >= taskTargetVec[2]) ? MLS_OK : MLS_FINISH_TASK_FAILED;
		break;
	case ESGS_TASK_TARGET_PASS_NO_BLOOD:		//��ʹ��Ѫƿͨ��
	case ESGS_TASK_TARGET_PASS_NO_DIE:			//����ͨ��
	case ESGS_TASK_TARGET_NO_USE_SPECIAL_SKILL:	//��ʹ�����⼼��ͨ��
	case ESGS_TASK_TARGET_KILL_ALL_MONSTER:		//ɱ�����й�
	case ESGS_TASK_TARGET_DESTROY_ALL_SCENE:	//�ƻ����г�����
	case ESGS_TASK_TARGET_LOGIN_RECEIVE_VIT:	//��½��ȡ����
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

	//�Ƴ�����
	acceptTasks.erase(it);

	std::vector<SPropertyAttr> emailAttachmentsVec;
	SPropertyAttr* pModifyProps = pTaskRewardRes->modifyProps;
	BYTE maxProps = (BYTE)((MAX_RES_USER_BYTES - member_offset(SReceiveTaskRewardRes, taskId, modifyProps)) / sizeof(SPropertyAttr));

	//��ȡ������ɽ���
	TaskCfg::STaskReward taskReward;
	int retCode = GetGameCfgFileMan()->GetTaskReard(taskId, level, taskReward);
	if (MLS_OK != retCode)
		return retCode;
	std::vector<SDropPropCfgItem>& rewardVec = taskReward.props;

	UINT32 token = 0;
	UINT64 u64Gold = taskReward.gold;
	UINT64 u64Exp = taskReward.exp;

	//������Ʒ
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
									   //�����Ų���, ���ʼ�����
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

	/* zpy 2015.12.1�������͑���Ҫ����Ƭ��Ϣ���뱳�� */
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
		ProduceToken(token, "������");

	if (u64Gold > 0)
		ProduceGold(u64Gold, "������");

	if (u64Exp > 0)
	{
		//���Ӿ���
		AddRoleExp(u64Exp);

		//�ж��Ƿ�����(�������ӲŻ�����)
		BYTE newLv = 0;
		GetGameCfgFileMan()->JudgeRoleUpgrade(occuId, level, BigNumberToU64(exp), newLv);
		if (newLv > level)	//��Ҫ����
		{
			//�����¼���
			GetGameCfgFileMan()->GetRoleAllInitSkills(occuId, level, newLv, studySkills);
			//�µȼ�
			level = newLv;
			//�µȼ���ɫhp/mp
			SRoleAttrPoint attr;
			if (MLS_OK == GetGameCfgFileMan()->GetRoleCfgBasicAttr(occuId, level, attr))
			{
				hp = attr.u32HP;
				mp = attr.u32MP;
				attack = attr.attack;
				def = attr.def;
			}
			//������
			pTaskRewardRes->isUpgrade = 1;

			//���¼���ս��
			CalcRoleCombatPower();
		}
	}

	//�Ƿ����ʼ�
	BYTE attachmentsNum = (BYTE)emailAttachmentsVec.size();
	if (attachmentsNum > 0)
	{
		EmailCleanMan::SSyncEmailSendContent email;
		email.type = ESGS_EMAIL_TASK_REWARD;
		email.receiverRoleId = roleId;
		email.receiverUserId = userId;
		email.attachmentsNum = attachmentsNum;
		email.pAttachments = emailAttachmentsVec.data();
		email.pGBSenderNick = "ϵͳ";		//�������ǳ�
		email.pGBCaption = "������, ������";	//����
		email.pGBBody = "��������Ʒ";		//����
		GetEmailCleanMan()->AsyncSendEmail(email);
	}

	//��Ӧ����
	pTaskRewardRes->taskId = taskId;
	pTaskRewardRes->exp = exp;		//����(��ǰӵ��+��������)
	pTaskRewardRes->gold = gold; 	//��ӵ���ܵ���Ϸ��(��ǰӵ��+��������)
	pTaskRewardRes->rpcoin = cash;  //��ӵ���ܵĴ���(��ǰӵ��+��������)
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

		//�Ƿ����ѽ������б���
		if (IsExistAcceptedTasks(dailyTaskId))
			continue;

		//�Ƿ������
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
	case ESGS_TASK_TARGET_KILL_MONSTER:			//ɱ��
		if ((pAcceptTask->target[0] == pTarget[0]) && (pAcceptTask->target[1] == pTarget[1] || pAcceptTask->target[1] == 0))	//����id && ��id
			pAcceptTask->target[2] = SGSU32Add(pAcceptTask->target[2], pTarget[2]);
		break;

	case ESGS_TASK_TARGET_DESTROY_SCENE:		//�ƻ�������
		if ((pAcceptTask->target[0] == pTarget[0]) && (pAcceptTask->target[1] == pTarget[1] || pAcceptTask->target[1] == 0))	//����id && ����������
			pAcceptTask->target[2] = SGSU32Add(pAcceptTask->target[2], pTarget[2]);	//����
		break;

	case ESGS_TASK_TARGET_FETCH_PROP:			//��ȡ����
		if ((pAcceptTask->target[0] == pTarget[0]) &&
			(pAcceptTask->target[1] == 0 ||
			(pAcceptTask->target[1] == pTarget[1] && pAcceptTask->target[2] == 0) ||
			(pAcceptTask->target[1] == pTarget[1] && pAcceptTask->target[2] == pTarget[2])))	//����id && �������� && ����id
			pAcceptTask->target[3] = SGSU32Add(pAcceptTask->target[3], pTarget[3]);	//����
		break;
	case ESGS_TASK_TARGET_LIMIT_TIME:			//��ʱͨ��
		if (pAcceptTask->target[0] == pTarget[0])	//����id
			pAcceptTask->target[1] = pTarget[1];	//ʱ��
		break;

	case ESGS_TASK_TARGET_ESCORT:				//����
		if (pAcceptTask->target[1] == pTarget[1])	//NPC
			pAcceptTask->target[2] = pTarget[2];	//����(HP)
		break;

	case ESGS_TASK_TARGET_PASS_NO_HURT:			//����ͨ��(HP)
	case ESGS_TASK_TARGET_BLOCK_TIMES:			//�񵲶��ٴ�
	case ESGS_TASK_TARGET_COMBO_NUM_REACH:		//�ﵽ�㹻���������
	case ESGS_TASK_TARGET_KEEP_BLOOD:			//Ѫ������(HP)
	case ESGS_TASK_TARGET_FIND_NPC:				//�ҵ�NPC
		if (pAcceptTask->target[0] == pTarget[0])	//����id
			pAcceptTask->target[1] = pTarget[1];
		break;

	case ESGS_TASK_TARGET_FETCH_PROP_LIMIT_TIME:	//��ʱ��ȡ���ߣ�������
		if ((pAcceptTask->target[0] == pTarget[0]) &&
			(pAcceptTask->target[1] == 0 ||
			(pAcceptTask->target[1] == pTarget[1] && pAcceptTask->target[2] == 0) ||
			(pAcceptTask->target[1] == pTarget[1] && pAcceptTask->target[2] == pTarget[2])))	//����id && �������� && ����id
		{
			pAcceptTask->target[3] = pTarget[3];	//ʱ��
			pAcceptTask->target[4] = SGSU32Add(pAcceptTask->target[4], pTarget[4]);	//����
		}
		break;

	case ESGS_TASK_TARGET_KILL_MONSTER_LIMIT_TIME:	//��ʱɱ��
	case ESGS_TASK_TARGET_DESTROY_SCENE_LIMIT_TIME:	//��ʱ�ƻ������������
		if ((pAcceptTask->target[0] == pTarget[0]) && (pAcceptTask->target[1] == pTarget[1] || pAcceptTask->target[1] == 0))	//����id && ��id/����������
		{
			pAcceptTask->target[2] = pTarget[2];	//ʱ��
			pAcceptTask->target[3] = SGSU32Add(pAcceptTask->target[3], pTarget[3]);	//����
		}
		break;

	case ESGS_TASK_TARGET_CASH:					//�һ�
	case ESGS_TASK_TARGET_STRENGTHEN:			//ǿ��
	case ESGS_TASK_TARGET_RISE_STAR:			//����
	case ESGS_TASK_TARGET_COMPOSE:				//�ϳ�
		if (pAcceptTask->target[0] == pTarget[0] && pAcceptTask->target[1] == pTarget[1])	//�������� && ����id
			pAcceptTask->target[2] = SGSU32Add(pAcceptTask->target[2], pTarget[2]);	//����
		break;
	case ESGS_TASK_TARGET_ROLE_LV_REACH:		//��ɫ�ȼ��ﵽ
		pAcceptTask->target[0] = min(level, pTarget[0]);
		break;

	case ESGS_TASK_TARGET_PASS_STAGE_NUM:		//ͨ��������ͨ��������
	case ESGS_TASK_TARGET_PASS_RAND_STAGE_NUM:	//ͨ�������ͨ��������
		pAcceptTask->target[1] = SGSU32Add(pAcceptTask->target[1], 1);
		break;

	case ESGS_TASK_TARGET_OPEN_CHEST_BOX_NUM:		//�ֶ���������
		pAcceptTask->target[2] = SGSU32Add(pAcceptTask->target[2], 1);
		break;

	case ESGS_TASK_TARGET_PASS_STAGE:			//ͨ��
	case ESGS_TASK_TARGET_PASS_NO_BLOOD:		//��ʹ��Ѫƿͨ��
	case ESGS_TASK_TARGET_NO_USE_SPECIAL_SKILL:	//��ʹ�����⼼��ͨ��
	case ESGS_TASK_TARGET_PASS_NO_DIE:			//����ͨ��
	case ESGS_TASK_TARGET_KILL_ALL_MONSTER:		//ɱ�����й�
	case ESGS_TASK_TARGET_DESTROY_ALL_SCENE:	//�ƻ����г�����
	case ESGS_TASK_TARGET_LOGIN_RECEIVE_VIT:	//��½��ȡ����
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

	//�ӽ��
	UINT64 goldNum = BigNumberToU64(pEmailAttachmentsRes->gold);
	if (goldNum > 0)
		ProduceGold(goldNum, GetGameCfgFileMan()->GetEmailTypeDesc(emailType));
	//SBigNumberAdd(gold, pEmailAttachmentsRes->gold);
	pEmailAttachmentsRes->gold = gold;

	//�Ӵ���
	if (pEmailAttachmentsRes->rpcoin > 0)
		ProduceToken(pEmailAttachmentsRes->rpcoin, GetGameCfgFileMan()->GetEmailTypeDesc(emailType));
	//cash = SGSU32Add(cash, pEmailAttachmentsRes->rpcoin);
	pEmailAttachmentsRes->rpcoin = cash;

	//�Ӿ���
	isUpgrade = false;
	if (BigNumberToU64(pEmailAttachmentsRes->exp) > 0)
	{
		BYTE newLevel = 0;
		AddRoleExp(pEmailAttachmentsRes->exp);

		//�ж��Ƿ�����
		GetGameCfgFileMan()->JudgeRoleUpgrade(occuId, level, BigNumberToU64(exp), newLevel);
		if (newLevel > level)
		{
			//�����¼���
			GetGameCfgFileMan()->GetRoleAllInitSkills(occuId, level, newLevel, studySkills);

			//�µȼ���ɫhp/mp
			SRoleAttrPoint attr;
			if (MLS_OK == GetGameCfgFileMan()->GetRoleCfgBasicAttr(occuId, newLevel, attr))
			{
				hp = attr.u32HP;
				mp = attr.u32MP;
				attack = attr.attack;
				def = attr.def;
			}

			//�µȼ�
			level = newLevel;
			isUpgrade = true;
		}
	}
	pEmailAttachmentsRes->exp = exp;

	//������
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

	//�Ƿ����
	if (goodsAttr.sellDateLimit > 0 && GetMsel() > goodsAttr.sellDateLimit)
		return MLS_DRESS_OUT_DATE;

	//�Ƿ�ﵽVIP�ȼ�����
	BYTE vipLv = GetGameCfgFileMan()->CalcVIPLevel(cashcount);
	if (vipLv < goodsAttr.vipLvLimit)
		return MLS_VIP_LV_NOT_ENOUGH;

	//��ͨ�ۿ�
	if (IS_DISCOUNT_EQUAL_ZERO(goodsAttr.discount))
		spendToken = goodsAttr.priceToken * num;
	else
		spendToken = goodsAttr.priceToken * goodsAttr.discount * num / 10;

	//VIP�ۿ�
	BYTE vipDiscount = 100;
	if (vipLv > 0)
	{
		GetGameCfgFileMan()->GetVIPMallBuyDiscount(vipLv, vipDiscount);
		spendToken = spendToken * vipDiscount / 100;
	}

	//�����Ƿ��㹻
	if (cash < spendToken)
		return MLS_CASH_NOT_ENOUGH;

	switch (goodsAttr.type)
	{
	case ESGS_PROP_DRESS:				//ʱװ
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
	case ESGS_PROP_FRAGMENT:	// װ����Ƭ
	{
									UINT16 sum = num * goodsAttr.stackNum;
									AddEquipFragment(goodsAttr.propId, sum);
									break;
	}
	case ESGS_PROP_CHAIN_SOUL_FRAGMENT:	//������Ƭ
	{
											UINT16 sum = num * goodsAttr.stackNum;
											AddChainSoulFragment(goodsAttr.propId, sum);
											break;
	}
	case ESGS_PROP_EQUIP:				//װ��
	case ESGS_PROP_GEM:					//��ʯ
	case ESGS_PROP_MATERIAL:			//����
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

	//�۳�����
	//cash = SGSU32Sub(cash, spendToken);
	ConsumeToken(spendToken, "�̳ǹ�����Ʒ");
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
	//�ۼӸ������
	BYTE n = SGSU8Add(reviveTimes, 1);

	//����VIP���Ӹ������
	BYTE vipAddTimes = 0;
	BYTE vipLv = GetGameCfgFileMan()->CalcVIPLevel(cashcount);
	if (vipLv > 0)
		GetGameCfgFileMan()->GetVIPDeadReviveTimes(vipLv, vipAddTimes);

	//��ȡ���踴���
	UINT32 reviveCoin = 0;
	int retCode = GetGameCfgFileMan()->GetReviveConsume(reviveId, n, vipAddTimes, reviveCoin);
	if (retCode != MLS_OK)
		return retCode;

	//�����ĸ����
	if (reviveCoin == 0)
	{
		pReviveRes->token = cash;
		pReviveRes->num = 0;
		reviveTimes = n;
		return MLS_OK;
	}

	//��Ҫ���ĸ����
	UINT32 spendToken = 0;
	UINT32 reviveCoinExchage = 0;
	GetGameCfgFileMan()->GetMaterialFuncValue(ESGS_MATERIAL_REVIVE_COIN, reviveCoinExchage);
	UINT16 allReviveCoinNum = NumberOfProperty(ESGS_MATERIAL_REVIVE_COIN, reviveCoin);
	if (allReviveCoinNum == 0)	//һ������Ҷ�û��
	{
		spendToken = reviveCoinExchage * reviveCoin;
		if (cash < spendToken)
			return MLS_CASH_NOT_ENOUGH;
	}
	else
	{
		if (allReviveCoinNum < reviveCoin)	//����Ҳ�������������Ƿ��㹻
		{
			spendToken = reviveCoinExchage * (reviveCoin - allReviveCoinNum);
			if (cash < spendToken)
				return MLS_CASH_NOT_ENOUGH;
		}

		//�۳������
		BYTE maxItemNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SReviveRoleRes, token, data)) / sizeof(SConsumeProperty));
		DropBackpackPropEx(ESGS_MATERIAL_REVIVE_COIN, allReviveCoinNum < reviveCoin ? allReviveCoinNum : reviveCoin, pReviveRes->data, maxItemNum, pReviveRes->num);
	}

	//�۳�����
	if (spendToken > 0)
		ConsumeToken(spendToken, "��������");
	//cash = SGSU32Sub(cash, spendToken);

	reviveTimes = n;
	pReviveRes->token = cash;
	return retCode;
}

int SRoleInfos::GoldExchange(SGoldExchangeRes* pExchangeRes)
{
	//��ȡvip���Ӵ���
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

	//�۳�����
	//cash = SGSU32Sub(cash, spendToken);
	ConsumeToken(spendToken, "��Ҷһ�");
	//���ӽ��
	//SBigNumberAdd(gold, getGold);
	ProduceGold(getGold, "��Ҷһ�");
	//���Ӷһ�����
	exchangedTimes = SGSU16Add(exchangedTimes, 1);

	//��������
	pExchangeRes->exchangedTimes = exchangedTimes;
	pExchangeRes->gold = gold;
	pExchangeRes->rpcoin = cash;

	return retCode;
}

/* zpy �����һ� */
int SRoleInfos::VitExchange(SVitExchangeRes* pExchangeRes)
{
	//��ȡvip���Ӵ���
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

	//�۳�����
	ConsumeToken(spendToken, "�����һ�");

	//��������
	vit = SGSU32Add(vit, getVit);

	//���Ӷһ�����
	vitExchangedTimes = SGSU16Add(vitExchangedTimes, 1);

	//��������
	pExchangeRes->exchangedTimes = vitExchangedTimes;
	pExchangeRes->vit = vit;
	pExchangeRes->rpcoin = cash;

	return retCode;
}

/* zpy ��ȡÿ��ǩ������ */
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

				// ˫������
				if (pDailySignItem->double_vip_level != 0 && VIP >= pDailySignItem->double_vip_level)
				{
					num = num * 2;
				}

				switch (propType)
				{
				case ESGS_PROP_GOLD:					// ���
					ProduceGold(num, "ÿ��ǩ������");
					pRefresh->gold = true;
					break;
				case ESGS_PROP_TOKEN:					// ����
					ProduceToken(num, "ÿ��ǩ������");
					pRefresh->rpcoin = true;
					break;
				case ESGS_PROP_VIT:						// ����
					vit = SGSU32Add(vit, num);
					pRefresh->vit = true;
					break;
				case ESGS_PROP_EXP:						// ����
					AddRoleExp(num);
					pRefresh->exp = true;
					break;
				case ESGS_PROP_FRAGMENT:				// װ����Ƭ
					AddEquipFragment(propId, num);
					pRefresh->fragment = true;
					break;
				case ESGS_PROP_CHAIN_SOUL_FRAGMENT:		// ������Ƭ
					AddChainSoulFragment(propId, num);
					pRefresh->chainSoulFragment = true;
					break;
				case ESGS_PROP_EQUIP:					// װ��
				case ESGS_PROP_GEM:						// ��ʯ
				case ESGS_PROP_MATERIAL:				// ����
				{
															BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(propType, propId);
															if (!CanPutIntoBackpack(propId, num, maxStack))
															{
																//�����Ų���, ���ʼ�����
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
				case ESGS_PROP_MAGIC:					//ħ����
					magics.push_back(propId);
					pRefresh->magics = true;
					break;
				case ESGS_PROP_DRESS:					//ʱװ
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

	// ��ȡ������Ƿ�����
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


//add by hxw 20151015 Rank����ͨ����ȡ
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
			ProduceGold(it->num, "����");
			pRefresh->gold = true;
			break;
		case ESGS_PROP_TOKEN:
			ProduceToken(it->num, "����");
			pRefresh->rpcoin = true;
			break;
		case ESGS_PROP_EXP:						// ����
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
									   //�����Ų���, ���ʼ�����
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

	// ��ȡ������Ƿ�����
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
	//����VIP�ȼ�
	BYTE VIP = GetGameCfgFileMan()->CalcVIPLevel(cashcount);
	if (vipLv > VIP)
		return MLS_VIP_LV_NOT_ENOUGH;

	//��ȡVIP����
	std::vector<VIPCfg::SVIPGiveProp>* pVIPRewardVec = NULL;
	GetGameCfgFileMan()->GetVIPGiveProps(vipLv, pVIPRewardVec);
	if (pVIPRewardVec == NULL)
		return MLS_VIP_LV_NOT_ENOUGH;

	//����VIP����
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
			ProduceGold(num, "VIP����");
			pRefresh->gold = true;
			break;
		case ESGS_PROP_TOKEN:
			//cash = SGSU32Add(cash, num);
			ProduceToken(num, "VIP����");
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
									   //�����Ų���, ���ʼ�����
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

/* zpy �ɾ�ϵͳ���� */
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
			case ESGS_PROP_GOLD:					// ���
				ProduceGold(num, "�ɾ�ϵͳ����");
				pRefresh->gold = true;
				break;
			case ESGS_PROP_TOKEN:					// ����
				ProduceToken(num, "�ɾ�ϵͳ����");
				pRefresh->rpcoin = true;
				break;
			case ESGS_PROP_VIT:						// ����
				vit = SGSU32Add(vit, num);
				pRefresh->vit = true;
				break;
			case ESGS_PROP_EXP:						// ����
				AddRoleExp(num);
				pRefresh->exp = true;
				break;
			case ESGS_PROP_FRAGMENT:				// װ����Ƭ
				AddEquipFragment(propId, num);
				pRefresh->fragment = true;
				break;
			case ESGS_PROP_CHAIN_SOUL_FRAGMENT:		// ������Ƭ
				AddChainSoulFragment(propId, num);
				pRefresh->chainSoulFragment = true;
				break;
			case ESGS_PROP_EQUIP:					// װ��
			case ESGS_PROP_GEM:						// ��ʯ
			case ESGS_PROP_MATERIAL:				// ����
			{
														BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(propType, propId);
														if (!CanPutIntoBackpack(propId, num, maxStack))
														{
															//�����Ų���, ���ʼ�����
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
			case ESGS_PROP_MAGIC:					//ħ����
				magics.push_back(propId);
				pRefresh->magics = true;
				break;
			case ESGS_PROP_DRESS:					//ʱװ
				dress.push_back(propId);
				pRefresh->dress = true;
				break;
			default:
				break;
			}
		}

		// ��ȡ������Ƿ�����
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

/* zpy ��ʱ����� */
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
		case ESGS_PROP_GOLD:					// ���
			ProduceGold(num, "��ʱ�����");
			pRefresh->gold = true;
			break;
		case ESGS_PROP_TOKEN:					// ����
			ProduceToken(num, "��ʱ�����");
			pRefresh->rpcoin = true;
			break;
		case ESGS_PROP_VIT:						// ����
			vit = SGSU32Add(vit, num);
			pRefresh->vit = true;
			break;
		case ESGS_PROP_EXP:						// ����
			AddRoleExp(num);
			pRefresh->exp = true;
			break;
		case ESGS_PROP_FRAGMENT:				// װ����Ƭ
			AddEquipFragment(propId, num);
			pRefresh->fragment = true;
			break;
		case ESGS_PROP_CHAIN_SOUL_FRAGMENT:		// ������Ƭ
			AddChainSoulFragment(propId, num);
			pRefresh->chainSoulFragment = true;
			break;
		case ESGS_PROP_EQUIP:					// װ��
		case ESGS_PROP_GEM:						// ��ʯ
		case ESGS_PROP_MATERIAL:				// ����
		{
													BYTE maxStack = GetGameCfgFileMan()->GetPropertyMaxStack(propType, propId);
													if (!CanPutIntoBackpack(propId, num, maxStack))
													{
														//�����Ų���, ���ʼ�����
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
		case ESGS_PROP_MAGIC:					//ħ����
			magics.push_back(propId);
			pRefresh->magics = true;
			break;
		case ESGS_PROP_DRESS:					//ʱװ
			dress.push_back(propId);
			pRefresh->dress = true;
			break;
		default:
			break;
		}
	}

	// ��ȡ������Ƿ�����
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

	if (newStrengthenLv > level)		//������ɫ�ȼ�
		return MLS_MAX_STRENGTHEN_LEVEL;

	GameCfgFileMan::SEquipStrengthenSpend spend;
	int retCode = GetGameCfgFileMan()->GetEquipStrengthenSpend(pos, newStrengthenLv, spend);
	if (retCode != MLS_OK)
		return retCode;

	UINT64 curGold = BigNumberToU64(gold);
	if (curGold < spend.gold)
		return MLS_GOLD_NOT_ENOUGH;		//��Ҳ���

	if (!IsExistProperty(ESGS_MATERIAL_STRENGTHEN_STONE, spend.stones))
		return MLS_MATERIAL_NOT_ENOUGH;	//���Ĳ�����������

	//�۽��
	if (spend.gold > 0)
		ConsumeGold(spend.gold, "װ��λǿ��");
	//SBigNumberSub(gold, spend.gold);

	//�����Ĳ���
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SEquipPosStrengthenRes, newStrengthenLv, consumeStones)) / sizeof(SConsumeProperty));
	DropBackpackPropEx(ESGS_MATERIAL_STRENGTHEN_STONE, spend.stones, pStrengthenRes->consumeStones, maxNum, pStrengthenRes->num);

	equipPos[idx].lv = newStrengthenLv;		//�µ�ǿ���ȼ�
	pStrengthenRes->gold = gold;
	pStrengthenRes->newStrengthenLv = newStrengthenLv;

	//���¼���ս��
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

	//�Ƿ񳬹�����Ǽ�
	if (riseStar > GetGameCfgFileMan()->GetEquipMaxStarLevel())
		return MLS_MAX_STAR_LEVEL;

	//��������
	RiseStarCfg::SRiseStarCfg cfg;
	int retCode = GetGameCfgFileMan()->GetRiseStarCfg(riseStar, cfg);
	if (retCode != MLS_OK)
		return retCode;
	if (level < cfg.limitLevel)
		return MLS_STAR_LIMIT_LEVEL;

	//��ȡ���ǻ���
	RiseStarCfg::SRiseStarSpend spend;
	retCode = GetGameCfgFileMan()->GetRiseStarSpend(riseStar, spend);
	if (retCode != MLS_OK)
		return retCode;

	UINT64 curGold = BigNumberToU64(gold);
	if (curGold < spend.gold)
		return MLS_GOLD_NOT_ENOUGH;		//��Ҳ���

	if (!IsExistProperty(ESGS_MATERIAL_VEINS_STAR_STONE, spend.stones))
		return MLS_MATERIAL_NOT_ENOUGH;	//���Ĳ���(����ʯ)��������

	//��âʯ(����������)
	BYTE decDamagedChance = 0;
	if (param.decDamagedChance > 0)
	{
		if (!IsExistProperty(ESGS_MATERIAL_SHINE_STONE, param.decDamagedChance))
			return MLS_MATERIAL_NOT_ENOUGH;	//���Ĳ���(��âʯ)��������

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

	//�Ի�ʯ(���ӳɹ���)
	BYTE addSuccedChance = 0;
	if (param.addSuccedChance > 0)
	{
		if (!IsExistProperty(ESGS_MATERIAL_BRILLIANT_STONE, param.addSuccedChance))
			return MLS_MATERIAL_NOT_ENOUGH;	//���Ĳ���(�Ի�ʯ)��������

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

	//����
	BYTE maxSuccessChance = GetGameCfgFileMan()->GetRiseStarMaxSuccessChance();
	BYTE succedChance = min(maxSuccessChance, SGSU8Add(cfg.succedChance, (BYTE)addSuccedChance));
	if (rand() % 100 < succedChance)	//�ɹ�
	{
		equipPos[idx].star = riseStar;
	}
	else
	{
		BYTE minDamagedChance = GetGameCfgFileMan()->GetRiseStarMinDamageChance();
		BYTE damagedChance = max(minDamagedChance, SGSU8Sub(cfg.damagedChance, (BYTE)decDamagedChance));
		if (rand() % 100 < damagedChance)	//���𽵼�
		{
			equipPos[idx].star = equipPos[idx].star > 0 ? SGSU8Sub(equipPos[idx].star, 1) : 0;
		}
	}
	pRiseStarRes->newStarLv = equipPos[idx].star;

	//�۽��
	if (spend.gold > 0)
		ConsumeGold(spend.gold, "װ��λ����");
	//SBigNumberSub(gold, spend.gold);
	pRiseStarRes->gold = gold;

	//������ʯ
	BYTE retNum = 0;
	pRiseStarRes->num = 0;
	BYTE maxNum = (BYTE)((MAX_RES_USER_BYTES - member_offset(SEquipPosRiseStarRes, newStarLv, consumeStones)) / sizeof(SConsumeProperty));
	DropBackpackPropEx(ESGS_MATERIAL_VEINS_STAR_STONE, spend.stones, pRiseStarRes->consumeStones, maxNum, retNum);
	pRiseStarRes->num = retNum;

	//�۹�âʯ(����������)
	if (param.decDamagedChance > 0/* && maxNum > pRiseStarRes->num*/)
	{
		retNum = 0;
		BYTE n = SGSU8Sub(maxNum, pRiseStarRes->num);
		SConsumeProperty* p = pRiseStarRes->consumeStones + pRiseStarRes->num;
		DropBackpackPropEx(ESGS_MATERIAL_SHINE_STONE, param.decDamagedChance, p, n, retNum);
		pRiseStarRes->num += retNum;
	}

	//�ۻԻ�ʯ(���ӳɹ���)
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

// zpy ��ȡ������Ƭ����
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

// zpy �۳�������Ƭ
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

// zpy ����������Ƭ
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

// zpy ��ʼ����ӵ��������Ƭ
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

// zpy ��ʼ�����겿��
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
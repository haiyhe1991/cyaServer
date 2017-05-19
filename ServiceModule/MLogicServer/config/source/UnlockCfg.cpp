#include "UnlockCfg.h"
#include "ServiceErrorCode.h"

UnlockCfg::UnlockCfg()
{

}

UnlockCfg::~UnlockCfg()
{

}

BOOL UnlockCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	int si = rootValue["Lock"].size();
	for (int i = 0; i < si; ++i)
	{
		SUnlockObj obj;
		BYTE type = rootValue["Lock"][i]["Type"].asUInt();
		std::map<BYTE/*解锁类型*/, SUnlockObj>::iterator it = m_unlockCfgMap.find(type);
		if (it == m_unlockCfgMap.end())
			m_unlockCfgMap.insert(std::make_pair(type, obj));

		SUnlockObj& objRef = m_unlockCfgMap[type];
		UINT16 unlockId = rootValue["Lock"][i]["ID"].asUInt();
		std::map<UINT16/*解锁id*/, SUnlockItem>::iterator it2 = objRef.unlockItemMap.find(unlockId);
		if (it2 != objRef.unlockItemMap.end())
		{
			ASSERT(false);
			continue;
		}

		SUnlockItem item;
		objRef.unlockItemMap.insert(std::make_pair(unlockId, item));
		SUnlockItem& itemRef = objRef.unlockItemMap[unlockId];
		itemRef.gold = rootValue["Lock"][i]["Golds"].asUInt();
		itemRef.isLock = rootValue["Lock"][i]["BLock"].asUInt();
		itemRef.lvLimit = rootValue["Lock"][i]["LvLimit"].asUInt();
		itemRef.rpcoin = rootValue["Lock"][i]["Token"].asUInt();
	}

	return true;
}

int UnlockCfg::GetUnlockItem(BYTE type, UINT16 unlockId, SUnlockItem*& pUnlockItem)
{
	std::map<BYTE/*解锁类型*/, SUnlockObj>::iterator it = m_unlockCfgMap.find(type);
	if (it == m_unlockCfgMap.end())
		return MLS_INVALID_UNLOCK_ID;

	std::map<UINT16/*解锁id*/, SUnlockItem>::iterator it2 = it->second.unlockItemMap.find(unlockId);
	if (it2 == it->second.unlockItemMap.end())
		return MLS_INVALID_UNLOCK_ID;

	pUnlockItem = &it2->second;
	return MLS_OK;
}

int UnlockCfg::GetUnlockedItems(BYTE type, std::vector<UINT16>& itemsVec)
{
	std::map<BYTE/*解锁类型*/, SUnlockObj>::iterator it = m_unlockCfgMap.find(type);
	if (it == m_unlockCfgMap.end())
		return MLS_OK;

	std::map<UINT16/*解锁id*/, SUnlockItem>::iterator it2 = it->second.unlockItemMap.begin();
	for (; it2 != it->second.unlockItemMap.end(); ++it2)
	{
		if (it2->second.isLock)
			continue;

		itemsVec.push_back(it2->first);
	}

	return MLS_OK;
}
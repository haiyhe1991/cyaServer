#include "InitRoleGiveCfg.h"

InitRoleGiveCfg::InitRoleGiveCfg()
{

}

InitRoleGiveCfg::~InitRoleGiveCfg()
{

}

BOOL InitRoleGiveCfg::Load(const char* filename)
{
	m_filename = filename;
	return true;
}

BOOL InitRoleGiveCfg::GetRoleInitGiveItem(SInitRoleGiveItem& giveItem)
{
	Json::Value rootValue;
	if (!OpenCfgFile(m_filename.c_str(), rootValue))
		return false;

	giveItem.money.gold = rootValue["RoleInitGive"]["Money"]["Gold"].asUInt();
	giveItem.money.rpcoin = rootValue["RoleInitGive"]["Money"]["Token"].asUInt();

	giveItem.other.exp = rootValue["RoleInitGive"]["Other"]["Exp"].asUInt();
	giveItem.other.vit = rootValue["RoleInitGive"]["Other"]["Vit"].asUInt();

	UINT16 wearEquipId = rootValue["RoleInitGive"]["WearEquip"]["Armor"].asUInt();
	if (wearEquipId > 0)
		giveItem.wearEquipVec.push_back(wearEquipId);

	wearEquipId = rootValue["RoleInitGive"]["WearEquip"]["Glove"].asUInt();
	if (wearEquipId > 0)
		giveItem.wearEquipVec.push_back(wearEquipId);

	wearEquipId = rootValue["RoleInitGive"]["WearEquip"]["Necklace"].asUInt();
	if (wearEquipId > 0)
		giveItem.wearEquipVec.push_back(wearEquipId);

	wearEquipId = rootValue["RoleInitGive"]["WearEquip"]["Ring"].asUInt();
	if (wearEquipId > 0)
		giveItem.wearEquipVec.push_back(wearEquipId);

	wearEquipId = rootValue["RoleInitGive"]["WearEquip"]["Shoes"].asUInt();
	if (wearEquipId > 0)
		giveItem.wearEquipVec.push_back(wearEquipId);

	wearEquipId = rootValue["RoleInitGive"]["WearEquip"]["Weapen"].asUInt();
	if (wearEquipId > 0)
		giveItem.wearEquipVec.push_back(wearEquipId);

	int si = rootValue["RoleInitGive"]["Prop"].size();
	for (int i = 0; i < si; ++i)
	{
		SDropPropCfgItem item;
		item.num = rootValue["RoleInitGive"]["Prop"][i]["Num"].asUInt();
		item.propId = rootValue["RoleInitGive"]["Prop"][i]["ID"].asUInt();
		item.type = rootValue["RoleInitGive"]["Prop"][i]["Type"].asUInt();
		giveItem.propVec.push_back(item);
	}

	return true;
}
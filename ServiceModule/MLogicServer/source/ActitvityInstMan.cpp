#include "ActitvityInstMan.h"

#include <algorithm>
#include "RoleInfos.h"
#include "DBSClient.h"
#include "ServiceProtocol.h"
#include "RolesInfoMan.h"
#include "ConfigFileMan.h"
#include "GameCfgFileMan.h"

static ActitvityInstMan* sg_pActitvityInstMan = NULL;
void InitActitvityInstMan()
{
	ASSERT(sg_pActitvityInstMan == NULL);
	sg_pActitvityInstMan = new ActitvityInstMan();
	ASSERT(sg_pActitvityInstMan != NULL);
}

ActitvityInstMan* GetActitvityInstMan()
{
	return sg_pActitvityInstMan;
}

void DestroyActitvityInstMan()
{
	ActitvityInstMan* pActitvityInstMan = sg_pActitvityInstMan;
	sg_pActitvityInstMan = NULL;
	if (pActitvityInstMan)
		delete pActitvityInstMan;
}

ActitvityInstMan::ActitvityInstMan()
{
}

ActitvityInstMan::~ActitvityInstMan()
{
}

// ��ѯ�����
int ActitvityInstMan::QueryHasActitvityInstType(UINT32 userId, UINT32 roleId, SQueryHasActivityInstTypeRes *pHasActivityInstType)
{
	bool insert = false;
	DBS_RESULT result = NULL;
	DBS_ROWLENGTH rowValLen = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT DATEDIFF(CURRENT_DATE(),last_send_time),activity FROM randactivity WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	int can_receive_day = 0;							//����ȡ������
	std::vector<SActivityInstItem> actitvity_type_list;	//����ɵĻ����
	DBS_ROW row_data = DBS_FetchRow(result, &rowValLen);
	if (row_data != NULL)
	{
		can_receive_day = atoi(row_data[0]);
		if (rowValLen[1] > 0)
		{
			ASSERT(rowValLen[1] % sizeof(SActivityInstItem) == 0);
			UINT32 size = rowValLen[1] / sizeof(SActivityInstItem);
			SActivityInstItem *pActitvityInst = (SActivityInstItem *)row_data[1];
			for (int i = 0; i < size; ++i)
			{
				actitvity_type_list.push_back(pActitvityInst[i]);
			}
		}
	}
	else
	{
		DBS_FreeResult(result);

		sprintf(szSQL, "SELECT createtts FROM actor WHERE id=%u", roleId);
		int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
		if (retCode != DBS_OK)
			return retCode;

		row_data = DBS_FetchRow(result, &rowValLen);
		if (row_data != NULL && rowValLen[0] > 0)
		{
			// ������ת��������
			can_receive_day = (GetMsel() - LocalStrTimeToMSel(row_data[0])) / 1000 / 60 / 60 / 24;
			++can_receive_day;
		}
		DBS_FreeResult(result);
		insert = true;
	}

	// ���Ż����
	if (can_receive_day > 0)
	{
		UINT32 upperLimit = GetGameCfgFileMan()->GetActivityInstUpperLimit();
		if ((can_receive_day + actitvity_type_list.size()) > upperLimit)
		{
			can_receive_day = upperLimit - actitvity_type_list.size();
		}

		// ��ѯ�ȼ��Լ�ͨ�ظ�����Ϣ
		SRoleInfos roleinfo;
		std::set<UINT16> passInsts;
		retCode = GetRolesInfoMan()->GetRoleInfo(userId, roleId, &roleinfo);
		if (retCode != MLS_OK)
			return retCode;

		sprintf(szSQL, "SELECT id FROM instancehistory WHERE actorid=%u", roleId);
		retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
		if (retCode != DBS_OK)
			return retCode;

		while ((row_data = DBS_FetchRow(result, &rowValLen)) != NULL)
		{
			passInsts.insert(atoi(row_data[0]));
		}
		DBS_FreeResult(result);

		/**
		* 2015.12.21 �߻��ĵ�����
		* �߻���
		* ��ۼ����޸���VIP�ȼ����ӣ���ͬ��ؿ�����Ϊ5�����䣬��VIP��ҿ����ۼ���ͬ��ؿ����������
		* Vip.json.ResourceInstChalNum�ֶζ�ȡ��ͬVIP�ȼ�ӵ�ж�����������
		*/
		if (can_receive_day > 0)
		{
			for (int i = 0; i < can_receive_day; ++i)
			{
				SActivityInstItem ActitvityInst;
				if (GetGameCfgFileMan()->GenerateActivityInst(roleinfo.level, passInsts, ActitvityInst))
				{
					actitvity_type_list.push_back(ActitvityInst);
				}
			}
		}
		else
		{
			// �����������Ī�������������
			SActivityInstItem ActitvityInst;
			if (GetGameCfgFileMan()->GenerateActivityInst(roleinfo.level, passInsts, ActitvityInst))
			{
				// �����ܽ������
				int count = 0;
				int index = -1;
				for (size_t i = 0; i < actitvity_type_list.size(); ++i)
				{
					if (actitvity_type_list[i].instId == ActitvityInst.instId)
					{
						index = i;
						count += actitvity_type_list[i].num;
					}
				}

				if (index != -1)
				{
					// �Ƿ�ﵽvip����
					UINT16 chalTimes = 0;
					SRoleInfos role_info;
					GetRolesInfoMan()->GetRoleInfo(userId, roleId, &role_info);
					BYTE vipLevel = GetGameCfgFileMan()->CalcVIPLevel(role_info.cashcount);
					GetGameCfgFileMan()->GetVIPResourceInstChalNum(vipLevel, chalTimes);
					UINT16 enterNum = GetGameCfgFileMan()->GetInstEnterNum(ActitvityInst.instId);
					if (count < (enterNum + chalTimes))
					{
						actitvity_type_list[index].num = (count + ActitvityInst.num) < (enterNum + chalTimes) ? count + ActitvityInst.num : enterNum + chalTimes;
					}
				}
			}
		}
	}

	// ��������
	if (insert)
	{
		char szDate[32] = { 0 };
		MselToLocalStrTime(GetMsel(), szDate);
		sprintf(szSQL, "INSERT INTO randactivity VALUES(%u,'", roleId);
		int lenght = strlen(szSQL);
		DBS_EscapeString(szSQL + lenght, (const char*)actitvity_type_list.data(), actitvity_type_list.size() * sizeof(SActivityInstItem));
		char szTail[64] = { 0 };
		sprintf(szTail, "','%s')", szDate);
		strcat(szSQL, szTail);
		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}
	else if (can_receive_day > 0)
	{
		char szDate[32] = { 0 };
		MselToLocalStrTime(GetMsel(), szDate);
		sprintf(szSQL, "UPDATE randactivity SET last_send_time='%s',activity='", szDate);
		int lenght = strlen(szSQL);
		DBS_EscapeString(szSQL + lenght, (const char*)actitvity_type_list.data(), actitvity_type_list.size() * sizeof(SActivityInstItem));
		char szTail[64] = { 0 };
		sprintf(szTail, "' WHERE actorid=%u", roleId);
		strcat(szSQL, szTail);
		GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
	}

	// ��ֵ����
	pHasActivityInstType->num = actitvity_type_list.size();
	for (int i = 0; i < actitvity_type_list.size(); ++i)
	{
		pHasActivityInstType->data[i] = actitvity_type_list[i];
	}

	return MLS_OK;
}

// ��������
int ActitvityInstMan::EnterActitvityInst(UINT32 userId, UINT32 roleId, UINT32 instId)
{
	DBS_RESULT result = NULL;
	DBS_ROWLENGTH rowValLen = NULL;
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "SELECT activity FROM randactivity WHERE actorid=%u", roleId);
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	// ����ɵĻ����
	std::vector<SActivityInstItem> actitvity_type_list;
	DBS_ROW row_data = DBS_FetchRow(result, &rowValLen);
	if (row_data != NULL)
	{
		if (rowValLen[0] > 0)
		{
			ASSERT(rowValLen[0] % sizeof(SActivityInstItem) == 0);
			UINT32 size = rowValLen[0] / sizeof(SActivityInstItem);
			SActivityInstItem *pActitvityInst = (SActivityInstItem *)row_data[0];
			for (int i = 0; i < size; ++i)
			{
				actitvity_type_list.push_back(pActitvityInst[i]);
			}
		}
	}
	DBS_FreeResult(result);

	// ��������
	int min_index = -1;
	for (size_t i = 0; i < actitvity_type_list.size(); ++i)
	{
		if (actitvity_type_list[i].instId == instId)
		{
			ASSERT(actitvity_type_list[i].num > 0);
			if (min_index == -1 || actitvity_type_list[i].num < actitvity_type_list[min_index].num)
			{
				min_index = i;
			}
		}
	}
	if (min_index == -1)
	{
		return MLS_NOT_HAS_ACTIVITY_INST_TYPE;
	}

	if (actitvity_type_list[min_index].num > 0)
	{
		--actitvity_type_list[min_index].num;
	}


	if (actitvity_type_list[min_index].num <= 0)
	{
		std::swap(actitvity_type_list[min_index], actitvity_type_list.back());
		actitvity_type_list.pop_back();
	}

	// ������
	strcpy(szSQL, "UPDATE randactivity SET activity='");
	DBS_EscapeString(szSQL + strlen(szSQL), (const char*)actitvity_type_list.data(), actitvity_type_list.size() * sizeof(SActivityInstItem));
	char szTail[64] = { 0 };
	sprintf(szTail, "' WHERE actorid=%u", roleId);
	strcat(szSQL, szTail);
	GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);

	return MLS_OK;
}
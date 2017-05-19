#include "moduleinfo.h"
#include "LuaScriptMan.h"
#include "ServiceErrorCode.h"
#include "cyaLog.h"

static LuaScriptMan* sg_luaScriptMan = NULL;
void InitLuaScriptMan()
{
	ASSERT(sg_luaScriptMan == NULL);
	sg_luaScriptMan = new LuaScriptMan();
	ASSERT(sg_luaScriptMan != NULL);
	sg_luaScriptMan->LoadLuaScriptFile();
}

LuaScriptMan* GetLuaScriptMan()
{
	return sg_luaScriptMan;
}

void DestroyLuaScriptMan()
{
	LuaScriptMan* luaMan = sg_luaScriptMan;
	sg_luaScriptMan = NULL;
	if (luaMan != NULL)
	{
		luaMan->UnLoadLuaScriptFile();
		delete luaMan;
	}
}

LuaScriptMan::LuaScriptMan()
{

}

LuaScriptMan::~LuaScriptMan()
{

}

//����lua�ű��ļ�
void LuaScriptMan::LoadLuaScriptFile(const char* filepath/* = NULL*/, const char* luaSuffixName/* = "*.lua"*/)
{
	char szAbsouleFileDir[MAX_PATH] = { 0 };
	MergeFileName(GetExeDir(), filepath, szAbsouleFileDir);

#if defined(WIN32)
	char szFilePath[MAX_PATH] = { 0 };
	strcpy(szFilePath, szAbsouleFileDir);
	strcat(szFilePath, "\\");
	strcat(szFilePath, luaSuffixName);
	WIN32_FIND_DATAA findData;
	HANDLE hd = ::FindFirstFileA(szFilePath, &findData);
	if (hd == INVALID_HANDLE_VALUE)
		return;

	do
	{
		char fullFileName[MAX_PATH] = { 0 };
		strcpy(fullFileName, szAbsouleFileDir);
		strcat(fullFileName, "\\");
		strcat(fullFileName, findData.cFileName);
		LuaGlue* pLuaState = new LuaGlue();
		if (pLuaState->dofile(fullFileName))
		{
			m_luaStateMap.insert(std::make_pair(findData.cFileName, pLuaState));
			LogInfo(("����ű��ļ�[%s]�ɹ�!", findData.cFileName));
		}
		else
		{
			delete pLuaState;
			LogInfo(("����ű��ļ�[%s]ʧ��!", findData.cFileName));
		}

	} while (FindNextFileA(hd, &findData));
	FindClose(hd);

#elif defined(__linux__)
	SCAN_FILE_FILTER scanfilter;
	scanfilter.count = scandir(szAbsouleFileDir, &scanfilter.nameList, SCAN_FILE_FILTER::scandir_filter_, alphasort);
	int count = scanfilter.count;
	struct dirent** nameList = scanfilter.nameList;
	if (count <= 0 || nameList == NULL)
		return;

	const char* pSuffixName = strstr(luaSuffixName, ".");
	if (pSuffixName == NULL)
		pSuffixName = luaSuffixName;
	else
		pSuffixName++;

	for (int i = 0; i < count; ++i)
	{
		char fullFileName[MAX_PATH] = { 0 };
		strcpy(fullFileName, szAbsouleFileDir);
		strcat(fullFileName, "/");
		strcat(fullFileName, nameList[i]->d_name);

		const char* fileSuffixName = GetFileSuffixName(fullFileName);
		if (strcmp(fileSuffixName, pSuffixName/*"lua"*/) != 0)
			continue;

		LuaGlue* pLuaState = new LuaGlue();
		if (pLuaState->dofile(fullFileName))
		{
			m_luaStateMap.insert(std::make_pair(nameList[i]->d_name, pLuaState));
			LogInfo(("����ű��ļ�[%s]�ɹ�!", nameList[i]->d_name));
		}
		else
		{
			delete pLuaState;
			LogInfo(("����ű��ļ�[%s]ʧ��!", nameList[i]->d_name));
		}
	}
#endif
	int r = InitDefaultParams();
	if (r == MLS_OK)
		LogInfo(("��ʼ��luaĬ�ϲ����ɹ�!"));
	else
		LogInfo(("��ʼ��luaĬ�ϲ���ʧ��!"));
}

void LuaScriptMan::UnLoadLuaScriptFile()
{
	std::map<std::string/*lua�ű��ļ���*/, LuaGlue*>::iterator it = m_luaStateMap.begin();
	for (; it != m_luaStateMap.end(); ++it)
		delete it->second;
	m_luaStateMap.clear();
}

int LuaScriptMan::IsValidRoleType(UINT16 roleType)
{
	LuaGlue* pLuaGlue = GetLuaGlue("create_role.lua");
	if (pLuaGlue == NULL)
		return MLS_SCRIPT_EXE_FAILED;

	LuaGlue lua(pLuaGlue->GetLuaState());

	///�������
	Variant inParam;
	inParam.SetInt(roleType);
	VariantVec inParamVec;
	inParamVec.push_back(inParam);

	//���
	VariantVec outParamVec;
	bool ret = lua.ExecFunction("IsValidType", &inParamVec, 1, outParamVec);
	if (!ret || outParamVec.size() < 1)
		return MLS_SCRIPT_EXE_FAILED;

	if (outParamVec[0].GetType() != Variant::VT_NUMBER)
		return MLS_SCRIPT_EXE_FAILED;

	return !!outParamVec[0].GetInt() ? MLS_OK : MLS_ROLE_TYPE_INVALID;
}

int LuaScriptMan::GetCreateRoleDefaultInfo(UINT16 roleType, SRoleInfos* pInfo, UINT16& guildId, BYTE& sex)
{
	LuaGlue* pLuaGlue = GetLuaGlue("create_role.lua");
	if (pLuaGlue == NULL)
		return MLS_SCRIPT_EXE_FAILED;

	guildId = 0;

	//�������
	LuaGlue lua(pLuaGlue->GetLuaState());
	Variant inParam;
	inParam.SetInt(roleType);
	VariantVec inParamVec;
	inParamVec.push_back(inParam);

	//���
	VariantVec outParamVec;

	//��ȡ�Ա�
	bool ret = lua.ExecFunction("GetSex", &inParamVec, 1, outParamVec);
	if (!ret || outParamVec.size() < 1)
		return MLS_SCRIPT_EXE_FAILED;
	sex = (BYTE)outParamVec[0].GetUnsignedInt();

	//��ȡ4�ֻ�������
	outParamVec.clear();
	ret = lua.ExecFunction("GetBasicAttr", &inParamVec, 4, outParamVec);
	if (!ret || outParamVec.size() < 4)
		return MLS_SCRIPT_EXE_FAILED;
	// 	pInfo->str = (UINT16)outParamVec[3].GetUnsignedInt();	//����
	// 	pInfo->dex = (UINT16)outParamVec[2].GetUnsignedInt();	//����
	// 	pInfo->intelligence = (UINT16)outParamVec[1].GetUnsignedInt();	//����
	// 	pInfo->sp = (UINT16)outParamVec[0].GetUnsignedInt();			//����

	//��ȡ����, ħ��, ����
	outParamVec.clear();
	ret = lua.ExecFunction("GetBasicValue", &inParamVec, 3, outParamVec);
	if (!ret || outParamVec.size() < 3)
		return MLS_SCRIPT_EXE_FAILED;
	pInfo->hp = outParamVec[2].GetUnsignedInt();		//����
	pInfo->mp = outParamVec[1].GetUnsignedInt();		//ħ��
	pInfo->vit = outParamVec[0].GetUnsignedInt();		//����
	pInfo->ap = 0;	//ʣ�����Ե�

	//��ȡ����,�ֿ��С
	outParamVec.clear();
	ret = lua.ExecFunction("GetStorageSpace", &inParamVec, 2, outParamVec);
	if (!ret || outParamVec.size() < 2)
		return MLS_SCRIPT_EXE_FAILED;
	pInfo->packSize = (BYTE)outParamVec[1].GetUnsignedInt();		//������С
	pInfo->wareSize = (BYTE)outParamVec[0].GetUnsignedInt();		//�ֿ��С
	return MLS_OK;
}

int LuaScriptMan::GetMaxInlaidStones(UINT16 propId, BYTE propGrade, BYTE& maxStones)
{
	LuaGlue* pLuaGlue = GetLuaGlue("inlaid_stone.lua");
	if (pLuaGlue == NULL)
		return MLS_SCRIPT_EXE_FAILED;

	LuaGlue lua(pLuaGlue->GetLuaState());
	Variant idParam;
	idParam.SetInt(propId);
	Variant gradeParam;
	gradeParam.SetInt(propGrade);

	VariantVec inParamVec;
	inParamVec.push_back(idParam);
	inParamVec.push_back(gradeParam);

	VariantVec outParamVec;
	bool ret = lua.ExecFunction("GetMaxInlaidStones", &inParamVec, 1, outParamVec);
	if (!ret || outParamVec.size() < 1)
		return MLS_SCRIPT_EXE_FAILED;

	if (outParamVec[0].GetType() != Variant::VT_NUMBER)
		return MLS_SCRIPT_RETVAL_INVALID;

	maxStones = (BYTE)outParamVec[0].GetInt();
	return MLS_OK;
}

int LuaScriptMan::CalcUpgradeTotalExp(int lv, UINT64& totalExp, UINT64& taskExp, UINT64& instExp, UINT64& actionExp)
{
	totalExp = 0;
	taskExp = 0;
	instExp = 0;
	actionExp = 0;
	if (lv <= 1)
		return MLS_OK;

	LuaGlue* pLuaGlue = GetLuaGlue("calc_attributes.lua");
	if (pLuaGlue == NULL)
		return MLS_SCRIPT_EXE_FAILED;

	VariantVec inParamVec;
	VariantVec outParamVec;
	Variant lvParam;
	lvParam.SetUnsignedInt(lv);
	inParamVec.push_back(lvParam);

	LuaGlue lua(pLuaGlue->GetLuaState());
	bool ret = lua.ExecFunction("GetUpgradeParams", &inParamVec, 3, outParamVec);
	if (!ret || outParamVec.size() < 3)
		return MLS_SCRIPT_EXE_FAILED;

	UINT32 taskCount = outParamVec[2].GetUnsignedInt();
	UINT32 instCount = outParamVec[1].GetUnsignedInt();
	UINT32 actionCount = outParamVec[0].GetUnsignedInt();

	if (taskCount > 0)
		taskExp = (UINT64)(m_baseTaskExp + (lv - m_baseTaskLv/*2*/) * m_taskExpRose) * taskCount;
	if (instCount > 0)
		instExp = (UINT64)(m_baseInstExp + (lv - m_baseInstLv/*4*/) * m_instExpRose) * instCount;
	if (actionCount > 0)
		actionExp = (UINT64)(m_baseActionExp + (lv - m_baseActionLv/*11*/) * m_actionExpRose) * actionCount;

	totalExp = (UINT64)(taskExp + instExp + actionExp) * m_baseConst;

	return MLS_OK;
}

LuaGlue* LuaScriptMan::GetLuaGlue(const char* luaScripFile)
{
	std::map<std::string/*lua�ű��ļ���*/, LuaGlue*>::iterator it = m_luaStateMap.find(luaScripFile);
	if (it == m_luaStateMap.end())
		return NULL;
	return it->second;
}

int LuaScriptMan::InitDefaultParams()
{
	m_instRankRecords = 10;
	m_maxCreateRoles = 4;
	LuaGlue* pLuaGlue = GetLuaGlue("default_params.lua");
	if (pLuaGlue == NULL)
		return MLS_SCRIPT_EXE_FAILED;

	LuaGlue lua(pLuaGlue->GetLuaState());
	VariantVec inParamVec;
	VariantVec outParamVec;

	//��ȡ��ʼ������,�������Ƿ�
	bool ret = lua.ExecFunction("GetBaseTaskExpAndRose", &inParamVec, 2, outParamVec);
	if (!ret || outParamVec.size() < 2)
		return MLS_SCRIPT_EXE_FAILED;
	m_baseTaskExp = outParamVec[1].GetUnsignedInt();
	m_taskExpRose = outParamVec[0].GetUnsignedInt();

	//��ȡ��ʼ��������,���������Ƿ�
	outParamVec.clear();
	ret = lua.ExecFunction("GetBaseInstExpAndRose", &inParamVec, 2, outParamVec);
	if (!ret || outParamVec.size() < 2)
		return MLS_SCRIPT_EXE_FAILED;
	m_baseInstExp = outParamVec[1].GetUnsignedInt();
	m_instExpRose = outParamVec[0].GetUnsignedInt();

	//��ȡ��ʼ�����,������Ƿ�
	outParamVec.clear();
	ret = lua.ExecFunction("GetBaseActionExpAndRose", &inParamVec, 2, outParamVec);
	if (!ret || outParamVec.size() < 2)
		return MLS_SCRIPT_EXE_FAILED;
	m_baseActionExp = outParamVec[1].GetUnsignedInt();
	m_actionExpRose = outParamVec[0].GetUnsignedInt();

	//��ȡ��������
	outParamVec.clear();
	ret = lua.ExecFunction("GetBaseConst", &inParamVec, 1, outParamVec);
	if (!ret || outParamVec.size() < 1)
		return MLS_SCRIPT_EXE_FAILED;
	m_baseConst = outParamVec[0].GetDouble();

	//��ȡ��Ҫ������,��������,��������ڵĵȼ�
	outParamVec.clear();
	ret = lua.ExecFunction("GetBaseLevels", &inParamVec, 3, outParamVec);
	if (!ret || outParamVec.size() < 3)
		return MLS_SCRIPT_EXE_FAILED;
	m_baseTaskLv = (BYTE)outParamVec[2].GetUnsignedInt();
	m_baseInstLv = (BYTE)outParamVec[1].GetUnsignedInt();
	m_baseActionLv = (BYTE)outParamVec[0].GetUnsignedInt();

	//ÿ���������������������
	outParamVec.clear();
	ret = lua.ExecFunction("GetInstRankNum", &inParamVec, 1, outParamVec);
	if (!ret || outParamVec.size() < 1)
		return MLS_SCRIPT_EXE_FAILED;
	m_instRankRecords = (BYTE)outParamVec[0].GetUnsignedInt();

	//���ɴ�����ɫ����
	outParamVec.clear();
	ret = lua.ExecFunction("GetMaxCreateRoles", &inParamVec, 1, outParamVec);
	if (!ret || outParamVec.size() < 1)
		return MLS_SCRIPT_EXE_FAILED;
	m_maxCreateRoles = (BYTE)outParamVec[0].GetUnsignedInt();

	m_luaStateMap.erase("default_params.lua");
	delete pLuaGlue;

	return MLS_OK;
}
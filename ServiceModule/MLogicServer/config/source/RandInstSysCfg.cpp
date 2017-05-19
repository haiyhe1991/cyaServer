#include "RandInstSysCfg.h"
#include "cyaTime.h"
#include "cyaMaxMin.h"

RandInstSysCfg::RandInstSysCfg()
{

}

RandInstSysCfg::~RandInstSysCfg()
{

}

BOOL RandInstSysCfg::Load(const char* filename)
{
	m_filename = filename;
	return true;
}

BOOL RandInstSysCfg::GetRandInstSysParam(SRandInstSysParam& param)
{
	Json::Value rootValue;
	if (!OpenCfgFile(m_filename.c_str(), rootValue))
		return false;

	if (!rootValue.isMember("SStageSystem"))
		return false;

	param.isOpen = (BYTE)rootValue["SStageSystem"]["IsOpen"].asInt();
	param.chance = (BYTE)rootValue["SStageSystem"]["Chance"].asInt();
	param.timerInterVal = rootValue["SStageSystem"]["Interval"].asInt();
	param.openDurationTime = rootValue["SStageSystem"]["LastTime"].asInt();
	param.maxNumPerTransfer = rootValue["SStageSystem"]["LimitNum"].asInt();
	param.maxOpenTransferNum = rootValue["SStageSystem"]["MaxOpenNum"].asInt();
	param.minOpenTransferNum = rootValue["SStageSystem"]["MinOpenNum"].asInt();
	UINT16 transferNum = rootValue["SStageSystem"]["TransferNum"].asInt();
	for (UINT16 i = 1; i <= transferNum; ++i)
		param.transferVec.push_back(i);

	const char* pszStartTime = rootValue["SStageSystem"]["StartTime"].asCString();
	param.startHour = atoi(pszStartTime);
	const char* pSep = strstr(pszStartTime, ":");
	if (pSep != NULL)
		param.startMin = atoi(pSep + sizeof(char));
	else
		param.startMin = 0;

	const char* pszEndTime = rootValue["SStageSystem"]["EndTime"].asCString();
	param.endHour = atoi(pszEndTime);
	pSep = strstr(pszEndTime, ":");
	if (pSep != NULL)
		param.endMin = atoi(pSep + sizeof(char));
	else
		param.endMin = 0;

	return true;
}
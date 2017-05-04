#include "ServiceProtocol.h"
#include "ServiceErrorCode.h"
#include "LicenceSession.h"
#include "ExtServeLogicMan.h"
#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "PartitionInfoMan.h"
#include "cyaMaxMin.h"

#define ADD_EXTMAN_PROC_HANDLER(cmdCode, fun) \
	do \
		{	\
	m_handlerMap.insert(std::make_pair(cmdCode, &ExtServeLogicMan::fun));	\
		} while(0)

static ExtServeLogicMan* sg_extLogicMan = NULL;
void InitExtServeLogicMan()
{
	ASSERT(sg_extLogicMan == NULL);
	sg_extLogicMan = new ExtServeLogicMan();
	ASSERT(sg_extLogicMan != NULL);
}

ExtServeLogicMan* GetExtServeLogicMan()
{
	return sg_extLogicMan;
}

void DestroyExtServeLogicMan()
{
	ExtServeLogicMan* pLogicMan = sg_extLogicMan;
	sg_extLogicMan = NULL;
	if (pLogicMan != NULL)
		delete pLogicMan;
}

ExtServeLogicMan::ExtServeLogicMan()
{
	ADD_EXTMAN_PROC_HANDLER(LCS_QUERY_ACCOUNT_PARTITION, OnQueryUserPartition);
	ADD_EXTMAN_PROC_HANDLER(LCS_QUERY_PLAYER_ACCOUNT, OnQueryPlayerAccount);
	ADD_EXTMAN_PROC_HANDLER(LCS_QUERY_GAME_PARTITION, OnQueryGamePartition);

	ADD_EXTMAN_PROC_HANDLER(LCS_ADD_PARTITION, OnAddPartition);
	ADD_EXTMAN_PROC_HANDLER(LCS_DEL_PARTITION, OnDelPartition);
	ADD_EXTMAN_PROC_HANDLER(LCS_MODIFY_PARTITION, OnModPartitionName);
	ADD_EXTMAN_PROC_HANDLER(LCS_SET_RECOMMEND_PARTITION, OnSetRecommandPartition);

	ADD_EXTMAN_PROC_HANDLER(LCS_START_LINKER_SERVE, OnStartLinkServeResPkt);
	ADD_EXTMAN_PROC_HANDLER(LCS_STOP_LINKER_SERVE, OnStopLinkServeResPkt);
	ADD_EXTMAN_PROC_HANDLER(LCS_START_PARTITION_SERVE, OnStartPartitionServe);
	ADD_EXTMAN_PROC_HANDLER(LCS_STOP_PARTITION_SERVE, OnStopPartitionServe);
}

ExtServeLogicMan::~ExtServeLogicMan()
{

}

int ExtServeLogicMan::ProcessLogic(LicenceSession* licSession, UINT16 cmdCode, const void* payload, int nPayloadLen, BYTE pktType)
{
	int retCode = LCS_OK;
	std::map<UINT16, fnExtManProcessHandler>::iterator it = m_handlerMap.find(cmdCode);
	if (it != m_handlerMap.end())
		retCode = (this->*(it->second))(licSession, payload, nPayloadLen);
	else
	{
		if (pktType != DATA_CTRL_PACKET)
			licSession->SendCommonReply(cmdCode, LCS_INVALID_CMD_CODE);
	}
	return retCode;
}

int ExtServeLogicMan::OnQueryUserPartition(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(nPayloadLen == sizeof(SExtManQueryAccountPartitionReq));
	if (nPayloadLen != sizeof(SExtManQueryAccountPartitionReq))
		return licSession->SendCommonReply(LCS_QUERY_ACCOUNT_PARTITION, LCS_INVALID_CMD_PACKET);

	SExtManQueryAccountPartitionReq* req = (SExtManQueryAccountPartitionReq*)payload;
	req->ntoh();

	BYTE buf[MAX_BUF_LEN] = { 0 };
	SExtManQueryAccountPartitionRes* pAccountPartitionRes = (SExtManQueryAccountPartitionRes*)(buf + SGS_BASIC_RES_LEN);
	pAccountPartitionRes->total = 1;
	pAccountPartitionRes->retNum = 1;
	pAccountPartitionRes->userPartitions[0].id = 1;
	strcpy(pAccountPartitionRes->userPartitions[0].name, "用户数据区一");
	pAccountPartitionRes->hton();

	int nRawPayloadLen = SGS_RES_HEAD_LEN + sizeof(SExtManQueryAccountPartitionRes) + (pAccountPartitionRes->retNum - 1) * sizeof(SAccountPartitionInfo);
	return licSession->SendDataReply(buf, nRawPayloadLen, LCS_QUERY_ACCOUNT_PARTITION, LCS_OK);
}

int ExtServeLogicMan::OnQueryPlayerAccount(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(nPayloadLen == sizeof(SExtManQueryPlayerAccountReq));
	if (nPayloadLen != sizeof(SExtManQueryPlayerAccountReq))
		return licSession->SendCommonReply(LCS_QUERY_PLAYER_ACCOUNT, LCS_INVALID_CMD_PACKET);

	SExtManQueryPlayerAccountReq* req = (SExtManQueryPlayerAccountReq*)payload;
	req->ntoh();

	BYTE buf[MAX_BUF_LEN] = { 0 };
	int nRawPayloadLen = SGS_BASIC_RES_LEN;
	SExtManQueryPlayerAccountRes* pAccountRes = (SExtManQueryPlayerAccountRes*)(buf + nRawPayloadLen);

	int maxNum = (MAX_RES_INNER_USER_BYTES - member_offset(SExtManQueryPlayerAccountRes, total, users)) / sizeof(SAccountnfo);

	DBS_ROW row = NULL;
	DBS_RESULT result = NULL;
	char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
	strcpy(szSQL, "select count(id) from account");
	int retCode = GetDBSClient(DBCLI_VERIFY)->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return licSession->SendCommonReply(LCS_QUERY_PLAYER_ACCOUNT, retCode);

	UINT32 nRows = DBS_NumRows(result);
	UINT32 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 1)
	{
		DBS_FreeResult(result);
		nRawPayloadLen += sizeof(SExtManQueryPlayerAccountRes) + (pAccountRes->retNum - 1) * sizeof(SAccountnfo);
		return licSession->SendDataReply(buf, nRawPayloadLen, LCS_QUERY_PLAYER_ACCOUNT, LCS_OK);
	}
	row = DBS_FetchRow(result);
	pAccountRes->total = _atoi64(row[0]);
	if (pAccountRes->total == 0)
	{
		DBS_FreeResult(result);
		nRawPayloadLen += sizeof(SExtManQueryPlayerAccountRes) + (pAccountRes->retNum - 1) * sizeof(SAccountnfo);
		return licSession->SendDataReply(buf, nRawPayloadLen, LCS_QUERY_PLAYER_ACCOUNT, LCS_OK);
	}

	memset(szSQL, 0, sizeof(szSQL));
	sprintf(szSQL, "select id, name, regtts, visit, lasttts from account limit %u, %d", req->from, min(maxNum, req->num));
	retCode = GetDBSClient(DBCLI_VERIFY)->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return licSession->SendCommonReply(LCS_QUERY_PLAYER_ACCOUNT, retCode);

	nRows = DBS_NumRows(result);
	nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 5)
	{
		DBS_FreeResult(result);
		nRawPayloadLen += sizeof(SExtManQueryPlayerAccountRes) + (pAccountRes->retNum - 1) * sizeof(SAccountnfo);
		return licSession->SendDataReply(buf, nRawPayloadLen, LCS_QUERY_PLAYER_ACCOUNT, LCS_OK);
	}

	UINT16 idx = 0;
	while ((row = DBS_FetchRow(result)) != NULL)
	{
		pAccountRes->users[idx].userId = (UINT32)_atoi64(row[0]);
		strcpy(pAccountRes->users[idx].username, row[1]);
		strcpy(pAccountRes->users[idx].regtts, row[2]);
		pAccountRes->users[idx].visitNum = (UINT32)_atoi64(row[3]);
		strcpy(pAccountRes->users[idx].ltts, row[4]);
		++idx;
	}
	DBS_FreeResult(result);
	pAccountRes->retNum = idx;
	pAccountRes->hton();
	nRawPayloadLen += sizeof(SExtManQueryPlayerAccountRes) + (pAccountRes->retNum - 1) * sizeof(SAccountnfo);
	return licSession->SendDataReply(buf, nRawPayloadLen, LCS_QUERY_PLAYER_ACCOUNT, LCS_OK);
}

int ExtServeLogicMan::OnQueryGamePartition(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(nPayloadLen == sizeof(SExtManQueryGamePartitionReq));
	if (nPayloadLen != sizeof(SExtManQueryGamePartitionReq))
		return licSession->SendCommonReply(LCS_QUERY_GAME_PARTITION, LCS_INVALID_CMD_PACKET);

	SExtManQueryGamePartitionReq* req = (SExtManQueryGamePartitionReq*)payload;
	req->ntoh();

	BYTE buf[MAX_BUF_LEN] = { 0 };
	int nRawPayloadLen = SGS_BASIC_RES_LEN;
	SExtManQueryGamePartitionRes* pGamePartRes = (SExtManQueryGamePartitionRes*)(buf + nRawPayloadLen);
	int retCode = GetPartitionInfoMan()->QueryGamePartition(req->from, req->num, pGamePartRes);
	if (retCode != LCS_OK)
		return licSession->SendCommonReply(LCS_QUERY_GAME_PARTITION, retCode);

	nRawPayloadLen += sizeof(SExtManQueryGamePartitionRes) + (pGamePartRes->retNum - 1) * sizeof(SGamePartitionInfo);
	pGamePartRes->hton();
	return licSession->SendDataReply(buf, nRawPayloadLen, LCS_QUERY_GAME_PARTITION, LCS_OK);
}

int ExtServeLogicMan::OnAddPartition(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(nPayloadLen == sizeof(SExtAddPartitionReq));
	if (nPayloadLen != sizeof(SExtAddPartitionReq))
		return licSession->SendCommonReply(LCS_ADD_PARTITION, LCS_INVALID_CMD_PACKET);

	SExtAddPartitionReq* req = (SExtAddPartitionReq*)payload;

	BYTE buf[MAX_BUF_LEN] = { 0 };
	int nRawPayloadLen = SGS_BASIC_RES_LEN;
	SExtAddPartitionRes* pAddRes = (SExtAddPartitionRes*)(buf + nRawPayloadLen);
	int retCode = GetPartitionInfoMan()->AddPartition(req->name, req->lic, req->isRecommend, pAddRes->newPartId);
	if (retCode != LCS_OK)
		return licSession->SendCommonReply(LCS_ADD_PARTITION, retCode);

	nRawPayloadLen += sizeof(SExtAddPartitionRes);
	pAddRes->hton();
	return licSession->SendDataReply(buf, nRawPayloadLen, LCS_ADD_PARTITION, LCS_OK);
}

int ExtServeLogicMan::OnDelPartition(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(nPayloadLen == sizeof(SExtDelPartitionReq));
	if (nPayloadLen != sizeof(SExtDelPartitionReq))
		return licSession->SendCommonReply(LCS_DEL_PARTITION, LCS_INVALID_CMD_PACKET);

	SExtDelPartitionReq* req = (SExtDelPartitionReq*)payload;
	req->ntoh();

	//停止分区服务
	int retCode = GetPartitionInfoMan()->StopPartitionLinkerServe(req->partitionId);
	if (retCode != LCS_OK)
		return licSession->SendCommonReply(LCS_DEL_PARTITION, retCode);

	retCode = GetPartitionInfoMan()->DelPartition(req->partitionId);
	return licSession->SendCommonReply(LCS_DEL_PARTITION, retCode);
}

int ExtServeLogicMan::OnModPartitionName(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(nPayloadLen == sizeof(SExtModPartitionReq));
	if (nPayloadLen != sizeof(SExtModPartitionReq))
		return licSession->SendCommonReply(LCS_MODIFY_PARTITION, LCS_INVALID_CMD_PACKET);

	SExtModPartitionReq* req = (SExtModPartitionReq*)payload;
	req->ntoh();

	int retCode = GetPartitionInfoMan()->ModifyPartitionName(req->partitionId, req->newName);
	return licSession->SendCommonReply(LCS_MODIFY_PARTITION, retCode);
}

int ExtServeLogicMan::OnSetRecommandPartition(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(nPayloadLen == sizeof(SExtSetRecommendPartitionReq));
	if (nPayloadLen != sizeof(SExtSetRecommendPartitionReq))
		return licSession->SendCommonReply(LCS_SET_RECOMMEND_PARTITION, LCS_INVALID_CMD_PACKET);

	SExtSetRecommendPartitionReq* req = (SExtSetRecommendPartitionReq*)payload;
	req->ntoh();

	int retCode = GetPartitionInfoMan()->SetRecommendPartition(req->partitionId);
	return licSession->SendCommonReply(LCS_SET_RECOMMEND_PARTITION, retCode);
}

int ExtServeLogicMan::OnStartPartitionServe(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	//向link服务器发起开启请求
	ASSERT(sizeof(SExtCtrlPartitionReq) == nPayloadLen);
	if (sizeof(SExtCtrlPartitionReq) != nPayloadLen)
		return licSession->SendCommonReply(LCS_START_PARTITION_SERVE, LCS_INVALID_CMD_PACKET);

	SExtCtrlPartitionReq* req = (SExtCtrlPartitionReq*)payload;
	req->ntoh();
	int retCode = GetPartitionInfoMan()->StartPartitionLinkerServe(req->partitionId);
	return licSession->SendCommonReply(LCS_START_PARTITION_SERVE, retCode);
}

int ExtServeLogicMan::OnStopPartitionServe(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	//向link服务器发起停止请求
	ASSERT(sizeof(SExtCtrlPartitionReq) == nPayloadLen);
	if (sizeof(SExtCtrlPartitionReq) != nPayloadLen)
		return licSession->SendCommonReply(LCS_STOP_PARTITION_SERVE, LCS_INVALID_CMD_PACKET);

	SExtCtrlPartitionReq* req = (SExtCtrlPartitionReq*)payload;
	req->ntoh();
	int retCode = GetPartitionInfoMan()->StopPartitionLinkerServe(req->partitionId);
	return licSession->SendCommonReply(LCS_STOP_PARTITION_SERVE, retCode);
}

int ExtServeLogicMan::OnStartLinkServeResPkt(LicenceSession* /*licSession*/, const void* /*payload*/, int /*nPayloadLen*/)
{
	return LCS_OK;
}

int ExtServeLogicMan::OnStopLinkServeResPkt(LicenceSession* /*licSession*/, const void* /*payload*/, int /*nPayloadLen*/)
{
	return LCS_OK;
}
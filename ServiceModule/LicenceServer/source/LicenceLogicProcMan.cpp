#include "cyaLog.h"
#include "LicenceLogicProcMan.h"
#include "LicenceSession.h"
#include "ServiceErrorCode.h"
#include "PartitionInfoMan.h"
#include "ConfigFileMan.h"
#include "UserInfoMan.h"
#include "cyaIpCvt.h"
#include "ServiceLicence.h"

#define ADD_LC_PROC_HANDLER(cmdCode, fun) \
    do \
	    {   \
        m_lgProcHandler.insert(std::make_pair(cmdCode, &LicenceLogicProcMan::fun)); \
	    } while(0)

static LicenceLogicProcMan* sg_lcProcMan = NULL;
void InitLicenceLogicProcMan()
{
	ASSERT(sg_lcProcMan == NULL);
	sg_lcProcMan = new LicenceLogicProcMan();
	ASSERT(sg_lcProcMan != NULL);
}

LicenceLogicProcMan* GetLicenceLogicProcMan()
{
	return sg_lcProcMan;
}

void DestroyLicenceLogicProcMan()
{
	LicenceLogicProcMan* lcProcMan = sg_lcProcMan;
	sg_lcProcMan = NULL;
	if (lcProcMan != NULL)
		delete lcProcMan;
}

LicenceLogicProcMan::LicenceLogicProcMan()
{
	ADD_LC_PROC_HANDLER(LICENCE_LINKER_LOGIN, OnLinkerLogin);
	ADD_LC_PROC_HANDLER(LICENCE_USER_LOGIN, OnUserLogin);
	ADD_LC_PROC_HANDLER(LICENCE_LINKER_QUERY_PARTITION_STATUS, OnQueryPartitionStatus);
	ADD_LC_PROC_HANDLER(LICENCE_LINKER_REPORT_CONNECTIONS, OnReportLinkerConnections);

	ADD_LC_PROC_HANDLER(LICENCE_USER_ENTER_PARTITION, OnUserEnterPartition);
	ADD_LC_PROC_HANDLER(LICENCE_USER_QUERY_PARTITION, OnUserQueryPartitionInfo);
	ADD_LC_PROC_HANDLER(LICENCE_LINKER_USER_CHECK, OnNotifyLinkerUserOffline);
	ADD_LC_PROC_HANDLER(LICENCE_LINKER_ROLE_CHANGE, OnCreateOrDelRole);

	ADD_LC_PROC_HANDLER(LICENCE_QUERY_USER_LOGIN, OnQueryUserLoginInfo);
	ADD_LC_PROC_HANDLER(LICENCE_LINKER_GET_TOKEN, OnLinkGetTokenResPkt);
	ADD_LC_PROC_HANDLER(LICENCE_GET_REPORT_TIMER, OnGetLinkerReportTimer);
	ADD_LC_PROC_HANDLER(LICENCE_USER_REGIST, OnRegistUser);

	ADD_LC_PROC_HANDLER(LICENCE_QUERY_UNLOCK_JOBS, OnQueryUnlockJobs);
	ADD_LC_PROC_HANDLER(LICENCE_LINKER_QUERY_UNLOCK_JOBS, OnLinkerQueryUnlockJobsResPkt);
}

LicenceLogicProcMan::~LicenceLogicProcMan()
{

}

int LicenceLogicProcMan::ProcessLogic(LicenceSession* licSession, UINT16 cmdCode, const void* payload, int nPayloadLen, BYTE pktType)
{
	int retCode = LCS_OK;
	std::map<UINT16, fnLgProcessHandler>::iterator it = m_lgProcHandler.find(cmdCode);
	if (it != m_lgProcHandler.end())
		retCode = (this->*(it->second))(licSession, payload, nPayloadLen);
	else
	{
		if (pktType != DATA_CTRL_PACKET)
			licSession->SendCommonReply(cmdCode, LCS_INVALID_CMD_CODE);
	}
	return retCode;
}

#if SEPARATOR("LinkSession")
void LicenceLogicProcMan::LinkerLogout(UINT32 partitionId, BYTE linkerId)
{
	GetPartitionInfoMan()->LinkerLogout(partitionId, linkerId);
	GetUserInfoMan()->PartitionLinkerBroken(partitionId, linkerId);
}

int LicenceLogicProcMan::OnLinkerLogin(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(nPayloadLen == sizeof(SLinkerLoginReq));
	if (nPayloadLen != sizeof(SLinkerLoginReq))
		return licSession->SendCommonReply(LICENCE_LINKER_LOGIN, LCS_INVALID_CMD_PACKET);

	SLinkerLoginReq* req = (SLinkerLoginReq*)payload;
	req->ntoh();

	char szLinkerIp[16] = { 0 };
	GetStringIP(szLinkerIp, req->linkerIp);
	LogInfo(("LinkServer[%d, %s:%d]登录!", req->linkerId, szLinkerIp, req->linkerPort));

	/*
	验证码确认暂略
	*/

	//添加link会话
	int retCode = GetPartitionInfoMan()->LinkerLogin(req->partitionId, req->linkerId, licSession, req->linkerIp, req->linkerPort);
	if (retCode == LCS_OK)
	{
		licSession->SetId(req->linkerId);
		licSession->SetPartitionId(req->partitionId);
	}

	licSession->GetServeMan()->EnableSessionKeepAliveCheck(licSession, false);
	licSession->SetSessionType(LicenceSession::linker_session);
	licSession->SetSessionTick(GetTickCount());
	return licSession->SendCommonReply(LICENCE_LINKER_LOGIN, retCode);
}

int LicenceLogicProcMan::OnQueryPartitionStatus(LicenceSession* licSession, const void* /*payload*/, int /*nPayloadLen*/)
{
	if (licSession->GetId() == 0)
		return licSession->SendCommonReply(LICENCE_LINKER_QUERY_PARTITION_STATUS, LCS_USER_NOT_LOGIN);

	BYTE status = 0;
	int retCode = GetPartitionInfoMan()->GetPartitionStatus(licSession->GetPartitionId(), status);
	if (retCode != LCS_OK)
		return licSession->SendCommonReply(LICENCE_LINKER_QUERY_PARTITION_STATUS, retCode);

	int nRawPayloadLen = SGS_RES_HEAD_LEN + sizeof(SPartitionStatusQueryRes);
	BYTE buf[MAX_BUF_LEN] = { 0 };
	SPartitionStatusQueryRes* pStatus = (SPartitionStatusQueryRes*)(buf + SGS_BASIC_RES_LEN);
	pStatus->status = status;

	LogDebug(("查询分区%d状态[%d]!", licSession->GetPartitionId(), status));
	return licSession->SendDataReply(buf, nRawPayloadLen, LICENCE_LINKER_QUERY_PARTITION_STATUS, retCode);
}

int LicenceLogicProcMan::OnReportLinkerConnections(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	licSession->SetSessionTick(GetTickCount());
	ASSERT(nPayloadLen == sizeof(SLinkerReportConnections));
	if (nPayloadLen != sizeof(SLinkerReportConnections))
		return licSession->SendCommonReply(LICENCE_LINKER_REPORT_CONNECTIONS, LCS_INVALID_CMD_PACKET);

	SLinkerReportConnections* pReport = (SLinkerReportConnections*)payload;
	pReport->ntoh();
	int retCode = GetPartitionInfoMan()->UpdateLinkerConnections(licSession->GetPartitionId(), licSession->GetId(), pReport->connections, pReport->tokens);
	return licSession->SendCommonReply(LICENCE_LINKER_REPORT_CONNECTIONS, retCode);
}

int LicenceLogicProcMan::OnGetLinkerReportTimer(LicenceSession* licSession, const void* /*payload*/, int /*nPayloadLen*/)
{
	if (licSession->GetId() == 0)
		return licSession->SendCommonReply(LICENCE_GET_REPORT_TIMER, LCS_USER_NOT_LOGIN);

	DWORD dwTimer = GetConfigFileMan()->GetLinkerHeartbeatInterval() / 1000;
	int nRawPayloadLen = SGS_RES_HEAD_LEN + sizeof(SLinkerGetReportTimerRes);
	BYTE buf[MAX_BUF_LEN] = { 0 };
	SLinkerGetReportTimerRes* pTimer = (SLinkerGetReportTimerRes*)(buf + SGS_BASIC_RES_LEN);
	pTimer->timerSec = dwTimer > 0 ? dwTimer : DEFAULT_LINKER_HEART_TIMER / 1000;

	LogDebug(("linker[%d]获取上报连接数时间间隔[%u]秒!", licSession->GetId(), pTimer->timerSec));

	pTimer->hton();
	return licSession->SendDataReply(buf, nRawPayloadLen, LICENCE_GET_REPORT_TIMER, MLS_OK);
}

int LicenceLogicProcMan::OnLinkGetTokenResPkt(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	//向用户回复
	ASSERT(SGS_RES_HEAD_LEN + sizeof(SLinkerGetUserTokenRes) == nPayloadLen);
	if (SGS_RES_HEAD_LEN + sizeof(SLinkerGetUserTokenRes) != nPayloadLen)
		return GetUserInfoMan()->SendUserEnterLinkerReply(licSession->GetPartitionId(), 0, NULL, 0, 0, LCS_INVALID_CMD_PACKET);

	SGSResPayload* res = (SGSResPayload*)payload;
	SLinkerGetUserTokenRes* pTokenRes = (SLinkerGetUserTokenRes*)((char*)res + SGS_RES_HEAD_LEN);
	pTokenRes->ntoh();

	//获取分区linker的地址信息
	UINT32 linkerIp;
	UINT16 linkerPort;
	int retCode = GetPartitionInfoMan()->GetPartitionLinkerAddr(licSession->GetPartitionId(), licSession->GetId(), linkerIp, linkerPort);
	//ASSERT(retCode == LCS_OK);

	//向用户发送token,linkerIp,linkerPort
	LogDebug(("收到Linker[%d]生成用户[%u]token[%s]", licSession->GetId(), pTokenRes->userId, pTokenRes->token));
	GetUserInfoMan()->SendUserEnterLinkerReply(licSession->GetPartitionId(), pTokenRes->userId, pTokenRes->token, linkerIp, linkerPort, retCode);
	return LCS_OK;
}

//通知其他linker上的用户下线
int LicenceLogicProcMan::OnNotifyLinkerUserOffline(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(sizeof(SLinkerNotifyUserCheck) == nPayloadLen);
	if (sizeof(SLinkerNotifyUserCheck) != nPayloadLen)
		return LCS_OK;

	SLinkerNotifyUserCheck* chk = (SLinkerNotifyUserCheck*)payload;
	chk->ntoh();
	return GetPartitionInfoMan()->NotifyUserFromLinkerOffline(licSession->GetPartitionId(), licSession->GetId(), chk->userId);
}

//用户创建或删除角色
int LicenceLogicProcMan::OnCreateOrDelRole(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(nPayloadLen == sizeof(SRoleOperReq));
	if (nPayloadLen != sizeof(SRoleOperReq))
		return licSession->SendCommonReply(LICENCE_LINKER_ROLE_CHANGE, LCS_INVALID_CMD_PACKET);

	//  SRoleOperReq* opt = (SRoleOperReq*)payload;
	//  opt->ntoh();
	//  int retCode = GetPartitionRolesMan()->AddOrDelRole(opt->userId, licSession->GetPartitionId(), opt->model);
	return licSession->SendCommonReply(LICENCE_LINKER_ROLE_CHANGE, LCS_OK);
}

int LicenceLogicProcMan::OnLinkerQueryUnlockJobsResPkt(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	//向用户回复
	SLinkerQueryUnlockJobsRes* pUnlockJobsRes = (SLinkerQueryUnlockJobsRes*)((char*)payload + SGS_RES_HEAD_LEN);
	pUnlockJobsRes->ntoh();
	return GetUserInfoMan()->SendUserQueryUnlockJobsReply(pUnlockJobsRes);
}
#endif

#if SEPARATOR("UserSession")
//查询已解锁职业
int LicenceLogicProcMan::OnQueryUnlockJobs(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(sizeof(SQueryUnlockJobsReq) == nPayloadLen);
	if (nPayloadLen != sizeof(SQueryUnlockJobsReq))
		return licSession->SendCommonReply(LICENCE_QUERY_UNLOCK_JOBS, LCS_INVALID_CMD_PACKET);

	SQueryUnlockJobsReq* req = (SQueryUnlockJobsReq*)payload;
	req->hton();

	LogInfo(("用户[%d]查询已解锁职业", req->userId));

	int retCode = GetUserInfoMan()->QueryUserUnlockJobs(req->userId);
	if (retCode != LCS_OK)
		licSession->SendCommonReply(LICENCE_QUERY_UNLOCK_JOBS, retCode);

	return retCode;
}

void LicenceLogicProcMan::UserLogout(UINT32 userId)
{
	GetUserInfoMan()->UserLogout(userId);
}

int LicenceLogicProcMan::OnUserLogin(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(nPayloadLen == sizeof(SUserTokenLoginReq) || nPayloadLen == sizeof(SUserPswLoginReq));
	if (nPayloadLen != sizeof(SUserTokenLoginReq) && nPayloadLen != sizeof(SUserPswLoginReq))
		return licSession->SendCommonReply(LICENCE_USER_LOGIN, LCS_INVALID_CMD_PACKET);

	int retCode = LCS_OK;
	BYTE mode = *(BYTE*)payload;
	UINT32 userId = 0;
	if (mode == 0)
	{
		if (nPayloadLen != sizeof(SUserTokenLoginReq))
			return licSession->SendCommonReply(LICENCE_USER_LOGIN, LCS_INVALID_CMD_PACKET);

		SUserTokenLoginReq* pLogin = (SUserTokenLoginReq*)payload;
		retCode = GetUserInfoMan()->UserLogin(licSession, pLogin->token, NULL, pLogin->loginPlat, pLogin->os, pLogin->devid, pLogin->model, pLogin->ver, pLogin->mode, userId);
		if (retCode == LCS_OK)
			LogInfo(("用户[%s]ID[%u],平台[%s],模式[0]登录!", pLogin->token, userId, pLogin->loginPlat));
	}
	else if (mode == 1)
	{
		if (nPayloadLen != sizeof(SUserPswLoginReq))
			return licSession->SendCommonReply(LICENCE_USER_LOGIN, LCS_INVALID_CMD_PACKET);

		SUserPswLoginReq* pLogin = (SUserPswLoginReq*)payload;
		retCode = GetUserInfoMan()->UserLogin(licSession, pLogin->username, pLogin->password, pLogin->loginPlat, pLogin->os, pLogin->devid, pLogin->model, pLogin->ver, pLogin->mode, userId);
		if (retCode == LCS_OK)
			LogInfo(("用户[%s]ID[%u],平台[%s],模式[1]登录!", pLogin->username, userId, pLogin->loginPlat));
	}
	else
	{
		ASSERT(false);
		retCode = LCS_INVALID_CMD_PACKET;
	}

	if (retCode != LCS_OK)
		return licSession->SendCommonReply(LICENCE_USER_LOGIN, retCode);

	int nRawPayloadLen = SGS_RES_HEAD_LEN + sizeof(SUserLoginRes);
	BYTE buf[MAX_BUF_LEN] = { 0 };
	SUserLoginRes* pLoginRes = (SUserLoginRes*)(buf + SGS_BASIC_RES_LEN);
	pLoginRes->userId = userId;
	strcpy(pLoginRes->username, mode == 0 ? ((SUserTokenLoginReq*)payload)->token : ((SUserPswLoginReq*)payload)->username);
	pLoginRes->hton();

	licSession->SetId(userId);
	licSession->SetSessionType(LicenceSession::user_session);
	//licSession->SetSessionTick(GetTickCount());

	return licSession->SendDataReply(buf, nRawPayloadLen, LICENCE_USER_LOGIN, retCode);
}

//用户进入分区
int LicenceLogicProcMan::OnUserEnterPartition(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	if (licSession->GetId() == 0)
		return licSession->SendCommonReply(LICENCE_USER_ENTER_PARTITION, LCS_USER_NOT_LOGIN);

	ASSERT(nPayloadLen == sizeof(SUserEnterPartitionReq));
	if (nPayloadLen != sizeof(SUserEnterPartitionReq))
		return licSession->SendCommonReply(LICENCE_USER_ENTER_PARTITION, LCS_INVALID_CMD_PACKET);

	SUserEnterPartitionReq* req = (SUserEnterPartitionReq*)payload;
	req->ntoh();

	//获取登陆账号id的账号名(是否登陆过)
	std::string username;
	UINT16 enterPartId = 0;
	int retCode = GetUserInfoMan()->GetUsernameByUserId(username, enterPartId, req->userId);
	if (retCode != LCS_OK)
		return licSession->SendCommonReply(LICENCE_USER_ENTER_PARTITION, retCode);

	//非自动选区
	if (!GetConfigFileMan()->IsAutoPartition())
		enterPartId = req->partitionId;

	LogDebug(("用户[%s]进入分区%d", username.c_str(), enterPartId));

	//记录用户访问信息
	LTMSEL nowMSel = GetMsel();
	GetUserInfoMan()->UpdateUserLoginGameInfo(req->userId, enterPartId, nowMSel);

	//获取分区负载最小的linker
	BYTE linkerId;
	retCode = GetPartitionInfoMan()->GetMinPayloadLinker(enterPartId, linkerId);
	if (retCode != LCS_OK)
		return licSession->SendCommonReply(LICENCE_USER_ENTER_PARTITION, retCode);

	//保存用户要进入分区的partition/linker id/时间
	retCode = GetUserInfoMan()->SetUserEnterPartitionLinkerId(req->userId, enterPartId, linkerId, nowMSel);
	if (retCode != LCS_OK)
		return licSession->SendCommonReply(LICENCE_USER_ENTER_PARTITION, retCode);

	//到对应的linker服务器中获取token
	retCode = GetPartitionInfoMan()->UserEnterLinkerGetToken(enterPartId, linkerId, req->userId, username.c_str());
	if (retCode != LCS_OK)
		return licSession->SendCommonReply(LICENCE_USER_ENTER_PARTITION, retCode);

	return LCS_OK;
}

//用户查询分区信息
int LicenceLogicProcMan::OnUserQueryPartitionInfo(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(sizeof(SUserQueryPartitionReq) == nPayloadLen);
	if (nPayloadLen != sizeof(SUserQueryPartitionReq))
		return licSession->SendCommonReply(LICENCE_USER_QUERY_PARTITION, LCS_INVALID_CMD_PACKET);

	if (licSession->GetId() == 0)
		return licSession->SendCommonReply(LICENCE_USER_QUERY_PARTITION, LCS_USER_NOT_LOGIN);

	SUserQueryPartitionReq* req = (SUserQueryPartitionReq*)payload;
	req->ntoh();

	BYTE buf[MAX_BUF_LEN] = { 0 };
	int nRawPayloadLen = SGS_BASIC_RES_LEN;
	SUserQueryPartitionRes* pPartitionInfoRes = (SUserQueryPartitionRes*)(buf + SGS_BASIC_RES_LEN);
	int retCode = GetPartitionInfoMan()->GetPartitionInfoTable(req->from, req->num, pPartitionInfoRes);
	if (retCode != LCS_OK)
		return licSession->SendCommonReply(LICENCE_USER_QUERY_PARTITION, retCode);

	nRawPayloadLen += sizeof(SUserQueryPartitionRes) + (pPartitionInfoRes->num - 1) * sizeof(SPartitionInfo);
	pPartitionInfoRes->hton();
	LogDebug(("用户[%d]查询分区信息!", licSession->GetId()));
	return licSession->SendDataReply(buf, nRawPayloadLen, LICENCE_USER_QUERY_PARTITION, retCode);
}

//查询用户登录信息
int LicenceLogicProcMan::OnQueryUserLoginInfo(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(sizeof(SUserQueryLoginReq) == nPayloadLen);
	if (nPayloadLen != sizeof(SUserQueryLoginReq))
		return licSession->SendCommonReply(LICENCE_QUERY_USER_LOGIN, LCS_INVALID_CMD_PACKET);

	SUserQueryLoginReq* req = (SUserQueryLoginReq*)payload;
	req->ntoh();

	BYTE buf[MAX_BUF_LEN] = { 0 };
	int nRawPayloadLen = SGS_BASIC_RES_LEN;
	SUserQueryLoginRes* pQueryRes = (SUserQueryLoginRes*)(buf + nRawPayloadLen);
	int retCode = GetUserInfoMan()->QueryUserLatestLoginInfo(req->userId, pQueryRes);
	if (retCode != LCS_OK)
		return licSession->SendCommonReply(LICENCE_QUERY_USER_LOGIN, retCode);

	//获取分区名称
	GetPartitionInfoMan()->GetPartitionName(pQueryRes->lastLoginPartId, pQueryRes->name);

	nRawPayloadLen += sizeof(SUserQueryLoginRes);
	pQueryRes->hton();
	return licSession->SendDataReply(buf, nRawPayloadLen, LICENCE_QUERY_USER_LOGIN, retCode);
}

//注册用户
int LicenceLogicProcMan::OnRegistUser(LicenceSession* licSession, const void* payload, int nPayloadLen)
{
	ASSERT(sizeof(SUserRegistReq) == nPayloadLen);
	if (nPayloadLen != sizeof(SUserRegistReq))
		return licSession->SendCommonReply(LICENCE_USER_REGIST, LCS_INVALID_CMD_PACKET);

	SUserRegistReq* req = (SUserRegistReq*)payload;
	int retCode = GetUserInfoMan()->RegistUser(req);
	return licSession->SendCommonReply(LICENCE_USER_REGIST, retCode);
}
#endif

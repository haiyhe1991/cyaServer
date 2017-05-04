#include "md5.h"
#include "stringHash.h"
#include "cyaLog.h"
#include "DBSClient.h"
#include "cyaMaxMin.h"

#include "ServiceErrorCode.h"
#include "ConfigFileMan.h"
#include "LicenceSession.h"
#include "LicenceServeMan.h"
#include "PartitionInfoMan.h"
#include "UserInfoMan.h"

static UserInfoMan* sg_userInfoMan = NULL;
void InitUserInfoMan()
{
	ASSERT(sg_userInfoMan == NULL);
	sg_userInfoMan = new UserInfoMan();
	ASSERT(sg_userInfoMan != NULL);
}

UserInfoMan* GetUserInfoMan()
{
	return sg_userInfoMan;
}

void DestroyUserInfoMan()
{
	UserInfoMan* userInfoMan = sg_userInfoMan;
	sg_userInfoMan = NULL;
	if (userInfoMan != NULL)
		delete userInfoMan;
}

UserInfoMan::UserInfoMan()
{

}

UserInfoMan::~UserInfoMan()
{

}

#if SEPARATOR("For UserMsg or UserSession")
int UserInfoMan::UserLogin(LicenceSession* session,
	const char* username,
	const char* password,
	const char* platform,
	const char* os,
	const char* devId,
	const char* model,
	const char* ver,
	BYTE mode, UINT32& userId)
{
	UINT16 enterPartId = 0;
	UINT16 lastPartId = 0;
	LTMSEL lasttts = 0;
	int retCode = LCS_OK;
	ASSERT(username != NULL && platform != NULL);
	if (password != NULL)    //从数据库中获取密码
		retCode = LocalUserPswVerify(username, password, userId, lastPartId, lasttts);
	else
		retCode = ThrirdUserPswVerify(username, username, devId, model, ver, platform, os, userId, lastPartId, lasttts);

	//是否自动分区
	if (GetConfigFileMan()->IsAutoPartition())
	{
		if (lastPartId == 0) //未进入过分区
		{
			UINT32 accountNum = 0;
			GetPartitionInfoMan()->GetMinAccountPartition(enterPartId, accountNum);
		}
		else
			enterPartId = lastPartId;   //进入过分区
	}

	if (retCode != LCS_OK || userId <= 0)
		return LCS_ACCOUNT_PSW_WRONG;

	//最大用户负载数
	DWORD maxUserPayload = GetConfigFileMan()->GetMaxUserPayload();

	CXTAutoLock lock(m_locker);
	size_t users = m_userInfoMap.size();
	if (maxUserPayload > 0 && users >= maxUserPayload)   //超过最大负载
		return LCS_OVER_MAX_USER_PAYLOAD;

	std::map<UINT32, UserInfo>::iterator it = m_userInfoMap.find(userId);
	if (it != m_userInfoMap.end())
	{
		LicenceSession* userSession = it->second.session;
		if (userSession != NULL && userSession == session)   //同一个连接多次登陆 ?
		{
			//LogError(("用户[%u]在同一个连接中多次登录", userId));
			//return LCS_USER_LOGIN_AGAIN;
			LogInfo(("用户[%u]在同一个连接中多次登录, 无动作", userId));
			return LCS_OK;
		}

		it->second.session = session;
		it->second.mode = mode;
		it->second.enterLinkerId = 0;
		it->second.username = username;
		//it->second.platform = platform;
		it->second.lastPartId = lastPartId;
		it->second.enterPartId = enterPartId;
		it->second.lastMSel = lasttts;
		userSession->SetId(0);
		userSession->GetServeMan()->CloseSession(userSession);
		LogWarning(("强制前一个用户[%u]下线", userId));
		return LCS_OK;
	}

	UserInfo user;
	user.mode = mode;
	//user.platform = platform;
	user.username = username;
	user.lastMSel = lasttts;
	user.lastPartId = lastPartId;
	user.enterPartId = enterPartId;
	user.session = session;
	m_userInfoMap.insert(std::make_pair(userId, user));
	return LCS_OK;
}

void UserInfoMan::UserLogout(UINT32 userId)
{
	CXTAutoLock lock(m_locker);
	std::map<UINT32, UserInfo>::iterator it = m_userInfoMap.find(userId);
	if (it != m_userInfoMap.end())
		m_userInfoMap.erase(it);
}

int UserInfoMan::GetUsernameByUserId(std::string& username, UINT16& enterPartId, UINT32 userId)
{
	CXTAutoLock lock(m_locker);
	std::map<UINT32, UserInfo>::iterator it = m_userInfoMap.find(userId);
	if (it == m_userInfoMap.end())
		return LCS_USER_NOT_LOGIN;

	username = it->second.username.c_str();
	enterPartId = it->second.enterPartId;
	return LCS_OK;
}

int  UserInfoMan::SetUserEnterPartitionLinkerId(UINT32 userId, UINT16 partitionId, BYTE linkerId, LTMSEL nowMSel)
{
	CXTAutoLock lock(m_locker);
	std::map<UINT32, UserInfo>::iterator it = m_userInfoMap.find(userId);
	if (it == m_userInfoMap.end())
		return LCS_USER_NOT_LOGIN;

	it->second.enterLinkerId = linkerId;
	it->second.lastPartId = partitionId;
	it->second.enterPartId = partitionId;
	it->second.lastMSel = nowMSel;
	return LCS_OK;
}

int UserInfoMan::LocalUserPswVerify(const char* username, const char* psw, UINT32& userId, UINT16& lastPartId, LTMSEL& lasttts)
{
	userId = ELFHash(username);
	std::string strAccountTable;
	GetConfigFileMan()->HashTableName(username, strAccountTable);

	DBS_RESULT result = NULL;
	char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
	sprintf(szSQL, "select pwd, partid, lasttts from %s where id=%u", strAccountTable.c_str(), userId);
	int retCode = GetDBSClient(DBCLI_VERIFY)->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 3)
	{
		DBS_FreeResult(result);
		return LCS_NOT_EXIST_USER;
	}

	DBS_ROW rows = DBS_FetchRow(result);
	ASSERT(rows != NULL);

	std::string strPSW = psw;
	MD5         md5(strPSW);
	if ((strcmp(rows[0], md5.toString().c_str()) != 0)
		&& (strcmp(rows[0], psw) != 0))
	{
		DBS_FreeResult(result);
		return LCS_ACCOUNT_PSW_WRONG;
	}

	lastPartId = atoi(rows[1]);
	lasttts = LocalStrTimeToMSel(rows[2]);
	return LCS_OK;
}

int UserInfoMan::ThrirdUserPswVerify(const char* username,
	const char* psw,
	const char* pszDevId,
	const char* pszModel,
	const char* pszVer,
	const char* pszPlat,
	const char* pszOS,
	UINT32& userId, UINT16& lastPartId, LTMSEL& lasttts)
{
	userId = ELFHash(username);
	std::string strAccountTable;
	GetConfigFileMan()->HashTableName(username, strAccountTable);

	DBS_RESULT result = NULL;
	char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
	sprintf(szSQL, "select pwd, partid, lasttts, ver from %s where id=%u", strAccountTable.c_str(), userId);
	int retCode = GetDBSClient(DBCLI_VERIFY)->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(result);
	UINT16 nCols = DBS_NumFields(result);
	if (nRows <= 0 || nCols < 4) //无此用户
	{
		std::string strAccountTable;
		GetConfigFileMan()->HashTableName(username, strAccountTable);
		char pszDate[32] = { 0 };
		GetLocalStrTime(pszDate);
		memset(szSQL, 0, sizeof(szSQL));
		const char* pStrDevId = pszDevId == NULL ? "" : pszDevId;
		std::string strPSW = psw;
		MD5         md5(strPSW);
		sprintf(szSQL, "insert into %s(id, user, pwd, partid, ip, regtts, visit, regdev, model, ver, lastdev, lasttts, platsource, os, status) "
			"values(%u, '%s', '%s', %d, '%s', '%s', %u, '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d)",
			strAccountTable.c_str(), userId, username, md5.toString().c_str(), 0, "", pszDate, 1, pStrDevId, pszModel, pszVer, pStrDevId, pszDate, pszPlat, pszOS, 0);
		GetDBSClient(DBCLI_VERIFY)->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);

		if ((*pszVer == 0) || (strcmp(pszVer, "0") == 0)) // debug
		{
			DBS_RESULT result2 = NULL;
			memset(szSQL, 0, sizeof(szSQL));
			sprintf(szSQL, "select platid from playdevice where dev_idfa='%s'", pStrDevId);
			retCode = GetDBSClient(2)->Query(GetConfigFileMan()->GetActionDbName(), szSQL, result2);
			if (retCode == DBS_OK)
			{
				nRows = DBS_NumRows(result2);
				nCols = DBS_NumFields(result2);
				if ((nRows > 0) && (nCols == 1))
				{
					DBS_ROW tmprows = DBS_FetchRow(result2);
					memset(szSQL, 0, sizeof(szSQL));
					sprintf(szSQL, "update %s set ver='%s' where id=%u", strAccountTable.c_str(), tmprows[0], userId);
					GetDBSClient(DBCLI_VERIFY)->Update(GetConfigFileMan()->GetDBName(), szSQL, NULL);

					LogInfo(("ThrirdUserPswVerify01--user[%s]ID[%u]pStrDevId[%s] sql[%s]", username, userId, pStrDevId, szSQL));
				}
			}
			else
			{
				LogInfo(("ThrirdUserPswVerify--query platid fail! user[%s]ID[%u]table[%s] sql[%s]", username, userId, strAccountTable.c_str(), szSQL));
			}
			DBS_FreeResult(result2);
		}

		lastPartId = 0;
		lasttts = 0;
	}
	else    //用户存在
	{
		DBS_ROW rows = DBS_FetchRow(result);
		lastPartId = atoi(rows[1]);
		lasttts = LocalStrTimeToMSel(rows[2]);

		if ((*pszVer != 0) && (strcmp(pszVer, "0") != 0) && (strcmp(rows[3], "0") == 0))  // debug
		{
			memset(szSQL, 0, sizeof(szSQL));
			sprintf(szSQL, "update %s set ver='%s' where id=%u", strAccountTable.c_str(), pszVer, userId);
			GetDBSClient(DBCLI_VERIFY)->Update(GetConfigFileMan()->GetDBName(), szSQL, NULL);
			LogInfo(("ThrirdUserPswVerify02--user[%s]ID[%u]DevId[%s]ver[%s][%s] sql[%s]", username, userId, (pszDevId == NULL) ? "" : pszDevId, pszVer, rows[3], szSQL));
		}
	}

	DBS_FreeResult(result);
	return LCS_OK;
}

void UserInfoMan::UpdateUserLoginGameInfo(UINT32 userId, UINT16 partId, LTMSEL nowMSel)
{
	char pszDate[32] = { 0 };
	MselToLocalStrTime(nowMSel, pszDate);

	UINT16 lastEnterPart = 0;
	std::string strAccountTable;
	GetConfigFileMan()->HashTableName(userId, strAccountTable);

	{
		CXTAutoLock lock(m_locker);
		std::map<UINT32, UserInfo>::iterator it = m_userInfoMap.find(userId);
		if (it == m_userInfoMap.end())
			return;
		lastEnterPart = it->second.lastPartId;
	}

	//更新用户访问信息
	char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
	sprintf(szSQL, "update %s set partid=%d, visit=visit+1, lasttts='%s' where id=%u", strAccountTable.c_str(), partId, pszDate, userId);
	GetDBSClient(DBCLI_VERIFY)->Update(GetConfigFileMan()->GetDBName(), szSQL, NULL);

	//更新分区信息(自动选区)
	if (lastEnterPart != partId && GetConfigFileMan()->IsAutoPartition())
		GetPartitionInfoMan()->UpdatePartitionUsers(partId);
}

int UserInfoMan::QueryUserLatestLoginInfo(UINT32 userId, SUserQueryLoginRes* pQueryRes)
{
	CXTAutoLock lock(m_locker);
	std::map<UINT32, UserInfo>::iterator it = m_userInfoMap.find(userId);
	if (it == m_userInfoMap.end())
		return LCS_USER_NOT_LOGIN;

	MselToLocalStrTime(it->second.lastMSel, pQueryRes->lastLoginDate);
	pQueryRes->lastLoginPartId = it->second.lastPartId;
	return LCS_OK;
}

int UserInfoMan::RegistUser(const SUserRegistReq* req)
{
	char szSQL[LCS_SQL_BUF_SIZE] = { 0 };
	UINT32 userId = ELFHash(req->username);
	std::string strTableName;
	GetConfigFileMan()->HashTableName(userId, strTableName);
	//  sprintf(szSQL, "select id from %s where id=%u", strTableName.c_str(), userId);
	//
	//  DBS_RESULT result = NULL;
	//  int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	//  if(retCode != DBS_OK)
	//      return retCode;
	//
	//  UINT32 nRows = DBS_NumRows(result);
	//  DBS_FreeResult(result);
	//  if(nRows > 0)
	//      return LCS_EXIST_ACCOUNT_NAME;
	//
	//  memset(szSQL, 0, sizeof(szSQL));
	char szDate[32] = { 0 };
	GetLocalStrTime(szDate);
	std::string strPSW = req->password;
	MD5         md5(strPSW);
	sprintf(szSQL, "insert into %s(id, user, pwd, ip, regtts, platsource, model, os, regdev, visit, lasttts, lastdev, ver, partid, status) values"
		"(%u, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %u, '%s', '%s', '%s', %d, %d)", strTableName.c_str(), userId,
		req->username, md5.toString().c_str(), "", szDate, req->loginPlat, req->model, req->os, req->devid, 0, szDate, req->devid, req->ver, 0, 0);

	int retCode = GetDBSClient(DBCLI_VERIFY)->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	LogInfo(("注册账号[%s]ID[%u]", req->username, userId));

	// cash表设置accountid字段
	char buf[32] = { 0 };
	sprintf(buf, "cash_%d", (userId % SGS_PLAYER_ACCOUNT_TABLE_NUM) + 1);
	sprintf(szSQL, "insert into %s(accountid) values(%u)", buf, userId);
	GetDBSClient(DBCLI_CASH)->Insert(GetConfigFileMan()->GetCashDbName(), szSQL, NULL);

	return retCode == DBS_OK ? LCS_OK : LCS_EXIST_ACCOUNT_NAME;
}

int UserInfoMan::QueryUserUnlockJobs(UINT32 userId)
{
	UINT16 enterPartId = 0;
	{
		CXTAutoLock lock(m_locker);
		std::map<UINT32, UserInfo>::iterator it = m_userInfoMap.find(userId);
		if (it == m_userInfoMap.end())
			return LCS_USER_NOT_LOGIN;

		enterPartId = it->second.enterPartId;
	}

	LogInfo(("用户[%d]向连接服务器发起职业解锁查询", userId));
	// 向连接服务器发起职业解锁查询
	return GetPartitionInfoMan()->QueryUnlockJobs(enterPartId, userId);
}
#endif

#if SEPARATOR("For LinkServerMsg or LinkSession")
int UserInfoMan::SendUserQueryUnlockJobsReply(const SLinkerQueryUnlockJobsRes* pUnlockJobsRes)
{
	BYTE buf[MAX_BUF_LEN] = { 0 };
	BOOL isCrypt = GetConfigFileMan()->IsDataEncrypt();
	SQueryUnlockJobsRes* pUserUnlockJobsRes = (SQueryUnlockJobsRes*)(buf + SGS_BASIC_RES_LEN);
	UINT16 maxNum = (UINT16)((MAX_RES_USER_BYTES - member_offset(SQueryUnlockJobsRes, num, data)) / sizeof(BYTE));
	pUserUnlockJobsRes->num = min(pUnlockJobsRes->num, maxNum);

	for (UINT16 i = 0; i < pUserUnlockJobsRes->num; ++i)
		pUserUnlockJobsRes->data[i] = pUnlockJobsRes->data[i];

	int nRawPayloadLen = SGS_RES_HEAD_LEN + sizeof(SQueryUnlockJobsRes) + (pUserUnlockJobsRes->num - 1) * sizeof(BYTE);
	pUserUnlockJobsRes->hton();

	LogInfo(("收到连接服务器的职业解锁查询回复"));

	CXTAutoLock lock(m_locker);
	std::map<UINT32, UserInfo>::iterator it = m_userInfoMap.find(pUnlockJobsRes->userId);
	if (it == m_userInfoMap.end())
		return LCS_USER_NOT_LOGIN;

	LicenceSession* pUserSession = it->second.session;
	if (pUserSession != NULL)
		pUserSession->SendDataReply(buf, nRawPayloadLen, LICENCE_QUERY_UNLOCK_JOBS, LCS_OK, isCrypt);

	return LCS_OK;
}

int  UserInfoMan::SendUserEnterLinkerReply(UINT16 partitionId, UINT32 userId, const char* token, UINT32 linkerIp, UINT16 linkerPort, int retCode)
{
	CXTAutoLock lock(m_locker);
	std::map<UINT32, UserInfo>::iterator it = m_userInfoMap.find(userId);
	if (it == m_userInfoMap.end())
		return LCS_USER_NOT_LOGIN;  //丢掉

	if (it->second.enterPartId != partitionId)
		return LCS_USER_NOT_LOGIN;  //丢掉

	LicenceSession* session = it->second.session;
	if (session == NULL)
		return LCS_USER_NOT_LOGIN;

	it->second.enterLinkerId = 0;
	it->second.enterPartId = 0;
	return session->SendUserEnterLinkerReply(partitionId, userId, token, linkerIp, linkerPort, retCode);
}

void UserInfoMan::PartitionLinkerBroken(UINT16 partitionId, BYTE linkerId)
{
	if (GetLicenceServeMan() == NULL)
		return;

	CXTAutoLock lock(m_locker);
	std::map<UINT32, UserInfo>::iterator it = m_userInfoMap.begin();
	for (; it != m_userInfoMap.end(); ++it)
	{
		UserInfo& user = it->second;
		LicenceSession* session = it->second.session;
		if (session != NULL && user.enterLinkerId == linkerId && user.enterPartId == partitionId)
			session->SendCommonReply(LICENCE_USER_ENTER_PARTITION, LCS_LINKER_NOT_LOGIN);
	}
}
#endif
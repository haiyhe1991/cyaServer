#include "EmailCleanMan.h"
#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "GameCfgFileMan.h"
#include "cyaLog.h"
#include "RolesInfoMan.h"
#include "cyaMaxMin.h"
#include "gb_utf8.h"
#include "GWSClientMan.h"

static EmailCleanMan* sg_emailCleanMan = NULL;
void InitEmailCleanMan()
{
	ASSERT(sg_emailCleanMan == NULL);
	sg_emailCleanMan = new EmailCleanMan();
	ASSERT(sg_emailCleanMan != NULL);
	sg_emailCleanMan->Start();
}

EmailCleanMan* GetEmailCleanMan()
{
	return sg_emailCleanMan;
}

void DestroyEmailCleanMan()
{
	EmailCleanMan* cleanMan = sg_emailCleanMan;
	sg_emailCleanMan = NULL;
	if (cleanMan != NULL)
	{
		cleanMan->Stop();
		delete cleanMan;
	}
}

EmailCleanMan::EmailCleanMan()
: m_thHandle(INVALID_OSTHREAD)
, m_threadExit(true)
, m_lastCleanMSel(0)
{

}

EmailCleanMan::~EmailCleanMan()
{

}

void EmailCleanMan::Start()
{
	m_threadExit = false;
	OSCreateThread(&m_thHandle, NULL, THWorker, this, 0);
}

void EmailCleanMan::Stop()
{
	m_threadExit = true;
	m_sleep.Interrupt();
	if (m_thHandle != INVALID_OSTHREAD)
	{
		OSCloseThread(m_thHandle);
		m_thHandle = INVALID_OSTHREAD;
	}
}

int EmailCleanMan::THWorker(void* param)
{
	EmailCleanMan* pThis = (EmailCleanMan*)param;
	ASSERT(pThis != NULL);
	return pThis->OnWorker();
}

int EmailCleanMan::OnWorker()
{
	while (!m_threadExit)
	{
		LTMSEL ltNow = GetMsel();
		LTMSEL n = ltNow - m_lastCleanMSel;
		if (n < 0 || n >= 600 * 1000)	//10分钟清理一次
		{
			OnCleanUnReadEmails(ltNow);	//清理过期未读邮件
			OnCleanReadEmails(ltNow);	//清理过期已读邮件
			m_lastCleanMSel = ltNow;
		}

		//异步发送邮件
		OnSendEmail();

		m_sleep.Sleep(5 * 1000);	//5秒
	}
	return 0;
}

void EmailCleanMan::OnCleanUnReadEmails(LTMSEL ltNow)
{
	LTMSEL delTime = ltNow - ((LTMSEL)GetConfigFileMan()->GetMaxUnReadEmailSaveDays()) * 24 * 3600 * 1000;	//毫秒
	char szTime[32] = { 0 };
	SYSTEMTIME sysDelTime = ToTime(delTime);
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", sysDelTime.wYear, sysDelTime.wMonth, sysDelTime.wDay, sysDelTime.wHour, sysDelTime.wMinute, sysDelTime.wSecond);
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "delete from email where state=0 and tts<='%s'", szTime);

	UINT32 delRows = 0;
	GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), szSQL, &delRows);
	if (delRows > 0)
		LogInfo(("清理掉%d条过期未读邮件!", delRows));
}

void EmailCleanMan::OnCleanReadEmails(LTMSEL ltNow)
{
	LTMSEL delTime = ltNow - ((LTMSEL)GetConfigFileMan()->GetMaxReadEmailSaveDays()) * 24 * 3600 * 1000;	//毫秒
	char szTime[32] = { 0 };
	SYSTEMTIME sysDelTime = ToTime(delTime);
	sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d", sysDelTime.wYear, sysDelTime.wMonth, sysDelTime.wDay, sysDelTime.wHour, sysDelTime.wMinute, sysDelTime.wSecond);
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "delete from email where state=1 and tts<='%s'", szTime);

	UINT32 delRows = 0;
	GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), szSQL, &delRows);
	if (delRows > 0)
		LogInfo(("清理掉%d条过期已读邮件!", delRows));
}

UINT32 EmailCleanMan::GetRoleUnReadEmails(UINT32 roleId)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select count(id) from email where actorid=%u and state=0", roleId);
	DBS_RESULT reslut = NULL;
	int ret = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, reslut);
	if (ret != DBS_OK)
		return 0;

	UINT32 nRows = DBS_NumRows(reslut);
	UINT16 nCols = DBS_NumRows(reslut);
	if (nRows <= 0 || nCols <= 0)
	{
		DBS_FreeResult(reslut);
		return 0;
	}

	DBS_ROW row = NULL;
	row = DBS_FetchRow(reslut);
	ASSERT(row != NULL);
	UINT32 emails = _atoi64(row[0]);
	DBS_FreeResult(reslut);
	return emails;
}

int EmailCleanMan::QueryEmailList(UINT32 roleId, UINT16 from, BYTE num, SQueryEmailListRes* pEmailList)
{
	pEmailList->retNum = 0;
	// 	UINT32 nTotalEmails = GetRoleUnReadEmails(roleId);
	// 	if(nTotalEmails <= 0)
	// 		return MLS_OK;
	if (num <= 0)
		return MLS_OK;

	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	//sprintf(szSQL, "select id, type, caption, sendernick, tts from email where actorid=%u and state=0 order by tts DESC limit %d, %d", roleId, from, num);
	sprintf(szSQL, "select id, type, caption, sendernick, tts, getattachment, state from email where actorid=%u limit %d, %d", roleId, from, num);
	DBS_RESULT reslut = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, reslut);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(reslut);
	UINT16 nCols = DBS_NumFields(reslut);
	if (nRows <= 0 || nCols < 7)
	{
		DBS_FreeResult(reslut);
		return retCode;
	}

	DBS_ROW row = NULL;
	BYTE maxRetNums = (MAX_RES_USER_BYTES - member_offset(SQueryEmailListRes, retNum, emails)) / sizeof(SEmailListGenInfo);
	while ((row = DBS_FetchRow(reslut)) != NULL)
	{
		pEmailList->emails[pEmailList->retNum].emailId = _atoi64(row[0]);
		pEmailList->emails[pEmailList->retNum].type = atoi(row[1]);
		strcpy(pEmailList->emails[pEmailList->retNum].caption, row[2]);
		strcpy(pEmailList->emails[pEmailList->retNum].senderNick, row[3]);
		strcpy(pEmailList->emails[pEmailList->retNum].sendTime, row[4]);
		pEmailList->emails[pEmailList->retNum].isGetAttachment = (BYTE)atoi(row[5]);
		pEmailList->emails[pEmailList->retNum].isRead = (BYTE)atoi(row[6]);
		pEmailList->emails[pEmailList->retNum].hton();

		if (++pEmailList->retNum >= maxRetNums)
			break;
	}
	DBS_FreeResult(reslut);
	return retCode;
}

int EmailCleanMan::ReadEmailContent(UINT32 emailId, SQueryEmailContentRes* pEmailContent, int& nBytes)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select gold, cash, exp, vit, attachment, body from email where id=%u", emailId);
	DBS_RESULT reslut = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, reslut);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(reslut);
	UINT16 nCols = DBS_NumFields(reslut);
	if (nRows <= 0 || nCols < 6)
	{
		DBS_FreeResult(reslut);
		return MLS_EMAIL_NOT_EXIST;
	}

	DBS_ROWLENGTH pRowLen = NULL;
	DBS_ROW row = DBS_FetchRow(reslut, &pRowLen);
	if (row == NULL || pRowLen == NULL)
	{
		DBS_FreeResult(reslut);
		nBytes = sizeof(SQueryEmailContentRes)-sizeof(char);
		return retCode;
	}

	//金币
	INT64 n64Gold = _atoi64(row[0]);
	U64ToBigNumber(n64Gold, pEmailContent->gold);

	//代币
	pEmailContent->rpcoin = (UINT32)_atoi64(row[1]);

	//经验
	INT64 n64Exp = _atoi64(row[2]);
	U64ToBigNumber(n64Exp, pEmailContent->exp);

	//体力
	pEmailContent->vit = (UINT32)_atoi64(row[3]);

	//物品
	int n = sizeof(SQueryEmailContentRes);
	if (pRowLen[4] > 0)
	{
		BYTE maxPropNums = (BYTE)((MAX_RES_USER_BYTES - member_offset(SQueryEmailContentRes, emailId, data)) / sizeof(SEmailPropGenInfo));
		BYTE* p = (BYTE*)row[4];
		BYTE nCount = (BYTE)((pRowLen[4] - sizeof(BYTE)) / sizeof(SPropertyAttr));
		nCount = min(maxPropNums, min(nCount, *p));
		pEmailContent->propNum = nCount;

		SEmailPropGenInfo* pPropGenInfo = (SEmailPropGenInfo*)pEmailContent->data;
		SPropertyAttr* pPropAttr = (SPropertyAttr*)(p + sizeof(BYTE));
		for (BYTE i = 0; i < nCount; ++i)
		{
			pPropGenInfo[i].propType = pPropAttr[i].type;
			pPropGenInfo[i].propId = pPropAttr[i].propId;
			pPropGenInfo[i].num = pPropAttr[i].num;
			pPropGenInfo[i].hton();
			n += sizeof(SEmailPropGenInfo);
		}
	}

	//文字内容
	int nBodyBytes = MAX_RES_USER_BYTES - n;
	if (nBodyBytes <= 0)
	{
		nBytes = n;
		return retCode;
	}

	nBodyBytes = min(nBodyBytes, pRowLen[5]);
	if (nBodyBytes > 0)
	{
		char* pszBody = (char*)pEmailContent + sizeof(SQueryEmailContentRes)-sizeof(char)+sizeof(SEmailPropGenInfo)* pEmailContent->propNum;
		memcpy(pszBody, row[5], nBodyBytes);
		n += nBodyBytes;
	}
	pEmailContent->textLen = nBodyBytes;
	nBytes = n;
	return retCode;
}

int EmailCleanMan::ReceiveEmailAttachments(UINT32 emailId, BYTE& emialType, SBigNumber& gold, UINT32& rpcoin, SBigNumber& exp, UINT32& vit,
	SEmailAttachmentsProp& hasPropType, SPropertyAttr* pAttachmentsProp, BYTE maxNum, BYTE& retNum)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select gold, cash, exp, vit, attachment, type from email where id=%u && getattachment=0", emailId);
	DBS_RESULT reslut = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, reslut);
	if (retCode != DBS_OK)
		return retCode;

	UINT32 nRows = DBS_NumRows(reslut);
	UINT16 nCols = DBS_NumFields(reslut);
	if (nRows <= 0 || nCols < 6)
	{
		DBS_FreeResult(reslut);
		return MLS_EMAIL_NOT_EXIST;
	}

	DBS_ROWLENGTH pRowLen = NULL;
	DBS_ROW row = DBS_FetchRow(reslut, &pRowLen);
	if (row == NULL || pRowLen == NULL)
	{
		DBS_FreeResult(reslut);
		return retCode;
	}

	//金币
	INT64 n64Gold = _atoi64(row[0]);
	U64ToBigNumber(n64Gold, gold);

	//代币
	rpcoin = (UINT32)_atoi64(row[1]);

	//经验
	INT64 n64Exp = _atoi64(row[2]);
	U64ToBigNumber(n64Exp, exp);

	//体力
	vit = (UINT32)_atoi64(row[3]);

	//物品
	if (pRowLen[4] > 0)
	{
		BYTE* p = (BYTE*)row[4];
		BYTE nCount = (BYTE)((pRowLen[4] - sizeof(BYTE)) / sizeof(SPropertyAttr));
		nCount = min(maxNum, min(nCount, *p));

		SPropertyAttr* pPropAttr = (SPropertyAttr*)(p + sizeof(BYTE));
		for (BYTE i = 0; i < nCount; ++i)
		{
			if (pPropAttr[i].type == ESGS_PROP_MAGIC)
				hasPropType.hasMagic = 1;
			else if (pPropAttr[i].type == ESGS_PROP_DRESS)
				hasPropType.hasDress = 1;
			else if (pPropAttr[i].type == ESGS_PROP_FRAGMENT)
				hasPropType.hasFragment = 1;
			else if (pPropAttr[i].type == ESGS_PROP_CHAIN_SOUL_FRAGMENT)
				hasPropType.hasChainSoulFragment = 1;
			else
				hasPropType.hasPackProp = 1;

			memcpy(&pAttachmentsProp[i], &pPropAttr[i], sizeof(SPropertyAttr));
		}
		retNum = nCount;
	}

	//邮件类型
	emialType = atoi(row[5]);
	return retCode;
}

int EmailCleanMan::SetEmailReadStatus(UINT32 emailId, BOOL isRead)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update email set state=%d where id=%u", isRead ? 1 : 0, emailId);
	return GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

int EmailCleanMan::SetEmailAttachmentsStatus(UINT32 emailId, BOOL isGet)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "update email set getattachment=%d where id=%u", isGet ? 1 : 0, emailId);
	return GetDBSClient()->Update(GetConfigFileMan()->GetDBName(), szSQL);
}

int EmailCleanMan::SyncSendEmail(const SSyncEmailSendContent& email)
{
	//当前时间
	char pszDate[32] = { 0 };
	GetLocalStrTime(pszDate);

	//发送者昵称
	char szUTF8SenderNick[MAX_UTF8_NICK_LEN] = { 0 };
	if (email.pGBSenderNick)
		gb2312_utf8(email.pGBSenderNick, (int)strlen(email.pGBSenderNick), szUTF8SenderNick, sizeof(szUTF8SenderNick)-sizeof(char));

	//邮件标题
	char szUTF8Caption[MAX_UTF8_EMAIL_CAPTION_LEN] = { 0 };
	if (email.pGBCaption)
		gb2312_utf8(email.pGBCaption, (int)strlen(email.pGBCaption), szUTF8Caption, sizeof(szUTF8Caption)-sizeof(char));

	//邮件正文
	char szUTF8Body[MAX_UTF8_EMAIL_BODY_LEN] = { 0 };
	if (email.pGBBody)
		gb2312_utf8(email.pGBBody, (int)strlen(email.pGBBody), szUTF8Body, sizeof(szUTF8Body)-sizeof(char));

	UINT64 gold = email.gold;		//金币
	UINT32 rpcoin = email.rpcoin;	//代币
	UINT64 exp = email.exp;			//经验
	UINT32 vit = email.vit;			//体力
	SPropertyAttr* pAttachments = email.pAttachments;	//物品
	BYTE attachmentNum = email.attachmentsNum;	//物品数量
	if (pAttachments == NULL || attachmentNum == 0)	//无物品	
	{
		BYTE n = 0;
		char pszAttachments[8] = { 0 };
		BYTE isGetAttachments = (gold > 0 || rpcoin > 0 || exp > 0 || vit > 0) ? 0 : 1;
		DBS_EscapeString(pszAttachments, (const char*)&n, sizeof(BYTE));
		char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
		sprintf(szSQL, "insert into email(actorid, type, senderid, sendernick, caption, body, gold, exp, vit, cash, attachment, tts, getattachment, state) "
			"values(%u, %d, %u, '%s', '%s', '%s', %llu, %llu, %u, %u, '%s', '%s', %d, %d)",
			email.receiverRoleId, email.type, email.senderId,
			szUTF8SenderNick, szUTF8Caption, szUTF8Body, email.gold, email.exp, email.vit, email.rpcoin,
			pszAttachments, pszDate, isGetAttachments, 0);

		GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);
	}
	else //有物品
	{
		BYTE offset = 0;
		char pszAttachments[8 * 1024] = { 0 };
		BYTE maxAttachmentNum = GetGameCfgFileMan()->GetMaxEmailAttachments();	//最大附件个数
		while (attachmentNum > 0)
		{
			SSyncEmailSendContent emailContent;
			emailContent.gold = gold;
			emailContent.rpcoin = rpcoin;
			emailContent.exp = exp;
			emailContent.vit = vit;
			emailContent.type = email.type;
			emailContent.receiverRoleId = email.receiverRoleId;
			emailContent.senderId = email.senderId;

			emailContent.attachmentsNum = min(maxAttachmentNum, attachmentNum);
			int n = DBS_EscapeString(pszAttachments, (const char*)&emailContent.attachmentsNum, sizeof(BYTE));
			if (emailContent.attachmentsNum > 0)
				DBS_EscapeString(pszAttachments + n, (const char*)(pAttachments + offset), emailContent.attachmentsNum * sizeof(SPropertyAttr));

			//写入数据库
			char szSQL[MAX_SQL_BUF_LEN] = { 0 };
			sprintf(szSQL, "insert into email(actorid, type, senderid, sendernick, caption, body, gold, exp, vit, cash, attachment, tts, getattachment, state) "
				"values(%u, %d, %u, '%s', '%s', '%s', %llu, %llu, %u, %u, '%s', '%s', %d, %d)",
				emailContent.receiverRoleId, emailContent.type, emailContent.senderId,
				szUTF8SenderNick, szUTF8Caption, szUTF8Body, emailContent.gold, emailContent.exp, emailContent.vit, emailContent.rpcoin,
				pszAttachments, pszDate, 0, 0);

			GetDBSClient()->Insert(GetConfigFileMan()->GetDBName(), szSQL, NULL);

			gold = 0;
			rpcoin = 0;
			exp = 0;
			vit = 0;
			attachmentNum = SGSU8Sub(attachmentNum, emailContent.attachmentsNum);
			offset = SGSU8Add(offset, emailContent.attachmentsNum);
		}
	}

	//新邮件通知
	NewEmailNotify(email.receiverUserId, email.receiverRoleId);
	return MLS_OK;
}

void EmailCleanMan::AsyncSendEmail(const SSyncEmailSendContent& email)
{
	BOOL enforce = false;
	SAsyncEmailSendContent emailItem;
	emailItem.senderId = email.senderId;	//发送者id
	emailItem.receiverRoleId = email.receiverRoleId;	//接收者角色id
	emailItem.receiverUserId = email.receiverUserId;	//接收者账号id
	emailItem.type = email.type;			//邮件类型
	emailItem.gold = email.gold;			//金币
	emailItem.rpcoin = email.rpcoin;		//代币
	emailItem.exp = email.exp;				//经验
	emailItem.vit = email.vit;				//体力
	for (BYTE i = 0; i < email.attachmentsNum; ++i)	//附件
		emailItem.attachmentsVec.push_back(email.pAttachments[i]);

	//发送者昵称
	if (email.pGBSenderNick)
		emailItem.strGBSenderNick = email.pGBSenderNick;

	//邮件标题
	if (email.pGBCaption)
		emailItem.strGBCaption = email.pGBCaption;

	//邮件正文
	if (email.pGBBody)
		emailItem.strGBBody = email.pGBBody;

	{
		CXTAutoLock lock(m_asyncEmailListLocker);
		m_asyncEmailList.push_back(emailItem);
		enforce = m_asyncEmailList.size() >= GetConfigFileMan()->GetMaxCacheItems() ? true : false;
	}

	//强制执行一次
	if (enforce)
		m_sleep.Interrupt();
}

int EmailCleanMan::DelEmails(UINT32 /*roleId*/, UINT32* pEmailId, BYTE idNum)
{
	if (pEmailId == NULL || idNum <= 0)
		return MLS_OK;

	char szSQL[MAX_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "delete from email where id=%u", pEmailId[0]);
	for (BYTE i = 1; i < idNum; ++i)
	{
		char szBuf[64] = { 0 };
		sprintf(szBuf, " or id=%u", pEmailId[i]);
		strcat(szSQL, szBuf);
	}

	return GetDBSClient()->Del(GetConfigFileMan()->GetDBName(), szSQL, NULL);
}

void EmailCleanMan::OnSendEmail()
{
	if (m_asyncEmailList.empty())
		return;

	std::list<SAsyncEmailSendContent> emailList;

	{
		CXTAutoLock lock(m_asyncEmailListLocker);
		emailList.assign(m_asyncEmailList.begin(), m_asyncEmailList.end());
		m_asyncEmailList.clear();
	}

	SSyncEmailSendContent email;
	std::list<SAsyncEmailSendContent>::iterator it = emailList.begin();
	for (; it != emailList.end(); ++it)
	{
		email.senderId = it->senderId;		//发送者id
		email.receiverRoleId = it->receiverRoleId;	//接收者角色id
		email.receiverUserId = it->receiverUserId;	//接收者账号id
		email.type = it->type;				//邮件类型
		email.pGBSenderNick = it->strGBSenderNick.c_str();	//发送者昵称
		email.pGBCaption = it->strGBCaption.c_str();		//标题
		email.pGBBody = it->strGBBody.c_str();				//正文
		email.gold = it->gold;		//金币
		email.rpcoin = it->rpcoin;	//代币
		email.exp = it->exp;		//经验
		email.vit = it->vit;		//体力
		email.attachmentsNum = (BYTE)it->attachmentsVec.size();	//附件个数
		email.pAttachments = email.attachmentsNum > 0 ? it->attachmentsVec.data() : NULL;	//附件数据
		SyncSendEmail(email);
	}
}

void EmailCleanMan::NewEmailNotify(UINT32 userId, UINT32 roleId)
{
	if (!GetRolesInfoMan()->IsUserRoleOnline(userId, roleId))
		return;

	BYTE linkerId = 0;
	int retCode = GetRolesInfoMan()->GetUserLinkerId(userId, linkerId);
	if (retCode == MLS_OK && linkerId > 0)
	{
		GWSClient* gwsSession = GetGWSClientMan()->RandGWSClient();
		if (gwsSession != NULL)
			gwsSession->SendBasicResponse(linkerId, userId, MLS_OK, MLS_NEW_EMAIL_NOTIFY);
	}
}

BOOL EmailCleanMan::IsHasInstDropEmail(UINT32 roleId)
{
	char szSQL[GENERIC_SQL_BUF_LEN] = { 0 };
	sprintf(szSQL, "select count(id) from email where actorid=%u and type=%d and getattachment=0", roleId, ESGS_EMAIL_INST_DROP);

	DBS_RESULT result = NULL;
	int retCode = GetDBSClient()->Query(GetConfigFileMan()->GetDBName(), szSQL, result);
	if (retCode != MLS_OK)
		return false;

	DBS_ROW row = DBS_FetchRow(result);
	if (row == NULL)
	{
		DBS_FreeResult(result);
		return false;
	}

	int n = atoi(row[0]);
	DBS_FreeResult(result);
	return n > 0 ? true : false;
}
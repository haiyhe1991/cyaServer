#include "UpdateDataMan.h"
#include "GMCfgMan.h"
#include "GMComm.h"
#include "ServeConnMan.h"
#include "cyaLog.h"
#include "AES.h"
#include "gb_utf8.h"

CUpdateDataMan::CUpdateDataMan()
{

}


CUpdateDataMan::~CUpdateDataMan()
{

}

int CUpdateDataMan::ExecuteNewQuerySql(UINT16 ChannelId, UINT16 partId, DBS_MSG_TYPE dsType, const char* db, const char* sql)
{
	std::map<UINT16, DBSHandle> handle;
	GetServeConnObj()->GetDBShandle(ChannelId, partId, handle);
	if (handle.size() == 0)
		return GM_ERROR;
	int ret = GM_OK;
	//CXTAutoLock locked(m_dbLocker);
	std::map<UINT16, DBSHandle>::iterator it = handle.begin();
	for (it; it != handle.end(); it++)
	{
		DBS_RESULT result = NULL;
		if (it->second == NULL)
			continue;
		ret = DBS_ExcuteSQL(it->second, dsType, db, sql, result, strlen(sql));
		if (result != NULL)
			m_dbsNewResult.insert(std::make_pair(it->first, result));
	}
	return ret;
}

int CUpdateDataMan::ExecuteNewRechargeDbQuerySql(DBS_MSG_TYPE dsType, const char* db, const char* sql, UINT32* count)
{
	DBS_RESULT result = NULL;
	DBSHandle handle = NULL;
	GetServeConnObj()->GetRechargeDBShandle(handle);
	if (handle == NULL)
		return 1;
	int ret = DBS_ExcuteSQL(handle, dsType, db, sql, result, strlen(sql));
	if (result == NULL)
		*count += 0;
	else
	{
		DBS_ROW row = DBS_FetchRow(result);
		if (row == NULL)
			return ret;
		*count += atof(row[0]);
		DBS_FreeResult(result);
	}
	return ret;
}

int CUpdateDataMan::ExecuteNewAccountDbQuerySql(DBS_MSG_TYPE dsType, const char* db, const char* sql, UINT32* count)
{
	DBS_RESULT result = NULL;
	DBSHandle handle = NULL;
	GetServeConnObj()->GetAccountDBShandle(handle);
	if (handle == NULL)
		return 1;
	int ret = DBS_ExcuteSQL(handle, dsType, db, sql, result, strlen(sql));
	if (result == NULL)
		*count += 0;
	else
	{
		DBS_ROW row = DBS_FetchRow(result);
		if (row == NULL)
			return ret;
		*count += atof(row[0]);
		DBS_FreeResult(result);
	}
	return ret;
}

int CUpdateDataMan::ExecuteNewRechargeDbQuerySql(DBS_MSG_TYPE dsType, const char* db, const char* sql, DBS_RESULT& result)
{
	DBSHandle handle = NULL;
	GetServeConnObj()->GetRechargeDBShandle(handle);
	if (handle == NULL)
		return 1;
	int ret = DBS_ExcuteSQL(handle, dsType, db, sql, result, strlen(sql));
	return ret;
}

int CUpdateDataMan::ExecuteNewAccountDbQuerySql(DBS_MSG_TYPE dsType, const char* db, const char* sql, DBS_RESULT& result)
{
	DBSHandle handle = NULL;
	GetServeConnObj()->GetAccountDBShandle(handle);
	if (handle == NULL)
		return 1;
	int ret = DBS_ExcuteSQL(handle, dsType, db, sql, result, strlen(sql));
	return ret;
}

int CUpdateDataMan::ExecuteNewRechargeQuerySql(DBSHandle handle, DBS_MSG_TYPE dsType, const char* db, const char* sql)
{
	DBS_RESULT result = NULL;
	if (handle == NULL)
		return 1;
	int ret = DBS_ExcuteSQL(handle, dsType, db, sql, result, strlen(sql));
	if (result != NULL)
		DBS_FreeResult(result);
	return ret;
}

int CUpdateDataMan::ExecuteNewRechargeQuerySql(DBSHandle handle, DBS_MSG_TYPE dsType, const char* db, const char* sql, UINT32* count)
{
	DBS_RESULT result = NULL;
	if (handle == NULL)
		return 1;
	DBS_ExcuteSQL(handle, dsType, db, sql, result, strlen(sql));
	if (result == NULL)
		*count += 0;
	else
	{
		DBS_ROW row = DBS_FetchRow(result);
		if (row == NULL)
			return 0;
		*count += atof(row[0]);
		DBS_FreeResult(result);
	}
	return 0;
}

int CUpdateDataMan::ExecuteNewQuerySql(UINT16 ChannelId, UINT16 partId, DBS_MSG_TYPE dsType, const char* db, const char* sql, std::map<UINT16, float>& getCount)
{
	std::map<UINT16, DBSHandle> handle;
	GetServeConnObj()->GetDBShandle(ChannelId, partId, handle);
	if (handle.size() <= 0)
		return GM_ERROR;

	//CXTAutoLock locked(m_dbLocker);
	std::map<UINT16, DBSHandle>::iterator it = handle.begin();
	for (it; it != handle.end(); it++)
	{
		float count = 0;
		DBS_RESULT result;
		if (it->second == NULL)
			continue;
		DBS_ExcuteSQL(it->second, dsType, db, sql, result, strlen(sql));
		if (result == NULL)
			count += 0;
		else
		{

			DBS_ROW row = DBS_FetchRow(result);
			if (row == NULL)
				return 0;
			count += atof(row[0]);
			DBS_FreeResult(result);
		}
		getCount.insert(std::make_pair(it->first, count));
	}
	handle.clear();
	return GM_OK;
}

int CUpdateDataMan::ExecuteNewQuerySql(UINT16 ChannelId, UINT16 partId, DBS_MSG_TYPE dsType, const char* db, const char* sql, std::map<UINT16, UINT32>& getCount, BYTE isAffect)
{
	std::map<UINT16, DBSHandle> handle;
	int ret = GM_OK;
	GetServeConnObj()->GetDBShandle(ChannelId, partId, handle);
	if (handle.size() <= 0)
		return GM_ERROR;

	//CXTAutoLock locked(m_dbLocker);
	std::map<UINT16, DBSHandle>::iterator it = handle.begin();
	for (it; it != handle.end(); it++)
	{
		UINT32 count = 0;
		DBS_RESULT result = NULL;
		if (it->second == NULL)
			continue;
		ret = DBS_ExcuteSQL(it->second, dsType, db, sql, result, strlen(sql));
		if (result == NULL)
			count += 0;
		else
		{
			DBS_ROW row = DBS_FetchRow(result);
			if (row == NULL)
				return 0;
			count += atof(row[0]);
			DBS_FreeResult(result);
		}
		getCount.insert(std::make_pair(it->first, count));
	}
	handle.clear();
	return ret;
}

void CUpdateDataMan::ClearNewDBSResult()
{
	std::map<UINT16, DBS_RESULT>::iterator it = m_dbsNewResult.begin();
	for (it; it != m_dbsNewResult.end();)
	{
		DBS_RESULT result = it->second;
		if (result != NULL)
			DBS_FreeResult(result);
		m_dbsNewResult.erase(it++);
		if (m_dbsNewResult.size() <= 0)
			break;
	}
}

void  CUpdateDataMan::DateMan(UINT16 ChannelId, const char* registTime, BYTE day, char lkTime[20])
{
	char gTime[20] = { 0 };
	char sql[1024] = { 0 };
	memset(lkTime, 0, 20);

	GetDate(registTime, gTime, FALSE);
	if (day == 90)
		sprintf(sql, "select date_format(date_add('%s',interval 1 month), '%sY-%sm')", gTime, "%", "%");
	else
		sprintf(sql, "select date_add('%s',interval %u day)", gTime, day);
	ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_QUERY, DB_SGSVERIFY, sql);
	if (m_dbsNewResult.size() <= 0)
		return;

	std::map<UINT16, DBS_RESULT>::iterator it = m_dbsNewResult.begin();
	DBS_RESULT result = it->second;
	if (result == NULL)
		return;
	DBS_ROW row = NULL;
	while ((row = DBS_FetchRow(result)) != NULL)
		strcpy(lkTime, row[0]);

	ClearNewDBSResult();
}

void CUpdateDataMan::GetPlatRegistCount(UINT16 platid, UINT16 ChannelId, BYTE lktType, const char* regTime, std::map<UINT16/*����ID*/, UINT32/*ע����*/>& reg)
{
	char sql[1024 * 2] = { 0 };
	std::vector<UINT16/*����ID*/> partIdVec;
	GetCfgManObj()->GetPlatAllPartId(ChannelId, partIdVec);
	for (int n = 0; n < partIdVec.size(); n++)
	{
		sprintf(sql, "select ");
		//UINT32 count = 0;
		for (int i = 1; i < 6; i++)
		{
			//ע��������
			if (lktType == 90)
			{
				if (i == 5)	//1��12�޸����platsource =platid;
					sprintf(sql, "%s (select count(*) from player_account_%u where date_format(regtts, '%sY-%sm')= '%s' and partid=%d and platsource=%d and ver=%d )", sql, i, "%", "%", regTime, partIdVec[n], platid, ChannelId);
				else
					sprintf(sql, "%s (select count(*) from player_account_%u where date_format(regtts, '%sY-%sm')= '%s'  and partid=%d and platsource=%d and ver=%d ) + ", sql, i, "%", "%", regTime, partIdVec[n], platid, ChannelId);
			}
			else
			{
				if (i == 5)
					sprintf(sql, "%s (select count(*) from player_account_%u where date(regtts)= date('%s')  and partid=%d and platsource=%d and ver=%d )", sql, i, regTime, partIdVec[n], platid, ChannelId);
				else
					sprintf(sql, "%s (select count(*) from player_account_%u where date(regtts)= date('%s')  and partid=%d and platsource=%d and ver=%d) + ", sql, i, regTime, partIdVec[n], platid, ChannelId);
			}
		}
		ExecuteNewQuerySql(ChannelId, partIdVec[n], DBS_MSG_QUERY, DB_SGSVERIFY, sql, reg, 0);
		//ExecuteNewAccountDbQuerySql(DBS_MSG_QUERY, DB_SGSVERIFY, sql, &count);
		//memset(sql, 0, 1024*2);
		//reg.insert(std::make_pair(partIdVec[n], count));
	}

}

void CUpdateDataMan::GetPlatLoginCount(UINT16 platId, UINT16 ChannelId, BYTE lktType, const char* regTime, const char* lgTime, std::map<UINT16/*����ID*/, UINT32/*��¼��*/>& lg)
{
	char sql[1024 * 2] = { 0 };
	std::vector<UINT16/*����ID*/> partIdVec;
	GetCfgManObj()->GetPlatAllPartId(ChannelId, partIdVec);
	for (int n = 0; n < partIdVec.size(); n++)
	{
		///��ע�������ڴ�X�պ��¼��
		sprintf(sql, "select ");
		//UINT32 count = 0;
		for (int i = 1; i < 6; i++)
		{
			if (lktType == 90)
			{
				if (i == 5)
					sprintf(sql, "%s (select count(*) from player_account_%u where (date_format(lasttts, '%sY-%sm') \
								 								 = date_format('%s', '%sY-%sm')) and (date_format(regtts, '%sY-%sm') = '%s') \
																 								 and (partid=%d) and (platsource=%d) and (ver=%d) ) ", sql, i, "%", "%", lgTime, "%", "%", "%", "%", regTime, partIdVec[n], platId, ChannelId);
				else
					sprintf(sql, "%s (select count(*) from player_account_%u where (date_format(lasttts, '%sY-%sm') \
								 								 = date_format('%s', '%sY-%sm')) and (date_format(regtts, '%sY-%sm') = '%s') and \
																 								  (partid=%d) and (platsource=%d) and (ver=%d)) + ", sql, i, "%", "%", lgTime, "%", "%", "%", "%", regTime, partIdVec[n], platId, ChannelId);
			}
			else
			{
				if (i == 5)
					sprintf(sql, "%s (select count(*) from player_account_%u where (date(lasttts) = date('%s')) and \
								 								 (date(regtts)=date('%s'))  and (partid=%d)  and (platsource=%d) and (ver=%d) )", sql, i, lgTime, regTime, partIdVec[n], platId, ChannelId);
				else
					sprintf(sql, "%s (select count(*) from player_account_%u where (date(lasttts) = date('%s')) and \
								 								 (date(regtts)=date('%s'))  and (partid=%d) and (platsource=%d) and (ver=%d) ) + ", sql, i, lgTime, regTime, partIdVec[n], platId, ChannelId);
			}

		}

		ExecuteNewQuerySql(ChannelId, partIdVec[n], DBS_MSG_QUERY, DB_SGSVERIFY, sql, lg, 0);
		/*ExecuteNewAccountDbQuerySql(DBS_MSG_QUERY, DB_SGSVERIFY, sql, &count);
		memset(sql, 0, 1024*2);
		lg.insert(std::make_pair(partIdVec[n], count));*/
	}
}

void CUpdateDataMan::GetTodayLoginCount(UINT16 platId, UINT16 ChannelId, const char* updateTime, std::map<UINT16/*����ID*/, UINT32/*���յ�¼��*/>& todayLg)
{
	char sql[1024 * 2] = { 0 };
	std::vector<UINT16/*����ID*/> partIdVec;
	GetCfgManObj()->GetPlatAllPartId(ChannelId, partIdVec);
	for (int n = 0; n < partIdVec.size(); n++)
	{
		sprintf(sql, "select ");
		//UINT32 count = 0;
		///ÿ�յ�¼������
		for (int i = 1; i < 6; i++)
		{
			if (i == 5)
				sprintf(sql, "%s (select count(*) from player_account_%u where date(lasttts) = date('%s')  and (partid=%d) and (platsource=%d) and (ver=%d) )", sql, i, updateTime, partIdVec[n], platId, ChannelId);
			else
				sprintf(sql, "%s (select count(*) from player_account_%u where date(lasttts) = date('%s')  and (partid=%d) and (platsource=%d) and (ver=%d) + ", sql, i, updateTime, partIdVec[n], platId, ChannelId);
		}
		ExecuteNewQuerySql(ChannelId, partIdVec[n], DBS_MSG_QUERY, DB_SGSVERIFY, sql, todayLg, 0);
		/*ExecuteNewAccountDbQuerySql(DBS_MSG_QUERY, DB_SGSVERIFY, sql, &count);
		memset(sql, 0, 1024*2);
		todayLg.insert(std::make_pair(partIdVec[n], count));*/
	}
}

void CUpdateDataMan::GetPlatLoginDeviceCount(UINT16 ChannelId, BYTE lktType, const char* regTime, const char* lgTime, std::map<UINT16/*����ID*/, UINT32/*��¼�豸��*/>& lgDevice)
{
	char sql[1024 * 2] = { 0 };
	std::vector<UINT16/*����ID*/> partIdVec;
	GetCfgManObj()->GetPlatAllPartId(ChannelId, partIdVec);
	for (int n = 0; n < partIdVec.size(); n++)
	{
		sprintf(sql, "select ");
		//UINT32 count = 0;
		for (int i = 1; i < 6; i++)
		{
			///��¼���ݴ���
			if (lktType == 90)
			{
				if (i == 5)
					sprintf(sql, "%s (select count(*) from player_account_%u where (date_format(lasttts, '%sY-%sm') = \
								 							 date_format('%s', '%sY-%sm')) and (date_format(regtts, '%sY-%sm') = '%s') and \
															 							 (strcmp(regdev, lastdev) = 0)  and (partid=%d)) ", sql, i,
																						 "%", "%", lgTime, "%", "%", "%", "%", regTime, partIdVec[n]);
				else
					sprintf(sql, "%s (select count(*) from player_account_%u where (date_format(lasttts, '%sY-%sm') = \
								 								 date_format('%s', '%sY-%sm')) and (date_format(regtts, '%sY-%sm') = '%s') and \
																 								 (strcmp(regdev, lastdev) = 0)  and (partid=%d)) + ", sql, i, "%",
																								 "%", lgTime, "%", "%", "%", "%", regTime, partIdVec[n]);
			}
			else
			{
				if (i == 5)
					sprintf(sql, "%s (select count(*) from player_account_%u where (date(lasttts) = '%s') and \
								 								 (date(regtts)=date('%s')) and (strcmp(regdev, lastdev)=0)  and \
																 								 (partid=%d))", sql, i, lgTime, regTime, partIdVec[n]);
				else
					sprintf(sql, "%s (select count(*) from player_account_%u where (date(lasttts) = '%s') and \
								 								 (date(regtts)=date('%s')) and (strcmp(regdev, lastdev)=0)  and \
																 								 (partid=%d)) + ", sql, i, lgTime, regTime, partIdVec[n]);
			}
		}
		ExecuteNewQuerySql(ChannelId, partIdVec[n], DBS_MSG_QUERY, DB_SGSVERIFY, sql, lgDevice, 0);
		/*ExecuteNewAccountDbQuerySql(DBS_MSG_QUERY, DB_SGSVERIFY, sql, &count);
		memset(sql, 0, 1024*2);
		lgDevice.insert(std::make_pair(partIdVec[n], count));*/
	}
}

void CUpdateDataMan::GetTodayLoginDeviceCount(UINT16 ChannelId, const char* updateTime, std::map<UINT16/*����ID*/, UINT32/*���յ�¼�豸��*/>& todayLgDevice)
{
	char sql[1024 * 2] = { 0 };
	std::vector<UINT16/*����ID*/> partIdVec;
	GetCfgManObj()->GetPlatAllPartId(ChannelId, partIdVec);
	for (int n = 0; n < partIdVec.size(); n++)
	{
		sprintf(sql, "select ");
		//UINT32 count = 0;
		for (int i = 1; i < 6; i++)
		{
			if (i == 5)
				sprintf(sql, "%s (select count(*) from player_account_%u where date(lasttts) = date('%s') \
							 							 and (strcmp(regdev, lastdev)=0)  and (partid=%d))", sql, i, updateTime, partIdVec[n]);
			else
				sprintf(sql, "%s (select count(*) from player_account_%u where date(lasttts) = date('%s') \
							 							 and (strcmp(regdev, lastdev)=0)  and (partid=%d)) + ", sql, i, updateTime, partIdVec[n]);
			//ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_QUERY, DB_SGSVERIFY, sql, todayLgDevice, 0);
		}
		ExecuteNewQuerySql(ChannelId, partIdVec[n], DBS_MSG_QUERY, DB_SGSVERIFY, sql, todayLgDevice, 0);
		/*ExecuteNewAccountDbQuerySql(DBS_MSG_QUERY, DB_SGSVERIFY, sql, &count);
		memset(sql, 0, 1024*2);
		todayLgDevice.insert(std::make_pair(partIdVec[n], count));*/
	}
}

///ƽ̨������ɫ����������
void CUpdateDataMan::GetPlatCreateActorCount(UINT16 ChannelId, BYTE lktType, const char* regTime, std::map<UINT16/*����ID*/, UINT32/*������ɫ��*/>& createActor)
{
	char sql[1024] = { 0 };

	if (lktType == 90)
		sprintf(sql, "select count(*) from actor where date_format(createtts, '%sY-%sm')= '%s'", "%", "%", regTime);
	else
		sprintf(sql, "select count(*) from actor where date(createtts)= date('%s')", regTime);
	ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_QUERY, DB_SGS, sql, createActor, 0);
}

void CUpdateDataMan::GetPlatLoginActorCount(UINT16 ChannelId, BYTE lktType, const char* regTime, const char* lgTime, std::map<UINT16/*����ID*/, UINT32/*��¼��ɫ��*/>& lgActor)
{
	char sql[1024] = { 0 };
	if (lktType == 90)
		sprintf(sql, "select count(*) from actor where (date_format(lvtts, '%sY-%sm')= date_format('%s', '%sY-%sm')) and \
					 					 (date_format(createtts, '%sY-%sm')='%s')", "%", "%", lgTime, "%", "%", "%", "%", regTime);
	else
		sprintf(sql, "select count(*) from actor where (date(lvtts)= date('%s')) and (date(createtts)=date('%s'))", lgTime, regTime);
	ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_QUERY, DB_SGS, sql, lgActor, 0);
}

void CUpdateDataMan::GetTodayLoginActorCount(UINT16 ChannelId, const char* updateTime, std::map<UINT16/*����ID*/, UINT32/*���յ�¼��ɫ��*/>& todayLgActor)
{
	char sql[1024] = { 0 };
	sprintf(sql, "select count(*) from actor where date(lvtts) = date('%s')", updateTime);
	ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_QUERY, DB_SGS, sql, todayLgActor, 0);
}

int CUpdateDataMan::OnlineDataMan(SOCKET sockt, const char* key, UINT32* count)
{
	char sReadbuf[1024] = { 0 };
	char sHead[65] = { 0 };
	int ret = 0;

	UINT16 unCryptLen = GET_SGSPAYLOAD_REQ_SIZE;
	UINT16 cryptLen = BytesAlign(unCryptLen);
	int sLen = cryptLen + sizeof(SGSProtocolHead);
	BYTE* buf = (BYTE*)DataAlloc(sLen);
	ASSERT(buf != NULL);
	memset(buf, 0, sLen);

	///��Linker���ͻ�ȡ������������
	SGSProtocolHead* phead = (SGSProtocolHead*)buf;
	SGS_PROTO_HEAD_DATA_MAN__(phead, cryptLen, DATA_PACKET, 1);
	SGSReqPayload* ppayload = (SGSReqPayload*)(buf + GET_SGS_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_REQ_DATA_MAN__(ppayload, 0, 0, LINKER_GET_ONLINE_CONNECT_NUM);
	appEncryptDataWithKey((AES_BYTE*)buf + GET_SGS_PROTOHEAD_SIZE, unCryptLen, (ANSICHAR*)key);
	ret = SockWriteAllByTime(sockt, buf, sLen, 3000);
	if (ret <0)
	{
		if (buf != NULL)
			FreeAlloc(buf);
		return GM_WRITE_LINKER_DATA_TIMEOUT;
	}
	FreeAlloc(buf);

	///����Linker ��������
	phead = NULL;
	if (SockCheckRead(sockt, 5000))
	{
		ret = SockReadAllByTime(sockt, sHead, GET_SGS_PROTOHEAD_SIZE, 3000);
		phead = (SGSProtocolHead*)sHead;
		if (ret <0)
			return GM_READ_LINKER_DATA_TIMEOUT;
		ASSERT(phead != NULL);
		phead->ntoh();
		ret = SockReadAllByTime(sockt, sReadbuf, phead->pduLen, 3000);
		if (ret <0)
			return GM_READ_LINKER_DATA_TIMEOUT;
	}
	if (phead == NULL)
		return GM_READ_LINKER_DATA_TIMEOUT;

	///����Linker��������
	if (phead->isCrypt)
		appDecryptDataWithKey((AES_BYTE*)sReadbuf, phead->pduLen, (ANSICHAR*)key);
	SGSResPayload* pres = (SGSResPayload*)sReadbuf;
	ASSERT(pres != NULL);
	pres->ntoh();
	*count = pres->token;
	return GM_OK;
}

void CUpdateDataMan::RechargeDataMan(UINT16 ChannelId, UINT16 partId, BYTE operType, const char* updateTime, UINT32* count)
{
	*count = 0;
	char sql[1024 * 2] = { 0 };
	sprintf(sql, "select ");
	for (int i = 1; i < 6; i++)
	{
		switch (operType)
		{
		case PY_CASH_ACTOR:
			if (i == 5)
				sprintf(sql, "%s (select count(distinct actorid) from player_cash_%u where (date(confirmtts)=date('%s')) and \
							 					(status = 1)  and (partid=%d)) ", sql, i, updateTime, partId);
			else
				sprintf(sql, "%s (select count(distinct actorid) from player_cash_%u where date(confirmtts)=date('%s') and \
							 							 (status = 1)  and (partid=%d)) + ", sql, i, updateTime, partId);
			break;
		case PY_CASH_COUNT:
			if (i == 5)
				sprintf(sql, "%s (select count(*) from player_cash_%u where date(confirmtts)= date('%s') and \
							 							 (status = 1)  and (partid=%d))", sql, i, updateTime, partId);
			else
				sprintf(sql, "%s (select count(*) from player_cash_%u where date(confirmtts)= date('%s') and \
							 							 (status = 1)  and (partid=%d)) + ", sql, i, updateTime, partId);
			break;
		case PY_CASH_TOTAL:
			if (i == 5)
				sprintf(sql, "%s (select count(distinct accountid) from player_cash_%u where date(confirmtts) <= date('%s') and \
							 							 (status = 1)  and (partid=%d))", sql, i, updateTime, partId);
			else
				sprintf(sql, "%s (select count(distinct accountid) from player_cash_%u where date(confirmtts) <= date('%s') and \
							 							 (status = 1)  and (partid=%d)) + ", sql, i, updateTime, partId);
			break;
		case PY_CASH_NRMB:
			if (i == 5)
				sprintf(sql, "%s (select ifnull(sum(rmb),0) from player_cash_%u where date(confirmtts) = date('%s') and \
							 							 (status = 1)  and (partid=%d))", sql, i, updateTime, partId);
			else
				sprintf(sql, "%s (select ifnull(sum(rmb),0) from player_cash_%u where date(confirmtts) = date('%s') and \
							 							 (status = 1)  and (partid=%d)) + ", sql, i, updateTime, partId);
			break;
		case PY_CASH_CRMB:
			if (i == 5)
				sprintf(sql, "%s (select ifnull(sum(rmb),0) from player_cash_%u where date(confirmtts) = date('%s') and \
							 							 (status= 1)  and (partid=%d))", sql, i, updateTime, partId);
			else
				sprintf(sql, "%s (select ifnull(sum(rmb),0) from player_cash_%u where date(confirmtts) = date('%s') and \
							 							 (status= 1)  and (partid=%d))+ ", sql, i, updateTime, partId);
			break;
		case PY_CASH_TORMB:
			if (i == 5)
				sprintf(sql, "%s (select ifnull(sum(rmb),0) from player_cash_%u where date(confirmtts) <= date('%s') and \
							 							 (status = 1)  and (partid=%d))", sql, i, updateTime, partId);
			else
				sprintf(sql, "%s (select ifnull(sum(rmb),0) from player_cash_%u where date(confirmtts) <= date('%s') and \
							 							 (status = 1)  and (partid=%d)) + ", sql, i, updateTime, partId);
			break;
		default:
			break;
		}

	}
	ExecuteNewRechargeDbQuerySql(DBS_MSG_QUERY, DB_SGSRECHARGE, sql, count);
	memset(sql, 0, 1024 * 2);
}

void CUpdateDataMan::FirstRechargeMan(UINT16 ChannelId, UINT16 partId, DBSHandle handle, const char* updateTime, UINT32* count)
{
	*count = 0;
	char sql[1024] = { 0 };
	std::map<UINT16, UINT32> extRes;
	sprintf(sql, "select count(distinct actorid) from recharge where date(confirmtts) = date('%s') and status=1", updateTime);
	ExecuteNewQuerySql(ChannelId, partId, DBS_MSG_QUERY, DB_SGS, sql, extRes, 0);
	std::map<UINT16, UINT32>::iterator it = extRes.find(partId);
	if (it != extRes.end())
		*count = it->second;
}

int CUpdateDataMan::AccountLeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* registTime, const char* updateTime, BYTE day, BYTE lktType)
{

	char lkTime[20] = { 0 }; //����ʱ��
	int ret = GM_OK;

	std::map<UINT16/*����ID*/, UINT32/*ע����*/> partRegCount; //������ע����
	std::map<UINT16/*����ID*/, UINT32/*��¼��*/> partLgCount; //��������¼��
	std::map<UINT16/*����ID*/, UINT32/*��¼��*/> todayLgCount; //���ո�������¼��

	DateMan(ChannelId, registTime, day, lkTime);
	GetPlatRegistCount(platid, ChannelId, lktType, registTime, partRegCount);
	GetPlatLoginCount(platid, ChannelId, lktType, registTime, lkTime, partLgCount);
	GetTodayLoginCount(platid, ChannelId, updateTime, todayLgCount);

	if (partRegCount.size() == 0)
		return GM_OK;

	std::map<UINT16, UINT32>::iterator it1 = partRegCount.begin();
	std::map<UINT16, UINT32>::iterator it2 = partLgCount.begin();
	std::map<UINT16, UINT32>::iterator it3 = todayLgCount.begin();
	for (it1; it1 != partRegCount.end();)
	{
		float lkRate = 0.0;
		//����������ʺ�����
		if (it1->second != 0)
			lkRate = (float)it2->second / (float)it1->second;
		else
			lkRate;
		if (GetCfgManObj()->IsWriteIntoDb())
			return StoreLeaveKeepIntoDb(platid, ChannelId, it1->first, ACCOUNT_LK, registTime, updateTime, it1->second, it2->second, it3->second, lkRate, lktType);
		else
			LogInfo(("�����ʺ�����ʱ�䣺[%s], ������:[%d], ��¼��:[%d], ����:[%.4f], ��������: [%u], ˢ��ʱ��:[%s]",
			registTime, it1->second, it2->second, lkRate * 100, lktType, updateTime));
		it1++; it2++; it3++;
	}
	return ret;
}

///�豸���洦��
int CUpdateDataMan::DeviceLeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* registTime, const char* updateTime, BYTE day, BYTE lktType)
{
	char lkTime[20] = { 0 }; //�豸����ʱ��
	int ret = GM_OK;

	std::map<UINT16/*����ID*/, UINT32/*ע���豸��*/> partRegCount; //������ע���豸��
	std::map<UINT16/*����ID*/, UINT32/*��¼�豸��*/> partLgCount; //��������¼�豸��
	std::map<UINT16/*����ID*/, UINT32/*���յ�¼�豸��*/> todayLgCount; //���ո�������¼�豸��

	DateMan(ChannelId, registTime, day, lkTime);
	GetPlatRegistCount(platid, ChannelId, lktType, registTime, partRegCount);
	GetPlatLoginDeviceCount(ChannelId, lktType, registTime, lkTime, partLgCount);
	GetTodayLoginDeviceCount(ChannelId, updateTime, todayLgCount);

	if (partRegCount.size() == 0)
		return GM_OK;

	std::map<UINT16, UINT32>::iterator it1 = partRegCount.begin();
	std::map<UINT16, UINT32>::iterator it2 = partLgCount.begin();
	std::map<UINT16, UINT32>::iterator it3 = todayLgCount.begin();
	for (it1; it1 != partRegCount.end();)
	{
		//����������豸����
		float lkRate = 0.0;
		if (it1->second != 0)
			lkRate = (float)it2->second / (float)it1->second;

		if (GetCfgManObj()->IsWriteIntoDb())
			return StoreLeaveKeepIntoDb(platid, ChannelId, it1->first, DEVICE_LK, registTime, updateTime, it1->second, it2->second, it3->second, lkRate, lktType);

		else
			LogInfo(("�����ʺ�����ʱ�䣺[%s], ������:[%d], ��¼��:[%d], ����:[%.4f], ��������: [%u], ˢ��ʱ��:[%s]",
			registTime, it1->second, it2->second, lkRate * 100, lktType, updateTime));
		it1++; it2++; it3++;
	}
	return ret;
}

int CUpdateDataMan::ActorLeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* registTime, const char* updateTime, BYTE day, BYTE lktType)
{
	char lkTime[20] = { 0 }; //��ɫ����ʱ��
	int ret = GM_OK;

	std::map<UINT16/*����ID*/, UINT32/*������ɫ��*/> partcreateCount; //��������ɫ������
	std::map<UINT16/*����ID*/, UINT32/*��¼��ɫ��*/> partLgCount; //��������¼��ɫ��
	std::map<UINT16/*����ID*/, UINT32/*���յ�¼��ɫ��*/> todayLgCount; //���ո�������¼��ɫ��

	DateMan(ChannelId, registTime, day, lkTime);
	GetPlatCreateActorCount(ChannelId, lktType, registTime, partcreateCount);
	GetPlatLoginActorCount(ChannelId, lktType, registTime, lkTime, partLgCount);
	GetTodayLoginActorCount(ChannelId, updateTime, todayLgCount);

	if (partcreateCount.size() == 0)
		return GM_OK;

	std::map<UINT16, UINT32>::iterator it1 = partcreateCount.begin();
	std::map<UINT16, UINT32>::iterator it2 = partLgCount.begin();
	std::map<UINT16, UINT32>::iterator it3 = todayLgCount.begin();
	for (it1; it1 != partcreateCount.end();)
	{
		//�����������ɫ����
		float lkRate = 0.0;
		if (it1->second != 0)
			lkRate = (float)it2->second / (float)it1->second;
		if (GetCfgManObj()->IsWriteIntoDb())
			return StoreLeaveKeepIntoDb(platid, ChannelId, it1->first, ACTOR_LK, registTime, updateTime, it1->second, it2->second, it3->second, lkRate, lktType);

		else
			LogInfo(("������ɫ����ʱ�䣺[%s], ������:[%d], ��¼��:[%d], ����:[%.4f], ��������: [%u], ˢ��ʱ��:[%s]",
			registTime, it1->second, it2->second, lkRate * 100, lktType, updateTime));
		it1++; it2++; it3++;
	}
	return ret;
}

int CUpdateDataMan::StoreLeaveKeepIntoDb(UINT16 platid, UINT16 ChannelId, UINT16 partId, BYTE lkType, const char* stime, const char* updateTime, UINT32 rgCount, UINT32 lgCount, UINT32 currLgCount, float lkRate, BYTE lktType)
{
	char sql[1024] = { 0 };
	std::map<UINT16, UINT32> extResult;

	if (lkType == 1) //�ʺ�����
		sprintf(sql, "insert into account_leavekeep(platformid, partid, regcount, lgcount, rate, tts, type, uptts, uplgcount,ver) \
					 					 values(%d, %d, %d, %d, %.4f, '%s', %u, '%s', %d, %d)", platid, partId, rgCount, lgCount, lkRate, stime, lktType, updateTime, currLgCount, ChannelId);
	else if (lkType == 2) //�豸����
		sprintf(sql, "insert into device_leavekeep(platformid, partid, regcount, lgcount, rate, tts, type, uptts, uplgcount,ver) \
					 					 values(%d, %d, %d, %d, %.4f, '%s', %u, '%s', %d, %d)", platid, partId, rgCount, lgCount, lkRate, stime, lktType, updateTime, currLgCount, ChannelId);
	else if (lkType == 3) //��ɫ����
		sprintf(sql, "insert into actor_leavekeep(platformid, partid, regcount, lgcount, rate, tts, type, uptts, uplgcount,ver) \
					 					 values(%d, %d, %d, %d, %.4f, '%s', %u, '%s', %d, %d)", platid, partId, rgCount, lgCount, lkRate, stime, lktType, updateTime, currLgCount, ChannelId);
	else
		return GM_UNKNOWN_LKTYPE;

	int ret = ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_INSERT, DB_SGSINFO, sql, extResult, 1);
	if (ret <= 0 || extResult.size() <= 0)
		return GM_LK_STORE_DB_FALIED;
	return ret;
}


void CUpdateDataMan::StoreAddupNewIntoDb(UINT16 platid, UINT16 ChannelId, UINT16 partId, const char* stime, UINT32 count, BYTE addType, BYTE dataType)
{
	char sql[1024] = { 0 };
	std::map<UINT16, UINT32> extResult;

	if (addType == 1)
		sprintf(sql, "insert into regist_account(platformid, partid, regtts, count, type, ver ) values(%d, %d, '%s', %d, %u, %d)", platid, partId, stime, count, dataType, ChannelId);
	else if (addType == 2)
		sprintf(sql, "insert into active_device(platformid, partid, activetts, count, type,ver) values(%d, %d, '%s', %d, %u,%d)", platid, partId, stime, count, dataType, ChannelId);
	else if (addType == 3)
		sprintf(sql, "insert into create_actor(platformid, partid, createtts, count, type,ver) values(%d, %d, '%s', %d, %u ,%d)", platid, partId, stime, count, dataType, ChannelId);

	ExecuteNewQuerySql(ChannelId, partId, DBS_MSG_INSERT, DB_SGSINFO, sql, extResult, 0);
}

///��������д��db
int CUpdateDataMan::StoreOnlineIntoDb(UINT16 platid, UINT16 ChannelId, UINT16 partId, const char* oltime, UINT32 count, BYTE olType)
{
	char sql[1024] = { 0 };
	std::map<UINT16, UINT32> extRes; //ִ��sql���

	sprintf(sql, "insert into online(platformid, partid, count, tts, type,ver) values(%d, %d, %d,'%s', %u, %d)", platid, partId, count, oltime, olType, ChannelId);
	int ret = ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_INSERT, DB_SGSINFO, sql, extRes, 1);
	if (ret != GM_OK || extRes.size() <= 0)
		return GM_ONLINE_STORE_DB_FALIED;

	return ret;
}

void CUpdateDataMan::UpdateAddupNewData(const char* updateTime)
{

	std::vector<UINT16> platid = GetCfgManObj()->GetAllPlatId();
	for (int j = 0; j<(int)platid.size(); j++)
	{
		std::vector<UINT16> channelId = GetCfgManObj()->GetAllchannel();
		for (int i = 0; i < (int)channelId.size(); i++)
		{

			//////������������
			//UpdateLeaveKeepMan(platid[j],channelId[i], updateTime);

			////���������������
			//UpdateMaxOnlineMan(platid[j],channelId[i], updateTime);

			////����ƽ����������
			//UpdateAverageOnlineMan(platid[j],channelId[i], updateTime);

			////���¸�������
			//UpdatePaymentMan(platid[j],channelId[i], updateTime);

		}
	}
}

void CUpdateDataMan::UpdateAccountData(UINT16 platid, UINT16 ChannelId, const char* updateTime)
{
	//��ȡ�����ʺ�����(��ע��͵�¼��)
	char sql[1024 * 2] = { 0 };
	std::vector<UINT16/*����ID*/> partIdVec;
	//ָ�������ķ���;
	GetCfgManObj()->GetPlatAllPartId(ChannelId, partIdVec);

	for (int i = 0; i < (int)partIdVec.size(); i++)
	{
		DBS_RESULT result = NULL;
		for (int j = 1; j < 6; j++)
		{
			if (j == 5)
				sprintf(sql, "%s (select * from player_account_%u where (date(lasttts) = date('%s') or date(regtts) = date('%s'))  and partid=%d and ver=%d and platsource=%d ) ", sql, j, updateTime, updateTime, partIdVec[i], ChannelId, platid);
			else
				sprintf(sql, "%s (select * from player_account_%u where (date(lasttts) = date('%s') or date(regtts) = date('%s'))  and partid=%d and ver=%d and platsource=%d ) union ", sql, j, updateTime, updateTime, partIdVec[i], ChannelId, platid);
		}
		ExecuteNewQuerySql(ChannelId, partIdVec[i], DBS_MSG_QUERY, DB_SGSVERIFY, sql);
		std::map<UINT16, DBS_RESULT>::iterator it = GetNewDBSResult().find(partIdVec[i]);
		if (it != GetNewDBSResult().end())
			result = it->second;
		//ExecuteNewAccountDbQuerySql(DBS_MSG_QUERY, DB_SGSVERIFY, sql, result);
		if (result == NULL)
			continue;
		else
		{
			DBS_ROW row = NULL;
			char storeSql[1024 * 9] = { 0 };
			sprintf(storeSql, "insert into account_info(accountid, user, pwd, ip, regtts, platsource, os, model, regdev, ver, visit, lastdev, lasttts, partid, status) values");
			int num = 0;
			int count = 0;

			UINT32 affectRow = DBS_AffectedRows(result);
			while ((row = DBS_FetchRow(result)) != NULL)
			{
				num++;
				count++;
				if ((num == 19) || (count == affectRow))
				{
					sprintf(storeSql, "%s (%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, '%s', '%s', %d, %d)",
						storeSql, atoi(row[0]), row[1], row[2], row[3], row[4], row[5], row[6], row[7], row[8], row[9], atoi(row[10]),
						row[11], row[12], atoi(row[13]), atoi(row[14]));
					ExecuteNewQuerySql(ChannelId, partIdVec[i], DBS_MSG_INSERT, DB_SGSINFO, storeSql);
					if (count == affectRow)
					{
						ClearNewDBSResult();
						break;
					}
					memset(storeSql, 0, 1024 * 9);
					sprintf(storeSql, "insert into account_info(accountid, user, pwd, ip, regtts, platsource, os, model, regdev, ver, visit, lastdev, lasttts, partid, status) values");
					num = 0;
				}
				else
					sprintf(storeSql, "%s (%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, '%s', '%s', %d, %d),",
					storeSql, atoi(row[0]), row[1], row[2], row[3], row[4], row[5], row[6], row[7], row[8], row[9], atoi(row[10]),
					row[11], row[12], atoi(row[13]), atoi(row[14]));
			}
		}
	}
}

void CUpdateDataMan::UpdateActorData(UINT16 platid, UINT16 ChannelId, const char* stime)
{
	//��ȡ����ɫ����(�������͵�¼��)
	char sql[1024] = { 0 };
	std::vector<UINT16/*����ID*/> partIdVec;
	std::map<UINT16, DBS_RESULT>::iterator it;
	DBS_RESULT  result = NULL;

	GetCfgManObj()->GetPlatAllPartId(ChannelId, partIdVec);
	sprintf(sql, "select id, createtts, accountid, nick, sex, gold, level, occu, exp, \
				 				 cash, cashcount, actortype, hp, mp, attack, def, vit, cp, ap, packsize,\
								 				 waresize, guild, dressid, faction, vist, online, lvtts from actor\
												 				 where date(lvtts)=date('%s') or date(createtts)=date('%s')", stime, stime);
	for (int i = 0; i < (int)partIdVec.size(); i++)
	{
		ClearNewDBSResult();
		ExecuteNewQuerySql(ChannelId, partIdVec[i], DBS_MSG_QUERY, DB_SGS, sql);
		it = m_dbsNewResult.find(partIdVec[i]);
		if (it != m_dbsNewResult.end())
			result = it->second;

		if (result == NULL)
			continue;
		DBS_ROW row = NULL;
		char storeSql[1024 * 9] = { 0 };
		int num = 0;
		int count = 0;
		UINT32 affectRow = DBS_AffectedRows(result);
		/*const char* name =  "���˹������";
		char gbToUt[128] = { 0 };
		gb2312_utf8(name, strlen(name), gbToUt, 128);*/

		sprintf(storeSql, "insert into actor_info(actorid, createtts, accountid, nick, sex, gold, level\
						  					   , occu, exp, cash, cashcount, actortype, hp, mp, attack, def, vit, cp, ap, packsize\
											   					   , waresize, guild, dressid, faction, vist, online,lvtts) values");
		while ((row = DBS_FetchRow(result)) != NULL)
		{
			num++;
			count++;
			SBigNumber gold;
			SBigNumber exp;
			SBigNumber online;
			U64ToBigNumber(_atoi64(row[5]), gold);
			U64ToBigNumber(_atoi64(row[8]), exp);
			U64ToBigNumber(_atoi64(row[25]), online);

			if ((num == 19) || (affectRow == count))
			{
				sprintf(storeSql, "%s (%d, '%s', %u, '%s', '%s', %llu, %u, %u, %llu, %u, %u, %u, %u \
								  								  , %u, %u,%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %llu, '%s')", storeSql
																  , atoi(row[0]), row[1], atoi(row[2]), row[3], row[4], BigNumberToU64(gold)
																  , atoi(row[6]), atoi(row[7]), BigNumberToU64(exp), atoi(row[9]), atoi(row[10])
																  , atoi(row[11]), atoi(row[12]), atoi(row[13]), atoi(row[14]), atoi(row[15])
																  , atoi(row[16]), atoi(row[17]), atoi(row[18]), atoi(row[19]), atoi(row[20])
																  , atoi(row[21]), atoi(row[22]), atoi(row[23]), atoi(row[24]), BigNumberToU64(online)
																  , row[26]);
				ExecuteNewQuerySql(ChannelId, partIdVec[i], DBS_MSG_INSERT, DB_SGSINFO, storeSql);
				memset(storeSql, 0, 1024 * 9);
				sprintf(storeSql, "insert into actor_info(actorid, createtts, accountid, nick, sex, gold, level\
								  								  , occu, exp, cash, cashcount, actortype, hp, mp, attack, def, vit, cp, ap, packsize\
																  								  , waresize, guild, dressid, faction, vist, online,lvtts) values");
				num = 0;
				if ((affectRow == count))
				{
					ClearNewDBSResult();
					break;
				}
			}
			else
				sprintf(storeSql, "%s (%d, '%s', %u, '%s', '%s', %llu, %u, %u, %llu, %u, %u, %u, %u \
								  								  , %u, %u,%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %llu, '%s'),", storeSql
																  , atoi(row[0]), row[1], atoi(row[2]), row[3], row[4], BigNumberToU64(gold)
																  , atoi(row[6]), atoi(row[7]), BigNumberToU64(exp), atoi(row[9]), atoi(row[10])
																  , atoi(row[11]), atoi(row[12]), atoi(row[13]), atoi(row[14]), atoi(row[15])
																  , atoi(row[16]), atoi(row[17]), atoi(row[18]), atoi(row[19]), atoi(row[20])
																  , atoi(row[21]), atoi(row[22]), atoi(row[23]), atoi(row[24]), BigNumberToU64(online)
																  , row[26]);
		}
	}
}

///�ڲ����洦��[lkType:1-�ʺ�����, 2-�豸����, 3-��ɫ����]
int CUpdateDataMan::LeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* registTime/*ע��򴴽�ʱ��*/, const char* updateTime/*��������ʱ��*/, BYTE day/*��X�ջ�������*/, BYTE lktType/*����ʱ������*/)
{
	if (registTime == NULL || day == 0)
		return GM_INVALID_PARAM;
	char qtime[20] = { 0 };

	char Ttime[20] = { 0 };
	///��ȡʱ��(isNow= TRUE:��ȡ��ǰϵͳʱ��, isNow= FALSE:��ȡstime�е�ʱ��)
	GetTime(updateTime, Ttime, FALSE);
	sprintf(qtime, "%s %s", registTime, Ttime);

	//�ʺ�������´���
	AccountLeaveKeepMan(platid, ChannelId, qtime, updateTime, day, lktType);

	//�豸������´���
	DeviceLeaveKeepMan(platid, ChannelId, qtime, updateTime, day, lktType);

	//��ɫ������´���
	ActorLeaveKeepMan(platid, ChannelId, qtime, updateTime, day, lktType);

	return GM_OK;
}

//�ɸ���Ϊ1-120��ʱ����д��;
void CUpdateDataMan::UpdateLeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* updateTime)
{
	std::map<UINT16/*����ID*/, LeaveDateVec/*��������*/> partLkDate;
	char sql[1024 * 2] = { 0 };

	//ͳ�Ƹ���������������
	for (int i = 1; i < 6; i++)
	{
		if (i == 5)
			sprintf(sql, "%s (select distinct date(regtts) from player_account_%u  )", sql, i);
		else
			sprintf(sql, "%s (select distinct date(regtts) from player_account_%u) union ", sql, i);
	}

	ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_QUERY, DB_SGSVERIFY, sql);
	if (m_dbsNewResult.size() <= 0)
		return;

	//��ȡ��ע�������
	std::map<UINT16, DBS_RESULT>::iterator it = m_dbsNewResult.begin();
	for (it; it != m_dbsNewResult.end(); it++)
	{
		LeaveDateVec leaveDateVec;
		DBS_RESULT result = it->second;
		DBS_ROW row = NULL;
		int i = 0;
		while ((row = DBS_FetchRow(result)) != NULL)
		{
			LeaveDate lkDate;
			memset(&lkDate, 0, sizeof(LeaveDate));
			leaveDateVec.push_back(lkDate);
			strcpy(leaveDateVec[i].date, row[0]);
			i++;

		}
		partLkDate.insert(std::make_pair(it->first, leaveDateVec));
	}

	ClearNewDBSResult();
	//�Բ�ͬ���������洦��
	std::map<UINT16, LeaveDateVec>::iterator its = partLkDate.begin();
	int count = (int)GetCfgManObj()->GetLeaveKeepTimeType().size();
	for (its; its != partLkDate.end(); its++)
	{
		for (int i = 0; i <(int)its->second.size(); i++)
		{
			for (int j = 0; j < count; j++)
			{
				LeaveKeepMan(platid, ChannelId, its->second[i].date, updateTime, GetCfgManObj()->GetLeaveKeepTimeType()[j], GetCfgManObj()->GetLeaveKeepTimeType()[j]);
			}
		}
	}
}

void CUpdateDataMan::UpdateAddupNewAccountMan(UINT16 platid, UINT16 ChannelId, const char* updateTime, const char* keyWord, BYTE addType)
{
	char sql[1024 * 2] = { 0 };
	std::map<UINT16, UINT32> addNewCount; //������(������δ������ɫ���ʺŻ��豸)
	std::map<UINT16, UINT32> todayAddNew; //������(��δ������ɫ���ʺŻ��豸)
	std::map<UINT16, UINT32> toNowAddNew; //��������������(��δ������ɫ���ʺŻ��豸)

	//����������(������δ������ɫ���ʺŻ��豸)
	//��ѯ�����豸��Ŀǰ�޷��ж��������豸�����Ľ�ɫ���Ƿ������ʺŴ����Ľ�ɫ
	sprintf(sql, "select count(distinct %s) from actor where date(createtts) = date('%s')", keyWord, updateTime);
	ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_QUERY, DB_SGS, sql, addNewCount, 0);
	std::map<UINT16, UINT32>::iterator it = addNewCount.begin();
	for (it; it != addNewCount.end(); it++)
	{
		if (GetCfgManObj()->IsWriteIntoDb())
			StoreAddupNewIntoDb(platid, ChannelId, it->first, updateTime, it->second, addType, 1);
	}

	addNewCount.clear();
	memset(sql, 0, 1024 * 2);

	//����������(��δ������ɫ���ʺŻ��豸)
	sprintf(sql, "select ");
	for (int i = 1; i < 6; i++)
	{
		if (i == 5)
			sprintf(sql, "%s (select count(*) from player_account_%u where date(regtts) = date('%s')) ", sql, i, updateTime);
		else
			sprintf(sql, "%s (select count(*) from player_account_%u where date(regtts) = date('%s')) + ", sql, i, updateTime);
	}

	ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_QUERY, DB_SGSVERIFY, sql, todayAddNew, 0);
	it = todayAddNew.begin();
	for (it; it != todayAddNew.end(); it++)
	{
		if (GetCfgManObj()->IsWriteIntoDb())
			StoreAddupNewIntoDb(platid, ChannelId, it->first, updateTime, it->second, addType, 2);
	}

	todayAddNew.clear();
	memset(sql, 0, 1024 * 2);

	//������������������(��δ������ɫ���ʺŻ��豸)
	sprintf(sql, "select ");
	for (int i = 1; i < 6; i++)
	{
		if (i == 5)
			sprintf(sql, "%s (select count(*) from player_account_%u where regtts <= '%s')", sql, i, updateTime);
		else
			sprintf(sql, "%s (select count(*) from player_account_%u where regtts <= '%s') + ", sql, i, updateTime);

	}
	ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_QUERY, DB_SGSVERIFY, sql, toNowAddNew, 0);
	it = toNowAddNew.begin();
	for (it; it != toNowAddNew.end(); it++)
	{
		if (GetCfgManObj()->IsWriteIntoDb())
			StoreAddupNewIntoDb(platid, ChannelId, it->first, updateTime, it->second, addType, 3);
	}
	toNowAddNew.clear();
}

void CUpdateDataMan::UpdateAddupNewActorMan(UINT16 platid, UINT16 ChannelId, const char* updateTime)
{
	char sql[1024] = { 0 };
	std::map<UINT16, UINT32> addNewCount; //������(���մ����Ľ�ɫ��)
	std::map<UINT16, UINT32> toNowAddNew; //������(�������ڴ����Ľ�ɫ��)

	//���մ����Ľ�ɫ������
	sprintf(sql, "select count(*) from actor where date(createtts) = date('%s')", updateTime);
	ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_QUERY, DB_SGS, sql, addNewCount, 0);
	std::map<UINT16, UINT32>::iterator it = addNewCount.begin();
	for (it; it != addNewCount.end(); it++)
	{
		if (GetCfgManObj()->IsWriteIntoDb())
			StoreAddupNewIntoDb(platid, ChannelId, it->first, updateTime, it->second, 3, 1);

	}
	addNewCount.clear();

	//�������ڴ����Ľ�ɫ������
	memset(sql, 0, 1024);
	sprintf(sql, "select count(*) from actor where createtts <= '%s'", updateTime);
	ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_QUERY, DB_SGS, sql, toNowAddNew, 0);
	it = toNowAddNew.begin();
	for (it; it != toNowAddNew.end(); it++)

	{
		if (GetCfgManObj()->IsWriteIntoDb())
			StoreAddupNewIntoDb(platid, ChannelId, it->first, updateTime, it->second, 3, 2);
	}
	toNowAddNew.clear();

}

///���¶�ʱˢ��������������
void CUpdateDataMan::UpdateCommOnlineMan(UINT16 platid, UINT16 ChannelId, const char* updateTime)
{
	PartLkSockKey sockKey;
	GetServeConnObj()->GetLinkSocketKey(ChannelId, 0, sockKey);
	if (sockKey.size() <= 0)
		return;
	PartLkSockKey::iterator it = sockKey.begin();
	for (it; it != sockKey.end(); it++)
	{
		UINT32 onlineCount = 0; //��ǰ������������
		for (int i = 0; i <(int)it->second.size(); i++)
		{
			UINT32 num = 0;
			OnlineDataMan(it->second[i].sock, it->second[i].key, &num);
			onlineCount += num;
		}
		StoreOnlineIntoDb(platid, ChannelId, it->first, updateTime, onlineCount, OL_COMM);
	}
}

void CUpdateDataMan::UpdateMaxOnlineMan(UINT16 platid, UINT16 ChannelId, const char* updateTime)
{
	char sql[1024] = { 0 };
	std::map<UINT16, DBS_RESULT> extResult;


	sprintf(sql, "select max(count), tts from online where date(tts) = date('%s')", updateTime);
	ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_QUERY, DB_SGSINFO, sql);
	std::map<UINT16, DBS_RESULT>::iterator it = GetNewDBSResult().begin();
	for (it; it != GetNewDBSResult().end(); it++)
	{
		DBS_RESULT result = it->second;
		DBS_ROW row = NULL;
		if (result == NULL)
			continue;
		while ((row = DBS_FetchRow(result)) != NULL)
		{
			if (GetCfgManObj()->IsWriteIntoDb())
				StoreOnlineIntoDb(platid, ChannelId, it->first, row[1], atoi(row[0]), OL_MAX);
		}

	}
	ClearNewDBSResult();

}

void CUpdateDataMan::UpdateAverageOnlineMan(UINT16 platid, UINT16 ChannelId, const char* updateTime)
{
	char sql[1024] = { 0 };
	sprintf(sql, "select count(id), sum(count) from online where date(tts) = date('%s') and type = 1", updateTime);
	ExecuteNewQuerySql(ChannelId, 0, DBS_MSG_QUERY, DB_SGSINFO, sql);

	if (m_dbsNewResult.size() <= 0)
		return;

	std::map<UINT16, DBS_RESULT>::iterator it = m_dbsNewResult.begin();
	for (it; it != m_dbsNewResult.end(); it++)
	{
		DBS_RESULT result = it->second;
		if (result == NULL)
			continue;
		DBS_ROW row = DBS_FetchRow(result);
		if (row == NULL)
			continue;

		int  average = 0;
		if (atoi(row[0]) == 0)
			average;
		else
			average = atoi(row[1]) / atoi(row[0]);
		if (GetCfgManObj()->IsWriteIntoDb())
			StoreOnlineIntoDb(platid, ChannelId, it->first, updateTime, average, OL_AVERAGE);
		else

			LogInfo(("��ǰˢ��ʱ��[%s], ���ڷ���[%u], ��������[%d], ��������[%u]", updateTime, atoi(row[0]), average, OL_AVERAGE));
	}
	ClearNewDBSResult();
}


void CUpdateDataMan::UpdatePaymentMan(UINT16 platid, UINT16 ChannelId, const char* updateTime)
{
	char sql[1024] = { 0 };

	UINT32 actorNum = 0;		//���ո��ѽ�ɫ��
	UINT32 paymentNum = 0;	//���ո��Ѵ���
	UINT32 firstRechNum = 0;	//�׳��û���(��ѯ�����ݱ���ȷ��������Ҫ��������)

	UINT32 totalNum = 0;		//�ۼƸ����û���
	UINT32 addNewRmb = 0;		//���������û����ѽ��
	UINT32 rmb = 0;			 //���ո��ѽ��
	UINT32 totalRmb = 0;		//�ܸ��ѽ��

	std::map<UINT16, DBSHandle> sockKey;
	GetServeConnObj()->GetDBShandle(ChannelId, 0, sockKey);
	std::map<UINT16, DBSHandle>::iterator it = sockKey.begin();
	for (it; it != sockKey.end(); it++)
	{
		RechargeDataMan(ChannelId, it->first, PY_CASH_ACTOR, updateTime, &actorNum);
		RechargeDataMan(ChannelId, it->first, PY_CASH_COUNT, updateTime, &paymentNum);
		FirstRechargeMan(ChannelId, it->first, it->second, updateTime, &firstRechNum);
		RechargeDataMan(ChannelId, it->first, PY_CASH_TOTAL, updateTime, &totalNum);
		RechargeDataMan(ChannelId, it->first, PY_CASH_NRMB, updateTime, &addNewRmb);
		RechargeDataMan(ChannelId, it->first, PY_CASH_CRMB, updateTime, &rmb);
		RechargeDataMan(ChannelId, it->first, PY_CASH_TORMB, updateTime, &totalRmb);

		sprintf(sql, "insert into cash(platformid, partid, actorcount, paymentcount, firstrechargecount, \
					 					 totalcount, adduprmb, rmb, totalrmb, tts) values(%d, %d, %d, %d,%d, %d, %d, %d, %d, '%s')",
										 ChannelId, it->first, actorNum, paymentNum, firstRechNum, totalNum, addNewRmb, rmb, totalRmb, updateTime);

		if (GetCfgManObj()->IsWriteIntoDb())
			ExecuteNewRechargeQuerySql(it->second, DBS_MSG_INSERT, DB_SGSINFO, sql);
		else
			LogInfo(("��ǰˢ��ʱ��[%s], ���ո��ѽ�ɫ��[%d], ���ո��Ѵ���[%d], \
					 					 �׳��û���[%d], �ۼƸ����û���[%d], ���������û����ѽ��[%d], \
										 					 ���ո��ѽ��[%d],�ܸ��ѽ��[%d]", updateTime, actorNum, paymentNum,
															 firstRechNum, totalNum, addNewRmb, rmb, totalRmb));
	}
}


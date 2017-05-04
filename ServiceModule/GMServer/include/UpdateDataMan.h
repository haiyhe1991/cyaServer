#ifndef __CUPDATE_DATA_MAN_H__
#define __CUPDATE_DATA_MAN_H__

#include "GMProtocal.h"
#include "GMComm.h"
#include "DBSSdk.h"
#include "cyaLocker.h"
#include <vector>
#include <map>

class CUpdateDataMan
{

public:
	CUpdateDataMan();
	~CUpdateDataMan();


	///更新当日数据并写入数据库
	void UpdateAddupNewData(const char* updateTime);

	///更新帐号数据
	void UpdateAccountData(UINT16 platid, UINT16 ChannelId, const char* updateTime);
	///更新角色数据
	void UpdateActorData(UINT16 platid, UINT16 ChannelId, const char* updateTime);
	///更新留存数据
	void UpdateLeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* updateTime);
	///更新当日帐号或设备新增数
	void UpdateAddupNewAccountMan(UINT16 platid, UINT16 ChannelId, const char* updateTime, const char* keyWord, BYTE addType);
	///更新当日角色新增数
	void UpdateAddupNewActorMan(UINT16 platid, UINT16 ChannelId, const char* updateTime);
	///更新定时刷新在线人数处理
	void UpdateCommOnlineMan(UINT16 platid, UINT16 ChannelId, const char* updateTime);
	///更新最大在线人数处理
	void UpdateMaxOnlineMan(UINT16 platid, UINT16 ChannelId, const char* stime);
	///更新平均在线人数处理
	void UpdateAverageOnlineMan(UINT16 platid, UINT16 ChannelId, const char* stime);
	///更新付费数据处理
	void UpdatePaymentMan(UINT16 platid, UINT16 ChannelId, const char* stime);

public:
	///执行sql(返回数量, partId=0:获取平台所有分区, partId不为0表示指定分区)
	int ExecuteNewQuerySql(UINT16 ChannelId, UINT16 partId, DBS_MSG_TYPE dsType, const char* db, const char* sql, std::map<UINT16, UINT32>& getCount, BYTE isAffect);
	///sql 执行（针对留存处理, partId=0:获取平台所有分区, partId不为0表示指定分区）
	int ExecuteNewQuerySql(UINT16 ChannelId, UINT16 partId, DBS_MSG_TYPE dsType, const char* db, const char* sql, std::map<UINT16, float>& getCount);
	///执行sql(不返回结果集，需要手动获取, partId=0:获取平台所有分区, partId不为0表示指定分区)
	int ExecuteNewQuerySql(UINT16 ChannelId, UINT16 partId, DBS_MSG_TYPE dsType, const char* db, const char* sql);
	///充值数据库执行sql处理
	int ExecuteNewRechargeDbQuerySql(DBS_MSG_TYPE dsType, const char* db, const char* sql, UINT32* count);
	///充值数据库执行sql处理
	int ExecuteNewRechargeDbQuerySql(DBS_MSG_TYPE dsType, const char* db, const char* sql, DBS_RESULT& result);
	///帐号数据库执行sql处理
	int ExecuteNewAccountDbQuerySql(DBS_MSG_TYPE dsType, const char* db, const char* sql, UINT32* count);
	///帐号数据库执行sql处理
	int ExecuteNewAccountDbQuerySql(DBS_MSG_TYPE dsType, const char* db, const char* sql, DBS_RESULT& result);

	///清除执行sql获得的结果集
	void ClearNewDBSResult();
	///获取执行sql结果集
	std::map<UINT16, DBS_RESULT>& GetNewDBSResult()
	{
		return m_dbsNewResult;
	}

private:
	///充值数据执行sql处理
	int ExecuteNewRechargeQuerySql(DBSHandle handle, DBS_MSG_TYPE dsType, const char* db, const char* sql);
	///充值数据执行sql处理
	int ExecuteNewRechargeQuerySql(DBSHandle handle, DBS_MSG_TYPE dsType, const char* db, const char* sql, UINT32* count);
	///日期处理
	void DateMan(UINT16 ChannelId, const char* registTime, BYTE day, char lkTime[20]);



private:
	///平台分区注册用户数处理
	void GetPlatRegistCount(UINT16 platid/*平台*/, UINT16 ChannelId, BYTE lktType/*留存时间类型*/, const char* regTime/*注册数据*/, std::map<UINT16/*分区ID*/, UINT32/*注册数*/>& reg);
	///平台分区登录用户数处理
	void GetPlatLoginCount(UINT16 platId, UINT16 ChannelId, BYTE lktType, const char* regTime, const char* lgTime/*登录时间*/, std::map<UINT16/*分区ID*/, UINT32/*注册数*/>& lg);
	///今日登录用户数处理
	void GetTodayLoginCount(UINT16 platId, UINT16 ChannelId, const char* updateTime, std::map<UINT16/*分区ID*/, UINT32/*注册数*/>& todayLg);
	///平台分区登录设备数处理
	void GetPlatLoginDeviceCount(UINT16 ChannelId, BYTE lktType, const char* regTime, const char* lgTime, std::map<UINT16/*分区ID*/, UINT32/*登录设备数*/>& lgDevice);
	///今日登录设备数处理
	void GetTodayLoginDeviceCount(UINT16 ChannelId, const char* updateTime, std::map<UINT16/*分区ID*/, UINT32/*今日登录设备数*/>& todayLgDevice);
	///平台分区角色创建数处理
	void GetPlatCreateActorCount(UINT16 ChannelId, BYTE lktType, const char* regTime, std::map<UINT16/*分区ID*/, UINT32/*创建角色数*/>& createActor);
	///平台分区登录角色数处理
	void GetPlatLoginActorCount(UINT16 ChannelId, BYTE lktType, const char* regTime, const char* lgTime, std::map<UINT16/*分区ID*/, UINT32/*登录角色数*/>& lgActor);

	///今日登录角色数处理
	void GetTodayLoginActorCount(UINT16 ChannelId, const char* updateTime, std::map<UINT16/*分区ID*/, UINT32/*今日登录角色数*/>& todayLgActor);
	///在线人数数据处理
	int OnlineDataMan(SOCKET sockt, const char* key/*数据表关键字段名*/, UINT32* count/*在线人数*/);
	///充值数据库数据处理
	void RechargeDataMan(UINT16 ChannelId, UINT16 partId, BYTE operType, const char* updateTime, UINT32* count/*返回相应的充值数据*/);
	///首充角色数处理
	void FirstRechargeMan(UINT16 ChannelId, UINT16 partId, DBSHandle handle, const char* updateTime, UINT32* count/*首充人数*/);

private:
	///内部留存处理[lkType:1-帐号留存, 2-设备留存, 3-角色留存]
	int LeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* registTime/*注册留存时间*/, const char* updateTime/*更新留存时间*/, BYTE day, BYTE lktType);
	///帐号留存处理
	int AccountLeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* registTime, const char* updateTime, BYTE day, BYTE lktType);
	///设备留存处理
	int DeviceLeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* registTime, const char* updateTime, BYTE day, BYTE lktType);
	///角色留存处理
	int ActorLeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* registTime, const char* updateTime, BYTE day, BYTE lktType);

private:
	///留存数据存入db
	int StoreLeaveKeepIntoDb(UINT16 platid, UINT16 ChannelId, UINT16 partId, BYTE lkType, const char* stime, const char* updateTime, UINT32 rgCount, UINT32 lgCount, UINT32 currLgCount, float lkRate, BYTE lktType);
	///写入新增数据写入db[stime:写入时间, addType:写入类型(1-帐号新增, 2-设备新增, 3-角色新增)]
	void StoreAddupNewIntoDb(UINT16 platid, UINT16 ChannelId, UINT16 partId, const char* stime, UINT32 count, BYTE addType, BYTE dataType/*数据表字段中的数据类型*/);
	///在线人数写入db[oltime:写入时间, olType:类型(1-普通在线, 2-最大在线, 3-平均在线)]
	int StoreOnlineIntoDb(UINT16 platid, UINT16 ChannelId, UINT16 partId, const char* oltime, UINT32 count, BYTE olType);

private:

	///留存日期
	struct LeaveDate
	{
		char date[11]; //留存日期
	};

	typedef std::vector<LeaveDate> LeaveDateVec;
	std::map<UINT16, DBS_RESULT> m_dbsNewResult;	//db 结果集
	CXTLocker m_dbLocker;
};

#endif
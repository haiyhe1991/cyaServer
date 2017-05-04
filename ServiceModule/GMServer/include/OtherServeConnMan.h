#ifndef __OTHER_SERVE_CONN_MAN_H__
#define __OTHER_SERVE_CONN_MAN_H__

#include "GMCfgMan.h"
#include "cyaCoreTimer.h"
#include "cyaLocker.h"
#include "cyaThread.h"
#include "GMProtocal.h"
#include "GMComm.h"
#include "DBSSdk.h"
#include <vector>
#include "UpdateDataMan.h"

typedef std::vector<OnlineInfo*> OnlineInfoVec;
class COherServeConnMan
{
	///Linker sokcet and key
	struct LinkSocketKey
	{
		SOCKET sock;
		char key[33];
	};

public:
	COherServeConnMan();
	~COherServeConnMan();
	/*实时新增统计;GM_NEW_STATISTICS GMRealTimeAddedStatisticsReq(接口查询内容)*/
	int QueryAddNewStatistics(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, std::vector<TimeAddedinfo>&Addedinfo, UINT32*count);
	/*查询订单详情;GM_NEW_STATISTICS GMRealTimeAddedStatisticsReq(接口查询内容)*/
	int QueryRechargeOrderDealwith(UINT16 platId, UINT16 channelId, UINT16 partId, UINT32 from, BYTE to, const char* startTime, const char* endTime, std::vector<RecharegeStatisticeIofo>&RecharnInfo,
		UINT32*count);
	/*查询充值统计;GM_NEW_STATISTICS GMRealTimeAddedStatisticsReq(接口查询内容)*/
	int QueryRechargeStatistics(UINT16 platId, UINT16 channelId, UINT16 partId, UINT32 from, BYTE to, const char* startTime, const char* endTime, std::vector<StartisticsInfo>&RecharnInfo, UINT32*count);
	/*留存详情统计;GM_NEW_STATISTICS GMRealTimeAddedStatisticsReq(接口查询内容)*/
	int GetLeaveKeepStatistics(UINT16 platId, UINT16 channelId/*渠道*/, UINT16 partId, const char* starTime, const char* endTime, BYTE lktype, BYTE lkTimeType, std::vector<GMQLeavekeepDateInfo>&LeaveKeepInfo, UINT32* getcount);
	/*在线人数统计;GM_NEW_STATISTICS GMRealTimeAddedStatisticsReq(接口查询内容)*/
	int GetOnlineHistory(UINT16 platId, UINT16 channelId, UINT16 partId, const char* starTime, const char* endTime, std::vector<OnlineInfo>& olInfo, UINT32 from, BYTE to, UINT32* total);
	/*等级分布统计;GM_NEW_STATISTICS GMRealTimeAddedStatisticsReq(接口查询内容)*/
	int Grade_RatingStatistics(UINT16 platId, UINT16 channelId, UINT16 partId, const char* starTime, const char* endTime, std::vector<GM_Grade_RatingStatisticsInfo>& OnlineStatisticsInfo, BYTE* rentNum, UINT32* Grade_sum);

private:
	/*=======================================================================================*/
	/*实时新增统计*/

	int QueryAddNewDevice(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count);///	新增设备数处理;

	int QueryAddNewAccount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count);///	新增账号数处理;

	int QueryAddNewActor(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count);	///		新增角色数处理;

	int QueryLogDevice(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count);	///		登录设备数处理; 

	int QueryLogAccount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count);	///		登录账号数处理; 

	int QueryLogActor(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count);	///		登录角色数处理; 

	int QueryActiveDevice(UINT32 countA, UINT32 countB, UINT32* count);															///		活跃设备数处理;

	int	QueryActiveAccount(UINT32 countA, UINT32 countB, UINT32* count);															///		活跃账号数处理;

	int QueryAcitveActor(UINT32 countA, UINT32 countB, UINT32* count);															///		活跃角色数处理;

	//================sql数据处理;
	int GetAccount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, const char* keyWord, UINT32* count);	///		获取帐号数据;

	int GetDevice(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, const char* keyWord, UINT32* count);	///		获取设备数据;

	int GetActor(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, const char* keyWord, UINT32* count);	///		获取角色数据;
	///查询登录用户,设备，角色[typ=1:帐号活跃数，2:设备活跃数，3:角色活跃数]///
	int QLoginCount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count, BYTE type = 1);		///		查询活跃数;	


	/*=======================================================================================*/
	/*实时充值统计*/

	int QueryPaymentActor(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32& count);					///		查询充值统计——查询付费用户数;

	int QueryFirstRecharge(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32& count);				///		查询充值统计——新增付费用户数;

	int QueryPaymentCount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32& count);					///		查询充值统计——查询付费次数;

	int QueryPaymentMoney(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32& count);					///		查询充值统计——查询付费金额(当日付费金额);

	int QueryPaymentLogin(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, float& retArpu);				///		查询充值统计——查询登录付费率;

	int QueryPaymentAdd(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, float& retArpu);					///		查询充值统计——查询注册付费率;

	int QuerypaymentArpu(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, float& retArpu);				///		查询充值统计——查询付费平均数ARPU;

	int	QueryLoginArpu(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, float& retArpu);					///		查询充值统计——查询登录平均数ARPU;

	int ResolveTimeNum(const char* startTime, const char* endtime/*,const UINT32 from,BYTE to,char * FromTime, char*FromTotime*/);					///		查询充值统计——获取查询天数;

	int GoToTime(char* STime, char* ETime, int i, bool trues);																							///		查询充值统计——控制查询天数;

	int GetPaymentMoneySQL(UINT16 platId, UINT16 channelId, UINT16 partId, const char*startTime, const char* endTime, StartisticsInfo & RecharInfo);		///		查询充值统计——构造查询Sql语句;


	/*=======================================================================================*/
	/*查询订单详情*/
	int GetOrderInfo(UINT16 partId, UINT32 from, BYTE to, const char* startTime, const char* endTime, char sql[1024 * 2]);								///		查询订单详情——构造sql信息处理;

	void GetOrderTotal(UINT16 platId, UINT16 channelId, UINT16 partId, UINT32 from, const char* start, const char* endt, UINT32* total);				///		查询订单详情——获取订单总数;

	int PaymentMan(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, const char* keyWord, UINT32& count);	///		付费处理;

	int CashDataMan(UINT16 platId, UINT16 channelId, UINT16 partId, BYTE operType, const char* startTime, const char* endTime, UINT32& count);		///		付费处理2;

	/*=======================================================================================*/
	/*留存统计*/

	int GetIfLeaveKeep(UINT16 platId, UINT16 channelId, UINT16 partId, const char* stime, const char* tbName, BYTE lkTimeType, std::vector<GMQLeavekeepDateInfo>&LeaveKeepInfo, int ic);///留存——分类查询处理;

	int GetLeaveKeep(UINT16 platId, UINT16 channelId, UINT16 partId, const char* stime, const char* tbName, BYTE lkTimeType, UINT16* Retained, int i, UINT32* NewRecount);				///	留存——获取数据处理;

	int DeviceLeave(UINT16 platId, UINT16 channelId, UINT16 partId, BYTE operType, const char* startTime, const char* endTime, UINT32* count);									///	留存——设备留存;

	int AccountLeave(UINT16 platId, UINT16 channelId, UINT16 partId, BYTE operType, const char* startTime, const char* endTime, UINT32* count);									///	留存——账号留存;

	int ActorLeave(UINT16 platId, UINT16 channelId, UINT16 partId, BYTE operType, const char* startTime, const char* endTime, UINT32* count);									///	留存——角色留存;

	int LeaveRate(UINT16 platId, UINT16 channelId, UINT16 partId, const char* stime, const char* tbName, BYTE lkTimeType, int ic, float* Getrate);									///	获取留存率;

	/*=======================================================================================*/
	/*在线数据*///①.后台数据基于账号统计用户；
	//实时在线——构造在线人数查询sql;
	void SqlOnelineTimeMan(UINT16 platId, UINT16 channelId, UINT16 partId, char extSql[1024], char sql[1024], const char* startTime, const char* endTime, UINT32 from, BYTE to, BYTE stype);
	//实时在线——获取在线人数记录数据;
	int SqlOneLineUserMan(UINT16 platId, UINT16 channelId, UINT16 partId, const char* extSql, const char* sql, std::vector<OnlineInfo>& olInfo, UINT32* total);

	///在线数据统计;=======================================
	//int OnlineStatistics(UINT16 platId, UINT16 partId,const char* starTime,const char* endTime, std::vector<GMOnlineDateStatisticsInfo>& OnlineStatisticsInfo, UINT32* count);

	/////在线数据统计处理;
	//int GetOnlineStatistics(UINT16 platId, UINT16 partId, const char* extSql, const char* sql, std::vector<GMOnlineDateStatisticsInfo>& olInfo, UINT32* count);

	////Sql在线数据统计处理;
	//int SqlGetOnlineStatistics(UINT16 platId, UINT16 partId,char extSql[1024], char sql[1024], const char* startTime, const char* endTime, UINT32* count);

	/////数据统计获取数据;
	//int GetSqlOlineStatistics(UINT16 platId, UINT16 partId, const char* extSql, const char* sql, std::vector<GMOnlineDateStatisticsInfo>& olInfo, UINT32* count);

	/*=======================================================================================*/
	/*等级分布统计*/

	int Get_Grade_RattingStatistics(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, std::vector<GM_Grade_RatingStatisticsInfo> & OnlineStatisticsInfo, BYTE* rentNum, UINT32* Grade_sum);																													///		获取等级分布统计处理;

	int GetcoutNum(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* Grade_sum);					///		获取总数;
	/*=======================================================================================*/

	void CheckTime(const char* startTime, const char* endTime, const char* keyWord, char start[125], char endt[125]);													///	时间检测;

	void CheckTimeB(const char* startTime, const char* endTime, const char* TendTime, const char* keyWord, char start[125], char endt[125], const char* TkeyWord, char Tendt[125]);		///时间检测2;

public:
	CUpdateDataMan& GetExtSqlObj()
	{
		return m_update;
	}

protected:
	///定时获取在线人数回调函数;调用次函数，进行获取在线人数;
	static void GetOnlineUserByTimer(void* p, TTimerID id);
	///实时获取更新;
	static void GetRealTimeNewByTimer(void *p, TTimerID id);
	//刷新在线人数的时间;m_reflushLogTimer(ReflushOnlineLogByTimer, this, 60*60*1000),
	static void ReflushOnlineLogByTimer(void* p, TTimerID id);
	//检测每天结束的线程;
	static int	CheckEndOfEveryDayThread(void*);

	///定时更新数据库,当天晚上23：59：59更新数据库内容;
	void OnCheckRunning();
	///实时更新数据库;根据配置参数来设定；
	void OnNewRuning();

	///正在获取在线人数
	void OnGetOnlineUser();

private:

	///在线人数处理
	void OnlineMan(UINT32 from, UINT16 platId, UINT16 partId, const char* extSql, const char* sql, std::vector<OnlineInfo>& olInfo, UINT32* count);
	///首充用户处理
	int FirstRechargeMan(UINT16 platId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count);
	///构造查询帐号sql信息处理;
	int GetAccountInfo(UINT16 partId, UINT32 from, BYTE to, BYTE isNow, const char* start, const char* endt, char extSql[1024 * 2], char sql[1024 * 2]);
	///构造查询帐号sql信息处理;
	int GetActorInfo(UINT16 partId, UINT32 from, BYTE to, BYTE isNow, const char* start, const char* endt, char extSql[1024 * 2], char sql[1024 * 2]);
	///获取总数;
	void GetTotal(UINT16 platId, UINT16 partId, UINT32 from, const char* db, const char* sql, UINT32* total);
	///获取帐号统计总数;
	void GetAccountTotal(UINT16 platId, UINT16 partId, UINT32 from, const char* start, const char* endt, UINT32* total);
	///获取激活设备总数;
	void GetActivatedDevice(UINT32 from, const char* start, const char* endt, UINT32* total);

	///日期处理
	void DateMan(UINT16 platId, UINT16 partId, const char* stime, BYTE day, char lkTime[20]);

	///当前查询日期是否为现在日期;
	BOOL IsNow(const char* startTime, const char* endTime);
	///当前查询日期是否为同一天
	BOOL IsSameDay(const char* startTime, const char* endTime);
	///在线时间处理
	void OnlineTimeMan(UINT16 platId, UINT16 partId, UINT32 from, BYTE to, char extSql[1024], char sql[1024], const char* startTime, const char* endTime, BYTE stype);
	///获取结果
	void GetExtSqlResult(std::map<UINT16, UINT32>& extRes, UINT16 partId, UINT32* count);
	///获取结果
	void GetExtSqlResult(std::map<UINT16, float>& extRes, UINT16 partId, float* count);
	///获取结果
	void GetExtSqlResult(std::map<UINT16, DBS_RESULT>& extRes, UINT16 partId, DBS_RESULT& result);

	char* tm2str(tm* pTm);

	tm* str2tm(char date[]);

	char* addDays(char date[], int days);


private:
	//定时器;
	CyaCoreTimer m_olineTimer;						//在线人数Timer,定时回去在线人数的时间;
	CyaCoreTimer m_reflushLogTimer;				//刷新logTimer
	CyaCoreTimer m_realTimeNew;					//实时新增定时器;
	BOOL m_isRun;

	CXTLocker m_locker;
	CXTLocker m_dbLocker;

	OSThread m_osthrd;
	OSThreadID m_osthrdId;
	CUpdateDataMan m_update;

};

void InitOtherServeManObj();

COherServeConnMan* GetOtherServeManObj();

void DestroyOtherServeManObj();

#endif
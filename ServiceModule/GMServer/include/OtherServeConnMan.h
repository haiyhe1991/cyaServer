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
	/*ʵʱ����ͳ��;GM_NEW_STATISTICS GMRealTimeAddedStatisticsReq(�ӿڲ�ѯ����)*/
	int QueryAddNewStatistics(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, std::vector<TimeAddedinfo>&Addedinfo, UINT32*count);
	/*��ѯ��������;GM_NEW_STATISTICS GMRealTimeAddedStatisticsReq(�ӿڲ�ѯ����)*/
	int QueryRechargeOrderDealwith(UINT16 platId, UINT16 channelId, UINT16 partId, UINT32 from, BYTE to, const char* startTime, const char* endTime, std::vector<RecharegeStatisticeIofo>&RecharnInfo,
		UINT32*count);
	/*��ѯ��ֵͳ��;GM_NEW_STATISTICS GMRealTimeAddedStatisticsReq(�ӿڲ�ѯ����)*/
	int QueryRechargeStatistics(UINT16 platId, UINT16 channelId, UINT16 partId, UINT32 from, BYTE to, const char* startTime, const char* endTime, std::vector<StartisticsInfo>&RecharnInfo, UINT32*count);
	/*��������ͳ��;GM_NEW_STATISTICS GMRealTimeAddedStatisticsReq(�ӿڲ�ѯ����)*/
	int GetLeaveKeepStatistics(UINT16 platId, UINT16 channelId/*����*/, UINT16 partId, const char* starTime, const char* endTime, BYTE lktype, BYTE lkTimeType, std::vector<GMQLeavekeepDateInfo>&LeaveKeepInfo, UINT32* getcount);
	/*��������ͳ��;GM_NEW_STATISTICS GMRealTimeAddedStatisticsReq(�ӿڲ�ѯ����)*/
	int GetOnlineHistory(UINT16 platId, UINT16 channelId, UINT16 partId, const char* starTime, const char* endTime, std::vector<OnlineInfo>& olInfo, UINT32 from, BYTE to, UINT32* total);
	/*�ȼ��ֲ�ͳ��;GM_NEW_STATISTICS GMRealTimeAddedStatisticsReq(�ӿڲ�ѯ����)*/
	int Grade_RatingStatistics(UINT16 platId, UINT16 channelId, UINT16 partId, const char* starTime, const char* endTime, std::vector<GM_Grade_RatingStatisticsInfo>& OnlineStatisticsInfo, BYTE* rentNum, UINT32* Grade_sum);

private:
	/*=======================================================================================*/
	/*ʵʱ����ͳ��*/

	int QueryAddNewDevice(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count);///	�����豸������;

	int QueryAddNewAccount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count);///	�����˺�������;

	int QueryAddNewActor(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count);	///		������ɫ������;

	int QueryLogDevice(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count);	///		��¼�豸������; 

	int QueryLogAccount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count);	///		��¼�˺�������; 

	int QueryLogActor(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count);	///		��¼��ɫ������; 

	int QueryActiveDevice(UINT32 countA, UINT32 countB, UINT32* count);															///		��Ծ�豸������;

	int	QueryActiveAccount(UINT32 countA, UINT32 countB, UINT32* count);															///		��Ծ�˺�������;

	int QueryAcitveActor(UINT32 countA, UINT32 countB, UINT32* count);															///		��Ծ��ɫ������;

	//================sql���ݴ���;
	int GetAccount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, const char* keyWord, UINT32* count);	///		��ȡ�ʺ�����;

	int GetDevice(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, const char* keyWord, UINT32* count);	///		��ȡ�豸����;

	int GetActor(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, const char* keyWord, UINT32* count);	///		��ȡ��ɫ����;
	///��ѯ��¼�û�,�豸����ɫ[typ=1:�ʺŻ�Ծ����2:�豸��Ծ����3:��ɫ��Ծ��]///
	int QLoginCount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count, BYTE type = 1);		///		��ѯ��Ծ��;	


	/*=======================================================================================*/
	/*ʵʱ��ֵͳ��*/

	int QueryPaymentActor(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32& count);					///		��ѯ��ֵͳ�ơ�����ѯ�����û���;

	int QueryFirstRecharge(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32& count);				///		��ѯ��ֵͳ�ơ������������û���;

	int QueryPaymentCount(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32& count);					///		��ѯ��ֵͳ�ơ�����ѯ���Ѵ���;

	int QueryPaymentMoney(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32& count);					///		��ѯ��ֵͳ�ơ�����ѯ���ѽ��(���ո��ѽ��);

	int QueryPaymentLogin(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, float& retArpu);				///		��ѯ��ֵͳ�ơ�����ѯ��¼������;

	int QueryPaymentAdd(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, float& retArpu);					///		��ѯ��ֵͳ�ơ�����ѯע�Ḷ����;

	int QuerypaymentArpu(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, float& retArpu);				///		��ѯ��ֵͳ�ơ�����ѯ����ƽ����ARPU;

	int	QueryLoginArpu(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, float& retArpu);					///		��ѯ��ֵͳ�ơ�����ѯ��¼ƽ����ARPU;

	int ResolveTimeNum(const char* startTime, const char* endtime/*,const UINT32 from,BYTE to,char * FromTime, char*FromTotime*/);					///		��ѯ��ֵͳ�ơ�����ȡ��ѯ����;

	int GoToTime(char* STime, char* ETime, int i, bool trues);																							///		��ѯ��ֵͳ�ơ������Ʋ�ѯ����;

	int GetPaymentMoneySQL(UINT16 platId, UINT16 channelId, UINT16 partId, const char*startTime, const char* endTime, StartisticsInfo & RecharInfo);		///		��ѯ��ֵͳ�ơ��������ѯSql���;


	/*=======================================================================================*/
	/*��ѯ��������*/
	int GetOrderInfo(UINT16 partId, UINT32 from, BYTE to, const char* startTime, const char* endTime, char sql[1024 * 2]);								///		��ѯ�������顪������sql��Ϣ����;

	void GetOrderTotal(UINT16 platId, UINT16 channelId, UINT16 partId, UINT32 from, const char* start, const char* endt, UINT32* total);				///		��ѯ�������顪����ȡ��������;

	int PaymentMan(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, const char* keyWord, UINT32& count);	///		���Ѵ���;

	int CashDataMan(UINT16 platId, UINT16 channelId, UINT16 partId, BYTE operType, const char* startTime, const char* endTime, UINT32& count);		///		���Ѵ���2;

	/*=======================================================================================*/
	/*����ͳ��*/

	int GetIfLeaveKeep(UINT16 platId, UINT16 channelId, UINT16 partId, const char* stime, const char* tbName, BYTE lkTimeType, std::vector<GMQLeavekeepDateInfo>&LeaveKeepInfo, int ic);///���桪�������ѯ����;

	int GetLeaveKeep(UINT16 platId, UINT16 channelId, UINT16 partId, const char* stime, const char* tbName, BYTE lkTimeType, UINT16* Retained, int i, UINT32* NewRecount);				///	���桪����ȡ���ݴ���;

	int DeviceLeave(UINT16 platId, UINT16 channelId, UINT16 partId, BYTE operType, const char* startTime, const char* endTime, UINT32* count);									///	���桪���豸����;

	int AccountLeave(UINT16 platId, UINT16 channelId, UINT16 partId, BYTE operType, const char* startTime, const char* endTime, UINT32* count);									///	���桪���˺�����;

	int ActorLeave(UINT16 platId, UINT16 channelId, UINT16 partId, BYTE operType, const char* startTime, const char* endTime, UINT32* count);									///	���桪����ɫ����;

	int LeaveRate(UINT16 platId, UINT16 channelId, UINT16 partId, const char* stime, const char* tbName, BYTE lkTimeType, int ic, float* Getrate);									///	��ȡ������;

	/*=======================================================================================*/
	/*��������*///��.��̨���ݻ����˺�ͳ���û���
	//ʵʱ���ߡ�����������������ѯsql;
	void SqlOnelineTimeMan(UINT16 platId, UINT16 channelId, UINT16 partId, char extSql[1024], char sql[1024], const char* startTime, const char* endTime, UINT32 from, BYTE to, BYTE stype);
	//ʵʱ���ߡ�����ȡ����������¼����;
	int SqlOneLineUserMan(UINT16 platId, UINT16 channelId, UINT16 partId, const char* extSql, const char* sql, std::vector<OnlineInfo>& olInfo, UINT32* total);

	///��������ͳ��;=======================================
	//int OnlineStatistics(UINT16 platId, UINT16 partId,const char* starTime,const char* endTime, std::vector<GMOnlineDateStatisticsInfo>& OnlineStatisticsInfo, UINT32* count);

	/////��������ͳ�ƴ���;
	//int GetOnlineStatistics(UINT16 platId, UINT16 partId, const char* extSql, const char* sql, std::vector<GMOnlineDateStatisticsInfo>& olInfo, UINT32* count);

	////Sql��������ͳ�ƴ���;
	//int SqlGetOnlineStatistics(UINT16 platId, UINT16 partId,char extSql[1024], char sql[1024], const char* startTime, const char* endTime, UINT32* count);

	/////����ͳ�ƻ�ȡ����;
	//int GetSqlOlineStatistics(UINT16 platId, UINT16 partId, const char* extSql, const char* sql, std::vector<GMOnlineDateStatisticsInfo>& olInfo, UINT32* count);

	/*=======================================================================================*/
	/*�ȼ��ֲ�ͳ��*/

	int Get_Grade_RattingStatistics(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, std::vector<GM_Grade_RatingStatisticsInfo> & OnlineStatisticsInfo, BYTE* rentNum, UINT32* Grade_sum);																													///		��ȡ�ȼ��ֲ�ͳ�ƴ���;

	int GetcoutNum(UINT16 platId, UINT16 channelId, UINT16 partId, const char* startTime, const char* endTime, UINT32* Grade_sum);					///		��ȡ����;
	/*=======================================================================================*/

	void CheckTime(const char* startTime, const char* endTime, const char* keyWord, char start[125], char endt[125]);													///	ʱ����;

	void CheckTimeB(const char* startTime, const char* endTime, const char* TendTime, const char* keyWord, char start[125], char endt[125], const char* TkeyWord, char Tendt[125]);		///ʱ����2;

public:
	CUpdateDataMan& GetExtSqlObj()
	{
		return m_update;
	}

protected:
	///��ʱ��ȡ���������ص�����;���ôκ��������л�ȡ��������;
	static void GetOnlineUserByTimer(void* p, TTimerID id);
	///ʵʱ��ȡ����;
	static void GetRealTimeNewByTimer(void *p, TTimerID id);
	//ˢ������������ʱ��;m_reflushLogTimer(ReflushOnlineLogByTimer, this, 60*60*1000),
	static void ReflushOnlineLogByTimer(void* p, TTimerID id);
	//���ÿ��������߳�;
	static int	CheckEndOfEveryDayThread(void*);

	///��ʱ�������ݿ�,��������23��59��59�������ݿ�����;
	void OnCheckRunning();
	///ʵʱ�������ݿ�;�������ò������趨��
	void OnNewRuning();

	///���ڻ�ȡ��������
	void OnGetOnlineUser();

private:

	///������������
	void OnlineMan(UINT32 from, UINT16 platId, UINT16 partId, const char* extSql, const char* sql, std::vector<OnlineInfo>& olInfo, UINT32* count);
	///�׳��û�����
	int FirstRechargeMan(UINT16 platId, UINT16 partId, const char* startTime, const char* endTime, UINT32* count);
	///�����ѯ�ʺ�sql��Ϣ����;
	int GetAccountInfo(UINT16 partId, UINT32 from, BYTE to, BYTE isNow, const char* start, const char* endt, char extSql[1024 * 2], char sql[1024 * 2]);
	///�����ѯ�ʺ�sql��Ϣ����;
	int GetActorInfo(UINT16 partId, UINT32 from, BYTE to, BYTE isNow, const char* start, const char* endt, char extSql[1024 * 2], char sql[1024 * 2]);
	///��ȡ����;
	void GetTotal(UINT16 platId, UINT16 partId, UINT32 from, const char* db, const char* sql, UINT32* total);
	///��ȡ�ʺ�ͳ������;
	void GetAccountTotal(UINT16 platId, UINT16 partId, UINT32 from, const char* start, const char* endt, UINT32* total);
	///��ȡ�����豸����;
	void GetActivatedDevice(UINT32 from, const char* start, const char* endt, UINT32* total);

	///���ڴ���
	void DateMan(UINT16 platId, UINT16 partId, const char* stime, BYTE day, char lkTime[20]);

	///��ǰ��ѯ�����Ƿ�Ϊ��������;
	BOOL IsNow(const char* startTime, const char* endTime);
	///��ǰ��ѯ�����Ƿ�Ϊͬһ��
	BOOL IsSameDay(const char* startTime, const char* endTime);
	///����ʱ�䴦��
	void OnlineTimeMan(UINT16 platId, UINT16 partId, UINT32 from, BYTE to, char extSql[1024], char sql[1024], const char* startTime, const char* endTime, BYTE stype);
	///��ȡ���
	void GetExtSqlResult(std::map<UINT16, UINT32>& extRes, UINT16 partId, UINT32* count);
	///��ȡ���
	void GetExtSqlResult(std::map<UINT16, float>& extRes, UINT16 partId, float* count);
	///��ȡ���
	void GetExtSqlResult(std::map<UINT16, DBS_RESULT>& extRes, UINT16 partId, DBS_RESULT& result);

	char* tm2str(tm* pTm);

	tm* str2tm(char date[]);

	char* addDays(char date[], int days);


private:
	//��ʱ��;
	CyaCoreTimer m_olineTimer;						//��������Timer,��ʱ��ȥ����������ʱ��;
	CyaCoreTimer m_reflushLogTimer;				//ˢ��logTimer
	CyaCoreTimer m_realTimeNew;					//ʵʱ������ʱ��;
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
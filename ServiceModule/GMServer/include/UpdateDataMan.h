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


	///���µ������ݲ�д�����ݿ�
	void UpdateAddupNewData(const char* updateTime);

	///�����ʺ�����
	void UpdateAccountData(UINT16 platid, UINT16 ChannelId, const char* updateTime);
	///���½�ɫ����
	void UpdateActorData(UINT16 platid, UINT16 ChannelId, const char* updateTime);
	///������������
	void UpdateLeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* updateTime);
	///���µ����ʺŻ��豸������
	void UpdateAddupNewAccountMan(UINT16 platid, UINT16 ChannelId, const char* updateTime, const char* keyWord, BYTE addType);
	///���µ��ս�ɫ������
	void UpdateAddupNewActorMan(UINT16 platid, UINT16 ChannelId, const char* updateTime);
	///���¶�ʱˢ��������������
	void UpdateCommOnlineMan(UINT16 platid, UINT16 ChannelId, const char* updateTime);
	///�������������������
	void UpdateMaxOnlineMan(UINT16 platid, UINT16 ChannelId, const char* stime);
	///����ƽ��������������
	void UpdateAverageOnlineMan(UINT16 platid, UINT16 ChannelId, const char* stime);
	///���¸������ݴ���
	void UpdatePaymentMan(UINT16 platid, UINT16 ChannelId, const char* stime);

public:
	///ִ��sql(��������, partId=0:��ȡƽ̨���з���, partId��Ϊ0��ʾָ������)
	int ExecuteNewQuerySql(UINT16 ChannelId, UINT16 partId, DBS_MSG_TYPE dsType, const char* db, const char* sql, std::map<UINT16, UINT32>& getCount, BYTE isAffect);
	///sql ִ�У�������洦��, partId=0:��ȡƽ̨���з���, partId��Ϊ0��ʾָ��������
	int ExecuteNewQuerySql(UINT16 ChannelId, UINT16 partId, DBS_MSG_TYPE dsType, const char* db, const char* sql, std::map<UINT16, float>& getCount);
	///ִ��sql(�����ؽ��������Ҫ�ֶ���ȡ, partId=0:��ȡƽ̨���з���, partId��Ϊ0��ʾָ������)
	int ExecuteNewQuerySql(UINT16 ChannelId, UINT16 partId, DBS_MSG_TYPE dsType, const char* db, const char* sql);
	///��ֵ���ݿ�ִ��sql����
	int ExecuteNewRechargeDbQuerySql(DBS_MSG_TYPE dsType, const char* db, const char* sql, UINT32* count);
	///��ֵ���ݿ�ִ��sql����
	int ExecuteNewRechargeDbQuerySql(DBS_MSG_TYPE dsType, const char* db, const char* sql, DBS_RESULT& result);
	///�ʺ����ݿ�ִ��sql����
	int ExecuteNewAccountDbQuerySql(DBS_MSG_TYPE dsType, const char* db, const char* sql, UINT32* count);
	///�ʺ����ݿ�ִ��sql����
	int ExecuteNewAccountDbQuerySql(DBS_MSG_TYPE dsType, const char* db, const char* sql, DBS_RESULT& result);

	///���ִ��sql��õĽ����
	void ClearNewDBSResult();
	///��ȡִ��sql�����
	std::map<UINT16, DBS_RESULT>& GetNewDBSResult()
	{
		return m_dbsNewResult;
	}

private:
	///��ֵ����ִ��sql����
	int ExecuteNewRechargeQuerySql(DBSHandle handle, DBS_MSG_TYPE dsType, const char* db, const char* sql);
	///��ֵ����ִ��sql����
	int ExecuteNewRechargeQuerySql(DBSHandle handle, DBS_MSG_TYPE dsType, const char* db, const char* sql, UINT32* count);
	///���ڴ���
	void DateMan(UINT16 ChannelId, const char* registTime, BYTE day, char lkTime[20]);



private:
	///ƽ̨����ע���û�������
	void GetPlatRegistCount(UINT16 platid/*ƽ̨*/, UINT16 ChannelId, BYTE lktType/*����ʱ������*/, const char* regTime/*ע������*/, std::map<UINT16/*����ID*/, UINT32/*ע����*/>& reg);
	///ƽ̨������¼�û�������
	void GetPlatLoginCount(UINT16 platId, UINT16 ChannelId, BYTE lktType, const char* regTime, const char* lgTime/*��¼ʱ��*/, std::map<UINT16/*����ID*/, UINT32/*ע����*/>& lg);
	///���յ�¼�û�������
	void GetTodayLoginCount(UINT16 platId, UINT16 ChannelId, const char* updateTime, std::map<UINT16/*����ID*/, UINT32/*ע����*/>& todayLg);
	///ƽ̨������¼�豸������
	void GetPlatLoginDeviceCount(UINT16 ChannelId, BYTE lktType, const char* regTime, const char* lgTime, std::map<UINT16/*����ID*/, UINT32/*��¼�豸��*/>& lgDevice);
	///���յ�¼�豸������
	void GetTodayLoginDeviceCount(UINT16 ChannelId, const char* updateTime, std::map<UINT16/*����ID*/, UINT32/*���յ�¼�豸��*/>& todayLgDevice);
	///ƽ̨������ɫ����������
	void GetPlatCreateActorCount(UINT16 ChannelId, BYTE lktType, const char* regTime, std::map<UINT16/*����ID*/, UINT32/*������ɫ��*/>& createActor);
	///ƽ̨������¼��ɫ������
	void GetPlatLoginActorCount(UINT16 ChannelId, BYTE lktType, const char* regTime, const char* lgTime, std::map<UINT16/*����ID*/, UINT32/*��¼��ɫ��*/>& lgActor);

	///���յ�¼��ɫ������
	void GetTodayLoginActorCount(UINT16 ChannelId, const char* updateTime, std::map<UINT16/*����ID*/, UINT32/*���յ�¼��ɫ��*/>& todayLgActor);
	///�����������ݴ���
	int OnlineDataMan(SOCKET sockt, const char* key/*���ݱ�ؼ��ֶ���*/, UINT32* count/*��������*/);
	///��ֵ���ݿ����ݴ���
	void RechargeDataMan(UINT16 ChannelId, UINT16 partId, BYTE operType, const char* updateTime, UINT32* count/*������Ӧ�ĳ�ֵ����*/);
	///�׳��ɫ������
	void FirstRechargeMan(UINT16 ChannelId, UINT16 partId, DBSHandle handle, const char* updateTime, UINT32* count/*�׳�����*/);

private:
	///�ڲ����洦��[lkType:1-�ʺ�����, 2-�豸����, 3-��ɫ����]
	int LeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* registTime/*ע������ʱ��*/, const char* updateTime/*��������ʱ��*/, BYTE day, BYTE lktType);
	///�ʺ����洦��
	int AccountLeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* registTime, const char* updateTime, BYTE day, BYTE lktType);
	///�豸���洦��
	int DeviceLeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* registTime, const char* updateTime, BYTE day, BYTE lktType);
	///��ɫ���洦��
	int ActorLeaveKeepMan(UINT16 platid, UINT16 ChannelId, const char* registTime, const char* updateTime, BYTE day, BYTE lktType);

private:
	///�������ݴ���db
	int StoreLeaveKeepIntoDb(UINT16 platid, UINT16 ChannelId, UINT16 partId, BYTE lkType, const char* stime, const char* updateTime, UINT32 rgCount, UINT32 lgCount, UINT32 currLgCount, float lkRate, BYTE lktType);
	///д����������д��db[stime:д��ʱ��, addType:д������(1-�ʺ�����, 2-�豸����, 3-��ɫ����)]
	void StoreAddupNewIntoDb(UINT16 platid, UINT16 ChannelId, UINT16 partId, const char* stime, UINT32 count, BYTE addType, BYTE dataType/*���ݱ��ֶ��е���������*/);
	///��������д��db[oltime:д��ʱ��, olType:����(1-��ͨ����, 2-�������, 3-ƽ������)]
	int StoreOnlineIntoDb(UINT16 platid, UINT16 ChannelId, UINT16 partId, const char* oltime, UINT32 count, BYTE olType);

private:

	///��������
	struct LeaveDate
	{
		char date[11]; //��������
	};

	typedef std::vector<LeaveDate> LeaveDateVec;
	std::map<UINT16, DBS_RESULT> m_dbsNewResult;	//db �����
	CXTLocker m_dbLocker;
};

#endif
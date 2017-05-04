#ifndef __PARTITION_INFO_MAN_H__
#define __PARTITION_INFO_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
分区管理类
*/

#include <map>
#include "cyaTypes.h"
#include "cyaLocker.h"
#include "cyaRefCount.h"
#include "cyaCoreTimer.h"
#include "ServiceExtMan.h"
#include "ServiceLicence.h"

class LicenceSession;
class PartitionInfoMan
{
private:
	class PartitionSite : public BaseRefCount1	//分区信息
	{
		struct LinkerInfo	//在线分区link服务器信息
		{
			BYTE status;		//linker状态(0-正常,1-维护)
			UINT32 linkerIp;	//link服务器IP
			UINT16 linkerPort;	//link服务器端口
			UINT32 connections;	//linker服务器当前连接数
			UINT32 tokens;		//token数
			LicenceSession* session; //登陆到验证服务器的linker连接

			LinkerInfo()
			{
				status = 0;
				linkerIp = 0;
				linkerPort = 0;
				connections = 0;
				tokens = 0;
				session = NULL;
			}
		};

	public:
		PartitionSite();
		~PartitionSite();

		int LinkerLogin(BYTE linkerId, LicenceSession* session, UINT32 linkerIp, UINT16 linkerPort);
		int LinkerLogout(BYTE linkerId);
		int UpdateLinkerConnections(BYTE linkerId, UINT32 connections, UINT32 tokens);

		int StartAllLinkerServe();
		int StopAllLinkerServe();

		int GetMinPayloadLinker(BYTE& linkerId);
		int EnterLinkerGetToken(BYTE linkerId, UINT32 userId, const char* username);
		int GetLinkerAddr(BYTE linkerId, UINT32& linkerIp, UINT16& linkerPort);
		int NotifyUserOffline(BYTE exceptLinkerId, UINT32 userId);
		int QueryUnlockJobs(UINT32 userId);

		void CheckHeartbeat();
		void DelAllLinkers();

		BYTE GetStatus() const
		{
			return m_status;
		}

		const char* GetName() const
		{
			return m_name.c_str();
		}

		BOOL IsRecommend() const
		{
			return m_isRecommend;
		}

		void SetName(const char* name)
		{
			m_name = name;
		}

		void SetStatus(BYTE status)
		{
			m_status = status;
		}

		void SetRecommendFlag(BOOL isRecommend)
		{
			m_isRecommend = isRecommend;
		}

		UINT32 GetRegistAccountNum() const
		{
			return m_accountNum;
		}

		void SetRegistAccountNum(UINT32 num)
		{
			m_accountNum = num;
		}

		BOOL HasLinkerLogin()
		{
			return m_linkersMap.empty() ? false : true;
		}

	private:
		std::string m_name;			//分区名称
		BYTE   m_status;			//分区状态(0-正常,1-维护)
		BOOL   m_isRecommend;		//是否推荐
		UINT32 m_accountNum;		//分区账号数量
		CXTLocker m_linkersMapLocker;
		std::map<BYTE/*分区linker id*/, LinkerInfo> m_linkersMap;	//分区linker表
	};

public:
	PartitionInfoMan();
	~PartitionInfoMan();

	//载入分区信息
	BOOL LoadPartitionInfo();
	//卸载分区信息
	void UnLoadPartitionInfo();
	//linker登陆
	int LinkerLogin(UINT16 partitionId, BYTE linkerId, LicenceSession* session, UINT32 linkerIp, UINT16 linkerPort);
	//linker退出
	int LinkerLogout(UINT16 partitionId, BYTE linkerId);
	//获取分区状态
	int GetPartitionStatus(UINT16 partitionId, BYTE& status);
	//是否推荐分区
	BOOL IsRecommendPartition(UINT16 partitionId);
	//更新linker连接数
	int UpdateLinkerConnections(UINT16 partitionId, BYTE linkerId, UINT32 connections, UINT32 tokens);
	//开启分区linker
	int StartPartitionLinkerServe(UINT16 partitionId);
	//停止分区linker
	int StopPartitionLinkerServe(UINT16 partitionId);
	//获取分区内负载最小的linker
	int GetMinPayloadLinker(UINT16 partitionId, BYTE& linkerId);
	//从linker获取token
	int UserEnterLinkerGetToken(UINT16 partitionId, BYTE linkerId, UINT32 userId, const char* username);
	//获取分区linker地址信息
	int GetPartitionLinkerAddr(UINT16 partitionId, BYTE linkerId, UINT32& linkerIp, UINT16& linkerPort);
	//获取分区信息表
	int GetPartitionInfoTable(UINT16 from, UINT16 num, SUserQueryPartitionRes* pQueryRes);
	//通知linker踢用户下线
	int NotifyUserFromLinkerOffline(UINT16 partitionId, BYTE exceptLinkerId, UINT32 userId);
	//获取推荐分区id
	UINT16 GetRecommendPartitionId();
	//获取最小注册用户分区
	void GetMinAccountPartition(UINT16& partId, UINT32& accountNum);
	//查询账号已解锁职业
	int QueryUnlockJobs(UINT16 partId, UINT32 userId);
	//更新分区注册用户数量
	void UpdatePartitionUsers(UINT16 partId);

public:
	//查询游戏分区
	int QueryGamePartition(UINT16 from, BYTE num, SExtManQueryGamePartitionRes* pRes);
	//增加分区
	int AddPartition(const char* name, const char* lic, BYTE isRecommend, UINT16& newPartId);
	//修改分区名字
	int ModifyPartitionName(UINT16 partitionId, const char* newName);
	//设置推荐分区
	int SetRecommendPartition(UINT16 partitionId);
	//删除分区
	int DelPartition(UINT16 partitionId);
	//获取分区名称
	int GetPartitionName(UINT16 partitionId, char* pszName);

private:
	PartitionSite* GetPartitionSite(UINT16 partitionId);
	static void LinkerHeartTimer(void* param, TTimerID id);
	void OnHeartCheck();

private:
	CyaCoreTimer m_linkerHeartTimer;
	CXTLocker m_partitionInfoMapLocker;
	std::map<UINT16/*分区id*/, PartitionSite*> m_partitionInfoMap;	//分区信息表
};

BOOL InitPartitionInfoMan();
PartitionInfoMan* GetPartitionInfoMan();
void DestroyPartitionInfoMan();

#endif
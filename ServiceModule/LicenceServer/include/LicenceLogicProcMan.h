#ifndef __LICENCE_LOGIC_PROC_MAN_H__
#define __LICENCE_LOGIC_PROC_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
验证服务器逻辑处理
*/

#include <map>
#include "cyaTypes.h"

class LicenceSession;
class LicenceLogicProcMan
{
public:
	LicenceLogicProcMan();
	~LicenceLogicProcMan();

public:
	void LinkerLogout(UINT32 partitionId, BYTE linkerId);
	void UserLogout(UINT32 userId);
	int  ProcessLogic(LicenceSession* licSession, UINT16 cmdCode, const void* payload, int nPayloadLen, BYTE pktType);

private:
	typedef int (LicenceLogicProcMan::*fnLgProcessHandler)(LicenceSession* licSession, const void* payload, int nPayloadLen);
	std::map<UINT16, fnLgProcessHandler> m_lgProcHandler;

private:
	//link服务器登录
	int OnLinkerLogin(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//用户登陆
	int OnUserLogin(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//查询分区状态
	int OnQueryPartitionStatus(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//上报linker当前连接数
	int OnReportLinkerConnections(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//获取上报linker连接数时间间隔
	int OnGetLinkerReportTimer(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//从link服务器获取token响应包
	int OnLinkGetTokenResPkt(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//用户进入分区
	int OnUserEnterPartition(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//用户查询分区信息
	int OnUserQueryPartitionInfo(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//通知其他linker上的用户下线
	int OnNotifyLinkerUserOffline(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//拥护创建或删除角色
	int OnCreateOrDelRole(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//查询用户登陆信息
	int OnQueryUserLoginInfo(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//注册用户
	int OnRegistUser(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//查询已解锁职业
	int OnQueryUnlockJobs(LicenceSession* licSession, const void* payload, int nPayloadLen);
	//从link服务器获取解锁职业响应包
	int OnLinkerQueryUnlockJobsResPkt(LicenceSession* licSession, const void* payload, int nPayloadLen);
};

void InitLicenceLogicProcMan();
LicenceLogicProcMan* GetLicenceLogicProcMan();
void DestroyLicenceLogicProcMan();

#endif
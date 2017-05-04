#ifndef __USER_INFO_MAN_H__
#define __USER_INFO_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
用户信息管理类
*/

#include <map>
#include "cyaLocker.h"
#include "cyaCoreTimer.h"
#include "ServiceLicence.h"
#include "LCSProtocol.h"

class LicenceSession;
class UserInfoMan
{
public:
	UserInfoMan();
	~UserInfoMan();

	//用户登陆
	int  UserLogin(LicenceSession* session,
		const char* username,
		const char* password,
		const char* platform,
		const char* os,
		const char* devId,
		const char* model,
		const char* ver,
		BYTE mode, UINT32& userId);
	//用户退出
	void UserLogout(UINT32 userId);
	//通过用户id获取用户名,上次登陆分区
	int  GetUsernameByUserId(std::string& username, UINT16& enterPartId, UINT32 userId);
	//发送用户进入分区回复
	int  SendUserEnterLinkerReply(UINT16 partitionId, UINT32 userId, const char* token, UINT32 linkerIp, UINT16 linkerPort, int retCode);
	//设置用户进入分区信息
	int  SetUserEnterPartitionLinkerId(UINT32 userId, UINT16 partitionId, BYTE linkerId, LTMSEL nowMSel);
	//分区linker断线
	void PartitionLinkerBroken(UINT16 partitionId, BYTE linkerId);

	//更新用户登陆游戏信息
	void UpdateUserLoginGameInfo(UINT32 userId, UINT16 partId, LTMSEL nowMSel);
	//查询用户最近登陆信息
	int QueryUserLatestLoginInfo(UINT32 userId, SUserQueryLoginRes* pQueryRes);
	//注册用户
	int RegistUser(const SUserRegistReq* req);
	//查询已解锁职业
	int QueryUserUnlockJobs(UINT32 userId);
	//发送用户查询解锁职业回复
	int SendUserQueryUnlockJobsReply(const SLinkerQueryUnlockJobsRes* pUnlockJobsRes);

private:
	int LocalUserPswVerify(const char* username, const char* psw, UINT32& userId, UINT16& lastPartId, LTMSEL& lasttts);
	int ThrirdUserPswVerify(const char* username,
		const char* psw,
		const char* pszDevId,
		const char* pszModel,
		const char* pszVer,
		const char* pszPlat,
		const char* pszOS,
		UINT32& userId, UINT16& lastPartId, LTMSEL& lasttts);

private:
	struct UserInfo
	{
		BYTE mode;	//验证模式
		BYTE enterLinkerId;		//要进入的分区linker编号(0-还未进行进入分区的操作)
		std::string username;	//账号名
		//std::string platform;	//登录平台
		LTMSEL lastMSel;		//最后一次登陆时间
		UINT16 lastPartId;		//最后一次登陆分区id
		UINT16 enterPartId;		//要进入的分区
		LicenceSession* session;	//用户连接会话

		UserInfo()
		{
			mode = 1;
			enterLinkerId = 0;
			username = "";
			//platform = "";
			lastMSel = 0;
			lastPartId = 0;
			enterPartId = 0;
			session = NULL;
		}
	};

	CXTLocker m_locker;
	std::map<UINT32, UserInfo> m_userInfoMap;
};

void InitUserInfoMan();
UserInfoMan* GetUserInfoMan();
void DestroyUserInfoMan();

#endif
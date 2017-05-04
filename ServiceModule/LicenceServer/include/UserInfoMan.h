#ifndef __USER_INFO_MAN_H__
#define __USER_INFO_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�û���Ϣ������
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

	//�û���½
	int  UserLogin(LicenceSession* session,
		const char* username,
		const char* password,
		const char* platform,
		const char* os,
		const char* devId,
		const char* model,
		const char* ver,
		BYTE mode, UINT32& userId);
	//�û��˳�
	void UserLogout(UINT32 userId);
	//ͨ���û�id��ȡ�û���,�ϴε�½����
	int  GetUsernameByUserId(std::string& username, UINT16& enterPartId, UINT32 userId);
	//�����û���������ظ�
	int  SendUserEnterLinkerReply(UINT16 partitionId, UINT32 userId, const char* token, UINT32 linkerIp, UINT16 linkerPort, int retCode);
	//�����û����������Ϣ
	int  SetUserEnterPartitionLinkerId(UINT32 userId, UINT16 partitionId, BYTE linkerId, LTMSEL nowMSel);
	//����linker����
	void PartitionLinkerBroken(UINT16 partitionId, BYTE linkerId);

	//�����û���½��Ϸ��Ϣ
	void UpdateUserLoginGameInfo(UINT32 userId, UINT16 partId, LTMSEL nowMSel);
	//��ѯ�û������½��Ϣ
	int QueryUserLatestLoginInfo(UINT32 userId, SUserQueryLoginRes* pQueryRes);
	//ע���û�
	int RegistUser(const SUserRegistReq* req);
	//��ѯ�ѽ���ְҵ
	int QueryUserUnlockJobs(UINT32 userId);
	//�����û���ѯ����ְҵ�ظ�
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
		BYTE mode;	//��֤ģʽ
		BYTE enterLinkerId;		//Ҫ����ķ���linker���(0-��δ���н�������Ĳ���)
		std::string username;	//�˺���
		//std::string platform;	//��¼ƽ̨
		LTMSEL lastMSel;		//���һ�ε�½ʱ��
		UINT16 lastPartId;		//���һ�ε�½����id
		UINT16 enterPartId;		//Ҫ����ķ���
		LicenceSession* session;	//�û����ӻỰ

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
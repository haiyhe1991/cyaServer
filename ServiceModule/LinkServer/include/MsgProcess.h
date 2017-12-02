/*
* Session��������Ϣ�ĳ����
* User <--> Session <--> Role
* ����Session����һ��������������Ϣ�ظ�
*/

#ifndef __MSG_PROCESS_H__
#define __MSG_PROCESS_H__

#define INVALID_USER_ID 0
#define INVALID_ROLE_ID 0

#include <map>
#include "cyaLocker.h"
#include "ChatClient.h"
#include "MLogicClient.h"
#include "cyaRefCount.h"
#include "CyaObject.h"
#include "GWSSDK.h"

class MsgProcess
{
	DECLAR_FETCH_OBJ(MsgProcess)
public:
	/// session��Ϣ�������
	/// GWS��Ϣ�첽д�룬����ʱ
	int MsgProcessSite(const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID, bool nEncrypt);
	/// ֪ͨ��֤���������û���½/�ǳ��ɹ���׼������
	int NotifyLicenseUserLogin(UINT32 nUserID);
	int NotifyLicenseUserExit(UINT32 nUserID);
	int NotifyChatUserExit(UINT32 nUserID);

	/// ͨ��USER_ID��Session
	/// @param repeat:�Ƿ����ظ���½���Ѿ������ظ�������֪ͨ��֤���������ˡ�
	bool RegistSession(void* pSessionSite, UINT32 nUserID, bool& repeat);
	void UnRegistSession(void* pSessionSite, UINT32 nUserID);

	/// ��ɫID���û�IDΪһһ��Ӧ����ɫID�ظ����޸�
	int RegistRole(UINT32 nUserID, UINT32 roleID);
	void UnRegistRole(UINT32 nUserID);
	UINT32 FindRoleIDByUserID(UINT32 nUserID);

	/// ��������Session���ݻظ�
	void InputToSessionMsg(UINT32 nUserID, UINT16 nPktType, UINT16 nCmdCode, UINT16 nRetCode, const void* pPktData, UINT16 nPktLen);
	void InputToSessionError(UINT16 nCmdCode, UINT16 nRetCode, UINT16 nPktType, UINT32 nUserID);

	/// ���ˣ��ظ���½��ʱ��ǰһ�����ߡ�
	void KickSession(UINT32 nUserID);

	/// Ⱥ��Ϣ���߼��������������͵ġ�
	void GroupMsg(UINT16 nPktType, UINT16 nCmdCode, UINT16 nRetCode, const void* pPktData, UINT16 nPktLen);


	/// ��ȡ��ǰLinker��������
	UINT32 GetOnlineUserNum();

private:
	static void GWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen, void* context);
	void OnGwsDataCallBack(SGWSProtocolHead* pHead, const void* payload, int payloadLen);

	void InitProc();
	void FiniProc();
	/// ����
	void OnCleanUserMap();
	void OnDestroyClientPtr(BaseClient*& p);

private:
	MsgProcess();
	~MsgProcess();

	class UserInfo : public BaseRefCount1
	{
	public:
		UserInfo(void* session);
		~UserInfo();

		UINT32 FetchRoleID() const;
		void SetRoleID(UINT32 roleId);
		void FlushHeartBeatTime();
		bool SessionBroken(DWORD timeOut) const;
		void* GetSession() const;
		virtual void DeleteObj();
	private:
		UINT32 m_roleid;
		LTMSEL m_time;
		void* m_session;
	};

	typedef std::map<UINT32, UserInfo*> UserMap;
private:
	BaseClient* m_pMLogicClient;
	BaseClient* m_pChatClient;
	BaseClient* m_pRankClient;
	BaseClient* m_pMauClient;
	GWSHandle m_gwsHandle;
	UserMap m_users;
	CXTLocker m_mapUserLocker;
};

#endif
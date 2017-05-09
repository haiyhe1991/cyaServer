/*
* Session、网关消息的出入口
* User <--> Session <--> Role
* 所有Session保存一个副本，用于消息回复
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
	/// session消息处理入口
	/// GWS消息异步写入，不耗时
	int MsgProcessSite(const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID, bool nEncrypt);
	/// 通知验证服务器，用户登陆/登出成功，准备踢人
	int NotifyLicenseUserLogin(UINT32 nUserID);
	int NotifyLicenseUserExit(UINT32 nUserID);
	int NotifyChatUserExit(UINT32 nUserID);

	/// 通过USER_ID绑定Session
	/// @param repeat:是否有重复登陆？已经发现重复，则不用通知验证服务器踢人。
	bool RegistSession(void* pSessionSite, UINT32 nUserID, bool& repeat);
	void UnRegistSession(void* pSessionSite, UINT32 nUserID);

	/// 角色ID和用户ID为一一对应，角色ID重复则修改
	int RegistRole(UINT32 nUserID, UINT32 roleID);
	void UnRegistRole(UINT32 nUserID);
	UINT32 FindRoleIDByUserID(UINT32 nUserID);

	/// 给关联的Session数据回复
	void InputToSessionMsg(UINT32 nUserID, UINT16 nPktType, UINT16 nCmdCode, UINT16 nRetCode, const void* pPktData, UINT16 nPktLen);
	void InputToSessionError(UINT16 nCmdCode, UINT16 nRetCode, UINT16 nPktType, UINT32 nUserID);

	/// 踢人，重复登陆的时候，前一个被踢。
	void KickSession(UINT32 nUserID);

	/// 群消息，逻辑服务器主动推送的。
	void GroupMsg(UINT16 nPktType, UINT16 nCmdCode, UINT16 nRetCode, const void* pPktData, UINT16 nPktLen);


	/// 获取当前Linker在线人数
	UINT32 GetOnlineUserNum();

private:
	static void GWSDataCallback(SGWSProtocolHead* pHead, const void* payload, int payloadLen, void* context);
	void OnGwsDataCallBack(SGWSProtocolHead* pHead, const void* payload, int payloadLen);

	void InitProc();
	void FiniProc();
	/// 清理
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
	GWSHandle m_gwsHandle;
	UserMap m_users;
	CXTLocker m_mapUserLocker;
};

#endif
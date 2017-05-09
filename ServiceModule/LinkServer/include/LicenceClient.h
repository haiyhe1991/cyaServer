#ifndef __LICENCE_CLIENT_H__
#define __LICENCE_CLIENT_H__

#include "LscSessionMaker.h"
#include "LscSession.h"
#include "cyaSync.h"
#include "cyaCoreTimer.h"

class LicenceClient
{
public:
	LicenceClient();
	~LicenceClient();

	enum LINK_ON_OFF_STATUS
	{
		LINK_STATUS_ON = 0,
		LINK_STATUS_OFF = 1,
		LINK_STATUS_UNKOWN = 2
	};

	/// 创建与验证服务器的连接  dwConTimeOut:超时时间 bReConnect:是否重连 dwInterval:重连间隔
	int ConnectLicenceServer(DWORD dwConTimeOut = 5000, BOOL bReConnect = true, DWORD dwInterval = 1000);
	int ReConnectLicenceServer();
	/// 登录验证服务器
	int LogInLicenceServer(SOCKET sockLogin);
	/// 获取分区状态
	int GetLinkStatus(BYTE& cLkStatus);
	/// 向验证服务器报告角色创建 nUerID:用户ID
	int ReportRoleCreated(UINT32 nUerID);
	/// 向验证服务器报告角色删除 nUerID:用户ID
	int ReportRoleDeleted(UINT32 nUerID);
	/// 通知账户下线
	void ReportUserExit(UINT32 nUerID);
	/// 通知验证服务器用户登陆成功
	int ReportUserLogin(UINT32 nUerID);

	void SetLocalLSCSession(LSCSession* pSession);
	/// 掉线通知
	void LscSessionBroken();

	/// 与验证服务器的连接状态
	bool LSClientStatus() const;
	/// 获取心跳(上报连接数)时间间隔
	int GetReportTimer();

	/// 向验证服务器发送数据接口
	int SendCtrlData(UINT16 cmdCode, UINT16 retCode, const void* payload, UINT16 payloadLen);

private:
	/// 验证服务器消息回调
	static int LscClientMsgData(bool bEncrypt, UINT16 nPktType, void* pMsgData, int nMsgLen, void* context);

	/// 令牌包入口(DATA_TOKEN_KEY_PACKET)
	int OnKeyPacket(void* pMsgData, int nMsgLen);
	/// 数据包入口(PAKCET_DATA)
	int OnDataPacket(void* pMsgData, int nMsgLen);
	/// 控制包入口(DATA_CTRL_PACKET)
	int OnContrlPacket(void* pMsgData, int nMsgLen);

	/*  DATA_TOKEN_KEY_PACKET   */
	/// 获取密钥回复
	int OnKeyConnectMsg(const void* pMsgData, int MsgLen);

	/*  PAKCET_DATA */
	/// 登录验证服务器回复
	int OnDataLoginMsg(UINT16 nRetCode);
	/// 获取LinkServer状态回复
	int OnDataStatusMsg(UINT16 nRetCode, BYTE cLkStatus);
	/// 获取心跳间隔回复
	int OnGetReportTimer(UINT16 nRetCode, UINT32 theTime);

	/*  DATA_CTRL_PACKET    */
	/// 验证服务器获取Token
	int OnContrlGetToken(void* pMsgData, int nMsgLen);
	/// 验证服务器启动LinkServer
	int OnContrlStartLinker();
	/// 验证服务器停止LinkServer
	int OnContrlStopLinker();
	/// 接收到踢人消息
	int OnContrlKickUser(void* pMsgData, int nMsgLen);

	/// 向验证服务器上报连接数
	int OnConnectsMsg();


private:
	/// 重连定时器
	static void ReconnectTimer(void* pParam, TTimerID id);
	void OnReconnect();

	/// 上报连接数量定时器, 默认为5秒上报一次，没有数量变化，或者LinkServer关闭，则不用报
	static void RePortConsTimer(void* pParam, TTimerID id);
	void OnRePortCons();

	/// 消息回调处理函数
	int OnLscClientMsgData(bool bEncrypt, UINT16 nPktType, void* pMsgData, int nMsgLen);
	/// 上报角色删除或创建
	int ReportRoleChanged(UINT32 nUerID, BYTE cModel);
	/// 发送数据到验证服务器
	int InputSessinData(const void* pSessionData, int nDataLen);

	BOOL WaitSema(DWORD dwWaitTimeOut = 5000);
	void PostSema();
private:
	LSCSessionMaker* m_pLscSessionMaker;
	ICyaTcpServeMan* m_pLscSessionMan;
	LSCSession* m_pLscSession;  /// 与Client对应的Session

	DWORD m_dwConTimeOut;       /// 连接验证服务器的超时时间
	BOOL m_bReConEnable;        /// 是否断线重连
	DWORD m_dwReConInterval;    /// 断线重连时间间隔
	BOOL m_bLoginOk;            /// 是否登录成功
	BYTE m_cLinkStatus;         /// LinkServer状态

	CyaCoreTimer m_iReConTimer;    /// 断线重连Timer
	CyaCoreTimer m_iReportTimer;   /// 上报连接数Timer
	int m_nLinkSesssionNum;         /// 保存LinkServer连接数，没有变化就不用上报

	UINT32 m_nPayloadToken;     /// 通信协议Token
	char m_szPayloadKey[DEFAULT_KEY_SIZE + 1];    /// 通信数据密钥
	OSSema m_iSema; ///消息接收为异步，用信号量同步
};

#endif
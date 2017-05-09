#include "LicenceClient.h"
#include "cyaSock.h"
#include "cyaLog.h"
#include "cyaIpCvt.h"
#include "cyaTcp.h"

#include "AES.h"

#include "LCSProtocol.h"
#include "ServiceErrorCode.h"

#include "PubFunction.h"
#include "LinkGlobalObj.h"
#include "LinkConfig.h"
#include "LinkServer.h"
#include "TokenMan.h"
#include "MsgProcess.h"

LicenceClient::LicenceClient()
: m_pLscSessionMaker(new LSCSessionMaker())
, m_pLscSessionMan(NULL)
, m_pLscSession(NULL)
, m_dwConTimeOut(5000)  /// 默认超时5秒
, m_bReConEnable(true)  /// 默认为启动断线重连
, m_dwReConInterval(1000) /// 默认断线重连间隔1秒
, m_bLoginOk(false)     /// 登陆标志
, m_cLinkStatus(LINK_STATUS_UNKOWN)         /// LinkServer连接状态未知
, m_iReConTimer(ReconnectTimer, this, 1000) /// 重连Timer
, m_iReportTimer(RePortConsTimer, this, 5000)   /// 上报LinkServer连接数Timer
, m_nLinkSesssionNum(-1)
{
	ASSERT(m_pLscSessionMaker);
	if (m_pLscSessionMaker)
	{
		CyaTcp_MakeServeMan(m_pLscSessionMan, m_pLscSessionMaker, 64);
	}
	OSSemaInit(&m_iSema);
	m_iReportTimer.SetPeriod(GlobalGetLinkConfig()->GetLSReportInvterval() * 1000); /// 设置上报间隔，从配置文件获取
}

LicenceClient::~LicenceClient()
{
	m_bLoginOk = false;
	m_bReConEnable = false;
	if (m_iReportTimer.IsStarted())
		m_iReportTimer.Stop();

	if (m_iReConTimer.IsStarted())
		m_iReConTimer.Stop();

	if (m_pLscSessionMan)
	{
		m_pLscSessionMan->DeleteThis();
		m_pLscSessionMan = NULL;
	}
	if (m_pLscSessionMaker)
	{
		delete m_pLscSessionMaker;
		m_pLscSessionMaker = NULL;
	}
	OSSemaDestroy(&m_iSema);
}

void LicenceClient::ReconnectTimer(void* pParam, TTimerID id)
{
	LicenceClient* pThis = (LicenceClient*)pParam;
	if (pThis)
		return pThis->OnReconnect();
}

void LicenceClient::OnReconnect()
{
	if (m_bLoginOk)
		return;
	if (LINK_OK != ConnectLicenceServer(m_dwConTimeOut, m_bReConEnable, m_dwReConInterval))
	{
		m_bLoginOk = FALSE;
		if (NULL != m_pLscSession)
		{
			m_pLscSession->GetServeMan()->CloseSession(m_pLscSession);
			m_pLscSession = NULL;
		}
		return;
	}
	m_iReConTimer.Stop(FALSE);
	if (!m_iReportTimer.IsStarted())
		m_iReportTimer.Start();
}

void LicenceClient::LscSessionBroken()
{
	LogDebug(("LsClient掉线"));
	m_pLscSession = NULL;
	if (m_bLoginOk)
		m_bLoginOk = false;

	if (m_bReConEnable && !m_iReConTimer.IsStarted())
	{
		m_iReConTimer.Start();
		LogDebug(("LsClient启动重连"));
	}

	if (m_iReportTimer.IsStarted())
	{
		m_iReportTimer.Stop();
	}
}

bool LicenceClient::LSClientStatus() const
{
	return NULL != m_pLscSession && m_bLoginOk;
}

#if SEPARATOR("Connect LicenceServer")
int LicenceClient::ConnectLicenceServer(DWORD dwConTimeOut/*=5000*/, BOOL bReConnect/*=true*/, DWORD dwInterval/*=1000*/)
{
	SOCKET s_sock = INVALID_SOCKET;
	SGSProtocolHead* head = NULL;
	char headBuf[64] = { 0 };
	char pck[1024] = { 0 };
	UINT16 nLen = 0;

	if (NULL == m_pLscSessionMan)
		return LINK_ERROR;

	s_sock = TCPNewClientByTime(GetDWordIP(GlobalGetLinkConfig()->GetLSAddr()), GlobalGetLinkConfig()->GetLSPort(), dwConTimeOut);
	if (!SockValid(s_sock))
	{
		LogInfo(("连接验证服务器( %s:%d )失败", GlobalGetLinkConfig()->GetLSAddr(), GlobalGetLinkConfig()->GetLSPort()));
		goto CONNECT_EXIT;
	}

	if (sizeof(SGSProtocolHead) != SockReadAllByTime(s_sock, headBuf, sizeof(SGSProtocolHead), dwConTimeOut))   /// 是否收到验证服务器的消息回复
	{
		LogInfo(("验证服务器( %s:%d ) 没有回复消息", GlobalGetLinkConfig()->GetLSAddr(), GlobalGetLinkConfig()->GetLSPort()));
		goto CONNECT_EXIT;
	}
	head = (SGSProtocolHead*)headBuf;
	head->ntoh();
	nLen = head->pduLen;

	if (nLen != SockReadAllByTime(s_sock, pck, nLen, dwConTimeOut) ||       /// 是接收验证服务器的令牌消息
		head->pktType != DATA_TOKEN_KEY_PACKET ||                                       /// 是否为令牌包
		LINK_OK != OnLscClientMsgData(head->isCrypt, head->pktType, pck, head->pduLen) ||   /// 返回的令牌包是否正确
		LINK_OK != LogInLicenceServer(s_sock) ||                                        /// 发送登陆命令是否成功
		sizeof(SGSProtocolHead) != SockReadAllByTime(s_sock, headBuf, sizeof(SGSProtocolHead), dwConTimeOut))       /// 是否收到数据回复
	{
		LogInfo(("登陆服务器消息( %s:%d )失败", GlobalGetLinkConfig()->GetLSAddr(), GlobalGetLinkConfig()->GetLSPort()));
		goto CONNECT_EXIT;
	}

	head = (SGSProtocolHead*)headBuf;
	head->ntoh();
	nLen = head->pduLen;

	if (nLen != SockReadAllByTime(s_sock, pck, head->pduLen, dwConTimeOut) ||       /// 收到登陆回复
		head->pktType != DATA_PACKET ||                                                 /// 是否数据包
		LINK_OK != OnLscClientMsgData(head->isCrypt, head->pktType, pck, head->pduLen) ||   /// 验证返回的登陆包
		!m_bLoginOk)                                                                        /// 登陆是否成功
	{
		LogInfo(("登陆证服务器( %s:%d )失败", GlobalGetLinkConfig()->GetLSAddr(), GlobalGetLinkConfig()->GetLSPort()));
		goto CONNECT_EXIT;
	}

	m_pLscSessionMan->TrustSession(s_sock, this, true);
	m_dwConTimeOut = dwConTimeOut;
	if (NULL == m_pLscSession)
	{
		SockClose(s_sock);
		if (m_bReConEnable && !m_iReConTimer.IsStarted())
			m_iReConTimer.Start();
		return LINK_ERROR;
	}
	m_pLscSession->SetLscClientCallBack(LscClientMsgData, this);

	m_bReConEnable = bReConnect;
	m_dwReConInterval = dwInterval;
	m_iReConTimer.SetPeriod(dwInterval);

	GetReportTimer();
	if (!m_iReportTimer.IsStarted())
	{
		m_iReportTimer.Start();
	}

	return LINK_OK;
CONNECT_EXIT:
	SockClose(s_sock);
	if (bReConnect && !m_iReConTimer.IsStarted())
		m_iReConTimer.Start();
	return LINK_CONNECT_LS;
}

int LicenceClient::ReConnectLicenceServer()
{
	if (NULL != m_pLscSessionMan && NULL != m_pLscSession)
	{
		m_pLscSessionMan->CloseSession(m_pLscSession);
	}
	return LINK_OK;
}

int LicenceClient::LogInLicenceServer(SOCKET sockLogin)
{
	BYTE loginBuf[512] = { 0 };
	memset(loginBuf, 0, sizeof(loginBuf));

	UINT16 unAlignLen = SGS_PROTO_HEAD_LEN + SGS_REQ_HEAD_LEN + LOGINLS_HSIZE;  //未对齐原始数据长度
	UINT16 enlignedLen = GetAlignedLen(true, unAlignLen, 16);           /// 16字节对齐后的数据长度
	UINT16 data_boffset = SGS_PROTO_HEAD_LEN + SGS_REQ_HEAD_LEN;    /// 登录信息数据区偏移量

	DWORD dwLsAddr = GetDWordIP(GlobalGetLinkConfig()->GetLSAddr());
	UINT16 wLsPort = GlobalGetLinkConfig()->GetLSPort();

	BYTE iLinkNum = (BYTE)GlobalGetLinkConfig()->GetLinkNumber();
	DWORD dwLkAddr = GetDWordIP(GlobalGetLinkConfig()->GetLinkAddr());
	UINT16 wLkPort = GlobalGetLinkConfig()->GetLinkListenPort();

	const char* strLsToken = GlobalGetLinkConfig()->GetLSToken();
	UINT16 wPatitionId = GlobalGetLinkConfig()->GetLinkerPartitionID();

	CreateLinkLoginLsReq(loginBuf + data_boffset, wPatitionId, dwLsAddr, wLsPort, iLinkNum, dwLkAddr, wLkPort, strLsToken);     /// Token
	CreateProtocolPacket(loginBuf, true, DATA_PACKET, enlignedLen);
	UINT16 cmdCode = LICENCE_LINKER_LOGIN;
	CreateEncryptReqPayLoad(m_nPayloadToken, loginBuf + SGS_PROTO_HEAD_LEN, loginBuf + data_boffset,
		LOGINLS_HSIZE, cmdCode, (BYTE*)m_szPayloadKey);

	return SockWriteAllByTime(sockLogin, loginBuf, enlignedLen + SGS_PROTO_HEAD_LEN, m_dwConTimeOut) > 0 ? LINK_OK : LINK_LOGIN_LS;
}

int LicenceClient::GetLinkStatus(BYTE& cLkStatus)
{
	BYTE statusBuf[512] = { 0 };
	memset(statusBuf, 0, sizeof(statusBuf));
	UINT16 unAlignLen = SGS_PROTO_HEAD_LEN + SGS_REQ_HEAD_LEN;
	UINT16 enlignedLen = GetAlignedLen(true, unAlignLen, 16);

	CreateProtocolPacket(statusBuf, true, DATA_PACKET, enlignedLen);
	UINT16 cmdCode = LICENCE_LINKER_QUERY_PARTITION_STATUS;
	CreateEncryptReqPayLoad(m_nPayloadToken, (BYTE*)statusBuf + SGS_PROTO_HEAD_LEN, NULL, 0, cmdCode, (BYTE*)m_szPayloadKey);

	if (LINK_OK == InputSessinData(statusBuf, enlignedLen + SGS_PROTO_HEAD_LEN) && WaitSema(m_dwConTimeOut))/// 超时内收到状态查询回复
	{
		cLkStatus = m_cLinkStatus;
		return LINK_OK;
	}

	return LINK_STATUS;
}
#endif

void LicenceClient::SetLocalLSCSession(LSCSession* pSession)
{
	m_pLscSession = pSession;
}

int LicenceClient::InputSessinData(const void* pSessionData, int nDataLen)
{
	if (NULL == m_pLscSession)
		return LINK_ERROR;
	return m_pLscSession->GetServeMan()->SendSessionData(m_pLscSession, pSessionData, nDataLen);
}

int LicenceClient::SendCtrlData(UINT16 cmdCode, UINT16 retCode, const void* payload, UINT16 payloadLen)
{
	UINT16 uDataLen = 0;
	UINT16 uEncrLen = 0;
	if (NULL != payload)
	{
		if (0 == payloadLen)
		{
			LogDebug(("数据长度错误"));
			return LINK_ERROR;
		}
		uDataLen = (SGS_RES_HEAD_LEN + payloadLen) > 4096 ? (4096 - SGS_RES_HEAD_LEN) : payloadLen;
	}
	uEncrLen = uDataLen + SGS_RES_HEAD_LEN;

	// uSendLen = uAlignedLen + SGS_PROTO_HEAD_LEN; 所以如下定义数组大小
	BYTE Buf[4096 + SGS_PROTO_HEAD_LEN] = { 0 };
	UINT16 uAlignedLen = GetAlignedLen(true, uEncrLen, 16);

	CreateProtocolPacket(Buf, true, DATA_CTRL_PACKET, uAlignedLen);
	SGSResPayload* pResPayload = (SGSResPayload*)(Buf + SGS_PROTO_HEAD_LEN);
	pResPayload->cmdCode = cmdCode;
	pResPayload->retCode = retCode;
	pResPayload->rawDataBytes = uEncrLen; // 等于SGS_RES_HEAD_LEN + payloadLen
	pResPayload->token = 0;
	pResPayload->hton();

	if (NULL != payload)
	{
		memcpy(pResPayload->data, payload, uDataLen);
	}

	appEncryptDataWithKey((AES_BYTE*)(Buf + SGS_PROTO_HEAD_LEN), uEncrLen, (char*)m_szPayloadKey);
	return InputSessinData(Buf, uAlignedLen + SGS_PROTO_HEAD_LEN);
}

BOOL LicenceClient::WaitSema(DWORD dwWaitTimeOut/* = 5000*/)
{
	return OSSemaWait(&m_iSema, dwWaitTimeOut);
}

void LicenceClient::PostSema()
{
	OSSemaPost(&m_iSema);
}

#if SEPARATOR("LicenceServer msg process")
int LicenceClient::LscClientMsgData(bool bEncrypt, UINT16 nPktType, void* pMsgData, int nMsgLen, void* context)
{
	LicenceClient* pThis = (LicenceClient*)context;
	if (pThis)
		return pThis->OnLscClientMsgData(bEncrypt, nPktType, pMsgData, nMsgLen);

	return LINK_OK;
}

// 收到LicenseServer消息的处理
int LicenceClient::OnLscClientMsgData(bool bEncrypt, UINT16 nPktType, void* pMsgData, int nMsgLen)
{
	if (bEncrypt)
	{
		appDecryptDataWithKey((AES_BYTE*)pMsgData, nMsgLen, (ANSICHAR*)(DATA_TOKEN_KEY_PACKET == nPktType ? DEFAULT_TOKEN_KEY : m_szPayloadKey));
	}

	if (DATA_TOKEN_KEY_PACKET == nPktType)          /// 密钥包
	{
		return OnKeyPacket(pMsgData, nMsgLen);
	}
	else if (DATA_PACKET == nPktType)               /// 数据包
	{
		return OnDataPacket(pMsgData, nMsgLen);
	}
	else if (DATA_CTRL_PACKET == nPktType)          /// 控制包
	{
		return OnContrlPacket(pMsgData, nMsgLen);
	}

	return LINK_OK;
}

int LicenceClient::OnKeyPacket(void* pMsgData, int nMsgLen)
{
	ASSERT(NULL != pMsgData && SGS_RES_HEAD_LEN < nMsgLen);
	SGSResPayload* ResPayload = (SGSResPayload*)pMsgData;
	ResPayload->ntoh();
	m_nPayloadToken = ResPayload->token;
	if (ResPayload->retCode != 0)
		return ResPayload->retCode;

	return OnKeyConnectMsg(ResPayload->data, ResPayload->rawDataBytes - SGS_RES_HEAD_LEN);
}

int LicenceClient::OnDataPacket(void* pMsgData, int nMsgLen)
{
	ASSERT(NULL != pMsgData && nMsgLen >= SGS_RES_HEAD_LEN);
	SGSResPayload* ResPayload = (SGSResPayload*)pMsgData;
	ResPayload->ntoh();
	m_nPayloadToken = ResPayload->token;
	if (LICENCE_LINKER_LOGIN == ResPayload->cmdCode)
	{
		return OnDataLoginMsg(ResPayload->retCode);
	}
	else if (LICENCE_LINKER_QUERY_PARTITION_STATUS == ResPayload->cmdCode)
	{
		return OnDataStatusMsg(ResPayload->retCode, ResPayload->data[0]);
	}
	else if (LICENCE_LINKER_REPORT_CONNECTIONS == ResPayload->cmdCode)
	{
		return OnConnectsMsg();
	}
	else if (LICENCE_GET_REPORT_TIMER == ResPayload->cmdCode)
	{
		SLinkerGetReportTimerRes* p = (SLinkerGetReportTimerRes*)ResPayload->data;
		p->ntoh();
		return OnGetReportTimer(ResPayload->retCode, p->timerSec);
	}

	return LINK_OK;
}

int LicenceClient::OnContrlPacket(void* pMsgData, int nMsgLen)
{
	ASSERT(nMsgLen >= SGS_RES_HEAD_LEN);
	SGSReqPayload* ReqPayload = (SGSReqPayload*)pMsgData;
	ReqPayload->ntoh();
	if (LCS_START_LINKER_SERVE == ReqPayload->cmdCode)
	{
		return OnContrlStartLinker();
	}
	else if (LCS_STOP_LINKER_SERVE == ReqPayload->cmdCode)
	{
		return OnContrlStopLinker();
	}
	else if (LICENCE_LINKER_GET_TOKEN == ReqPayload->cmdCode)
	{
		return OnContrlGetToken(ReqPayload->data, ReqPayload->rawDataBytes - SGS_REQ_HEAD_LEN);
	}
	else if (LICENCE_NOTIFY_USER_OFFLINE == ReqPayload->cmdCode)
	{
		return OnContrlKickUser(ReqPayload->data, ReqPayload->rawDataBytes - SGS_REQ_HEAD_LEN);
	}
	else if (LICENCE_QUERY_UNLOCK_JOBS == ReqPayload->cmdCode)
	{
		MsgProcess* pMsgPrc = MsgProcess::FetchMsgProcess();
		ASSERT(NULL != pMsgPrc);

		//LogInfo(("查询已解锁职业"));
		pMsgPrc->MsgProcessSite(ReqPayload->data, ReqPayload->rawDataBytes - SGS_REQ_HEAD_LEN, LICENCE_QUERY_UNLOCK_JOBS, DATA_PACKET, 0, false);
	}

	return LINK_OK;
}

int LicenceClient::OnKeyConnectMsg(const void* pMsgData, int MsgLen)
{
	if (NULL == pMsgData || 0 >= MsgLen)
	{
		LogInfo(("验证服务器返回令牌包格式不对"));
		return LINK_ERROR;
	}
	ASSERT(DEFAULT_KEY_SIZE >= MsgLen);
	memcpy(m_szPayloadKey, pMsgData, MsgLen);
	m_szPayloadKey[DEFAULT_KEY_SIZE] = '\0';
	LogInfo(("连接验证服务器( %s:%d )成功",
		GlobalGetLinkConfig()->GetLSAddr(),
		GlobalGetLinkConfig()->GetLSPort()
		));
	//PostSema();
	return LINK_OK;
}

int LicenceClient::OnDataLoginMsg(UINT16 nRetCode)
{
	m_bLoginOk = (0 == nRetCode ? true : false);
	LogInfo(("登录验证服务器( %s:%d )%s(%d)",
		GlobalGetLinkConfig()->GetLSAddr(),
		GlobalGetLinkConfig()->GetLSPort(),
		m_bLoginOk ? "成功" : "失败", nRetCode
		));
	//PostSema();
	return nRetCode;
}

int LicenceClient::OnDataStatusMsg(UINT16 nRetCode, BYTE cLkStatus)
{
	if (0 == nRetCode)
	{
		m_cLinkStatus = cLkStatus;
		LogInfo(("Link服务器状态: %s", m_cLinkStatus == 0 ? "开启" : "关闭"));
	}
	else
	{
		LogInfo(("查询服务器状态失败"));
	}
	PostSema();
	return nRetCode;
}

int LicenceClient::OnGetReportTimer(UINT16 nRetCode, UINT32 theTime)
{
	if (0 == nRetCode)
	{
		m_iReportTimer.SetPeriod(theTime * 1000);   /// 设置上报间隔，从配置文件获取
		LogInfo(("上报连接数时间间隔: %d 秒", theTime));
	}
	else
	{
		LogInfo(("获取上报连接数时间间隔失败"));
	}
	return nRetCode;
}

int LicenceClient::OnContrlGetToken(void* pMsgData, int nMsgLen)
{
	ASSERT(NULL != pMsgData && sizeof(SLinkerGetUserTokenReq) == nMsgLen);
	SLinkerGetUserTokenReq* ReqPayload = (SLinkerGetUserTokenReq*)pMsgData;
	ReqPayload->ntoh();
	std::string strToken = TokenMan::FetchTokenMan()->CreateToken(ReqPayload->userId);

	BYTE resBuf[512] = { 0 };
	memset(resBuf, 0, sizeof(resBuf));
	BYTE payloadToken[DEFAULT_TOKEN_SIZE + 1] = { 0 };
	memcpy(payloadToken, &m_nPayloadToken, DEFAULT_TOKEN_SIZE);
	payloadToken[DEFAULT_TOKEN_SIZE] = '\0';

	UINT16 cmdCode = LICENCE_LINKER_GET_TOKEN;
	if (strToken.empty())
	{
		UINT16 unAlignedLen = RES_HEAD_SIZE;
		UINT16 enAlignedLen = GetAlignedLen(true, unAlignedLen, 16);

		CreateProtocolPacket(resBuf, true, DATA_CTRL_PACKET, enAlignedLen);
		CreateEncryptResPayLoad(payloadToken, resBuf + SGS_PROTO_HEAD_LEN, NULL, 0, cmdCode, LINK_ERROR, (BYTE*)m_szPayloadKey);
		InputSessinData(resBuf, SGS_PROTO_HEAD_LEN + RES_HEAD_SIZE); // 应该为 InputSessinData(resBuf, enAlignedLen + SGS_PROTO_HEAD_LEN);
	}
	else
	{
		UINT16 unAlignedLen = SGS_RES_HEAD_LEN + sizeof(SLinkerGetUserTokenRes);
		UINT16 enAlignedLen = GetAlignedLen(true, unAlignedLen, 16);
		UINT16 data_offset = SGS_PROTO_HEAD_LEN + SGS_RES_HEAD_LEN;

		CreateProtocolPacket(resBuf, true, DATA_CTRL_PACKET, enAlignedLen);
		CreateTokenRes(resBuf + data_offset, ReqPayload->userId, strToken.c_str());

		UINT16 payloadLen = sizeof(SLinkerGetUserTokenRes);
		CreateEncryptResPayLoad(payloadToken, resBuf + SGS_PROTO_HEAD_LEN, resBuf + data_offset, payloadLen, cmdCode, 0, (BYTE*)m_szPayloadKey);
		InputSessinData(resBuf, enAlignedLen + SGS_PROTO_HEAD_LEN);
	}

	LogDebug(("验证服务器获取验证Token为%s", strToken.c_str()));
	return LINK_OK;
}

int LicenceClient::OnContrlStartLinker()
{
	GlobalGetLinkServer()->InitLinkServer();
	int r = GlobalGetLinkServer()->StartLinkServer();

	BYTE resBuf[128] = { 0 };
	memset(resBuf, 0, sizeof(resBuf));
	BYTE payloadToken[DEFAULT_TOKEN_SIZE + 1] = { 0 };
	memcpy(payloadToken, &m_nPayloadToken, DEFAULT_TOKEN_SIZE);
	payloadToken[DEFAULT_TOKEN_SIZE] = '\0';

	UINT16 unAlignedLen = RES_HEAD_SIZE;
	UINT16 enAlignedLen = GetAlignedLen(true, unAlignedLen, 16);

	CreateProtocolPacket(resBuf, true, DATA_CTRL_PACKET, enAlignedLen);
	UINT16 nRetCode = (0 == r ? LINK_OK : LINK_START);
	UINT16 cmdCode = LCS_START_LINKER_SERVE;
	CreateEncryptResPayLoad(payloadToken, resBuf + SGS_PROTO_HEAD_LEN, NULL, 0, cmdCode, nRetCode, (BYTE*)m_szPayloadKey);
	InputSessinData(resBuf, enAlignedLen + SGS_PROTO_HEAD_LEN);
	LogInfo(("验证服务器控制启动LinkServer %s", 0 == r ? "成功" : "失败"));

	return LINK_OK;
}

int LicenceClient::OnContrlStopLinker()
{
	GlobalGetLinkServer()->StopLinkServer();
	GlobalGetLinkServer()->UnitLinkServer();
	bool link_status = GlobalGetLinkServer()->ServerStarted();

	BYTE resBuf[128] = { 0 };
	memset(resBuf, 0, sizeof(resBuf));
	BYTE payloadToken[DEFAULT_TOKEN_SIZE + 1] = { 0 };
	memcpy(payloadToken, &m_nPayloadToken, DEFAULT_TOKEN_SIZE);
	payloadToken[DEFAULT_TOKEN_SIZE] = '\0';

	UINT16 unAlignedLen = RES_HEAD_SIZE;
	UINT16 enAlignedLen = GetAlignedLen(true, unAlignedLen, 16);

	CreateProtocolPacket(resBuf, true, DATA_CTRL_PACKET, enAlignedLen);
	UINT16 nRetCode = link_status ? LINK_STOP : LINK_OK;
	UINT16 cmdCode = LCS_STOP_LINKER_SERVE;
	CreateEncryptResPayLoad(payloadToken, resBuf + SGS_PROTO_HEAD_LEN, NULL, 0, cmdCode, nRetCode, (BYTE*)m_szPayloadKey);

	InputSessinData(resBuf, enAlignedLen + SGS_PROTO_HEAD_LEN);
	LogInfo(("验证服务器控制关闭LinkServer %s", link_status ? "失败" : "成功"));

	return LINK_OK;
}

int LicenceClient::OnContrlKickUser(void* pMsgData, int nMsgLen)
{
	ASSERT(NULL != pMsgData && sizeof(SLinkerNotifyUserOffline) == nMsgLen);
	SLinkerNotifyUserOffline* pNotify = (SLinkerNotifyUserOffline*)pMsgData;
	pNotify->ntoh();
	if (pNotify->linkerId == GlobalGetLinkConfig()->GetLinkNumber())    /// 本服务器不用踢
	{
		return LINK_OK;
	}

	MsgProcess::FetchMsgProcess()->KickSession(pNotify->userId);

	return LINK_OK;
}

int LicenceClient::OnConnectsMsg()
{
	//LogDebug(("收到连接数报告回复"));
	return LINK_OK;
}

int LicenceClient::GetReportTimer()
{
	BYTE getBuf[512] = { 0 };
	memset(getBuf, 0, sizeof(getBuf));
	UINT16 unAlignLen = SGS_PROTO_HEAD_LEN + SGS_REQ_HEAD_LEN;
	UINT16 enlignedLen = GetAlignedLen(true, unAlignLen, 16);

	CreateProtocolPacket(getBuf, true, DATA_PACKET, enlignedLen);
	UINT16 cmdCode = LICENCE_GET_REPORT_TIMER;
	CreateEncryptReqPayLoad(m_nPayloadToken, (BYTE*)getBuf + SGS_PROTO_HEAD_LEN, NULL, 0, cmdCode, (BYTE*)m_szPayloadKey);

	InputSessinData(getBuf, enlignedLen + SGS_PROTO_HEAD_LEN);

	return 0;
}

void LicenceClient::RePortConsTimer(void* pParam, TTimerID id)
{
	LicenceClient* pThis = (LicenceClient*)pParam;
	if (pThis)
		pThis->OnRePortCons();
}

void LicenceClient::OnRePortCons()
{
	LinkServer* links = GlobalGetLinkServer();
	/// LinkServer关闭状态、连接数量没变 不用报告，
	if (NULL == links || !links->ServerStarted())
		m_nLinkSesssionNum = 0;
	else
		m_nLinkSesssionNum = links->GetSessionsCount();

	BYTE conBuf[512] = { 0 };
	memset(conBuf, 0, sizeof(conBuf));
	UINT16 unAlignedLen = SGS_REQ_HEAD_LEN + CONNECT_HSIZE;
	UINT16 enAlignedLen = GetAlignedLen(true, unAlignedLen, 16);
	UINT16 data_offset = SGS_PROTO_HEAD_LEN + SGS_REQ_HEAD_LEN;

	CreateLinkSessinsReq(conBuf + data_offset, m_nLinkSesssionNum, 0);
	CreateProtocolPacket(conBuf, true, DATA_PACKET, enAlignedLen);
	UINT16 cmdCode = LICENCE_LINKER_REPORT_CONNECTIONS;
	CreateEncryptReqPayLoad(m_nPayloadToken, conBuf + SGS_PROTO_HEAD_LEN, conBuf + data_offset, CONNECT_HSIZE, cmdCode, (BYTE*)m_szPayloadKey);

	InputSessinData(conBuf, enAlignedLen + SGS_PROTO_HEAD_LEN);
}
#endif

#if SEPARATOR("User RoleOperationMsg Process for LogicClient")
int LicenceClient::ReportRoleChanged(UINT32 nUerID, BYTE cModel)
{
	BYTE roleBuf[512] = { 0 };
	memset(roleBuf, 0, sizeof(roleBuf));
	UINT16 unAlignedLen = SGS_REQ_HEAD_LEN + ROLECHG_HSIZE;
	UINT16 enAlignedLen = GetAlignedLen(true, unAlignedLen, 16);
	UINT16 data_offset = SGS_PROTO_HEAD_LEN + SGS_REQ_HEAD_LEN;

	CreateRoleChanged(roleBuf + data_offset, nUerID, cModel);
	CreateProtocolPacket(roleBuf, true, DATA_PACKET, enAlignedLen);
	UINT16 cmdCode = LICENCE_LINKER_ROLE_CHANGE;
	CreateEncryptReqPayLoad(m_nPayloadToken, roleBuf + SGS_PROTO_HEAD_LEN, roleBuf + data_offset, ROLECHG_HSIZE, cmdCode, (BYTE*)m_szPayloadKey);

	InputSessinData(roleBuf, enAlignedLen + SGS_PROTO_HEAD_LEN);
	return 0;
}

int LicenceClient::ReportRoleCreated(UINT32 nUerID)
{
	LogInfo(("用户(%d)角色创建", nUerID));
	return ReportRoleChanged(nUerID, REPORT_ROLE_CREATE);
}

int LicenceClient::ReportRoleDeleted(UINT32 nUerID)
{
	LogInfo(("用户(%d)角色删除", nUerID));
	return ReportRoleChanged(nUerID, REPORT_ROLE_DELETE);
}

void LicenceClient::ReportUserExit(UINT32 nUerID)
{
	BYTE reportBuf[512] = { 0 };
	memset(reportBuf, 0, sizeof(reportBuf));
	UINT16 unAlignedLen = SGS_REQ_HEAD_LEN + sizeof(SLinkerNotifyUserOffline);
	UINT16 enAlignedLen = GetAlignedLen(true, unAlignedLen, 16);
	UINT16 notifyOffset = SGS_PROTO_HEAD_LEN + SGS_REQ_HEAD_LEN;

	SLinkerNotifyUserOffline* pNotify = (SLinkerNotifyUserOffline*)(reportBuf + notifyOffset);
	pNotify->userId = nUerID;
	pNotify->linkerId = GlobalGetLinkConfig()->GetLinkNumber();
	pNotify->hton();

	CreateProtocolPacket(reportBuf, true, DATA_PACKET, enAlignedLen);
	UINT16 pktLen = sizeof(SLinkerNotifyUserOffline);
	UINT16 cmdCode = LICENCE_NOTIFY_USER_OFFLINE;
	CreateEncryptReqPayLoad(m_nPayloadToken, reportBuf + SGS_PROTO_HEAD_LEN, pNotify, pktLen, cmdCode, (BYTE*)m_szPayloadKey);

	InputSessinData(reportBuf, enAlignedLen + SGS_PROTO_HEAD_LEN);
}

int LicenceClient::ReportUserLogin(UINT32 nUerID)
{
	BYTE reportBuf[512] = { 0 };
	memset(reportBuf, 0, sizeof(reportBuf));
	UINT16 unAlignedLen = SGS_REQ_HEAD_LEN + sizeof(SLinkerNotifyUserCheck);
	UINT16 enAlignedLen = GetAlignedLen(true, unAlignedLen);
	UINT16 notifyOffset = SGS_PROTO_HEAD_LEN + SGS_REQ_HEAD_LEN;

	SLinkerNotifyUserCheck* pNotify = (SLinkerNotifyUserCheck*)(reportBuf + notifyOffset);
	pNotify->userId = nUerID;
	pNotify->hton();

	CreateProtocolPacket(reportBuf, true, DATA_PACKET, enAlignedLen);
	UINT16 pktLen = sizeof(SLinkerNotifyUserCheck);
	UINT16 cmdCode = LICENCE_LINKER_USER_CHECK;
	CreateEncryptReqPayLoad(m_nPayloadToken, reportBuf + SGS_PROTO_HEAD_LEN, pNotify, pktLen, cmdCode, (BYTE*)m_szPayloadKey);

	InputSessinData(reportBuf, enAlignedLen + SGS_PROTO_HEAD_LEN);
	return 0;
}
#endif
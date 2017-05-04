#include "GMSdkHelper.h"
#include "cyaIpcvt.h"
#include "AES.h"
#include "ServiceProtocol.h"
#include "GMComm.h"
#include <vector>
#include "GMGlobalObjMan.h"
#include "GMSdkSession.h"
#include "cyaSock.h"
#include "ServiceCmdCode.h"
#include "gb_utf8.h"
#include "ServiceErrorCode.h"
#include <fstream>

#define ADD_FUNC_HANDLE_MAN__(stype, func) \
	m_funcHanlde.insert(std::make_pair(stype, &CGMSdkHelper::func));

CGMSdkHelper::CGMSdkHelper(
	const char* gmsIp,
	UINT16 gmsPort,
	UINT16 connTimeout/* = 5000*/,
	UINT16 readTimeout/* = 0*/,
	UINT16 writeTimeout/* = 0*/
	)
	:m_requence(0),
	m_gmsPort(gmsPort),
	m_connTimeout(connTimeout),
	m_pLocalSession(NULL),
	m_isLogin(false),
	m_reconnectTimer(ReconnectTimer, this, 3 * 1000),
	m_heartbeatTimer(HeartbeatTimer, this, m_heartbeatInterval * 1000),
	m_heartbeatTime(GetTickCount()),
	m_heartbeatInterval(0)
{
	memset(m_gmsIp, 0, 17);
	memset(m_key, 0, DEFAULT_KEY_SIZE);
	strcpy(m_gmsIp, gmsIp);
	strcpy(m_key, DEFAULT_TOKEN_KEY);
	memset(&m_hearbeatHead, 0, GET_GM_PROTOHEAD_SIZE);
	GM_PROTO_HEAD_DATA_MAN1__(m_hearbeatHead, GET_GM_PROTOHEAD_SIZE, DATA_HEARTBEAT_PACKET, 0, 0, 1);
}

CGMSdkHelper::~CGMSdkHelper()
{

}

void CGMSdkHelper::InitFunc()
{
	//ADD_FUNC_HANDLE_MAN__(ACCOUNT_CREATE, QueryCurrentDayAddNewAccountNum)
	//ADD_FUNC_HANDLE_MAN__(ADD_NEW_USER, QueryCurrentDayAddNewActorNum)
	//ADD_FUNC_HANDLE_MAN__(LOGIN_USER, QueryCurrentDayLoginActorNum)
	//ADD_FUNC_HANDLE_MAN__(LOGIN_USER, QueryCurrentDayLoginActorNum)
}

int CGMSdkHelper::Login()
{
	//创建socket连接,返回拿到socket号,将ip ,端口，超时连接的时间，
	SOCKET sockt = TCPNewClientByTime(GetIPByName(m_gmsIp), m_gmsPort, m_connTimeout);
	//检查是否连接上,如果为连接则返回1;
	if (!SockValid(sockt))
	{
		printf("SOCKET创建失败！\n");
		m_reconnectTimer.Start();
	}
	char headBuf[65] = { 0 };
	char buf[1024] = { 0 };
	GMProtocalHead* phead = NULL;

	//指定长度数据接收完成;
	int ret = SockReadAllByTime(sockt, headBuf, GET_GM_PROTOHEAD_SIZE, 5000);
	if (ret <= 0)
	{
		printf("SockReadAllByTime发生错误！\n");
		return 1;
	}
	phead = (GMProtocalHead*)headBuf;
	ASSERT(phead != NULL);
	phead->ntoh();
	//指定长度数据接收完成;指定接收数据;
	ret = SockReadAllByTime(sockt, buf, phead->pduLen, 3000);
	if (ret <= 0)
	{
		return 1;
		printf("SockReadAllByTime2发生错误\n");
	}
	if (phead->isCrypt)

		//数据解密;
		appDecryptDataWithKey((AES_BYTE*)buf, phead->pduLen, m_key);

	//响应数据结构体对象，将解密的数据存放到回应数据中,用来存放服务器返回的数据;
	SGSResPayload* ppreqPlayload = (SGSResPayload*)buf;
	ASSERT(ppreqPlayload != NULL);
	ppreqPlayload->ntoh();

	//判断是否是命令包及密钥，判断返回码是否为0;
	if (phead->pktType == DATA_TOKEN_KEY_PACKET && ppreqPlayload->retCode == 0)
	{
		//从连接后服务器返回来的响应数据中获取key和心跳间隔时间;
		GMConntedTokenHeart* p = (GMConntedTokenHeart*)ppreqPlayload->data;
		ASSERT(p != NULL);
		//存放心跳和Key;
		printf("初始Key值：%d\n", m_key);
		p->ntoh();
		memcpy(m_key, p->key, DEFAULT_KEY_SIZE);
		printf("服务器发送过来的Key:%d\n", m_key);

		//获取心跳时间间隔;
		m_heartbeatInterval = p->heatbeatInterval;
	}
	else
		return 1;

	CXTAutoLock locked(m_locker);
	if (m_heartbeatInterval > 0)
	{
		m_heartbeatTimer.SetPeriod(m_heartbeatInterval * 1000);
		//启动定时器;
		if (m_heartbeatTimer.Start())
			printf("定时器开启成功！\n");
	}

	GetGMGlobalObjMan()->GetServeMan()->TrustSession(sockt, this, true);
	m_heartbeatTime = GetTickCount();
	m_isLogin = true;
	printf("登录：m_isLogin=true  ret:%d\n", ret);
	return 0;
}

void CGMSdkHelper::Loginout()
{
	m_isLogin = false;

	if (m_reconnectTimer.IsStarted())
		m_reconnectTimer.Stop(false);

}

//查询统计实时新增数据
TimeAddedinfo* CGMSdkHelper::QueryRealTimeAddStatistics(UINT16 platId, UINT16 channel, UINT16 partId, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total)
{
	if (!m_isLogin)
	{
		*retCode = GM_CONN_SERVER_FAILED;
		return NULL;
	}

	BYTE buf[1024] = { 0 };
	UINT16 uncrptLen = GET_SGSPAYLOAD_REQ_SIZE + GET_ANY_STRUC_SIZE(GMRealTimeAddedStatisticsReq, 0);
	UINT16 cryptLen = BytesAlign(uncrptLen);
	DWORD req = AddOneForRequence();
	BYTE isCrypt = 1;


	GMRealTimeAddedStatisticsReq* p = (GMRealTimeAddedStatisticsReq*)(buf + GET_GM_PROTOHEAD_SIZE + GET_SGSPAYLOAD_REQ_SIZE);
	p->partId = partId;
	p->channelId = channel;
	p->platId = platId;
	strcpy(p->startTime, startTime);
	strcpy(p->endTime, endTime);
	p->hton();

	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, cryptLen, DATA_PACKET, isCrypt, req, 1);
	SGSReqPayload* payload = (SGSReqPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_REQ_DATA_MAN__(payload, uncrptLen, 0, GM_NEW_STATISTICS);

	BYTE* sbuf = (BYTE*)buf;

	EncryptPakcge(isCrypt, sbuf, uncrptLen, GET_GM_PROTOHEAD_SIZE, m_key, &cryptLen);
	void* cBuff = NULL;
	UINT32 pLen = 0;
	SendDataMan(GM_NEW_STATISTICS, req, buf, cryptLen, 1, cBuff, &pLen);
	if (IsNull(cBuff))
		return 0;
	if (isCrypt)
		appDecryptDataWithKey((AES_BYTE*)cBuff, pLen, m_key);

	ASSERT(cBuff != NULL);
	SGSResPayload* pres = (SGSResPayload*)cBuff;
	pres->ntoh();
	TimeAddedinfo* retData = NULL;
	int nSize = sizeof(TimeAddedinfo);
	nSize;
	retData = new TimeAddedinfo;
	if (pres->cmdCode == GM_NEW_STATISTICS && pres->retCode == 0)
	{
		GMRealTImeAddedStAllNumRes* poactorDetailRes = (GMRealTImeAddedStAllNumRes*)pres->data;
		poactorDetailRes->InfoNum.ntoh();
		retData->EquipmentAddedNum = poactorDetailRes->InfoNum.EquipmentAddedNum;
		retData->AccountAddedNum = poactorDetailRes->InfoNum.AccountAddedNum;
		retData->ActorAddedNum = poactorDetailRes->InfoNum.ActorAddedNum;
		retData->EquipmentLogInNum = poactorDetailRes->InfoNum.EquipmentLogInNum;
		retData->AccountLogInNum = poactorDetailRes->InfoNum.AccountLogInNum;
		retData->ActorLogInNum = poactorDetailRes->InfoNum.ActorLogInNum;
		retData->EquipmentActiveNum = poactorDetailRes->InfoNum.EquipmentActiveNum;
		retData->AccountActiveNum = poactorDetailRes->InfoNum.AccountActiveNum;
		retData->ActorActiveNum = poactorDetailRes->InfoNum.ActorActiveNum;
	}
	else
	{
		*retCode = GM_QUERY_NEWSTATISTICS_FAILED;
		*total = 0;
	}

	return retData;
}


///查询充值订单详情;
RecharegeStatisticeIofo* CGMSdkHelper::QueryOrderTracking(UINT16 platId, UINT16 channel, UINT16 partId, UINT32 from, BYTE to, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total)
{
	if (!m_isLogin)
	{
		*retCode = GM_CONN_SERVER_FAILED;
		return NULL;
	}

	BYTE buf[1024] = { 0 };
	UINT16 uncrptLen = GET_SGSPAYLOAD_REQ_SIZE + GET_ANY_STRUC_SIZE(GMGetOrderTrackingRsq, 0);
	UINT16 cryptLen = BytesAlign(uncrptLen);
	DWORD req = AddOneForRequence();
	BYTE isCrypt = 1;


	GMGetOrderTrackingRsq* p = (GMGetOrderTrackingRsq*)(buf + GET_GM_PROTOHEAD_SIZE + GET_SGSPAYLOAD_REQ_SIZE);
	strcpy(p->startTime, startTime);
	strcpy(p->endTime, endTime);
	p->partId = partId;
	p->channelId = channel;
	p->platId = platId;
	p->from = from;
	p->to = to;
	p->hton();

	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, cryptLen, DATA_PACKET, isCrypt, req, 1);
	SGSReqPayload* payload = (SGSReqPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_REQ_DATA_MAN__(payload, uncrptLen, 0, GM_ORDER_TRACKING);

	BYTE* sbuf = (BYTE*)buf;

	EncryptPakcge(isCrypt, sbuf, uncrptLen, GET_GM_PROTOHEAD_SIZE, m_key, &cryptLen);
	void* cBuff = NULL;
	UINT32 pLen = 0;
	SendDataMan(GM_ORDER_TRACKING, req, buf, cryptLen, 1, cBuff, &pLen);
	if (IsNull(cBuff))
		return 0;
	if (isCrypt)
		appDecryptDataWithKey((AES_BYTE*)cBuff, pLen, m_key);

	ASSERT(cBuff != NULL);
	SGSResPayload* pres = (SGSResPayload*)cBuff;
	pres->ntoh();
	RecharegeStatisticeIofo* retData = NULL;
	int nSize = sizeof(RecharegeStatisticeIofo);
	nSize;

	if (pres->cmdCode == GM_ORDER_TRACKING && pres->retCode == 0)
	{
		GMGetRealtimeRechargeStatisticsRes* StatisticsRes = (GMGetRealtimeRechargeStatisticsRes*)pres->data;
		StatisticsRes->ntoh();
		retNum = StatisticsRes->retNum;
		if (from == 0)
			*total = StatisticsRes->total;
		retData = new RecharegeStatisticeIofo[StatisticsRes->retNum];
		printf("当前返回数量: %u\n", StatisticsRes->retNum);
		for (int i = 0; i < StatisticsRes->retNum; i++)
		{
			StatisticsRes->data[i].ntoh();
			char gbNikc[33] = { 0 };
			retData[i].actorId = StatisticsRes->data[i].actorId;
			utf8_gb2312(StatisticsRes->data[i].actorName, strlen(StatisticsRes->data[i].actorName), gbNikc, 33);
			strcpy(retData[i].actorName, gbNikc);
			retData[i].platformid = StatisticsRes->data[i].platformid;
			retData[i].rmb = StatisticsRes->data[i].rmb;
			retData[i].starus = StatisticsRes->data[i].starus;
			strcpy(retData[i].RechargeTime, StatisticsRes->data[i].RechargeTime);
		}
	}
	else
	{
		*retCode = GM_RECHARGE_STATISTICS_FAILED;
		*total = 0;
	}

	return retData;
}

///查询充值数据统计;
StartisticsInfo* CGMSdkHelper::QueryPecharGeStatistcs(UINT16  platId, UINT16 channel, UINT16  partId, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total)
{
	if (!m_isLogin)
	{
		*retCode = GM_CONN_SERVER_FAILED;
		return NULL;
	}

	BYTE buf[1024] = { 0 };
	UINT16 uncrptLen = GET_SGSPAYLOAD_REQ_SIZE + GET_ANY_STRUC_SIZE(GMGetRechargeStatisticsRsq, 0);
	UINT16 cryptLen = BytesAlign(uncrptLen);
	DWORD req = AddOneForRequence();
	BYTE isCrypt = 1;


	GMGetRechargeStatisticsRsq* p = (GMGetRechargeStatisticsRsq*)(buf + GET_GM_PROTOHEAD_SIZE + GET_SGSPAYLOAD_REQ_SIZE);
	strcpy(p->startTime, startTime);
	strcpy(p->endTime, endTime);
	p->platId = platId;
	p->channelId = channel;
	p->partId = partId;

	p->hton();
	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, cryptLen, DATA_PACKET, isCrypt, req, 1);
	SGSReqPayload* payload = (SGSReqPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_REQ_DATA_MAN__(payload, uncrptLen, 0, GM_ORDER_STATISTICS);

	BYTE* sbuf = (BYTE*)buf;

	EncryptPakcge(isCrypt, sbuf, uncrptLen, GET_GM_PROTOHEAD_SIZE, m_key, &cryptLen);
	void* cBuff = NULL;
	UINT32 pLen = 0;
	SendDataMan(GM_ORDER_STATISTICS, req, buf, cryptLen, 1, cBuff, &pLen);
	if (IsNull(cBuff))
		return 0;
	if (isCrypt)
		appDecryptDataWithKey((AES_BYTE*)cBuff, pLen, m_key);

	ASSERT(cBuff != NULL);
	SGSResPayload* pres = (SGSResPayload*)cBuff;
	pres->ntoh();

	int nSize = sizeof(RetActorInfo);
	nSize;
	GMGetRecharStatisticsRes* RecharStatistics = (GMGetRecharStatisticsRes*)pres->data;
	StartisticsInfo* retData = new StartisticsInfo[RecharStatistics->retNum];
	if (pres->cmdCode == GM_ORDER_STATISTICS && pres->retCode == 0)
	{
		retNum = RecharStatistics->retNum;
		*total = RecharStatistics->retNum;
		for (BYTE i = 0; i<RecharStatistics->retNum; i++)
		{
			RecharStatistics->Rechardata[i].ntoh();
			strcpy(retData[i].dateTime, RecharStatistics->Rechardata[i].dateTime);
			retData[i].PaymentUserNum = RecharStatistics->Rechardata[i].PaymentUserNum;
			retData[i].newPaymentUserNum = RecharStatistics->Rechardata[i].newPaymentUserNum;
			retData[i].PaymentcountNum = RecharStatistics->Rechardata[i].PaymentcountNum;
			retData[i].PaymentMoney = RecharStatistics->Rechardata[i].PaymentMoney;
			retData[i].LoginPaymentRates = RecharStatistics->Rechardata[i].LoginPaymentRates;
			retData[i].RegiPaymentRates = RecharStatistics->Rechardata[i].RegiPaymentRates;
			retData[i].PaymentARPU = RecharStatistics->Rechardata[i].PaymentARPU;
			retData[i].LoginARPU = RecharStatistics->Rechardata[i].LoginARPU;

		}

	}
	else
	{
		*retCode = GM_RECHARGE_STATISTICS_NUM;
		*total = 0;
	}

	return retData;
}

//查询留存统计
GMQLeavekeepDateInfo* CGMSdkHelper::QueryLeaveKeepStatistics(UINT16 platId, UINT16 channel, UINT16 partId, const char* starTime, const char* endTime, BYTE lktype, BYTE lkTimeType, BYTE& retNum, UINT16* retCode, UINT32* total)
{
	if (!m_isLogin)
	{
		*retCode = GM_CONN_SERVER_FAILED;
		return NULL;
	}

	BYTE buf[1024] = { 0 };
	UINT16 uncrptLen = GET_SGSPAYLOAD_REQ_SIZE + GET_ANY_STRUC_SIZE(GMQLeaveKeepReq, 0);
	UINT16 cryptLen = BytesAlign(uncrptLen);
	DWORD req = AddOneForRequence();
	BYTE isCrypt = 1;


	GMQLeaveKeepReq* p = (GMQLeaveKeepReq*)(buf + GET_GM_PROTOHEAD_SIZE + GET_SGSPAYLOAD_REQ_SIZE);
	strcpy(p->startTime, starTime);
	strcpy(p->entTime, endTime);
	p->platId = platId;
	p->channelId = channel;
	p->partId = partId;
	p->lktype = lktype;
	p->lkTimeType = lkTimeType;
	p->hton();
	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, cryptLen, DATA_PACKET, isCrypt, req, 1);
	SGSReqPayload* payload = (SGSReqPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_REQ_DATA_MAN__(payload, uncrptLen, 0, GM_RETENTION_STATISTICS);

	BYTE* sbuf = (BYTE*)buf;

	EncryptPakcge(isCrypt, sbuf, uncrptLen, GET_GM_PROTOHEAD_SIZE, m_key, &cryptLen);
	void* cBuff = NULL;
	UINT32 pLen = 0;
	SendDataMan(GM_RETENTION_STATISTICS, req, buf, cryptLen, 1, cBuff, &pLen);
	if (IsNull(cBuff))
		return 0;
	if (isCrypt)
		appDecryptDataWithKey((AES_BYTE*)cBuff, pLen, m_key);

	ASSERT(cBuff != NULL);
	SGSResPayload* pres = (SGSResPayload*)cBuff;
	pres->ntoh();
	GMQLeavekeepDateInfo* retData = NULL;
	int nSize = sizeof(GMQLeavekeepDateInfo);
	nSize;
	if (pres->cmdCode == GM_RETENTION_STATISTICS && pres->retCode == 0)
	{
		GMQLeaveKeepRes* LeaveKeepRes = (GMQLeaveKeepRes*)pres->data;
		retData = new GMQLeavekeepDateInfo[LeaveKeepRes->retTimeNum];
		memset(retData, 0, sizeof(GMQLeaveKeepRes));
		retNum = LeaveKeepRes->retTimeNum;
		*total = 0;

		for (UINT32 i = 0; i< LeaveKeepRes->retTimeNum; i++)
		{
			LeaveKeepRes->KeepTime[i].ntoh();
			retData[i].retNum = LeaveKeepRes->KeepTime[i].retNum;
			retData[i].NewRecount = LeaveKeepRes->KeepTime[i].NewRecount;
			strcpy(retData[i].LeaveTime, LeaveKeepRes->KeepTime[i].LeaveTime);

			for (UINT16 j = 0; j<retData[i].retNum; j++)
			{
				retData[i].Retained[j] = LeaveKeepRes->KeepTime[i].Retained[j];
				retData[i].rate[j] = LeaveKeepRes->KeepTime[i].rate[j];
			}
		}

	}
	else
	{
		*retCode = GM_QUERY_ACCOUNT_LK_FAILED;
		*total = 0;
	}

	return retData;
}

///等级分布统计;
GM_Grade_RatingStatisticsInfo* CGMSdkHelper::QueryGrade_RatingStatisticsInfo(UINT16 platId, UINT16 channel, UINT16 partId, const char* starTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total)
{
	if (!m_isLogin)
	{
		*retCode = GM_CONN_SERVER_FAILED;
		return NULL;
	}

	BYTE buf[1024] = { 0 };
	UINT16 uncrptLen = GET_SGSPAYLOAD_REQ_SIZE + GET_ANY_STRUC_SIZE(GM_Grade_RatingStatisticsRsq, 0);
	UINT16 cryptLen = BytesAlign(uncrptLen);
	DWORD req = AddOneForRequence();
	BYTE isCrypt = 1;


	GM_Grade_RatingStatisticsRsq* p = (GM_Grade_RatingStatisticsRsq*)(buf + GET_GM_PROTOHEAD_SIZE + GET_SGSPAYLOAD_REQ_SIZE);
	p->partId = partId;
	p->channelId = channel;
	p->platId = platId;
	strcpy(p->startTime, starTime);
	strcpy(p->endTime, endTime);
	p->hton();

	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, cryptLen, DATA_PACKET, isCrypt, req, 1);
	SGSReqPayload* payload = (SGSReqPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_REQ_DATA_MAN__(payload, uncrptLen, 0, GM_LEVEL_DISTRIBUTION);

	BYTE* sbuf = (BYTE*)buf;

	EncryptPakcge(isCrypt, sbuf, uncrptLen, GET_GM_PROTOHEAD_SIZE, m_key, &cryptLen);
	void* cBuff = NULL;
	UINT32 pLen = 0;
	SendDataMan(GM_LEVEL_DISTRIBUTION, req, buf, cryptLen, 1, cBuff, &pLen);
	if (IsNull(cBuff))
		return 0;
	if (isCrypt)
		appDecryptDataWithKey((AES_BYTE*)cBuff, pLen, m_key);

	ASSERT(cBuff != NULL);
	SGSResPayload* pres = (SGSResPayload*)cBuff;
	pres->ntoh();
	GM_Grade_RatingStatisticsInfo* retData = NULL;
	int nSize = sizeof(GM_Grade_RatingStatisticsInfo);
	nSize;
	if (pres->cmdCode == GM_LEVEL_DISTRIBUTION && pres->retCode == 0)
	{
		GM_Grade_RatingStatisticsRes* Grade_RatingS = (GM_Grade_RatingStatisticsRes*)pres->data;
		retNum = Grade_RatingS->retNum;
		*total = Grade_RatingS->Grade_sum;
		retData = new GM_Grade_RatingStatisticsInfo[Grade_RatingS->retNum];
		for (BYTE i = 0; i<Grade_RatingS->retNum; i++)
		{
			Grade_RatingS->date[i].ntoh();
			retData[i].acterNum = Grade_RatingS->date[i].acterNum;
			retData[i].grade = Grade_RatingS->date[i].grade;
			retData[i].proportion = Grade_RatingS->date[i].proportion;
		}
	}
	else
	{
		*retCode = GM_QUERY_LEVEL_DISTRIBUTION_FAILED;
	}

	return retData;
}

long CGMSdkHelper::AddOneForRequence()
{
	if (InterlockedIncrement(&m_requence) <= 0)
		m_requence = 1;
	return m_requence;
}

void  CGMSdkHelper::ReleaseHandle()
{
	delete this;
}

void CGMSdkHelper::StartReconnect()
{
	m_reconnectTimer.Start();
}

//心跳Time触发;
void CGMSdkHelper::HeartbeatTimer(void* param, TTimerID id)
{
	CGMSdkHelper* preconn = (CGMSdkHelper*)param;
	ASSERT(preconn != NULL);

	preconn->OnHeartbeat();
}

//心跳检测;
void CGMSdkHelper::OnHeartbeat()
{
	if (m_heartbeatTime == 0)
	{
		m_heartbeatTime = GetTickCount();
		return;
	}
	DWORD nowInterval = (GetTickCount() - m_heartbeatTime) / 1000;
	if (m_heartbeatTime > 0 && nowInterval > m_heartbeatInterval * 3000)
	{
		GMSdkSesssion* psession = m_pLocalSession;
		m_pLocalSession = NULL;
		GetGMGlobalObjMan()->GetServeMan()->CloseSession(psession, true);
	}
	else
		SendHeartPackge();

}

//重连Timer触发函数;
void CGMSdkHelper::ReconnectTimer(void* param, TTimerID id)
{
	CGMSdkHelper* preconn = (CGMSdkHelper*)param;
	ASSERT(preconn != NULL);

	preconn->OnReconnect();
}
//发送心跳包处理;
void CGMSdkHelper::SendHeartPackge()
{
	GetGMGlobalObjMan()->GetServeMan()->SendSessionData(m_pLocalSession, &m_hearbeatHead, GET_GM_PROTOHEAD_SIZE);
}

//正在连接...
void CGMSdkHelper::OnReconnect()
{
	SOCKET sockt = TCPNewClientByTime(GetIPByName(m_gmsIp), m_gmsPort, m_connTimeout);
	if (!SockValid(sockt))
	{
		LogInfo(("正在重连......\n"));
		return;
	}

	char headBuf[65] = { 0 };
	char buf[1024] = { 0 };
	GMProtocalHead* phead = NULL;
	memset(m_key, 0, 33);
	strcpy(m_key, DEFAULT_TOKEN_KEY);

	int ret = SockReadAllByTime(sockt, headBuf, GET_GM_PROTOHEAD_SIZE, 5000);
	if (ret <= 0)
		return;
	phead = (GMProtocalHead*)headBuf;
	ASSERT(phead != NULL);
	phead->ntoh();

	ret = SockReadAllByTime(sockt, buf, phead->pduLen, 3000);
	if (ret <= 0)
		return;

	if (phead->isCrypt)
		appDecryptDataWithKey((AES_BYTE*)buf, phead->pduLen, m_key);

	SGSResPayload* ppreqPlayload = (SGSResPayload*)buf;
	ASSERT(ppreqPlayload != NULL);
	ppreqPlayload->ntoh();
	if (phead->pktType == DATA_TOKEN_KEY_PACKET && ppreqPlayload->retCode == 0)
	{
		GMConntedTokenHeart* p = (GMConntedTokenHeart*)ppreqPlayload->data;
		ASSERT(p != NULL);
		p->ntoh();
		memcpy(m_key, p->key, DEFAULT_KEY_SIZE);
		m_heartbeatInterval = p->heatbeatInterval;
	}
	else
		return;

	//CXTAutoLock locked(m_locker);
	if (m_heartbeatInterval > 0)
	{
		if (!m_heartbeatTimer.IsStarted())
		{
			m_heartbeatTimer.SetPeriod(m_heartbeatInterval * 1000);
			m_heartbeatTimer.Start();
		}
	}

	GetGMGlobalObjMan()->GetServeMan()->TrustSession(sockt, this, true);
	m_isLogin = true;
	m_heartbeatTime = GetTickCount();
	m_reconnectTimer.Stop(false);

}

//发送数据;
int CGMSdkHelper::SendDataMan(UINT16 cmdCode, DWORD req, void* buf, int bufLen, BYTE isWait, void*& outData, UINT32* outDataLen)
{
	CXTAutoLock locked(m_locker);
	if (m_pLocalSession == NULL)
		return 1;

	if (!isWait)
		return GetGMGlobalObjMan()->GetServeMan()->SendSessionData(m_pLocalSession, buf, bufLen);

	CSemaDataMan* psema = CyaTcp_New(CSemaDataMan);
	ASSERT(psema != NULL);
	AutoRefCountObjTmpl<CSemaDataMan> autoObj(psema);
	m_semaData.insert(std::make_pair(req, psema));
	int ret = GetGMGlobalObjMan()->GetServeMan()->SendSessionData(m_pLocalSession, buf, bufLen);
	if (IS_TEST)
		autoObj->Wait();
	else
		autoObj->Wait(10000);

	{
		SemaData::iterator it = m_semaData.find(req);
		if (it != m_semaData.end())
			m_semaData.erase(req);
	}

	outData = autoObj->DelData();
	*outDataLen = autoObj->GetDataLen();

	return ret;

}

//数据加密;
void CGMSdkHelper::EncryptPakcge(BYTE isCrypt, BYTE*& buf, UINT16 unCrptfLen, UINT16 sourceHead, char* key, UINT16* crptLen)
{
	if (isCrypt)
		appEncryptDataWithKey((AES_BYTE*)buf + sourceHead, unCrptfLen, key);

	if (isCrypt)
		*crptLen += sourceHead;
	else
		*crptLen = unCrptfLen + sourceHead;
}

//写入进程数据;
void CGMSdkHelper::InputProcessData(UINT16 packType, void* data, int dataLen, DWORD req, BOOL isOver)
{
	if (data == NULL || dataLen <= 0)
		return;

	SemaData::iterator it = m_semaData.find(req);
	CSemaDataMan* psema = NULL;
	if (m_semaData.size() > 0 && it != m_semaData.end())
	{
		psema = it->second;
		ASSERT(psema != NULL);
		if (psema != NULL)
			psema->AddRef();
	}
	else
	{
		if (data != NULL)
			CyaTcp_Free(data);
		data = NULL;
		dataLen = 0;
		return;
	}

	AutoRefCountObjTmpl<CSemaDataMan> autoObj(psema);
	autoObj->InputData(data, dataLen, isOver);
}

//查询实时在线;
OnlineInfo* CGMSdkHelper::QueryOnlineCurrent(UINT16 platId, UINT16 channel, UINT16 partId, UINT32 from, BYTE to, const char* sTime, BYTE& retNum, UINT16* retCode, UINT32* total)
{
	if (!m_isLogin)
	{
		*retCode = GM_CONN_SERVER_FAILED;
		return NULL;
	}

	BYTE buf[1024] = { 0 };
	UINT16 uncrptLen = GET_SGSPAYLOAD_REQ_SIZE + GET_ANY_STRUC_SIZE(GMOnlineCountReq, 0);
	UINT16 cryptLen = BytesAlign(uncrptLen);
	DWORD req = AddOneForRequence();
	BYTE isCrypt = 1;


	GMOnlineCountReq* p = (GMOnlineCountReq*)(buf + GET_GM_PROTOHEAD_SIZE + GET_SGSPAYLOAD_REQ_SIZE);
	strcpy(p->startTime, sTime);
	p->partId = partId;
	p->channelId = channel;
	p->platId = platId;
	p->from = from;
	p->to = to;
	p->hton();

	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, cryptLen, DATA_PACKET, isCrypt, req, 1);
	SGSReqPayload* payload = (SGSReqPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_REQ_DATA_MAN__(payload, uncrptLen, 0, GM_ONLINE_REALTIME);

	BYTE* sbuf = (BYTE*)buf;

	EncryptPakcge(isCrypt, sbuf, uncrptLen, GET_GM_PROTOHEAD_SIZE, m_key, &cryptLen);
	void* cBuff = NULL;
	UINT32 pLen = 0;
	SendDataMan(GM_ONLINE_REALTIME, req, buf, cryptLen, 1, cBuff, &pLen);
	if (IsNull(cBuff))
		return 0;
	if (isCrypt)
		appDecryptDataWithKey((AES_BYTE*)cBuff, pLen, m_key);
	ASSERT(cBuff != NULL);
	SGSResPayload* pres = (SGSResPayload*)cBuff;
	pres->ntoh();
	OnlineInfo* retData = NULL;
	int nSize = sizeof(OnlineInfo);
	nSize;

	if (pres->cmdCode == GM_ONLINE_REALTIME && pres->retCode == 0)
	{
		GMOnlineCountRes* OnlineCount = (GMOnlineCountRes*)pres->data;
		retNum = OnlineCount->retNum;
		*total = OnlineCount->total;
		retData = new OnlineInfo[OnlineCount->retNum];
		for (BYTE i = 0; i < OnlineCount->retNum; i++)
		{
			OnlineCount->info[i].ntoh();
			retData[i].onlineCount = OnlineCount->info[i].onlineCount;
			strcpy(retData[i].olTime, OnlineCount->info[i].olTime);
		}
	}
	else
		*retCode = GM_QUERY_ONLINENUM_NUM_FAILED;


	return retData;
}

//设置本地会话;
void CGMSdkHelper::SetLocalSession(GMSdkSesssion* session)
{
	m_pLocalSession = session;
}

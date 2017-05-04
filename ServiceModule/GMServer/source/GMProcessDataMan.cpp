#include "GMProcessDataMan.h"
#include "GMSession.h"
#include "OtherServeConnMan.h"
#include "GMServeMan.h"
#include "GMComm.h"
#include "AES.h"
#include "ServiceCmdCode.h"
#include "ServiceErrorCode.h"
#include "gb_utf8.h"

CGMProcessDataMan* g_processDataManObj = NULL;


#define ADD_PROCESS_FUNC_MAN__(cmd, func) \
	m_processFuncHandler.insert(std::make_pair(cmd, &CGMProcessDataMan::func))

BYTE* CGMProcessDataMan::GetData(void* data)
{
	return ((BYTE*)data + member_offset(SGSReqPayload, rawDataBytes, data));
}

void CGMProcessDataMan::CryptEncryptPackge(BYTE isCrypt, BYTE*& data, UINT16 headSize, UINT16 sourceLen, char* enccrptKey, UINT16* crptLen)
{
	if (isCrypt)
		appEncryptDataWithKey((AES_BYTE*)data + headSize, sourceLen, enccrptKey);

	if (isCrypt)
		*crptLen += headSize;
	else
		*crptLen = headSize + sourceLen;
}

CGMProcessDataMan::CGMProcessDataMan()
{

	ADD_PROCESS_FUNC_MAN__(GM_NEW_STATISTICS, QueryNewStatistics);	//��ѯʵʱ����ͳ��;
	ADD_PROCESS_FUNC_MAN__(GM_ORDER_TRACKING, QueryRechargeOrder);	//��ѯ��ֵ��������;
	ADD_PROCESS_FUNC_MAN__(GM_ORDER_STATISTICS, QueryPecharGeStatistcs);//��ֵ����ͳ��;
	ADD_PROCESS_FUNC_MAN__(GM_RETENTION_STATISTICS, QueryLeaveKeepStatistics);	//����ͳ��;
	ADD_PROCESS_FUNC_MAN__(GM_ONLINE_REALTIME, QueryRealtimeoneline);	//ʵʱ����;
	ADD_PROCESS_FUNC_MAN__(GM_ONLINE_STATISTICS, QueryLeaveKeepStatistics);//��������ͳ��;
	ADD_PROCESS_FUNC_MAN__(GM_LEVEL_DISTRIBUTION, QueryLevelDistribution);//�ȼ��ֲ�;

}

CGMProcessDataMan::~CGMProcessDataMan()
{

}

int CGMProcessDataMan::OnMsgProcessDataMan(UINT16 cmdCode, void* data, int dataLen, CCMSession* session, DWORD sreq)
{
	std::map<UINT16, FuncProcessDataHandler>::iterator it = m_processFuncHandler.find(cmdCode);
	if (it != m_processFuncHandler.end())
		return (this->*(it->second))(data, dataLen, session, sreq);
	else
		//����ʶ�������
		return GM_INVALID_COMMAND;
	return 0;
}

void ActiveDeviceDataMan(std::vector<ActiveDevInfo> actDevInfo, ActiveDevInfo data[1])
{
	for (int i = 0; i < (int)actDevInfo.size(); i++)
	{
		strcpy(data[i].devIdfa, actDevInfo[i].devIdfa);
		strcpy(data[i].devIp, actDevInfo[i].devIp);
		strcpy(data[i].devType, actDevInfo[i].devType);
		strcpy(data[i].sys, actDevInfo[i].sys);
		strcpy(data[i].region, actDevInfo[i].region);
		strcpy(data[i].tts, actDevInfo[i].tts);
	}
	actDevInfo.clear();
}

//��ȡ���ߵ���Ϣ;
void AddOnlineData(std::vector<OnlineInfo>& olInfo, OnlineInfo info[1])
{
	if (olInfo.size() <= 0)
		return;
	for (int i = 0; i < (int)olInfo.size(); i++)
	{
		strcpy(info[i].olTime, olInfo[i].olTime);
		info[i].onlineCount = olInfo[i].onlineCount;
		info[i].hton();
	}
}


// �������ݷ���;
int CGMProcessDataMan::ProcessDataSend(CCMSession* session, void* data, int dataLen)
{
	ASSERT(session != NULL);
	if (session == NULL)
	{
		FreeMan(data);
		return GM_NOT_EXISTS_SESSION;
	}

	int ret = GetGMServeManObj()->GetGMServeMan()->SendSessionData(session, data, dataLen);
	FreeMan(data);
	return ret;

}

//ww add 20151127;
///ʵʱ����ͳ��;GM_NEW_STATISTICS GMRealTimeAddedStatisticsReq
int CGMProcessDataMan::QueryNewStatistics(void* data, int dataLen, CCMSession* session, DWORD sreq)
{
	if (session == NULL)
		return GM_NOT_EXISTS_SESSION;
	BYTE isCrpt = 1;
	UINT32 total = 0;
	std::vector<TimeAddedinfo> Addedinfo;
	GMRealTimeAddedStatisticsReq* p = (GMRealTimeAddedStatisticsReq*)GetData(data);
	p->ntoh();
	//����endTime��ȡstartTime;
	ASSERT(p != NULL);
	//�����ѯ�ӿ�;
	int ret = GetOtherServeManObj()->QueryAddNewStatistics(p->platId, p->channelId, p->partId, p->startTime, p->endTime, Addedinfo, &total);

	if (ret != GM_OK)
	{
		LogInfo(("��ѯ[ʵʱ����ͳ��]�����룺[%d]\n", GM_QUERY_NEWSTATISTICS_FAILED));
		return GM_QUERY_NEWSTATISTICS_FAILED;
	}
	LogInfo(("��ѯ[�����û���]\n"));
	UINT16 uncrptLen = GET_SGSPAYLOAD_RES_SIZE + GET_ANY_STRUC_SIZE(GMRealTImeAddedStAllNumRes, 1);
	UINT16 crptLen = BytesAlign(uncrptLen);
	BYTE* buf = (BYTE*)AllocMan(crptLen + GET_GM_PROTOHEAD_SIZE);
	ASSERT(buf != NULL);

	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, crptLen, DATA_PACKET, isCrpt, sreq, 1);
	SGSResPayload* pRes = (SGSResPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_RES_DATA_MAN__(pRes, uncrptLen, 0, GM_NEW_STATISTICS, GM_OK);
	GMRealTImeAddedStAllNumRes* paddNewAccountRes = (GMRealTImeAddedStAllNumRes*)(buf + GET_SGSPAYLOAD_RES_SIZE + GET_GM_PROTOHEAD_SIZE);
	paddNewAccountRes->InfoNum.EquipmentAddedNum = 0;
	paddNewAccountRes->InfoNum.AccountAddedNum = 0;
	paddNewAccountRes->InfoNum.ActorAddedNum = 0;
	paddNewAccountRes->InfoNum.EquipmentLogInNum = 0;
	paddNewAccountRes->InfoNum.AccountLogInNum = 0;
	paddNewAccountRes->InfoNum.ActorLogInNum = 0;
	paddNewAccountRes->InfoNum.EquipmentActiveNum = 0;
	paddNewAccountRes->InfoNum.AccountActiveNum = 0;
	paddNewAccountRes->InfoNum.ActorActiveNum = 0;
	for (int i = 0; i<Addedinfo.size(); i++)
	{
		paddNewAccountRes->InfoNum.EquipmentAddedNum = paddNewAccountRes->InfoNum.EquipmentAddedNum + Addedinfo[i].EquipmentAddedNum;
		paddNewAccountRes->InfoNum.AccountAddedNum = paddNewAccountRes->InfoNum.AccountAddedNum + Addedinfo[i].AccountAddedNum;
		paddNewAccountRes->InfoNum.ActorAddedNum = paddNewAccountRes->InfoNum.ActorAddedNum + Addedinfo[i].ActorAddedNum;
		paddNewAccountRes->InfoNum.EquipmentLogInNum = paddNewAccountRes->InfoNum.EquipmentLogInNum + Addedinfo[i].EquipmentLogInNum;
		paddNewAccountRes->InfoNum.AccountLogInNum = paddNewAccountRes->InfoNum.AccountLogInNum + Addedinfo[i].AccountLogInNum;
		paddNewAccountRes->InfoNum.ActorLogInNum = paddNewAccountRes->InfoNum.ActorLogInNum + Addedinfo[i].ActorLogInNum;
		paddNewAccountRes->InfoNum.EquipmentActiveNum = paddNewAccountRes->InfoNum.EquipmentActiveNum + Addedinfo[i].EquipmentActiveNum;
		paddNewAccountRes->InfoNum.AccountActiveNum = paddNewAccountRes->InfoNum.AccountActiveNum + Addedinfo[i].AccountActiveNum;
		paddNewAccountRes->InfoNum.ActorActiveNum = paddNewAccountRes->InfoNum.ActorActiveNum + Addedinfo[i].ActorActiveNum;
	}

	paddNewAccountRes->InfoNum.hton();
	CryptEncryptPackge(isCrpt, buf, GET_GM_PROTOHEAD_SIZE, uncrptLen, session->GetKey(), &crptLen);

	Addedinfo.clear();
	//��������;
	return ProcessDataSend(session, buf, crptLen);

}

///������������������������������������������ʵʱ��ֵͳ��;
///������ѯ;
int CGMProcessDataMan::QueryRechargeOrder(void* data, int dataLen, CCMSession* session, DWORD sreq)
{
	if (session == NULL)
		return GM_NOT_EXISTS_SESSION;

	UINT32 total = 0;
	BYTE isCrpt = 1;
	std::vector<RecharegeStatisticeIofo> StatisticeIofo;
	GMGetOrderTrackingRsq* p = (GMGetOrderTrackingRsq*)GetData(data);
	p->ntoh();
	ASSERT(p != NULL);
	ASSERT(p->platId != 0 && p->partId != 0);
	int ret = GetOtherServeManObj()->QueryRechargeOrderDealwith(p->platId, p->channelId, p->partId, p->from, p->to, p->startTime, p->endTime, StatisticeIofo, &total);

	if (ret != GM_OK)
	{
		LogInfo(("��ѯ[��ѯ��������]�����룺[%d]\n", GM_RECHARGE_STATISTICS_FAILED));
		return GM_RECHARGE_STATISTICS_FAILED;
	}

	LogInfo(("��ѯ[��ѯ��������]\n"));

	UINT16 uncrptLen = GET_SGSPAYLOAD_RES_SIZE + GET_ANY_STRUC_SIZE(GMGetRealtimeRechargeStatisticsRes, 0) + sizeof(RecharegeStatisticeIofo)*((int)StatisticeIofo.size() - 1);
	UINT16 crptLen = BytesAlign(uncrptLen);
	BYTE* buf = (BYTE*)AllocMan(crptLen + GET_GM_PROTOHEAD_SIZE);
	ASSERT(buf != NULL);

	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, crptLen, DATA_PACKET, isCrpt, sreq, 1);
	SGSResPayload* pRes = (SGSResPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_RES_DATA_MAN__(pRes, uncrptLen, 0, GM_ORDER_TRACKING, GM_OK);
	GMGetRealtimeRechargeStatisticsRes* pqaccountDetailRes = (GMGetRealtimeRechargeStatisticsRes*)(buf + GET_SGSPAYLOAD_RES_SIZE + GET_GM_PROTOHEAD_SIZE);
	pqaccountDetailRes->retNum = (UINT16)StatisticeIofo.size();
	pqaccountDetailRes->total = total;
	RechargeOrder(StatisticeIofo, pqaccountDetailRes->data);
	pqaccountDetailRes->hton();


	CryptEncryptPackge(isCrpt, buf, GET_GM_PROTOHEAD_SIZE, uncrptLen, session->GetKey(), &crptLen);

	StatisticeIofo.clear();

	return ProcessDataSend(session, buf, crptLen);

}

///��ֵ����ͳ��;
int CGMProcessDataMan::QueryPecharGeStatistcs(void* data, int dataLen, CCMSession* session, DWORD sreq)
{

	if (session == NULL)
		return GM_NOT_EXISTS_SESSION;

	UINT32 total = 0;
	BYTE isCrpt = 1;
	std::vector<StartisticsInfo> StatisticeIofo;

	GMGetRechargeStatisticsRsq* p = (GMGetRechargeStatisticsRsq*)GetData(data);
	p->ntoh();


	int ret = GetOtherServeManObj()->QueryRechargeStatistics(p->platId, p->channelId, p->partId, p->from, p->to, p->startTime, p->endTime, StatisticeIofo, &total);

	if (ret != GM_OK)
	{
		LogInfo(("��ѯ[��ֵ����ͳ��]�����룺[%d]\n", GM_RECHARGE_STATISTICS_NUM));
		return GM_RECHARGE_STATISTICS_NUM;
	}

	LogInfo(("��ѯ[��ֵ����ͳ��]\n"));
	UINT16 uncrptLen = GET_SGSPAYLOAD_RES_SIZE + GET_ANY_STRUC_SIZE(GMGetRecharStatisticsRes, 0) + sizeof(StartisticsInfo)*((int)StatisticeIofo.size() - 1);;
	UINT16 crptLen = BytesAlign(uncrptLen);
	BYTE* buf = (BYTE*)AllocMan(crptLen + GET_GM_PROTOHEAD_SIZE);
	ASSERT(buf != NULL);

	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, crptLen, DATA_PACKET, isCrpt, sreq, 1);
	SGSResPayload* pRes = (SGSResPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_RES_DATA_MAN__(pRes, uncrptLen, 0, GM_ORDER_STATISTICS, GM_OK);
	GMGetRecharStatisticsRes* paddNewAccountRes = (GMGetRecharStatisticsRes*)(buf + GET_SGSPAYLOAD_RES_SIZE + GET_GM_PROTOHEAD_SIZE);
	paddNewAccountRes->retNum = (UINT16)StatisticeIofo.size();
	paddNewAccountRes->total = total;
	RecharegeStat(StatisticeIofo, paddNewAccountRes->Rechardata);

	CryptEncryptPackge(isCrpt, buf, GET_GM_PROTOHEAD_SIZE, uncrptLen, session->GetKey(), &crptLen);

	StatisticeIofo.clear();
	//��������;
	return ProcessDataSend(session, buf, crptLen);

}

//��ֵ������ϸ��Ϣ����;
void CGMProcessDataMan::RechargeOrder(std::vector<RecharegeStatisticeIofo>& OrderInfo, RecharegeStatisticeIofo data[1])
{
	for (int i = 0; i < (int)OrderInfo.size(); i++)
	{
		data[i].actorId = OrderInfo[i].actorId;
		strcpy(data[i].actorName, OrderInfo[i].actorName);
		strcpy(data[i].RechargeTime, OrderInfo[i].RechargeTime);
		data[i].rmb = OrderInfo[i].rmb;
		data[i].starus = OrderInfo[i].starus;
		data[i].platformid = OrderInfo[i].platformid;
		data[i].hton();
	}
	OrderInfo.clear();
}

//����ͳ��;
int CGMProcessDataMan::QueryLeaveKeepStatistics(void* data, int dataLen, CCMSession* session, DWORD sreq)
{

	if (session == NULL)
		return GM_NOT_EXISTS_SESSION;
	UINT32 getcount = NULL;
	BYTE isCrpt = 1;
	std::vector<GMQLeavekeepDateInfo> LeaveKeepInfo;

	GMQLeaveKeepReq* p = (GMQLeaveKeepReq*)GetData(data);
	p->ntoh();



	int ret = GetOtherServeManObj()->GetLeaveKeepStatistics(p->platId, p->channelId, p->partId, p->startTime, p->entTime, p->lktype, p->lkTimeType, LeaveKeepInfo, &getcount);


	if (ret != GM_OK)
	{
		LogInfo(("��ѯ[��������ͳ��]�����룺[%d]\n", GM_QUERY_TIME));
		return GM_QUERY_TIME;
	}
	LogInfo(("[�鿴����]\n"));
	UINT16 uncrptLen = GET_SGSPAYLOAD_RES_SIZE + GET_ANY_STRUC_SIZE(GMQLeaveKeepRes, 0) + sizeof(GMQLeavekeepDateInfo)*((int)LeaveKeepInfo.size() - 1);

	UINT16 crptLen = BytesAlign(uncrptLen);
	BYTE* buf = (BYTE*)AllocMan(crptLen + GET_GM_PROTOHEAD_SIZE);
	ASSERT(buf != NULL);

	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, crptLen, DATA_PACKET, isCrpt, sreq, 1);
	SGSResPayload* pRes = (SGSResPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_RES_DATA_MAN__(pRes, uncrptLen, 0, GM_RETENTION_STATISTICS, GM_OK);
	GMQLeaveKeepRes* pqLeaveKeepeRes = (GMQLeaveKeepRes*)(buf + GET_SGSPAYLOAD_RES_SIZE + GET_GM_PROTOHEAD_SIZE);
	pqLeaveKeepeRes->retTimeNum = getcount;

	for (int i = 0; i < LeaveKeepInfo.size(); i++)
	{
		pqLeaveKeepeRes->KeepTime[i].retNum = LeaveKeepInfo[i].retNum;
		pqLeaveKeepeRes->KeepTime[i].NewRecount = LeaveKeepInfo[i].NewRecount;
		strcpy(pqLeaveKeepeRes->KeepTime[i].LeaveTime, LeaveKeepInfo[i].LeaveTime);
		for (UINT16 j = 0; j<LeaveKeepInfo[i].retNum; j++)
		{
			pqLeaveKeepeRes->KeepTime[i].Retained[j] = LeaveKeepInfo[i].Retained[j];
			pqLeaveKeepeRes->KeepTime[i].rate[j] = LeaveKeepInfo[i].rate[j];
		}
		pqLeaveKeepeRes->KeepTime[i].hton();
	}

	LeaveKeepInfo.clear();

	CryptEncryptPackge(isCrpt, buf, GET_GM_PROTOHEAD_SIZE, uncrptLen, session->GetKey(), &crptLen);

	return ProcessDataSend(session, buf, crptLen);
}

///*---����ͳ��---*
///ʵʱ����
int CGMProcessDataMan::QueryRealtimeoneline(void* data, int datalen, CCMSession* session, DWORD sreq)
{
	if (session == NULL)
		return GM_NOT_EXISTS_SESSION;

	GMOnlineCountReq* p = (GMOnlineCountReq*)GetData(data);
	p->ntoh();
	BYTE isCrpt = 1;
	UINT32 total = 0;
	std::vector<OnlineInfo> infoVec;
	int ret = 0;
	int DayTime = 0;
	int MinuteTime = 0;
	char endTime[20] = { 0 };
	//��ȡ��ǰʱ�䣬ÿ�� ��ѯ��ȡ��ǰʱ��Ϊ��ѯ������;���ص���������ʱ��Ϊ��һ�β�ѯ��ʱ�俪ʼ��;
	GetSysCurrentTime(endTime);
	if ((DayTime = IfDayDateTime(p->startTime)) == 0)//�ж�ʱ���Ƿ�Ϊ0��0��ʾ�ǵ���;
	if ((MinuteTime = IfMinuteTime(p->startTime)) == 1) //�ж�ʱ���Ƿ�С�ڵ�ʱ;

	if (DayTime == 0 && MinuteTime != -1)//��ѯС�ڵ�ǰʱ��;
	{
		Timetodefine(p->startTime, endTime, TRUE);
		ret = GetOtherServeManObj()->GetOnlineHistory(p->platId, p->channelId, p->partId, p->startTime, endTime, infoVec, p->from, p->to, &total);
	}
	if (DayTime == -1)//�뵱�����ڲ�ͬ���ѯ��ʷ��¼;
	{
		Timetodefine(p->startTime, endTime, FALSE);
		ret = GetOtherServeManObj()->GetOnlineHistory(p->platId, p->channelId, p->partId, p->startTime, endTime, infoVec, p->from, p->to, &total);
	}
	if (DayTime == GM_ERROR)
	{
		LogInfo(("��ѯ[��������]ʧ��:��ѯʱ�䲻���ϱ�׼[%d]\n", GM_ERROR));
		return GM_ERROR;
	}

	if (ret != GM_OK)
	{
		LogInfo(("��ѯ[��������]ʧ��:[%d]\n", GM_GET_ONLINE_FAILED));
		return GM_GET_ONLINE_FAILED;
	}

	UINT16 uncrptLen = GET_SGSPAYLOAD_RES_SIZE + GET_ANY_STRUC_SIZE(GMOnlineCountRes, 1) + sizeof(OnlineInfo)*((int)infoVec.size() - 1);
	UINT16 crptLen = BytesAlign(uncrptLen);
	BYTE* buf = (BYTE*)AllocMan(crptLen + GET_GM_PROTOHEAD_SIZE);
	ASSERT(buf != NULL);
	GMOnlineCountRes* ponlineCountRes = (GMOnlineCountRes*)(buf + GET_SGSPAYLOAD_RES_SIZE + GET_GM_PROTOHEAD_SIZE);
	ponlineCountRes->retNum = (UINT16)infoVec.size();
	ponlineCountRes->total = total;
	LogInfo(("��ѯ[��������]-[��ѯ������]��[%d]_[��������]:[%d]\n", ponlineCountRes->total, ponlineCountRes->retNum));
	AddOnlineData(infoVec, ponlineCountRes->info);
	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, crptLen, DATA_PACKET, isCrpt, sreq, 1);
	SGSResPayload* pRes = (SGSResPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_RES_DATA_MAN__(pRes, uncrptLen, 0, GM_ONLINE_REALTIME, GM_OK);
	CryptEncryptPackge(isCrpt, buf, GET_GM_PROTOHEAD_SIZE, uncrptLen, session->GetKey(), &crptLen);



	return ProcessDataSend(session, buf, crptLen);

}

///////��������ͳ��
//int CGMProcessDataMan::QueryOnlineNumStatistics( void* data,int dataLen,CCMSession* session, DWORD sreq )
//{
//	
//		if(session == NULL)
//			return GM_NOT_EXISTS_SESSION;
//		UINT32 getcount=NULL;
//		BYTE isCrpt = 1;
//		int ret = 0;
//		std::vector<GMOnlineDateStatisticsInfo> OnlineDateStatisticsInfo;
//		GMOnlineDateStatisticsReq* p = (GMOnlineDateStatisticsReq*)GetData(data);
//		p->ntoh();
//			ret = GetOtherServeManObj()->OnlineStatistics(p->platId, p->partId,p->startTime,p->endTime,OnlineDateStatisticsInfo,&getcount);
//
//		if(ret != GM_OK)
//			{
//			LogInfo(("��ѯ[��ȡ��������ͳ��]ʧ��:[%d]\n", GM_QUERY_ONLINENUM_STATISTICS_FAILED));
//			return GM_QUERY_ONLINENUM_STATISTICS_FAILED;
//			}
//
//
//		LogInfo(("[�鿴����]\n"));
//		UINT16 uncrptLen = GET_SGSPAYLOAD_RES_SIZE + GET_ANY_STRUC_SIZE(GMQLeaveKeepRes, 0);
//		UINT16 crptLen = BytesAlign(uncrptLen);
//		BYTE* buf = (BYTE*)AllocMan(crptLen + GET_GM_PROTOHEAD_SIZE);
//		ASSERT(buf != NULL);
//
//		GMProtocalHead* phead = (GMProtocalHead*)buf;
//		GM_PROTO_HEAD_DATA_MAN__(phead, crptLen, DATA_PACKET, isCrpt, sreq, 1);
//		SGSResPayload* pRes = (SGSResPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
//		SGS_PROTO_PAYLOAD_RES_DATA_MAN__(pRes, uncrptLen, 0, GM_ONLINE_STATISTICS, GM_OK);
//		GMQLeaveKeepRes* pqLeaveKeepeRes = (GMQLeaveKeepRes*)(buf + GET_SGSPAYLOAD_RES_SIZE + GET_GM_PROTOHEAD_SIZE);
//		pqLeaveKeepeRes->getcount= getcount;	//���ز�ѯ������;Ҳ�����ö�������¼;
//		for (int i = 0; i < (int)LeaveKeepInfo.size(); i++)
//			{
//			pqLeaveKeepeRes->keepInfo[i].NewRecount = LeaveKeepInfo[i].NewRecount;
//			pqLeaveKeepeRes->keepInfo[i].rate =  LeaveKeepInfo[i].rate;
//			pqLeaveKeepeRes->keepInfo[i].RetainedNumber =  LeaveKeepInfo[i].RetainedNumber;
//			pqLeaveKeepeRes->keepInfo[i].hton();	
//
//			}
//		LeaveKeepInfo.clear();
//
//		CryptEncryptPackge(isCrpt, buf, GET_GM_PROTOHEAD_SIZE, uncrptLen, session->GetKey(), &crptLen);
//
//		return ProcessDataSend(session, buf, crptLen);
//
//	return 0;
//}

///*---�ȼ��ֲ�---*
int CGMProcessDataMan::QueryLevelDistribution(void* data, int dataLen, CCMSession* session, DWORD sreq)
{
	if (session == NULL)
		return GM_NOT_EXISTS_SESSION;

	BYTE rentNum = NULL;
	UINT32 Grade_sum = NULL;
	BYTE isCrpt = 1;
	int ret = 0;
	std::vector<GM_Grade_RatingStatisticsInfo> OnlineDateStatisticsInfo;
	GM_Grade_RatingStatisticsRsq* p = (GM_Grade_RatingStatisticsRsq*)GetData(data);
	p->ntoh();
	ret = GetOtherServeManObj()->Grade_RatingStatistics(p->platId, p->channelId, p->partId, p->startTime, p->endTime, OnlineDateStatisticsInfo, &rentNum, &Grade_sum);

	if (ret != GM_OK)
	{
		LogInfo(("��ѯ[��ȡ�ȼ��ֲ�]ʧ��:[%d]\n", GM_QUERY_LEVEL_DISTRIBUTION_FAILED));
		return GM_QUERY_LEVEL_DISTRIBUTION_FAILED;
	}

	LogInfo(("[�鿴�ȼ��ֲ�]\n"));
	UINT16 uncrptLen = GET_SGSPAYLOAD_RES_SIZE + GET_ANY_STRUC_SIZE(GM_Grade_RatingStatisticsRes, 0)*((UINT16)OnlineDateStatisticsInfo.size());
	UINT16 crptLen = BytesAlign(uncrptLen);
	BYTE* buf = (BYTE*)AllocMan(crptLen + GET_GM_PROTOHEAD_SIZE);
	ASSERT(buf != NULL);

	GMProtocalHead* phead = (GMProtocalHead*)buf;
	GM_PROTO_HEAD_DATA_MAN__(phead, crptLen, DATA_PACKET, isCrpt, sreq, 1);
	SGSResPayload* pRes = (SGSResPayload*)(buf + GET_GM_PROTOHEAD_SIZE);
	SGS_PROTO_PAYLOAD_RES_DATA_MAN__(pRes, uncrptLen, 0, GM_LEVEL_DISTRIBUTION, GM_OK);
	GM_Grade_RatingStatisticsRes* Grade_RatingStatis = (GM_Grade_RatingStatisticsRes*)(buf + GET_SGSPAYLOAD_RES_SIZE + GET_GM_PROTOHEAD_SIZE);
	Grade_RatingStatis->Grade_sum = Grade_sum;	//���ز�ѯ������;Ҳ�����ö�������¼;
	Grade_RatingStatis->retNum = rentNum;
	for (int i = 0; i < (int)OnlineDateStatisticsInfo.size(); i++)
	{
		OnlineDateStatisticsInfo[i].ntoh();
		Grade_RatingStatis->date[i].grade = OnlineDateStatisticsInfo[i].grade;
		Grade_RatingStatis->date[i].acterNum = OnlineDateStatisticsInfo[i].acterNum;
		Grade_RatingStatis->date[i].proportion = OnlineDateStatisticsInfo[i].proportion;
	}
	OnlineDateStatisticsInfo.clear();
	CryptEncryptPackge(isCrpt, buf, GET_GM_PROTOHEAD_SIZE, uncrptLen, session->GetKey(), &crptLen);
	return ProcessDataSend(session, buf, crptLen);
}

//����Ƿ�Ϊ����ǰ����;
BOOL CGMProcessDataMan::IfDayDateTime(const char* startTime)
{
	char sDateTime[20] = { 0 };
	char eDateTime[20] = { 0 };
	//��ȡ�������ڣ���ϵͳ��ǰ����;
	GetDate(startTime, sDateTime, FALSE);
	GetDate("", eDateTime, TRUE);
	//�Ƚ����ڣ��Ƿ���ͬһ��;
	return strcmp(sDateTime, eDateTime);

}
///����Ƿ�Ϊ����ʱ��(Сʱ���֣���);��С����10����Ϊ׼;
int CGMProcessDataMan::IfMinuteTime(char* startTime)
{
	char stTime[20] = { 0 };
	char enTime[20] = { 0 };
	//��ȡ���ݽ�����ʱ��;
	GetTime(startTime, stTime, FALSE);
	GetTime("", enTime, TRUE);
	//�Ƚ����ڣ��Ƿ���ͬһ��;
	int ab = 0;
	ab = *(enTime + 4) - *(stTime + 4);
	int ret = strcmp(stTime, enTime);
	if (ret>0)
		return -1;
	return 1;
}

//ʱ��綨,��ѯʵʱ��������ʹ��;
void CGMProcessDataMan::Timetodefine(char* startTIme, char* endTime, BOOL trueS)
{
	startTIme[11] = '0';
	startTIme[12] = '0';
	startTIme[13] = ':';
	startTIme[14] = '0';
	startTIme[15] = '0';
	startTIme[16] = ':';
	startTIme[17] = '0';
	startTIme[18] = '0';

	if (trueS == FALSE)
	{
		GetDate(startTIme, endTime, FALSE);
		endTime[10] = ' ';
		endTime[11] = '2';
		endTime[12] = '3';
		endTime[13] = ':';
		endTime[14] = '5';
		endTime[15] = '9';
		endTime[16] = ':';
		endTime[17] = '5';
		endTime[18] = '9';
	}

}
///��ֵͳ����󷵻�ȡֵ����;
void CGMProcessDataMan::RecharegeStat(std::vector<StartisticsInfo>& OrderInfo, StartisticsInfo data[1])
{
	for (int i = 0; i < (int)OrderInfo.size(); i++)
	{
		strcpy(data[i].dateTime, OrderInfo[i].dateTime);
		data[i].PaymentUserNum = OrderInfo[i].PaymentUserNum;
		data[i].newPaymentUserNum = OrderInfo[i].newPaymentUserNum;
		data[i].PaymentcountNum = OrderInfo[i].PaymentcountNum;
		data[i].PaymentMoney = OrderInfo[i].PaymentMoney;
		data[i].LoginPaymentRates = OrderInfo[i].LoginPaymentRates;
		data[i].RegiPaymentRates = OrderInfo[i].RegiPaymentRates;
		data[i].PaymentARPU = OrderInfo[i].PaymentARPU;
		data[i].LoginARPU = OrderInfo[i].LoginARPU;
		data[i].ntoh();
	}
	OrderInfo.clear();
}
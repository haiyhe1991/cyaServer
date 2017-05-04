#include "GMSSDk.h"
#include "GMSdkHelper.h"
#include "GMGlobalObjMan.h"

GMSSDK_API GMHandle  GM_FetchGMHandle(const char* ip,
	UINT16 port,
	UINT16 connTimeout,
	UINT16 readTimeout /*= 0*/,
	UINT16 writeTimeout/* = 0*/)
{
	//初始化库;
	InitGMGlobalObjMan();
	CGMSdkHelper* p = new CGMSdkHelper(ip, port, connTimeout, readTimeout, writeTimeout);
	ASSERT(p != NULL);
	int i = p->Login();
	if (i == 1)
		printf("连接管理服务器失败！\n");
	return p;
}

//===========================================================================
//				操作接口函数;
//===========================================================================

//注册账号：
//GMSSDK_API int GMS_CreateUser( GMSSHandle handle,const char* name, const char*passwd, bool*isOk)
//{
//	return ((GMSSdkHelper*)handle)->CreateUser(name,passwd,isOk);
//}

//用户登录
GMSSDK_API int GM_UserLogin(GMHandle handle, const char* name, const char* passwd, bool* isOk)
{
	return 0/*((CGMSdkHelper*)handle)->UserLogin(name, passwd, isOk)*/;
}

GMSSDK_API void GM_Del(void* p)
{
	delete[]p;
}

//=======================================================
//				查询函数接口;
//=======================================================
//实时新增统计;
GMSSDK_API	TimeAddedinfo* GM_QueryRealTimeAddedStatistics(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total)
{
	return ((CGMSdkHelper*)handle)->QueryRealTimeAddStatistics(platId, channel, partId, startTime, endTime, retNum, retCode, total);
}

//订单详情查询;
GMSSDK_API RecharegeStatisticeIofo* GM_QueryOrderTracking(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, UINT32 from, BYTE to, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total)
{
	return ((CGMSdkHelper*)handle)->QueryOrderTracking(platId, channel, partId, from, to, startTime, endTime, retNum, retCode, total);

}

//充值查询统计;
GMSSDK_API	StartisticsInfo* GM_QueryPecharGeStatistcs(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total)
{
	return ((CGMSdkHelper*)handle)->QueryPecharGeStatistcs(platId, channel, partId, startTime, endTime, retNum, retCode, total);
}

//查询留存;
GMSSDK_API	GMQLeavekeepDateInfo* GM_QueryLeaveKeepStatistics(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, const char* starTime, const char* endTime, BYTE lktype, BYTE lkTimeType, BYTE& retNum, UINT16* retCode, UINT32* total)
{
	return ((CGMSdkHelper*)handle)->QueryLeaveKeepStatistics(platId, channel, partId, starTime, endTime, lktype, lkTimeType, retNum, retCode, total);

}

//销毁Handle;
GMSSDK_API void GM_ReleaseHandle(GMHandle handle)
{
	((CGMSdkHelper*)handle)->ReleaseHandle();
}
//在线统计;
GMSSDK_API	OnlineInfo* GM_QueryRealtimeoneline(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, UINT32 from, BYTE to, const char* starTime, BYTE& retNum, UINT16* retCode, UINT32* total)
{
	return ((CGMSdkHelper*)handle)->QueryOnlineCurrent(platId, channel, partId, from, to, starTime, retNum, retCode, total);
}

GMSSDK_API	GM_Grade_RatingStatisticsInfo* GM_QueryLevelDistribution(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, const char* starTime, const char *TendTime, BYTE& retNum, UINT16* retCode, UINT32* total)
{
	return ((CGMSdkHelper*)handle)->QueryGrade_RatingStatisticsInfo(platId, channel, partId, starTime, TendTime, retNum, retCode, total);
}
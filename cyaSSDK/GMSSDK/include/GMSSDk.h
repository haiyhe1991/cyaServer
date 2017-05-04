#ifndef __GMSSDK_H__
#define __GMSSDK_H__

//跨平台动态库连接;
//然后进行实例化，初始化，生成,Heler.cpp,QueryPort.cpp对象，通过，此dll,接口实现登录，退出连接等工作,Heler.cpp则是具体;
#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if defined(GMSSDK_DECL)
	#undef GMSSDK_DECL
	#define GMSSDK_DECL
#endif

#ifndef GMSSDK_DECL
	#ifdef WIN32
		#ifdef GMSSDK_EXPORTS
			#define GMSSDK_DECL __declspec(dllexport)
		#else
			#define GMSSDK_DECL __declspec(dllimport)
		#endif // GMSSDK_EXPORTS
	#else
		#if defined(__GNUC__) && __GNUC__ >= 4
			#define GMSSDK_DECL __attribute__((visibility("default")))
		#else
			#define GMSSDK_DECL
		#endif
	#endif// WIN32
#endif//GMSSDK_DECL


#ifdef __cplusplus     //用c语言作为接口;
	#define GMSSDK_API extern "C" GMSSDK_DECL
#else
	#define GMSSDK_API GMSSDK_DECL
#endif

#include "cyaBase.h"
#include "GMProtocal.h"

typedef void* GMHandle;

//进行连接GM管理器建立连接;
//获取连接handle;（获取socket连接）
GMSSDK_API GMHandle  GM_FetchGMHandle(const char* ip,
	UINT16 port,
	UINT16 connTimeout,
	UINT16 readTimeout = 0,
	UINT16 writeTimeout = 0
	);


//=====================================================================================
//			操作接口;
//=====================================================================================

//注册管理员账号;
//GMSSDK_API int GMS_CreateUser(GMSSHandle handle,const char* name, const char*passwd ,bool*isOk);


///用户登录
GMSSDK_API int GM_UserLogin(GMHandle handle, const char* name, const char* passwd, bool* isOk);


///释放
GMSSDK_API void GM_Del(void* p);

//=====================================================================================
//			查询接口;
//=====================================================================================

///实时新增统计;GMQActorDetailInofReq	GM_ACTOR_DETAIL
///@handle[IN]: handlle;
///@platId[IN]:平台;
///@partId[IN]:分区;
///@startTime[IN]: 起止时间[2015-08-14 09:16:22表示:2015年8月14日9点16分22秒]
///@endTime[IN]: 截至时间;
///@stype[IN]: 查询类型;设备-2（新增、登录、活跃）、账号-2、角色-2;
///@getCount[OUT]: 查询到的数量;
///统计设备、账号、角色的新增、登陆、活跃情况（默认返回相应的数量只返回数量）
GMSSDK_API	TimeAddedinfo* GM_QueryRealTimeAddedStatistics(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total);


///订单查询详情;GMGetOrderTrackingRsq	GM_ORDER_TRACKING
///@handle[IN]: handlle;
///@platId[IN]:平台;
///@partId[IN]:分区;
///@startTime[IN]: 起止时间[2015-08-14 09:16:22表示:2015年8月14日9点16分22秒]
///@endTime[IN]: 截至时间;
///@getCount[OUT]: 查询到的数量;
GMSSDK_API	RecharegeStatisticeIofo* GM_QueryOrderTracking(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, UINT32 from, BYTE to, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total);


///充值查询统计;GMQActorDetailInofReq	GM_ACTOR_DETAIL
///@handle[IN]: handlle;
///@platId[IN]:平台;
///@partId[IN]:分区;
///@startTime[IN]: 起止时间[2015-08-14 09:16:22表示:2015年8月14日9点16分22秒]
///@endTime[IN]: 截至时间;
///@stype[IN]: 查询类型;设备、账号、角色留存(1，2，3)
///@getCount[OUT]: 查询到的数量;
///统计留存用户再登陆相关数据（默认统计设备，账号，角色的留存数量）;
GMSSDK_API	StartisticsInfo* GM_QueryPecharGeStatistcs(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total);



///留存统计;GMQActorDetailInofReq	GM_ACTOR_DETAIL
///@handle[IN]: handlle;
///@platId[IN]:平台;
///@partId[IN]:分区;
///@startTime[IN]: 起止时间[2015-08-14 09:16:22表示:2015年8月14日9点16分22秒]
///@endTime[IN]: 截至时间;
///@stype[IN]: 查询类型;设备、账号、角色留存(1，2，3)
///@getCount[OUT]: 查询到的数量;
///统计留存用户再登陆相关数据（默认统计设备，账号，角色的留存数量）;
GMSSDK_API	GMQLeavekeepDateInfo* GM_QueryLeaveKeepStatistics(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, const char* startTime, const char* endTime, BYTE lktype, BYTE lkTimeType, BYTE& retNum, UINT16* retCode, UINT32* total);



///在线统计;GMQActorDetailInofReq	GM_ACTOR_DETAIL
///@handle[IN]: handlle;
///@platId[IN]:平台;
///@partId[IN]:分区;
///@startTime[IN]: 起止时间[2015-08-14 09:16:22表示:2015年8月14日9点16分22秒]
///@endTime[IN]: 截至时间;	
///@stype[IN]: 查询类型;设备、账号、角色留存(1，2，3)
///@getCount[OUT]: 查询到的数量;
///统计实时在线人数、统计时长、反复登陆情况（）;
///实时在线人数查询;
GMSSDK_API	OnlineInfo* GM_QueryRealtimeoneline(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, UINT32 from, BYTE to, const char* starTime, BYTE& retNum, UINT16* retCode, UINT32* total);


///等级分布统计;GMQActorDetailInofReq	GM_ACTOR_DETAIL
///@handle[IN]: handlle;
///@platId[IN]:平台;
///@partId[IN]:分区;
///@startTime[IN]: 起止时间[2015-08-14 09:16:22表示:2015年8月14日9点16分22秒]
///@endTime[IN]: 截至时间;	
///@stype[IN]: 查询类型;设备、账号、角色留存(1，2，3)
///@getCount[OUT]: 查询到的数量;
///统计实时在线人数、统计时长、反复登陆情况（）;
///实时在线人数查询;
GMSSDK_API	GM_Grade_RatingStatisticsInfo* GM_QueryLevelDistribution(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, const char* starTime, const char* endtTime, BYTE& retNum, UINT16* retCode, UINT32* total);

///销毁Handle
GMSSDK_API void GM_ReleaseHandle(GMHandle handle);

#endif
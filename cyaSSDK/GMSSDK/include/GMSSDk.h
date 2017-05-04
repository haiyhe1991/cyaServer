#ifndef __GMSSDK_H__
#define __GMSSDK_H__

//��ƽ̨��̬������;
//Ȼ�����ʵ��������ʼ��������,Heler.cpp,QueryPort.cpp����ͨ������dll,�ӿ�ʵ�ֵ�¼���˳����ӵȹ���,Heler.cpp���Ǿ���;
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


#ifdef __cplusplus     //��c������Ϊ�ӿ�;
	#define GMSSDK_API extern "C" GMSSDK_DECL
#else
	#define GMSSDK_API GMSSDK_DECL
#endif

#include "cyaBase.h"
#include "GMProtocal.h"

typedef void* GMHandle;

//��������GM��������������;
//��ȡ����handle;����ȡsocket���ӣ�
GMSSDK_API GMHandle  GM_FetchGMHandle(const char* ip,
	UINT16 port,
	UINT16 connTimeout,
	UINT16 readTimeout = 0,
	UINT16 writeTimeout = 0
	);


//=====================================================================================
//			�����ӿ�;
//=====================================================================================

//ע�����Ա�˺�;
//GMSSDK_API int GMS_CreateUser(GMSSHandle handle,const char* name, const char*passwd ,bool*isOk);


///�û���¼
GMSSDK_API int GM_UserLogin(GMHandle handle, const char* name, const char* passwd, bool* isOk);


///�ͷ�
GMSSDK_API void GM_Del(void* p);

//=====================================================================================
//			��ѯ�ӿ�;
//=====================================================================================

///ʵʱ����ͳ��;GMQActorDetailInofReq	GM_ACTOR_DETAIL
///@handle[IN]: handlle;
///@platId[IN]:ƽ̨;
///@partId[IN]:����;
///@startTime[IN]: ��ֹʱ��[2015-08-14 09:16:22��ʾ:2015��8��14��9��16��22��]
///@endTime[IN]: ����ʱ��;
///@stype[IN]: ��ѯ����;�豸-2����������¼����Ծ�����˺�-2����ɫ-2;
///@getCount[OUT]: ��ѯ��������;
///ͳ���豸���˺š���ɫ����������½����Ծ�����Ĭ�Ϸ�����Ӧ������ֻ����������
GMSSDK_API	TimeAddedinfo* GM_QueryRealTimeAddedStatistics(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total);


///������ѯ����;GMGetOrderTrackingRsq	GM_ORDER_TRACKING
///@handle[IN]: handlle;
///@platId[IN]:ƽ̨;
///@partId[IN]:����;
///@startTime[IN]: ��ֹʱ��[2015-08-14 09:16:22��ʾ:2015��8��14��9��16��22��]
///@endTime[IN]: ����ʱ��;
///@getCount[OUT]: ��ѯ��������;
GMSSDK_API	RecharegeStatisticeIofo* GM_QueryOrderTracking(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, UINT32 from, BYTE to, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total);


///��ֵ��ѯͳ��;GMQActorDetailInofReq	GM_ACTOR_DETAIL
///@handle[IN]: handlle;
///@platId[IN]:ƽ̨;
///@partId[IN]:����;
///@startTime[IN]: ��ֹʱ��[2015-08-14 09:16:22��ʾ:2015��8��14��9��16��22��]
///@endTime[IN]: ����ʱ��;
///@stype[IN]: ��ѯ����;�豸���˺š���ɫ����(1��2��3)
///@getCount[OUT]: ��ѯ��������;
///ͳ�������û��ٵ�½������ݣ�Ĭ��ͳ���豸���˺ţ���ɫ������������;
GMSSDK_API	StartisticsInfo* GM_QueryPecharGeStatistcs(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, const char* startTime, const char* endTime, BYTE& retNum, UINT16* retCode, UINT32* total);



///����ͳ��;GMQActorDetailInofReq	GM_ACTOR_DETAIL
///@handle[IN]: handlle;
///@platId[IN]:ƽ̨;
///@partId[IN]:����;
///@startTime[IN]: ��ֹʱ��[2015-08-14 09:16:22��ʾ:2015��8��14��9��16��22��]
///@endTime[IN]: ����ʱ��;
///@stype[IN]: ��ѯ����;�豸���˺š���ɫ����(1��2��3)
///@getCount[OUT]: ��ѯ��������;
///ͳ�������û��ٵ�½������ݣ�Ĭ��ͳ���豸���˺ţ���ɫ������������;
GMSSDK_API	GMQLeavekeepDateInfo* GM_QueryLeaveKeepStatistics(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, const char* startTime, const char* endTime, BYTE lktype, BYTE lkTimeType, BYTE& retNum, UINT16* retCode, UINT32* total);



///����ͳ��;GMQActorDetailInofReq	GM_ACTOR_DETAIL
///@handle[IN]: handlle;
///@platId[IN]:ƽ̨;
///@partId[IN]:����;
///@startTime[IN]: ��ֹʱ��[2015-08-14 09:16:22��ʾ:2015��8��14��9��16��22��]
///@endTime[IN]: ����ʱ��;	
///@stype[IN]: ��ѯ����;�豸���˺š���ɫ����(1��2��3)
///@getCount[OUT]: ��ѯ��������;
///ͳ��ʵʱ����������ͳ��ʱ����������½�������;
///ʵʱ����������ѯ;
GMSSDK_API	OnlineInfo* GM_QueryRealtimeoneline(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, UINT32 from, BYTE to, const char* starTime, BYTE& retNum, UINT16* retCode, UINT32* total);


///�ȼ��ֲ�ͳ��;GMQActorDetailInofReq	GM_ACTOR_DETAIL
///@handle[IN]: handlle;
///@platId[IN]:ƽ̨;
///@partId[IN]:����;
///@startTime[IN]: ��ֹʱ��[2015-08-14 09:16:22��ʾ:2015��8��14��9��16��22��]
///@endTime[IN]: ����ʱ��;	
///@stype[IN]: ��ѯ����;�豸���˺š���ɫ����(1��2��3)
///@getCount[OUT]: ��ѯ��������;
///ͳ��ʵʱ����������ͳ��ʱ����������½�������;
///ʵʱ����������ѯ;
GMSSDK_API	GM_Grade_RatingStatisticsInfo* GM_QueryLevelDistribution(GMHandle handle, UINT16 platId, UINT16 channel, UINT16 partId, const char* starTime, const char* endtTime, BYTE& retNum, UINT16* retCode, UINT32* total);

///����Handle
GMSSDK_API void GM_ReleaseHandle(GMHandle handle);

#endif
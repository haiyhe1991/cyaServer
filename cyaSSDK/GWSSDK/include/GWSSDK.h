#ifndef __GWSSDK_H__
#define __GWSSDK_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if defined(GWSSDK_DECL)
	#undef GWSSDK_DECL
	#define GWSSDK_DECL
#endif

#ifndef GWSSDK_DECL
	#ifdef WIN32
		#ifdef GWSSDK_EXPORTS
			#define GWSSDK_DECL __declspec(dllexport)
		#else
			#define GWSSDK_DECL __declspec(dllimport)
		#endif
	#else
		#if defined(__GNUC__) && __GNUC__ >= 4
			#define GWSSDK_DECL __attribute__((visibility("default")))
		#else
			#define GWSSDK_DECL
		#endif
	#endif
#endif

#ifdef __cplusplus
	#define GWSSDK_API extern "C" GWSSDK_DECL
#else
	#define GWSSDK_API GWSSDK_DECL
#endif

#include "GWSProtocol.h"

typedef void* GWSHandle;
typedef void(*fnDataCallback)(SGWSProtocolHead* pHead, const void* payload, int payloadLen, void* context);
typedef void(*fnEventCallback)(EServerEvent evt, const void* data, int nDataLen, void* context);

///���ʼ��
GWSSDK_API void GWS_Init();

///��ȡ����handle
///@gwsIp[IN] : ������IP 
///@dbsPort[IN] : �������˿�
///@connTimeout[IN] : ���ӳ�ʱ(ms)
///@serverType[IN] : ����������
///@cb[IN] : �ص�����
///@context[IN] : �ص������Ĳ���
///@writeTimeout[IN] : ���ͳ�ʱ(ms)
///@readTimeout[IN] : ���ճ�ʱ(ms)
GWSSDK_API GWSHandle GWS_FetchHandle(const char* gwsIp, int gwsPort,
		EServerType serverType,
		fnDataCallback cb, void* context,
		unsigned int connTimeout = 5000,
		unsigned int writeTimeout = 0,
		unsigned int readTimeout = 0);


///�����¼�֪ͨ�ص�����
GWSSDK_API void GWS_SetEventCallback(GWSHandle handle, fnEventCallback cb, void* context);

///��½
///@handle[IN] : GWS_FetchHandle����ֵ
GWSSDK_API int  GWS_Login(GWSHandle handle);

///�˳�
///@handle[IN] : GWS_FetchHandle����ֵ
GWSSDK_API void  GWS_Logout(GWSHandle handle);

///��������(һ���������������ݷ�����)
///@data[IN] : Ҫ���͵����ݵ�ַ
///@dataBytes[IN] : Ҫ���͵������ֽ���
///@targetId[IN] : ����Ŀ����������
///@userId[IN] : ��һỰid(һ������ҿͻ���������Link��������socket��Ϊ��id)
///@pktType[IN] : ������
///@isCrypt[IN] : �Ƿ����(0-������,1-����)
///@seq[IN] : �����()
GWSSDK_API int   GWS_SendDataPacket(GWSHandle handle, const void* data,
	int dataBytes, int targetId,
	unsigned int userId, ESGSPacketType pktType,
	int isCrypt, long* seq = NULL);

///��������(�ܵ��ص�fnDataCallback���ݺ�ظ�)
///@pHead[IN] : ����Э��ͷ
///@data[IN] : Ҫ���͵����ݵ�ַ
///@dataBytes[IN] : Ҫ���͵������ֽ���
GWSSDK_API int   GWS_SendDataPacketEx(GWSHandle handle, const SGWSProtocolHead* pHead, const void* data, int dataBytes);

///�ͷ�handle
///@handle[IN] : GWS_FetchHandle����ֵ
GWSSDK_API void GWS_ReleaseHandle(GWSHandle handle);

///������
GWSSDK_API void GWS_Cleanup();


#endif
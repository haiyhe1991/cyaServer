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

///库初始化
GWSSDK_API void GWS_Init();

///获取连接handle
///@gwsIp[IN] : 服务器IP 
///@dbsPort[IN] : 服务器端口
///@connTimeout[IN] : 连接超时(ms)
///@serverType[IN] : 服务器类型
///@cb[IN] : 回调函数
///@context[IN] : 回调上下文参数
///@writeTimeout[IN] : 发送超时(ms)
///@readTimeout[IN] : 接收超时(ms)
GWSSDK_API GWSHandle GWS_FetchHandle(const char* gwsIp, int gwsPort,
		EServerType serverType,
		fnDataCallback cb, void* context,
		unsigned int connTimeout = 5000,
		unsigned int writeTimeout = 0,
		unsigned int readTimeout = 0);


///设置事件通知回调函数
GWSSDK_API void GWS_SetEventCallback(GWSHandle handle, fnEventCallback cb, void* context);

///登陆
///@handle[IN] : GWS_FetchHandle返回值
GWSSDK_API int  GWS_Login(GWSHandle handle);

///退出
///@handle[IN] : GWS_FetchHandle返回值
GWSSDK_API void  GWS_Logout(GWSHandle handle);

///发送数据(一般由主动发送数据方调用)
///@data[IN] : 要发送的数据地址
///@dataBytes[IN] : 要发送的数据字节数
///@targetId[IN] : 发送目标服务器编号
///@userId[IN] : 玩家会话id(一般用玩家客户端连接上Link服务器的socket作为该id)
///@pktType[IN] : 包类型
///@isCrypt[IN] : 是否加密(0-不加密,1-加密)
///@seq[IN] : 包序号()
GWSSDK_API int   GWS_SendDataPacket(GWSHandle handle, const void* data,
	int dataBytes, int targetId,
	unsigned int userId, ESGSPacketType pktType,
	int isCrypt, long* seq = NULL);

///发送数据(受到回调fnDataCallback数据后回复)
///@pHead[IN] : 网关协议头
///@data[IN] : 要发送的数据地址
///@dataBytes[IN] : 要发送的数据字节数
GWSSDK_API int   GWS_SendDataPacketEx(GWSHandle handle, const SGWSProtocolHead* pHead, const void* data, int dataBytes);

///释放handle
///@handle[IN] : GWS_FetchHandle返回值
GWSSDK_API void GWS_ReleaseHandle(GWSHandle handle);

///库清理
GWSSDK_API void GWS_Cleanup();


#endif
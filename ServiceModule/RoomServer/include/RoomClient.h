/************************************************************************/
/// 与房间服务器通信
/// 1、生成进入密码
/// 2、接受进程信息并上报RoomServer进程信息(IP、Port、Password)
/// 3、接受完成连接消息并上报RoomServer(唯一关联ID、完成时间)
/************************************************************************/
#ifndef __ROOM_CLIENT_H__
#define __ROOM_CLIENT_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

#include "cyaCoreTimer.h"
#include "RoomSessionMaker.h"
#include "ServiceRoomDaemon.h"

typedef void pfnRoomClientConnectStatusCallBack(ERoomConnectStatus eStatus, void* pContent);
typedef void pfnRoomClientDataCallBack(const void* pData, const INT nDataLen, void* pContent);

class CRoomClient
{
public:
	static CRoomClient* FetchRoomClientInstance();
	static void ReleaseRoomClientInstance();

	/// 启动与房间服务器的连接，异步。可以通过壮态回调(SetRoomClientConnectStatusCallBack)或者(GetRoomClientConnectStatus)获取到连接壮态
	INT RoomClientStart();
	void RoomClientStop();

	ERoomConnectStatus GetRoomClientConnectStatus() const	{ return m_eRoomConStatus; }
	void SetRoomClientConnectStatusCallBack(pfnRoomClientConnectStatusCallBack* pfnCallBack, void* pUserContent);

	void SetRoomClientDataCallBack(pfnRoomClientDataCallBack* pfnCallBack, void* pUserContent);
	INT  SendRoomClientData(const void* pData, const INT nDataLen);

	BOOL DaemonServerSessionRegist();
private:
	CRoomClient();
	~CRoomClient();

	CRoomClient(const CRoomClient&);
	CRoomClient& operator=(const CRoomClient&);
private:
	void ConnectRoomServer(DWORD dwConnectTimeout = 15000);
	static void ConnectTimerCallBack(void* pParam, TTimerID id);
	void OnCconnectWork();
private:
	static CRoomClient* m_this;
	CRoomSessionMaker* m_pSessionMaker;
	ICyaTcpServeMan*  m_pSessionServe;
	ERoomConnectStatus m_eRoomConStatus;
	pfnRoomClientConnectStatusCallBack* m_pfnStatusCallBcak;
	void* m_pStatusUserContent;
	pfnRoomClientDataCallBack* m_pfnDataCallBack;
	void* m_pDataUserContent;

	CyaCoreTimer m_iConnectTimer;
};

#endif
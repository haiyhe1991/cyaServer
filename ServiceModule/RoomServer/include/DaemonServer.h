/************************************************************************/
/// 1、创建进程，进程管理(唯一关联ID、密码、连接数(UE3Server多少人的房间))
/// 2、上报进程信息(IP、Port、Password)
/// 3、接受所有客户端完成连接消息，通知RoomClient完成连接(唯一关联ID、完成时间)
/// 4、通知玩家断开连接
/// 5、通知RoomServer房间关闭
/// 6、汇报进程数
/// 7、汇报比赛结果(并结束房间)，汇报数据以字符串数据形式，支持分页大数据发送。
/// 和UE3Server心跳，防止僵死
/************************************************************************/

#ifndef __DAEMON_SERVER_H__
#define __DAEMON_SERVER_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "DaemonSessionMaker.h"
#include "ServiceRoomDaemon.h"

class CDaemonServer
{
public:
	static CDaemonServer* FetchDaemonServerInstance();
	static void ReleaseDaemonServerInstance();

	INT  DaemonServerStart();
	void DaemonServerStop();

	BOOL DaemonServerIsListening() const;

private:
	CDaemonServer();
	~CDaemonServer();

	CDaemonServer(const CDaemonServer&);
	CDaemonServer& operator=(const CDaemonServer&);
private:
	static void OnRoomClientConnectStatusCallBack(ERoomConnectStatus eStatus, void* pContent);
	static void OnRoomClientDataCallBack(const void* pData, const INT nDataLen, void* pContent);

	void OnRoomClientConnectStatusCallBack(ERoomConnectStatus eStatus);
	void OnRoomClientDataCallBack(const void* pData, const INT nDataLen);
private:
	static CDaemonServer* m_this;
	CDaemonSessionMaker* m_pSessionMaker;
	ICyaTcpServeMan* m_pSessionServ;
	BOOL m_bListenStatus;
};

#endif
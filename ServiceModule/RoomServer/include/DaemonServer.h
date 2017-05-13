/************************************************************************/
/// 1���������̣����̹���(Ψһ����ID�����롢������(UE3Server�����˵ķ���))
/// 2���ϱ�������Ϣ(IP��Port��Password)
/// 3���������пͻ������������Ϣ��֪ͨRoomClient�������(Ψһ����ID�����ʱ��)
/// 4��֪ͨ��ҶϿ�����
/// 5��֪ͨRoomServer����ر�
/// 6���㱨������
/// 7���㱨�������(����������)���㱨�������ַ���������ʽ��֧�ַ�ҳ�����ݷ��͡�
/// ��UE3Server��������ֹ����
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
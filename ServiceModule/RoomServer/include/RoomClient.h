/************************************************************************/
/// �뷿�������ͨ��
/// 1�����ɽ�������
/// 2�����ܽ�����Ϣ���ϱ�RoomServer������Ϣ(IP��Port��Password)
/// 3���������������Ϣ���ϱ�RoomServer(Ψһ����ID�����ʱ��)
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

	/// �����뷿������������ӣ��첽������ͨ��׳̬�ص�(SetRoomClientConnectStatusCallBack)����(GetRoomClientConnectStatus)��ȡ������׳̬
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
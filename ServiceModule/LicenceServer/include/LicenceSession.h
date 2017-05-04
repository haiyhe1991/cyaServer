#ifndef __LICENCE_SESSION_H__
#define __LICENCE_SESSION_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTcp.h"
#include "ServiceProtocol.h"
#include "LCSProtocol.h"

class LicenceSessionMaker : public ICyaTcpSessionMaker
{
public:
	LicenceSessionMaker();
	~LicenceSessionMaker();

	virtual int GetSessionObjSize();
	virtual void MakeSessionObj(void* session);
};

class LicenceSession : public CyaTcpSessionTmpl
{
public:
	LicenceSession();
	~LicenceSession();

	virtual void OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL);
	virtual void OnConnectedSend();
	virtual void OnClosed(int cause);
	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify);
	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv);

	enum LicenceSessionType
	{
		linker_session,
		user_session,
		external_man_session,
		other_session,
	};

	DWORD GetSessionTick()
	{
		return m_sessionTick;
	}

	void SetSessionTick(DWORD tick)
	{
		m_sessionTick = tick;
	}

	void SetId(UINT32 id)
	{
		m_id = id;
	}

	UINT32 GetId() const
	{
		return m_id;
	}

	void SetSessionType(LicenceSessionType sessType)
	{
		m_sessionType = sessType;
	}

	LicenceSessionType GetSessionType() const
	{
		return m_sessionType;
	}

	void SetPartitionId(UINT16 partitionId)
	{
		m_partitionId = partitionId;
	}

	UINT16 GetPartitionId() const
	{
		return m_partitionId;
	}

public:
	//停止/开启linker服务
	int  ControlLinkerServe(BOOL startORstop);

	//发送获取token命令到linker
	int  UserEnterLinkerGetToken(UINT32 userId, const char* username);

	//普通回复
	int SendCommonReply(UINT16 cmdCode, UINT16 retCode, BOOL upToken = true, ESGSPacketType pktType = DATA_PACKET);

	//发送用户进入分区回复数据
	int  SendUserEnterLinkerReply(UINT16 partitionId, UINT32 userId, const char* token, UINT32 linkerIp, UINT16 linkerPort, int retCode);

	//通知linker用户下线
	int  NotifyUserOffline(UINT32 userId);

	//获取已解锁职业
	int GetUnlockJobs(UINT32 userId);

	//发送数据回复
	int SendDataReply(BYTE* buf, int nRawPayloadLen, UINT16 cmdCode, UINT16 retCode, BOOL upToken = true, BYTE pktType = DATA_PACKET);

private:
	//处理数据包
	int OnProcessPacket(const void* payload, int nPayloadLen);

	//处理DATA_PACKET
	int OnProcessDataPacket(const void* payload);

	//处理DATA_CTRL_PACKET
	int OnProcessCtrlPacket(const void* payload);

private:
	BOOL IsValidCommunicationToken(UINT32 u32Token)
	{
		return u32Token == m_communicationToken ? true : false;
	}

private:
	enum ERecvStatus
	{
		recv_header,
		recv_payload
	};

	UINT32 m_id;			//(用户id/linker id/...)
	UINT16 m_partitionId;	//link所在分区id
	DWORD m_sessionTick;	//上线时间
	LicenceSessionType m_sessionType;
	UINT32 m_communicationToken;	//通信令牌
	ERecvStatus m_recvStatus;
	SGSProtocolHead m_sgsProHeader;
	char m_key[DEFAULT_KEY_SIZE + 1];
};


#endif
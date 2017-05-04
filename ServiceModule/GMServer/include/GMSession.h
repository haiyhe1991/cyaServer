#ifndef __GM_SESSION_H__
#define __GM_SESSION_H__

#include "cyaTcp.h"
#include "cyaLog.h"
#include "cyaLocker.h"
#include "GMComm.h"
#include "ServiceProtocol.h"
#include "GMProcessDataMan.h"

class CGMSessionMaker : public ICyaTcpSessionMaker
{
public:
	CGMSessionMaker();
	~CGMSessionMaker();
	virtual int GetSessionObjSize();
	virtual void MakeSessionObj(void* session);
};

class CCMSession : public CyaTcpSessionTmpl
{
public:
	CCMSession();
	~CCMSession();

	virtual void OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL);
	virtual void OnConnectedSend();
	virtual void OnClosed(int cause);
	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify);
	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv);
	virtual void OnSent(const void* buf, int bufLen, BOOL isCopy, void* context, BOOL isSendSuccess);


public:
	char* GetKey()
	{
		return m_cryptKey;
	}

private:
	///�������ݴ���
	int ProcessDataMan(void* data, int dataLen, DWORD req);
	///������
	void ErrorMan(UINT16 cmdCode, UINT16 retCode, DWORD req);
	///������֤
	int PasswordVerify(const char* name, const char* passwd);

	///��¼��֤
	int OnLogin(void* data, int dataLen, DWORD req);
	///�Ƴ��Ự
	void RemoveSession(UINT32 userId);
	///�����յ���Msg
	int OnMsgRequestMan(UINT16 cmdCode, void* data, int dataLen, DWORD req);
	///�������ݰ�
	void EncryptDatapack(BYTE isCrypt, BYTE*& buf, UINT16 uncryptLen, UINT16 sourceHead, char* key, UINT16* cryptLen);

	///�������ݴ���
	void OnHeartbeatMan();

private:
	enum RecvStatus
	{
		RECV_HEAD,
		RECV_BODY
	};

	RecvStatus m_recvStatus;
	GMProtocalHead m_gmshead;
	char m_cryptKey[33];
	CGMProcessDataMan m_processDataMan;
	UINT32 m_userId;
	BOOL m_isLogin;

	CXTLocker m_locker;
	std::map<UINT32, CCMSession*> m_sessionMan;



};
#endif
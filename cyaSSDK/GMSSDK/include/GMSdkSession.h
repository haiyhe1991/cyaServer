#ifndef __GM_SDK_SESSION_H__
#define __GM_SDK_SESSION_H__

#include "cyaTcp.h"
#include "GMProtocal.h"

class CGMSdkHelper;
class GMSdkSesssionMaker : public ICyaTcpSessionMaker
{
public:
	GMSdkSesssionMaker();
	~GMSdkSesssionMaker();

	virtual int GetSessionObjSize();
	virtual void MakeSessionObj(void* session);
};

class GMSdkSesssion : public CyaTcpSessionTmpl
{
public:
	GMSdkSesssion();
	~GMSdkSesssion();

	virtual void OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL);
	virtual void OnClosed(int cause);
	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify);
	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv);
	virtual void OnSent(const void* buf, int bufLen, BOOL isCopy, void* context, BOOL isSendSuccess);

	void SetGMSdkHelper(CGMSdkHelper* p = NULL)
	{
		m_pgmHelper = p;
	}


private:
	enum RecvStatus
	{
		RECV_HEAD,
		RECV_BODY
	};

	RecvStatus m_recvStatus; //����״̬
	GMProtocalHead m_prohead; //ͨ��Э��ͷ
	CGMSdkHelper* m_pgmHelper; //sdk helper����


};

#endif
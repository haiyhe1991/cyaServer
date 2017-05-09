#ifndef __LINK_SESSION_H__
#define __LINK_SESSION_H__

#include "cyaTcp.h"
#include "ServiceProtocol.h"

class LinkSession : public CyaTcpSessionTmpl
{
public:
	LinkSession();
	~LinkSession();

	virtual void OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param = NULL);
	virtual void OnConnectedSend();
	virtual void OnClosed(int cause);

	virtual int  RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify);
	virtual int  OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv);

	void InputPacket(bool encrypt, UINT16 nPktType, UINT16 cmdCode, UINT16 retCode, const void* pktData, UINT16 pktLen);
	/// ���δ������ݷ��ִ���ʱ����Ϣ�ظ�
	int ResReturnCode(bool bEncrypt, UINT16 nPktType, UINT16 nCmdCode, UINT16 nRetCode);
private:
	/// �������ݵ��ͻ���
	void InputPacket(const void* nPktData, UINT16 nPktLen);
	/// �յ����ݴ������
	int OnRoughMsgSite(void* pBuf, int nBufLen, bool bIsCrypt, BYTE* pKey, UINT16 nPktType);
	int OnLogIn(const void* pLoginData, UINT16 nDataLen);
	int OnLogOut();
	int OnHeartBeat();
	int OnGetCurrentLinkerOnlineNum(UINT32 onlineCount);

	UINT16 OnCreateKeyPacket(BYTE*& buf, const BYTE* token, const BYTE* data);

	void OnInputUnencryptPakcet(UINT16 nPktType, UINT16 nCmdCode, UINT16 cRetCode, const void* pPktData, UINT16 nPktLen);
	void OnInputEncryptPakcet(UINT16 nPktType, UINT16 nCmdCode, UINT16 nRetCode, const void* pPktData, UINT16 nPktLen);
private:
	typedef enum
	{
		CYA_HEAD = 0,
		CYA_BODY
	} CYA_DATA_TYPE;

	CYA_DATA_TYPE m_ePacketContrl;

	BOOL m_bLongin;
	UINT32 m_nUserID;
	BYTE m_szKey[DEFAULT_KEY_SIZE + 1];		/// ͨ��������Կ
	BYTE m_szToken[DEFAULT_TOKEN_SIZE + 1];	/// ͨ��Э��Token,��ͻ��˻ظ���Ϣ(�������ݳ���)����
	SGSProtocolHead m_proHeader;
};

#endif
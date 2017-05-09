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
	/// 初次处理数据发现错误时的消息回复
	int ResReturnCode(bool bEncrypt, UINT16 nPktType, UINT16 nCmdCode, UINT16 nRetCode);
private:
	/// 发送数据到客户端
	void InputPacket(const void* nPktData, UINT16 nPktLen);
	/// 收到数据处理入口
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
	BYTE m_szKey[DEFAULT_KEY_SIZE + 1];		/// 通信数据密钥
	BYTE m_szToken[DEFAULT_TOKEN_SIZE + 1];	/// 通信协议Token,向客户端回复消息(聊天数据除外)更新
	SGSProtocolHead m_proHeader;
};

#endif
#include "ExtServerTcpSession.h"
#include "RandomString.h"
#include "PubFunction.h"
#include "AES.h"
#include "ServiceExtMan.h"
#include "DelegetMsg.h"
#include "ServiceErrorCode.h"

ESTcpSession::ESTcpSession()
: m_recv_type(PACKET_HEAD)
{

}

ESTcpSession::~ESTcpSession()
{

}

void ESTcpSession::OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param /*= NULL*/)
{
	ASSERT(sock>0 && 0<sessionCount);
}

void ESTcpSession::OnConnectedSend()
{

}


void ESTcpSession::OnClosed(int cause)
{

}

int  ESTcpSession::RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify)
{
	rt = ICyaTcpSession::io_mustRead;
	if (PACKET_HEAD == m_recv_type)
	{
		buf = &m_proHeader;
		bufLen = SGS_PROTO_HEAD_LEN;
		memset(buf, 0, bufLen);
		asynNotify = FALSE;
	}
	else if (PACKET_BODY == m_recv_type)
	{
		asynNotify = TRUE;
		buf = CyaTcp_Alloc(m_proHeader.pduLen + 1);
		ASSERT(buf);
		bufLen = m_proHeader.pduLen;
	}
	else
		ASSERT(FALSE);
	return 0;
}

int  ESTcpSession::OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv)
{
	ASSERT(buf && 0 < bufLen);
	if (NULL == buf || 0 >= bufLen)
	{
		return -1;
	}

	continueRecv = TRUE;
	if (PACKET_HEAD == m_recv_type)
	{
		ASSERT(SGS_PROTO_HEAD_LEN == bufLen);
		m_proHeader.ntoh();
		m_recv_type = PACKET_BODY;
	}
	else if (PACKET_BODY == m_recv_type)
	{
		ASSERT(bufLen == m_proHeader.pduLen);
		int delegate_ret = 0;
		DelegetMessage* pDeleget = DelegetMessage::InstanceDeleteget();
		ASSERT(pDeleget);
		if (DATA_CONFIRM_PACKET == m_proHeader.pktType)
		{
			appDecryptDataWithKey((AES_BYTE*)buf, bufLen, (ANSICHAR*)DEFAULT_TOKEN_KEY);
			delegate_ret = pDeleget->DelegetMsgHandle(buf, bufLen, this, 0);
		}
		else
		{
			UINT32* pToken = (UINT32*)buf;
			const char* key = pDeleget->FindEncryptKey(*pToken);	/// 相当于验证登陆
			if (NULL == key)
			{
				return pDeleget->ResponseErrMsg(0, 0, EXT_NOT_LOGIN, this, DEFAULT_TOKEN_KEY);
			}
			appDecryptDataWithKey((AES_BYTE*)(pToken + 1), bufLen - sizeof(UINT32), (ANSICHAR*)key);
			pDeleget->UpdateSessionLot(*pToken, this);	/// 第一次Session会不存在,后面更新保存的Session，以便回复对应的Session
			delegate_ret = pDeleget->DelegetMsgHandle(pToken + 1, bufLen - sizeof(UINT32), this, *pToken);
		}
		m_recv_type = PACKET_HEAD;
		return delegate_ret;
	}
	else
		ASSERT(FALSE);
	return 0;
}


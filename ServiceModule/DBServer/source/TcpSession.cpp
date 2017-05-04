#include "TcpSession.h"
#include "MsgProcess.h"
#include "cyaLog.h"

DBTcpSession::DBTcpSession()
	: m_eRecvType(RecvHead)
	, m_msgData((char*)CyaTcp_Alloc(MAX_PACKET))
	, m_msgOffet(0)
{

}

DBTcpSession::~DBTcpSession()
{
	CyaTcp_Free(m_msgData);
}

void  DBTcpSession::OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param /*= NULL*/)
{
	LogDebug(("New A Client Connect; Session Count %d", sessionCount));
	GetMsgProObj().SessionRegist(this);
}

void DBTcpSession::OnClosed(int cause)
{
	LogDebug(("Client Closed"));
	GetMsgProObj().SessionUnRegist(this);
}

int  DBTcpSession::RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify)
{
	rt = DBTcpSession::io_mustRead;
	if (RecvHead == m_eRecvType)
	{
		bufLen = sizeof(SDBSProtocolHead);
		buf = static_cast<void*>(&m_sProHead);
		memset(&m_sProHead, 0, bufLen);
		asynNotify = false;
	}
	else if (RecvBody == m_eRecvType)
	{
		bufLen = m_sProHead.pktLen;
		buf = CyaTcp_Alloc(m_sProHead.pktLen + 1);
		static_cast<char*>(buf)[m_sProHead.pktLen] = '\0';
		asynNotify = true;
	}
	else
	{
		ASSERT(false);
	}

	return 0;
}

int  DBTcpSession::OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv)
{
	if (NULL == buf)
	{
		ASSERT(NULL != buf);
		return -1;
		continueRecv = false;
	}
	continueRecv = true;

	if (RecvHead == m_eRecvType)
	{
		ASSERT(sizeof(SDBSProtocolHead) == bufLen);
		m_eRecvType = RecvBody;
		m_sProHead.ntoh();
	}
	else if (RecvBody == m_eRecvType)
	{
		ASSERT(m_sProHead.pktLen == bufLen);
		m_eRecvType = RecvHead;

		if (m_msgOffet + bufLen >= MAX_PACKET)
		{
			char* tmpb = (char*)CyaTcp_Alloc(m_msgOffet + bufLen + 1);
			memcpy(tmpb, m_msgData, m_msgOffet);
			tmpb[m_msgOffet] = '\0';
			CyaTcp_Free(m_msgData);
			m_msgData = tmpb;
		}

		memcpy(m_msgData + m_msgOffet, buf, bufLen);
		m_msgOffet += bufLen;
		m_msgData[m_msgOffet] = '\0';
		CyaTcp_Free(buf);

		if (!m_sProHead.pktOver)
		{
			return 0;
		}
		int retMsgNo = GetMsgProObj().MessageProxy(this, m_msgData, m_msgOffet);
		m_msgOffet = 0;
		if (0>retMsgNo)
		{
			LogDebug(("ProxyMsg Error %d ", retMsgNo));
			return -1;
		}
	}
	else
	{
		ASSERT(false);
		return -1;
	}

	return 0;
}

int DBTcpSession::OnSubmitCmdPacket(const void* data, int bufLen, bool lastPacket)
{
	/// if (NULL==data || 0==bufLen) 发送结束标记
	// 	if (NULL==data || 0>=bufLen)
	// 		return -1;

	char* SubBuf = (char*)CyaTcp_Alloc(bufLen + sizeof(SDBSProtocolHead) + 1);
	ASSERT(SubBuf);
	SDBSProtocolHead* pHead = (SDBSProtocolHead*)SubBuf;
	pHead->pktLen = bufLen;
	pHead->pktOver = lastPacket;
	pHead->seq = m_sProHead.seq;
	pHead->hton();
	if (NULL != data && 0 < bufLen)
		memcpy(SubBuf + sizeof(SDBSProtocolHead), data, bufLen);

	GetServeMan()->SendSessionData(this, SubBuf, bufLen + sizeof(SDBSProtocolHead));
	CyaTcp_Free(SubBuf);

	return 0;
}

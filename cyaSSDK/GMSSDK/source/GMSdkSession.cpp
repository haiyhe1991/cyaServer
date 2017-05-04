#include "GMSdkSession.h"
#include "GMComm.h"
#include "GMSdkHelper.h"

GMSdkSesssionMaker::GMSdkSesssionMaker()
{

}


GMSdkSesssionMaker::~GMSdkSesssionMaker()
{

}

int GMSdkSesssionMaker::GetSessionObjSize()
{
	return (int)sizeof(GMSdkSesssion);
}


void GMSdkSesssionMaker::MakeSessionObj(void* session)
{
	::new(session)GMSdkSesssion();
}

GMSdkSesssion::GMSdkSesssion()
			:m_recvStatus(RECV_HEAD),
			m_pgmHelper(NULL)
{
	memset(&m_prohead, 0, GET_GM_PROTOHEAD_SIZE);
}

GMSdkSesssion::~GMSdkSesssion()
{

}

void GMSdkSesssion::OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param /* = NULL */)
{
	sessionCount; peerIPP; sock;
	m_pgmHelper = (CGMSdkHelper*)param;
	ASSERT(m_pgmHelper != NULL);
	m_pgmHelper->SetLocalSession(this);

}

void GMSdkSesssion::OnClosed(int cause)
{

	LogInfo(("会话关闭服务器关闭[%d]", cause));
	if (m_pgmHelper != NULL)
	{
		m_pgmHelper->StartReconnect();
		m_pgmHelper = NULL;
	}
}

int  GMSdkSesssion::RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify)
{
	if (m_recvStatus == RECV_HEAD)
	{
		buf = &m_prohead;
		bufLen = GET_GM_PROTOHEAD_SIZE;
		rt = ICyaTcpSession::io_mustRead;
		asynNotify = false;
	}
	else if (m_recvStatus == RECV_BODY)
	{
		if (m_prohead.pduLen == 0)
		{
			ASSERT(false);
			return -1;
		}
		buf = CyaTcp_Alloc(m_prohead.pduLen + 1);
		ASSERT(buf != NULL);
		bufLen = m_prohead.pduLen;
		rt = ICyaTcpSession::io_mustRead;
		asynNotify = true;
	}
	else
		ASSERT(false);
	return 0;
}

int  GMSdkSesssion::OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv)
{
	continueRecv = true;
	callbackByTask;
	if (m_recvStatus == RECV_HEAD)
	{
		m_prohead.ntoh();
		if (m_prohead.pktType == DATA_HEARTBEAT_PACKET)
		{
			m_recvStatus = RECV_HEAD;
			m_pgmHelper->UpdateHeatbeatTime();
			return 0;
		}
		if (m_prohead.pduLen == 0)
		{
			m_recvStatus = RECV_HEAD;
			ASSERT(false);
			return -1;
		}
		else
			m_recvStatus = RECV_BODY;
	}
	else if (m_recvStatus == RECV_BODY)
	{
		m_recvStatus = RECV_HEAD;
		///process data man
		m_pgmHelper->InputProcessData(m_prohead.pktType, buf, bufLen, m_prohead.req, m_prohead.isOver);
	}
	else
		ASSERT(false);
	return 0;
}

void GMSdkSesssion::OnSent(const void* buf, int bufLen, BOOL isCopy, void* context, BOOL isSendSuccess)
{
	buf; bufLen; isCopy; context; isSendSuccess;
}
#include "GWSClientSession.h"
#include "GWSHelper.h"

GWSClientSessionMaker::GWSClientSessionMaker()
{

}

GWSClientSessionMaker::~GWSClientSessionMaker()
{

}

int  GWSClientSessionMaker::GetSessionObjSize()
{
	return sizeof(GWSClientSession);
}

void GWSClientSessionMaker::MakeSessionObj(void* session)
{
	::new(session)GWSClientSession();
}

GWSClientSession::GWSClientSession()
		: m_status(recv_data_header)
		, m_gwsHelper(NULL)
{

}

GWSClientSession::~GWSClientSession()
{

}

void GWSClientSession::OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param/* = NULL*/)
{
	ASSERT(m_gwsHelper == NULL);
	m_gwsHelper = (GWSHelper*)param;
	m_status = recv_data_header;
	m_gwsHelper->SetLocalSession(this);
}

void GWSClientSession::OnClosed(int /*cause*/)
{
	if (m_gwsHelper != NULL)
	{
		m_gwsHelper->NotifySessionBroken();
		m_gwsHelper = NULL;
	}
	m_status = recv_data_header;
}

int  GWSClientSession::RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify)
{
	if (m_status == recv_data_header)
	{
		buf = &m_protoHeader;
		bufLen = sizeof(SGWSProtocolHead);
		rt = ICyaTcpSession::io_mustRead;
		asynNotify = false;
	}
	else if (m_status == recv_data_body)
	{
		if (m_protoHeader.pduLen <= 0)
		{
			ASSERT(false);
			return -1;
		}
		buf = CyaTcp_Alloc(m_protoHeader.pduLen + 1);
		ASSERT(buf != NULL);
		if (buf == NULL)
			return -1;
		((char*)buf)[m_protoHeader.pduLen] = '\0';
		bufLen = m_protoHeader.pduLen;
		rt = ICyaTcpSession::io_mustRead;
		asynNotify = /*false*/true;
	}
	else
		ASSERT(false);
	return 0;
}

int  GWSClientSession::OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv)
{
	continueRecv = true;
	if (m_status == recv_data_header)
	{
		ASSERT(bufLen == sizeof(SGWSProtocolHead));
		m_protoHeader.ntoh();
		m_status = recv_data_body;
	}
	else if (m_status == recv_data_body)
	{
		ASSERT(bufLen == m_protoHeader.pduLen);
		m_gwsHelper->InputData(&m_protoHeader, buf, bufLen);
		CyaTcp_Free(buf);
		m_status = recv_data_header;
	}
	else
		ASSERT(false);
	return 0;
}
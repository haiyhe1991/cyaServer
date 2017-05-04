#include "DBSSession.h"
#include "DBSHelper.h"

DBSSessionMaker::DBSSessionMaker()
{

}

DBSSessionMaker::~DBSSessionMaker()
{

}

int DBSSessionMaker::GetSessionObjSize()
{
	return sizeof(DBSSession);
}

void DBSSessionMaker::MakeSessionObj(void* session)
{
	::new(session)DBSSession();
}

DBSSession::DBSSession()
			: m_recvStatus(read_header)
			, m_bodyBuf(NULL)
			, m_dbsHelper(NULL)
{

}

DBSSession::~DBSSession()
{

}

void  DBSSession::OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param /*= NULL*/)
{
	m_recvStatus = read_header;
	m_dbsHelper = (DBSHelper*)param;
	ASSERT(m_dbsHelper != NULL);
	m_dbsHelper->SetLocalSession(this);
}

void DBSSession::OnClosed(int /*cause*/)
{
	ASSERT(m_dbsHelper != NULL);
	if (m_bodyBuf != NULL)
	{
		CyaTcp_Free(m_bodyBuf);
		m_bodyBuf = NULL;
	}
	m_dbsHelper->NotifySessionBroken();
	m_dbsHelper = NULL;
	m_recvStatus = read_header;
}

int  DBSSession::RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify)
{
	asynNotify = false;
	if (m_recvStatus == read_header)
	{
		buf = &m_header;
		bufLen = sizeof(SDBSProtocolHead);
		rt = ICyaTcpSession::io_mustRead;
	}
	else if (m_recvStatus == read_body)
	{
		WORD nBytes = m_header.pktLen;
		buf = CyaTcp_Alloc(nBytes + 1);
		if (buf == NULL)
		{
			ASSERT(false);
			return -1;
		}
		((char*)buf)[nBytes] = 0;
		bufLen = nBytes;
		m_bodyBuf = buf;
		rt = ICyaTcpSession::io_mustRead;
	}
	else
		ASSERT(false);
	return 0;
}

int  DBSSession::OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv)
{
	continueRecv = true;
	if (m_recvStatus == read_header)
	{
		ASSERT(bufLen == sizeof(SDBSProtocolHead));
		ASSERT(buf != NULL);
		m_header.ntoh();
		m_recvStatus = read_body;
	}
	else if (m_recvStatus == read_body)
	{
		ASSERT(m_dbsHelper != NULL);
		m_dbsHelper->InputResponseData(m_header.seq, (const char*)buf, m_header.pktLen, !!m_header.pktOver);
		m_recvStatus = read_header;
		m_bodyBuf = NULL;
	}
	else
		ASSERT(false);
	return 0;
}
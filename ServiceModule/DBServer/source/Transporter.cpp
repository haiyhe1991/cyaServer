#include "Transporter.h"
#include "cyaTcp.h"
#include "DBSProtocol.h"
#include "TcpSession.h"

Transporter::Transporter(void* theSession)
	: m_session(theSession)
	, m_cache(NULL)
	, m_len(0)
{
	m_cache = (char*)CyaTcp_Alloc(MAX_PACKET);
	ASSERT(m_cache);
	memset(m_cache, 0, MAX_PACKET);
}

Transporter::~Transporter()
{
	if (m_cache)
		CyaTcp_Free(m_cache);
	m_cache = NULL;
	m_len = 0;
	m_session = NULL;
}

int Transporter::Transport(const char* cache,int len,bool last)
{
	int tmplen = len;
	int cache_offset = 0;
	while (m_len + tmplen > MAX_PACKET)
	{
		memcpy(m_cache + m_len, cache + cache_offset, MAX_PACKET - m_len);
		DBTcpSession* ps = (DBTcpSession*)m_session;
		ps->OnSubmitCmdPacket(m_cache, MAX_PACKET, false);
		tmplen -= MAX_PACKET - m_len;
		cache_offset += MAX_PACKET - m_len;
		m_len = 0;
	}

	if (tmplen > 0)
	{
		memcpy(m_cache + m_len, cache + cache_offset, tmplen);
		m_len += tmplen;
	}

	if (last)
	{
		DBTcpSession* ps = (DBTcpSession*)m_session;
		ps->OnSubmitCmdPacket(m_cache, m_len, true);
		memset(m_cache, 0, MAX_PACKET);
		m_len = 0;
	}

	return len;
}

int Transporter::CleanCache(bool Last)
{
	ASSERT(m_session);
	if (NULL == m_session)
		return 0;

	DBTcpSession* ps = (DBTcpSession*)m_session;
	if (m_len)
	{
		ps->OnSubmitCmdPacket(m_cache, m_len, Last);
		memset(m_cache, 0, MAX_PACKET);
		m_len = 0;
	}
	return m_len;
}

int Transporter::SendData(const char* cacheData, int cacheLen, bool Last)
{
	ASSERT(m_session);
	if (NULL == m_session)
		return 0;

	DBTcpSession* ps = (DBTcpSession*)m_session;
	if (cacheLen > 0)
	{
		int sendLen = 0;
		while (cacheLen - ((int)MAX_PACKET + sendLen) > 0)
		{
			ps->OnSubmitCmdPacket(cacheData + sendLen, MAX_PACKET, false);
			sendLen += MAX_PACKET;
		}
		ps->OnSubmitCmdPacket(cacheData + sendLen, cacheLen - sendLen, Last);
		sendLen = 0;
	}
	if ((NULL == cacheData || cacheLen == 0) && Last)
	{
		ps->OnSubmitCmdPacket(NULL, 0, Last);
	}

	return cacheLen;
}
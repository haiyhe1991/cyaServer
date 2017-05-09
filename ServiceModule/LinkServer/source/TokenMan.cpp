#include "TokenMan.h"
#include "RandomString.h"
#include "cyaTcp.h"
#include "LinkConfig.h"
#include "LinkGlobalObj.h"
#include "MsgProcess.h"

DEFINE_FETCH_OBJ(TokenMan)

TokenMan::TokenMan()
	: m_pastDue(10 * 3600 * 1000)	/// 默认失效时间为10分钟
	, m_destroyTimer(TokenTimerCallback, this, 10 * 1000/*1000*60*/, 1000)
{
	DWORD cleanClock = GlobalGetLinkConfig()->GetLinkCleanInvterval();
	if (0 != cleanClock)
	{
		m_pastDue = cleanClock * 1000;
		m_destroyTimer.Start();
	}
}

TokenMan::~TokenMan()
{
	if (m_destroyTimer.IsStarted())
	{
		m_destroyTimer.Stop(TRUE);
	}
	if (!m_tokens.empty())
	{
		CXTAutoLock locke(m_tokenMapLocker);
		AccountTokenMap::iterator itr = m_tokens.begin();
		for (itr; m_tokens.end() != itr; ++itr)
		{
			if (NULL != itr->second)
				itr->second->ReleaseRef();
		}
		m_tokens.clear();
	}
}

std::string TokenMan::CreateToken(UINT32 accountID)
{
	CXTAutoLock locke(m_tokenMapLocker);
	AccountTokenMap::iterator itr = m_tokens.find(accountID);
	if (m_tokens.end() != itr)
		return (char*)(itr->second->GetToken());
	TokenMan::ClientToken* staSite = CyaTcp_New(TokenMan::ClientToken);
	if (NULL == staSite)
		return "";
	VERIFY(m_tokens.insert(std::make_pair(accountID, staSite)).second);

	return (char*)(staSite->GetToken());
}

bool TokenMan::UserOnline(UINT32 accountID, const std::string& accountToken)
{
	CXTAutoLock locke(m_tokenMapLocker);
	AccountTokenMap::iterator itr = m_tokens.find(accountID);
	if ((m_tokens.end() == itr) || (FALSE == itr->second->CompareToken(accountToken)))
	{
		// invalid token
		return FALSE;
	}

	itr->second->OffLine(FALSE);
	return TRUE;
	//return (m_tokens.end() != itr && itr->second->CompareToken(accountToken));
}

void TokenMan::UserOffline(UINT32 accountID)
{
	CXTAutoLock locke(m_tokenMapLocker);
	AccountTokenMap::iterator itr = m_tokens.find(accountID);
	if (m_tokens.end() != itr && NULL != itr->second)
	{
		itr->second->OffLine(TRUE);
	}
}

bool TokenMan::Accept()
{
	CXTAutoLock locke(m_tokenMapLocker);
	return m_tokens.size() > MAX_LINKER_CLIENT_NUM + 128;
}

void TokenMan::TokenTimerCallback(void* param, TTimerID id)
{
	TokenMan* pThis = (TokenMan*)param;
	if (pThis)
		pThis->OnTimerWork();
}

void TokenMan::OnTimerWork()
{
	CXTAutoLock locke(m_tokenMapLocker);
	AccountTokenMap::iterator itr = m_tokens.begin();
	for (itr; m_tokens.end() != itr;)
	{
		if (NULL == itr->second)
		{
			m_tokens.erase(itr++);
			continue;
		}
		if (itr->second->OffLine() && itr->second->IsPastDue(m_pastDue))
		{
			MsgProcess::FetchMsgProcess()->MsgProcessSite(NULL, 0, MLS_USER_EXIT, DATA_PACKET, itr->first, true);
			itr->second->ReleaseRef();
			m_tokens.erase(itr++);
			continue;
		}
		else
		{
			itr->second->FlushTime();
		}
		++itr;
	}
}

TokenMan::ClientToken::ClientToken()
	: m_offLine(true)
	, m_time(GetMsel())
{
	CreateRandomString(m_token, 32);
	m_token[32] = '\0';
}

TokenMan::ClientToken::~ClientToken()
{

}

BOOL TokenMan::ClientToken::OffLine() const
{
	return m_offLine;
}

void TokenMan::ClientToken::OffLine(BOOL fls)
{
	m_offLine = fls;
}

void TokenMan::ClientToken::FlushTime()
{
	m_time = GetMsel();
}

BOOL TokenMan::ClientToken::CompareToken(const std::string& accountToken)
{
	return accountToken == (char*)m_token;
}

BOOL TokenMan::ClientToken::IsPastDue(DWORD pastDue)
{
	return GetMsel() - m_time > pastDue;
}

void TokenMan::ClientToken::DeleteObj()
{
	CyaTcp_Delete(this);
}

const BYTE* TokenMan::ClientToken::GetToken() const
{
	return m_token;
}
#include "LogicLog.h"

#include "ServiceRank.h"
#include "ServiceCmdCode.h"
#include "ServiceProtocol.h"
#include "GWSClientMan.h"

void LogicLog(int level, const TCHAR* fmt, ...)
{
	int nLen = 0;
	char buf[MAX_BUF_LEN] = { 0 };
	SGSLogHead *pLogHead = (SGSLogHead *)buf;
	TCHAR *pwch = (TCHAR *)(pLogHead + 1);

	pLogHead->cmdcode = RANK_SEND_LOG_INFO;
	pLogHead->base.level = level;
	pLogHead->base.module = 2;		//enum ELogModule
	pLogHead->base.time = GetMsel();
	pLogHead->hton();

	va_list va;
	va_start(va, fmt);
	nLen = sprintf_traits(pwch, fmt, va);
	va_end(va);
	nLen += sizeof(SGSLogHead)+1;
	// ±£Ö¤·¢ËÍË³Ðò
	GetGWSClientMan()->GetFristGWSClient()->SendResponse(OTHER_LOGIC_SERVER, 0, MLS_OK, RANK_SEND_LOG_INFO, buf, nLen);
}

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "TcpSessionMaker.h"

DBSessionMaker::DBSessionMaker()
{

}

DBSessionMaker::~DBSessionMaker()
{

}

int DBSessionMaker::GetSessionObjSize()
{
	return sizeof(DBTcpSession);
}

void DBSessionMaker::MakeSessionObj(void* session)
{
	::new(session)DBTcpSession;
}

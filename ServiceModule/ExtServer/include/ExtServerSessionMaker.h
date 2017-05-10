#ifndef __EXT_SERVER_SESSION_MAKER_H__
#define __EXT_SERVER_SESSION_MAKER_H__

#include "ExtServerTcpSession.h"


class ESTcpSessionMaker : public ICyaTcpSessionMaker
{
public:
	ESTcpSessionMaker() {}
	~ESTcpSessionMaker() {}

	virtual int GetSessionObjSize()
	{
		return sizeof(ESTcpSession);
	}

	virtual void MakeSessionObj(void* session)
	{
		::new(session)ESTcpSession;
	}
};


#endif
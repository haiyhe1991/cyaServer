#ifndef __DAEMON_SESSION_MAKER_H__
#define __DAEMON_SESSION_MAKER_H__

#include "DaemonSession.h"

class CDaemonSessionMaker : public ICyaTcpSessionMaker
{
public:
	CDaemonSessionMaker();
	~CDaemonSessionMaker();

	virtual int GetSessionObjSize();

	///构造session对象,总是这样实现{::new(session) XXXSession;}
	virtual void MakeSessionObj(void* session);
};

#endif
#ifndef __DAEMON_SESSION_MAKER_H__
#define __DAEMON_SESSION_MAKER_H__

#include "DaemonSession.h"

class CDaemonSessionMaker : public ICyaTcpSessionMaker
{
public:
	CDaemonSessionMaker();
	~CDaemonSessionMaker();

	virtual int GetSessionObjSize();

	///����session����,��������ʵ��{::new(session) XXXSession;}
	virtual void MakeSessionObj(void* session);
};

#endif
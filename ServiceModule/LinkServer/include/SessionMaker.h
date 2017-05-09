#ifndef __SESSION_MAKER_H__
#define __SESSION_MAKER_H__

#include "LinkSession.h"

class SessionMaker : public ICyaTcpSessionMaker
{
public:
	SessionMaker();
	~SessionMaker();

	virtual int GetSessionObjSize();

	///构造session对象,总是这样实现{::new(session) XXXSession;}
	virtual void MakeSessionObj(void* session);
};

#endif
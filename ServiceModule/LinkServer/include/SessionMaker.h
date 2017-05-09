#ifndef __SESSION_MAKER_H__
#define __SESSION_MAKER_H__

#include "LinkSession.h"

class SessionMaker : public ICyaTcpSessionMaker
{
public:
	SessionMaker();
	~SessionMaker();

	virtual int GetSessionObjSize();

	///����session����,��������ʵ��{::new(session) XXXSession;}
	virtual void MakeSessionObj(void* session);
};

#endif
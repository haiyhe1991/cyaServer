#ifndef __TCP_SESSION_MAKER_H__
#define __TCP_SESSION_MAKER_H__

#include "TcpSession.h"

class DBSessionMaker : public ICyaTcpSessionMaker
{
public:
	DBSessionMaker();
	~DBSessionMaker();

	///获取session大小,总是这样实现{return sizeof(XXXSession);}
	virtual int GetSessionObjSize();

	///构造session对象,总是这样实现{::new(session) XXXSession;}
	virtual void MakeSessionObj(void* session);

};	/// class DBSessionMaker End

#endif
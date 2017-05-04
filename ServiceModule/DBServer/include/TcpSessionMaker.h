#ifndef __TCP_SESSION_MAKER_H__
#define __TCP_SESSION_MAKER_H__

#include "TcpSession.h"

class DBSessionMaker : public ICyaTcpSessionMaker
{
public:
	DBSessionMaker();
	~DBSessionMaker();

	///��ȡsession��С,��������ʵ��{return sizeof(XXXSession);}
	virtual int GetSessionObjSize();

	///����session����,��������ʵ��{::new(session) XXXSession;}
	virtual void MakeSessionObj(void* session);

};	/// class DBSessionMaker End

#endif
#ifndef __ROOM_SESSION_MAKER_H__
#define __ROOM_SESSION_MAKER_H__

#include "RoomSession.h"

class CRoomSessionMaker : public ICyaTcpSessionMaker
{
public:
	CRoomSessionMaker();
	~CRoomSessionMaker();

	virtual int GetSessionObjSize();

	///构造session对象,总是这样实现{::new(session) XXXSession;}
	virtual void MakeSessionObj(void* session);
};

#endif
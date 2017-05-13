#include "RoomSessionMaker.h"

CRoomSessionMaker::CRoomSessionMaker()
{

}

CRoomSessionMaker::~CRoomSessionMaker()
{

}

int CRoomSessionMaker::GetSessionObjSize()
{
	return sizeof(CRoomSession);
}

///构造session对象,总是这样实现{::new(session) XXXSession;}
void CRoomSessionMaker::MakeSessionObj(void* session)
{
	::new(session)CRoomSession;
}
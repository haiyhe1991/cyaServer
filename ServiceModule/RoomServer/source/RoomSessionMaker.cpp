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

///����session����,��������ʵ��{::new(session) XXXSession;}
void CRoomSessionMaker::MakeSessionObj(void* session)
{
	::new(session)CRoomSession;
}
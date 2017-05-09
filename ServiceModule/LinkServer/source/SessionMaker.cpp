#include "SessionMaker.h"


SessionMaker::SessionMaker()
{

}

SessionMaker::~SessionMaker()
{

}

int SessionMaker::GetSessionObjSize()
{
	return sizeof(LinkSession);
}

void SessionMaker::MakeSessionObj(void* session)
{
	::new(session)LinkSession;
}
#include "LscSessionMaker.h"

LSCSessionMaker::LSCSessionMaker()
{

}

LSCSessionMaker::~LSCSessionMaker()
{

}

int LSCSessionMaker::GetSessionObjSize()
{
	return sizeof(LSCSession);
}

void LSCSessionMaker::MakeSessionObj(void* session)
{
	::new(session)LSCSession;
}
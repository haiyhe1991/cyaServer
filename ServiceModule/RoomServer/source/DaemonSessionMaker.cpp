#include "DaemonSessionMaker.h"

CDaemonSessionMaker::CDaemonSessionMaker()
{

}

CDaemonSessionMaker::~CDaemonSessionMaker()
{

}

int CDaemonSessionMaker::GetSessionObjSize()
{
	return sizeof(CDaemonSession);
}

void CDaemonSessionMaker::MakeSessionObj(void* session)
{
	::new(session)CDaemonSession;
}
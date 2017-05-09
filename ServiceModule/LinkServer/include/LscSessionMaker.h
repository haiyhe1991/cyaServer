#ifndef __LSC_SESSION_MAKER_H__
#define __LSC_SESSION_MAKER_H__

#include "LscSession.h"

class LSCSessionMaker : public ICyaTcpSessionMaker
{
public:
	LSCSessionMaker();
	~LSCSessionMaker();

	virtual int GetSessionObjSize();
	virtual void MakeSessionObj(void* session);
};

#endif
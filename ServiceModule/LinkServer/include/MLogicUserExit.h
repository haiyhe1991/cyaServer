#ifndef __MLOGIC_USER_EXIT_H__
#define __MLOGIC_USER_EXIT_H__

#include "IMLogic.h"

class MLogicUserExit : public IMLogicClient
{
public:
	MLogicUserExit();
	~MLogicUserExit();

	virtual int ProcessMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID);
};


#endif
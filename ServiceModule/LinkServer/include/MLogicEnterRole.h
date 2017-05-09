#ifndef __MLOGIC_ENTER_ROLE_H__
#define __MLOGIC_ENTER_ROLE_H__

#include "IMLogic.h"
/// ½øÈë½ÇÉ«

class MLogicEnterRole : public IMLogicClient
{
public:
	MLogicEnterRole();
	~MLogicEnterRole();

	virtual int ProcessMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID);
};

#endif
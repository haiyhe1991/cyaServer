#ifndef __MLOGIC_QUERY_FINISH_HELP_H__
#define __MLOGIC_QUERY_FINISH_HELP_H__

#include "IMLogic.h"
//10009, 查询已完成新手引导信息

class MLogicQueryFinishHelp : public IMLogicClient
{
public:
	MLogicQueryFinishHelp();
	~MLogicQueryFinishHelp();

	virtual int ProcessMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID);
};

#endif
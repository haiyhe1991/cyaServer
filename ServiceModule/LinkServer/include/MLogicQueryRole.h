#ifndef __MLOGIC_QUERY_ROLE_H__
#define __MLOGIC_QUERY_ROLE_H__

#include "IMLogic.h"
//10000, ��ѯ��ɫ��Ϣ

class MLogicQueryRole : public IMLogicClient
{
public:
	MLogicQueryRole();
	~MLogicQueryRole();

	virtual int ProcessMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID);
};


#endif
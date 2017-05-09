/*
主逻辑服务器消息代理基类
每个基类处理不同的消息类型
*/

#ifndef __IM_LOGIC_H__
#define __IM_LOGIC_H__

#include "cyaTypes.h"
#include "GWSSDK.h"

class IMLogicClient
{
public:
	virtual ~IMLogicClient() {}

	virtual int ProcessMsg(GWSHandle gwsHandle, const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID) = 0;
};

#endif
/*
���߼���������Ϣ�������
ÿ�����ദ��ͬ����Ϣ����
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
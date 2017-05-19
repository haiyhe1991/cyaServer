#ifndef __GM_LOGIC_PROC_MAN_H__
#define __GM_LOGIC_PROC_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include "GWSProtocol.h"

class GWSClient;
class GMLogicProcMan
{
	GMLogicProcMan(const GMLogicProcMan&);
	GMLogicProcMan& operator = (const GMLogicProcMan&);

public:
	GMLogicProcMan();
	~GMLogicProcMan();

	int ProcessLogic(GWSClient* gwsSession, UINT16 cmdCode, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

private:
	int OnQueryPlayerActor(GWSClient* gwsSession, UINT16 cmdCode, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

private:
	typedef int (GMLogicProcMan::*fnGMProcHandler)(GWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	std::map<UINT16, fnGMProcHandler> m_procHandler;
};

void InitGMLogicProcMan();
GMLogicProcMan* GetGMLogicProcMan();
void DestroyGMLogicProcMan();

#endif
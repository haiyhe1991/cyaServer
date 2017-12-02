#ifndef __MAU_MSG_MAN_H__
#define __MAU_MSG_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

/*
聊天逻辑处理
*/

#include <map>
#include "GWSProtocol.h"
#include "MauProtocol.h"
#include "MauTableMan.h"

class MauGWSClient;
class MauMsgMan
{
public:
	MauMsgMan();
	~MauMsgMan();

	int ProcessLogic(MauGWSClient* gwsSession, UINT16 cmdCode, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);

private:
	typedef int (MauMsgMan::*fnMsgProcessHandler)(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	std::map<UINT16, fnMsgProcessHandler> m_msgProcHandler;

	//创建牌桌
	int OnCreateMauTable(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//加入牌桌
	int OnJoinMauTable(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//离开牌桌
	int OnLeaveMauTable(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//用户退出
	int OnUserExit(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//玩家准备
	int OnReadyTable(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
private:
	//群发消息
	void SendMauMsgEx(MauGWSClient* gwsSession, BYTE linkerId, UINT16 cmdCode, std::vector<MauTableMan::SMauRecvParam>& vec, const char* pData, int nDatalen);
};

void InitMauMsgMan();
MauMsgMan* GetMauMsgMan();
void DestroyMauMsgMan();

#endif
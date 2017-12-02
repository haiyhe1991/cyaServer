#ifndef __MAU_MSG_MAN_H__
#define __MAU_MSG_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
#pragma once
#endif

/*
�����߼�����
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

	//��������
	int OnCreateMauTable(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//��������
	int OnJoinMauTable(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//�뿪����
	int OnLeaveMauTable(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//�û��˳�
	int OnUserExit(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
	//���׼��
	int OnReadyTable(MauGWSClient* gwsSession, SGWSProtocolHead* pHead, const BYTE* pData, int nDatalen);
private:
	//Ⱥ����Ϣ
	void SendMauMsgEx(MauGWSClient* gwsSession, BYTE linkerId, UINT16 cmdCode, std::vector<MauTableMan::SMauRecvParam>& vec, const char* pData, int nDatalen);
};

void InitMauMsgMan();
MauMsgMan* GetMauMsgMan();
void DestroyMauMsgMan();

#endif
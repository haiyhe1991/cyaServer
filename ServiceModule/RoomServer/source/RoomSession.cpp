#include "RoomSession.h"

CRoomSession::CRoomSession()
{

}

CRoomSession::~CRoomSession()
{

}

void CRoomSession::OnConnected(SOCKET sock, const IPPADDR& peerIPP, int sessionCount, void* param/* = NULL*/)
{

}

void CRoomSession::OnConnectedSend()
{

}

void CRoomSession::OnClosed(int cause)
{

}

int  CRoomSession::RequestReceive(void*& buf, int& bufLen, IoReadType& rt, BOOL& asynNotify)
{
	return 0;
}

int  CRoomSession::OnReceived(void* buf, int bufLen, BOOL callbackByTask, BOOL& continueRecv)
{
	return 0;
}
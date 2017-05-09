#include "MLogicCommonCmd.h"
#include "MsgProcess.h"
#include "MLogicProtocol.h"
#include "cyaTcp.h"

MLogicCommonCmd::MLogicCommonCmd()
{

}

MLogicCommonCmd::~MLogicCommonCmd()
{

}

int MLogicCommonCmd::ProcessMsg(GWSHandle gwsHandle, const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID)
{
	ASSERT(NULL != pMsgData && 0 < nMsgLen);

	char* reqBuf = (char*)CyaTcp_Alloc(sizeof(UINT16)+nMsgLen + sizeof(char));
	ASSERT(reqBuf);
	if (NULL == reqBuf)
	{
		return LINK_MEMERY;
	}

	UINT16* pCmd = (UINT16*)reqBuf;
	*pCmd = htons(nCmdCode);
	if (nMsgLen > 0)
		memcpy(reqBuf + sizeof(UINT16), pMsgData, nMsgLen);

	GWS_SendDataPacket(gwsHandle, reqBuf, nMsgLen + sizeof(UINT16), MAIN_LOGIC_SERVER, nUserID, (ESGSPacketType)nPktType, true);

	return LINK_OK;
}

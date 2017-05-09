#include "MLogicCommonUser.h"
#include "MsgProcess.h"
#include "MLogicProtocol.h"
#include "cyaTcp.h"

MLogicCommonUser::MLogicCommonUser()
{

}

MLogicCommonUser::~MLogicCommonUser()
{

}

int MLogicCommonUser::ProcessMsg(GWSHandle gwsHandle, const void* pMsgData, int nMsgLen, UINT16 nCmdCode, UINT16 nPktType, UINT32 nUserID)
{
	ASSERT(NULL != pMsgData && 0 < nMsgLen);

	UINT16 addLen = sizeof(UINT16)+sizeof(UINT32);

	char* reqBuf = (char*)CyaTcp_Alloc(nMsgLen + addLen + sizeof(char));
	ASSERT(NULL != reqBuf);
	if (NULL == reqBuf)
	{
		return LINK_MEMERY;
	}

	UINT16* cmdReq = (UINT16*)reqBuf;
	*cmdReq = htons(nCmdCode);
	UINT32* uID = (UINT32*)(reqBuf + sizeof(UINT16));
	*uID = htonl(nUserID);
	if (nMsgLen > 0)
		memcpy(reqBuf + addLen, pMsgData, nMsgLen);

	GWS_SendDataPacket(gwsHandle, reqBuf, nMsgLen + addLen, MAIN_LOGIC_SERVER, nUserID, (ESGSPacketType)nPktType, true);
	CyaTcp_Free(reqBuf);

	return LINK_OK;
}

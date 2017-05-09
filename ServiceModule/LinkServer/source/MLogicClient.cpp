#include "MLogicClient.h"

#include "cyaLog.h"

#include "PubFunction.h"
#include "MsgProcess.h"
#include "LinkGlobalObj.h"
#include "LicenceClient.h"
#include "LinkConfig.h"
#include "MLogicFactory.h"
#include "IMLogic.h"

MLogicClient::MLogicClient()
{

}

MLogicClient::~MLogicClient()
{

}

/// MAIN_LOGIC_SERVER
int MLogicClient::ClientMsg(GWSHandle gwsHandle, const void* msgData, int msgLen, UINT16 cmdCode, UINT16 pktType, UINT32 userID, bool encrypt)
{
	IMLogicClient* pLogicClient = ProcessFactory::FetchProcessFactory()->Instance(cmdCode);
	if (NULL != pLogicClient)
	{
		int ret = pLogicClient->ProcessMsg(gwsHandle, msgData, msgLen, cmdCode, pktType, userID);
		ProcessFactory::FetchProcessFactory()->DeleteInstance(pLogicClient);
		return ret;
	}
	LogInfo(("不支持的命令"));

	return LINK_CMD_CODE;
}

void MLogicClient::WGSMsg(SGWSProtocolHead* pHead, UINT16 cmdCode, UINT16 retCode, const void* payload, int payloadLen)
{
	MsgProcess* pMsgP = MsgProcess::FetchMsgProcess();
	ASSERT(NULL != pMsgP);
	if (NULL == pMsgP)
		return;

	ASSERT(NULL != pHead);
	if (LINK_OK != retCode)
	{
		return pMsgP->InputToSessionError(cmdCode, retCode, pHead->pktType, pHead->userId);
	}

	switch (cmdCode)
	{
	case MLS_ENTER_ROLE:
	{
						   SEnterRoleRes* pRes = (SEnterRoleRes*)payload;
						   pRes->ntoh();
						   pMsgP->RegistRole(pRes->userId, pRes->roleId);
						   LogDebug(("%d进入角色%d", pRes->userId, pRes->roleId));
						   pRes->hton();
						   break;
	}
	case MLS_CREATE_ROLE:
		LogDebug(("创建角色"));
		GlobalGetLicenceClient()->ReportRoleCreated(pHead->userId);
		break;
	case MLS_DEL_ROLE:
		LogDebug(("删除角色"));
		GlobalGetLicenceClient()->ReportRoleDeleted(pHead->userId);
		break;

	case MLS_LEAVE_ROLE:
		LogDebug(("%d退出角色", pHead->userId));
		pMsgP->NotifyChatUserExit(pHead->userId);
		return;
	case MLS_USER_EXIT:
		LogDebug(("%d退出账户", pHead->userId));
		return;

	case LICENCE_QUERY_UNLOCK_JOBS:
		LogDebug(("查询解锁职业,错误码[%d]", retCode));
		GlobalGetLicenceClient()->SendCtrlData(LICENCE_LINKER_QUERY_UNLOCK_JOBS, retCode, payload, payloadLen);
		return;

	case MLS_SYS_NOTICE_NOTIFY:
	case MLS_RAND_INST_OPEN_NOTIFY:         //10092, 随机副本开启通知
	case MLS_RAND_INST_CLOSE_NOTIFY:        //10093, 随机副本关闭通知
		LogDebug(("收到需要组播的回复 %d", cmdCode));
		pMsgP->GroupMsg(pHead->pktType, cmdCode, retCode, payload, payloadLen);
		return;

	default:
		break;
	}

	LogDebug(("收到回复 %d", cmdCode));
	pMsgP->InputToSessionMsg(pHead->userId, pHead->pktType, cmdCode, retCode, payload, payloadLen);

#pragma compiling_remind("新的对主逻辑消息类型请求, 有消息回复在这添加代码查看结果")
}
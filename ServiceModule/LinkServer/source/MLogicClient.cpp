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
	LogInfo(("��֧�ֵ�����"));

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
						   LogDebug(("%d�����ɫ%d", pRes->userId, pRes->roleId));
						   pRes->hton();
						   break;
	}
	case MLS_CREATE_ROLE:
		LogDebug(("������ɫ"));
		GlobalGetLicenceClient()->ReportRoleCreated(pHead->userId);
		break;
	case MLS_DEL_ROLE:
		LogDebug(("ɾ����ɫ"));
		GlobalGetLicenceClient()->ReportRoleDeleted(pHead->userId);
		break;

	case MLS_LEAVE_ROLE:
		LogDebug(("%d�˳���ɫ", pHead->userId));
		pMsgP->NotifyChatUserExit(pHead->userId);
		return;
	case MLS_USER_EXIT:
		LogDebug(("%d�˳��˻�", pHead->userId));
		return;

	case LICENCE_QUERY_UNLOCK_JOBS:
		LogDebug(("��ѯ����ְҵ,������[%d]", retCode));
		GlobalGetLicenceClient()->SendCtrlData(LICENCE_LINKER_QUERY_UNLOCK_JOBS, retCode, payload, payloadLen);
		return;

	case MLS_SYS_NOTICE_NOTIFY:
	case MLS_RAND_INST_OPEN_NOTIFY:         //10092, �����������֪ͨ
	case MLS_RAND_INST_CLOSE_NOTIFY:        //10093, ��������ر�֪ͨ
		LogDebug(("�յ���Ҫ�鲥�Ļظ� %d", cmdCode));
		pMsgP->GroupMsg(pHead->pktType, cmdCode, retCode, payload, payloadLen);
		return;

	default:
		break;
	}

	LogDebug(("�յ��ظ� %d", cmdCode));
	pMsgP->InputToSessionMsg(pHead->userId, pHead->pktType, cmdCode, retCode, payload, payloadLen);

#pragma compiling_remind("�µĶ����߼���Ϣ��������, ����Ϣ�ظ�������Ӵ���鿴���")
}
#include "PubFunction.h"
#include "AES.h"
#include "GWSProtocol.h"
#include "ServiceErrorCode.h"
#include "LCSProtocol.h"
#include "ServiceLicence.h"

#include "cyaTcp.h"

void CreateProtocolPacket(void* pktBuf, bool encrypt, UINT16 pktType, UINT16 payloadLen)
{
	ASSERT(NULL != pktBuf);
	SGSProtocolHead* pProc = (SGSProtocolHead*)pktBuf;
	pProc->isCrypt = encrypt;
	pProc->pktType = pktType;
	pProc->pduLen = payloadLen;
	pProc->hton();
}

void CreateUnEncryptResPayLoad(BYTE* token, void* pktBuf, const void* pktData, UINT16 pktLen, UINT16 cmdCode, UINT16 retCode)
{
	ASSERT(NULL != pktBuf);
	SGSResPayload* pPayLoad = (SGSResPayload*)pktBuf;
	pPayLoad->rawDataBytes = pktLen;
	pPayLoad->cmdCode = cmdCode;
	pPayLoad->retCode = retCode;
	memcpy(&(pPayLoad->token), token, DEFAULT_TOKEN_SIZE);
	if (NULL != pktData && 0 != pktLen)
		memcpy(pPayLoad->data, pktData, pktLen);
	pPayLoad->hton();
}

void CreateEncryptResPayLoad(BYTE* token, void* pktBuf, const void* pktData, UINT16 pktLen, UINT16 cmdCode, UINT16 retCode, const BYTE* enKey)
{
	UINT16 payloadLen = 0;
	if (NULL == pktData || 0 == pktLen)
		payloadLen = SGS_RES_HEAD_LEN;
	else
		payloadLen = pktLen + SGS_RES_HEAD_LEN;
	CreateUnEncryptResPayLoad(token, pktBuf, pktData, payloadLen, cmdCode, retCode);
	appEncryptDataWithKey((AES_BYTE*)pktBuf, payloadLen, (char*)enKey);
}

void CreateUnEncryptReqPayLoad(UINT32 token, void* pktBuf, const void* pktData, UINT16 pktLen, UINT16 cmdCode)
{
	ASSERT(NULL != pktBuf);
	SGSReqPayload* pPayload = (SGSReqPayload*)pktBuf;
	{
		pPayload->cmdCode = cmdCode;
		pPayload->rawDataBytes = pktLen;
		pPayload->token = token;
	}
	if (NULL != pktData && 0 != pktLen)
		memcpy(pPayload->data, pktData, pktLen);
	pPayload->hton();
}

void CreateEncryptReqPayLoad(UINT32 token, void* pktBuf, const void* pktData, UINT16 pktLen, UINT16 cmdCode, const BYTE* enKey)
{
	UINT16 paloadLen = 0;
	if (NULL == pktData || 0 == pktLen)
		paloadLen = SGS_REQ_HEAD_LEN;
	else
		paloadLen = pktLen + SGS_REQ_HEAD_LEN;
	CreateUnEncryptReqPayLoad(token, pktBuf, pktData, paloadLen, cmdCode);
	if (NULL != enKey)
		appEncryptDataWithKey((AES_BYTE*)pktBuf, paloadLen, (char*)enKey);
}

UINT16 GetAlignedLen(bool encrypt, UINT16 baseLen, UINT16 baseAligned/* = 16*/)
{
	UINT16 bytePayloadLen = 0;
	if (encrypt)
		bytePayloadLen = (baseLen%baseAligned == 0) ? baseLen : ((baseLen / baseAligned + 1)*baseAligned);
	else
		bytePayloadLen = baseLen;
	return bytePayloadLen;
}

bool EqualToken(UINT32 dwToken, const BYTE* srcToken, UINT16 srcLen)
{
	ASSERT(NULL != srcToken);
	if (srcLen != sizeof(UINT32))
		return false;
	UINT32 dwSrcToken = *(UINT32*)srcToken;
	return dwToken == dwSrcToken;
}

void CreateLinkLoginLsReq(void* reqBuf, UINT16 partitionId, UINT32 lsIp, UINT16 lsPort, BYTE linkID, UINT32 linkIp, UINT16 linkPort, const char* token)
{
	ASSERT(NULL != reqBuf);
	SLinkerLoginReq* loginReq = (SLinkerLoginReq*)(reqBuf);
	loginReq->partitionId = partitionId;
	loginReq->linkerId = linkID;
	loginReq->linkerIp = linkIp;
	loginReq->linkerPort = linkPort;
	loginReq->subareIp = lsIp;
	loginReq->subarePort = lsPort;
	strcpy(loginReq->linkerToken, token);
	loginReq->hton();
}

void CreateLinkSessinsReq(void* reqBuf, UINT32 sessions, UINT32 tokens)
{
	ASSERT(NULL != reqBuf);
	SLinkerReportConnections* ReqPayload = (SLinkerReportConnections*)reqBuf;
	ReqPayload->connections = sessions;
	ReqPayload->tokens = tokens;
	ReqPayload->hton();
}

void CreateRoleChanged(void* reqBuf, UINT32 userId,/* UINT32 roleId,*/ BYTE model)
{
	ASSERT(NULL != reqBuf);
	SRoleOperReq* ReqPayload = (SRoleOperReq*)reqBuf;
	ReqPayload->userId = userId;
	//	ReqPayload->roleId = roleId;
	ReqPayload->model = model;
	ReqPayload->hton();
}

void CreateTokenRes(void* reqBuf, UINT32 userId, const char* userToken)
{
	ASSERT(NULL != reqBuf);
	SLinkerGetUserTokenRes* ResPayload = (SLinkerGetUserTokenRes*)reqBuf;
	ResPayload->userId = userId;
	strcpy(ResPayload->token, userToken);
	ResPayload->hton();
}
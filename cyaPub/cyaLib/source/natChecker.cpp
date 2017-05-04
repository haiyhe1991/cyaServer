#include "natChecker.h"

#define CHANGE_PORT 2
#define CHANGE_IP	4

NatChecker::NatChecker(const char* ipORdomain /*= "stunserver.org"*/, int port /*= 3478*/,
						DWORD localIp /*= INADDR_ANY*/, DWORD timeout /*= 5*1000*/)
						: m_isValid(false)
						, m_timeout(timeout)
						, m_localIp(localIp)
						, m_stunServerPort(port)
{
	if (ipORdomain == NULL || port <= 0)
		return;

	m_stunServerPort = port;
	m_timeout = timeout;
	if (IsValidStringIP(ipORdomain))
		m_stunServerIp = ipORdomain;
	else
	{
		DWORD dwIp = SockResolveIP(ipORdomain);
		if (INADDR_NONE == dwIp)
			return;
		char szIp[16] = { 0 };
		m_stunServerIp = GetStringIP(szIp, dwIp);
	}

	// 	if(m_localIp == INADDR_ANY)
	// 		m_localIp = IfcfgGetHostIp();
	m_isValid = true;
}

NatChecker::~NatChecker()
{
	m_isValid = false;
}

NatChecker::NatType NatChecker::GetNatType()
{
#if defined(_DEBUG)
	char szIp[16] = { 0 };
	DWORD dwSourceAddr = 0;
#endif
	int r = 0;
	int resLen = 0;
	int localPort = 0;
	SOCKET sock = INVALID_SOCKET;
	stun_message msg1, msg3;
	BYTE res1[1500], res2[1500], res3[1500], res4[1500];
	WORD attrLen = 0;
	WORD addrSize = 0;	//4->BYTE familyReserve;BYTE family;WORD mapped_port;
	DWORD dwMappedAddr1 = 0;
	DWORD dwChangeAddr = 0;
	DWORD dwMappedAddr3 = 0;
	NatChecker::NatType nt = ERROR_DETECTING_NAT;

	if (!m_isValid)
	{
		nt = ERROR_DETECTING_NAT;
		goto __end__;
	}

	sock = OnCreateUDPSocket(&localPort);
	if (!SockValid(sock))
	{
		nt = ERROR_DETECTING_NAT;
		goto __end__;
	}

	//发送BINDING_REQUEST
	resLen = OnSendStunBindRequest(sock, GetDWordIP(m_stunServerIp.c_str()), m_stunServerPort, res1, 1500);
	if (resLen < 0)
	{
		nt = ERROR_DETECTING_NAT;
		goto __end__;
	}
	if (resLen == 0)
	{
		nt = FIREWALL_BLOCKS_UDP;
		goto __end__;
	}

	r = OnParseStunMsg(res1, resLen, &msg1);
	if (r != 0 || !msg1.stun_header || BINDING_RESPONSE != ntohs(msg1.stun_header->msgType))
	{
		nt = ERROR_DETECTING_NAT;
		goto __end__;
	}

	if (!msg1.mapped_addr)
	{
		nt = ERROR_DETECTING_NAT;
		goto __end__;
	}

	attrLen = htons(msg1.mapped_addr->attrLen);
	addrSize = attrLen - 4;	//4->BYTE familyReserve;BYTE family;WORD mapped_port;
	ASSERT(sizeof(DWORD) >= addrSize);
	memcpy(&dwMappedAddr1, msg1.mapped_addr->mapped_addr, addrSize);
	if (m_localIp/*GetLocalIP()*/ == dwMappedAddr1 &&
		/*ntohs(msg1.mapped_addr->mapped_port) == localPort &&*/	//不比较port,不同主机若用相同port发送数据则nat映射的端口和主机发送port可能不一样
		AF_INET == msg1.mapped_addr->family)
	{
#if defined(_DEBUG)
		GetStringIP(szIp, dwMappedAddr1);
		printf("mapped_ip:%s, mapped_port:%d\n", szIp, ntohs(msg1.mapped_addr->mapped_port));
#endif
		nt = OPEN_INTERNET;		//没有穿越NAT,在互联网中
		goto __end__;
	}

#if defined(_DEBUG)
	GetStringIP(szIp, dwMappedAddr1);
	printf("mapped_ip:%s, mapped_port:%d\n", szIp, ntohs(msg1.mapped_addr->mapped_port));
#endif

	if (!msg1.source_addr || !msg1.changed_addr)
	{
		nt = BEHIND_NAT;
		goto __end__;
	}

#if defined(_DEBUG)
	memset(szIp, 0, sizeof(szIp));
	attrLen = htons(msg1.source_addr->attrLen);
	addrSize = attrLen - 4;	//4->BYTE familyReserve;BYTE family;WORD mapped_port;
	ASSERT(sizeof(DWORD) >= addrSize);
	memcpy(&dwSourceAddr, msg1.source_addr->source_addr, addrSize);
	GetStringIP(szIp, dwSourceAddr);
	printf("source_ip:%s, source_port:%d\n", szIp, ntohs(msg1.source_addr->source_port));
#endif

	//请求stun服务器用不同于前一次的ip/port发送响应
	r = OnSendStunBindChangeRequest(sock, GetDWordIP(m_stunServerIp.c_str()), m_stunServerPort, CHANGE_IP | CHANGE_PORT, res2, 1500);
	if (r < 0)
	{
		nt = ERROR_DETECTING_NAT;
		goto __end__;
	}
	if (r > 0)
	{
		nt = FULL_CONE_NAT;	//还能收到响应则是完全锥型NAT
		goto __end__;
	}

	//向stun服务器另一个ip/port发送BINDING_REQUEST
	attrLen = htons(msg1.source_addr->attrLen);
	addrSize = attrLen - 4;	//4->BYTE familyReserve;BYTE family;WORD mapped_port;
	ASSERT(sizeof(DWORD) >= addrSize);
	memcpy(&dwChangeAddr, msg1.changed_addr->changed_addr, addrSize);
#if defined(_DEBUG)
	memset(szIp, 0, sizeof(szIp));
	GetStringIP(szIp, dwChangeAddr);
	printf("changed_ip:%s, changed_port:%d\n", szIp, ntohs(msg1.changed_addr->changed_port));
#endif
	resLen = OnSendStunBindRequest(sock, dwChangeAddr, ntohs(msg1.changed_addr->changed_port), res3, 1500);
	if (resLen <= 0)
	{
		nt = ERROR_DETECTING_NAT;
		goto __end__;
	}
	r = OnParseStunMsg(res3, resLen, &msg3);
	if (r != 0 || !msg3.mapped_addr || !msg3.stun_header || BINDING_RESPONSE != ntohs(msg3.stun_header->msgType))
	{
		nt = ERROR_DETECTING_NAT;
		goto __end__;
	}

	attrLen = htons(msg3.source_addr->attrLen);
	addrSize = attrLen - 4;	//4->BYTE familyReserve;BYTE family;WORD mapped_port;
	ASSERT(sizeof(DWORD) >= addrSize);
	memcpy(&dwMappedAddr3, msg3.mapped_addr->mapped_addr, addrSize);
	if (msg1.mapped_addr->family != msg3.mapped_addr->family ||
		msg1.mapped_addr->mapped_port != msg3.mapped_addr->mapped_port ||
		dwMappedAddr1 != dwMappedAddr3)
	{
		nt = SYMMETRIC_NAT;	//这一次的mapped地址和第一次的mapped地址不一样则是对称NAT(到这里若使用打洞的手段来p2p可能已经无效)
		goto __end__;
	}

	//请求stun服务器用不同于m_stunServerPort的端口来做响应
	r = OnSendStunBindChangeRequest(sock, GetDWordIP(m_stunServerIp.c_str()), m_stunServerPort, CHANGE_PORT, res4, 1500);
	if (r > 0)
		nt = RESTRICTED_CONE_NAT;
	else if (r == 0)
		nt = RESTRICTED_PORT_CONE_NAT;
	else
		nt = ERROR_DETECTING_NAT;

__end__:
	if (SockValid(sock))
		SockClose(sock);
	return nt;
}

int NatChecker::GetStunMappedAddress(SOCKET udpSock, DWORD* mappedIp, int* mappedPort, int* family)
{
	if (!SockValid(udpSock))
		return -1;

	BYTE res[1500] = { 0 };
	int resLen = OnSendStunBindRequest(udpSock, GetDWordIP(m_stunServerIp.c_str()), m_stunServerPort, res, 1500);
	if (resLen <= 0)
		return -1;

	stun_message msg;
	int r = OnParseStunMsg(res, resLen, &msg);
	if (r != 0 || !msg.mapped_addr)
		return -1;

	if (mappedIp)
		*mappedIp = *(DWORD*)msg.mapped_addr->mapped_addr;
	if (mappedPort)
		*mappedPort = ntohs(msg.mapped_addr->mapped_port);
	if (family)
		*family = msg.mapped_addr->family;

	return 0;
}

const char* NatChecker::GetStringNatType(NatType nt)
{
	const char* szType = "unknown";
	switch (nt)
	{
	case ERROR_DETECTING_NAT:
		szType = "ERROR_DETECTING_NAT";
		break;
	case OPEN_INTERNET:
		szType = "OPEN_INTERNET";
		break;
	case FIREWALL_BLOCKS_UDP:
		szType = "FIREWALL_BLOCKS_UDP";
		break;
	case FULL_CONE_NAT:
		szType = "FULL_CONE_NAT";
		break;
	case SYMMETRIC_NAT:
		szType = "SYMMETRIC_NAT";
		break;
	case RESTRICTED_CONE_NAT:
		szType = "RESTRICTED_CONE_NAT";
		break;
	case RESTRICTED_PORT_CONE_NAT:
		szType = "RESTRICTED_PORT_CONE_NAT";
		break;
	case BEHIND_NAT:
		szType = "BEHIND_NAT";
		break;
	default:
		szType = "unknown";
		break;
	}
	return szType;
}

SOCKET NatChecker::OnCreateUDPSocket(int* localPort)
{
	SOCKET sock = UDPNew();
	if (!SockValid(sock))
		return INVALID_SOCKET;;
	int r = UDPBind(sock, m_localIp/*INADDR_ANY*/, 0);
	if (r != 0)
		return INVALID_SOCKET;

	SockGetLocal(sock, localPort);
	SockSetNonBlock(sock, true);
	SockSetReceiveBufSize(sock, 64 * 1024);
	SockSetSendBufSize(sock, 64 * 1024);

	return sock;
}
int NatChecker::OnSendStunBindRequest(SOCKET sock, DWORD serverIp, int serverPort, BYTE* res, int nResBytes)
{
	BYTE stunHeader[20];
	OnCreateStunMsgHeader(stunHeader, BINDING_REQUEST, 0);

	int ret = SockSendTo(sock, serverIp, serverPort, stunHeader, 20);
	if (ret != 20)
		return -1;

	ret = SockCheckRead(sock, m_timeout);
	if (ret <= 0)
		return 0;

	ret = SockReceiveFrom(sock, NULL, NULL, res, nResBytes);
	return ret;
}

int NatChecker::OnSendStunBindChangeRequest(SOCKET sock, DWORD serverIp, int serverPort, UINT32 changeFlag, BYTE* res, int nResBytes)
{
	BYTE stunMsg[28];
	OnCreateStunMsgHeader(stunMsg, BINDING_REQUEST, 8);
	UINT32 flag = htonl(changeFlag);
	WORD attrType = htons(CHANGE_REQUEST);
	WORD attrLen = htons((WORD)sizeof(DWORD));
	memcpy(stunMsg + 20, &attrType, 2);
	memcpy(stunMsg + 22, &attrLen, 2);
	memcpy(stunMsg + 24, &flag, 4);
	int ret = SockSendTo(sock, serverIp, serverPort, stunMsg, 28);
	if (ret != 28)
		return -1;
	ret = SockCheckRead(sock, m_timeout);
	if (ret <= 0)
		return 0;
	ret = SockReceiveFrom(sock, NULL, NULL, res, nResBytes);
	ASSERT(ret > 0);
	return ret;
}

void NatChecker::OnGenerateTransactionId(BYTE id[16])
{
	static int count = 0;
	++count;

	srand((unsigned int)time(NULL));
	for (int i = 0; i < 16; ++i)
		id[i] = (unsigned char)rand();
	id[0] = (unsigned char)count;
}

void NatChecker::OnCreateStunMsgHeader(BYTE p[20], WORD msgType, WORD msgLen)
{
	WORD m = htons(msgType);
	WORD n = htons(msgLen);
	memcpy(p, &m, 2);
	memcpy(p + 2, &n, 2);
	OnGenerateTransactionId(p + 4);
}

int NatChecker::OnParseStunMsg(const BYTE* data, int /*dataLen*/, stun_message* msg)
{
	int ret = 0;
	WORD nReadSize = 0;
	WORD msgLen = ntohs(*(WORD*)(data + 2));
	BYTE* payload = (BYTE*)data + 20;
	memset(msg, 0, sizeof(stun_message));
	msg->stun_header = (stun_message_header*)data;

	while (nReadSize < msgLen)
	{
		WORD attrType = ntohs(*(WORD*)payload);
		WORD attrLen = ntohs(*(WORD*)(payload + 2));
		switch (attrType)
		{
		case MAPPED_ADDRESS:
			msg->mapped_addr = (stun_mapped_address*)payload;
			break;
		case RESPONSE_ADDRESS:
			break;
		case CHANGE_REQUEST:
			break;
		case SOURCE_ADDRESS:
			msg->source_addr = (stun_source_address*)payload;
			break;
		case CHANGED_ADDRESS:
			msg->changed_addr = (stun_changed_address*)payload;
			break;
		case USERNAME:
			break;
		case PASSWORD:
			break;
		case MESSAGE_INTEGRITY:
			break;
		case ERROR_CODE:
			break;
		case UNKNOWN_ATTRIBUTES:
			break;
		case REFLECTED_FROM:
			break;
		default:
			break;
		}

		payload = payload + (attrLen + 4);	//4->(WORD attrType;WORD attrLen;)
		nReadSize = (WORD)(nReadSize + attrLen + 4);
	}

	return ret;
}
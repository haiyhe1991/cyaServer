#include "stunServe.h"
#include "cyaIpCvt.h"
#include "cyaSock.h"

#define CHANGE_PORT 2
#define CHANGE_IP	4

StunServe::StunServe()
	: m_thHandle(NULL)
	, m_exitThread(false)
	, m_stunIp(INADDR_ANY)
	, m_stunPort(0)
	, m_stunIp2(INADDR_ANY)
	, m_stunPort2(0)
{

}

StunServe::~StunServe()
{

}

BOOL StunServe::Start(const char* stunIp, int stunPort, const char* stunIp2, int stunPort2)
{
	if (stunIp == NULL || strlen(stunIp) <= 0)
		return false;

	m_stunIp = GetDWordIP(stunIp);
	m_stunPort = stunPort;
	if (stunIp2 != NULL && strlen(stunIp2) > 0)
		m_stunIp2 = GetDWordIP(stunIp2);
	m_stunPort2 = stunPort2;

	m_exitThread = false;
	OSCreateThread(&m_thHandle, NULL, THWorker, this, 0);
	return true;
}

void StunServe::Stop()
{
	m_exitThread = true;
	if (m_thHandle != NULL)
	{
		OSCloseThread(m_thHandle);
		m_thHandle = NULL;
	}
}

int StunServe::THWorker(void* param)
{
	StunServe* pThis = (StunServe*)param;
	return pThis->OnWorker();
}

int StunServe::OnWorker()
{
	int maxfd = -1;
	SOCKET sock1 = INVALID_SOCKET;
	SOCKET sock2 = INVALID_SOCKET;

	sock1 = UDPNew();
	if (!SockValid(sock1))
		return 0;
	int r = UDPBind(sock1, m_stunIp, m_stunPort);
	if (r != 0)
	{
		SockClose(sock1);
		return 0;
	}
	SockSetNonBlock(sock1, true);
	SockSetReceiveBufSize(sock1, 256 * 1024);
	SockSetSendBufSize(sock1, 256 * 1024);
	maxfd = sock1;
	int fdCount = 1;

	if (m_stunIp2 != INADDR_ANY)
	{
		sock2 = UDPNew();
		if (!SockValid(sock2))
		{
			SockClose(sock1);
			return 0;
		}
		int r = UDPBind(sock2, m_stunIp2, m_stunPort2);
		if (r != 0)
		{
			SockClose(sock1);
			SockClose(sock2);
			return 0;
		}

		SockSetNonBlock(sock2, true);
		SockSetReceiveBufSize(sock2, 256 * 1024);
		SockSetSendBufSize(sock2, 256 * 1024);
		if (sock2 > maxfd)
			maxfd = sock2;
		++fdCount;
	}

#if defined(__linux__)
	struct pollfd pfds[2];
	pfds[0].fd = sock1;
	pfds[0].events = 0;
	pfds[0].events |= POLLIN;
	pfds[0].revents = 0;
	if (fdCount == 2)
	{
		pfds[1].fd = sock2;
		pfds[1].events = 0;
		pfds[1].events |= POLLIN;
		pfds[1].revents = 0;
	}
#endif

	while (!m_exitThread)
	{
#if defined(WIN32)
		struct fd_set rfds;
		struct timeval tmv;
		FD_ZERO(&rfds);
		FD_SET(sock1, &rfds);
		if (SockValid(sock2))
			FD_SET(sock2, &rfds);
		tmv.tv_sec = 0;
		tmv.tv_usec = 50 * 1000;
		int n = select(maxfd, &rfds, NULL, NULL, &tmv);
		if (n < 0)
			return 0;
		else if (n == 0)
			continue;
		else
		{
			if (FD_ISSET(sock1, &rfds))
				OnRead__(sock1);
			if (SockValid(sock2) && FD_ISSET(sock2, &rfds))
				OnRead__(sock2);
		}
#else
		int timeout = 50;
		int n = poll(pfds, fdCount, timeout);
		while (n<0 && errno == EINTR)
		{
			if (timeout == 0)
				break;
			else if (0<timeout && timeout <= 10)
			{
				Sleep(timeout);
				timeout = 0;
			}
			else if (timeout > 10)
			{
				Sleep(10);
				timeout -= 10;
			}
			else
			{
				ASSERT(-1 == timeout);
			}
			n = poll(pfds, fdCount, timeout);
		}
		if (n < 0)
			return 0;
		else if (n == 0)
			continue;
		for (int i = 0; i < fdCount; ++i)
		{
			short revents = (pfds[i].events & pfds[i].revents);
			if (0 != revents)
			{
				if (revents & POLLIN)
					OnRead__(pfds[i].fd);
				pfds[i].revents = 0;
			}
		}
#endif
	}

	if (SockValid(sock1))
		SockClose(sock1);

	if (SockValid(sock2))
		SockClose(sock2);
	return 0;
}

void StunServe::OnRead__(SOCKET sock)
{
	DWORD peerIp;
	int peerPort;
	char res[1500] = { 0 };
	int len = SockReceiveFrom(sock, &peerIp, &peerPort, res, sizeof(res));
	if (len <= 0)
		return;

	stun_message msg;
	int r = OnParseStunMessage(res, len, &msg);
	if (r != 0)
		return;

	OnProcessStunMessage(msg, sock, peerIp, peerPort);
}

void StunServe::OnProcessStunMessage(const stun_message& msg, SOCKET sock, DWORD peerIp, int peerPort)
{
	BYTE buf[1500] = { 0 };
	DWORD changedIp = m_stunIp2;
	int changedPort = m_stunPort2;

	int sourcePort;
	DWORD sourceIp = SockGetLocal(sock, &sourcePort);

	int offset = 20;
	int n = OnAddAddressAttr(buf + offset, htons(MAPPED_ADDRESS), htons(8), htons(AF_INET), htons(peerPort), peerIp);
	offset += n;

	SOCKET sendSock = sock;
	WORD stunMsgType = ntohs(msg.stun_header->msgType);
	if (stunMsgType == BINDING_REQUEST)
	{
		if (!msg.change_request)
		{
			n = OnAddAddressAttr(buf + offset, htons(SOURCE_ADDRESS), htons(8), htons(AF_INET), htons(sourcePort), sourceIp);
			offset += n;
		}
		else
		{
			//用不同的ip/port或者port发送响应
			ASSERT(ntohs(msg.change_request->attrType) == CHANGE_REQUEST);
			WORD attrLen = ntohs(msg.change_request->attrLen);
			ASSERT(sizeof(DWORD) >= attrLen);
			DWORD flag;
			memcpy(&flag, msg.change_request->change_attr, attrLen);
			flag = ntohl(flag);
			if (flag & (CHANGE_PORT | CHANGE_IP))
			{
				sendSock = UDPNew();
				UDPBind(sendSock, changedIp, 0);
				SockGetLocal(sendSock, &sourcePort);	//用changedIp和sourcePort发送数据

				n = OnAddAddressAttr(buf + offset, htons(SOURCE_ADDRESS), htons(8), htons(AF_INET), htons(sourcePort), changedIp);
				offset += n;
			}
			else if (flag & CHANGE_PORT)
			{
				sendSock = UDPNew();
				UDPBind(sendSock, sourceIp, 0);
				SockGetLocal(sendSock, &sourcePort); //用sourceIp和sourcePort发送数据

				n = OnAddAddressAttr(buf + offset, htons(SOURCE_ADDRESS), htons(8), htons(AF_INET), htons(sourcePort), sourceIp);
				offset += n;
			}
			else
			{
				ASSERT(false);
				return;
			}
		}
	}

	if (changedIp != INADDR_ANY && changedPort != 0)
	{
		n = OnAddAddressAttr(buf + offset, htons(CHANGED_ADDRESS), htons(8), htons(AF_INET), htons(changedPort), changedIp);
		offset += n;
	}
	OnCreateStunMsgHeader(buf, BINDING_RESPONSE, offset - 20, msg.stun_header->transaction);

	SockSendTo(sendSock, peerIp, peerPort, buf, offset);
	if (sendSock != sock)
		SockClose(sendSock);
}

int StunServe::OnParseStunMessage(const char* data, int dataLen, stun_message* msg)
{
	if (NULL == data || dataLen <= 0 || msg == NULL)
		return -1;

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
			msg->change_request = (stun_change_request*)payload;
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
			ret = -1;
			break;
		}

		payload = payload + (attrLen + 4);
		nReadSize = (WORD)(nReadSize + attrLen + 4);
	}

	return ret;
}

void StunServe::OnCreateStunMsgHeader(BYTE p[20], WORD msgType, WORD msgLen, BYTE transaction[16])
{
	WORD m = htons(msgType);
	WORD n = htons(msgLen);
	memcpy(p, &m, 2);
	memcpy(p + 2, &n, 2);
	memcpy(p + 4, transaction, 16);
}

int StunServe::OnAddAddressAttr(BYTE* buf, WORD attrType, WORD attrLen, WORD family, WORD port, DWORD ip)
{
	int offset = 0;
	memcpy(buf + offset, &attrType, 2);
	offset += 2;
	memcpy(buf + offset, &attrLen, 2);
	offset += 2;
	memcpy(buf + offset, &family, 2);
	offset += 2;
	memcpy(buf + offset, &port, 2);
	offset += 2;
	memcpy(buf + offset, &ip, 4);
	offset += 4;
	return offset;
}
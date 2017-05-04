#ifndef __CYA_SOCK_H__
#define __CYA_SOCK_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaString.h"
#include "cyaSync.h"
#include "cyaIfcfg.h"

#if defined(WIN32)
	#include <io.h>
	// 	#ifndef _WINSOCK2API_
	// 		#pragma warning(disable: 4115) /* 'xxx' : named type definition in parentheses) */
	// 		#include <winsock2.h>
	// 	#endif
	#pragma comment(lib, "ws2_32.lib")
	#ifndef _WS2TCPIP_H_
		#include <ws2tcpip.h>
	#endif
	#ifndef SIO_UDP_CONNRESET
		#define SIO_UDP_CONNRESET     _WSAIOW(IOC_VENDOR,12)
	#endif
	#ifdef VS_IS_VC6
	struct tcp_keepalive {
		u_long  onoff;
		u_long  keepalivetime;
		u_long  keepaliveinterval;
	};
	#define SIO_RCVALL            _WSAIOW(IOC_VENDOR,1)
	#define SIO_RCVALL_MCAST      _WSAIOW(IOC_VENDOR,2)
	#define SIO_RCVALL_IGMPMCAST  _WSAIOW(IOC_VENDOR,3)
	#define SIO_KEEPALIVE_VALS    _WSAIOW(IOC_VENDOR,4)
	#define SIO_ABSORB_RTRALERT   _WSAIOW(IOC_VENDOR,5)
	#define SIO_UCAST_IF          _WSAIOW(IOC_VENDOR,6)
	#define SIO_LIMIT_BROADCASTS  _WSAIOW(IOC_VENDOR,7)
	#define SIO_INDEX_BIND        _WSAIOW(IOC_VENDOR,8)
	#define SIO_INDEX_MCASTIF     _WSAIOW(IOC_VENDOR,9)
	#define SIO_INDEX_ADD_MCAST   _WSAIOW(IOC_VENDOR,10)
	#define SIO_INDEX_DEL_MCAST   _WSAIOW(IOC_VENDOR,11)
	#else
		#include <MSTcpIP.h>
	#endif

	typedef char* SOCKPARM;
	typedef int socklen_t;

#elif defined(__linux__)
	#include <signal.h>
	#include <unistd.h>
	#include <fcntl.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <netinet/ip.h>
	#include <netinet/ip_icmp.h>
	#include <netinet/tcp.h>
	#include <netdb.h>
	#include <sys/ioctl.h>
	#include <sys/poll.h>
	#include <sys/uio.h>
	#include <sys/un.h>
	#include <sys/stat.h>

	#define ioctlsocket ioctl
	typedef char* SOCKPARM;
	#define O_BINARY			0
	#define WSAENOBUFS			ENOBUFS
	#define WSAEWOULDBLOCK		EWOULDBLOCK

#elif defined(VXWORKS)
	#include <sockLib.h>
	#include <arpa/inet.h>
	#include <netinet/in.h>
	#include <ifLib.h>
	#define ioctlsocket ioctl
	typedef char* SOCKPARM;

#elif defined(__ECOS)
	#include <network.h>
	#define ioctlsocket ioctl
	typedef char* SOCKPARM;
#endif

#include "cyaSync.h"

struct SockBufVec
{
	void* buf;
	int   bufBytes;
};

#if defined(_MSC_VER)
	#pragma warning(disable : 4201) // nonstandard extension used : nameless struct/union
#endif

/* 离散buf socket读/写函数，函数内的临时堆栈结构变量长度 */
#define SOCK_BUF_STACK_COUNT	128

/* void FDClose(int fd); */
#define FDClose(fd) close(fd)

//初始化套接字库
BOOL SockInit();

//获取IP
DWORD SockResolveIP(const char* host);

/* 判断ip是否为组播IP */
BOOL IsMulticastIP(DWORD ip);

/* SOCKET SockRawNew(int protocol); */
#define SockRawNew(protocol)	\
	socket(AF_INET, SOCK_RAW, (WORD)(protocol))

/* SOCKET UDPNew(); */
#define UDPNew()	\
	socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)

/* is_AF_LOCAL: 是否使用本机进程通信优化方式 */
SOCKET TCPNew(BOOL is_AF_LOCAL DEFAULT_PARAM(false));

int SockClose(SOCKET sock);

/* int SockIoctl(SOCKET s, long cmd, u_long* argp); */
#define SockIoctl ioctlsocket

/* 得到sock的错误信息 */
/* !!!此函数WIN32平台有阻塞操作，耗时较长（一般>10ms），请谨慎使用!!! */
int SockGetError(SOCKET sock);

//得到sock类型
int SockGetType(SOCKET sock);

/* 返回值同recv(): <0 - error; =0 - peer gracefully closed; >0 - received bytes */
int SockRead(SOCKET sock, void* buf, int len);
/* 返回值同send() */
int SockWrite(SOCKET sock, const void* data, int len);

/* 返回值同sendto() */
int SockSendTo(SOCKET sock, DWORD ip, int port, const void* data, int len);
/* 返回值同recvfrom() */
int SockReceiveFrom(SOCKET sock, DWORD* pip /*= NULL*/, int* pport /*= NULL*/, void* buf, int len);

/* events: 1 - read; 2 - urgent; 4 - write; or bitmask of 1/2/4;
* 返回值: <0 - error; 0 - timeout; 1 - read; 2 - urgent; 4 - write;
* 或者1/2/4的位组合值 */
int SockPoll(SOCKET sock, int events, DWORD timeout, SOCKET forceBreakSock DEFAULT_PARAM(INVALID_SOCKET));

/* 返回值: <0 - error; 0 - timeout; 1 - read */
int SockCheckRead(SOCKET sock, DWORD timeout, SOCKET forceBreakSock DEFAULT_PARAM(INVALID_SOCKET));
/* 返回值: <0 - error; 0 - timeout; 4 - write */
int SockCheckWrite(SOCKET sock, DWORD timeout, SOCKET forceBreakSock DEFAULT_PARAM(INVALID_SOCKET));
/* 返回值: <0 - error; 0 - timeout; 1 - read; 2 - urgent; 4 - write;
* 或者1/2/4的位组合值 */
int SockCheckReadOrWrite(SOCKET sock, DWORD timeout, SOCKET forceBreakSock DEFAULT_PARAM(INVALID_SOCKET));

/* 带超时socket接收函数，返回值同recv()*/
int SockReadByTime(SOCKET sock, void* buf, int len, DWORD timeout, SOCKET forceBreakSock DEFAULT_PARAM(INVALID_SOCKET));
/* 带超时socket发送函数，返回值同send()*/
int SockWriteByTime(SOCKET sock, const void* data, int len, DWORD timeout, SOCKET forceBreakSock DEFAULT_PARAM(INVALID_SOCKET));
/* 返回值: ==len - 指定长度数据接收完成; <0 - 发生错误或对方关闭，此情况应立即关闭此SOCKET */
int SockReadAllByTime(SOCKET sock, void* buf, int len, DWORD timeout, SOCKET forceBreakSock DEFAULT_PARAM(INVALID_SOCKET));
/* 返回值: ==len - 指定长度数据发送完成, <0 - 发生错误或对方关闭，此情况应立即关闭此SOCKET */
int SockWriteAllByTime(SOCKET sock, const void* data, int len, DWORD timeout, SOCKET forceBreakSock DEFAULT_PARAM(INVALID_SOCKET));

/* 下面两个函数仅用于tcp非阻塞模式的数据的接收和发送 */
/* 返回值: <0 - 发生错误或断线; >=0 - 读到的数据长度 */
int SockTcpNonBlockReadOver(SOCKET sock, void* buf, int len, int* lastError DEFAULT_PARAM(NULL));
/* 返回值: <0 - 发生错误或断线; >=0 - 写成功的数据长度 */
int SockTcpNonBlockWriteOver(SOCKET sock, const void* data, int len, int* lastError DEFAULT_PARAM(NULL));


/* 离散buf读/写 */

/* 下面的离散buf计算函数要求: bufCount >= 0 */

/* 统计读buf总长 */
// int SockTotalizeReadBufVec(const SockReadBuf* bufVec, int bufCount);
// /* 统计写buf总长 */
// int SockTotalizeWriteBufVec(const SockWriteBuf* bufVec, int bufCount);
// /* 计算离散剩余读sockbuf，leavedBufVec至少能存放bufCount个buf */
// void SockCalcReadBufVec(IN const SockReadBuf* bufVec, IN int bufCount, IN int receivedLen,
// 						OUT SockReadBuf* leavedBufVec, OUT int* leavedCount);
// /* 计算离散剩余写sockbuf，leavedBufVec至少能存放bufCount个buf */
// void SockCalcWriteBufVec(IN const SockWriteBuf* bufVec, IN int bufCount, IN int sentLen,
// 						OUT SockWriteBuf* leavedBufVec, OUT int* leavedCount);

/* 下面的离散buf读/写函数要求: bufCount > 0 */

/* 返回值: <0 - 发生错误或断线; >=0 - 读到的数据长度 */
// int SockReadVec(SOCKET sock, const SockReadBuf* bufVec, int bufCount, fnMemAllocFuncPtr fnAlloc DEFAULT_PARAM(NULL), fnMemFreeFuncPtr fnFree DEFAULT_PARAM(NULL));
// /* 返回值: <0 - 发生错误或断线; >=0 - 写成功的数据长度 */
// int SockWriteVec(SOCKET sock, const SockWriteBuf* bufVec, int bufCount, fnMemAllocFuncPtr fnAlloc DEFAULT_PARAM(NULL), fnMemFreeFuncPtr fnFree DEFAULT_PARAM(NULL));
// /* 带超时多buf读, 返回值同SockReadVec() */
// int SockReadVecByTime(SOCKET sock, const SockReadBuf* bufVec, int bufCount, DWORD timeout, fnMemAllocFuncPtr fnAlloc DEFAULT_PARAM(NULL), fnMemFreeFuncPtr fnFree DEFAULT_PARAM(NULL), SOCKET forceBreakSock DEFAULT_PARAM(INVALID_SOCKET));
// /* 带超时多buf写, 返回值同SockWriteVec() */
// int SockWriteVecByTime(SOCKET sock, const SockWriteBuf* bufVec, int bufCount, DWORD timeout, fnMemAllocFuncPtr fnAlloc DEFAULT_PARAM(NULL), fnMemFreeFuncPtr fnFree DEFAULT_PARAM(NULL), SOCKET forceBreakSock DEFAULT_PARAM(INVALID_SOCKET));
// /* 返回值: ==len - 指定长度数据接收完成; <0 - 发生错误或对方关闭，此情况应立即关闭此SOCKET */
// int SockReadVecAllByTime(SOCKET sock, const SockReadBuf* bufVec, int bufCount, DWORD timeout, fnMemAllocFuncPtr fnAlloc DEFAULT_PARAM(NULL), fnMemFreeFuncPtr fnFree DEFAULT_PARAM(NULL), SOCKET forceBreakSock DEFAULT_PARAM(INVALID_SOCKET));
// /* 返回值: ==len - 指定长度数据发送完成; <0 - 发生错误或对方关闭，此情况应立即关闭此SOCKET */
// int SockWriteVecAllByTime(SOCKET sock, const SockWriteBuf* bufVec, int bufCount, DWORD timeout, fnMemAllocFuncPtr fnAlloc DEFAULT_PARAM(NULL), fnMemFreeFuncPtr fnFree DEFAULT_PARAM(NULL), SOCKET forceBreakSock DEFAULT_PARAM(INVALID_SOCKET));

DWORD SockGetPeer(SOCKET sock, int* port /*= NULL*/); /* !!!此函数WIN32平台有阻塞操作，耗时较长（一般>10ms），请谨慎使用!!! */
DWORD SockGetLocal(SOCKET sock, int* port /*= NULL*/);
#ifdef __cplusplus
	IPPADDR SockGetPeer(SOCKET sock);
	IPPADDR SockGetLocal(SOCKET sock);
#endif

int SockSetReceiveBufSize(SOCKET sock, int len);
int SockGetReceiveBufSize(SOCKET sock, int* len);
int SockSetSendBufSize(SOCKET sock, int len);
int SockGetSendBufSize(SOCKET sock, int* len);
int SockSetReceiveTimeOut(SOCKET sock, DWORD timeout); /* 对阻塞方式并配合原始的recv()、read()等函数有效 */
int SockSetSendTimeOut(SOCKET sock, DWORD timeout); /* 对阻塞方式并配合原始的send()、write()等函数有效 */
int SockSetBroadcast(SOCKET sock);
int SockSetAddMemberShip(SOCKET sock, DWORD ip, DWORD nicIP);
int SockSetDropMemberShip(SOCKET sock, DWORD ip, DWORD nicIP);
int SockSetMulticastIF(SOCKET sock, DWORD nicIP);
int SockSetMulticastTTL(SOCKET sock, int ttl);
int SockSetMulticastLoop(SOCKET sock, int optval);
int SockSetKeepAlive(SOCKET sock, BOOL onoff);
int SockSetKeepAliveVals(SOCKET sock, BOOL onoff, int keepalivetime, int keepaliveinterval);
int SockSetNonBlock(SOCKET sock, BOOL onoff);
int SockSetReuseAddr(SOCKET sock, BOOL onoff);
int SockSetLinger(SOCKET sock, BOOL onoff, DWORD timeout);
int SockSetTcpNoDelay(SOCKET sock, BOOL bNoDelay);
int UDPBind2(SOCKET sock, DWORD ip, int port, DWORD nicIP);
int UDPBind(SOCKET sock, DWORD ip, int port);
int TCPBind(SOCKET sock, DWORD ip, int port, BOOL is_AF_LOCAL DEFAULT_PARAM(false));
int SockConnect(SOCKET sock, DWORD ip, int port, BOOL is_AF_LOCAL DEFAULT_PARAM(false));
int TCPConnectByTime(SOCKET sock, DWORD ip, int port, DWORD timeout, BOOL is_AF_LOCAL DEFAULT_PARAM(false));
int TCPConnect(SOCKET sock, DWORD ip, int port, BOOL is_AF_LOCAL DEFAULT_PARAM(false));
SOCKET TCPNewClientByTime2(DWORD ip, int port, DWORD timeout, DWORD localIP, int localPort, BOOL is_AF_LOCAL DEFAULT_PARAM(false));

int UDPConnect2(SOCKET sock, DWORD ip, int port, DWORD nicIP);
int UDPConnect(SOCKET sock, DWORD ip, int port);
/*如果发送数据报使用sendto函数结果"无法访问 ICMP 端口"响应和选择函数中的设置为 readfds，则程序返回 1，
后续调用recvfrom函数并不适用于 WSAECONNRESET （10054） 的错误响应。在 Microsoft Windows NT 4.0 中，
这种情况下会导致选择函数块或超时。*/
int UDPSetConnReset(SOCKET sock);

SOCKET UDPNewPeerReceiver(DWORD ip, int port);
SOCKET UDPNewPeerReceiver2(DWORD ip, int port, DWORD nicIP);

/* SOCKET UDPNewReceiver(int port); */
#define UDPNewReceiver(port) UDPNewPeerReceiver(INADDR_ANY, (port))

/* SOCKET UDPNewSender(); */
#define UDPNewSender UDPNew

SOCKET UDPNewPeerSender(DWORD ip, int port);
SOCKET UDPNewPeerSender2(DWORD ip, int port, DWORD nicIP);

/* reusePort, 是否在不同IP上用同一个port建立服务; numListen, 底层可缓冲的待处理连接数 */
SOCKET TCPNewServerEx(DWORD ip, int port, BOOL reusePort, int numListen, BOOL is_AF_LOCAL DEFAULT_PARAM(false));
SOCKET TCPNewServerForIP(DWORD ip, int port, BOOL is_AF_LOCAL DEFAULT_PARAM(false));
SOCKET TCPNewServer(int port, BOOL is_AF_LOCAL DEFAULT_PARAM(false));
SOCKET TCPAccept(SOCKET sock, DWORD* pip DEFAULT_PARAM(NULL), int* pport DEFAULT_PARAM(NULL));

SOCKET TCPNewClientByTime(DWORD ip, int port, DWORD timeout, BOOL is_AF_LOCAL DEFAULT_PARAM(false));
SOCKET TCPNewClient(DWORD ip, int port, BOOL is_AF_LOCAL DEFAULT_PARAM(false));


#ifdef __cplusplus

/// 对象析构时，若socket有效，则关闭SOCKET
class AutoSock
{
private:
	AutoSock(const AutoSock&);
	AutoSock& operator= (const AutoSock&);

public:
	SOCKET m_sock;

public:
	AutoSock()
		: m_sock(INVALID_SOCKET)
	{
	}
	AutoSock(SOCKET sock)
		: m_sock(sock)

	{
	}
	SOCKET Drop()
	{
		SOCKET s = m_sock;
		m_sock = INVALID_SOCKET;
		return s;
	}
	~AutoSock()
	{
		if (SockValid(m_sock))
			VERIFY(0 == SockClose(m_sock));
	}
};

#endif	/* #ifdef __cplusplus */

/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++函数实现++++++++++++++++++++++++++++*/
/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

inline DWORD SockResolveIP(const char* host)
{
	if (host == NULL || strlen(host) <= 0)
		return INADDR_NONE;
#if defined(WIN32)
	const struct hostent* hp;
	hp = gethostbyname(host);
	if (!hp)
		return INADDR_NONE;
	return ((const struct in_addr*)(hp->h_addr))->s_addr;
#elif defined(OS_IS_APPLE)
	const struct hostent* hp;
	hp = gethostbyname(host);
	if (!hp)
		return INADDR_NONE;
	return ((const struct in_addr*)(hp->h_addr))->s_addr;
#elif defined(__linux__)
	struct hostent hostInfo;
	struct hostent* pHostInfo;
	char buf[1024] = { 0 };
	int err;
	if (gethostbyname_r(host, &hostInfo, buf, sizeof(buf), &pHostInfo, &err) != 0)
		return INADDR_NONE;
	if (pHostInfo)
		return ((const struct in_addr*)(pHostInfo->h_addr))->s_addr;
	return INADDR_NONE;
#endif
	return INADDR_NONE;
}

inline BOOL SockInit()
{
#if defined(WIN32)
	WORD wVersionRequested;
	WSADATA wsaData;
	int high, low;
	static BOOL isInited = false;
	if (isInited)
		return true;
	for (high = 2; high >= 1; --high)
	{
		for (low = 2; low >= 0; --low)
		{
			wVersionRequested = MAKEWORD(low, high);
			if (0 == WSAStartup(wVersionRequested, &wsaData))
			{
				if (LOBYTE(wsaData.wVersion) == (BYTE)(low)
					&& HIBYTE(wsaData.wVersion) == (BYTE)(high))
				{
					isInited = true;
					return TRUE;
				}
			}
			WSACleanup();
		}
	}
	return FALSE;
#else
	return TRUE;
#endif
}

inline BOOL IsMulticastIP(DWORD ip)
{
	const DWORD lowerIP = (((DWORD)224 << 24) | ((DWORD)1 << 8));
	const DWORD upperIP = (((DWORD)239 << 24) | ((DWORD)255 << 16) | ((DWORD)255 << 8) | (DWORD)255);
	DWORD ip_hostOrder = ntohl(ip);
	return lowerIP <= ip_hostOrder && ip_hostOrder <= upperIP;
}

inline SOCKET TCPNew(BOOL is_AF_LOCAL /*= false*/)
{
#if defined(__linux__)
	return socket((is_AF_LOCAL ? AF_LOCAL : AF_INET), SOCK_STREAM, (is_AF_LOCAL ? IPPROTO_IP : IPPROTO_TCP));
#else
	(void)is_AF_LOCAL;
	return socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
#endif
}

#if defined(WIN32)
inline int SockClose(SOCKET sock)
{
	int r = closesocket(sock);
	if (0 != r && (WSAEWOULDBLOCK == GetLastError() || WSANOTINITIALISED == GetLastError()))
		r = 0;
	return r;
}
#elif defined(__linux__)
inline int SockClose(SOCKET sock)
{
	return close(sock);
}
#endif

inline int SockGetError(SOCKET sock)
{
	int soErr;
	socklen_t len = sizeof(soErr);
	int r = getsockopt(sock, SOL_SOCKET, SO_ERROR, (SOCKPARM)&soErr, &len);
	return (r == 0 ? soErr : r);
}

inline int SockGetType(SOCKET sock)
{
	int sockType;
	socklen_t len = sizeof(int);
	int r = getsockopt(sock, SOL_SOCKET, SO_TYPE, (SOCKPARM)&sockType, &len);
	return r == 0 ? sockType : r;
}

#if defined(__linux__)
inline int SockRead(SOCKET sock, void* buf, int len)
{
	int r = recv(sock, (char*)buf, len, 0);
	while (r<0 && errno == EINTR)
	{
		Sleep(1);
		r = recv(sock, (char*)buf, len, 0);
	}
	return r;
}

inline int SockWrite(SOCKET sock, const void* data, int len)
{
	int r = send(sock, (const char*)data, len, 0);
	while (r<0 && errno == EINTR)
	{
		Sleep(1);
		r = send(sock, (const char*)data, len, 0);
	}
	return r;
}
#else
inline int SockRead(SOCKET sock, void* buf, int len)
{
	return recv(sock, (char*)buf, len, 0);
}
inline int SockWrite(SOCKET sock, const void* data, int len)
{
	return send(sock, (const char*)data, len, 0);
}
#endif

inline int SockSendTo(SOCKET sock, DWORD ip, int port, const void* data, int len)
{
	struct sockaddr_in sinAddr;
	memset(&sinAddr, 0, sizeof(sinAddr));
	sinAddr.sin_family = AF_INET;
	sinAddr.sin_addr.s_addr = ip;
	sinAddr.sin_port = htons((WORD)port);
#if defined(VXWORKS)
	return sendto(sock, (caddr_t)data, len, 0, (struct sockaddr*)&sinAddr, sizeof(sinAddr));
#elif defined(WIN32)
	return sendto(sock, (const char*)data, len, 0, (struct sockaddr*)&sinAddr, sizeof(sinAddr));
#elif defined(__linux__)
	{
		int r = sendto(sock, (const char*)data, len, 0, (struct sockaddr*)&sinAddr, sizeof(sinAddr));
		while (r<0 && errno == EINTR)
		{
			Sleep(1);
			r = sendto(sock, (const char*)data, len, 0, (struct sockaddr*)&sinAddr, sizeof(sinAddr));
		}
		return r;
	}
#endif
}

inline int SockReceiveFrom(SOCKET sock, DWORD* pip /*= NULL*/, int* pport /*= NULL*/, void* buf, int len)
{
	struct sockaddr_in sinAddr;
	socklen_t addrlen = sizeof(sinAddr);
	int ret = recvfrom(sock, (char*)buf, len, 0, (struct sockaddr*)&sinAddr, &addrlen);
#if defined(__linux__)
	while (ret<0 && errno == EINTR)
	{
		Sleep(1);
		ret = recvfrom(sock, (char*)buf, len, 0, (struct sockaddr*)&sinAddr, &addrlen);
	}
#endif
	if (ret <= 0)
	{
		if (pip)
			*pip = INADDR_NONE;
		if (pport)
			*pport = -1;
		return ret;
	}
	if (pip)
		*pip = sinAddr.sin_addr.s_addr;
	if (pport)
		*pport = ntohs(sinAddr.sin_port);
	return ret;
}

inline int SockPoll(SOCKET sock, int events, DWORD timeout, SOCKET forceBreakSock /*DEFAULT_PARAM(INVALID_SOCKET)*/)
{
	return FDPoll(sock, events, timeout, forceBreakSock);
}

inline int SockCheckRead(SOCKET sock, DWORD timeout, SOCKET forceBreakSock /*DEFAULT_PARAM(INVALID_SOCKET)*/)
{
	return SockPoll(sock, 0x1, timeout, forceBreakSock);
}

inline int SockCheckWrite(SOCKET sock, DWORD timeout, SOCKET forceBreakSock /*DEFAULT_PARAM(INVALID_SOCKET)*/)
{
	return SockPoll(sock, 0x4, timeout, forceBreakSock);
}

inline int SockCheckReadOrWrite(SOCKET sock, DWORD timeout, SOCKET forceBreakSock /*DEFAULT_PARAM(INVALID_SOCKET)*/)
{
	return SockPoll(sock, 0x7, timeout, forceBreakSock);
}

inline int SockReadByTime(SOCKET sock, void* buf, int len, DWORD timeout, SOCKET forceBreakSock /*DEFAULT_PARAM(INVALID_SOCKET)*/)
{
	int r = SockCheckRead(sock, timeout, forceBreakSock);
	if (r <= 0)
		return r;
	return SockRead(sock, buf, len);
}

inline int SockWriteByTime(SOCKET sock, const void* data, int len, DWORD timeout, SOCKET forceBreakSock /*DEFAULT_PARAM(INVALID_SOCKET)*/)
{
	int r = SockCheckWrite(sock, timeout, forceBreakSock);
	if (r <= 0)
		return r;
	return SockWrite(sock, data, len);
}

inline int SockReadAllByTime(SOCKET sock, void* buf, int len, DWORD timeout, SOCKET forceBreakSock /*DEFAULT_PARAM(INVALID_SOCKET)*/)
{
	int n = len, k, nRead = 0;
	BYTE* p = (BYTE*)buf;
	while (n > 0)
	{
		k = SockReadByTime(sock, p, n, timeout, forceBreakSock);
		if (k < 0)
			return k;
		if (k == 0)
			return -1/*nRead*/;
		nRead += k;
		p += k;
		n -= k;
	}
	return nRead;
}

inline int SockWriteAllByTime(SOCKET sock, const void* data, int len, DWORD timeout, SOCKET forceBreakSock /*DEFAULT_PARAM(INVALID_SOCKET)*/)
{
	int n = len, k, nWrite = 0;
	const BYTE* p = (const BYTE*)data;
	while (n > 0)
	{
		k = SockWriteByTime(sock, p, n, timeout, forceBreakSock);
		if (k < 0)
			return k;
		if (k == 0)
			return -1/*nWrite*/;
		nWrite += k;
		p += k;
		n -= k;
	}
	return nWrite;
}

inline int SockTcpNonBlockReadOver(SOCKET sock, void* buf, int len, int* lastError /*DEFAULT_PARAM(NULL)*/)
{
	int n = len;
	int nRead = 0;
	BYTE* p = (BYTE*)buf;
	if (lastError)
		*lastError = 0;
	while (n > 0)
	{
		int k = SockRead(sock, p, n);
		if (k < 0)
		{
			int err = WSAGetLastError();
			if (lastError)
				*lastError = err;
			if (nRead>0 || WSAENOBUFS == err || WSAEWOULDBLOCK == err)
				return nRead;
			else
				return -1;
		}
		if (k == 0)
			return (nRead>0 ? nRead : -1);

		nRead += k;
		p += k;
		n -= k;
	}
	return nRead;
}


inline int SockTcpNonBlockWriteOver(SOCKET sock, const void* data, int len, int* lastError /*DEFAULT_PARAM(NULL)*/)
{
	int n = len;
	int nWrite = 0;
	const BYTE* p = (const BYTE*)data;
	if (lastError)
		*lastError = 0;
	while (n > 0)
	{
		int k = SockWrite(sock, p, n);
		if (k < 0)
		{
			int err = WSAGetLastError();
			if (lastError)
				*lastError = err;
			if (nWrite>0 || WSAENOBUFS == err || WSAEWOULDBLOCK == err)
				return nWrite;
			else
				return -1;
		}
		if (k == 0)
			return nWrite;

		nWrite += k;
		p += k;
		n -= k;
	}
	return nWrite;
}


// inline int SockTotalizeReadBufVec(const SockReadBuf* bufVec, int bufCount)
// {
// 	int total = 0;
// 	int i;
// 	ASSERT(bufCount >= 0);
// 	for(i=0; i<bufCount; ++i)
// 	{
// 		ASSERT(bufVec && bufVec[i].buf && bufVec[i].len>0);
// 		total += bufVec[i].len;
// 	}
// 	return total;
// }
// 
// inline int SockTotalizeWriteBufVec(const SockWriteBuf* bufVec, int bufCount)
// 	{	return SockTotalizeReadBufVec((const SockReadBuf*)bufVec, bufCount);	}
// 
// inline void SockCalcReadBufVec(IN const SockReadBuf* bufVec, IN int bufCount, IN int receivedLen,
// 								OUT SockReadBuf* leavedBufVec, OUT int* leavedCount)
// {
// 	int bytesSum = 0;
// 	int i;
// 	ASSERT(bufCount>=0 && receivedLen>=0);
// 	ASSERT(leavedBufVec && leavedCount);
// 	*leavedCount = 0;
// 	for(i=0; i<bufCount; ++i)
// 	{
// 		ASSERT(bufVec);
// 		if(receivedLen <= bytesSum) /* 一点都没处理 */
// 		{
// 			leavedBufVec[*leavedCount] = bufVec[i];
// 			++(*leavedCount);
// 		}
// 		else if(bytesSum<receivedLen && receivedLen<bytesSum+bufVec[i].len) /* 部分处理 */
// 		{
// 			leavedBufVec[*leavedCount].buf = (BYTE*)bufVec[i].buf + (receivedLen-bytesSum);
// 			leavedBufVec[*leavedCount].len = bufVec[i].len - (receivedLen-bytesSum);
// 			++(*leavedCount);
// 		}
// 		else
// 			ASSERT(true); /* 已经处理完毕 */
// 
// 		bytesSum += bufVec[i].len;
// 	}
// }
// 
// inline void SockCalcWriteBufVec(IN const SockWriteBuf* bufVec, IN int bufCount, IN int sentLen,
// 								OUT SockWriteBuf* leavedBufVec, OUT int* leavedCount)
// 	{	SockCalcReadBufVec((const SockReadBuf*)bufVec, bufCount, sentLen, (SockReadBuf*)leavedBufVec, leavedCount);	}

// inline int SockReadVec(SOCKET sock, const SockReadBuf* bufVec, int bufCount, fnMemAllocFuncPtr fnAlloc /*= NULL*/, fnMemFreeFuncPtr fnFree /*= NULL*/)
// {
// 	if(NULL==bufVec || bufCount<=0)
// 	{
// 		ASSERT(false);
// 		return -1;
// 	}
// 
// 	{
// #if defined(WIN32)
// 	int err;
// 	DWORD n;
// 	DWORD flags = 0;
// 	int i;
// 	BEGIN_SMART_BUF_EX(wsaBuf_sm, WSABUF, wsaBuf, SOCK_BUF_STACK_COUNT, bufCount, fnAlloc);
// 	for(i=0; i<bufCount; ++i)
// 	{
// 		if(bufVec[i].len > 0)
// 		{
// 			if(NULL == bufVec[i].buf)
// 			{
// 				END_SMART_BUF_EX(wsaBuf_sm, fnFree);
// 				ASSERT(false);
// 				return -1;
// 			}
// 		}
// 		else if(bufVec[i].len < 0)
// 		{
// 			END_SMART_BUF_EX(wsaBuf_sm, fnFree);
// 			ASSERT(false);
// 			return -1;
// 		}
// 		wsaBuf[i].buf = (char*)bufVec[i].buf;
// 		wsaBuf[i].len = bufVec[i].len;
// 	}
// 	if(0 == WSARecv(sock, wsaBuf, (DWORD)bufCount, &n, &flags, NULL, NULL))
// 	{
// 		END_SMART_BUF_EX(wsaBuf_sm, fnFree);
// 		return (int)n;
// 	}
// 
// 	err = WSAGetLastError();
// 	if(WSAENOBUFS==err || WSAEWOULDBLOCK==err)
// 	{
// 		END_SMART_BUF_EX(wsaBuf_sm, fnFree);
// 		return 0;
// 	}
// 
// 	END_SMART_BUF_EX(wsaBuf_sm, fnFree);
// 	return -1;
// #elif defined(__linux__)
// 	{
// 		int r;
// 		int i;
// 		BEGIN_SMART_BUF_EX(ioVec_sm, struct iovec, ioVec, SOCK_BUF_STACK_COUNT, bufCount, fnAlloc);
// 		for(i=0; i<bufCount; ++i)
// 		{
// 			if(bufVec[i].len > 0)
// 			{
// 				if(NULL == bufVec[i].buf)
// 				{
// 					END_SMART_BUF_EX(ioVec_sm, fnFree);
// 					ASSERT(false);
// 					return -1;
// 				}
// 			}
// 			else if(bufVec[i].len < 0)
// 			{
// 				END_SMART_BUF_EX(ioVec_sm, fnFree);
// 				ASSERT(false);
// 				return -1;
// 			}
// 			ioVec[i].iov_base = bufVec[i].buf;
// 			ioVec[i].iov_len = bufVec[i].len;
// 		}
// 		r = (int)readv(sock, ioVec, bufCount);
// 		while(r<0 && errno==EINTR)
// 		{
// 			Sleep(1);
// 			r = (int)readv(sock, ioVec, bufCount);
// 		}
// 		if(r < 0)
// 		{
// 			int err = WSAGetLastError();
// 			if(WSAENOBUFS==err || WSAEWOULDBLOCK==err)
// 				r = 0;
// 		}
// 		else if(r == 0) /* 对方关闭 */
// 			r = -1;
// 
// 		END_SMART_BUF_EX(ioVec_sm, fnFree);
// 		return r;
// 	}
// #else
// 	#error This platform dont support scatter and gather IO.
// #endif
// 	}
// }

// inline int SockWriteVec(SOCKET sock, const SockWriteBuf* bufVec, int bufCount, fnMemAllocFuncPtr fnAlloc /*= NULL*/, fnMemFreeFuncPtr fnFree /*= NULL*/)
// {
// 	if(NULL==bufVec || bufCount<=0)
// 	{
// 		ASSERT(false);
// 		return -1;
// 	}
// 
// 	{
// #if defined(WIN32)
// 	int err;
// 	DWORD n;
// 	int i;
// 	BEGIN_SMART_BUF_EX(wsaBuf_sm, WSABUF, wsaBuf, SOCK_BUF_STACK_COUNT, bufCount, fnAlloc);
// 	for(i=0; i<bufCount; ++i)
// 	{
// 		if(bufVec[i].len > 0)
// 		{
// 			if(NULL == bufVec[i].data)
// 			{
// 				END_SMART_BUF_EX(wsaBuf_sm, fnFree);
// 				ASSERT(false);
// 				return -1;
// 			}
// 		}
// 		else if(bufVec[i].len < 0)
// 		{
// 			END_SMART_BUF_EX(wsaBuf_sm, fnFree);
// 			ASSERT(false);
// 			return -1;
// 		}
// 		wsaBuf[i].buf = (char*)bufVec[i].data;
// 		wsaBuf[i].len = bufVec[i].len;
// 	}
// 	if(0 == WSASend(sock, wsaBuf, (DWORD)bufCount, &n, 0, NULL, NULL))
// 	{
// 		END_SMART_BUF_EX(wsaBuf_sm, fnFree);
// 		return (int)n;
// 	}
// 
// 	err = WSAGetLastError();
// 	if(WSAENOBUFS==err || WSAEWOULDBLOCK==err)
// 	{
// 		END_SMART_BUF_EX(wsaBuf_sm, fnFree);
// 		return 0;
// 	}
// 
// 	END_SMART_BUF_EX(wsaBuf_sm, fnFree);
// 	return -1;
// #elif defined(__linux__)
// 	{
// 		int r;
// 		int i;
// 		BEGIN_SMART_BUF_EX(ioVec_sm, struct iovec, ioVec, SOCK_BUF_STACK_COUNT, bufCount, fnAlloc);
// 		for(i=0; i<bufCount; ++i)
// 		{
// 			if(bufVec[i].len > 0)
// 			{
// 				if(NULL == bufVec[i].data)
// 				{
// 					END_SMART_BUF_EX(ioVec_sm, fnFree);
// 					ASSERT(false);
// 					return -1;
// 				}
// 			}
// 			else if(bufVec[i].len < 0)
// 			{
// 				END_SMART_BUF_EX(ioVec_sm, fnFree);
// 				ASSERT(false);
// 				return -1;
// 			}
// 			ioVec[i].iov_base = (void*)bufVec[i].data;
// 			ioVec[i].iov_len = bufVec[i].len;
// 		}
// 		r = (int)writev(sock, ioVec, bufCount);
// 		if(r<0 && errno==EINVAL)
// 		{
// 			/* 可能是IOV_MAX限制 */
// 			BYTE* tbuf;
// 			BYTE* p;
// 			int i;
// 			int si = 0;
// 			for(i=0; i<bufCount; ++i)
// 				si += ioVec[i].iov_len;
// 			tbuf = (BYTE*)(fnAlloc ? fnAlloc(si) : malloc(si));
// 			if(NULL == tbuf)
// 			{
// 				END_SMART_BUF_EX(ioVec_sm, fnFree);
// 				return -1;
// 			}
// 			p = tbuf;
// 			for(i=0; i<bufCount; ++i)
// 			{
// 				memcpy(p, ioVec[i].iov_base, ioVec[i].iov_len);
// 				p += ioVec[i].iov_len;
// 			}
// 			ASSERT(p-tbuf == si);
// 
// 			/* r = SockWrite(sock, tbuf, si); */
// 			r = SockTcpNonBlockWriteOver(sock, tbuf, si, NULL);
// 
// 			if(fnFree)
// 				fnFree(tbuf);
// 			else
// 				free(tbuf);
// 		}
// 		else
// 		{
// 			while(r<0 && errno==EINTR)
// 			{
// 				Sleep(1);
// 				r = (int)writev(sock, ioVec, bufCount);
// 			}
// 			if(r < 0)
// 			{
// 				int err = WSAGetLastError();
// 				if(WSAENOBUFS==err || WSAEWOULDBLOCK==err)
// 					r = 0;
// 			}
// 		}
// 
// 		END_SMART_BUF_EX(ioVec_sm, fnFree);
// 		return r;
// 	}
// #else
// 	#error This platform dont support scatter and gather IO.
// #endif
// 	}
// }

// inline int SockReadVecByTime(SOCKET sock, const SockReadBuf* bufVec, int bufCount, DWORD timeout, fnMemAllocFuncPtr fnAlloc /*= NULL*/, fnMemFreeFuncPtr fnFree /*= NULL*/, SOCKET forceBreakSock /*DEFAULT_PARAM(INVALID_SOCKET)*/)
// {
// 	int r = SockCheckRead(sock, timeout, forceBreakSock);
// 	if(r <= 0)
// 		return r;
// 	return SockReadVec(sock, bufVec, bufCount, fnAlloc, fnFree);
// }
// 
// inline int SockWriteVecByTime(SOCKET sock, const SockWriteBuf* bufVec, int bufCount, DWORD timeout, fnMemAllocFuncPtr fnAlloc /*= NULL*/, fnMemFreeFuncPtr fnFree /*= NULL*/, SOCKET forceBreakSock /*DEFAULT_PARAM(INVALID_SOCKET)*/)
// {
// 	int r = SockCheckWrite(sock, timeout, forceBreakSock);
// 	if(r <= 0)
// 		return r;
// 	return SockWriteVec(sock, bufVec, bufCount, fnAlloc, fnFree);
// }
// 
// inline int SockReadVecAllByTime(SOCKET sock, const SockReadBuf* bufVec, int bufCount, DWORD timeout, fnMemAllocFuncPtr fnAlloc /*= NULL*/, fnMemFreeFuncPtr fnFree /*= NULL*/, SOCKET forceBreakSock /*DEFAULT_PARAM(INVALID_SOCKET)*/)
// {
// 	int readed = 0;
// 	int total;
// 	BEGIN_SMART_BUF_EX(tmpBufs_sm, SockReadBuf, tmpBufs, SOCK_BUF_STACK_COUNT, bufCount, fnAlloc);
// 	total = SockTotalizeReadBufVec(bufVec, bufCount);
// 	ASSERT(total > 0);
// 	while(readed < total)
// 	{
// 		int tmpBufCount = 0;
// 		int r;
// 		SockCalcReadBufVec(bufVec, bufCount, readed, tmpBufs, &tmpBufCount);
// 		ASSERT(tmpBufCount > 0);
// 		r = SockReadVecByTime(sock, tmpBufs, tmpBufCount, timeout, fnAlloc, fnFree, forceBreakSock);
// 		if(r < 0)
// 		{
// 			END_SMART_BUF_EX(tmpBufs_sm, fnFree);
// 			return r;
// 		}
// 		if(r == 0)
// 		{
// 			END_SMART_BUF_EX(tmpBufs_sm, fnFree);
// 			return -1/*readed*/;
// 		}
// 		readed += r;
// 	}
// 
// 	END_SMART_BUF_EX(tmpBufs_sm, fnFree);
// 	return readed;
// }
// 
// inline int SockWriteVecAllByTime(SOCKET sock, const SockWriteBuf* bufVec, int bufCount, DWORD timeout, fnMemAllocFuncPtr fnAlloc /*= NULL*/, fnMemFreeFuncPtr fnFree /*= NULL*/, SOCKET forceBreakSock /*DEFAULT_PARAM(INVALID_SOCKET)*/)
// {
// 	int wrote = 0;
// 	int total;
// 	BEGIN_SMART_BUF_EX(tmpBufs_sm, SockWriteBuf, tmpBufs, SOCK_BUF_STACK_COUNT, bufCount, fnAlloc);
// 	total = SockTotalizeWriteBufVec(bufVec, bufCount);
// 	ASSERT(total > 0);
// 	while(wrote < total)
// 	{
// 		int tmpBufCount = 0;
// 		int r;
// 		SockCalcWriteBufVec(bufVec, bufCount, wrote, tmpBufs, &tmpBufCount);
// 		ASSERT(tmpBufCount > 0);
// 		r = SockWriteVecByTime(sock, tmpBufs, tmpBufCount, timeout, fnAlloc, fnFree, forceBreakSock);
// 		if(r < 0)
// 		{
// 			END_SMART_BUF_EX(tmpBufs_sm, fnFree);
// 			return r;
// 		}
// 		if(r == 0)
// 		{
// 			END_SMART_BUF_EX(tmpBufs_sm, fnFree);
// 			return -1/*wrote*/;
// 		}
// 		wrote += r;
// 	}
// 
// 	END_SMART_BUF_EX(tmpBufs_sm, fnFree);
// 	return wrote;
// }

inline DWORD SockGetPeer(SOCKET sock, int* port /*= NULL*/)
{
	struct sockaddr_in sn;
	socklen_t socklen = sizeof(sn);
	if (0 != getpeername(sock, (struct sockaddr*)&sn, &socklen))
	{
		if (port)
			*port = -1;
		return INADDR_NONE;
	}
	if (port)
		*port = ntohs(sn.sin_port);
	return sn.sin_addr.s_addr;
}

inline DWORD SockGetLocal(SOCKET sock, int* port /*= NULL*/)
{
	struct sockaddr_in sn;
	socklen_t socklen = sizeof(sn);
	if (0 != getsockname(sock, (struct sockaddr*)&sn, &socklen))
	{
		if (port)
			*port = -1;
		return INADDR_NONE;
	}
	if (port)
		*port = ntohs(sn.sin_port);
	return sn.sin_addr.s_addr;
}

#ifdef __cplusplus
inline IPPADDR SockGetPeer(SOCKET sock)
{
	int port = -1;
	DWORD ip = SockGetPeer(sock, &port);
	return IPPADDR(ip, port);
}
inline IPPADDR SockGetLocal(SOCKET sock)
{
	int port = -1;
	DWORD ip = SockGetLocal(sock, &port);
	return IPPADDR(ip, port);
}
#endif

inline int SockSetReceiveBufSize(SOCKET sock, int len)
{
	return setsockopt(sock, SOL_SOCKET, SO_RCVBUF, (SOCKPARM)&len, sizeof(len));
}

inline int SockGetReceiveBufSize(SOCKET sock, int* len)
{
	ASSERT(len); {socklen_t l = sizeof(*len); return getsockopt(sock, SOL_SOCKET, SO_RCVBUF, (SOCKPARM)len, &l); }
}

inline int SockSetSendBufSize(SOCKET sock, int len)
{
	return setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (SOCKPARM)&len, sizeof(len));
}

inline int SockGetSendBufSize(SOCKET sock, int* len)
{
	ASSERT(len); {socklen_t l = sizeof(*len); return getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (SOCKPARM)len, &l); }
}

inline int SockSetReceiveTimeOut(SOCKET sock, DWORD timeout)
{
	struct timeval timeo;
	if (INFINITE == timeout)
	{
		timeo.tv_sec = 0x7FFFFFFF;
		timeo.tv_usec = 0;
	}
	else
	{
		timeo.tv_sec = (timeout & 0x7FFFFFFF) / 1000;
		timeo.tv_usec = (timeout & 0x7FFFFFFF) % 1000 * 1000;
	}
	return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (SOCKPARM)&timeo, sizeof(timeo));
}

inline int SockSetSendTimeOut(SOCKET sock, DWORD timeout)
{
	struct timeval timeo;
	if (INFINITE == timeout)
	{
		timeo.tv_sec = 0x7FFFFFFF;
		timeo.tv_usec = 0;
	}
	else
	{
		timeo.tv_sec = (timeout & 0x7FFFFFFF) / 1000;
		timeo.tv_usec = (timeout & 0x7FFFFFFF) % 1000 * 1000;
	}
	return setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (SOCKPARM)&timeo, sizeof(timeo));
}

inline int SockSetBroadcast(SOCKET sock)
{
	long lVal = 1; return setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (SOCKPARM)&lVal, sizeof(lVal));
}

inline int SockSetAddMemberShip(SOCKET sock, DWORD ip, DWORD nicIP)
{
	struct ip_mreq mreq; /* 组播请求参数 */
	mreq.imr_multiaddr.s_addr = ip;
	mreq.imr_interface.s_addr = nicIP;
	return setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (SOCKPARM)&mreq, sizeof(mreq));
}

inline int SockSetDropMemberShip(SOCKET sock, DWORD ip, DWORD nicIP)
{
	struct ip_mreq mreq;
	mreq.imr_multiaddr.s_addr = ip;
	mreq.imr_interface.s_addr = nicIP;
	return setsockopt(sock, IPPROTO_IP, IP_DROP_MEMBERSHIP, (SOCKPARM)&mreq, sizeof(mreq));
}

inline int SockSetMulticastIF(SOCKET sock, DWORD nicIP)
{
	return setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (SOCKPARM)&nicIP, sizeof(struct in_addr));
}

inline int SockSetMulticastTTL(SOCKET sock, int ttl)
{
#if defined(VXWORKS)
	BYTE ttlx = (BYTE)ttl;
	return setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (SOCKPARM)&ttlx, sizeof(ttlx));
#else
	return setsockopt(sock, IPPROTO_IP, IP_MULTICAST_TTL, (SOCKPARM)&ttl, sizeof(ttl));
#endif
}

inline int SockSetMulticastLoop(SOCKET sock, int optval)
{
	return setsockopt(sock, IPPROTO_IP, IP_MULTICAST_LOOP, (SOCKPARM)&optval, sizeof(optval));
}

inline int SockSetKeepAlive(SOCKET sock, BOOL onoff)
{
	return setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (SOCKPARM)&onoff, sizeof(onoff));
}

inline int SockSetKeepAliveVals(SOCKET sock, BOOL onoff, int keepalivetime, int keepaliveinterval)
{
#if defined(WIN32)
	DWORD ulBytesReturn = 0;
	struct tcp_keepalive inKeepAlive;
	inKeepAlive.onoff = (onoff ? 1 : 0);
	inKeepAlive.keepalivetime = keepalivetime;
	inKeepAlive.keepaliveinterval = keepaliveinterval;
	return WSAIoctl(sock, SIO_KEEPALIVE_VALS, &inKeepAlive,
		sizeof(struct tcp_keepalive),
		NULL, 0,
		&ulBytesReturn,
		NULL,
		NULL);

#elif defined(__linux__)
	int ret;
	int count;
	ret = setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, (SOCKPARM)&onoff, sizeof(onoff));
	if (ret < 0)
		return ret;

	count = 10;
	ret = setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, (SOCKPARM)&count, sizeof(count));
	if (ret < 0)
		return ret;

	keepalivetime = keepalivetime / 1000;
	if (keepalivetime == 0)
		keepalivetime = 1;
#if !defined(OS_IS_APPLE)
	ret = setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, (SOCKPARM)&keepalivetime, sizeof(keepalivetime));
	if (ret < 0)
		return ret;
#endif

	keepaliveinterval = keepaliveinterval / 1000;
	if (keepaliveinterval == 0)
		keepaliveinterval = 1;
	ret = setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, (SOCKPARM)&keepaliveinterval, sizeof(keepaliveinterval));
	return ret;
#endif
}

inline int SockSetNonBlock(SOCKET sock, BOOL onoff)
{
	DWORD val = (onoff ? 1 : 0); return ioctlsocket(sock, FIONBIO, &val);
}

inline int SockSetReuseAddr(SOCKET sock, BOOL onoff)
{
	int opt = (onoff ? 1 : 0); return setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (SOCKPARM)&opt, sizeof(opt));
}

inline int SockSetLinger(SOCKET sock, BOOL onoff, DWORD timeout)
{
	struct linger l;
	l.l_onoff = (WORD)(onoff ? 1 : 0);
	l.l_linger = (WORD)(INFINITE == timeout ? 0xffff : ((timeout & 0x7FFFFFFF) / 1000));
	if (0 == l.l_linger && 0 != timeout)
		l.l_linger = 1;
	return setsockopt(sock, SOL_SOCKET, SO_LINGER, (SOCKPARM)&l, sizeof(l));
}

inline int SockSetTcpNoDelay(SOCKET sock, BOOL bNoDelay)
{
	int val = (bNoDelay ? 1 : 0); return setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (SOCKPARM)&val, sizeof(val));
}

inline int UDPBind2(SOCKET sock, DWORD ip, int port, DWORD nicIP)
{
	/* 是否多播/广播判断 */
	BOOL isMultiCast = IsMulticastIP(ip);

	struct sockaddr_in sinAddr;
	memset(&sinAddr, 0, sizeof(sinAddr));
#if defined(VXWORKS) || defined(__ECOS)
	sinAddr.sin_len = (BYTE)sizeof(sinAddr);
#endif
	sinAddr.sin_family = AF_INET;
	sinAddr.sin_addr.s_addr = ((isMultiCast || INADDR_BROADCAST == ip) ? INADDR_ANY : ip);
	sinAddr.sin_port = htons((WORD)port);
	if (0 != bind(sock, (struct sockaddr*)&sinAddr, sizeof(sinAddr)))
		return -1;

	if (INADDR_BROADCAST == ip) /* 广播 */
		return SockSetBroadcast(sock);

	if (isMultiCast) /* 组播 */
	{
#if defined(__linux__) && defined(__cplusplus)
		if (INADDR_ANY == nicIP)
			nicIP = IfcfgGetHostIp();
#endif
		if (SockSetAddMemberShip(sock, ip, nicIP))
			return -1;
	}

	return 0;
}

inline int UDPBind(SOCKET sock, DWORD ip, int port)
{
#if defined(VXWORKS)
	char localAddr[24];
	ifAddrGet("rtl0", localAddr);
	return UDPBind2(sock, ip, port, inet_addr(localAddr));
#else
	return UDPBind2(sock, ip, port, INADDR_ANY);
#endif
}

inline int TCPBind_inet_(SOCKET sock, DWORD ip, int port)
{
	struct sockaddr_in sinAddr;
	memset(&sinAddr, 0, sizeof(sinAddr));
	sinAddr.sin_family = AF_INET;
	sinAddr.sin_addr.s_addr = ip;
	sinAddr.sin_port = htons((WORD)port);
	if (0 != bind(sock, (struct sockaddr*)&sinAddr, sizeof(sinAddr)))
		return -1;
	return 0;
}

inline int TCPBind(SOCKET sock, DWORD ip, int port, BOOL is_AF_LOCAL /*= false*/)
{
#if defined(__linux__)
	if (!is_AF_LOCAL)
		return TCPBind_inet_(sock, ip, port);

	{
		struct sockaddr_un sunAddr;
		sunAddr.sun_family = AF_LOCAL;
		if (0 != bind(sock, (struct sockaddr*)&sunAddr, sizeof(sunAddr)))
			return -1;
		return 0;
	}
#else
	(void)is_AF_LOCAL;
	return TCPBind_inet_(sock, ip, port);
#endif
}

inline int SockConnect_inet_(SOCKET sock, DWORD ip, int port)
{
	int r;
	struct sockaddr_in sinAddr;
	memset(&sinAddr, 0, sizeof(sinAddr));
	sinAddr.sin_family = AF_INET;
	sinAddr.sin_addr.s_addr = ip;
	sinAddr.sin_port = htons((WORD)port);
	r = connect(sock, (struct sockaddr*)&sinAddr, sizeof(sinAddr));
#if defined(__linux__)
	while (r<0 && errno == EINTR)
	{
		Sleep(1);
		r = connect(sock, (struct sockaddr*)&sinAddr, sizeof(sinAddr));
	}
#endif
	return r;
}

inline int SockConnect(SOCKET sock, DWORD ip, int port, BOOL is_AF_LOCAL /*= false*/)
{
#if defined(__linux__)
	if (!is_AF_LOCAL)
		return SockConnect_inet_(sock, ip, port);

	{
		int r;
		struct stat fi;
		struct sockaddr_un sunAddr;
		sunAddr.sun_family = AF_LOCAL;
		r = connect(sock, (struct sockaddr*)&sunAddr, sizeof(sunAddr));
		while (r<0 && errno == EINTR)
		{
			Sleep(1);
			r = connect(sock, (struct sockaddr*)&sunAddr, sizeof(sunAddr));
		}
		return r;
	}
#else
	(void)is_AF_LOCAL;
	return SockConnect_inet_(sock, ip, port);
#endif
}

inline int TCPConnectByTime(SOCKET sock, DWORD ip, int port, DWORD timeout, BOOL is_AF_LOCAL /*= false*/)
{
	int r = SockSetNonBlock(sock, TRUE);
	if (r != 0)
		return r;

	/* 建立socket连接 */
	r = SockConnect(sock, ip, port, is_AF_LOCAL);
	if (0 == r)
		return SockSetNonBlock(sock, FALSE);

#if defined(__linux__)
	if (ENETUNREACH == errno)
	{
		SockSetNonBlock(sock, FALSE);
		return -1;
	}
#endif

	r = SockCheckWrite(sock, timeout, INVALID_SOCKET);
	if (r < 0)
		return r;
	if (r == 0)
		return -1;
	return SockSetNonBlock(sock, FALSE);
}

inline int TCPConnect(SOCKET sock, DWORD ip, int port, BOOL is_AF_LOCAL /*= false*/)
{
	return TCPConnectByTime(sock, ip, port, INFINITE, is_AF_LOCAL);
}

inline SOCKET TCPNewClientByTime(DWORD ip, int port, DWORD timeout, BOOL is_AF_LOCAL /*= false*/)
{
	int r;
	SOCKET sock = TCPNew(is_AF_LOCAL);
	if (!SockValid(sock))
		return sock;
	SockSetReuseAddr(sock, TRUE);
	r = TCPConnectByTime(sock, ip, port, timeout, is_AF_LOCAL);
	if (r != 0)
	{
		VERIFY(0 == SockClose(sock));
		return INVALID_SOCKET;
	}
	return sock;
}

inline SOCKET TCPNewClient(DWORD ip, int port, BOOL is_AF_LOCAL /*= false*/)
{
	return TCPNewClientByTime(ip, port, INFINITE, is_AF_LOCAL);
}

inline SOCKET TCPNewClientByTime2(DWORD ip, int port, DWORD timeout, DWORD localIP, int localPort, BOOL is_AF_LOCAL /*= false*/)
{
	int r;
	SOCKET sock = TCPNew(is_AF_LOCAL);
	if (!SockValid(sock))
		return sock;

	if (-1 == TCPBind(sock, localIP, localPort, is_AF_LOCAL))
	{
		VERIFY(0 == SockClose(sock));
		return INVALID_SOCKET;
	}
	SockSetReuseAddr(sock, TRUE);
	r = TCPConnectByTime(sock, ip, port, timeout, is_AF_LOCAL);
	if (r != 0)
	{
		VERIFY(0 == SockClose(sock));
		return INVALID_SOCKET;
	}
	return sock;
}

inline int UDPConnect2(SOCKET sock, DWORD ip, int port, DWORD nicIP)
{
	if (INADDR_BROADCAST == ip) /* 广播 */
	{
		if (SockSetBroadcast(sock))
			return -1;
	}
	else if (IsMulticastIP(ip)) /* 组播 */
	{
		if (SockSetMulticastTTL(sock, 32))
			return -1;
		if (SockSetMulticastIF(sock, nicIP))
			return -1;
	}

	return SockConnect(sock, ip, port, false);
}

inline int UDPConnect(SOCKET sock, DWORD ip, int port)
{
	// #if defined(VXWORKS)
	// 	char localAddr[24];
	// 	ifAddrGet("rtl0", localAddr);
	// 	return UDPConnect2(sock, ip, port, inet_addr(localAddr));
	// #else
	return UDPConnect2(sock, ip, port, INADDR_ANY);
	//#endif
}

inline SOCKET UDPNewPeerReceiver2(DWORD ip, int port, DWORD nicIP)
{
	SOCKET sock = UDPNew();
	if (sock == INVALID_SOCKET)
		return sock;
	SockSetReuseAddr(sock, TRUE);
	if (UDPBind2(sock, ip, port, nicIP))
	{
		VERIFY(0 == SockClose(sock));
		return INVALID_SOCKET;
	}
	return sock;
}

inline SOCKET UDPNewPeerReceiver(DWORD ip, int port)
{
	return UDPNewPeerReceiver2(ip, port, INADDR_ANY);
}

inline SOCKET UDPNewPeerSender2(DWORD ip, int port, DWORD nicIP)
{
	SOCKET sock = UDPNew();
	if (sock == INVALID_SOCKET)
		return sock;
	SockSetReuseAddr(sock, TRUE);
	if (UDPConnect2(sock, ip, port, nicIP))
	{
		VERIFY(0 == SockClose(sock));
		return INVALID_SOCKET;
	}
	return sock;
}

inline int UDPSetConnReset(SOCKET sock)
{
#if defined(WIN32)
	DWORD dwBytesReturned = 0;
	BOOL bNewBehavior = FALSE;
	DWORD status;

	// disable  new behavior using
	// IOCTL: SIO_UDP_CONNRESET
	status = WSAIoctl(sock, SIO_UDP_CONNRESET,
		&bNewBehavior, sizeof(bNewBehavior),
		NULL, 0, &dwBytesReturned,
		NULL, NULL);

	if (SOCKET_ERROR == status)
	{
		DWORD dwErr = WSAGetLastError();
		if (WSAEWOULDBLOCK == dwErr)
			return 0;	// nothing to do
		else
			return -1;
	}
	return 0;
#else
	sock;
	return 0;
#endif
}

inline SOCKET UDPNewPeerSender(DWORD ip, int port)
{
	return UDPNewPeerSender2(ip, port, INADDR_ANY);
}

inline SOCKET TCPNewServerEx_inet_(DWORD ip, int port, BOOL reusePort, int numListen)
{
	struct sockaddr_in sn;
	SOCKET sock = TCPNew(false);
	if (!SockValid(sock))
		return INVALID_SOCKET;
	if (reusePort)
		SockSetReuseAddr(sock, TRUE);
	sn.sin_family = AF_INET;
	sn.sin_addr.s_addr = ip;
	sn.sin_port = htons((short)port);
	if (0 == bind(sock, (struct sockaddr*)&sn, sizeof(sn)))
	{
		if (0 == listen(sock, numListen))
			return sock;
		else
			ASSERT(true);
	}
	else
		ASSERT(true);
	VERIFY(0 == SockClose(sock));
	return INVALID_SOCKET;
}


inline SOCKET TCPNewServerEx(DWORD ip, int port, BOOL reusePort, int numListen, BOOL is_AF_LOCAL /*= false*/)
{
#if defined(__linux__)
	if (!is_AF_LOCAL)
		return TCPNewServerEx_inet_(ip, port, reusePort, numListen);

	{
		struct sockaddr_un sunAddr;
		SOCKET sock = TCPNew(is_AF_LOCAL);
		if (!SockValid(sock))
			return INVALID_SOCKET;
		if (reusePort)
			SockSetReuseAddr(sock, TRUE);
		sunAddr.sun_family = AF_LOCAL;
		if (0 == bind(sock, (struct sockaddr*)&sunAddr, sizeof(sunAddr)))
		{
			if (0 == listen(sock, numListen))
				return sock;
		}
		else
			ASSERT(true);
		VERIFY(0 == SockClose(sock));
		return INVALID_SOCKET;
	}
#else
	(void)is_AF_LOCAL;
	return TCPNewServerEx_inet_(ip, port, reusePort, numListen);
#endif
}

inline SOCKET TCPNewServerForIP(DWORD ip, int port, BOOL is_AF_LOCAL /*= false*/)
{
	return TCPNewServerEx(ip, port, FALSE, 5, is_AF_LOCAL);
}

inline SOCKET TCPNewServer(int port, BOOL is_AF_LOCAL /*= false*/)
{
	return TCPNewServerEx(INADDR_ANY, port, FALSE, 5, is_AF_LOCAL);
}

inline SOCKET TCPAccept(SOCKET sock, DWORD* pip /*DEFAULT_PARAM(NULL)*/, int* pport /*DEFAULT_PARAM(NULL)*/)
{
	SOCKET sock2;
	struct sockaddr_in sn;
#ifdef WIN32
	int len = sizeof(sn);
#else
	socklen_t len = sizeof(sn);
#endif
	sock2 = accept(sock, (struct sockaddr*)&sn, &len);
#if defined(__linux__)
	while (!SockValid(sock2) && errno == EINTR)
	{
		Sleep(1);
		sock2 = accept(sock, (struct sockaddr*)&sn, &len);
	}
#endif
	if (SockValid(sock2))
	{
		if (pip)
			*pip = sn.sin_addr.s_addr;
		if (pport)
			*pport = ntohs(sn.sin_port);
	}
	return sock2;
}

#endif
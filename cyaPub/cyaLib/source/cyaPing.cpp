#include "cyaPing.h"

#if !defined(OS_IS_APPLE)

#include "cyaThread.h"
#include "cyaIpCvt.h"
#include "cyaMaxMin.h"

#define PACKET_SIZE		256
#define SEND_SIZE		56

#ifdef WIN32

#define ICMP_ECHO			8	//回射请求
#define ICMP_ECHOREPLY		0	//回射应答

#define IS_SMALL_EDITION	1

struct icmp			//ICMP头结构,ICMP报文被封装在IP数据报内部传输
{
	BYTE icmp_type;		//类型
	BYTE icmp_code;		//代码
	WORD icmp_cksum;	//校验和
	WORD icmp_id;		//标志
	WORD icmp_seq;		//序号
	DWORD icmp_data[1];	//负载数据
};

struct ip			//IP头结构
{
#if IS_SMALL_EDITION
	BYTE ip_hl : 4;	//ip报头长度(ip_hl << 2)/*位域，不能超过1个字节，连续的分配，这儿给每个都分配了4位*/
	BYTE ip_v : 4;	//版本
#else
	BYTE ip_v : 4;	//版本
	BYTE ip_hl : 4;	//ip报头长度(ip_hl << 2)
#endif
	BYTE ip_tos;	//服务类型
	WORD ip_len;	//总长度
	WORD ip_id;		//标志 
	WORD ip_off;	//分片偏移
	BYTE ip_ttl;	//生存时间
	BYTE ip_p;		//协议
	WORD ip_sum;	//检验和 
	DWORD ip_src;	//源IP地址
	DWORD ip_dst;	//目标IP地址
};

#endif	// #ifdef WIN32

static inline int GetICMPProtocol__()
{
	return IPPROTO_ICMP;
}

static WORD CalChksum__(WORD* addr, int len)
{
	int nLoop = len;
	int sum = 0;
	WORD *w = addr;
	WORD answer = 0;

	while (nLoop>1)
	{
		sum += *w++;/*先取内容，然后指针指向下一个WORD单元*/
		nLoop -= 2;
	}
	if (nLoop == 1)
	{
		*(BYTE*)(&answer) = *(BYTE*)w;
		sum += answer;
	}
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	answer = (WORD)~sum;/*sum按位取反*/
	return answer;
}

//打包
static int PackIcmpData__(int pack_no, WORD pid, BYTE* sendpacket)
{
	int packsize;
	struct icmp *icmpData;

	packsize = 8 + SEND_SIZE;
	icmpData = (struct icmp*)sendpacket;
	icmpData->icmp_type = ICMP_ECHO;
	icmpData->icmp_code = 0;
	icmpData->icmp_cksum = 0;
	icmpData->icmp_seq = (WORD)pack_no;
	icmpData->icmp_id = (WORD)pid;

	/*记录发送时间*/
#if defined(__linux__)
	icmpData->icmp_mask = GetTickCount();
#else
	icmpData->icmp_data[0] = GetTickCount();
#endif
	icmpData->icmp_cksum = CalChksum__((WORD*)icmpData, packsize); /*校验算法*/
	return packsize;
}

//解包
static int UnpackIcmpData__(BYTE* buf, int len, WORD pid)
{
	struct ip* ipData;
	struct icmp* icmpData;
	int iphdrlen;
	if (len < SIZEOF(struct ip))
		return -1;
	ipData = (struct ip*)buf;
	iphdrlen = ipData->ip_hl << 2;    /*求ip报头长度,即ip报头的长度标志乘4*/
	icmpData = (struct icmp*)(buf + iphdrlen);  /*越过ip报头,指向ICMP报头*/

	len -= iphdrlen;            /*ICMP报头及ICMP数据报的总长度*/
	if (len < 8)                /*小于ICMP报头长度则不合理*/
		return -1;

	/*确保所接收的是我所发的的ICMP的回应*/
	if ((icmpData->icmp_type == ICMP_ECHO) && (icmpData->icmp_id == (WORD)pid))
		return 1;

	if ((icmpData->icmp_type == ICMP_ECHOREPLY) && (icmpData->icmp_id == (WORD)pid))
	{
		// 		len = len-iphdrlen-8;
		// 		char szIp[16] = { 0 };
		// 		int rtt = GetTickCount() - *(DWORD*)(icmpData->icmp_data);
		// 		printf("Reply from %s: bytes=%d time<=%dms TTL=%d icmp_seq=%u\n",
		// 				GetStringIP(szIp, ipData->ip_src), len, rtt, ipData->ip_ttl, icmpData->icmp_seq);
		return 0;
	}

	return -1;
}

//发送
static int SendPacket__(SOCKET sock, DWORD ipAddr, WORD pid)
{
	static int pack_no = 0;
	BYTE sendPacket[PACKET_SIZE];
	memset(sendPacket, 0, sizeof(sendPacket));
	int nPacketSize = PackIcmpData__(pack_no++, pid, sendPacket); /*设置ICMP报头*/
	return nPacketSize == SockSendTo(sock, ipAddr, 0, sendPacket, nPacketSize) ? 0 : -1;
}

//接收
static int RecvPacket__(SOCKET sock, DWORD ms_timeout, DWORD ipAddr, WORD pid)
{
	BYTE recvpacket[PACKET_SIZE];
	int n;
	memset(recvpacket, 0, sizeof(recvpacket));
	DWORD recvIPAddr = 0;
	do
	{
		DWORD tick = GetTickCount();
		if (SockCheckRead(sock, ms_timeout) != 1)
			return -1;
		if ((n = SockReceiveFrom(sock, &recvIPAddr, NULL, recvpacket, sizeof(recvpacket))) <= 0)
			return -1;
		// 判断是否是自己Ping的回复
		if (recvIPAddr == ipAddr)
		{
			if (UnpackIcmpData__(recvpacket, n, pid) == 0)
				return 0;
		}
		if (INFINITE != ms_timeout)
			ms_timeout -= min(ms_timeout, (GetTickCount() - tick));
	} while (ms_timeout > 0);
	return -1;
}

CPingNoLock::CPingNoLock()
	: m_sockfd(INVALID_SOCKET)
{
	/*生成使用ICMP的原始套接字,在linux系统中,这种套接字只有在root权限下才能生成*/
	m_sockfd = SockRawNew(GetICMPProtocol__());
	if (!SockValid(m_sockfd))
		return;

	SockSetNonBlock(m_sockfd, true);
	// 	SockSetReceiveTimeOut(m_sockfd, 1000);
	// 	SockSetSendTimeOut(m_sockfd, 1000);

	/*扩大套接字接收缓冲区到64K这样做主要为了减小接收缓冲区溢出的
	的可能性,若无意中ping一个广播地址或多播地址,将会引来大量应答*/
	SockSetReceiveBufSize(m_sockfd, 64 * 1024);
}

CPingNoLock::~CPingNoLock()
{
	if (SockValid(m_sockfd))
		VERIFY(0 == SockClose(m_sockfd));
}

int CPingNoLock::Ping(DWORD ipAddr, DWORD ms_timeout)
{
	WORD pid = (WORD)OSThreadSelf();
	int ret = SendPacket__(m_sockfd, ipAddr, pid);
	if (ret != 0)
		return ret;
	return RecvPacket__(m_sockfd, ms_timeout, ipAddr, pid);
}

int CPingNoLock::Ping(const char* hostName, DWORD ms_timeout)
{
	DWORD dwIp = GetIPByName(hostName);
	if (dwIp == INADDR_NONE)
		return -1;
	return Ping(dwIp, ms_timeout);
}

#endif
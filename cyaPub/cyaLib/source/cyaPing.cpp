#include "cyaPing.h"

#if !defined(OS_IS_APPLE)

#include "cyaThread.h"
#include "cyaIpCvt.h"
#include "cyaMaxMin.h"

#define PACKET_SIZE		256
#define SEND_SIZE		56

#ifdef WIN32

#define ICMP_ECHO			8	//��������
#define ICMP_ECHOREPLY		0	//����Ӧ��

#define IS_SMALL_EDITION	1

struct icmp			//ICMPͷ�ṹ,ICMP���ı���װ��IP���ݱ��ڲ�����
{
	BYTE icmp_type;		//����
	BYTE icmp_code;		//����
	WORD icmp_cksum;	//У���
	WORD icmp_id;		//��־
	WORD icmp_seq;		//���
	DWORD icmp_data[1];	//��������
};

struct ip			//IPͷ�ṹ
{
#if IS_SMALL_EDITION
	BYTE ip_hl : 4;	//ip��ͷ����(ip_hl << 2)/*λ�򣬲��ܳ���1���ֽڣ������ķ��䣬�����ÿ����������4λ*/
	BYTE ip_v : 4;	//�汾
#else
	BYTE ip_v : 4;	//�汾
	BYTE ip_hl : 4;	//ip��ͷ����(ip_hl << 2)
#endif
	BYTE ip_tos;	//��������
	WORD ip_len;	//�ܳ���
	WORD ip_id;		//��־ 
	WORD ip_off;	//��Ƭƫ��
	BYTE ip_ttl;	//����ʱ��
	BYTE ip_p;		//Э��
	WORD ip_sum;	//����� 
	DWORD ip_src;	//ԴIP��ַ
	DWORD ip_dst;	//Ŀ��IP��ַ
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
		sum += *w++;/*��ȡ���ݣ�Ȼ��ָ��ָ����һ��WORD��Ԫ*/
		nLoop -= 2;
	}
	if (nLoop == 1)
	{
		*(BYTE*)(&answer) = *(BYTE*)w;
		sum += answer;
	}
	sum = (sum >> 16) + (sum & 0xFFFF);
	sum += (sum >> 16);
	answer = (WORD)~sum;/*sum��λȡ��*/
	return answer;
}

//���
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

	/*��¼����ʱ��*/
#if defined(__linux__)
	icmpData->icmp_mask = GetTickCount();
#else
	icmpData->icmp_data[0] = GetTickCount();
#endif
	icmpData->icmp_cksum = CalChksum__((WORD*)icmpData, packsize); /*У���㷨*/
	return packsize;
}

//���
static int UnpackIcmpData__(BYTE* buf, int len, WORD pid)
{
	struct ip* ipData;
	struct icmp* icmpData;
	int iphdrlen;
	if (len < SIZEOF(struct ip))
		return -1;
	ipData = (struct ip*)buf;
	iphdrlen = ipData->ip_hl << 2;    /*��ip��ͷ����,��ip��ͷ�ĳ��ȱ�־��4*/
	icmpData = (struct icmp*)(buf + iphdrlen);  /*Խ��ip��ͷ,ָ��ICMP��ͷ*/

	len -= iphdrlen;            /*ICMP��ͷ��ICMP���ݱ����ܳ���*/
	if (len < 8)                /*С��ICMP��ͷ�����򲻺���*/
		return -1;

	/*ȷ�������յ����������ĵ�ICMP�Ļ�Ӧ*/
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

//����
static int SendPacket__(SOCKET sock, DWORD ipAddr, WORD pid)
{
	static int pack_no = 0;
	BYTE sendPacket[PACKET_SIZE];
	memset(sendPacket, 0, sizeof(sendPacket));
	int nPacketSize = PackIcmpData__(pack_no++, pid, sendPacket); /*����ICMP��ͷ*/
	return nPacketSize == SockSendTo(sock, ipAddr, 0, sendPacket, nPacketSize) ? 0 : -1;
}

//����
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
		// �ж��Ƿ����Լ�Ping�Ļظ�
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
	/*����ʹ��ICMP��ԭʼ�׽���,��linuxϵͳ��,�����׽���ֻ����rootȨ���²�������*/
	m_sockfd = SockRawNew(GetICMPProtocol__());
	if (!SockValid(m_sockfd))
		return;

	SockSetNonBlock(m_sockfd, true);
	// 	SockSetReceiveTimeOut(m_sockfd, 1000);
	// 	SockSetSendTimeOut(m_sockfd, 1000);

	/*�����׽��ֽ��ջ�������64K��������ҪΪ�˼�С���ջ����������
	�Ŀ�����,��������pingһ���㲥��ַ��ಥ��ַ,������������Ӧ��*/
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
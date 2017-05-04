#include "cyaIfcfg.h"
#include "cyaLocker.h"
#include "cyaSock.h"
#include "cyaIpCvt.h"

#if defined(__linux__)
	#include <net/if.h>
#endif

#define MAX_NET_INTERFACE_COUNT	32
struct NetAddrInfo
{
	char name[260];
	BYTE macAddr[8];

	IP_MASK_GW netAddrs[MAX_NET_INTERFACE_COUNT]; // 前count个有效
	int count; // [0, MAX_SYS_IP_COUNT]

	NetAddrInfo()
	{
		memset(this, 0, sizeof(*this));
	}
};

typedef std::vector<NetAddrInfo> NetAddrInfoVec;
static NetAddrInfoVec sg_netAddrInfos;
static CXTLocker sg_ifcfgLocker;

#if defined(WIN32)
#if defined(VS_IS_VC6)
#define MAX_ADAPTER_DESCRIPTION_LENGTH  128 // arb.
#define MAX_ADAPTER_NAME_LENGTH         256 // arb.
#define MAX_ADAPTER_ADDRESS_LENGTH      8   // arb.

#define MAX_HOSTNAME_LEN                128 // arb.
#define MAX_DOMAIN_NAME_LEN             128 // arb.
#define MAX_SCOPE_ID_LEN                256 // arb.

typedef struct {
	char String[4 * 4];
} IP_ADDRESS_STRING, *PIP_ADDRESS_STRING, IP_MASK_STRING, *PIP_MASK_STRING;

typedef struct _IP_ADDR_STRING
{
	struct _IP_ADDR_STRING* Next;
	IP_ADDRESS_STRING IpAddress;
	IP_MASK_STRING IpMask;
	DWORD Context;
} IP_ADDR_STRING, *PIP_ADDR_STRING;

typedef struct _IP_ADAPTER_INFO {
	struct _IP_ADAPTER_INFO* Next;
	DWORD ComboIndex;
	char AdapterName[MAX_ADAPTER_NAME_LENGTH + 4];
	char Description[MAX_ADAPTER_DESCRIPTION_LENGTH + 4];
	UINT AddressLength;
	BYTE Address[MAX_ADAPTER_ADDRESS_LENGTH];
	DWORD Index;
	UINT Type;
	UINT DhcpEnabled;
	PIP_ADDR_STRING CurrentIpAddress;
	IP_ADDR_STRING IpAddressList;
	IP_ADDR_STRING GatewayList;
	IP_ADDR_STRING DhcpServer;
	BOOL HaveWins;
	IP_ADDR_STRING PrimaryWinsServer;
	IP_ADDR_STRING SecondaryWinsServer;
	time_t LeaseObtained;
	time_t LeaseExpires;
} IP_ADAPTER_INFO, *PIP_ADAPTER_INFO;

typedef struct {
	char HostName[MAX_HOSTNAME_LEN + 4];
	char DomainName[MAX_DOMAIN_NAME_LEN + 4];
	PIP_ADDR_STRING CurrentDnsServer;
	IP_ADDR_STRING DnsServerList;
	UINT NodeType;
	char ScopeId[MAX_SCOPE_ID_LEN + 4];
	UINT EnableRouting;
	UINT EnableProxy;
	UINT EnableDns;
} FIXED_INFO, *PFIXED_INFO;
#else
	#include <iptypes.h>
#endif
	typedef DWORD(CALLBACK * fnAPIGetAdaptersInfo)(PIP_ADAPTER_INFO, PULONG);//GetAdaptersInfo
#endif

static BOOL InitNetConfig__()
{
	if(sg_netAddrInfos.size() > 0)
		return true;

	CXTAutoLock lock(sg_ifcfgLocker);
	if(sg_netAddrInfos.size() > 0)
		return true;
#if defined(WIN32)
	// 获得IP Helper API
	static HMODULE hIphlpapiMod = NULL;
	if(NULL == hIphlpapiMod)
	{
		hIphlpapiMod = LoadLibrary(_T("iphlpapi.dll"));
		if(NULL == hIphlpapiMod)
			return false;
	}

	static fnAPIGetAdaptersInfo funGetAdaptersInfo = NULL;
	if(NULL == funGetAdaptersInfo)
	{
		funGetAdaptersInfo = (fnAPIGetAdaptersInfo)GetProcAddress(hIphlpapiMod, "GetAdaptersInfo");
		if(NULL == funGetAdaptersInfo)
			return false;
	}

		// 获得网卡数据
	ULONG ulSize = 0;
	funGetAdaptersInfo(NULL, &ulSize); // 第一次调用，获取缓冲区大小
	PIP_ADAPTER_INFO pInfo = (PIP_ADAPTER_INFO)malloc(ulSize);
	ASSERT(pInfo != NULL);
	funGetAdaptersInfo(pInfo, &ulSize);

	// 遍历每一张网卡
	while(pInfo)
	{
		NetAddrInfo netInfo;
		// 网卡名
		strcpy(netInfo.name, pInfo->AdapterName);
			// MAC地址
		memcpy(netInfo.macAddr, pInfo->Address, pInfo->AddressLength);

		// 取绑定于这张网卡之上所有IP
		PIP_ADDR_STRING pAddTemp = &(pInfo->IpAddressList);
		PIP_ADDR_STRING pGW = &(pInfo->GatewayList);
		while(pAddTemp)
		{
			if(MAX_NET_INTERFACE_COUNT == netInfo.count)
				break;
			DWORD dwIp = GetDWordIP(pAddTemp->IpAddress.String);
			DWORD dwMask = GetDWordIP(pAddTemp->IpMask.String);
			
			netInfo.netAddrs[netInfo.count].ip = dwIp;//GetDWordIP(pAddTemp->IpAddress.String);//SockGetIP(CtoTC_str(pAddTemp->IpAddress.String));
			netInfo.netAddrs[netInfo.count].mask = dwMask;//GetDWordIP(pAddTemp->IpMask.String);//SockGetIP(CtoTC_str(pAddTemp->IpMask.String));
			if(pGW)
			{
				netInfo.netAddrs[netInfo.count].gateway = GetDWordIP(pGW->IpAddress.String);
				pGW = pGW->Next;
			}
			else
				netInfo.netAddrs[netInfo.count].gateway = INADDR_ANY;

			++netInfo.count;
			pAddTemp = pAddTemp->Next;
		}
		if(netInfo.count > 0)
			sg_netAddrInfos.push_back(netInfo);

		// 将当前指针移向下一个
		pInfo = pInfo->Next;
	}
#else
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(fd <= 0)
		return false;
	struct ifreq buf[16];
	struct ifconf ifc;
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;
	if(0 == ioctl(fd, SIOCGIFCONF, (char*)(void*)&ifc))
	{
		int intrface = ifc.ifc_len/sizeof(struct ifreq);
		for(int i=0; i<intrface; ++i)
		{
			if(0 != ioctl(fd, SIOCGIFFLAGS, &buf[i]))
				break;
			if(strncmp(buf[i].ifr_name, "lo", 2)==0)
				continue;

			NetAddrInfo netInfo;
			// 网卡名
			strcpy(netInfo.name, buf[i].ifr_name);
			// 取MAC地址
			if(0 != ioctl(fd, SIOCGIFHWADDR, &buf[i]))
				break;
			memcpy(netInfo.macAddr, buf[i].ifr_hwaddr.sa_data, sizeof(netInfo.macAddr));

			// 取IP和mask
			if(0 != ioctl(fd, SIOCGIFADDR, &buf[i]))
				break;
			netInfo.netAddrs[netInfo.count].ip = (DWORD)((struct sockaddr_in*)(&buf[i].ifr_addr))->sin_addr.s_addr;
			if(0 != ioctl(fd, SIOCGIFNETMASK, &buf[i]))
				break;
			netInfo.netAddrs[netInfo.count].mask = (DWORD)((struct sockaddr_in*)(&buf[i].ifr_addr))->sin_addr.s_addr;
			++netInfo.count;
			sg_netAddrInfos.push_back(netInfo);
		}
	}
	FDClose(fd);
#endif
	return sg_netAddrInfos.size() > 0 ? true : false;
}

void IfCfgListInterface(std::vector<std::string>& ifs)
{
	if (!InitNetConfig__())
		return;
	for (int i = 0; i < (int)sg_netAddrInfos.size(); ++i)
		ifs.push_back(sg_netAddrInfos[i].name);
}

BOOL IfCfgGetMacAddrByName(const char* name, BYTE macAddr[8])
{
	if (!InitNetConfig__())
		return false;
	for (int i = 0; i < (int)sg_netAddrInfos.size(); ++i)
	{
		if (strcmp(name, sg_netAddrInfos[i].name) == 0)
		{
			memcpy(macAddr, sg_netAddrInfos[i].macAddr, sizeof(sg_netAddrInfos[i].macAddr));
			return true;
		}
	}
	return false;
}

BOOL IfCfgGetMacAddrByIp(DWORD ipAddr, BYTE macAddr[8])
{
	if (!InitNetConfig__())
		return false;

	if (INADDR_LOOPBACK_NET == ipAddr)
	{
		memcpy(macAddr, sg_netAddrInfos[0].macAddr, sizeof(sg_netAddrInfos[0].macAddr));
		return true;
	}

	for (int i = 0; i < (int)sg_netAddrInfos.size(); ++i)
	{
		NetAddrInfo& info = sg_netAddrInfos[i];
		for (int j = 0; j < info.count; ++j)
		{
			if (ipAddr == info.netAddrs[j].ip)
			{
				memcpy(macAddr, info.macAddr, sizeof(info.macAddr));
				return true;
			}
		}
	}
	return false;
}

//获取主机IP地址
int IfcfgGetHostIps(DWORD ips[32])
{
	int count = 0;
#if defined(WIN32)
	char name[256] = { 0 };
	int i = 0;
	struct hostent* hostinfo = NULL;
	if (gethostname(name, sizeof(name) - 1) == 0)
	{
		hostinfo = gethostbyname(name);
		if (hostinfo)
		{
			while (hostinfo->h_addr_list[i] != NULL)
			{
				ips[count] = *(DWORD*)(void*)hostinfo->h_addr_list[i];
				++count;
				if (count >= 32)
					break;
				++i;
			}
		}
	}
#elif defined(__linux__)
#if defined(OS_IS_APPLE)
	count = 0;
#else
	int fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (fd <= 0)
		return 0;

	struct ifreq buf[16];
	struct ifconf ifc;
	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;
	if (0 == ioctl(fd, SIOCGIFCONF, (char*)(void*)&ifc))
	{
		int intrface = ifc.ifc_len / sizeof(struct ifreq);
		for (int i = 0; i<intrface; ++i)
		{
			if (0 != ioctl(fd, SIOCGIFADDR, &buf[i]))
				continue;

			DWORD ip = (DWORD)((struct sockaddr_in*)(&buf[i].ifr_addr))->sin_addr.s_addr;
			if (ADDR_ANY == ip || INADDR_LOOPBACK_NET == ip || INADDR_NONE == ip)
				continue;

			ips[count] = ip;
			++count;
			if (count >= 32)
				break;
		}
	}
	FDClose(fd);
#endif
#endif
	return count;
}

DWORD IfcfgGetHostIp()
{
	DWORD ips[32];
	int nCount = IfcfgGetHostIps(ips);
	if (nCount > 0)
		return ips[0];
	return INADDR_NONE;
}

DWORD GetIPByName(const char* lpszName)
{
	return SockResolveIP(lpszName);
}

#if defined(OS_IS_APPLE)
BOOL GetHostMacAddr(BYTE macAddr[8])
{
	int mib[6];
	size_t len = 0;
	char* buf = NULL;
	unsigned char* ptr = NULL;
	struct if_msghdr* ifm = NULL;
	struct sockaddr_dl* sdl = NULL;
	mib[0] = CTL_NET;
	mib[1] = AF_ROUTE;
	mib[2] = 0;
	mib[3] = AF_LINK;
	mib[4] = NET_RT_IFLIST;

	if ((mib[5] = if_nametoindex("en0")) == 0)
		return false;
	if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0)
		return false;

	buf = (char*)malloc(len);
	if (sysctl(mib, 6, buf, &len, NULL, 0) < 0)
	{
		free(buf);
		return false;
	}

	ifm = (struct if_msghdr*)buf;
	sdl = (struct sockaddr_dl*)(ifm + 1);
	ptr = (unsigned char*)LLADDR(sdl);
	memcpy(macAddr, ptr, 6);
	free(buf);
	return true;
}
#endif

void IfCfgReset()
{
	CXTAutoLock lock(sg_ifcfgLocker);
	sg_netAddrInfos.clear();
}
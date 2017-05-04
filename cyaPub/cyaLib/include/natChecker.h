#ifndef __NAT_CHECKER_H__
#define __NAT_CHECKER_H__

/*
基于stun协议,检测NAT类型/获取NAT映射地址
author:hhy
date:2016-10-24
*/

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaIpCvt.h"
#include "cyaSock.h"

class NatChecker
{
public:
	NatChecker(const char* ipORdomain = "stunserver.org", int port = 3478, DWORD localIp = INADDR_ANY, DWORD timeout = 5 * 1000);
	~NatChecker();

public:
	enum NatType
	{
		ERROR_DETECTING_NAT = -1,
		OPEN_INTERNET,
		FIREWALL_BLOCKS_UDP,
		//SYMMETRIC_UDP_FIREWALL,
		FULL_CONE_NAT,
		SYMMETRIC_NAT,
		RESTRICTED_CONE_NAT,
		RESTRICTED_PORT_CONE_NAT,
		BEHIND_NAT
	};

	enum StunMsgType
	{
		BINDING_REQUEST = 0x0001,
		BINDING_RESPONSE = 0x0101,
		BINDING_ERROR_RESPONSE = 0x0111,
		SHARED_SECRET_REQUEST = 0x0002,
		SHARED_SECRET_RESPONSE = 0x0102,
		SHARED_SECRET_ERROR_RESPONSE = 0x0112
	};

	enum StunAttributeType
	{
		MAPPED_ADDRESS = 0x0001,
		RESPONSE_ADDRESS = 0x0002,
		CHANGE_REQUEST = 0x0003,
		SOURCE_ADDRESS = 0x0004,
		CHANGED_ADDRESS = 0x0005,
		USERNAME = 0x0006,
		PASSWORD = 0x0007,
		MESSAGE_INTEGRITY = 0x0008,
		ERROR_CODE = 0x0009,
		UNKNOWN_ATTRIBUTES = 0x000a,
		REFLECTED_FROM = 0x000b
	};

#pragma pack(push, 1)
	struct stun_mapped_address
	{
		WORD attrType;
		WORD attrLen;
		BYTE familyReserve;
		BYTE family;
		WORD mapped_port;
		BYTE mapped_addr[];
	};
	struct stun_source_address
	{
		WORD attrType;
		WORD attrLen;
		BYTE familyReserve;
		BYTE family;
		WORD source_port;
		BYTE source_addr[];
	};
	struct stun_changed_address
	{
		WORD attrType;
		WORD attrLen;
		BYTE familyReserve;
		BYTE family;
		WORD changed_port;
		BYTE changed_addr[];
	};
	struct stun_message_header
	{
		WORD msgType;
		WORD msgLen;
		BYTE transaction[16];
	};
	struct stun_message
	{
		stun_message_header*	stun_header;
		stun_mapped_address*	mapped_addr;
		stun_source_address*	source_addr;
		stun_changed_address*	changed_addr;
	};
#pragma pack(pop)

	NatType GetNatType();
	const char* GetStringNatType(NatType nt);
	int GetStunMappedAddress(SOCKET udpSock, DWORD* mappedIp, int* mappedPort, int* family);

private:
	SOCKET OnCreateUDPSocket(int* localPort);

	void OnGenerateTransactionId(BYTE id[16]);
	void OnCreateStunMsgHeader(BYTE p[20], WORD msgType, WORD msgLen);
	int OnParseStunMsg(const BYTE* data, int dataLen, stun_message* msg);

	int OnSendStunBindRequest(SOCKET sock, DWORD serverIp, int serverPort, BYTE* res, int nResBytes);
	int OnSendStunBindChangeRequest(SOCKET sock, DWORD serverIp, int serverPort, UINT32 changeFlag, BYTE* res, int nResBytes);

private:
	DWORD	m_localIp;
	DWORD	m_timeout;
	BOOL	m_isValid;
	int		m_stunServerPort;
	std::string m_stunServerIp;
};
#endif
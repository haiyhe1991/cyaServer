#ifndef __STUN_SERVE_H__
#define __STUN_SERVE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaThread.h"

class StunServe
{
private:
	StunServe(const StunServe&);
	StunServe& operator = (const StunServe&);

public:
	StunServe();
	~StunServe();

	BOOL Start(const char* stunIp, int stunPort, const char* stunIp2, int stunPort2);
	void Stop();

private:
	static int THWorker(void* param);
	int  OnWorker();
	void OnRead__(SOCKET sock);

private:
	OSThread m_thHandle;
	BOOL	m_exitThread;
	DWORD	m_stunIp;
	int		m_stunPort;
	DWORD	m_stunIp2;
	int		m_stunPort2;

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
	struct stun_change_request
	{
		WORD attrType;
		WORD attrLen;
		BYTE change_attr[];
	};
	struct stun_message_integrity
	{
		WORD attrType;
		WORD attrLen;
		BYTE hmac[20];
	};
	struct stun_attr_username
	{
		WORD attrType;
		WORD attrLen;
		BYTE username[];
	};
	struct sturn_error_code
	{
		WORD attrType;
		WORD attrLen;
		UINT32 reserved_class : 24; /**< 21 bit reserved (value = 0) and 3
									bit which indicates hundred digits
									of the response code (3 - 6) */
		UINT32 number : 8; /**< Number (0 - 99) */
		BYTE reason[]; /**< Variable-size reason */
	};
	struct stun_message_header
	{
		WORD msgType;
		WORD msgLen;
		BYTE transaction[16];
	};
	struct stun_message
	{
		stun_message_header* stun_header;
		stun_mapped_address* mapped_addr;
		stun_source_address* source_addr;
		stun_changed_address* changed_addr;
		stun_change_request* change_request;
		stun_message_integrity* msg_integrity;
		stun_attr_username* attr_username;
		sturn_error_code* err_code;
	};
#pragma pack(pop)

private:
	int OnParseStunMessage(const char* data, int dataLen, stun_message* msg);
	void OnCreateStunMsgHeader(BYTE p[20], WORD msgType, WORD msgLen, BYTE transaction[16]);
	int OnAddAddressAttr(BYTE* buf, WORD attrType, WORD attrLen, WORD family, WORD port, DWORD ip);
	void OnProcessStunMessage(const stun_message& msg, SOCKET sock, DWORD peerIp, int peerPort);
};

#endif
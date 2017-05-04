#ifndef __CYA_PING_H__
#define __CYA_PING_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaLocker.h"
#include "cyaSock.h"

#if !defined(OS_IS_APPLE)

class CPingNoLock
{
public:
	CPingNoLock();
	~CPingNoLock();

	int Ping(DWORD ipAddr, DWORD milliseconds_timeout = 5000);
	int Ping(const char* hostName, DWORD milliseconds_timeout = 5000);

private:
	CPingNoLock(const CPingNoLock&);
	CPingNoLock& operator=(const CPingNoLock&);

private:
	SOCKET m_sockfd;
};

class CPing : public CPingNoLock
{
public:
	int Ping(DWORD ipAddr, DWORD ms_timeout = 5000)
	{
		CXTAutoLock autoLock(m_lock);
		return CPingNoLock::Ping(ipAddr, ms_timeout);
	}

	int Ping(const char* hostName, DWORD ms_timeout = 5000)
	{
		CXTAutoLock autoLock(m_lock);
		return CPingNoLock::Ping(hostName, ms_timeout);
	}

private:
	CXTLocker m_lock;
};

inline BOOL Ping(DWORD ip, DWORD ms_timeout = 5000)
{
	CPingNoLock ping;
	return 0 == ping.Ping(ip, ms_timeout);
}

inline BOOL Ping(const char* host, DWORD ms_timeout = 5000)
{
	CPingNoLock ping;
	return 0 == ping.Ping(host, ms_timeout);
}

#endif  //OS_IS_APPLE

#endif
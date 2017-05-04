#ifndef __TRANSPORTER_H__
#define __TRANSPORTER_H__

#include "cyaRefCount.h"

class Transporter : public BaseRefCount1
{
public:
	Transporter(void* theSession);
	~Transporter();

	int Transport(const char* cache, int len, bool last);

private:
	int CleanCache(bool Last);
	int SendData(const char* cacheData , int cacheLen , bool Last);
private:
	void* m_session;
	char* m_cache;
	size_t m_len;
};

#endif
#ifndef __GWS_CLIENT_MAN_H__
#define __GWS_CLIENT_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
网关连接管理
*/

#include <vector>
#include "GWSClient.h"

class GWSClientMan
{
public:
	GWSClientMan();
	~GWSClientMan();

	int Init();
	void Cleanup();

private:
	std::vector<GWSClient*> m_gwsClientVec;
};

int InitGWSClientMan();
void DestroyGWSClientMan();


#endif
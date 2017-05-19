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
#include "cyaCoreTimer.h"

class GWSClientMan
{
	GWSClientMan(const GWSClientMan&);
	GWSClientMan& operator = (const GWSClientMan&);

public:
	GWSClientMan();
	~GWSClientMan();

	int Init();
	void Cleanup();

	GWSClient* RandGWSClient();

	GWSClient* GetFristGWSClient();

private:
	std::vector<GWSClient*> m_gwsClientVec;
};

int InitGWSClientMan();
GWSClientMan* GetGWSClientMan();
void DestroyGWSClientMan();

#endif	//_GWSClientMan_h__
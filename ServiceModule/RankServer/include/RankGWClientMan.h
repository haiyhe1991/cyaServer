#ifndef __RANK_GW_CLIENT_MAN_H__
#define __RANK_GW_CLIENT_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
网关连接管理
*/

#include <vector>
#include "RankGWClient.h"

class RankGWClientMan
{
public:
	RankGWClientMan();
	~RankGWClientMan();

	int Init();
	void Cleanup();

private:
	std::vector<RankGWClient*> m_gwsClientVec;
};

int InitRankGWClientMan();
void DestroyRankGWClientMan();

#endif
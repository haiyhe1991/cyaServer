#ifndef __MAU_GWS_CLIENT_MAN_H__
#define __MAU_GWS_CLIENT_MAN_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
网关连接管理
*/

#include <vector>
#include "MauGWSClient.h"

class MauGWSClientMan
{
public:
	MauGWSClientMan();
	~MauGWSClientMan();

	int Init();
	void Cleanup();

private:
	std::vector<MauGWSClient*> m_gwsClientVec;
};

int InitGWSClientMan();
void DestroyGWSClientMan();

#endif
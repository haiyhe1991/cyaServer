#ifndef __SIGN_IN_CFG_H__
#define __SIGN_IN_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
ǩ������
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class SignInCfg
{
public:
	SignInCfg();
	~SignInCfg();

#pragma pack(push, 1)
	struct SSignInItem
	{
		std::vector<SDropPropCfgItem> rewardVec;
	};
#pragma pack(pop)

public:
	//���������ļ�
	BOOL Load(const char* filename);
	//��ȡǩ������
	int GetSignInItem(BYTE nTimes, SSignInItem*& pItem);

private:
	std::map<BYTE/*����ǩ������*/, SSignInItem> m_signInMap;
};

#endif
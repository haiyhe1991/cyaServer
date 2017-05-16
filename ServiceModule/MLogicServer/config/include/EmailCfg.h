#ifndef __EMAIL_CFG_H__
#define __EMAIL_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <map>
#include <vector>
#include "CfgCom.h"

class EmailCfg
{
public:
	EmailCfg();
	~EmailCfg();

#pragma pack(push, 1)
	struct SNewPlayerEmailItem
	{
		UINT32 gold;
		UINT32 rpcoin;
		UINT32 exp;
		UINT32 vit;
		std::string strTitle;
		std::string strBody;
		std::vector<SDropPropCfgItem> props;
	};
#pragma pack(pop)

public:
	//载入配置
	BOOL Load(const char* filename);
	//获取邮件赠送
	int GetNewPlayerEmailGiveProps(std::vector<SNewPlayerEmailItem>& giveVec);

private:
	std::string m_filename;
};


#endif
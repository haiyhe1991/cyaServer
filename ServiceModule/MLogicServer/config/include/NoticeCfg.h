#ifndef __NOTICE_CFG_H__
#define __NOTICE_CFG_H__

/*
公告配置
*/

#include "CfgCom.h"

class NoticeCfg
{
public:
	NoticeCfg();
	~NoticeCfg();

#pragma pack(push, 1)
	struct SNoticeItem
	{
		UINT16 id;
		UINT32 interval;
		std::string strContent;
	};
#pragma pack(pop)

public:
	//载入配置
	BOOL Load(const char* filename);
	//获取公告
	BOOL GetNoticeItems(std::vector<SNoticeItem>& noticeVec, UINT32& timeInterval);
	//获取时间间隔
	UINT32 GetNoticeInterval();

private:
	std::string m_filename;
};


#endif
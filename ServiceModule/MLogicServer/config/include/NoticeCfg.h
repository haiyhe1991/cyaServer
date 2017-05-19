#ifndef __NOTICE_CFG_H__
#define __NOTICE_CFG_H__

/*
��������
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
	//��������
	BOOL Load(const char* filename);
	//��ȡ����
	BOOL GetNoticeItems(std::vector<SNoticeItem>& noticeVec, UINT32& timeInterval);
	//��ȡʱ����
	UINT32 GetNoticeInterval();

private:
	std::string m_filename;
};


#endif
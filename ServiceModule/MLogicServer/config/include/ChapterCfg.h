#ifndef __CHAPTER_CFG_H__
#define __CHAPTER_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
章节配置
*/

#include <map>
#include <vector>
#include "CfgCom.h"

class ChapterCfg
{
public:
	ChapterCfg();
	~ChapterCfg();

#pragma pack(push, 1)
	struct SChapterItem
	{
		UINT16 nextChapterId;			//下一个章节id
		std::vector<UINT16> mainTasks;	//章节主线任务
	};
#pragma pack(pop)

public:
	//载入章节配置文件
	BOOL Load(const char* filename);
	//添加章节主线任务id
	void AddChapterMainTask(UINT16 chapterId, UINT16 taskId);
	//获取章节数据
	int GetChapterItem(UINT16 chapterId, SChapterItem*& pChapterItem);
	//获取首次开放的章节
	UINT16 GetFirstOpenChapter();

private:
	std::map<UINT16, SChapterItem> m_chapterMap;
};


#endif
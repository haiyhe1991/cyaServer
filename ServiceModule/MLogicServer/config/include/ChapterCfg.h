#ifndef __CHAPTER_CFG_H__
#define __CHAPTER_CFG_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
�½�����
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
		UINT16 nextChapterId;			//��һ���½�id
		std::vector<UINT16> mainTasks;	//�½���������
	};
#pragma pack(pop)

public:
	//�����½������ļ�
	BOOL Load(const char* filename);
	//����½���������id
	void AddChapterMainTask(UINT16 chapterId, UINT16 taskId);
	//��ȡ�½�����
	int GetChapterItem(UINT16 chapterId, SChapterItem*& pChapterItem);
	//��ȡ�״ο��ŵ��½�
	UINT16 GetFirstOpenChapter();

private:
	std::map<UINT16, SChapterItem> m_chapterMap;
};


#endif
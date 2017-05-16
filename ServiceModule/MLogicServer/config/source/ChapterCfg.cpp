#include "ChapterCfg.h"
#include "ServiceErrorCode.h"

ChapterCfg::ChapterCfg()
{

}

ChapterCfg::~ChapterCfg()
{

}

BOOL ChapterCfg::Load(const char* filename)
{
	Json::Value rootValue;
	if (!OpenCfgFile(filename, rootValue))
		return false;

	if (!rootValue.isMember("Chapter") || !rootValue["Chapter"].isArray())
		return false;

	SChapterItem item;
	int si = rootValue["Chapter"].size();
	for (int i = 0; i < si; ++i)
	{
		UINT16 id = (UINT16)rootValue["Chapter"][i]["ID"].asUInt();
		std::map<UINT16, SChapterItem>::iterator it = m_chapterMap.find(id);
		if (it != m_chapterMap.end())
		{
			ASSERT(false);

#if defined(WIN32)
			throw(id);
#endif

			continue;
		}
		SChapterItem& itemRef = m_chapterMap[id];
		itemRef.nextChapterId = rootValue["Chapter"][i]["Next"].asUInt();
	}
	return true;
}

void ChapterCfg::AddChapterMainTask(UINT16 chapterId, UINT16 taskId)
{
	std::map<UINT16, SChapterItem>::iterator it = m_chapterMap.find(chapterId);
	if (it == m_chapterMap.end())
	{
		ASSERT(false);
		return;
	}

	it->second.mainTasks.push_back(taskId);
}

int ChapterCfg::GetChapterItem(UINT16 chapterId, SChapterItem*& pChapterItem)
{
	std::map<UINT16, SChapterItem>::iterator it = m_chapterMap.find(chapterId);
	if (it == m_chapterMap.end())
		return MLS_NOT_EXIST_CHAPTER;

	pChapterItem = &it->second;
	return MLS_OK;
}

UINT16 ChapterCfg::GetFirstOpenChapter()
{
	if (m_chapterMap.empty())
		return 0;

	return m_chapterMap.begin()->first;
}
#include "RandWords.h"

const char* chineseSurname[] = { "ÕÔ", "Ç®", "Ëï", "Àî", "ÖÜ", "Îâ", "Ö£", "Íõ", "·ë", "³Â", "ñÒ", "ÎÀ", "½¯", "Éò", "º«", "Ñî", "Öì", "ÇØ", "ÓÈ",
"ºÎ", "ÂÀ", "Ê©", "ÕÅ", "¿×", "²Ü", "ÑÏ", "»ª", "½ð", "Îº", "ÌÕ", "½ª", "ÆÝ", "Ð»", "×Þ", "Ó÷", "°Ø", "Ë®", "ñ¼", "ÕÂ", "ÔÆ", "ËÕ", "ÅË", "á¯", "Ñ¦", "À×",
"¸ð", "ÞÉ", "·¶", "Åí", "ÀÉ", "Â³", "Î¤", "²ý", "Âí", "Ãç", "·ï", "»¨", "·½", "Óá", "ÈÎ", "Ô¬", "Áø", "Ûº", "±«", "Ê·", "ÌÆ", "·Ñ", "Á®", "ºØ", "Äß", "ÌÀ",
"ÂÞ", "±Ï", "ºÂ", "Úù", "°²", "³£", "ÀÖ", "ÓÚ", "Ê±", "¸µ", "Æ¤", "±å", "Æë", "¿µ", "Îé", "Óà", "Ôª", "²·", "¹Ë", "ÃÏ", "Æ½", "»Æ", "ºÍ", "ëø", "Òó", "Ðí",
"ÄÂ", "Ïô", "Òü", "Ò¦", "ÉÛ", "Õ¿", "Íô", "Æî", "Ã«", "Óí", "µÒ", "Ã×", "±´", "Ã÷", "ê°", "¼Æ", "·ü", "³É", "´÷", "Ì¸", "ËÎ", "Ã©", "ÅÓ", "ÐÜ", "¼Í", "Êæ",
"Çü", "Ïî", "×£", "¶­", "Áº", "¶Å", "Èî", "À¶", "ãÉ", "Ï¯", "¼¾", "Âé", "Ç¿", "¼Ö", "Â·", "Â¦", "Î£", "½­", "Í¯", "ÑÕ", "¹ù", "Ã·", "Ê¢", "ÁÖ", "µó", "ÖÓ",
"Ðì", "Çñ", "Âæ", "¸ß", "ÏÄ", "²Ì", "Ìï", "·®", "ºú", "Áè", "»ô", "ÓÝ", "Íò", "Ö§", "¿Â", "êÃ", "¹Ü", "Â¬", "Äª", "¾­", "·¿", "ôÃ", "çÑ", "¸É", "½â", "Ó¦",
"×Ú", "¶¡", "Ðû", "êÚ", "µË", "Óô", "µ¥", "º¼", "ºé", "°ü", "Öî", "×ó", "Ê¯", "´Þ", "¼ª", "Å¥", "¹¨", "³Ì", "ïú", "ÐÏ", "»¬", "Åá", "Â½", "ÈÙ", "ÎÌ", "Ü÷",
"Ñò", "ÓÚ", "»Ý", "Õç", "Çú", "¼Ò", "·â", "ÜÇ", "ôà", "´¢", "½ù", "¼³", "Úû", "ÃÓ", "ËÉ", "¾®", "¶Î", "¸»", "Î×", "ÎÚ", "½¹", "°Í", "¹­", "ÄÁ", "Úó", "É½",
"¹È", "³µ", "ºî", "åµ", "Åî", "È«", "Û­", "°à", "Ñö", "Çï", "ÖÙ", "ÒÁ", "¹¬", "Äþ", "³ð", "èï", "±©", "¸Ê", "î×", "À÷", "ÈÖ", "×æ", "Îä", "·û", "Áõ", "¾°",
"Õ²", "Êø", "Áú", "Ò¶", "ÐÒ", "Ë¾", "ÉØ", "Û¬", "Àè", "¼»", "äß", "Ó¡", "ËÞ", "°×", "»³", "ÆÑ", "Û¢", "´Ó", "¶õ", "Ë÷", "ÏÌ", "¼®", "Àµ", "×¿", "Ýþ", "ÍÀ",
"ÃÉ", "³Ø", "ÇÇ", "Òõ", "Óô", "ñã", "ÄÜ", "²Ô", "Ë«", "ÎÅ", "Ý·", "µ³", "µÔ", "Ì·", "¹±", "ÀÍ", "åÌ", "¼§", "Éê", "·ö", "¶Â", "È½", "Ô×", "Ûª", "Óº", "È´",
"è³", "É£", "¹ð", "å§", "Å£", "ÊÙ", "Í¨", "±ß", "ìè", "Ñà", "¼½", "ÆÖ", "ÉÐ", "Å©", "ÎÂ", "±ð", "×¯", "êÌ", "²ñ", "öÄ", "ÑÖ", "³ä", "Ä½", "Á¬", "Èã", "Ï°",
"»Â", "°¬", "Óã", "ÈÝ", "Ïò", "¹Å", "Ò×", "É÷", "¸ê", "ÁÎ", "â×", "ÖÕ", "ôß", "¾Ó", "ºâ", "²½", "¶¼", "¹¢", "Âú", "ºë", "¿ï", "¹ú", "ÎÄ", "¿Ü", "¹ã", "Â»",
"ãÚ", "¶«", "Å·", "ì¯", "ÎÖ", "Àû", "Îµ", "Ô½", "Ùç", "Â¡", "Ê¦", "¹®", "ØÇ", "Äô", "êË", "¹´", "°½", "ÈÚ", "Àä", "ö¤", "ÐÁ", "ãÛ", "ÄÇ", "¼ò", "ÈÄ", "¿Õ",
"Ôø", "Îã", "É³", "Ø¿", "Ñø", "¾Ï", "Ðë", "·á", "³²", "¹Ø", "Øá", "Ïà", "²é", "ºó", "¾£", "ºì", "ÓÎ", "Û£", "óÃ", "È¨", "åÖ", "¸Ç", "Òæ", "»¸", "¹«", "Øë",
"¶½", "ÔÀ", "Ë§", "çÃ", "¿º", "¿ö", "àC", "ÓÐ", "ÇÙ", "¹é", "º£", "½ú", "³þ", "ãÆ", "·¨", "Èê", "Û³", "Í¿", "ÇÕ", "ÉÌ", "Ä²", "ÙÜ", "Ù¦", "²®", "ÉÍ", "Ä«",
"¹þ", "ÚÛ", "óò", "Äê", "°®", "Ñô", "Ù¡", "ÑÔ", "¸£", "ÄÏ", "»ð", "Ìú", "³Ù", "Æá", "¹Ù", "Ùþ", "Õæ", "Õ¹", "·±", "Ì´", "¼À", "ÃÜ", "¾´", "½Ò", "Ë´", "Â¥",
"Êè", "Ã°", "»ë", "Ö¿", "½º", "Ëæ", "¸ß", "¸Þ", "Ô­", "ÖÖ", "Á·", "ÃÖ", "²Ö", "íõ", "å¿", "ñû", "°¢", "ÃÅ", "ã¢", "À´", "ôë", "ÕÙ", "ÒÇ", "·ç", "½é", "¾Þ",
"Ä¾", "¾©", "ºü", "Û¨", "»¢", "Ã¶", "¿¹", "´ï", "è½", "ÜÉ", "ÕÛ", "Âó", "Çì", "¹ý", "Öñ", "¶Ë", "ÏÊ", "»Ê", "ØÁ", "ÀÏ", "ÊÇ", "ÃØ", "³©", "Ú÷", "»¹", "±ö",
"ãÌ", "¹¼", "×Ý", "‚G", "ÍòÙ¹", "Ë¾Âí", "ÉÏ¹Ù", "Å·Ñô", "ÏÄºî", "Öî¸ð", "ÎÅÈË", "¶«·½", "ºÕÁ¬", "»Ê¸¦", "ÑòÉà", "Î¾³Ù", "¹«Ñò", "å£Ì¨", "¹«Ò±", "×ÚÕý",
"å§Ñô", "´¾ÓÚ", "µ¥ÓÚ", "Ì«Êå", "ÉêÍÀ", "¹«Ëï", "ÖÙËï", "ÐùÔ¯", "Áîºü", "ÖÓÀë", "ÓîÎÄ", "³¤Ëï", "Ä½ÈÝ", "ÏÊÓÚ", "ãÌÇð", "Ë¾Í½", "Ë¾¿Õ", "Ø£¹Ù", "Ë¾¿Ü",
"ÄÏÃÅ", "ºôÑÓ", "×Ó³µ", "ò§Ëï", "¶ËÄ¾", "Î×Âí", "¹«Î÷", "Æáµñ", "³µÕý", "ÈÀæá", "¹«Á¼", "ÍØ°Ï", "¼Ð¹È", "Ô×¸¸", "¹ÈÁº", "¶Î¸É", "°ÙÀï", "¶«¹ù", "Î¢Éú",
"ÁºÇð", "×óÇð", "¶«ÃÅ", "Î÷ÃÅ", "ÄÏ¹¬", "µÚÎå", "¹«ÒÇ", "¹«³Ë", "Ì«Ê·", "ÖÙ³¤", "ÊåËï", "ÇüÍ»", "¶ûÖì", "¶«Ïç", "ÏàÀï", "ºúÄ¸", "Ë¾³Ç", "ÕÅÁÎ", "ÓºÃÅ",
"ÎãÇð", "ºØÀ¼", "ôëÎã", "ÎÝÂ®", "¶À¹Â", "ÄÏ¹ù", "±±¹¬", "ÍõËï"
};

CRandWords *prandWordsObj = NULL;

CRandWords::CRandWords() :m_pcsMap(NULL)
{
	m_pcsMap = new ChineseSurname;
	srand((unsigned)time(NULL));
	AddChineseSurname();
}

CRandWords::~CRandWords()
{
	if (m_pcsMap != NULL)
	{
		m_pcsMap->clear();
		delete m_pcsMap;
		m_pcsMap = NULL;
	}
}

void CRandWords::AddChineseSurname()
{
	if (m_pcsMap == NULL)
		return;

	int count = 598;
	ChineseSurname::iterator it;
	for (int i = 0; i < count; i++)
	{
		if (m_pcsMap->size() == 0)
		{
			m_pcsMap->insert(std::pair<const char*, const char*>(chineseSurname[i], chineseSurname[i]));
		}
		else
		{
			it = m_pcsMap->find(chineseSurname[i]);
			if (it != m_pcsMap->end())
				continue;
			else
				m_pcsMap->insert(std::pair<const char*, const char*>(chineseSurname[i], chineseSurname[i]));
		}
	}
}

bool CRandWords::SingleWordMatch(const char* sourceStr, char target[3])
{
	if (sourceStr == NULL)
		return false;

	int num = strlen(sourceStr) / 2;

	for (int i = 0; i < num; i++)
	{

		if ((char*)sourceStr[i] == target)
		{
			return true;
		}
	}
	return false;
}

bool CRandWords::RandChineseWordsMan(unsigned int chineseNum, std::string &outName)
{


	int count = 598;
	std::string randChinWords;
	int num = chineseNum;

	int high = 0xd7 - 0xc1; //16-55Çøºº×Ó
	int low = 0xfe - 0xa1;
	int high_zn = 0;
	int low_zn = 0;

	int desc = rand() % count;
	randChinWords += chineseSurname[desc];

	char name[3];
	name[2] = '\0';
	num--;

	while (num  > 0)
	{
		high_zn = rand() % high + 0xc1;
		low_zn = rand() % low + 0xa1;
		name[0] = high_zn;
		name[1] = low_zn;

		if (SingleWordMatch(randChinWords.c_str(), name))
			continue;
		else
			randChinWords += name;

		num--;
	}
	outName = randChinWords;
	return true;
}

int CRandWords::RandNumberMan(unsigned int wordsNum, std::string &outNum)
{
	int retRes = 0;
	unsigned int count = wordsNum;
	std::string resStr;
	int localNum;
	while (count > 0)
	{
		char szBuf[4] = { 0 };
		localNum = rand() % 10;
		sprintf_s(szBuf, "%d", localNum);
		resStr += szBuf;
		count--;
	}

	if (strcmp(resStr.c_str(), "") == 0)
		return 1;

	outNum = resStr;
	return 0;
}

int CRandWords::RandNumberMan()
{
	return (rand() % 10);
}

int CRandWords::RandWordsMan(unsigned int wordsNum, std::string &outName)
{
	int retRes = 0;
	unsigned int count = wordsNum;
	std::string resStr;

	while (count > 0)
	{
		char word = rand() % 128;

		if (word >= 'a' && word <= 'z')
		{
			resStr += word;
			count--;
		}
		else if (word >= 'A' && word <= 'Z')
		{
			resStr += word;
			count--;
		}
		else
			continue;
	}
	if (strcmp(resStr.c_str(), "") == 0 || resStr.length() == 0)
		return 1;

	outName = resStr;
	return retRes;

}

int CRandWords::RandWordsMan(unsigned int wordsNum, char &outName)
{
	int retRes = 0;
	unsigned int count = wordsNum;
	char word;
	while (wordsNum > 0)
	{
		word = rand() % 128;

		if ((word >= 'a' && word <= 'z') || (word >= 'A' && word <= 'Z'))
			wordsNum--;
		else
			continue;
	}
	outName = word;
	return retRes;

}

int CRandWords::RandNumbersWordsMan(unsigned int wordsNum, std::string &outName)
{
	int retRes = 0;
	unsigned int count = wordsNum;
	unsigned int redcount = wordsNum;
	std::string resStr;

	while (redcount > 0)
	{
		char szbuf[4] = { 0 };
		char word;
		int tmp;
		if (redcount % 2 == 0)
		{
			retRes = RandWordsMan(1, word);
			resStr += word;
			redcount--;
		}
		else
		{
			tmp = RandNumberMan();
			sprintf_s(szbuf, "%d", tmp);
			resStr += szbuf;
			redcount--;
		}

	}
	if (strcmp(resStr.c_str(), "") == 0 || resStr.length() == 0)
		return 1;

	outName = resStr;
	return retRes;

}

int CRandWords::GetRandEnglshName(unsigned int englishNum, std::string &outName)
{
	std::string retRes;
	int res = RandWordsMan(englishNum, outName);
	if (res != 0)
	{
		return 1;
	}

	return 0;
}

int CRandWords::GetRandChineseName(unsigned int chineseNum, std::string &outName)
{
	std::string retRes;

	int res = RandChineseWordsMan(chineseNum, retRes);
	if (res != 1)
	{
		return 1;
	}
	outName = retRes;
	return 0;
}

int CRandWords::GetRandNumbers(unsigned int numberNum, std::string &outName)
{
	std::string retRes;

	int res = RandNumberMan(numberNum, retRes);
	if (res != 0)
	{
		return 1;
	}
	outName = retRes;
	return 0;
}

int CRandWords::GetRandCEConsistName(unsigned int chineseNum, unsigned int englishNum, std::string &outName)
{
	std::string enStr;
	std::string cnStr;
	std::string concatStr;

	srand((unsigned int)time(NULL));
	int coun = rand();

	int res = RandWordsMan(englishNum, enStr);
	if (res != 0)
		return 1;

	res = RandChineseWordsMan(chineseNum, cnStr);
	if (res != 1)
		return 1;
	if (coun % 2 == 0)
	{
		concatStr += enStr;
		concatStr += cnStr;
	}
	else
	{
		concatStr += cnStr;
		concatStr += enStr;
	}

	outName = concatStr;
	return 0;
}

int CRandWords::GetRandCENConsistName(unsigned int chineseNum, unsigned int englishNum, unsigned int numberNum, std::string &outName)
{
	std::string enStr;
	std::string cnStr;
	std::string numStr;
	std::string concatStr;
	int coun = rand();

	int res = RandWordsMan(englishNum, enStr);
	if (res != 0)
		return 1;

	res = RandChineseWordsMan(chineseNum, cnStr);
	if (res != 1)
		return 1;
	res = RandNumberMan(numberNum, numStr);
	if (res != 0)
		return 1;

	if (coun % 2 == 0)
	{
		concatStr += enStr;
		concatStr += cnStr;
		concatStr += numStr;
	}
	else
	{
		concatStr += cnStr;
		concatStr += enStr;
		concatStr += numStr;
	}

	outName = concatStr;
	return 0;
}

int CRandWords::GetRandNumbersPasswd(unsigned int numberNum, std::string &outName)
{
	std::string retRes;

	int res = RandNumberMan(numberNum, retRes);
	if (res != 0)
	{
		return 1;
	}
	outName = retRes;
	return 0;
}

int CRandWords::GetRandENConsistPasswd(unsigned int numberNum, std::string &outName)
{
	std::string retRes;

	int res = RandNumbersWordsMan(numberNum, retRes);
	if (res != 0)
	{
		return 1;
	}
	outName = retRes;
	return 0;
}

void InitRandWordsObj()
{
	if (prandWordsObj != NULL)
		return;

	prandWordsObj = new CRandWords();

	if (prandWordsObj == NULL)
		printf("InitRandWordsObj failed!\n");
}


CRandWords *GetRandWordsObj()
{
	return prandWordsObj;
}


void DestroyRandWordsObj()
{
	if (prandWordsObj == NULL)
		return;

	if (prandWordsObj != NULL)
	{
		delete prandWordsObj;
		prandWordsObj = NULL;
	}
}
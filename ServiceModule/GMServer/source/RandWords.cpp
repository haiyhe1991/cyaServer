#include "RandWords.h"

const char* chineseSurname[] = { "��", "Ǯ", "��", "��", "��", "��", "֣", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��",
"��", "��", "ʩ", "��", "��", "��", "��", "��", "��", "κ", "��", "��", "��", "л", "��", "��", "��", "ˮ", "�", "��", "��", "��", "��", "�", "Ѧ", "��",
"��", "��", "��", "��", "��", "³", "Τ", "��", "��", "��", "��", "��", "��", "��", "��", "Ԭ", "��", "ۺ", "��", "ʷ", "��", "��", "��", "��", "��", "��",
"��", "��", "��", "��", "��", "��", "��", "��", "ʱ", "��", "Ƥ", "��", "��", "��", "��", "��", "Ԫ", "��", "��", "��", "ƽ", "��", "��", "��", "��", "��",
"��", "��", "��", "Ҧ", "��", "տ", "��", "��", "ë", "��", "��", "��", "��", "��", "�", "��", "��", "��", "��", "̸", "��", "é", "��", "��", "��", "��",
"��", "��", "ף", "��", "��", "��", "��", "��", "��", "ϯ", "��", "��", "ǿ", "��", "·", "¦", "Σ", "��", "ͯ", "��", "��", "÷", "ʢ", "��", "��", "��",
"��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "֧", "��", "��", "��", "¬", "Ī", "��", "��", "��", "��", "��", "��", "Ӧ",
"��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "ʯ", "��", "��", "ť", "��", "��", "��", "��", "��", "��", "½", "��", "��", "��",
"��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "ɽ",
"��", "��", "��", "�", "��", "ȫ", "ۭ", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��",
"ղ", "��", "��", "Ҷ", "��", "˾", "��", "۬", "��", "��", "��", "ӡ", "��", "��", "��", "��", "ۢ", "��", "��", "��", "��", "��", "��", "׿", "��", "��",
"��", "��", "��", "��", "��", "��", "��", "��", "˫", "��", "ݷ", "��", "��", "̷", "��", "��", "��", "��", "��", "��", "��", "Ƚ", "��", "۪", "Ӻ", "ȴ",
"�", "ɣ", "��", "�", "ţ", "��", "ͨ", "��", "��", "��", "��", "��", "��", "ũ", "��", "��", "ׯ", "��", "��", "��", "��", "��", "Ľ", "��", "��", "ϰ",
"��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "»",
"��", "��", "ŷ", "�", "��", "��", "ε", "Խ", "��", "¡", "ʦ", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��",
"��", "��", "ɳ", "ؿ", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "ۣ", "��", "Ȩ", "��", "��", "��", "��", "��", "��",
"��", "��", "˧", "��", "��", "��", "�C", "��", "��", "��", "��", "��", "��", "��", "��", "��", "۳", "Ϳ", "��", "��", "Ĳ", "��", "٦", "��", "��", "ī",
"��", "��", "��", "��", "��", "��", "١", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "չ", "��", "̴", "��", "��", "��", "��", "˴", "¥",
"��", "ð", "��", "ֿ", "��", "��", "��", "��", "ԭ", "��", "��", "��", "��", "��", "�", "��", "��", "��", "�", "��", "��", "��", "��", "��", "��", "��",
"ľ", "��", "��", "ۨ", "��", "ö", "��", "��", "�", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��", "��",
"��", "��", "��", "�G", "��ٹ", "˾��", "�Ϲ�", "ŷ��", "�ĺ�", "���", "����", "����", "����", "�ʸ�", "����", "ξ��", "����", "�̨", "��ұ", "����",
"���", "����", "����", "̫��", "����", "����", "����", "��ԯ", "���", "����", "����", "����", "Ľ��", "����", "����", "˾ͽ", "˾��", "أ��", "˾��",
"����", "����", "�ӳ�", "���", "��ľ", "����", "����", "���", "����", "����", "����", "�ذ�", "�й�", "�׸�", "����", "�θ�", "����", "����", "΢��",
"����", "����", "����", "����", "�Ϲ�", "����", "����", "����", "̫ʷ", "�ٳ�", "����", "��ͻ", "����", "����", "����", "��ĸ", "˾��", "����", "Ӻ��",
"����", "����", "����", "��®", "����", "�Ϲ�", "����", "����"
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

	int high = 0xd7 - 0xc1; //16-55������
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
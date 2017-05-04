#ifndef __RAND_WORDS_H__
#define __RAND_WORDS_H__

#include <map>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <iostream>
#include <stdio.h>

class CRandWords
{
public:
	CRandWords();
	~CRandWords();
	int GetRandChineseName(unsigned int chineseNum, std::string &outName);
	int GetRandEnglshName(unsigned int englishNum, std::string &outName);
	int GetRandNumbers(unsigned int numberNum, std::string &outName);
	int GetRandCEConsistName(unsigned int chineseNum, unsigned int englishNum, std::string &outName);
	int GetRandCENConsistName(unsigned int chineseNum, unsigned int englishNum, unsigned int numberNum, std::string &outName);
	int GetRandNumbersPasswd(unsigned int numberNum, std::string &outName);
	int GetRandENConsistPasswd(unsigned int numberNum, std::string &outName);
private:
	bool RandChineseWordsMan(unsigned int chineseNum, std::string &outName);
	int RandNumberMan(unsigned int numCount, std::string &outNum);
	int RandNumberMan();
	int RandWordsMan(unsigned int wordsNum, std::string &outName);
	int RandWordsMan(unsigned int wordsNum, char &outName);
	int RandNumbersWordsMan(unsigned int wordsNum, std::string &outName);

	bool CompareString(const char* destStr);
	void AddChineseSurname();
	bool SingleWordMatch(const char* sourceStr, char target[3]);

private:
	typedef std::map<const char*, const char*> ChineseSurname;
	ChineseSurname *m_pcsMap;


};

void InitRandWordsObj();

CRandWords *GetRandWordsObj();

void DestroyRandWordsObj();

#endif
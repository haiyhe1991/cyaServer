#include "StrHahs.h"

CStrHash::CStrHash()
{
	InitCryptTable();
}

CStrHash::~CStrHash(void)
{
}

/************************************************************************/
/*函数名：InitCryptTable
/*功  能：对哈希索引表预处理
/*返回值：无
/************************************************************************/
void CStrHash::InitCryptTable()
{
	unsigned long seed = 0x00100001, index1 = 0, index2 = 0, i;

	for (index1 = 0; index1 < 0x100; index1++)
	{
		for (index2 = index1, i = 0; i < 5; i++, index2 += 0x100)
		{
			unsigned long temp1, temp2;
			seed = (seed * 125 + 3) % 0x2AAAAB;
			temp1 = (seed & 0xFFFF) << 0x10;
			seed = (seed * 125 + 3) % 0x2AAAAB;
			temp2 = (seed & 0xFFFF);
			cryptTable[index2] = (temp1 | temp2);
		}
	}
}

/************************************************************************/
/*函数名：HashStr
/*功  能：求取哈希值
/*返回值：返回hash值
/************************************************************************/
unsigned long CStrHash::HashStr(const std::string& lpszString)
{
	unsigned long DF_HASH = 0;
	unsigned char *key = (unsigned char *)(const_cast<char*>(lpszString.c_str()));
	unsigned long seed1 = 0x7FED7FED, seed2 = 0xEEEEEEEE;
	int ch;

	while (*key != 0)
	{
		ch = toupper(*key++);

		seed1 = cryptTable[(DF_HASH << 8) + ch] ^ (seed1 + seed2);
		seed2 = ch + seed1 + seed2 + (seed2 << 5) + 3;
	}
	return seed1;
}
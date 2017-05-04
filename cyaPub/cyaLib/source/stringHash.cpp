#include "stringHash.h"

unsigned long RSHash(const char* str)
{
	unsigned long b = 378551;
	unsigned long a = 63689;
	unsigned long hash = 0;

	while (*str)
	{
		hash = hash * a + (*str++);
		a *= b;
	}

	return (hash & 0x7FFFFFFF);
}

unsigned long JSHash(const char* str)
{
	unsigned long hash = 1315423911;
	while (*str)
		hash ^= (hash << 5) + (*str++) + (hash >> 2);

	return (hash & 0x7FFFFFFF);
}

unsigned long PJWHash(const char* str)
{
	const unsigned long BitsInUnignedInt = sizeof(unsigned long) * 8;
	const unsigned long ThreeQuarters = BitsInUnignedInt * 3 / 4;
	const unsigned long OneEighth = BitsInUnignedInt / 8;
	const unsigned long HighBits = (unsigned long)(0xFFFFFFFF) << (BitsInUnignedInt - OneEighth);
	unsigned long hash = 0;
	unsigned long test = 0;

	while (*str)
	{
		hash = (hash << OneEighth) + (*str++);
		if ((test = hash & HighBits) != 0)
		{
			hash = ((hash ^ (test >> ThreeQuarters)) & (~HighBits));
		}
	}

	return (hash & 0x7FFFFFFF);
}

unsigned long ELFHash(const char* str)
{
	unsigned long hash = 0;
	unsigned long x = 0;

	while (*str)
	{
		hash = (hash << 4) + (*str++);
		if ((x = hash & 0xF0000000L) != 0)
		{
			hash ^= (x >> 24);
			hash &= ~x;
		}
	}

	return (hash & 0x7FFFFFFF);
}

unsigned long BKDRHash(const char* str)
{
	unsigned long seed = 131; /* 31 131 1313 13131 131313 etc.. */
	unsigned long hash = 0;

	while (*str)
		hash = hash * seed + (*str++);

	return (hash & 0x7FFFFFFF);
}

unsigned long SDBMHash(const char* str)
{
	unsigned long hash = 0;
	while (*str)
		hash = (*str++) + (hash << 6) + (hash << 16) - hash;

	return (hash & 0x7FFFFFFF);
}

unsigned long DJBHash(const char* str)
{
	unsigned long hash = 5381;
	while (*str)
		hash += (hash << 5) + (*str++);

	return (hash & 0x7FFFFFFF);
}

unsigned long APHash(const char* str)
{
	unsigned long hash = 0;
	int i;

	for (i = 0; *str; i++)
	{
		if ((i & 1) == 0)
			hash ^= (hash << 7) ^ (*str++) ^ (hash >> 3);
		else
			hash ^= ~((hash << 11) ^ (*str++) ^ (hash >> 5));
	}

	return (hash & 0x7FFFFFFF);
}

unsigned long PHPPJWHash(const char* str, int len)
{
	unsigned long h = 0, g;
	const char* end = str + len;

	while (str < end)
	{
		h = (h << 4) + *str++;
		if ((g = (h & 0xF0000000)) != 0)
		{
			h = h ^ (g >> 24);
			h = h ^ g;
		}
	}
	return h;
}

unsigned long MySqlHash(const char* str, int len)
{
	unsigned long nr = 1;
	unsigned long nr2 = 4;
	while (len--)
	{
		nr ^= ((nr & 63) + nr2) * (unsigned int)(*str++)
			+ (nr << 8);
		nr2 += 3;
	}
	return nr;
}

unsigned long NollVoHash(const char* str, int len)
{
	const char* end = str + len;
	unsigned long hash;
	for (hash = 0; str < end; str++)
	{
		hash *= 16777619;
		hash ^= (unsigned long)(*str);
	}
	return hash;
}

unsigned long ExpHash(const char* str)
{
	unsigned long h;
	for (h = 0; *str != 0; str++)
		h = 31 * h + *(unsigned char*)str;

	return h;
}

//int GetHashTablePos(char *lpszString, MPQHASHTABLE *lpTable, int nTableSize)
//{
//	const int HASH_OFFSET = 0, HASH_A = 1, HASH_B = 2;
//	int nHash = HashString(lpszString, HASH_OFFSET);
//	int nHashA = HashString(lpszString, HASH_A);
//	int nHashB = HashString(lpszString, HASH_B);
//	int nHashStart = nHash % nTableSize, nHashPos = nHashStart;
//
//	while (lpTable[nHashPos].bExists)
//	{
//		if (lpTable[nHashPos].nHashA == nHashA && lpTable[nHashPos].nHashB == nHashB)
//			return nHashPos;
//		else
//			nHashPos = (nHashPos 1) % nTableSize;
//
//		if (nHashPos == nHashStart)
//			break;
//	}
//
//	return -1; //Error value 
//}
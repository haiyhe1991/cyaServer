#include "LicenceCommon.h"
#include "cyaTypes.h"
#include <time.h>

void GenerateEncryptKey(char* pszKey, int keyBytes)
{
	ASSERT(NULL != pszKey);
	if (keyBytes <= 0)
		return;

	srand((unsigned)time(NULL));

	for (int i = 0; i < keyBytes; i++)
	{
		int flag = rand() % 3;
		switch (flag)
		{
		case 0:
			pszKey[i] = 'A' + rand() % 26;
			break;
		case 1:
			pszKey[i] = 'a' + rand() % 26;
			break;
		case 2:
			pszKey[i] = '0' + rand() % 10;
			break;
		default:
			pszKey[i] = 'x';
			break;
		}
	}
}
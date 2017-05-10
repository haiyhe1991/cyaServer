#include "GenerateString.h"
#include "cyaTime.h"

UINT16 GenerateRandomString(BYTE* token, UINT16 bufLen)
{
	ASSERT(NULL != token);
	memset(token, 0, bufLen);
	if (bufLen <= 0)
		return 0;

	int flag, i;
	srand((unsigned)time(NULL));
	for (i = 0; i < bufLen; i++)
	{
		flag = rand() % 3;
		switch (flag)
		{
		case 0:
			token[i] = 'A' + rand() % 26;
			break;
		case 1:
			token[i] = 'a' + rand() % 26;
			break;
		case 2:
			token[i] = '0' + rand() % 10;
			break;
		default:
			token[i] = 'x';
			break;
		}
	}
	return bufLen;
}
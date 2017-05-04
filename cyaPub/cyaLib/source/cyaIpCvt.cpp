#include "cyaIpCvt.h"

BOOL IsValidStringIP(const char* lpszIP, DWORD* dwIp /*= NULL*/)
{
	int nFieldCount = 0;
	char ch;
	char chField[4];
	char ch255[4] = { '2', '5', '5', '\0' };
	int i = 0, j = 0;  /*j用来计数，4个ip的3位数字如192。168.1.100里面的192*/
#ifdef __cplusplus
	union
	{
		BYTE ip2_b4[4];
		DWORD ip2_dw;
	} ip2_uni;
	BYTE* ip2 = ip2_uni.ip2_b4;
#else
	BYTE ip2[4];
#endif
	chField[3] = 0;
	if (dwIp)
		*dwIp = INADDR_NONE;

	for (;;)
	{
		ch = lpszIP[i];
		if ((ch < '0' || ch > '9') && ch != '.' && ch != 0) /* 出现非法字符 */
			return false;

		if (ch && ch != '.')
		{
			if (j >= 3 || i >= 15) /* 该域数字个数大于3个 或者字符串超长 */
				return false;
			chField[j++] = lpszIP[i++];
		}
		else
		{
			if (j == 0) /* 该域为空 */
				return false;

			if (j == 2)
			{
				chField[2] = chField[1];
				chField[1] = chField[0];
				chField[0] = '0';
			}
			else if (j == 1)
			{
				chField[2] = chField[0];
				chField[1] = '0';
				chField[0] = '0';
			}
			j = 0;

			if (strcmp(chField, ch255) > 0)
				return false; /* 该域的值大于了255 */

			if (dwIp)
				ip2[nFieldCount] = (BYTE)((chField[0] - '0') * 100 + (chField[1] - '0') * 10 + (chField[2] - '0'));

			nFieldCount++;

			if (lpszIP[i] == 0)
				break;	/* break off while(1) */

			if (nFieldCount == 4)
				return false;

			i++;
		}
	}

	if (nFieldCount != 4) /* 不是4个域 */
		return false;

	if (dwIp)
	{
#ifdef __cplusplus
		*dwIp = ip2_uni.ip2_dw;
#else
		*dwIp = *(DWORD*)ip2;
#endif
	}

	return true;
}

// DWORD ip 转化为 字符串ip
// szBuf必须传递，至少为16字节长
const char* GetStringIP(char szBuf[16], DWORD dwIP)
{
	if (NULL == szBuf)
		return NULL;
	sprintf(szBuf, "%d.%d.%d.%d", (BYTE)dwIP, (BYTE)(dwIP >> 8), (BYTE)(dwIP >> 16), (BYTE)(dwIP >> 24));
	return szBuf;
}

// 字符串ip 转化为 DWORD ip，失败返回INADDR_NONE
DWORD GetDWordIP(const char* lpszIP)
{
	DWORD dwIp;
	IsValidStringIP(lpszIP, &dwIp);
	return dwIp;
}
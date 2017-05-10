#ifndef __GENERATE_STRING_H__
#define __GENERATE_STRING_H__

#include "cyaTypes.h"

/// 产生随机字符串，包含大小写字母和数字, 返回字符串的长度
UINT16 GenerateRandomString(BYTE* token, UINT16 bufLen);

#endif
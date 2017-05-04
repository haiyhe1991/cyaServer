#ifndef __SERVICE_COMMON_H__
#define __SERVICE_COMMON_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#if !defined(SERVICE_UNREAL_BUILD)
	#include "cyaTypes.h"
#endif	//SERVICE_UNREAL_BUILD

#define SGS_DATE_TIME_LEN 20	//1970-01-01 00:00:00

#define MAX_SGS_U8_NUM    0xFF
#define MAX_SGS_U16_NUM	  0xFFFF
#define MAX_SGS_U32_NUM   0xFFFFFFFF
#define MAX_SGS_BIG_NUM	  0xFFFFFFFFFFFF

#pragma pack(push, 1)

struct SBigNumber
{
	//short  hVal;
	//int    lVal;     //1½øÎ»
	UINT16 hi;
	UINT32 lo;
};

#pragma pack(pop)

inline UINT64 BigNumberToU64(const SBigNumber& bgNum)
{
	return (UINT64)bgNum.hi * 100000000 + bgNum.lo;
}

inline void U64ToBigNumber(UINT64 u64Val, SBigNumber& bgNum)
{
	if (u64Val > MAX_SGS_BIG_NUM)
	{
		u64Val = MAX_SGS_BIG_NUM;
	}
	bgNum.lo = (UINT32)(u64Val % 100000000);
	bgNum.hi = (UINT16)(u64Val / 100000000);
}

inline void SBigNumberAdd(SBigNumber& addNum1, const SBigNumber& addNum2)
{
	UINT64 sum = BigNumberToU64(addNum1);
	sum += BigNumberToU64(addNum2);
	if(sum > MAX_SGS_BIG_NUM)
		sum = MAX_SGS_BIG_NUM;
	U64ToBigNumber(sum, addNum1);
}

inline void SBigNumberAdd(SBigNumber& addNum1, UINT64 addNum2)
{
	if(addNum2 >= MAX_SGS_BIG_NUM)
		U64ToBigNumber(addNum2, addNum1);
	else
	{
		UINT64 sum = BigNumberToU64(addNum1);
		if(sum < MAX_SGS_BIG_NUM)
			sum += addNum2;
		if(sum > MAX_SGS_BIG_NUM)
			sum = MAX_SGS_BIG_NUM;
		U64ToBigNumber(sum, addNum1);
	}
}

inline void SBigNumberAdd(SBigNumber& bgNum, UINT64 addNum1, UINT64 addNum2)
{
	if(addNum1 >= MAX_SGS_BIG_NUM || addNum2 >= MAX_SGS_BIG_NUM)
		U64ToBigNumber(MAX_SGS_BIG_NUM, bgNum);
	else
	{
		UINT64 sum = addNum1 + addNum2;
		if(sum > MAX_SGS_BIG_NUM)
			sum = MAX_SGS_BIG_NUM;
		U64ToBigNumber(sum, bgNum);
	}
}

inline void SBigNumberSub(SBigNumber& subNum1, const SBigNumber& subNum2)
{
	UINT64 sum = BigNumberToU64(subNum1);
	UINT64 sub = BigNumberToU64(subNum2);
	if(sum <= sub)
	{
		subNum1.hi = 0;
		subNum1.lo = 0;
	}
	else
	{
		sum -= sub;
		if(sum > MAX_SGS_BIG_NUM)
			sum = MAX_SGS_BIG_NUM;
		U64ToBigNumber(sum, subNum1);
	}	
}

inline void SBigNumberSub(SBigNumber& subNum1, UINT64 subNum2)
{
	UINT64 sum = BigNumberToU64(subNum1);
	if(sum <= subNum2)
	{
		subNum1.hi = 0;
		subNum1.lo = 0;
	}
	else
	{
		sum -= subNum2;
		if(sum > MAX_SGS_BIG_NUM)
			sum = MAX_SGS_BIG_NUM;
		U64ToBigNumber(sum, subNum1);
	}	
}

inline void SBigNumberSub(SBigNumber& bgNum, UINT64 subNum1, UINT64 subNum2)
{
	if(subNum1 <= subNum2)
	{
		bgNum.lo = 0;
		bgNum.hi = 0;
	}
	else
	{
		UINT64 sum = subNum1 - subNum2;
		if(sum > MAX_SGS_BIG_NUM)
			sum = MAX_SGS_BIG_NUM;
		U64ToBigNumber(sum, bgNum);
	}
}

inline UINT64 SGSU64Add(UINT64 add1, UINT64 add2)
{
	if(add1 >= MAX_SGS_BIG_NUM || add2 >= MAX_SGS_BIG_NUM)
		return MAX_SGS_BIG_NUM;
	return add1 + add2;
}

inline UINT64 SGSU64Sub(UINT64 sub1, UINT64 sub2)
{
	if(sub1 <= sub2)
		return 0;
	return sub1 - sub2;
}

inline UINT32 SGSU32Add(UINT32 add1, UINT32 add2)
{
	UINT64 sum = (UINT64)add1 + add2;
	return sum > MAX_SGS_U32_NUM ? MAX_SGS_U32_NUM : (UINT32)sum;
}

inline UINT16 SGSU16Add(UINT16 add1, UINT16 add2)
{
	UINT32 sum = (UINT32)add1 + add2;
	return sum > MAX_SGS_U16_NUM ? MAX_SGS_U16_NUM : (UINT16)sum;
}

inline BYTE SGSU8Add(BYTE add1, BYTE add2)
{
	UINT16 sum = (UINT16)add1 + add2;
	return sum > MAX_SGS_U8_NUM ? MAX_SGS_U8_NUM : (BYTE)sum;
}

inline UINT32 SGSU32Sub(UINT32 sub1, UINT32 sub2)
{
	if(sub1 <= sub2)
		return 0;

	return sub1 - sub2;
}

inline UINT16 SGSU16Sub(UINT16 sub1, UINT16 sub2)
{
	if(sub1 <= sub2)
		return 0;

	return sub1 - sub2;
}

inline BYTE SGSU8Sub(BYTE sub1, BYTE sub2)
{
	if(sub1 <= sub2)
		return 0;

	return sub1 - sub2;
}

#endif	//_SGSCommon_h__
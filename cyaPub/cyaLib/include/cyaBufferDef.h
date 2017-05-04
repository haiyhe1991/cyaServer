#ifndef __CYA_BUFFER_DEF_H__
#define __CYA_BUFFER_DEF_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaTypes.h"

#define BEGIN_SMART_BUF_EX(str_id, TYPE, buf_name, stack_len, need_len, fnAlloc)	\
	TYPE str_id##_buf_SMART_BUF[(stack_len)];							\
	TYPE* str_id##_p_SMART_BUF = NULL;									\
	TYPE* buf_name;														\
	TYPE** str_id##_buf_name_SMART_BUF = &buf_name;						\
	/* 若STATIC_ASSERT()失败，表示用法错误（预定义堆栈尺寸太大） */			\
	/* negative subscript or subscript is too large */					\
		{ typedef int stack_size_too_big[(sizeof(TYPE)*(stack_len) <= 512*1024) ? 1 : -1];	}	\
	if((need_len) <= (stack_len))										\
	buf_name = str_id##_buf_SMART_BUF;								\
		else																\
	buf_name = str_id##_p_SMART_BUF =								\
	(TYPE*)((fnAlloc)									\
	? (fnAlloc)(sizeof(TYPE)*(need_len))			\
	: malloc(sizeof(TYPE)*(need_len))				\
	)

#define END_SMART_BUF_EX(str_id, fnFree)								\
	do																	\
		{																	\
	if(str_id##_p_SMART_BUF)										\
			{																\
		if(fnFree)													\
		(fnFree)(str_id##_p_SMART_BUF);							\
						else														\
			free(str_id##_p_SMART_BUF);								\
			str_id##_p_SMART_BUF = NULL;								\
			*str_id##_buf_name_SMART_BUF = NULL;						\
			}																\
		} while(0)


#define BEGIN_SMART_BUF(str_id, TYPE, buf_name, stack_len, need_len)	\
	BEGIN_SMART_BUF_EX(str_id, TYPE, buf_name, stack_len, need_len, (fnMemAllocFuncPtr)NULL)

#define END_SMART_BUF(str_id)	END_SMART_BUF_EX(str_id, (fnMemFreeFuncPtr)NULL)

#endif
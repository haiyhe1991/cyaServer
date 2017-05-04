#ifndef __PERFORMANCE_H__
#define __PERFORMANCE_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include "cyaOS_opt.h"

class PerformanceCounter
{
public:
	PerformanceCounter();
	~PerformanceCounter();

	//��ȡ����ִ��ʱ��,��ȷ�ĵ�΢��
	INT64 GetExcuteUsec();

private:
	INT64 m_startCounter;
};


#endif
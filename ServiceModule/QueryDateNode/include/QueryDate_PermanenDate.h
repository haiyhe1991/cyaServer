#ifndef __QUERY_DATE_PERMANEN_DATA_H__
#define __QUERY_DATE_PERMANEN_DATA_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <BaseTsd.h>
#include <map>

//节点字段信息;
struct M_helps_Date
{
	//引导数;
	char guideNum;
	//引导1;
	UINT16 guide_1;
};

//新手引导保存数据;
struct M_permanentDateres
{
	//id;
	int actorid;
	//创建时间;
	char help_tts[20];
	//最后登录时间;
	char magic_tts[20];
	//新手引导号;
	M_helps_Date M_pdata[1];
};


class QueryDate_PermanenDate
{
	typedef void* GMHandle;
public:
	QueryDate_PermanenDate(void);
	~QueryDate_PermanenDate(void);

	//查询已完成新手引导;
	int M_QueryRoleFinishedHelps();

	//接收数据线程;

	void M_ReadDataThread();

	//发送数据线程;
	void M_SendDataThread();

	//发送数据;
	void M_ReadData();

	//查询数据;
	int M_QueryData(const char* ip, int port, GMHandle gmhande, M_permanentDateres* DateresB);
private:


};

#endif
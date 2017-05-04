#ifndef __QUERY_DATE_API_H__
#define __QUERY_DATE_API_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

class QueryDateAPI
{
public:
	QueryDateAPI(void);
	~QueryDateAPI(void);

	//查询节点;
	void QuerNodeExcel();
	//查询登录;
	void QuerActor_info();
};

#endif
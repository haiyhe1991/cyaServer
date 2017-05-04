#ifndef __QUERY_DATE_PERMANEN_DATA_H__
#define __QUERY_DATE_PERMANEN_DATA_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

#include <BaseTsd.h>
#include <map>

//�ڵ��ֶ���Ϣ;
struct M_helps_Date
{
	//������;
	char guideNum;
	//����1;
	UINT16 guide_1;
};

//����������������;
struct M_permanentDateres
{
	//id;
	int actorid;
	//����ʱ��;
	char help_tts[20];
	//����¼ʱ��;
	char magic_tts[20];
	//����������;
	M_helps_Date M_pdata[1];
};


class QueryDate_PermanenDate
{
	typedef void* GMHandle;
public:
	QueryDate_PermanenDate(void);
	~QueryDate_PermanenDate(void);

	//��ѯ�������������;
	int M_QueryRoleFinishedHelps();

	//���������߳�;

	void M_ReadDataThread();

	//���������߳�;
	void M_SendDataThread();

	//��������;
	void M_ReadData();

	//��ѯ����;
	int M_QueryData(const char* ip, int port, GMHandle gmhande, M_permanentDateres* DateresB);
private:


};

#endif
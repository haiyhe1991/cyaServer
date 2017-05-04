#include "stdafx.h"
#include "QueryDate_PermanenDate.h"
#include <string>

#include "ServiceErrorCode.h"
#include "DBSSdk.h"
#include "GMSSDk.h"
using namespace std;
//��Ҫ��ѯ�����ݿ�����;
std::string dbName = "wgsverify";
std::string dbIp = "192.168.1.145";

QueryDate_PermanenDate::QueryDate_PermanenDate(void)
{

}

QueryDate_PermanenDate::~QueryDate_PermanenDate(void)
{

}

int QueryDate_PermanenDate::M_QueryRoleFinishedHelps()
{
	return 0;
}

int QueryDate_PermanenDate::M_QueryData(const char* ip, int port, GMHandle gmhande, M_permanentDateres* DateresB)
{
	char sql[1024] = { 0 };
	//�������ݿ������;
	DBSHandle handle = DBS_Connect(ip, port);
	//��ָ������;
	DBS_RESULT result = NULL;

	//char stime[20]={0};
	//char sdate[20]={0};
	//char conTime[20]={0};

	//��ѯSql���;
	sprintf(sql, "select actorid  helps help_tts  magic_tts from permanent ");

	//��Sql���в���,result��ѯ�����;
	int retCode = DBS_ExcuteSQL(handle, DBS_MSG_QUERY, "sgs", sql, result, strlen(sql));
	if (retCode != DBS_OK)
		return retCode;
	//��ȡ��¼����;
	UINT32 nRows = DBS_NumRows(result);
	if (nRows == 0)
	{
		//���Ϊ�����ͷŵ�;
		DBS_FreeResult(result);
		return DBS_ERROR;
	}
	DBS_ROW row = NULL;

	while ((row = DBS_FetchRow(result)) != NULL)
	{


	}

	memset(sql, 0, 1024);

	return 0;


}
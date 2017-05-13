// main.cpp : ���������Ӧ�ó������ڵ㡣
//
#include "GMCfgMan.h"
#include "GMServeMan.h"
#include "cyaIpCvt.h"
#include "mainloop.h"
#include "minidump.h"
#include "cyaLog.h"
#include "OtherServeConnMan.h"
#include "ServeConnMan.h"

#define CONFIG_PATH			"./cyaGMServer.properties"

int main(int argc, char** argv)
{
	///����core�ļ�
	SaveExceptionDumpFile();

	//����logĿ¼
#if defined(WIN32)
	CyaLogSetDir("log\\gmslog");
#else
	CyaLogSetDir("log/gmslog");
#endif

	///����ÿ������ˢ����־�ļ�
	CyaLogSetFlushLines(2);

	///���ع���������������
	InitCfgManObj(CONFIG_PATH);

	///��ʼ���������������
	InitGMServeManObj();

	///��������
	int ret = GetGMServeManObj()->GetGMServeMan()->StartServe(IPPADDR(INADDR_ANY, GetCfgManObj()->GetServePort()));
	if (ret != 0)
		LogInfo(("����������ʧ��,[%d]", ret));
	else
	{
#if IS_SCREEN_LOG
		LogInfo(("���������������[�˿ں�: %d]�����ɹ�!", GetCfgManObj()->GetServePort()));
#endif
	}



	///��ʼ��ʱʱд��db����+


	InitOtherServeManObj();

	RunMainLoop();

	///�������ù������
	DestroyCfgManObj();


	///ֹͣ����
	GetGMServeManObj()->GetGMServeMan()->StopServe();

	///���ٺ�ֹͣ����д��db����
	DestroyOtherServeManObj();


	///���ٷ���������
	DestroyGMServeManObj();

	return 0;
}


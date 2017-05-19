// main.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "mainLoop.h"
#include "cyaLog.h"
#include "miniDump.h"

#include "ConfigFileMan.h"
#include "GWSClientMan.h"
#include "DBSClient.h"
#include "MLogicProcessMan.h"
#include "GlobalTimerTaskMan.h"
#include "PersonalTimerTaskMan.h"
#include "GameCfgFileMan.h"
#include "FunModuleMan.h"
#include "CashDBMan.h"
//#include "PersonalRandInstMan.h"

#define CONFIG_PATH			"./cyaMLServer.properties"
#if 0// defined(WIN32)
#include "MLTest.h"
#endif

int main(int argc, char* argv[])
{

	//����core/dump�ļ�
	SaveExceptionDumpFile();

	//����logĿ¼
#if defined(WIN32)
	CyaLogSetDir("log\\mlslog");
#else
	CyaLogSetDir("log/mlslog");
#endif

	//��ʼ�������ļ�
	if (!InitConfigFileMan(CONFIG_PATH))
	{
		LogInfo(("��ȡ�����ļ�[%s]ʧ��!", CONFIG_PATH));
		goto __end__;
	}

	//20150928 add by hxw ���ӷ���������ʱ����
	while (1)
	{
		LTMSEL nowMSel = GetMsel();
		printf("time:%d\n", nowMSel - GetConfigFileMan()->GetPartOpenTime());
		if (nowMSel>GetConfigFileMan()->GetPartOpenTime())
			break;
		else
			Sleep(60);
	}

	//end



	//��ʼ����ɫ���������
	InitRoleInfosObjAllocator();

	//����������ݷ�����
	if (MLS_OK != InitDBSClient())
	{
		LogInfo(("����������ݷ�����ʧ��!"));
		goto __end__;
	}

	//���ӳ�ֵ���ݷ�����
	if (!ConnectRechargeDBServer())
	{
		LogInfo(("���ӳ�ֵ���ݷ�����ʧ��!"));
		goto __end__;
	}

	//add by hxw 20151117
	//���Ӵ������ݷ�����
	if (!ConnectCashDBServer())
	{
		LogInfo(("���Ӵ������ݷ�����ʧ��!"));
		goto __end__;
	}
	//end

	//��ʼ����Ϸ�����ļ�
	if (!InitGameCfgFileMan())
	{
		LogInfo(("��ʼ����Ϸ�����ļ�ʧ��!"));
		goto __end__;
	}

	//��ʼ������ģ��
	InitFunModuleMan();

	//��ʼ���߼�������
	InitMLogicProcessMan();

	//��ʼ��ȫ���������
	InitGlobalTimerTaskMan();

#if 0//defined(WIN32)
	MLTest* test = new MLTest;
	test->GoTest();
	delete test;
#endif

	//��ʼ��/��½���ط�����
	if (MLS_OK != InitGWSClientMan())
	{
		LogInfo(("��ʼ��/��½���ط�����ʧ��!"));
		goto __end__;
	}


	//����ϵͳ����
	OpenSystemNotice();

	//���������������
	//StartPersonalRandInst();

	LogInfo(("=========���߼�������[%s]�����ɹ�!=========", GetConfigFileMan()->GetVersion()));





	//��ѭ��
	RunMainLoop();

__end__:
	//ֹͣ�����������
	//StopPersonalRandInst();

	//�ر�ϵͳ����
	CloseSystemNotice();

	//�ͷ�GWSClientMan��Դ
	DestroyGWSClientMan();

	//�ͷ�ȫ���������
	DestroyGlobalTimerTaskMan();

	//�ͷ��߼�����������
	DestroyMLogicProcessMan();

	//�ͷŹ���ģ��
	DestroyFunModuleMan();

	//�Ͽ���ֵ���ݷ�����
	DisConnectRechargeDBServer();

	//add by hxw 20151117
	//�Ͽ��������ݷ�����
	DisConnectCashDBServer();

	//�ͷ�DBSClient��Դ
	DestroyDBSClient();

	//�ͷŽ�ɫ���������
	DestroyRoleInfosObjAllocator();

	//�ͷ���Ϸ�����ļ�����
	DestroyGameCfgFileMan();

	//�ͷ�ConfigFileMan��Դ
	DestroyConfigFileMan();

	getchar();

	return 0;
}


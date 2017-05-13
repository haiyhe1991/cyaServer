#include "mainloop.h"
#include "miniDump.h"
#include "cyaLog.h"

#include "DBSClient.h"
#include "ConfigFileMan.h"
#include "LicenceServeMan.h"
#include "LicenceLogicProcMan.h"
#include "PartitionInfoMan.h"
#include "UserInfoMan.h"

#define CONFIG_PATH			"./cyaLCServer.properties"

int main(int argc, char* argv[])
{
	//����core�ļ�
	SaveExceptionDumpFile();

	//����logĿ¼
#if defined(WIN32)
	CyaLogSetDir("log\\lcslog");
#else
	CyaLogSetDir("log/lcslog");
#endif

	//��ʼ�������ļ�
	if (!InitConfigFileMan(CONFIG_PATH))
	{
		LogInfo(("��ȡ�����ļ�ʧ��!"));
		goto __end__;
	}

	//��ʼ�����ݷ���������
	if (!InitDBSClient())
	{
		LogInfo(("��ʼ�����ݷ���������ʧ��!"));
		goto __end__;
	}

	//��ʼ���û���Ϣ�������
	InitUserInfoMan();

	//��ʼ��������Ϣ
	if (!InitPartitionInfoMan())
	{
		LogInfo(("��ʼ��������Ϣʧ��!"));
		goto __end__;
	}

	//��֤�������߼�����
	InitLicenceLogicProcMan();

	//�����������
	StartLicenceServe();

	LogInfo(("========��֤������[%s][%d]�����ɹ�!========", GetConfigFileMan()->GetVersion(), GetConfigFileMan()->GetServePort()));

__end__:

	//��ѭ��
	RunMainLoop();


	//ֹͣ�������
	StopLicenceServe();

	//��֤�������߼�����
	DestroyLicenceLogicProcMan();

	//���������Ϣ
	DestroyPartitionInfoMan();

	//�����û���Ϣ�������
	DestroyUserInfoMan();

	//�������ݷ��������Ӷ���
	DestroyDBSClient();

	//���������ļ��������
	DestroyConfigFileMan();

	return 0;
}
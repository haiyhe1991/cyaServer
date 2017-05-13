// GWServer.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "mainloop.h"
#include "minidump.h"
#include "cyaLog.h"

#include "GWSession.h"
#include "ConfigFileMan.h"
#include "RelaySessionMan.h"
#include "GWServeMan.h"


#define CONFIG_PATH			"./cyaGWServer.properties"

int main(int argc, char* argv[])
{
	//����core�ļ�
	SaveExceptionDumpFile();

	//����logĿ¼
#if defined(WIN32)
	CyaLogSetDir("log\\gwslog");
#else
	RoseLogSetDir("log/gwslog");
#endif

	//��ʼ�������ļ�����
	InitConfigFileMan(CONFIG_PATH);

	//��ʼ��ת���Ự����
	InitRelaySessionMan();

	//�����������
	StartGWServe();

	LogInfo(("========���ط�����[%s][%d]�����ɹ�!========", GetConfigFileMan()->GetVersion(), GetConfigFileMan()->GetServePort()));

	//��ѭ��
	RunMainLoop();

	//ֹͣ�������
	StopGWServe();

	//����ת���Ự����
	DestroyRelaySessionMan();

	//���������ļ�����
	DestroyConfigFileMan();

	return 0;
}

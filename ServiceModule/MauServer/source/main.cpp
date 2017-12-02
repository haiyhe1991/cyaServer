// ChatServer.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "mainloop.h"
#include "minidump.h"
#include "cyaLog.h"

#include "MauGWSClientMan.h"
#include "ConfigFileMan.h"
#include "MauTableMan.h"
#include "MauMsgMan.h"

#define CONFIG_PATH			"./cyaMauServer.properties"

int main(int argc, char* argv[])
{
	//����core�ļ�
	SaveExceptionDumpFile();

	//����logĿ¼
#if defined(WIN32)
	CyaLogSetDir("log\\maulog");
#else
	CyaLogSetDir("log/maulog");
#endif

	//��ȡ�����ļ�
	if (!InitConfigFileMan(CONFIG_PATH))
	{
		LogInfo(("��ȡ�����ļ�ʧ��!"));
		goto __end__;
	}

	//��ʼ��Ƶ������
	InitMauTableMan();

	//��ʼ��mau�߼�������
	InitMauMsgMan();

	//��ʼ��,��½���ط�����
	if (MAU_OK != InitGWSClientMan())
	{
		LogInfo(("��¼���ط�����ʧ��!"));
		goto __end__;
	}

	LogInfo(("===========��ɫ�����Ʒ�����[%s]�����ɹ�!===========", GetConfigFileMan()->GetVersion()));

__end__:
	//��ѭ��
	RunMainLoop();

	//����GWSClient��Դ
	DestroyGWSClientMan();

	//����mau�߼�������
	DestroyMauMsgMan();

	//����Ƶ��������Դ
	DestroyMauTableMan();

	//�������ù�����Դ
	DestroyConfigFileMan();

	return 0;
}


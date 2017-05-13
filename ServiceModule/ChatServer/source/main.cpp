// ChatServer.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "mainloop.h"
#include "minidump.h"
#include "cyaLog.h"

#include "GWSClientMan.h"
#include "ConfigFileMan.h"
#include "ChannelMan.h"
#include "ChatMsgMan.h"

#define CONFIG_PATH			"./cyaChatServer.properties"

int main(int argc, char* argv[])
{
	//����core�ļ�
	SaveExceptionDumpFile();

	//����logĿ¼
#if defined(WIN32)
	CyaLogSetDir("log\\chatlog");
#else
	RoseLogSetDir("log/chatlog");
#endif

	//��ȡ�����ļ�
	if (!InitConfigFileMan(CONFIG_PATH))
	{
		LogInfo(("��ȡ�����ļ�ʧ��!"));
		goto __end__;
	}

	//��ʼ��Ƶ������
	InitChannelMan();

	//��ʼ�������߼�������
	InitChatMsgMan();

	//��ʼ��,��½���ط�����
	if (CHAT_OK != InitGWSClientMan())
	{
		LogInfo(("��¼���ط�����ʧ��!"));
		goto __end__;
	}

	LogInfo(("===========���������[%s]�����ɹ�!===========", GetConfigFileMan()->GetVersion()));

__end__:
	//��ѭ��
	RunMainLoop();

	//����GWSClient��Դ
	DestroyGWSClientMan();

	//���ٻ������߼�������
	DestroyChatMsgMan();

	//����Ƶ��������Դ
	DestroyChannelMan();

	//�������ù�����Դ
	DestroyConfigFileMan();

	return 0;
}


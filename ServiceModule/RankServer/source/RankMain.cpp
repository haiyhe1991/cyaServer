#include "mainLoop.h"
#include "miniDump.h"
#include "cyaLog.h"

#include "ServiceErrorCode.h"
#include "RankDBSClient.h"
#include "RankConfigFileMan.h"
#include "RankGWClientMan.h"
#include "RankHandler.h"

int main(int argc, char* argv[])
{
	//����core�ļ�
	SaveExceptionDumpFile();

	//����logĿ¼
#if defined(WIN32)
	CyaLogSetDir("log\\rank\\ranklog");
	CyaLogSetDir("log\\rank\\linklog");
	CyaLogSetDir("log\\rank\\mlogiclog");
#else
	RoseLogSetDir(LOG_RANK, "log/rank/ranklog");
	RoseLogSetDir(LOG_LINK, "log/rank/linklog");
	RoseLogSetDir(LOG_MLOGIC, "log/rank/mlogiclog");
#endif
	// ����ÿn��flushһ��log�ļ�
	CyaLogSetOutScreen(true);
	CyaLogSetFlushLines(2);

	//��ȡ�����ļ�
	if (!InitRankConfigFileMan())
	{
		LogDebug(("��ȡ�����ļ�ʧ��!"));
		goto __end__;
	}

	//��ʼ�����ݷ���������
	if (!InitRankDBSClient())
		goto __end__;

	//��ʼ�����з������߼�������
	InitRankHandler();

	//��ʼ��,��½���ط�����
	if (RANK_OK != InitRankGWClientMan())
		goto __end__;

	LogDebug(("===========���з�����[%s]�����ɹ�!===========", GetRankConfigFileMan()->GetVersion()));

__end__:
	//��ѭ��
	RunMainLoop();

	//����GWSClient��Դ
	DestroyRankGWClientMan();

	//�������з������߼�������
	DestroyRankHandler();

	//�������ݷ��������Ӷ���
	DestroyRankDBSClient();

	//�������ù�����Դ
	DestroyRankConfigFileMan();

	return 0;
}


#include "stdafx.h"
#include "cyaOS_opt.h"
#include <set>
//class testc
//{
//public:
//	testc();
//	~testc();
//
//private:
//
//};
//
//testc::testc()
//{
//}
//
//testc::~testc()
//{
//}
//
//class testChild
//{
//public:
//	testChild();
//	~testChild();
//	
//	int s;
//	char xx[10];
//private:
//};
////
//testChild::testChild()
//{
//	s = 10;
//	for (int i = 0; i < 10; i++)
//	{
//		xx[i] = 65+i;
//	}
//}
//
//testChild::~testChild()
//{
//}
int main()
{
	//void* sss;
	//::new(sss)testChild;
	//int b = ((testChild *)sss)->s;
	//testc *session = new testChild();
	//int asd = offset_of(testChild, xx);
	//testChild* sessSite = (testChild*)((BYTE*)session - asd);
	//printf("��������ƣ�%d\n", ((testChild *)sss)->s);
	printf("----------------�������Ϣ����------------------\n");
	char name[MAX_PATH];
	DWORD nameLen = MAX_PATH;
	if (OSGetComputerName(name, nameLen))
	{
		printf("��������ƣ�%s\n",name);
	}
#if defined(WIN32)
	printf("Windows�汾��%s\n", GetWindowsVersionText());
#endif
	printf("CPU������%d\n", OSGetCpuCount());
	printf("CPUƵ�ʣ�%.2f GHz\n", (float)GetCpuFrequency()/(float)(1024*1024));
	//printf("CPUʹ���ʣ�%d GHz\n", OSGetCpuUsage());
	INT64 totalMem = 0;
	float freeMem = (float)OSGetSysFreeMem(&totalMem) / (float)(1024 * 1024 * 1024);
	printf("ϵͳ���ڴ棺%.2f GB  �����ڴ棺%.2f GB\n", (float)totalMem/(1024*1024*1024), freeMem);
	INT64 totalSpace = 0;
	INT64 freeSpace = 0;
#if defined(WIN32)
	for (int i = 0;; ++i)
	{
		INT64 perTotalSpace = 0;
		INT64 perFreeSpace = 0;
	    char c = ('C' + i);
		char str[4] = {0};
		str[0] = c;
		str[1] = ':';
		perFreeSpace = OSGetDiskSpace(str, &perTotalSpace);
		if (!perTotalSpace)
			break;
		printf("%c�̣������ܿռ䣺%.2f GB  ���пռ䣺%.2f GB\n", c, (float)perTotalSpace / (1024 * 1024 * 1024), (float)perFreeSpace / (1024 * 1024 * 1024));
		totalSpace += perTotalSpace;
		freeSpace += perFreeSpace;
	}
	printf("���д����ܿռ䣺%.2f GB  ���пռ䣺%.2f GB\n", (float)totalSpace / (1024 * 1024 * 1024), (float)freeSpace / (1024 * 1024 * 1024));
#endif
	printf("���������ƣ�%s   ģʽ��%s\n", GetCompilerName(), GetCompileBinaryType());

	printf("\n----------------�������ݼ�����------------------\n");
	printf("-----------S/s��ʾ�ػ���R/r��ʾ������L/l��ʾע��L------------\n");
	printf("�����������");
	switch (getchar())
	{
	case 'S':
	case 's':
		OSShutdown();
		break;
	case 'R':
	case 'r':
		OSReboot();
		break;
	case 'L':
	case 'l':
		OSLogOff();
		break;
	default:
		break;
	}
	while (1);
	return 0;
}
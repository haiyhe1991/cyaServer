
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
	//printf("计算机名称：%d\n", ((testChild *)sss)->s);
	printf("----------------计算机信息参数------------------\n");
	char name[MAX_PATH];
	DWORD nameLen = MAX_PATH;
	if (OSGetComputerName(name, nameLen))
	{
		printf("计算机名称：%s\n",name);
	}
#if defined(WIN32)
	printf("Windows版本：%s\n", GetWindowsVersionText());
#endif
	printf("CPU个数：%d\n", OSGetCpuCount());
	printf("CPU频率：%.2f GHz\n", (float)GetCpuFrequency()/(float)(1024*1024));
	//printf("CPU使用率：%d GHz\n", OSGetCpuUsage());
	INT64 totalMem = 0;
	float freeMem = (float)OSGetSysFreeMem(&totalMem) / (float)(1024 * 1024 * 1024);
	printf("系统总内存：%.2f GB  空闲内存：%.2f GB\n", (float)totalMem/(1024*1024*1024), freeMem);
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
		printf("%c盘，磁盘总空间：%.2f GB  空闲空间：%.2f GB\n", c, (float)perTotalSpace / (1024 * 1024 * 1024), (float)perFreeSpace / (1024 * 1024 * 1024));
		totalSpace += perTotalSpace;
		freeSpace += perFreeSpace;
	}
	printf("所有磁盘总空间：%.2f GB  空闲空间：%.2f GB\n", (float)totalSpace / (1024 * 1024 * 1024), (float)freeSpace / (1024 * 1024 * 1024));
#endif
	printf("编译器名称：%s   模式：%s\n", GetCompilerName(), GetCompileBinaryType());

	printf("\n----------------计算机快捷键操作------------------\n");
	printf("-----------S/s表示关机，R/r表示重启，L/l表示注销L------------\n");
	printf("输入操作符：");
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
#ifndef __MAIN_LOOP_H__
#define __MAIN_LOOP_H__

#if defined(_MSC_VER) && (_MSC_VER > 1000)
	#pragma once
#endif

#include <signal.h>
#if defined(WIN32)
	#include <conio.h>
#endif

#include "cyaTime.h"

class MainLoopHandler
{
private:
	MainLoopHandler(const MainLoopHandler&);
	MainLoopHandler& operator= (const MainLoopHandler&);

public:
	// 运行检测回调函数，返回true继续主循环，返回false退出主循环
	typedef BOOL(*fnRunCheck)(void* param);

public:
	explicit MainLoopHandler(fnRunCheck fnRunCheck = NULL, DWORD idleTick = 1, void* fnParam = NULL, BOOL hidleConsole = false)
		: m_fnRunCheck(fnRunCheck), m_fnParam(fnParam), m_idleTick(idleTick), m_hidleConsole(hidleConsole)
	{
#if defined(__linux__)
		signal(SIGTSTP, OnExit_); // Ctrl + Z
		signal(SIGINT, OnExit_); // Ctrl + C
#elif defined(WIN32)
		VERIFY(SetConsoleCtrlHandler(OnExit_, true));
		typedef HWND(WINAPI* NFGetConsoleWindow)();
		NFGetConsoleWindow fnGetConsoleWindow = (NFGetConsoleWindow)GetProcAddress(GetModuleHandle(_T("kernel32")), "GetConsoleWindow");
		HWND wnd;
		if (fnGetConsoleWindow && NULL != (wnd = fnGetConsoleWindow()))
		{
			char ch[1024];
			int l = GetWindowTextA(wnd, ch, sizeof(ch) - 16);
			char pid[16];
			int l2 = sprintf(pid, " - pid:%d", (int)GetCurrentProcessId());
			if (l<l2 || 0 != strcmp(pid, ch + l - l2))
			{
				strcat(ch, pid);
				SetWindowTextA(wnd, ch);
			}
			if (m_hidleConsole)
				ShowWindow(wnd, SW_HIDE);
		}
#endif
	}

	void RunLoop(BOOL print = true, const char* prompt = NULL)
	{
		if (print)
		{
			const char* p1 = (prompt ? prompt : "");
			const char* p2 = ((prompt && prompt[0] && prompt[strlen(prompt) - 1] != '\n') ? "\n" : "");
#if defined(WIN32)
			printf("%s%sPress 'Q' or 'Ctrl + C' or 'Ctrl + Break' to exit!\n", p1, p2);
#elif defined(__linux__)
			printf("%s%sPress 'Ctrl + Z' or 'Ctrl + C' to exit!\n", p1, p2);
#else
	#error Unsupported platform.
#endif
		}
		BOOL& exitFlag = ExitFlag_();
		exitFlag = false;
		while (!exitFlag
			&& (NULL == m_fnRunCheck || m_fnRunCheck(m_fnParam))
			)
		{
#if defined(WIN32)
			if (kbhit())
			{
				int ch = 0;
				while (kbhit())
					ch = getch();
				if (ch == 'Q' || ch == 'q')
					break;
			}
#endif
			Sleep(m_idleTick);
		}
	}

private:
	static inline BOOL& ExitFlag_()
	{
		static BOOL flag = false; return flag;
	}

#if defined(__linux__)
	static inline void OnExit_(int /*sig*/)
	{
		//	ASSERT(!ExitFlag_());
		ExitFlag_() = true;
	}
#elif defined(WIN32)
	static inline BOOL WINAPI OnExit_(DWORD dwCtrlType)
	{
		if (CTRL_C_EVENT == dwCtrlType // Ctrl + C
			|| CTRL_BREAK_EVENT == dwCtrlType // Ctrl + Break
			|| CTRL_CLOSE_EVENT == dwCtrlType // 关闭控制台窗口
			|| CTRL_LOGOFF_EVENT == dwCtrlType // 注销Windows当前用户
			|| CTRL_SHUTDOWN_EVENT == dwCtrlType) // 关机
		{
			//	ASSERT(!ExitFlag_());
			ExitFlag_() = true;
			return true;
		}
		else
		{
			ASSERT(false);
			return false;
		}
	}
#endif

private:
	fnRunCheck const m_fnRunCheck;
	void* const m_fnParam;
	DWORD m_idleTick;
	BOOL m_hidleConsole;
};

inline void RunMainLoop(MainLoopHandler::fnRunCheck fnRunCheck = NULL, DWORD idleTick = 100, void* fnParam = NULL, BOOL hideConsole = false, BOOL print = true, const char* prompt = NULL)
{
	MainLoopHandler mainLoop(fnRunCheck, idleTick, fnParam, hideConsole);
	mainLoop.RunLoop(print, prompt);
}

/* 例子：
int main(int argc, const char* argv[])
{
// 程序初始化工作...

RunMainLoop();

// 程序清理工作...

return 0;
}
*/


#endif
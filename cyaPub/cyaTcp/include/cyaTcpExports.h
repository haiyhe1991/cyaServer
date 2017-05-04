#ifndef __CYA_TCP_EXPORTS_H__
#define __CYA_TCP_EXPORTS_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
编译说明：
【WIN32工程】
1、如果把CYATCP_DECL描述的函数/类加入到工程一起编译使用（不使用dll），则需要在使用工程里定义全局宏CYATCP_DECL。
2、如果把CYATCP_DECL描述的函数/类放到某个dll里面导出使用，则在dll工程里面需要定义全局宏CYATCP_EXPORTS，使用dll的工程不需要定义任何宏。
【Linux工程】
所有情况都不需要定义任何全局宏

导出C函数可以使用CYATCP_API或CYATCP_DECL修饰，
导出C++类必须使用CYATCP_DECL修饰
*/
#if defined(CYATCP_DECL)
	#undef CYATCP_DECL
	#define CYATCP_DECL
#endif

#ifndef CYATCP_DECL
	#ifdef WIN32
		#ifdef CYATCP_EXPORTS
			#define CYATCP_DECL __declspec(dllexport)
		#else
			#define CYATCP_DECL __declspec(dllimport)
		#endif
	#else
		#if defined(__GNUC__) && __GNUC__ >= 4
			#define CYATCP_DECL __attribute__((visibility("default")))
		#else
			#define CYATCP_DECL
		#endif
	#endif
#endif

#ifdef __cplusplus
	#define CYATCP_API extern "C" CYATCP_DECL
#else
	#define CYATCP_API CYATCP_DECL
#endif

#endif
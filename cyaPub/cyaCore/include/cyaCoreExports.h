#ifndef __CYA_CORE_EXPORTS_H__
#define __CYA_CORE_EXPORTS_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
编译说明：
【WIN32工程】
1、如果把CYACORE_DECL描述的函数/类加入到工程一起编译使用（不使用dll），则需要在使用工程里定义全局宏CYACORE_DECL。
2、如果把CYACORE_DECL描述的函数/类放到某个dll里面导出使用，则在dll工程里面需要定义全局宏CYACORE_EXPORTS，使用dll的工程不需要定义任何宏。
【Linux工程】
所有情况都不需要定义任何全局宏

导出C函数可以使用CYACORE_API或CYACORE_DECL修饰，
导出C++类必须使用CYACORE_DECL修饰
*/

#ifdef CYACORE_DECL
	#undef CYACORE_DECL
	#define CYACORE_DECL
#endif

#ifndef CYACORE_DECL
	#ifdef WIN32
		#ifdef CYACORE_EXPORTS
			#define CYACORE_DECL __declspec(dllexport)
		#else
			#define CYACORE_DECL __declspec(dllimport)
		#endif
	#else
		#if defined(__GNUC__) && __GNUC__ >= 4
			#define CYACORE_DECL __attribute__((visibility("default")))
		#else
			#define CYACORE_DECL
		#endif
	#endif
#endif

#ifdef __cplusplus
	#define CYACORE_API extern "C" CYACORE_DECL
#else
	#define CYACORE_API CYACORE_DECL
#endif

#endif
#ifndef __DBSSDK_EXPORTS_H__
#define __DBSSDK_EXPORTS_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
����˵����
��WIN32���̡�
1�������DBSSDK_DECL�����ĺ���/����뵽����һ�����ʹ�ã���ʹ��dll��������Ҫ��ʹ�ù����ﶨ��ȫ�ֺ�DBSSDK_DECL��
2�������DBSSDK_DECL�����ĺ���/��ŵ�ĳ��dll���浼��ʹ�ã�����dll����������Ҫ����ȫ�ֺ�DBSSDK_EXPORTS��ʹ��dll�Ĺ��̲���Ҫ�����κκꡣ
��Linux���̡�
�������������Ҫ�����κ�ȫ�ֺ�

����C��������ʹ��DBSSDK_API��DBSSDK_DECL���Σ�
����C++�����ʹ��DBSSDK_DECL����
*/

#ifdef DBSSDK_DECL
	#undef DBSSDK_DECL
	#define DBSSDK_DECL
#endif

#ifndef DBSSDK_DECL
	#ifdef WIN32
		#ifdef DBSSDK_EXPORTS
			#define DBSSDK_DECL __declspec(dllexport)
		#else
			#define DBSSDK_DECL __declspec(dllimport)
		#endif
	#else
		#if defined(__GNUC__) && __GNUC__ >= 4
			#define DBSSDK_DECL __attribute__((visibility("default")))
		#else
			#define DBSSDK_DECL
		#endif
	#endif
#endif

#ifdef __cplusplus
	#define DBSSDK_API extern "C" DBSSDK_DECL
#else
	#define DBSSDK_API DBSSDK_DECL
#endif

#endif
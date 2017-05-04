#ifndef __CYA_CORE_EXPORTS_H__
#define __CYA_CORE_EXPORTS_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
����˵����
��WIN32���̡�
1�������CYACORE_DECL�����ĺ���/����뵽����һ�����ʹ�ã���ʹ��dll��������Ҫ��ʹ�ù����ﶨ��ȫ�ֺ�CYACORE_DECL��
2�������CYACORE_DECL�����ĺ���/��ŵ�ĳ��dll���浼��ʹ�ã�����dll����������Ҫ����ȫ�ֺ�CYACORE_EXPORTS��ʹ��dll�Ĺ��̲���Ҫ�����κκꡣ
��Linux���̡�
�������������Ҫ�����κ�ȫ�ֺ�

����C��������ʹ��CYACORE_API��CYACORE_DECL���Σ�
����C++�����ʹ��CYACORE_DECL����
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
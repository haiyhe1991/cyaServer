#ifndef __CYA_TCP_EXPORTS_H__
#define __CYA_TCP_EXPORTS_H__

#if defined(_MSC_VER) && _MSC_VER > 1000
	#pragma once
#endif

/*
����˵����
��WIN32���̡�
1�������CYATCP_DECL�����ĺ���/����뵽����һ�����ʹ�ã���ʹ��dll��������Ҫ��ʹ�ù����ﶨ��ȫ�ֺ�CYATCP_DECL��
2�������CYATCP_DECL�����ĺ���/��ŵ�ĳ��dll���浼��ʹ�ã�����dll����������Ҫ����ȫ�ֺ�CYATCP_EXPORTS��ʹ��dll�Ĺ��̲���Ҫ�����κκꡣ
��Linux���̡�
�������������Ҫ�����κ�ȫ�ֺ�

����C��������ʹ��CYATCP_API��CYATCP_DECL���Σ�
����C++�����ʹ��CYATCP_DECL����
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